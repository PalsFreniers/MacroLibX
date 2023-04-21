/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   application.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/04 22:10:52 by maldavid          #+#    #+#             */
/*   Updated: 2023/04/21 19:24:35 by maldavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "application.h"
#include <renderer/images/texture.h>
#include <renderer/core/render_core.h>
#include <array>
#include <utils/endian.h>

namespace mlx::core
{
	void Application::run() noexcept
	{
		while(_in->is_running())
		{
			_in->update();
			for(auto& gs : _graphics)
				gs->beginRender();

			if(_loop_hook)
				_loop_hook(_param);
			
			for(auto& gs : _graphics)
				gs->endRender();
		}
	}

	void* Application::newTexture(int w, int h)
	{
		_textures.emplace_front().create(nullptr, w, h, VK_FORMAT_R8G8B8A8_UNORM);
		_textures.front().openCPUmap();
		return &_textures.front();
	}

	void* Application::newStbTexture(char* file, int* w, int* h)
	{
		_textures.emplace_front(stbTextureLoad(file, w, h));
		_textures.front().openCPUmap();
		return &_textures.front();
	}

	char* Application::mapTexture(Texture* img, int* bits_per_pixel, int* size_line, int* endian)
	{
		static const int endianness = isSystemBigEndian();

		*bits_per_pixel = 32;
		*size_line = img->getWidth() * 4;
		*endian = endianness;
		return static_cast<char*>(img->getMap());
	}

	void Application::destroyTexture(void* ptr)
	{
        vkDeviceWaitIdle(Render_Core::get().getDevice().get());
		Texture* texture = static_cast<Texture*>(ptr);
		texture->destroy();
	}
}
