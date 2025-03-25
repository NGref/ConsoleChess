#include "CharMoveList.h"


CharMoveListH::CharMoveListH(int x, int y, int n, int padding, int move_width) :
	m_posx(x), m_posy(y), m_height(7), m_number_of_disp_turns(std::max(n, 1)), m_first_disp_idx(0),
	m_padding(std::max(padding,0)), m_move_width(std::max(move_width,1)), 
	m_move_stride(m_move_width + 2 * m_padding + 1),
	m_move_xoff(1 + m_padding + m_move_stride)
{
	m_width = (m_number_of_disp_turns + 1) * (m_move_width + 2 * m_padding) + m_number_of_disp_turns + 2;
}

int CharMoveListH::get_width() const
{
	return m_width;
}

int CharMoveListH::get_height() const
{
	return m_height;
}

int CharMoveListH::get_xpos() const
{
	return m_posx;
}

int CharMoveListH::get_ypos() const
{
	return m_posy;
}

const std::vector<CharMove>& CharMoveListH::get_move_list() const
{
	return m_move_list;
}

void CharMoveListH::set_xpos(int x)
{
	m_posx = x;
}

void CharMoveListH::set_ypos(int y)
{
	m_posy = y;
}

void CharMoveListH::set_number_of_moves(int n)
{
	m_number_of_disp_turns = std::max(n, 1);
	m_first_disp_idx = (static_cast<int>(m_move_list.size()) - 1) / 2 / m_number_of_disp_turns;
	m_width = (m_number_of_disp_turns + 1) * (m_move_width + 2 * m_padding) + m_number_of_disp_turns + 2;
}

void CharMoveListH::set_move_list(const std::vector<CharMove>& moves)
{
	m_move_list = moves;
	m_first_disp_idx = (static_cast<int>(m_move_list.size()) - 1)/ 2 / m_number_of_disp_turns;
}

void CharMoveListH::clear()
{
	m_move_list.clear();
	m_first_disp_idx = 0;
}

void CharMoveListH::push(const std::string& move)
{
	m_move_list.push_back(to_char_move(move));
	const bool page_turn_has_occured = (m_move_list.size() == 2 * (m_first_disp_idx + m_number_of_disp_turns) + 1);
	if (page_turn_has_occured) m_first_disp_idx += m_number_of_disp_turns;
}

void CharMoveListH::pop()
{
	if (m_move_list.empty()) return;
	m_move_list.pop_back();
	const bool page_turn_has_occured = (m_move_list.size() < 2 * m_first_disp_idx);
	if (page_turn_has_occured) m_first_disp_idx -= m_number_of_disp_turns;	
}

void CharMoveListH::draw(CharDisplay* display) const
{
	const int cdw = display->get_width();
	const int cdh = display->get_height();

	if (m_posx > cdw || m_posy > cdh || m_posx + m_width <= 0 || m_posy + m_height <= 0) return;  // Not in view
	else if (m_posx >= 0 && m_posx + m_width <= cdw && m_posy >= 0 && m_posy + m_height <= cdh) draw_full(display); // Full in view
	else draw_partial(display); // partially in view
}

void CharMoveListH::draw_push(CharDisplay* display, const std::string& move)
{
	m_move_list.push_back(to_char_move(move));
	const bool page_turn_has_occured = (m_move_list.size() == 2 * (m_first_disp_idx + m_number_of_disp_turns) + 1);
	if (page_turn_has_occured) {
		m_first_disp_idx += m_number_of_disp_turns;
		draw(display);
	}
	else {
		draw_last_move(display);
		const int caption_cdx_start = m_posx + m_padding + 1;
		const bool white_is_active = m_move_list.size() % 2 == 0;
		// Active Color
		display->set_buffer_unsafe(caption_cdx_start, m_posy + 3, white_is_active ? '>' : ' ');
		display->set_buffer_unsafe(caption_cdx_start, m_posy + 5, white_is_active ? ' ' : '>');
	}
}

void CharMoveListH::draw_pop(CharDisplay* display)
{
	if (m_move_list.empty()) return;
	m_move_list.pop_back();
	const bool page_turn_has_occured = (m_move_list.size() < 2*m_first_disp_idx);
	if (page_turn_has_occured) {
		m_first_disp_idx -= m_number_of_disp_turns;
		draw(display);
	}
	else {
		const int x_off = m_move_xoff + (m_move_list.size() / 2 - m_first_disp_idx) * m_move_stride;
		const int y_off = (m_move_list.size() % 2 == 1) ? 5 : 3;
		for (int i = 0; i < SAN_LEN_MAX; i++) {
			display->set_buffer_unsafe(m_posx + x_off + i, m_posy + y_off, ' ');
		}
		const int caption_cdx_start = m_posx + m_padding + 1;
		const bool white_is_active = m_move_list.size() % 2 == 0;
		// Active Color
		display->set_buffer_unsafe(caption_cdx_start, m_posy + 3, white_is_active ? '>' : ' ');
		display->set_buffer_unsafe(caption_cdx_start, m_posy + 5, white_is_active ? ' ' : '>');
	}
}

void CharMoveListH::draw_full(CharDisplay* display) const
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
	
	// hlines
	for (int cdx = m_posx; cdx < cdxend; cdx++) {
		display->set_buffer_unsafe(cdx, m_posy, '-');
		display->set_buffer_unsafe(cdx, m_posy + 2, '-');
		display->set_buffer_unsafe(cdx, m_posy + 4, '-');
		display->set_buffer_unsafe(cdx, m_posy + 6, '-');
	}

	// vlines
	for (int cdx = m_posx; cdx < cdxend; cdx += m_move_stride) {
		for (int cdy = m_posy; cdy < cdyend; cdy++) {
			display->set_buffer_unsafe(cdx, cdy, '|');
		}
	}

	//intersections
	for (int cdx = m_posx; cdx < cdxend; cdx += m_move_stride) {
		for (int cdy = m_posy; cdy < cdyend; cdy += 2) {
			display->set_buffer_unsafe(cdx, cdy, '+');
		}
	}

	// Captions
	const int caption_cdx_start = m_posx + m_padding + 1;
	
	const char move[7]  = " Move ";
	const char white[7] = " White";
	const char black[7] = " Black";
	for (int i = 0; i < 6; i++) {
		display->set_buffer_unsafe(caption_cdx_start + i, m_posy + 1, move[i]);
		display->set_buffer_unsafe(caption_cdx_start + i, m_posy + 3, white[i]);
		display->set_buffer_unsafe(caption_cdx_start + i, m_posy + 5, black[i]);
	}

	// MoveNr
	for (int i = 0; i < m_number_of_disp_turns; i++) {
		std::string si = std::to_string(m_first_disp_idx + i + 1);
		for (int j = 0; j < si.size(); j++) {
			display->set_buffer_unsafe(m_posx + m_move_xoff + i * m_move_stride + j, m_posy + 1, si[j]);
		}
	}
	
	// Moves
	for (int i = 0; i < m_number_of_disp_turns; i++) {
		const int white_mli = 2 * (m_first_disp_idx + i);
		const int black_mli = white_mli + 1;
		const CharMove& white_cm = white_mli < m_move_list.size() ? m_move_list[white_mli] : CharMove{ ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
		const CharMove& black_cm = black_mli < m_move_list.size() ? m_move_list[black_mli] : CharMove{ ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
		for (int j = 0; j < SAN_LEN_MAX; j++) {
			display->set_buffer_unsafe(m_posx + m_move_xoff + i * m_move_stride + j, m_posy + 3, white_cm[j]);
			display->set_buffer_unsafe(m_posx + m_move_xoff + i * m_move_stride + j, m_posy + 5, black_cm[j]);
		}
	}
	// Active Color
	display->set_buffer_unsafe(caption_cdx_start, m_posy + (m_move_list.size()%2==0 ? 3 : 5), '>');
}

//TOOD make more efficient
void CharMoveListH::draw_partial(CharDisplay* display) const
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

	// hlines
	for (int cdx = m_posx; cdx < cdxend; cdx++) {
		display->set_buffer(cdx, m_posy, '-');
		display->set_buffer(cdx, m_posy + 2, '-');
		display->set_buffer(cdx, m_posy + 4, '-');
		display->set_buffer(cdx, m_posy + 6, '-');
	}

	// vlines
	for (int cdx = m_posx; cdx < cdxend; cdx += m_move_stride) {
		for (int cdy = m_posy; cdy < cdyend; cdy++) {
			display->set_buffer(cdx, cdy, '|');
		}
	}

	//intersections
	for (int cdx = m_posx; cdx < cdxend; cdx += m_move_stride) {
		for (int cdy = m_posy; cdy < cdyend; cdy += 2) {
			display->set_buffer(cdx, cdy, '+');
		}
	}

	// Captions
	const int caption_cdx_start = m_posx + m_padding + 1;

	const char move[7] = " Move ";
	const char white[7] = " White";
	const char black[7] = " Black";
	for (int i = 0; i < 6; i++) {
		display->set_buffer(caption_cdx_start + i, m_posy + 1, move[i]);
		display->set_buffer(caption_cdx_start + i, m_posy + 3, white[i]);
		display->set_buffer(caption_cdx_start + i, m_posy + 5, black[i]);
	}

	// MoveNr
	for (int i = 0; i < m_number_of_disp_turns; i++) {
		std::string si = std::to_string(m_first_disp_idx + i + 1);
		for (int j = 0; j < si.size(); j++) {
			display->set_buffer(m_posx + m_move_xoff + i * m_move_stride + j, m_posy + 1, si[j]);
		}
	}

	// Moves
	for (int i = 0; i < m_number_of_disp_turns; i++) {
		const int white_mli = 2 * (m_first_disp_idx + i);
		const int black_mli = white_mli + 1;
		const CharMove& white_cm = white_mli < m_move_list.size() ? m_move_list[white_mli] : CharMove{ ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
		const CharMove& black_cm = black_mli < m_move_list.size() ? m_move_list[black_mli] : CharMove{ ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
		for (int j = 0; j < SAN_LEN_MAX; j++) {
			display->set_buffer(m_posx + m_move_xoff + i * m_move_stride + j, m_posy + 3, white_cm[j]);
			display->set_buffer(m_posx + m_move_xoff + i * m_move_stride + j, m_posy + 5, black_cm[j]);
		}
	}
	// Active Color
	display->set_buffer(caption_cdx_start, m_posy + (m_move_list.size() % 2 == 0 ? 3 : 5), '>');
}

void CharMoveListH::draw_last_move(CharDisplay* display) const
{
	const CharMove& cm = m_move_list.back();
	const int x_off = m_move_xoff + (((m_move_list.size() - 1) / 2) - m_first_disp_idx) * m_move_stride;
	const int y_off = (m_move_list.size()%2==0) ? 5 : 3;
	const int min_draw_size = std::min(static_cast<int>(cm.size()), m_move_width);
	for (int i = 0; i < min_draw_size; i++) {
		display->set_buffer_unsafe(m_posx + x_off + i, m_posy + y_off, cm[i]);
	}
}

CharMoveListV::CharMoveListV(int x, int y, int n, int padding, int move_width) :
	m_posx(x), m_posy(y), m_width(0), m_height(0), m_number_of_disp_turns(std::max(n, 1)), m_first_disp_idx(0),
	m_padding(std::max(padding, 0)), m_move_width(std::max(move_width, 1)),
	m_move_min_draw_size(std::min(SAN_LEN_MAX, move_width)),
	m_move_stride(m_move_width + 2 * m_padding + 1),
	m_move_xoff(m_movenr_width + 3 * m_padding + 2)
{
	m_width = m_movenr_width + 2 * m_move_width + 6 * m_padding + 4;
	m_height = 2 * m_number_of_disp_turns + 3;
}

int CharMoveListV::get_width() const
{
	return m_width;
}

int CharMoveListV::get_height() const
{
	return m_height;
}

int CharMoveListV::get_xpos() const
{
	return m_posx;
}

int CharMoveListV::get_ypos() const
{
	return m_posy;
}

const std::vector<CharMove>& CharMoveListV::get_move_list() const
{
	return m_move_list;
}

void CharMoveListV::set_xpos(int x)
{
	m_posx = x;
}

void CharMoveListV::set_ypos(int y)
{
	m_posy = y;
}

void CharMoveListV::set_number_of_moves(int n)
{
	m_number_of_disp_turns = n;
	m_first_disp_idx = (static_cast<int>(m_move_list.size()) - 1) / 2 / m_number_of_disp_turns;
	m_height = 2 * m_number_of_disp_turns + 3;
}

void CharMoveListV::set_move_list(const std::vector<CharMove>& moves)
{
	m_move_list = moves;
	m_first_disp_idx = (static_cast<int>(m_move_list.size()) - 1) / 2 / m_number_of_disp_turns;
}

void CharMoveListV::clear()
{
	m_move_list.clear();
	m_first_disp_idx = 0;
}

void CharMoveListV::push(const std::string& move)
{
	m_move_list.push_back(to_char_move(move));
	const bool page_turn_has_occured = (m_move_list.size() == 2 * (m_first_disp_idx + m_number_of_disp_turns) + 1);
	if (page_turn_has_occured) {
		m_first_disp_idx += m_number_of_disp_turns;
	}
}

void CharMoveListV::pop()
{
	if (m_move_list.empty()) return;
	m_move_list.pop_back();
	const bool page_turn_has_occured = (m_move_list.size() < 2 * m_first_disp_idx);
	if (page_turn_has_occured) {
		m_first_disp_idx -= m_number_of_disp_turns;
	}
}

void CharMoveListV::draw(CharDisplay* display) const
{
	const int cdw = display->get_width();
	const int cdh = display->get_height();

	if (m_posx > cdw || m_posy > cdh || m_posx + m_width <= 0 || m_posy + m_height <= 0) return;  // Not in view
	else if (m_posx >= 0 && m_posx + m_width <= cdw && m_posy >= 0 && m_posy + m_height <= cdh) draw_full(display); // Full in view
	else draw_partial(display); // partially in view
}

void CharMoveListV::draw_push(CharDisplay* display, const std::string& move)
{
	m_move_list.push_back(to_char_move(move));
	const bool page_turn_has_occured = (m_move_list.size() == 2 * (m_first_disp_idx + m_number_of_disp_turns) + 1);
	if (page_turn_has_occured) {
		m_first_disp_idx += m_number_of_disp_turns;
		draw(display);
	}
	else {
		draw_last_move(display);
		const int white_caption_cdx = m_posx + m_move_xoff;
		const int black_caption_cdx = m_posx + m_move_xoff + m_move_stride;
		const bool white_is_active = m_move_list.size() % 2 == 0;
		// Active Color
		display->set_buffer_unsafe(white_caption_cdx, m_posy + 1, white_is_active ? '>' : ' ');
		display->set_buffer_unsafe(black_caption_cdx, m_posy + 1, white_is_active ? ' ' : '>');
	}
}

void CharMoveListV::draw_pop(CharDisplay* display)
{
	if (m_move_list.empty()) return;
	m_move_list.pop_back();
	const bool page_turn_has_occured = (m_move_list.size() < 2 * m_first_disp_idx);
	if (page_turn_has_occured) {
		m_first_disp_idx -= m_number_of_disp_turns;
		draw(display);
	}
	else {
		const int x_off = m_move_xoff + (m_move_list.size() % 2) * m_move_stride;
		const int y_off = m_move_yoff + (m_move_list.size() / 2 - m_first_disp_idx) * 2;
		for (int i = 0; i < m_move_min_draw_size; i++) {
			display->set_buffer_unsafe(m_posx + x_off + i, m_posy + y_off, ' ');
		}
		const int white_caption_cdx = m_posx + m_move_xoff;
		const int black_caption_cdx = m_posx + m_move_xoff + m_move_stride;
		const bool white_is_active = m_move_list.size() % 2 == 0;
		// Active Color
		display->set_buffer_unsafe(white_caption_cdx, m_posy + 1, white_is_active ? '>' : ' ');
		display->set_buffer_unsafe(black_caption_cdx, m_posy + 1, white_is_active ? ' ' : '>');
	}
}

void CharMoveListV::draw_full(CharDisplay* display) const
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

	// hlines
	for (int cdx = m_posx; cdx < cdxend; cdx++) {
		for (int cdy = m_posy; cdy < cdyend; cdy+=2) {
			display->set_buffer_unsafe(cdx, cdy, '-');
		}
	}

	// vlines
	const int v0x = m_posx;
	const int v1x = m_posx + m_move_xoff - m_padding - 1;
	const int v2x = v1x + m_move_stride;
	const int v3x = v2x + m_move_stride;
	for (int cdy = m_posy; cdy < cdyend; cdy++) {
		display->set_buffer_unsafe(v0x, cdy, '|');
		display->set_buffer_unsafe(v1x, cdy, '|');
		display->set_buffer_unsafe(v2x, cdy, '|');
		display->set_buffer_unsafe(v3x, cdy, '|');
	}

	//intersections
	for (int cdy = m_posy; cdy < cdyend; cdy += 2) {
		display->set_buffer_unsafe(v0x, cdy, '+');
		display->set_buffer_unsafe(v1x, cdy, '+');
		display->set_buffer_unsafe(v2x, cdy, '+');
		display->set_buffer_unsafe(v3x, cdy, '+');
	}

	// Captions
	const char move[5] = "Move";
	const int move_caption_cdx = m_posx + m_padding + 1;
	for (int i = 0; i < 4; i++) {
		display->set_buffer_unsafe(move_caption_cdx + i, m_posy + 1, move[i]);
	}

	const char white[7] = " White";
	const char black[7] = " Black";
	const int white_caption_cdx = m_posx + m_move_xoff;
	const int black_caption_cdx = m_posx + m_move_xoff + m_move_stride;
	for (int i = 0; i < 6; i++) {
		display->set_buffer_unsafe(white_caption_cdx + i, m_posy + 1, white[i]);
		display->set_buffer_unsafe(black_caption_cdx + i, m_posy + 1, black[i]);
	}

	// MoveNr
	for (int i = 0; i < m_number_of_disp_turns; i++) {
		std::string si = std::to_string(m_first_disp_idx + i + 1);
		for (int j = 0; j < si.size(); j++) {
			display->set_buffer_unsafe(m_posx + 1 + m_padding + j, m_posy + m_move_yoff + 2*i, si[j]);
		}
	}

	// Moves
	for (int i = 0; i < m_number_of_disp_turns; i++) {
		const int white_mli = 2 * (m_first_disp_idx + i);
		const int black_mli = white_mli + 1;
		const CharMove& white_cm = white_mli < m_move_list.size() ? m_move_list[white_mli] : CharMove{ ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
		const CharMove& black_cm = black_mli < m_move_list.size() ? m_move_list[black_mli] : CharMove{ ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
		for (int j = 0; j < m_move_min_draw_size; j++) {
			display->set_buffer_unsafe(m_posx + m_move_xoff + j, m_posy + m_move_yoff + 2*i, white_cm[j]);
			display->set_buffer_unsafe(m_posx + m_move_xoff + m_move_stride + j, m_posy + m_move_yoff + 2 * i, black_cm[j]);
		}
	}
	// Active Color
	display->set_buffer_unsafe((m_move_list.size() % 2 == 0 ? white_caption_cdx : black_caption_cdx), m_posy + 1, '>');
}

void CharMoveListV::draw_partial(CharDisplay* display) const
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

	// hlines
	for (int cdx = m_posx; cdx < cdxend; cdx++) {
		for (int cdy = m_posy; cdy < cdyend; cdy += 2) {
			display->set_buffer(cdx, cdy, '-');
		}
	}

	// vlines
	const int v0x = m_posx;
	const int v1x = m_posx + m_move_xoff - m_padding - 1;
	const int v2x = v1x + m_move_stride;
	const int v3x = v2x + m_move_stride;
	for (int cdy = m_posy; cdy < cdyend; cdy++) {
		display->set_buffer(v0x, cdy, '|');
		display->set_buffer(v1x, cdy, '|');
		display->set_buffer(v2x, cdy, '|');
		display->set_buffer(v3x, cdy, '|');
	}

	//intersections
	for (int cdy = m_posy; cdy < cdyend; cdy += 2) {
		display->set_buffer(v0x, cdy, '+');
		display->set_buffer(v1x, cdy, '+');
		display->set_buffer(v2x, cdy, '+');
		display->set_buffer(v3x, cdy, '+');
	}

	// Captions
	const char move[5] = "Move";
	const int move_caption_cdx = m_posx + m_padding + 1;
	for (int i = 0; i < 4; i++) {
		display->set_buffer(move_caption_cdx + i, m_posy + 1, move[i]);
	}

	const char white[7] = " White";
	const char black[7] = " Black";
	const int white_caption_cdx = m_posx + m_move_xoff;
	const int black_caption_cdx = m_posx + m_move_xoff + m_move_stride;
	for (int i = 0; i < 6; i++) {
		display->set_buffer(white_caption_cdx + i, m_posy + 1, white[i]);
		display->set_buffer(black_caption_cdx + i, m_posy + 1, black[i]);
	}

	// MoveNr
	for (int i = 0; i < m_number_of_disp_turns; i++) {
		std::string si = std::to_string(m_first_disp_idx + i + 1);
		for (int j = 0; j < si.size(); j++) {
			display->set_buffer(m_posx + 1 + m_padding + j, m_posy + m_move_yoff + 2 * i, si[j]);
		}
	}

	// Moves
	for (int i = 0; i < m_number_of_disp_turns; i++) {
		const int white_mli = 2 * (m_first_disp_idx + i);
		const int black_mli = white_mli + 1;
		const CharMove& white_cm = white_mli < m_move_list.size() ? m_move_list[white_mli] : CharMove{ ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
		const CharMove& black_cm = black_mli < m_move_list.size() ? m_move_list[black_mli] : CharMove{ ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
		for (int j = 0; j < m_move_min_draw_size; j++) {
			display->set_buffer(m_posx + m_move_xoff + j, m_posy + m_move_yoff + 2 * i, white_cm[j]);
			display->set_buffer(m_posx + m_move_xoff + m_move_stride + j, m_posy + m_move_yoff + 2 * i, black_cm[j]);
		}
	}
	// Active Color
	display->set_buffer((m_move_list.size() % 2 == 0 ? white_caption_cdx : black_caption_cdx), m_posy + 1, '>');
}

void CharMoveListV::draw_last_move(CharDisplay* display) const
{
	const CharMove& cm = m_move_list.back();
	
	const int x_off = m_move_xoff + ( 1 - m_move_list.size() % 2 ) * m_move_stride;
	const int y_off = m_move_yoff + (((m_move_list.size() - 1) / 2) - m_first_disp_idx) * 2;
	for (int i = 0; i < m_move_min_draw_size; i++) {
		display->set_buffer_unsafe(m_posx + x_off + i, m_posy + y_off, cm[i]);
	}
}

CharMove to_char_move(const std::string& move)
{
	CharMove cm{ ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
	const int minsize = std::min(SAN_LEN_MAX, static_cast<int>(move.size()));
	for (int i = 0; i < minsize; i++) {
		cm[i] = move[i];
	}
	return cm;
}