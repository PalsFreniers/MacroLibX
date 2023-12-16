/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/20 02:13:03 by maldavid          #+#    #+#             */
/*   Updated: 2023/12/16 18:53:51 by maldavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __MLX_VK_MEMORY__
#define __MLX_VK_MEMORY__

#include <volk.h>
#include <vma.h>
#include <mlx_profile.h>

namespace mlx
{
	class GPUallocator
	{
		public:
			GPUallocator() = default;

			void init() noexcept;
			void destroy() noexcept;

			VmaAllocation createBuffer(const VkBufferCreateInfo* binfo, const VmaAllocationCreateInfo* vinfo, VkBuffer& buffer, const char* name = nullptr) noexcept;
			void destroyBuffer(VmaAllocation allocation, VkBuffer buffer) noexcept;

			VmaAllocation createImage(const VkImageCreateInfo* iminfo, const VmaAllocationCreateInfo* vinfo, VkImage& image, const char* name = nullptr) noexcept;
			void destroyImage(VmaAllocation allocation, VkImage image) noexcept;

			void mapMemory(VmaAllocation allocation, void** data) noexcept;
			void unmapMemory(VmaAllocation allocation) noexcept;

			void dumpMemoryToJson();

			void flush(VmaAllocation allocation, VkDeviceSize size, VkDeviceSize offset) noexcept;

			~GPUallocator() = default;

		private:
			VmaAllocator _allocator;
			uint32_t _active_buffers_allocations = 0;
			uint32_t _active_images_allocations = 0;
	};
}

#endif
