#include "CharDisplay.h"
#include <iostream>

CharDisplay::CharDisplay(int w, int h, char bg) : 
	m_width(w>0 ? w : 0), m_height(h>0 ? h : 0), m_buf_width(w>0 ? w+1 : 0),
	m_background(bg), m_cbuffer(m_buf_width* m_height, bg)
{
	const int size = m_cbuffer.size();
	for (int i = m_width; i < size; i += m_buf_width) m_cbuffer[i] = '\n';
}

void CharDisplay::render()
{
	std::cout << "\33[H";  // sets console cursor to start position
	std::cout << m_cbuffer;
}

void CharDisplay::flush()
{
	std::cout << "\33c";  // clears console screen
	clear();
}

void CharDisplay::clear()
{
	for (int i = 0; i < m_height; i++) {
		for (int j = 0; j < m_width; j++) {
			m_cbuffer[i * m_buf_width + j] = m_background;
		}
	}
}

int CharDisplay::get_width() const
{
	return m_width;
}

int CharDisplay::get_height() const
{
	return m_height;
}

void CharDisplay::set_size(int width, int height)
{
	m_width = width > 0 ? width : 0;
	m_height = height>0 ? height : 0;
	m_buf_width = width > 0 ? width + 1 : 0;
	m_cbuffer = std::string(m_buf_width * m_height, m_background);
	const int size = m_cbuffer.size();
	for (int i = m_width; i < size; i += m_buf_width) m_cbuffer[i] = '\n';
}

void CharDisplay::set_buffer(int x, int y, char c)
{
	if (x < 0 || x >= m_width) return;
	if (y < 0 || y >= m_height) return;
	m_cbuffer[y * m_buf_width + x] = c;
	return;
}

void CharDisplay::set_buffer_unsafe(int x, int y, char c)
{
	m_cbuffer[y * m_buf_width + x] = c;
	return;
}

char CharDisplay::get_buffer(int x, int y) const
{
	if (x < 0 || x >= m_width) return '\0';
	if (y < 0 || y >= m_height) return '\0';
	return m_cbuffer[y * m_buf_width + x];
}

char CharDisplay::get_buffer_unsafe(int x, int y) const
{
	return m_cbuffer[y * m_buf_width + x];
}