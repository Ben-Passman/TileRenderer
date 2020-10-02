#include "render_engine.h"

RenderEngine::RenderEngine(const unsigned int width, const unsigned int height)
{
	screen_width = width;
	screen_height = height;

	screen_position_old = glm::uvec2(screen_width, screen_height);

	float fullscreen[] = {
		// position				// UV
		 1.0f,  1.0f,		1.0f, 1.0f,		// top right
		 1.0f, -1.0f,		1.0f, 0.0f,		// bottom right
		-1.0f, -1.0f,		0.0f, 0.0f,		// bottom left
		-1.0f,  1.0f,		0.0f, 1.0f		// top left
	};
	unsigned int screen_indices[] = {
		0, 1, 3,
		1, 2, 3
	};
	full_screen.init(sizeof(fullscreen), &fullscreen, sizeof(screen_indices), &screen_indices, GL_STATIC_DRAW);
	
	unsigned int screenmask_coefficients[] = {
		// l, r, const				// b, t, const
		 0u,  1u,  0u,				0u,  0u,  screen_height,
		 0u,  1u,  0u,				0u,  1u,  0u,
		 1u,  0u,  0u,				0u,  1u,  0u,
		 1u,  0u,  0u,				0u,  0u,  screen_height,

		 0u,  0u,  screen_width,	0u,  1u,  0u,
		 0u,  0u,  screen_width,	1u,  0u,  0u,
		 0u,  0u,  0u,				1u,  0u,  0u,
		 0u,  0u,  0u,				0u,  1u,  0u,

		 0u,  1u,  0u,				1u,  0u,  0u,
		 0u,  1u,  0u,				0u,  0u,  0u,
		 1u,  0u,  0u,				0u,  0u,  0u,
		 1u,  0u,  0u,				1u,  0u,  0u		 
	};

	unsigned int mask_indices[] = {
		0, 1, 3,
		1, 2, 3,
		4, 5, 7,
		5, 6, 7,
		8, 9, 11,
		9, 10, 11
	};
	// NEED TO FIX, LOAD FLOATS AT PRESENT
	screen_mask.init_custom(sizeof(screenmask_coefficients), &screenmask_coefficients, sizeof(mask_indices), &mask_indices, GL_STREAM_DRAW);

	glm::uvec2 tile_indices[380];
	for (auto j = 0; j < 304 / 16; j++)
	{
		for (auto i = 0; i < 320 / 16; i++)
		{
			tile_indices[i + 20 * j] = glm::uvec2(i * 16u, j * 16u);
			//std::cout << std::hex << tile_indices[i].x << ", " << tile_indices[i].y << std::dec << std::endl;
		}
	}

	GLuint ubo_binding_index{ 0 };
	tile_shader.bind_uniform_block("tiles", ubo_binding_index);
	tile_map.init(GL_UNIFORM_BUFFER, sizeof(tile_indices), tile_indices, GL_STATIC_DRAW);
	tile_map.bind_uniform_block(ubo_binding_index);
	tile_map.unbind();
	
	GLint temp = tile_shader.get_ubo_size(ubo_binding_index);
	std::cout << "Size of tile index array in bytes : " << sizeof(tile_indices) << std::endl;
	std::cout << "Size of UBO structure in bytes : " << temp << std::endl;	
	
	frame_buffer.init(screen_width, screen_height);
	tileset.load_image("resources/tileset.png", Texture::Wrap::clamp, Texture::Filter::nearest);
}

void RenderEngine::render(const glm::uvec2 world_position)
{
	tile_shader.use();
	tile_shader.setUniform_vec2_u("screenSize", glm::uvec2(screen_width, screen_height));
	tile_shader.setUniform_vec2_u("worldCoords", world_position);
	tile_shader.setUniform_vec4_u("maskCoords", glm::uvec4(0, 0, 128, 128));
	tileset.bind_texture();
	frame_buffer.bind();
	screen_mask.draw();
	frame_buffer.unbind();
	
	glClear(GL_COLOR_BUFFER_BIT); // Redundant for this application?

	screen_shader.use();
	screen_shader.setUniform_vec2_f("offset", glm::vec2(0.0f, 0.0f));
	frame_buffer.bind_colour_buffer();
	full_screen.draw();

	screen_position_old = world_position;
}