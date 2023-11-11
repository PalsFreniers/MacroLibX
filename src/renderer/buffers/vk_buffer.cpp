/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vk_buffer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/08 18:55:57 by maldavid          #+#    #+#             */
/*   Updated: 2023/11/11 03:27:31 by maldavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "vk_buffer.h"
#include <renderer/command/vk_cmd_pool.h>
#include <renderer/core/render_core.h>
#include <vma.h>
#include <cstring>
#include <iostream>

namespace mlx
{
	void Buffer::create(Buffer::kind type, VkDeviceSize size, VkBufferUsageFlags usage, const void* data)
	{
		_usage = usage;
		VmaAllocationCreateInfo alloc_info{};
		alloc_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT;
		if(type == Buffer::kind::constant)
		{
			if(data == nullptr)
			{
				core::error::report(e_kind::warning, "Vulkan : trying to create constant buffer without data (constant buffers cannot be modified after creation)");
				return;
			}
			_usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			alloc_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
		}
		else if(type == Buffer::kind::uniform)
			alloc_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST;
		else
			alloc_info.usage = VMA_MEMORY_USAGE_AUTO;

		createBuffer(_usage, alloc_info, size);

		if(type == Buffer::kind::constant || data != nullptr)
		{
			void* mapped = nullptr;
			mapMem(&mapped);
				std::memcpy(mapped, data, size);
			unmapMem();

			if(type == Buffer::kind::constant)
				pushToGPU();
		}
	}

	void Buffer::destroy() noexcept
	{
		if(_is_mapped)
			unmapMem();
		Render_Core::get().getAllocator().destroyBuffer(_allocation, _buffer);
	}

	void Buffer::createBuffer(VkBufferUsageFlags usage, VmaAllocationCreateInfo info, VkDeviceSize size)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		_allocation = Render_Core::get().getAllocator().createBuffer(&bufferInfo, &info, _buffer, _alloc_infos);
	}

	void Buffer::pushToGPU() noexcept
	{
		VmaAllocationCreateInfo alloc_info{};
		alloc_info.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

		Buffer newBuffer;
		newBuffer._usage = (_usage & 0xFFFFFFFC) | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
		newBuffer.createBuffer(newBuffer._usage, alloc_info, _alloc_infos.size);

		CmdPool cmdpool;
		cmdpool.init();
		auto device = Render_Core::get().getDevice().get();

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = cmdpool.get();
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		VkBufferCopy copyRegion{};
		copyRegion.size = _alloc_infos.size;
		vkCmdCopyBuffer(commandBuffer, _buffer, newBuffer._buffer, 1, &copyRegion);

		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		auto graphicsQueue = Render_Core::get().getQueue().getGraphic();

		vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue);

		cmdpool.destroy();

		this->swap(newBuffer);

		newBuffer.destroy();
	}

	void Buffer::swap(Buffer& buffer) noexcept
	{
		VkBuffer temp_b = _buffer;
		_buffer = buffer._buffer;
		buffer._buffer = temp_b;

		VmaAllocationInfo temp_i = _alloc_infos;
		_alloc_infos = buffer._alloc_infos;
		buffer._alloc_infos = temp_i;

		VkBufferUsageFlags temp_u = _usage;
		_usage = buffer._usage;
		buffer._usage = temp_u;
	}

	void Buffer::flush(VkDeviceSize size, VkDeviceSize offset)
	{
		Render_Core::get().getAllocator().flush(_allocation, size, offset);
	}
}
