#include "CharMessageBox.h"


CharMessageBox::CharMessageBox(int x, int y, int width, int height, int max_number_of_messages) :
	m_posx(x), m_posy(y), m_width(width), m_height(height), m_messages(max_number_of_messages)
{
}

void CharMessageBox::draw(CharDisplay* display) const
{
	const int cdw = display->get_width();
	const int cdh = display->get_height();

	if (m_posx > cdw || m_posy > cdh || m_posx + m_width <= 0 || m_posy + m_height <= 0) return;  // Not in view
	else if (m_posx >= 0 && m_posx + m_width <= cdw && m_posy >= 0 && m_posy + m_height <= cdh) draw_full(display); // Full in view
	else draw_partial(display); // partially in view
}

void CharMessageBox::push(const std::string& message)
{
	m_messages.push_front(message);
}

int CharMessageBox::get_xpos() const
{
	return m_posx;
}

int CharMessageBox::get_ypos() const
{
	return m_posy;
}

int CharMessageBox::get_width() const
{
	return m_width;
}

int CharMessageBox::get_height() const
{
	return m_height;
}

int CharMessageBox::get_capacity() const
{
	return m_messages.capacity();
}

void CharMessageBox::clear()
{
	m_messages.clear();
}

void CharMessageBox::set_xpos(int x)
{
	m_posx = x;
}

void CharMessageBox::set_ypos(int y)
{
	m_posy = y;
}

void CharMessageBox::set_width(int w)
{
	m_width = w;
}

void CharMessageBox::set_height(int h)
{
	m_height = h;
}

void CharMessageBox::draw_full(CharDisplay* display) const
{
	const int cdw = display->get_width();
	const int cdh = display->get_height();

	const int cdxend = m_posx + m_width;
	const int cdyend = m_posy + m_height;

	//clear bg
	for (int cdy = m_posy; cdy < cdyend; cdy++) {
		for (int cdx = m_posx; cdx < cdxend; cdx++) {
			display->set_buffer_unsafe(cdx, cdy, ' ');
		}
	}
	// messages
	int y = m_height - 1;
	for (int i = 0; i < m_messages.size(); i++) {
		const std::string& message = m_messages[i];
		const int message_full_lines = message.size() / m_width;
		const int last_line_width = message.size() % m_width;
		//draw last line
		if (last_line_width > 0) {
			for (int x = 0; x < last_line_width; x++) {
				display->set_buffer_unsafe(m_posx + x, m_posy + y, message[message_full_lines * m_width + x]);
			}
			y--;
		}
		if (y < 0) return;
		// draw full lines
		for (int line = message_full_lines - 1; line >= 0; line--) {
			for (int x = 0; x < m_width; x++) {
				display->set_buffer_unsafe(m_posx + x, m_posy + y, message[line * m_width + x]);
			}
			y--;
			if (y < 0) return;
		}
	}
}

// TODO do some logic to reduce bounds checking
void CharMessageBox::draw_partial(CharDisplay* display) const
{
	const int cdw = display->get_width();
	const int cdh = display->get_height();

	const int cdxend = m_posx + m_width;
	const int cdyend = m_posy + m_height;

	//clear bg
	for (int cdy = m_posy; cdy < cdyend; cdy++) {
		for (int cdx = m_posx; cdx < cdxend; cdx++) {
			display->set_buffer(cdx, cdy, ' ');
		}
	}
	// messages
	int y = m_height - 1;
	for (int i = 0; i < m_messages.size(); i++) {
		const std::string& message = m_messages[i];
		const int message_full_lines = message.size() / m_width;
		const int last_line_width = message.size() % m_width;
		//draw last line
		if (last_line_width > 0) {
			for (int x = 0; x < last_line_width; x++) {
				display->set_buffer(m_posx + x, m_posy + y, message[message_full_lines * m_width + x]);
			}
			y--;
		}
		if (y < 0) return;
		// draw full lines
		for (int line = message_full_lines - 1; line >= 0; line--) {
			for (int x = 0; x < m_width; x++) {
				display->set_buffer(m_posx + x, m_posy + y, message[line * m_width + x]);
			}
			y--;
			if (y < 0) return;
		}
	}
}
