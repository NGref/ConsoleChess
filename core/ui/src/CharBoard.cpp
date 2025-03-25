#include "CharBoard.h"

#include <algorithm>
#include <fstream>
#include <filesystem>
#include <iostream>


CharBoard::CharBoard(int x, int y, BoardSize size, std::filesystem::path path, bool hlines, bool vlines, bool description) :
	m_posx(x), m_posy(y), m_size(size), m_hlines(hlines), m_vlines(vlines), m_rankrow_descr(description)
{
	if (m_size == BoardSize::SMALL) {
		m_whitebg_char = '-';
		m_blackbg_char = '-';
		m_vline_char = ' ';
		m_hline_char = ' ';
	}
	m_init_ok = load_textures(path);
	if (m_size == BoardSize::SMALL) {
		m_piece_tex = &m_piece_tex_small;
	}
	else if (m_size == BoardSize::MEDIUM) {
		m_piece_tex = &m_piece_tex_medium;
	}
	else {
		m_piece_tex = &m_piece_tex_large;
	}
	
	m_tilewidth = m_piece_tex->front().width;
	m_tileheight = m_piece_tex->front().height;
	tile_x_stride = m_tilewidth + (m_vlines ? 1 : 0);
	tile_y_stride = m_tileheight + (m_hlines ? 1 : 0);
	tile_x_off = (m_rankrow_descr ? 2 : 0) + (m_vlines ? 1 : 0);
	tile_y_off = (m_rankrow_descr ? 1 : 0) + (m_hlines ? 1 : 0);
	m_width = tile_x_off + 8 * tile_x_stride;
	m_height = tile_y_off + 8 * tile_y_stride;
}

void CharBoard::draw(CharDisplay* display, const std::vector<TileP>& all_tiles) const
{
	const int cdw = display->get_width();
	const int cdh = display->get_height();

	if (m_posx > cdw || m_posy > cdh || m_posx + m_width <= 0 || m_posy + m_height <= 0) return;  // Not in view
	else if (m_posx >= 0 && m_posx + m_width <= cdw && m_posy >= 0 && m_posy + m_height <= cdh) {
		// Full in view
		draw_board_full(display);
		draw_pieces(display, all_tiles);
	}
	else {
		draw_board_partial(display); // partially in view
		draw_pieces_partial(display, all_tiles);
	}
}

void CharBoard::draw_delta(CharDisplay* display, const std::vector<TileP>& delta_tiles) const
{
	const int cdw = display->get_width();
	const int cdh = display->get_height();

	if (m_posx > cdw || m_posy > cdh || m_posx + m_width <= 0 || m_posy + m_height <= 0) return;  // Not in view
	else if (m_posx >= 0 && m_posx + m_width <= cdw && m_posy >= 0 && m_posy + m_height <= cdh) draw_pieces(display, delta_tiles);  // Full in view
	else draw_pieces_partial(display, delta_tiles);
}

void CharBoard::draw_board(CharDisplay* cd) const
{
	const int cdw = cd->get_width();
	const int cdh = cd->get_height();
	
	if (m_posx > cdw || m_posy > cdh || m_posx + m_width <= 0 || m_posy + m_height <= 0) return;  // Not in view
	else if (m_posx >= 0 && m_posx + m_width <= cdw && m_posy >= 0 && m_posy + m_height <= cdh) draw_board_full(cd); // Full in view
	else draw_board_partial(cd); // partially in view
}

void CharBoard::draw_board_full(CharDisplay* cd) const
{
	const int cdw = cd->get_width();
	const int cdh = cd->get_height();
	
	const int cdxend = m_posx + m_width;
	const int cdyend = m_posy + m_height;

	if (m_hlines) {
		// hlines
		for (int cdy = m_posy + tile_y_off - 1; cdy < cdyend; cdy += tile_y_stride) {
			for (int cdx = m_posx + tile_x_off - 1; cdx < cdxend; cdx++) {
				cd->set_buffer_unsafe(cdx, cdy, m_hline_char);
			}
		}
	}

	if (m_vlines) {
		// vlines
		for (int cdx = m_posx + tile_x_off - 1; cdx < cdxend; cdx += tile_x_stride) {
			for (int cdy = m_posy + tile_y_off - 1; cdy < cdyend; cdy++) {
				cd->set_buffer_unsafe(cdx, cdy, m_vline_char);
			}
		}
	}

	if (m_hlines && m_vlines) {
		//intersections
		for (int cdx = m_posx + tile_x_off - 1; cdx < cdxend; cdx += tile_x_stride) {
			for (int cdy = m_posy + tile_y_off - 1; cdy < cdyend; cdy += tile_y_stride) {
				cd->set_buffer_unsafe(cdx, cdy, '+');
			}
		}
	}
	
	if (m_rankrow_descr) {
		//clear descr rows/columns
		for (int cdx = m_posx; cdx < cdxend; cdx++) cd->set_buffer_unsafe(cdx, m_posy, ' ');
		for (int cdy = m_posy; cdy < cdyend; cdy++) {
			cd->set_buffer_unsafe(m_posx, cdy, ' ');
			cd->set_buffer_unsafe(m_posx + 1, cdy, ' ');
		}
		// draw rank/row description
		const int desc_cdx_off = m_posx + tile_x_off + m_tilewidth / 2;
		const int desc_cdy_off = m_posy + tile_y_off + m_tileheight / 2;
		for (int i = 0; i < 8; i++) {
			cd->set_buffer(m_posx, desc_cdy_off + i * tile_y_stride, '8' - i); // board is flipped 
		}
		for (int i = 0; i < 8; i++) {

			cd->set_buffer(desc_cdx_off + i * tile_x_stride, m_posy, 'A' + i);
		}
	}
	
	// fill tiles with bg color
	for (int i = 0; i < 8; i++) {
		const int cdy_tile_end = m_posy + tile_y_off + i * tile_y_stride + m_tileheight;
		for (int j = 0; j < 8; j++) {
			const int cdx_tile_end = m_posx + tile_x_off + j * tile_x_stride + m_tilewidth;
			const char color = ((i + j) % 2 == 0) ? m_whitebg_char : m_blackbg_char;
			for (int cdy = m_posy + tile_y_off + i * tile_y_stride; cdy < cdy_tile_end; cdy++) {
				for (int cdx = m_posx + tile_x_off + j * tile_x_stride; cdx < cdx_tile_end; cdx++) {
					cd->set_buffer(cdx, cdy, color);
				}
			}
		}
	}
}

void CharBoard::draw_board_partial(CharDisplay* cd) const
{
	// TODO
	return;
}

void CharBoard::draw_pieces(CharDisplay* cd, const std::vector<TileP>& v) const
{
	for (const TileP& tile : v) {
		const MaskedTexture& ptex = get_piece_tex(tile.piece,tile.color);

		const char bgcolor = ((tile.position.x + tile.position.y) % 2 == 0) ? m_blackbg_char : m_whitebg_char;
		const int cdx_off = m_posx + tile_x_off + tile.position.x * tile_x_stride;
		const int cdy_off = m_posy + tile_y_off + (7 - tile.position.y) * tile_y_stride;

		for (int ty = 0; ty < ptex.height; ty++) {
			for (int tx = 0; tx < ptex.width; tx++) {
				const int tidx = ty * ptex.width + tx;
				cd->set_buffer_unsafe(cdx_off + tx, cdy_off + ty, ptex.mask[tidx] ? ptex.tex[tidx] : bgcolor);
			}
		}
	}
}

void CharBoard::draw_pieces_partial(CharDisplay* display, const std::vector<TileP>& pieces) const
{
	for (const TileP& tile : pieces) {
		const MaskedTexture& ptex = get_piece_tex(tile.piece, tile.color);

		const char bgcolor = ((tile.position.x + tile.position.y) % 2 == 0) ? m_blackbg_char : m_whitebg_char;
		const int cdx_off = m_posx + tile_x_off + tile.position.x * tile_x_stride;
		const int cdy_off = m_posy + tile_y_off + (7 - tile.position.y) * tile_y_stride;

		for (int ty = 0; ty < ptex.height; ty++) {
			for (int tx = 0; tx < ptex.width; tx++) {
				const int tidx = ty * ptex.width + tx;
				display->set_buffer(cdx_off + tx, cdy_off + ty, ptex.mask[tidx] ? ptex.tex[tidx] : bgcolor);
			}
		}
	}
}

const MaskedTexture& CharBoard::get_piece_tex(Piece p, ChessColor c) const
{
	if (p == Piece::EMPTY) return (*m_piece_tex)[0];
	return (*m_piece_tex)[int(p) + (c.white ? 0 : 6)];
}

void CharBoard::set_size(BoardSize s)
{
	if (m_size == s) return;
	m_size = s;
	if (m_size == BoardSize::SMALL) {
		m_whitebg_char = '-';
		m_blackbg_char = '-';
		m_vline_char = ' ';
		m_hline_char = ' ';
		m_piece_tex = &m_piece_tex_small;
	}
	else {
		m_whitebg_char = '#';
		m_blackbg_char = ' ';
		m_vline_char = '|';
		m_hline_char = '-';
		m_piece_tex = m_size == BoardSize::MEDIUM ? &m_piece_tex_medium : &m_piece_tex_large;
	}
	m_tilewidth = m_piece_tex->front().width;
	m_tileheight = m_piece_tex->front().height;
	tile_x_stride = m_tilewidth + (m_vlines ? 1 : 0);
	tile_y_stride = m_tileheight + (m_hlines ? 1 : 0);
	tile_x_off = (m_rankrow_descr ? 2 : 0) + (m_vlines ? 1 : 0);
	tile_y_off = (m_rankrow_descr ? 1 : 0) + (m_hlines ? 1 : 0);
	m_width = tile_x_off + 8 * tile_x_stride;
	m_height = tile_y_off + 8 * tile_y_stride;
}

void CharBoard::set_grid(bool h, bool v)
{
	if (m_hlines != h) {
		m_hlines = h;
		tile_y_stride = m_tileheight + (m_hlines ? 1 : 0);
		tile_y_off = (m_rankrow_descr ? 1 : 0) + (m_hlines ? 1 : 0);
		m_height = tile_y_off + 8 * tile_y_stride;
	}
	if (m_vlines != v) {
		m_vlines = v;
		tile_x_stride = m_tilewidth + (m_vlines ? 1 : 0);
		tile_x_off = (m_rankrow_descr ? 2 : 0) + (m_vlines ? 1 : 0);
		m_width = tile_x_off + 8 * tile_x_stride;
	}
}

void CharBoard::set_description(bool b)
{
	if (m_rankrow_descr == b) return;
	m_rankrow_descr = b;
	tile_x_off = (m_rankrow_descr ? 2 : 0) + (m_vlines ? 1 : 0);
	tile_y_off = (m_rankrow_descr ? 1 : 0) + (m_hlines ? 1 : 0);
	m_width = tile_x_off + 8 * tile_x_stride;
	m_height = tile_y_off + 8 * tile_y_stride;
}

void CharBoard::set_xpos(int x)
{
	m_posx = x;
}

void CharBoard::set_ypos(int y)
{
	m_posy = y;
}

int CharBoard::get_xpos() const
{
	return m_posx;
}

int CharBoard::get_ypos() const
{
	return m_posy;
}

int CharBoard::get_width() const 
{
	return m_width;
}

bool CharBoard::get_init_ok() const
{
	return m_init_ok;
}

int CharBoard::get_height() const
{
	return m_height;
}

bool CharBoard::load_textures(const std::filesystem::path& path_to_dir)
{
	const std::filesystem::path fsmall = path_to_dir / "pieces_small.txt";
	const std::filesystem::path fmedium = path_to_dir / "pieces_medium.txt";
	const std::filesystem::path flarge = path_to_dir / "pieces_large.txt";

	m_piece_tex = &m_piece_tex_small;
	bool small_ok = load_texture(fsmall);
	m_piece_tex = &m_piece_tex_medium;
	bool medium_ok = load_texture(fmedium);
	m_piece_tex = &m_piece_tex_large;
	bool large_ok = load_texture(flarge);
	return small_ok && medium_ok && large_ok;

}

bool CharBoard::load_texture(const std::filesystem::path& path_to_file)
{
	// Read from the text file
	std::ifstream tex_file(path_to_file);
	if (!tex_file.is_open()) return false;
	// Use a while loop together with the getline() function to read the file line by line
	std::string line;
	if (!bool(std::getline(tex_file, line))) return false;
	const int width = std::stoi(line);
	if (!bool(std::getline(tex_file, line))) return false;
	const int height = std::stoi(line);
	const int size = width * height;

	for (MaskedTexture& mtex : *m_piece_tex) {
		mtex.height = height;
		mtex.width = width;
		mtex.size = size;
		mtex.tex = std::make_unique<char[]>(size);

		for (int i = 0; i < height; i++) {
			if (!bool(std::getline(tex_file, line))) return false;
			if (line.size() != width) return false;
			std::memcpy(mtex.tex.get() + i * width, line.c_str(), sizeof(char) * width);  // this might shoot u in the foot
		}
		mtex.mask = std::make_unique<bool[]>(size);
		for (int i = 0; i < height; i++) {
			if (!bool(std::getline(tex_file, line))) return false;
			if (line.size() != width) return false;
			for (int j = 0; j < width; j++) {
				mtex.mask[i * width + j] = bool(line[j] - '0');
			}
		}
	}
	tex_file.close();
	return true;
}
