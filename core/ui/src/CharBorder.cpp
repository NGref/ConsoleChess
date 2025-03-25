#include "CharBorder.h"

CharBorder::CharBorder(int x, int y, int w, int h) :
	m_xpos(x), m_ypos(y), m_xupper(x+w-1), m_yupper(y+h-1)
{
}

void CharBorder::draw(CharDisplay* cd) const
{
	const int cdw = cd->get_width();
	const int cdh = cd->get_height();

	if (m_xpos >= cdw || m_ypos >= cdh || m_xupper < 0 || m_yupper < 0) return;
	if (m_xpos < 0 && m_ypos < 0 && m_xupper >= cdw && m_yupper >= cdh) return;

	const int xstart = std::max(0, m_xpos);
	const int ystart = std::max(0, m_ypos);
	const int xend = std::min(m_xupper, cdw - 1);
	const int yend = std::min(m_yupper, cdh - 1);


	if (xstart == m_xpos) {
		for (int y = ystart; y <= yend; y++) cd->set_buffer_unsafe(xstart, y, '|');
	}

	if (xend == m_xupper) {
		for (int y = ystart; y <= yend; y++) cd->set_buffer_unsafe(xend, y, '|');
	}

	if (ystart == m_ypos) {
		for (int x = xstart; x <= xend; x++) cd->set_buffer_unsafe(x, ystart, '-');
	}
	
	if (yend == m_yupper) {
		for (int x = xstart; x <= xend; x++) cd->set_buffer_unsafe(x, yend, '-');
	}

	cd->set_buffer(m_xpos, m_ypos, '+');
	cd->set_buffer(m_xpos, m_yupper, '+');
	cd->set_buffer(m_xupper, m_ypos, '+');
	cd->set_buffer(m_xupper, m_yupper, '+');
}

int CharBorder::get_xpos() const
{
	return m_xpos;
}

int CharBorder::get_ypos() const
{
	return m_ypos;
}

int CharBorder::get_width() const
{
	return m_xupper - m_xpos + 1;
}

int CharBorder::get_height() const
{
	return m_yupper - m_ypos + 1;
}

void CharBorder::set_xpos(int x)
{
	const int width = get_width();
	m_xpos = x;
	m_xupper = m_xpos + width - 1;
}

void CharBorder::set_ypos(int y)
{
	const int height = get_height();
	m_ypos = y;
	m_yupper = m_ypos + height - 1;
}

void CharBorder::set_width(int w)
{
	m_xupper = m_xpos + w - 1;
}

void CharBorder::set_height(int h)
{
	m_yupper = m_ypos + h - 1;
}
