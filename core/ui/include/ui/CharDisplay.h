#pragma once

#include <string>

/// <summary>
/// Displays m_buffer content in the console, when render is called. Buffer access via set_buffer, get_buffer.
/// Use unsafe variants if the caller does bounds checking.
/// Note that if there are multiple display instances, each instance will overwrite the other display in console upon calling render.
/// </summary>
class CharDisplay
{
public:
	CharDisplay(int width, int height, char background = ' ');
	// writes buffer to console
	void render();
	// clears console and buffer
	void flush();
	//clears buffer
	void clear();

	int get_width() const;
	int get_height() const;
	char get_buffer(int x, int y) const;
	char get_buffer_unsafe(int x, int y) const;

	void set_size(int width, int height);
	void set_buffer(int x, int y, char c);
	void set_buffer_unsafe(int x, int y, char c);
public:
	//debug
	std::string get_buffer_str() const { return m_cbuffer; }
private:
	int m_width;
	int m_height;
	int m_buf_width;
	char m_background;
	std::string m_cbuffer;
};