#pragma once

#include "CharDisplay.h"
#include "engine/GameInterfaceUtil.h"

#include <memory>
#include <vector>
#include <array>
#include <filesystem>

struct MaskedTexture {
	int width = 0;
	int height = 0;
	int size = 0;
	std::unique_ptr<char[]> tex = nullptr;
	std::unique_ptr<bool[]> mask = nullptr;
};

enum class BoardSize {
	SMALL,
	MEDIUM,
	LARGE
};

class CharBoard
{
public:
	CharBoard(int x, int y, BoardSize b, std::filesystem::path path_to_res, bool hlines = true, bool vlines = true, bool description = true);

	void draw(CharDisplay* display, const std::vector<TileP>& all_tiles) const;
	void draw_delta(CharDisplay* display, const std::vector<TileP>& delta_tiles) const;
	
	void set_size(BoardSize s);
	void set_grid(bool horizontal, bool vertical);
	void set_description(bool b);
	void set_xpos(int x);
	void set_ypos(int y);

	int get_xpos() const;
	int get_ypos() const;
	int get_height() const;
	int get_width() const;
	bool get_init_ok() const;

private:
	const MaskedTexture& get_piece_tex(Piece piece, ChessColor color) const;
	void draw_board(CharDisplay* display) const;
	void draw_board_full(CharDisplay* display) const;
	void draw_board_partial(CharDisplay* display) const;
	void draw_pieces(CharDisplay* display, const std::vector<TileP>& v) const;
	void draw_pieces_partial(CharDisplay* display,const std::vector<TileP>& pieces) const;
	bool load_textures(const std::filesystem::path& path_to_dir);
	bool load_texture(const std::filesystem::path& path_to_file);
private:
	int m_posx;
	int m_posy;
	int m_width;
	int m_height;
	int m_tilewidth;
	int m_tileheight;
	bool m_hlines;
	bool m_vlines;
	bool m_rankrow_descr;
	bool m_init_ok;
	BoardSize m_size;

	std::array<MaskedTexture, 13>* m_piece_tex;

	std::array<MaskedTexture, 13> m_piece_tex_small;
	std::array<MaskedTexture, 13> m_piece_tex_medium;
	std::array<MaskedTexture, 13> m_piece_tex_large;

	int tile_x_off;
	int tile_y_off;
	int tile_x_stride;
	int tile_y_stride;

	char m_hline_char = '-';
	char m_vline_char = '|';
	char m_blackbg_char = ' ';
	char m_whitebg_char = '#';
};

