#pragma once

#include "CharDisplay.h"

/// <summary>
/// A simple 1 char border. Width and Height <2 are not defined.
/// </summary>
class CharBorder
{
public:
	CharBorder(int x, int y, int width, int height);
	
	void draw(CharDisplay* display) const;

	int get_xpos() const;
	int get_ypos() const;
	int get_width() const;
	int get_height() const;
	
	void set_xpos(int x);
	void set_ypos(int y);
	void set_width(int w);
	void set_height(int h);
private:
	int m_xpos;
	int m_ypos;
	int m_xupper;
	int m_yupper;
};

