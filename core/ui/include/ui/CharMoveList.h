#pragma once

#include "CharDisplay.h"

#include <vector>
#include <array>
#include <string>

#ifndef SAN_LEN_MAX
#define SAN_LEN_MAX 7
#endif // !SAN_LEN_MAX


typedef std::array<char, SAN_LEN_MAX> CharMove;


/// <summary>
/// A table that displays movenr and white/black moves.
/// Moves can be added via push and removed via pop and clear.
/// If the only change after last draw call is push/pop,
/// call the effiecient draw_push/pop method.
/// </summary>
class ICharMoveList {
public:
	virtual ~ICharMoveList() {}

	// virtual ICharMoveList* clone() const = 0;

	virtual int get_width() const = 0;
	virtual int get_height() const = 0;
	virtual int get_xpos()  const = 0;
	virtual int get_ypos() const = 0;
	virtual const std::vector<CharMove>& get_move_list() const = 0;

	virtual void set_xpos(int x) = 0;
	virtual void set_ypos(int y) = 0;
	virtual void set_number_of_moves(int n) = 0;
	virtual void set_move_list(const std::vector<CharMove>& moves) = 0;

	virtual void clear() = 0;
	virtual void push(const std::string& move) = 0;
	virtual void pop() = 0;

	virtual void draw(CharDisplay* display) const = 0;
	virtual void draw_push(CharDisplay* display, const std::string& move) = 0;
	virtual void draw_pop(CharDisplay* display) = 0;
};
	

class CharMoveListH : public ICharMoveList {

public:
	CharMoveListH(int x, int y, int n, int padding = 1, int move_width = SAN_LEN_MAX);

	int get_width() const override;
	int get_height() const override;
	int get_xpos() const override;
	int get_ypos() const override;
	const std::vector<CharMove>& get_move_list() const override;

	void set_xpos(int x) override;
	void set_ypos(int y) override;
	void set_number_of_moves(int n) override;
	void set_move_list(const std::vector<CharMove>& moves) override;

	void clear() override;
	void push(const std::string& move) override;
	void pop() override;

	void draw(CharDisplay* display) const override;
	void draw_push(CharDisplay* display, const std::string& move) override;
	void draw_pop(CharDisplay* display) override;
private:
	void draw_full(CharDisplay* display) const;
	void draw_partial(CharDisplay* display) const;
	void draw_last_move(CharDisplay* display) const;
private:
	int m_posx;
	int m_posy;
	const int m_height;
	int m_width;

	std::vector<CharMove> m_move_list;
	int m_first_disp_idx;
	int m_number_of_disp_turns;
	
	const int m_padding;
	const int m_move_width;
	const int m_move_stride;
	const int m_move_xoff;
};

class CharMoveListV : public ICharMoveList {

public:
	CharMoveListV(int x, int y, int n, int padding = 1, int move_width = SAN_LEN_MAX);

	int get_width() const override;
	int get_height() const override;
	int get_xpos() const override;
	int get_ypos() const override;
	const std::vector<CharMove>& get_move_list() const override;

	void set_xpos(int x) override;
	void set_ypos(int y) override;
	void set_number_of_moves(int n) override;
	void set_move_list(const std::vector<CharMove>& moves) override;

	void clear() override;
	void push(const std::string& move) override;
	void pop() override;

	void draw(CharDisplay* display) const override;
	void draw_push(CharDisplay* display, const std::string& move) override;
	void draw_pop(CharDisplay* display) override;
private:
	void draw_full(CharDisplay* display) const;
	void draw_partial(CharDisplay* display) const;
	void draw_last_move(CharDisplay* display) const;
private:
	int m_posx;
	int m_posy;
	int m_height;
	int m_width;

	std::vector<CharMove> m_move_list;
	int m_first_disp_idx;
	int m_number_of_disp_turns;

	const int m_padding;
	const int m_move_width;
	const int m_move_min_draw_size;
	const int m_movenr_width = 4;
	const int m_move_stride;
	const int m_move_xoff;
	const int m_move_yoff = 3;
};


CharMove to_char_move(const std::string& str);