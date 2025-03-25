#pragma once

#include "CyclicArray.h"
#include "CharDisplay.h"

#include <string>


/// <summary>
/// Can display a number of messages inside a box.
/// Latest message at bottom.
/// Message capacity is const after construction.
/// </summary>
class CharMessageBox
{
public:
	CharMessageBox(int x, int y, int width, int height, int max_number_of_messages);
	void draw(CharDisplay* display) const;
	void push(const std::string& message);

	int get_xpos() const;
	int get_ypos() const;
	int get_width() const;
	int get_height() const;
	int get_capacity() const;

	void clear();

	void set_xpos(int x);
	void set_ypos(int y);
	void set_width(int w);
	void set_height(int h);

private:
	void draw_full(CharDisplay* display) const;
	void draw_partial(CharDisplay* display) const;

private:
	int m_posx;
	int m_posy;
	int m_height;
	int m_width;
	BoundedQueueHeap<std::string> m_messages;
};
