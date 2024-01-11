/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   graphics.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: maldavid <kbz_8.dev@akel-engine.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/02 14:49:49 by maldavid          #+#    #+#             */
/*   Updated: 2024/01/11 05:25:51 by maldavid         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef __MLX_GRAPHICS__
#define __MLX_GRAPHICS__

#include <memory>
#include <unordered_set>
#include <filesystem>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <platform/window.h>
#include <renderer/renderer.h>
#include <renderer/pixel_put.h>
#include <renderer/core/drawable_resource.h>
#include <renderer/images/texture_manager.h>
#include <renderer/texts/text_manager.h>
#include <utils/non_copyable.h>
#include <renderer/images/texture.h>
#include <mlx_profile.h>
#include <core/profiler.h>

namespace mlx
{
	class GraphicsSupport : public non_copyable
	{
		public:
			GraphicsSupport(std::size_t w, std::size_t h, Texture* render_target, int id);
			GraphicsSupport(std::size_t w, std::size_t h, std::string title, int id);

			inline int& getID() noexcept;
			inline std::shared_ptr<MLX_Window> getWindow();

			void render() noexcept;

			inline void clearRenderData() noexcept;
			inline void pixelPut(int x, int y, uint32_t color) noexcept;
			inline void stringPut(int x, int y, uint32_t color, std::string str);
			inline void texturePut(Texture* texture, int x, int y);
			inline void loadFont(const std::filesystem::path& filepath, float scale);

			inline bool hasWindow() const noexcept  { return _has_window; }

			inline Renderer& getRenderer() { return *_renderer; }

			~GraphicsSupport();

		private:
			Texture _pixel_put_texture;

			std::vector<DrawableResource*> _drawlist;
			
			TextManager _text_manager;
			TextureManager _texture_manager;
			
			glm::mat4 _proj = glm::mat4(1.0);
			
			std::shared_ptr<MLX_Window> _window;
			std::unique_ptr<Renderer> _renderer;

			std::size_t _width = 0;
			std::size_t _height = 0;
			
			int _id;

			bool _has_window;
	};
}

#include <core/graphics.inl>

#endif
