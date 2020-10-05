#include "gl_buffers.h"

Buffer::Buffer()
{
	glGenBuffers(1, &buffer_ID);
}

Buffer::~Buffer()
{
	glDeleteBuffers(1, &buffer_ID);
	std::cout << "Buffer Destructor, ID: " << buffer_ID << std::endl;
}

void Buffer::init(GLenum buffer_type, GLsizeiptr size, const void* data, GLenum usage)
{
	glBindBuffer(buffer_type, buffer_ID);
	glBufferData(buffer_type, size, data, usage); // Creates and initialises store. Should be in constructor, use glBufferSubData for updates

	target = buffer_type;
}

void Buffer::bind_uniform_block(GLuint index) const
{
	glBindBufferBase(GL_UNIFORM_BUFFER, index, buffer_ID); // glBindBufferRange for part binding
}

void Buffer::bind() const
{
	glBindBuffer(target, buffer_ID);
}

void Buffer::unbind() const
{
	glBindBuffer(target, 0);
}

// Originally calculated screen mask on CPU and updated vertex buffer. Calculations now performed in shader.
void Buffer::write(GLsizeiptr size, const void* data) const
{
	bind();
	void* mem_ptr = glMapBuffer(target, GL_WRITE_ONLY);
	memcpy(mem_ptr, data, size); // No check on data size, use with caution.
	glUnmapBuffer(target);
	unbind();
}



FrameBuffer::FrameBuffer(GLint width, GLint height, Texture::ColourMode format) : 
	colour_buffer{ width, height, { Texture::ColourMode::rgba, Texture::Wrap::clamp, Texture::Wrap::clamp, Texture::Filter::nearest, Texture::Filter::nearest } }
{
	glGenFramebuffers(1, &frame_buffer_ID);

	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_ID);
	colour_buffer.bind_texture();
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	colour_buffer.attach_to_frame_buffer();
	// No depth or stencil buffers used

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER::Incomplete buffer" << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &frame_buffer_ID);
	std::cout << "Frame Buffer Destructor, ID: " << frame_buffer_ID << std::endl;
}

void FrameBuffer::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, frame_buffer_ID);
}

void FrameBuffer::bind_colour_buffer() const
{
	colour_buffer.bind_texture();
}

void FrameBuffer::unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



VertexArray::VertexArray()
{
	glGenVertexArrays(1, &vertex_array_ID);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &vertex_array_ID);
	std::cout << "Vertex Array Destructor, ID: " << vertex_array_ID << std::endl;
}

void VertexArray::bind() const
{
	glBindVertexArray(vertex_array_ID);
}

void VertexArray::unbind() const
{
	glBindVertexArray(0);
}

void VertexArray::init(GLsizeiptr vertex_data_size, const void* vertex_data, GLsizeiptr element_data_size, const void* element_data, GLenum usage)
{
	glBindVertexArray(vertex_array_ID);

	vertex_buffer.init(GL_ARRAY_BUFFER, vertex_data_size, vertex_data, usage);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0); // Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))); // Texture Coords
	glEnableVertexAttribArray(1);

	element_buffer.init(GL_ELEMENT_ARRAY_BUFFER, element_data_size, element_data, usage);
	
	glBindVertexArray(0);
	vertex_buffer.unbind();
	element_buffer.unbind();

	vertex_count = element_data_size;
}

void VertexArray::init_custom(GLsizeiptr vertex_data_size, const void* vertex_data, GLsizeiptr element_data_size, const void* element_data, GLenum usage)
{
	glBindVertexArray(vertex_array_ID);

	vertex_buffer.init(GL_ARRAY_BUFFER, vertex_data_size, vertex_data, usage);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // x coefficients
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))); // y coefficients
	glEnableVertexAttribArray(1);

	element_buffer.init(GL_ELEMENT_ARRAY_BUFFER, element_data_size, element_data, usage);
	
	glBindVertexArray(0);
	vertex_buffer.unbind();
	element_buffer.unbind();

	vertex_count = element_data_size;
}

void VertexArray::draw() const
{
	bind();
	glDrawElements(GL_TRIANGLES, vertex_count, GL_UNSIGNED_INT, 0);
}
