#pragma once

#include "opengl.h"
#include "gl_texture.h"

namespace GLRender
{
	class FrameBuffer
	{
	private:
		GLuint m_frame_buffer_ID;
		Texture m_colour_buffer;
	public:
		FrameBuffer(const GLint width, const GLint height, const Texture::ColourMode format);
		FrameBuffer(const FrameBuffer&) = delete;
		FrameBuffer(FrameBuffer&&) = delete;
		FrameBuffer& operator=(const FrameBuffer&) = delete;
		FrameBuffer& operator=(FrameBuffer&&) = delete;
		~FrameBuffer();

		void bind() const;
		void bind_colour_buffer() const;
		void unbind() const;
	};
}