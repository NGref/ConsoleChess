#include "Game.h"

#include <sstream>
#include <algorithm>


Game::Game(const std::string& fen, GameMoveStrFmt fmt, uint8_t MAX_HALF_TURNS) :
	M_DEFAULT_FEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"),
	m_board(),
	m_swap_vars(),
	m_gamedelta_list(), m_legal_moves(), m_pseudo_moves(), m_block_check_indices(),
	m_pinned_direction(),
	m_ending_gamestate(),
	m_string_fmt(fmt==GameMoveStrFmt::DEFAULT ? GameMoveStrFmt::UCI : fmt),
	m_p2_index(-1),
	m_fen_valid(true),
	m_game_has_ended(false),
	m_turn_number(1),
	M_MAX_HALF_TURNS(MAX_HALF_TURNS),
	m_half_turn_number(0)
{
	m_pseudo_moves.reserve(28);
	m_legal_moves.reserve(80);
	m_gamedelta_list.reserve(100);

	if (!fen.empty()) {
		m_fen_valid = init_fen(fen);
		if (!m_fen_valid) return;
	}

	find_pinned_pieces();
	find_legal_moves();
	update_game_has_ended(get_is_check());
}

Game::Game(const Game& other) :
	m_board(other.m_board),
	m_swap_vars(other.m_swap_vars),
	m_gamedelta_list(other.m_gamedelta_list),
	m_legal_moves(other.m_legal_moves),
	m_pseudo_moves(other.m_pseudo_moves),
	m_block_check_indices(other.m_block_check_indices),
	M_DEFAULT_FEN(other.M_DEFAULT_FEN),
	m_pinned_direction(other.m_pinned_direction),
	m_ending_gamestate(other.m_ending_gamestate),
	m_string_fmt(other.m_string_fmt),
	m_p2_index(other.m_p2_index),
	m_fen_valid(other.m_fen_valid),
	m_game_has_ended(other.m_game_has_ended),
	m_turn_number(other.m_turn_number),
	M_MAX_HALF_TURNS(other.M_MAX_HALF_TURNS),
	m_half_turn_number(other.m_half_turn_number)
{
	// fix swapvar ptr
	m_swap_vars.active = &m_swap_vars.white ; 
	m_swap_vars.passive = &m_swap_vars.black;
	if (other.m_swap_vars.active->color.IsBlack()) m_swap_vars.Swap();
}

std::unique_ptr<IGame> Game::clone() const
{
	return std::make_unique<Game>(*this);
}

Game::~Game()
{
	return;
}

bool Game::get_init_ok() const
{
	return m_fen_valid;
}

ChessColor Game::get_active_color() const
{
	return m_swap_vars.active->color;
}

int Game::get_turn_number() const
{
	return m_turn_number;
}

std::vector<GameMove> Game::get_possible_moves() const 
{
	std::vector<GameMove> vret;
	vret.reserve(m_legal_moves.size());
	for (const GameMoveInt& m : m_legal_moves) {
		vret.emplace_back(m.get_from(), m.get_to(), m.get_promotion());
	}
	return vret;
}

std::vector<std::string> Game::get_possible_moves_str() const
{
	std::vector<std::string> vret;
	vret.reserve(m_legal_moves.size());
	for (const GameMoveInt& m : m_legal_moves) {
		vret.push_back(legal_to_string(m));
	}
	return vret;
}

std::vector<GameMove> Game::get_possible_moves(int from_ind) const
{
	std::vector<GameMove> vret;
	vret.reserve(28);
	for (const GameMoveInt& m : m_legal_moves) {
		if (m.get_from() == from_ind) {
			vret.emplace_back(m.get_from(), m.get_to(), m.get_promotion());
		}
	}
	vret.shrink_to_fit();
	return vret;
}

std::vector<int> Game::get_possible_moves_ind(int index) const
{
	std::vector<int> vret;
	vret.reserve(28);
	int pawn_to = 0, pawn_to_last = -1;
	for (const GameMoveInt& ms : m_legal_moves) {
		if (ms.get_from() == index) {
			if (m_board.get_piece_from_bindex(index) != Piece::PAWN)
				vret.push_back(ms.get_to());
			else {  // filter multiple promotion moves assuming they come in order XDDDDDDDDD
				pawn_to = ms.get_to();
				if (pawn_to != pawn_to_last) vret.push_back(pawn_to);
				pawn_to_last = pawn_to;
			}
		}
	}
	vret.shrink_to_fit();
	return vret;
}

std::vector<std::string> Game::get_possible_moves_str(const std::string& from_str) const
{
	//TODO
	return std::vector<std::string>();
}

std::vector<TileI> Game::get_all_tiles_ind() const
{
	std::vector<TileI> vret;
	vret.reserve(32);
	for (int bindex = 0; bindex < GAME_BOARD_SIZE; bindex++) {
		const UniquePiece up = m_board.get_up(bindex);
		if (!up.IsEmpty()) vret.emplace_back(bindex, up.p, up.GetColor());
	}
	vret.shrink_to_fit();
	return vret;
}

std::vector<TileI> Game::get_new_tiles_ind() const
{
	if (m_gamedelta_list.empty()) return std::vector<TileI>();
	std::vector<TileI> vret;
	vret.reserve(4);
	const GameDelta& gd_last = m_gamedelta_list.back();

	const UniquePiece& up_from = m_board.get_up(gd_last.move.from);
	Position p_from = bindex_to_position(gd_last.move.from);
	vret.emplace_back(gd_last.move.from, up_from.p, up_from.GetColor());

	const UniquePiece& up_to = m_board.get_up(gd_last.move.to);
	Position p_to = bindex_to_position(gd_last.move.to);
	vret.emplace_back(gd_last.move.to, up_to.p, up_to.GetColor());


	if (gd_last.IsEnPassant()) {
		vret.emplace_back(position_to_bindex({ p_to.x, p_from.y }), Piece::EMPTY, ChessColor{});
	}

	if (gd_last.IsCastle()) {
		const int rook_bindex = gd_last.move.from + (gd_last.IsKSCastle() ? 3 : -4);
		const int king_adjacent_bindex = gd_last.move.from + (gd_last.IsKSCastle() ? 1 : -1);
		vret.emplace_back(rook_bindex, Piece::EMPTY, ChessColor{});
		vret.emplace_back(king_adjacent_bindex, Piece::ROOK, up_to.GetColor());
	}

	return vret;
}

std::vector<TileI> Game::get_reverse_new_tiles_ind() const
{
	if (m_gamedelta_list.empty()) return std::vector<TileI>();
	std::vector<TileI> vret;
	vret.reserve(4);
	const GameDelta& gd_last = m_gamedelta_list.back();
	UniquePiece up_from = m_board.get_up(gd_last.move.to);
	if (gd_last.IsPromotion()) up_from.p = Piece::PAWN;
	vret.emplace_back(gd_last.move.from, up_from.p, up_from.GetColor());

	UniquePiece up_to{};
	if (gd_last.IsTakes()) {
		up_to = gd_last.takes;
	}

	if (gd_last.IsEnPassant()) {
		vret.emplace_back(gd_last.move.to - m_swap_vars.passive->pawn_forward, gd_last.takes.p, gd_last.takes.GetColor());
		up_to = UniquePiece();
	}
	vret.emplace_back(gd_last.move.to, up_to.p, up_to.GetColor());

	if (gd_last.IsCastle()) {
		const int corner_bindex = gd_last.move.from + (gd_last.IsKSCastle() ? 3 : -4);
		const int king_adjacent_bindex = gd_last.move.from + (gd_last.IsKSCastle() ? 1 : -1);
		//const int rook_id = m_bindex_to_id[king_adjacent_bindex];

		vret.emplace_back( corner_bindex, Piece::ROOK, up_from.GetColor());
		vret.emplace_back(king_adjacent_bindex, Piece::EMPTY, ChessColor{});
	}
	return vret;
}

GameMove Game::get_last_move() const
{
	return m_gamedelta_list.back().move;
}

std::string Game::get_last_move_str(GameMoveStrFmt fmt) const
{
	if (m_gamedelta_list.empty()) return std::string();
	if (fmt == GameMoveStrFmt::DEFAULT) fmt = m_string_fmt;

	return gd_to_string(m_gamedelta_list.back());
}


std::vector<GameMove> Game::get_all_moves() const
{
	std::vector<GameMove> vret;
	vret.reserve(100);
	for (const GameDelta& gd : m_gamedelta_list) {
		vret.push_back(gd.move);
	}
	vret.shrink_to_fit();
	return vret;
}

std::vector<std::string> Game::get_all_moves_str(GameMoveStrFmt fmt) const
{
	// TODO
	return std::vector<std::string>();
}

bool Game::get_is_check() const
{
	const int active_king_index = m_board.get_bindex(m_swap_vars.active->king_id);
	return m_board.is_covered_color(active_king_index, m_swap_vars.passive->color_offset);
}

bool Game::get_game_has_ended() const
{
	return m_game_has_ended;
}

GameEndState Game::get_ending_game_state() const
{
	return m_ending_gamestate;
}

GameMoveStrFmt Game::get_move_str_fmt() const
{
	return m_string_fmt;
}

/// <summary>
/// string wrapper for move
/// </summary>
/// <param name="m"></param>
/// <param name="fmt"></param>
/// <returns></returns>
GameState Game::move(const std::string& m, GameMoveStrFmt fmt)
{
	if (fmt == GameMoveStrFmt::DEFAULT) {
		fmt = m_string_fmt;
	}

	switch (fmt)
	{
	case GameMoveStrFmt::DEFAULT: return GameState::INVALID_MOVE;
	case GameMoveStrFmt::UCI: return move(uci_to_gamemove(m));
	case GameMoveStrFmt::SAN: return move(san_to_gamemove(m));
	case GameMoveStrFmt::LAN: return move(lan_to_gamemove(m));
	}
}


/// <summary>
/// if move is legal in current position it is applied.
/// -Updates board, castle rights etc.
/// -pushes delta
/// -swaps active color and find new legal moves
/// -updates game ending state
/// 
/// </summary>
/// <param name="move">proposal by the active agent</param>
/// <returns>GAME_STATE: GAME_HAS_ENDED if legal move ended the game else VALID_MOVE/INVALID_MOVE</returns>
GameState Game::move(const GameMove& move)
{
	//validate game state and move
	if (m_game_has_ended) return GameState::INVALID_MOVE;
	const GameMoveInt move_int = gm_to_gmi(move);
	auto it = std::find(m_legal_moves.begin(), m_legal_moves.end(), move_int);
	if (it == m_legal_moves.end()) return GameState::INVALID_MOVE;
	
	GameDelta gd = legal_to_gd(move);
	gd.white_castle = m_swap_vars.white.castles;
	gd.black_castle = m_swap_vars.black.castles;
	gd.half_turns = m_half_turn_number;
	gd.p2_index = m_p2_index;

	//execute move on board
	m_board.apply_gamedelta(gd);
	update_castles(gd);
	update_p2_index(gd);

	//end turn
	if (m_swap_vars.active->color.IsBlack()) m_turn_number += 1;
	m_half_turn_number += 1;
	if (gd.IsTakes() || (m_board.get_piece_from_bindex(gd.move.to) == Piece::PAWN)) m_half_turn_number = 0;

	m_swap_vars.Swap();

	gd.check = get_is_check();
	m_gamedelta_list.push_back(gd);

	find_pinned_pieces();
	find_legal_moves();
	update_game_has_ended(gd.check);

	if (m_game_has_ended) return GameState::GAME_HAS_ENDED;

	return GameState::VALID_MOVE;
}

void Game::undo()
{
	if (m_gamedelta_list.empty()) return;

	const GameDelta& gd_last = m_gamedelta_list.back();
	if (m_game_has_ended) m_game_has_ended = false;
	m_swap_vars.Swap();
	if (m_swap_vars.active->color.IsBlack()) m_turn_number--;
	m_half_turn_number = gd_last.half_turns;
	undo_update_castles(gd_last.white_castle, gd_last.black_castle);
	undo_update_p2_index(gd_last.p2_index);
	m_board.undo_gamedelta(gd_last);

	m_gamedelta_list.pop_back();

	find_pinned_pieces();
	find_legal_moves();

	return;
}

void Game::new_game(const std::string& fen)
{
	m_pseudo_moves.reserve(28);
	m_legal_moves.reserve(80);
	m_gamedelta_list.reserve(100);

	if (!fen.empty()) {
		m_fen_valid = init_fen(fen);
		if (!m_fen_valid) return;
	}
	else {
		m_fen_valid = init_fen(M_DEFAULT_FEN);
	}

	find_pinned_pieces();
	find_legal_moves();
	update_game_has_ended(get_is_check());
}

void Game::set_ending_game_state(GameEndState ges)
{
	m_game_has_ended = true;
	m_ending_gamestate = ges;
}

void Game::set_move_str_fmt(GameMoveStrFmt fmt)
{
	if (fmt == GameMoveStrFmt::DEFAULT) return;
	m_string_fmt = fmt;
	return;
}

uint64_t Game::perft(int depth)
{
	if (m_game_has_ended) return 0;
	if (depth == 0) return 1;
	if (depth == 1) return m_legal_moves.size();

	const std::vector<GameMoveInt> legal = m_legal_moves;
	const std::array<Direction, GAME_MAX_ID> pinned = m_pinned_direction;

	int number_of_moves = 0;
	for (const GameMoveInt m : legal) {
		perft_move(m);
		number_of_moves += perft(depth - 1);
		perft_undo();
		//should be able to comment those two out but cant???????
		m_pinned_direction = pinned;
		m_legal_moves = legal;
	}
	return number_of_moves;
}

void Game::perft_divide(int depth)
{
	std::vector<std::string> legal_str = get_possible_moves_str();
	std::sort(legal_str.begin(), legal_str.end());

	//const std::vector<GameMoveInt> legal = m_legal_moves;
	const std::array<Direction, GAME_MAX_ID> pinned = m_pinned_direction;


	uint64_t tot = 0;
	for (const std::string& m_str : legal_str) {
		std::cout << m_str << ": ";
		GameMove m = uci_to_gamemove(m_str);
		GameMoveInt m_int(m.from, m.to, m.promotion);
		perft_move(m_int);
		uint64_t n = perft(depth - 1);
		tot += n;
		std::cout << n << "\n";
		perft_undo();
		m_pinned_direction = pinned;
	}
	std::cout << "\n";
	std::cout << "total: " << tot << "\n";
	return;
}

void Game::save_board_DEBUG()
{
	std::ofstream myfile("coverage.txt", std::ios::app);
	if (myfile.is_open())
	{
		for (int y = 7; y >= 0; y--) {
			std::string sb;
			std::string scw;
			std::string scb;
			std::string pin;
			for (int x = 0; x < 8; x++) {
				int bindex = y * 8 + x;
				UniquePiece up = m_board.get_up(bindex);
				char cp = piece_to_char(up.p);
				if (up.IsBlack()) cp = std::tolower(cp);
				sb += cp;
				bool bc = m_board.is_covered_color(bindex, 16);
				bool wc = m_board.is_covered_color(bindex, 0);
				if (up.IsEmpty()) {
					scw += wc ? "1" : "0";
					scb += bc ? "1" : "0";
				}
				else if (up.IsWhite()) {
					scw += wc ? "3" : "0";
					scb += bc ? "2" : "0";
				}
				else {
					scw += wc ? "2" : "0";
					scb += bc ? "3" : "0";
				}
				pin += bindex_to_pinned_dir_char_DEBUG(bindex);;
			}
			myfile << sb << "  " << scw << "  " << scb << "  " << pin << "\n";
			sb.clear();
			scw.clear();
			scb.clear();
		}
		myfile << "\n";
		myfile.close();
	}
	return;
}

bool operator==(const Game& lhs, const Game& rhs)
{
	if (lhs.M_DEFAULT_FEN != rhs.M_DEFAULT_FEN) return false;
	if (lhs.m_board != rhs.m_board) return false;
	if (lhs.m_swap_vars != rhs.m_swap_vars) return false;
	if (lhs.m_gamedelta_list != rhs.m_gamedelta_list) return false;
	if (lhs.m_legal_moves != rhs.m_legal_moves) return false;
	if (lhs.m_pseudo_moves != rhs.m_pseudo_moves) return false;
	if (lhs.m_block_check_indices != rhs.m_block_check_indices) return false;
	if (lhs.m_pinned_direction != rhs.m_pinned_direction) return false;
	if (lhs.m_p2_index != rhs.m_p2_index) return false;
	if (lhs.m_game_has_ended != rhs.m_game_has_ended) return false;
	if (lhs.m_turn_number != rhs.m_turn_number) return false;
	if (lhs.m_half_turn_number != rhs.m_half_turn_number) return false;
	return true;
}


bool Game::is_en_passant(const GameMove& m)
{
	if (m_p2_index != -1 && m_board.get_piece_from_bindex(m.from) == Piece::PAWN && m_board.get_piece_from_bindex(m.to) == Piece::EMPTY) {
		const int d_from = std::abs(m_p2_index - m.from);
		const int d_to = std::abs(m_p2_index - m.to);
		if (d_from == 1 && d_to == 8) return true;
	}
	return false;
}

bool Game::is_en_passant(const GameMoveInt& m)
{
	const int from_bindex = m.get_from();
	const int to_bindex = m.get_to();
	if (m_p2_index != -1 && m_board.get_piece_from_bindex(from_bindex) == Piece::PAWN && m_board.get_piece_from_bindex(to_bindex) == Piece::EMPTY) {
		const int d_from = std::abs(m_p2_index - from_bindex);
		const int d_to = std::abs(m_p2_index - to_bindex);
		if (d_from == 1 && d_to == 8) return true;
	}
	return false;
}

/// <summary>
/// initializes the game from a fen string. returns true if initialization was successful and false if not 
/// (note that the fen string has to match the board dimensions).
///
/// </summary>
/// <param name="fen_string">
/// fen string as defined in the standard:
/// 
/// </param>
/// <returns>
/// bool 
/// </returns>
bool Game::init_fen(const std::string& fen_string)
{
	int number_of_sections = std::count(fen_string.begin(), fen_string.end(), ' ') + 1;
	if (number_of_sections != 6) return false;

	//validate board
	size_t section_off = 0;
	const std::string board_section = fen_string.substr(section_off, fen_string.find(' ', section_off) - section_off);
	bool board_valid = m_board.new_board(board_section);
	if (!board_valid) return false;

	//validate active color
	section_off += board_section.size() + 1;
	const std::string active_color_section = fen_string.substr(section_off, fen_string.find(' ', section_off) - section_off);
	if (active_color_section.size() != 1) return false;
	if (active_color_section[0] == 'w') {
		m_swap_vars.active = &m_swap_vars.white;
		m_swap_vars.passive = &m_swap_vars.black;
	}
	else if (active_color_section[0] == 'b') {
		m_swap_vars.active = &m_swap_vars.black;
		m_swap_vars.passive = &m_swap_vars.white;
	}
	else return false;

	//validate castles
	section_off += active_color_section.size() + 1;
	const std::string castles_section = fen_string.substr(section_off, fen_string.find(' ', section_off) - section_off);
	bool castle_valid = init_fen_castles(castles_section);
	if (!castle_valid) return false;

	// validate p2 index
	section_off += castles_section.size() + 1;
	const std::string en_passant_section = fen_string.substr(section_off, fen_string.find(' ', section_off) - section_off);
	bool p2_valid = init_fen_p2_index(en_passant_section);
	if (!p2_valid) return false;

	// validate half turn
	section_off += en_passant_section.size() + 1;
	const std::string half_turns_section = fen_string.substr(section_off, fen_string.find(' ', section_off) - section_off);
	if (half_turns_section.empty()) return false;
	m_half_turn_number = 0;
	for (const char c : half_turns_section) {
		if (isdigit(c)) {
			m_half_turn_number *= 10;
			m_half_turn_number += int(c - '0');
		}
		else return false;
	}

	//validate turn number
	section_off += half_turns_section.size() + 1;
	const std::string number_of_turns_section = fen_string.substr(section_off);
	if (number_of_turns_section.empty()) return false;
	m_turn_number = 0;
	for (const char c : number_of_turns_section) {
		if (isdigit(c)) {
			m_turn_number *= 10;
			m_turn_number += int(c - '0');
		}
		else return false;
	}

	//additional constraints
	//kings are not adjacent
	const Position white_king_pos = bindex_to_position(m_board.get_bindex(m_swap_vars.white.king_id));
	const Position black_king_pos = bindex_to_position(m_board.get_bindex(m_swap_vars.black.king_id));
	if (std::abs(white_king_pos.x - black_king_pos.x) <= 1 && std::abs(white_king_pos.y - black_king_pos.y) <= 1) return false;


	//pawns are not on last rank

	//passive king is not in check

	//p2 pawn did not jump a piece

	//if p2 pawn check, no queen/bishop in same dir
	// enemy bishop or queen dont come after p2 (should be moved to fen constraints)
	/*const DIRECTION check_dir = GetDirHVD(active_king_index, m_p2_index);
	const int8_t nsteps = GetOOBSteps(m_p2_index, check_dir);
	const int d_bindex_dir = DDIR[check_dir];
	int curr_index = m_p2_index;
	for (int8_t n = 1; n <= nsteps; n++) {
		curr_index += d_bindex_dir;
		const UniquePiece up = m_board.get_up(curr_index);
		if (up.IsEmpty()) continue;
		else if (up.IsEnemy(m_active_color)) {
			if (up.p == PIECE::BISHOP || up.p == PIECE::QUEEN) return;
			else break;
		}
		else break;
	}*/
	return true;
}

bool Game::init_fen_castles(const std::string& fen_castles_section)
{
	m_swap_vars.white.castles.kscastle = false;
	m_swap_vars.white.castles.qscastle = false;
	m_swap_vars.black.castles.kscastle = false;
	m_swap_vars.black.castles.qscastle = false;

	if (fen_castles_section.size() < 1 || fen_castles_section.size() > 4) return false;
	int white_king_count = 0;
	int white_queen_count = 0;
	int black_king_count = 0;
	int black_queen_count = 0;
	int hyphen_count = 0;
	for (const char c : fen_castles_section) {
		switch (c)
		{
		case 'K':
			white_king_count++;
			break;
		case 'Q':
			white_queen_count++;
			break;
		case 'k':
			black_king_count++;
			break;
		case 'q':
			black_queen_count++;
			break;
		case '-':
			hyphen_count++;
			break;
		default:
			return false;
		}
	}

	if (fen_castles_section.size() == 1 && hyphen_count == 1) return true;
	if (hyphen_count) return false;
	if (white_king_count) {
		m_swap_vars.white.castles.kscastle = true;
		if (white_king_count > 1) return false;
	}
	if (white_queen_count) {
		m_swap_vars.white.castles.qscastle = true;
		if (white_queen_count > 1) return false;
	}
	if (black_king_count) {
		m_swap_vars.black.castles.kscastle = true;
		if (black_king_count > 1) return false;
	}
	if (black_queen_count) {
		m_swap_vars.black.castles.qscastle = true;
		if (black_queen_count > 1) return false;
	}

	//could also check that white comes before black
	return true;
}

bool Game::init_fen_p2_index(const std::string& fen_ep_section)
{
	if (fen_ep_section.size() == 1 && fen_ep_section[0] == '-') {
		m_p2_index = -1;
		return true;
	}
	if (fen_ep_section.size() == 2) {
		const char file = fen_ep_section[0];
		if (file < 'a' || file > 'h') return false;
		const int x = file - 'a';
		const char rank = fen_ep_section[1];
		if (rank != '3' && rank != '6') return false;
		const int y = rank - '1';
		const int forward = y == 3 ? GAME_WIDTH : -GAME_WIDTH;
		m_p2_index = GAME_WIDTH * y + x + forward;
		return true;
	}
	return false;
}

void Game::find_legal_moves()
{
	m_legal_moves.clear();
	const int active_king_id = m_swap_vars.active->king_id;
	const int active_king_index = m_board.get_bindex(active_king_id);
	const int coverage_cnt = m_board.get_cover_count_color(active_king_index, m_swap_vars.passive->color_offset);
	const bool is_double_check = coverage_cnt > 1;
	const bool is_check = coverage_cnt == 1;

	if (is_double_check) {
		piece_moves(active_king_id);
		return;
	}
	if (is_check) {
		const int check_id = m_board.get_first_cover_id_color(active_king_index, m_swap_vars.passive->color_offset);
		const int check_index = m_board.get_bindex(check_id);
		find_block_indices(active_king_index, check_index);
		find_pseudo_moves();
		append_en_passant_if_resolves_check(active_king_index, check_id);
		filter_pinned_moves();
		filter_block_moves();
		append_and_clear();
		m_block_check_indices.clear();
		return;
	}
	find_pseudo_moves();
	filter_pinned_moves();
	append_and_clear();
	return;
}

void Game::find_pseudo_moves()
{
	const int active_king_id = m_swap_vars.active->king_id;
	const int id_end = active_king_id + GAME_MAX_COLOR_ID;
	for (int id = active_king_id; id < id_end; id++) {
		if (m_board.get_piece_from_id(id) != Piece::EMPTY) piece_moves(id);
	}
}

void Game::find_pinned_pieces()
{
	clear_pinned();

	int active_king_bindex = m_board.get_bindex(m_swap_vars.active->king_id);

	// horizontal and vertical pin
	for (Direction dir = Direction::N; dir <= Direction::W; ++dir) {
		find_pinned_direction(dir, active_king_bindex, Piece::ROOK);
	}
	// diagonal pin
	for (Direction dir = Direction::NE; dir <= Direction::NW; ++dir) {
		find_pinned_direction(dir, active_king_bindex, Piece::BISHOP);
	}
	return;
}

void Game::find_pinned_direction(const Direction dir, const int king_bindex, const Piece dirp)
{
	int8_t nsteps = GetOOBSteps(king_bindex, dir);
	int d = get_bindex_delta(dir);
	int to_index = king_bindex;
	int first_id = -1;

	for (int8_t n = 1; n <= nsteps; n++) {
		to_index += d;
		UniquePiece up = m_board.get_up(to_index);
		if (up.IsEmpty()) continue;
		else if (up.IsAlly(m_swap_vars.active->color)) {
			if (first_id == -1) first_id = up.id;
			else return;
		}
		else {
			if (up.p != dirp && up.p != Piece::QUEEN) return;
			else if (first_id != -1) {
				m_pinned_direction[first_id] = dir;
				return;
			}
			else return;
		}
	}
	return;
}

void Game::clear_pinned()
{
	m_pinned_direction.fill(Direction::NONE);
	return;
}

void Game::piece_moves(int id)
{
	const int index = m_board.get_bindex(id);
	switch (m_board.get_piece_from_id(id)) {
	case (Piece::KING):   return king_moves(index);
	case (Piece::QUEEN):  return queen_moves(index);
	case (Piece::BISHOP): return bishop_moves(index);
	case(Piece::KNIGHT):  return knight_moves(index);
	case(Piece::ROOK):    return rook_moves(index);
	case(Piece::PAWN):    return pawn_moves(index);
	default:              return;
	}
}

void Game::king_moves(int from_index)
{
	for (Direction dir = Direction::N; dir <= Direction::NW; ++dir) {
		int8_t nsteps = GetOOBSteps(from_index, dir);
		if (nsteps > 0) {
			int to_index = from_index + get_bindex_delta(dir);
			UniquePiece up_to = m_board.get_up(to_index);
			if (!m_board.is_covered_color(to_index, m_swap_vars.passive->color_offset)) {
				if (!up_to.IsAlly(m_swap_vars.active->color)) m_legal_moves.emplace_back(from_index, to_index);
			}
		}
	}
	if (m_swap_vars.active->castles.kscastle) ksc_append(from_index);
	if (m_swap_vars.active->castles.qscastle) qsc_append(from_index);
	return;
}

void Game::queen_moves(int bindex)
{
	rook_moves(bindex);
	bishop_moves(bindex);
}

void Game::bishop_moves(int from_index)
{
	for (Direction dir = Direction::NE; dir <= Direction::NW; ++dir) {
		move_and_append(dir, from_index);
	}
}

void Game::knight_moves(int from_index)
{
	for (Direction dir = Direction::NNE; dir <= Direction::NNW; ++dir) {
		move_and_append(dir, from_index);
	}
}

void Game::rook_moves(int from_index)
{
	for (Direction dir = Direction::N; dir <= Direction::W; ++dir) {
		move_and_append(dir, from_index);
	}
}

void Game::pawn_moves(int from_index)
{
	const int forward = m_swap_vars.active->pawn_forward;
	const int starting_y = m_swap_vars.active->pawn_start_y;
	const int promo_y = m_swap_vars.active->pawn_promo_y;

	Position pos = bindex_to_position(from_index);

	if (pos.y == promo_y) {
		const std::array<Piece, 4> promo_piece = {Piece::QUEEN, Piece::ROOK, Piece::BISHOP, Piece::KNIGHT};
		//single_forward
		int to_index = from_index + forward;
		const UniquePiece up = m_board.get_up(to_index);
		if (up.IsEmpty()) {
			for (const Piece pp : promo_piece) {
				m_pseudo_moves.emplace_back(from_index, to_index, pp);
			}
		}
		//takes_left
		if (pos.x > 0) {
			int to_index = from_index + forward - 1;
			const UniquePiece up = m_board.get_up(to_index);
			if (up.IsEnemy(m_swap_vars.active->color)) {
				for (const Piece pp : promo_piece) {
					m_pseudo_moves.emplace_back(from_index, to_index, pp);
				}
			}
		}

		//takes_right
		if (pos.x < GAME_WIDTH - 1) {
			int to_index = from_index + forward + 1;
			UniquePiece up = m_board.get_up(to_index);
			if (up.IsEnemy(m_swap_vars.active->color)) {
				for (const Piece pp : promo_piece) {
					m_pseudo_moves.emplace_back(from_index, to_index, pp);
				}
			}
		}
		return;
	}

	//single_forward
	int to_index = from_index + forward;
	UniquePiece up = m_board.get_up(to_index);
	if (up.IsEmpty()) m_pseudo_moves.emplace_back(from_index, to_index);
	
	//takes_left
	if (pos.x > 0) {
		int to_index = from_index + forward - 1;
		UniquePiece up = m_board.get_up(to_index);
		if (up.IsEnemy(m_swap_vars.active->color)) {
			m_pseudo_moves.emplace_back(from_index, to_index);
		}
	}
	
	//takes_right
	if (pos.x < 7) {
		int to_index = from_index + forward + 1;
		UniquePiece up = m_board.get_up(to_index);
		if (up.IsEnemy(m_swap_vars.active->color)) {
			m_pseudo_moves.emplace_back(from_index, to_index);
		}
	}
	
	// double forward
	if (pos.y == starting_y) {
		int to_index = from_index + 2 * forward;
		UniquePiece upskip = m_board.get_up(from_index + forward);
		UniquePiece up = m_board.get_up(to_index);
		if (upskip.IsEmpty() && up.IsEmpty()) m_pseudo_moves.emplace_back(from_index, to_index); //m_pseudo_moves.push_back(GAME_MOVE_TYPE(from_index, to_index));
	}

	//en_passant
	if (m_p2_index!=-1) {
		Position p2pos = bindex_to_position(m_p2_index);
		
		int dx = p2pos.x - pos.x;
		int dy = p2pos.y - pos.y;
		if (dy==0 && (dx == 1 || dx == -1)) {
			// weired special case
			if (!en_passant_is_self_check(pos.x,pos.y,p2pos.x)) {
				m_pseudo_moves.emplace_back(from_index, from_index + forward + dx);
			}  
		}
	}
	return;
}

void Game::move_and_append(const Direction dir, const int from_index)
{
	int8_t nsteps = GetOOBSteps(from_index, dir);
	int d = get_bindex_delta(dir);
	int to_index = from_index;
	for (int8_t n = 1; n <= nsteps; n++) {
		to_index += d;
		const UniquePiece up = m_board.get_up(to_index);
		if (up.IsEmpty()) m_pseudo_moves.emplace_back(from_index, to_index);
		else if (up.IsEnemy(m_swap_vars.active->color)) {
			m_pseudo_moves.emplace_back(from_index, to_index);
			return;
		}
		else return;
	}
	return;
}

void Game::ksc_append(int from_index)
{
	if (m_board.is_covered_color(from_index,m_swap_vars.passive->color_offset)) return; 
	if (m_board.is_covered_color(from_index + 1, m_swap_vars.passive->color_offset)) return;
	if (m_board.is_covered_color(from_index + 2, m_swap_vars.passive->color_offset)) return;
	
	if (!m_board.get_up(from_index + 1).IsEmpty()) return;
	if (!m_board.get_up(from_index + 2).IsEmpty()) return;

	m_legal_moves.emplace_back(from_index, from_index + 2);
	return;
	
}

void Game::qsc_append(int from_index)
{
	if (m_board.is_covered_color(from_index, m_swap_vars.passive->color_offset)) return;
	if (m_board.is_covered_color(from_index - 1, m_swap_vars.passive->color_offset)) return;
	if (m_board.is_covered_color(from_index - 2, m_swap_vars.passive->color_offset)) return;

	if (!m_board.get_up(from_index - 1).IsEmpty()) return;
	if (!m_board.get_up(from_index - 2).IsEmpty()) return;
	if (!m_board.get_up(from_index - 3).IsEmpty()) return;

	m_legal_moves.emplace_back(from_index, from_index - 2);
	return;
}

bool Game::en_passant_is_self_check(int from_x, int from_y, int ep_x)
{
	int active_king_index = m_board.get_bindex(m_swap_vars.active->king_id);
	Position kpos = bindex_to_position(active_king_index);

	if (kpos.y == from_y) {
		if (kpos.x < from_x) {
			for (int x = kpos.x + 1; x < GAME_WIDTH; x++) {
				if (x == from_x || x == ep_x) continue;
				UniquePiece up = m_board.get_up(position_to_bindex({ x, from_y }));
				if (up.IsEmpty()) continue;
				else if (up.IsAlly(m_swap_vars.active->color)) return false;
				else {
					if (up.p == Piece::ROOK || up.p == Piece::QUEEN) {
						return true;
					}
					else return false;
				}
			}
		}
		else {
			for (int x = kpos.x - 1; x >= 0; x--) {
				if (x == from_x || x == ep_x) continue;
				UniquePiece up = m_board.get_up(position_to_bindex({ x, from_y }));
				if (up.IsEmpty()) continue;
				else if (up.IsAlly(m_swap_vars.active->color)) return false;
				else {
					if (up.p == Piece::ROOK || up.p == Piece::QUEEN) {
						return true;
					}
					else return false;
				}
			}
		}
	}
	return false;
}

void Game::filter_pinned_moves()
{
	for (GameMoveInt& m : m_pseudo_moves) {
		if (!m.is_null()) {
			const int from_bindex = m.get_from();
			const int from_id = m_board.get_id(from_bindex);
			Direction pdir = m_pinned_direction[from_id];
			if (pdir != Direction::NONE) {
				Direction mdir = get_hvd(from_bindex, m.get_to());
				if (!(pdir==mdir || pdir==get_opposite_direction(mdir))) m.set_null();
			}
		}
	}
	return;
}

void Game::filter_block_moves()
{
	for (GameMoveInt& m : m_pseudo_moves) {
		if (!m.is_null()) {
			bool found = false;
			for (const int block_index : m_block_check_indices) {
				if (m.get_to() == block_index) {
					found = true;
					break;
				}
			}
			if (!found) m.set_null();
		}
	}
	return;
}

void Game::find_block_indices(int king_index, int check_index)
{
	m_block_check_indices.push_back(check_index);
	Direction cdir = get_hvd(king_index, check_index);
	if (cdir==Direction::NONE) return;

	int8_t nsteps = GetOOBSteps(king_index, cdir);
	int d = get_bindex_delta(cdir);
	int to_index = king_index;
	for (int8_t n = 1; n <= nsteps; n++) {
		to_index += d;
		const UniquePiece up = m_board.get_up(to_index);
		if (up.IsEmpty()) {
			m_block_check_indices.push_back(to_index);
		}
		else return;
	}
}

void Game::append_and_clear()
{
	for (const GameMoveInt& m : m_pseudo_moves) {
		if (!m.is_null()) m_legal_moves.push_back(m);
	}
	m_pseudo_moves.clear();
	return;
}

/// <summary>
/// called when allied king is in check directly after pseudo-move generation
/// Appends en-passant moves to legal if they resolve the check.
/// Note that if the pevious pawn move led to a discovered check this function should not append anything.
/// </summary>
/// <param name="active_king_index"></param>
void Game::append_en_passant_if_resolves_check(int active_king_index, int check_id)
{
	//last move was p2
	if (m_p2_index == -1) return;
	// check is given by p2
	if (check_id != m_board.get_id(m_p2_index)) return;
	// append eps
	for (const GameMoveInt& m : m_pseudo_moves) {
		if (is_en_passant(m)) m_legal_moves.push_back(m);
	}
}

bool Game::move_is_legal(const GameMove& m) const
{
	const GameMoveInt mint = gm_to_gmi(m);
	for (const GameMoveInt& lm : m_legal_moves) {
		if (mint == lm) return true;
	}
	return false;
}

GameDelta Game::legal_to_gd(const GameMove& move)
{
	GameDelta gd(move);

	UniquePiece from_up = m_board.get_up(move.from);
	UniquePiece to_up = m_board.get_up(move.to);

	//is castle?
	if (from_up.p == Piece::KING) {
		int dx = move.to - move.from;
		if (dx == 2) gd.ksc = true;
		else if (dx == -2) gd.qsc = true;
	}

	//is_ep?
	if (is_en_passant(move)) {
		gd.ep = true;
		gd.takes = m_board.get_up(m_p2_index);
	}

	//is_takes
	if (to_up.IsEnemy(m_swap_vars.active->color)) gd.takes = to_up;

	return gd;
}

GameMove Game::string_to_gamemove(const std::string& s) const
{
	//TODO
	return uci_to_gamemove(s);
}

GameMove Game::uci_to_gamemove(const std::string& uci) const 
{
	GameMove m_ret{};
	const int uci_string_size = uci.size();
	if (uci_string_size < 4 || uci_string_size>5) return m_ret;
	if (uci_string_size == 5) {
		const char c = uci[4];
		if (c == 'q' || c == 'r' || c == 'b' || c == 'n') {
			m_ret.promotion = char_to_piece(c);
		}
		else return m_ret;
	}

	Position from{ uci[0] - 'a' , uci[1] - '1' };
	if (from.x < 0 || from.x>=GAME_WIDTH || from.y < 0 || from.y>=GAME_HEIGHT) return m_ret;
	
	Position to{ uci[2] - 'a' , uci[3] - '1' };
	if (to.x < 0 || to.x >= GAME_WIDTH || to.y < 0 || to.y >= GAME_HEIGHT) return m_ret;

	m_ret.from = position_to_bindex(from);
	m_ret.to = position_to_bindex(to);
	return m_ret;
}

GameMove Game::san_to_gamemove(const std::string& san) const
{
	// TODO
	// Do ksc,qsc separate
	// identify move piece
	// lfind first digit -> to pos
	// if move piece is king lookup from pos -> finished
	// check = right of first digit for promo
	// generate all possible moves from to pos for move piece.
	// lookup all indicies -> if only one -> finished
	// not one -> find all contenders and from pos hint
	// filter -> if only one 
	return GameMove();
}

GameMove Game::lan_to_gamemove(const std::string& lan) const
{
	GameMove m_ret{};
	const int lan_size = lan.size();
	//not sure about 10 as max size (pawn takes into promo check should be longest)
	if (lan_size < 4 || lan_size>10) return GameMove();
	
	int lan_index = std::isupper(lan[0]) ? 1 : 0 ;
	
	
	Position from{ lan[lan_index++] - 'a' , lan[lan_index++] - '1' };
	if (from.x < 0 || from.x >= GAME_WIDTH || from.y < 0 || from.y >= GAME_HEIGHT) return m_ret;
	m_ret.from = position_to_bindex(from);
	if (lan[lan_index]=='x' || lan[lan_index] == '-') lan_index++;

	//from here on we need to check string size
	if (lan_index >= lan_size - 1) return GameMove();
	Position to{ lan[lan_index++] - 'a' , lan[lan_index++] - '1' };
	if (to.x < 0 || to.x >= GAME_WIDTH || to.y < 0 || to.y >= GAME_HEIGHT) return m_ret;
	m_ret.to = position_to_bindex(to);

	if (lan_index == lan_size) return m_ret; //without promo
	if (lan_index >= lan_size - 1) return GameMove();

	if (lan[lan_index] != '=') return GameMove();
	const char c = lan[lan_index+1];
	if (c == 'Q' || c == 'R' || c == 'B' || c == 'N') {
		m_ret.promotion = char_to_piece(c);
		return m_ret; //with promo
	}
	return GameMove();
}

std::string Game::legal_to_string(const GameMoveInt& move) const
{
	//TODO
	return legal_to_uci(move);
}

std::string Game::legal_to_uci(const GameMoveInt& move) const
{
	/*const GameDelta& gd = m_gamedelta_list.back();
	std::string uci(gd.m.IsPromo() ? 5 : 4,' ');

	const Position from = IndexToPosition(gd.m.from);
	uci[0] = 'a' + from.x;
	uci[1] = '1' + from.y;
	const Position to = IndexToPosition(gd.m.to);
	uci[2] = 'a' + to.x;
	uci[3] = '1' + to.y;
	if (gd.m.IsPromo()) uci[4] = PIECE_TO_CHAR.at(gd.m.promo);
	return uci;*/
	std::string uci(4,' ');

	const Position from = bindex_to_position(move.get_from());
	uci[0] = 'a' + from.x;
	uci[1] = '1' + from.y;
	const Position to = bindex_to_position(move.get_to());
	uci[2] = 'a' + to.x;
	uci[3] = '1' + to.y;
	if (move.is_promotion()) uci += piece_to_char(move.get_promotion());
	return uci;
	
}

std::string Game::legal_to_san(const GameMoveInt& move) const
{
	//const GameDelta& gd = m_gamedelta_list.back();
	//std::stringstream ss;

	//// castling special cases
	//if (gd.m.IsKSCastle()) {
	//	ss << "O-O";
	//	if (gd.check) ss << "+";
	//	return ss.str();
	//}
	//if (gd.m.IsQSCastle()) {
	//	ss << "O-O-O";
	//	if (gd.check) ss << "+";
	//	return ss.str();
	//}

	//// From Piece?
	//UniquePiece from_up = m_board.index2up[gd.m.to];
	//if (from_up.p != PIECE::PAWN) ss << PIECE_TO_CHAR.at(from_up.p);
	//
	//// From rank, file?
	//const Position from = IndexToPosition(gd.m.from);
	//for (int index = GAME_WIDTH * from.y; index < GAME_WIDTH * (from.y + 1); index++) {
	//	UniquePiece up = m_board.index2up[index];
	//	if (up.p == from_up.p && up.IsAlly(from_up.GetColor())) {
	//		ss << from.x - 'a';
	//		break;
	//	}
	//}
	//for (int index = from.x; index < GAME_NUMBER_OF_TILES; index += GAME_WIDTH) {
	//	UniquePiece up = m_board.index2up[index];
	//	if (up.p == from_up.p && up.IsAlly(from_up.GetColor())) {
	//		ss << from.y - '1';
	//		break;
	//	}
	//}
	//
	//if (gd.m.IsTakes()) ss << 'x';

	//const Position to = IndexToPosition(gd.m.to);
	//ss << to.x - 'a' << to.y - '1';
	//
	//if(gd.m.IsPromo()) ss << '=' << PIECE_TO_CHAR.at(gd.m.promo);

	//if (gd.check) ss << '+';

	//return ss.str();
	// TODO
	return std::string("TODO");
}

std::string Game::legal_to_lan(const GameMoveInt& move) const
{
	//const GameDelta& gd = m_gamedelta_list.back();
	//std::stringstream ss;

	//// castling special cases
	//if (gd.m.IsKSCastle()) {
	//	ss << "O-O";
	//	if (gd.check) ss << "+";
	//	return ss.str();
	//}
	//if (gd.m.IsQSCastle()) {
	//	ss << "O-O-O";
	//	if (gd.check) ss << "+";
	//	return ss.str();
	//}

	//// From Piece?
	//UniquePiece from_up = m_board.index2up[gd.m.to];
	//if (from_up.p != PIECE::PAWN) ss << PIECE_TO_CHAR.at(from_up.p);

	//// From rank, file?
	//const Position from = IndexToPosition(gd.m.from);
	//ss << from.x - 'a' << from.y - '1';
	//
	//if (gd.m.IsTakes()) ss << 'x';

	//const Position to = IndexToPosition(gd.m.to);
	//ss << to.x - 'a' << to.y - '1';

	//if (gd.m.IsPromo()) ss << '=' << PIECE_TO_CHAR.at(gd.m.promo);

	//if (gd.check) ss << '+';

	//return ss.str();
	return std::string("TODO");
}

std::string Game::gd_to_string(const GameDelta& gd) const
{
	switch (m_string_fmt)
	{
	case GameMoveStrFmt::DEFAULT: return std::string{};
	case GameMoveStrFmt::UCI: return gd_to_uci(gd);
	case GameMoveStrFmt::SAN: return gd_to_san(gd);
	case GameMoveStrFmt::LAN: return gd_to_lan(gd);
	}
}

std::string Game::gd_to_uci(const GameDelta& gd) const
{
	std::string uci(4, ' ');

	const Position from = bindex_to_position(gd.move.from);
	uci[0] = 'a' + from.x;
	uci[1] = '1' + from.y;
	const Position to = bindex_to_position(gd.move.to);
	uci[2] = 'a' + to.x;
	uci[3] = '1' + to.y;
	if (gd.IsPromotion()) uci += piece_to_char(gd.move.promotion);
	return uci;
}

std::string Game::gd_to_san(const GameDelta& gd) const
{
	return std::string();
}

std::string Game::gd_to_lan(const GameDelta& gd) const
{
	return std::string();
}

void Game::update_p2_index(const GameDelta& gd)
{

	if (m_board.get_piece_from_bindex(gd.move.to) == Piece::PAWN && std::abs(gd.move.to - gd.move.from) == 16) {
		m_p2_index = gd.move.to;
		return;
	}
	m_p2_index = -1;
	return;

}

void Game::update_castles(const GameDelta& gd)
{
	Piece moved_piece = m_board.get_piece_from_bindex(gd.move.from);
	if (m_swap_vars.active->castles.kscastle) {
		int ks_rook_index = m_swap_vars.active->color.IsWhite() ? GAME_WIDTH - 1 : GAME_BOARD_SIZE - 1;
		if (moved_piece == Piece::KING || gd.move.from == ks_rook_index) {
			m_swap_vars.active->castles.kscastle = false;
		}
	}
	if (m_swap_vars.active->castles.qscastle) {
		int qs_rook_index = m_swap_vars.active->color.IsWhite() ? 0 : GAME_BOARD_SIZE - GAME_WIDTH;
		if (moved_piece == Piece::KING || gd.move.from == qs_rook_index) {
			m_swap_vars.active->castles.qscastle = false;
		}
	}

	if (m_swap_vars.passive->castles.CanCastle()) {
		if (gd.IsTakes() && gd.takes.p == Piece::ROOK) {
			const int rook_bindex = gd.move.to;
			const int ks_rook_index = m_swap_vars.active->color.IsBlack() ? GAME_WIDTH - 1 : GAME_BOARD_SIZE - 1;
			const int qs_rook_index = m_swap_vars.active->color.IsBlack() ? 0 : GAME_BOARD_SIZE - GAME_WIDTH;
			if (rook_bindex == ks_rook_index) m_swap_vars.passive->castles.kscastle = false;
			if (rook_bindex == qs_rook_index) m_swap_vars.passive->castles.qscastle = false;
		}
	}
	return;
}

void Game::update_game_has_ended(bool is_check)
{
	if (m_legal_moves.empty()) {
		m_game_has_ended = true;
		if (is_check) {
			m_ending_gamestate = m_swap_vars.active->color.IsWhite() ? GameEndState::BLACK_WIN_CM : GameEndState::WHITE_WIN_CM;
		}
		else {
			m_ending_gamestate = GameEndState::END_DRAW_STALEMATE;
		}
		return;
	}
	// TODO Threefold logic here

	if (m_half_turn_number > M_MAX_HALF_TURNS) {
		m_game_has_ended = true;
		m_ending_gamestate = GameEndState::END_DRAW_MAX_HALF_TURNS;
		return;
	}
}

void Game::undo_update_p2_index(int p2_last)
{
	m_p2_index = p2_last;
	return;
}

void Game::undo_update_castles(PlayerCastles white_last, PlayerCastles black_last)
{
	m_swap_vars.white.castles = white_last;
	m_swap_vars.black.castles = black_last;
}

//--------------------------------DEBUG UTIL---------------------------------------//

GameState Game::perft_move(const GameMoveInt& gmove)
{
	GameDelta gd = legal_to_gd(gmi_to_gm(gmove));
	gd.white_castle = m_swap_vars.white.castles;
	gd.black_castle = m_swap_vars.black.castles;
	gd.half_turns = m_half_turn_number;
	gd.p2_index = m_p2_index;
	//execute move on board
	update_castles(gd);
	m_board.apply_gamedelta(gd);
	update_p2_index(gd);

	m_swap_vars.Swap();

	gd.check = get_is_check();
	m_gamedelta_list.push_back(gd);

	find_pinned_pieces();
	find_legal_moves();
	if (m_legal_moves.empty()) {
		m_game_has_ended = true;
		if (gd.check) {
			m_ending_gamestate = m_swap_vars.active->color.IsWhite() ? GameEndState::BLACK_WIN_CM : GameEndState::WHITE_WIN_CM;
		}
		else {
			m_ending_gamestate = GameEndState::END_DRAW_STALEMATE;
		}
		return GameState::GAME_HAS_ENDED;
	}
	return GameState::VALID_MOVE;
}

void Game::perft_undo()
{
	if (m_gamedelta_list.empty()) return;
	
	const GameDelta gd_last = m_gamedelta_list.back();
	m_gamedelta_list.pop_back();

	if (m_game_has_ended) m_game_has_ended = false;

	m_swap_vars.Swap();

	undo_update_castles(gd_last.white_castle, gd_last.black_castle);
	undo_update_p2_index(gd_last.p2_index);
	m_board.undo_gamedelta(gd_last);

	return;
}

char Game::bindex_to_pinned_dir_char_DEBUG(int bindex)
{
	UniquePiece up = m_board.get_up(bindex);
	if (up.IsEmpty()) return '.';
	if (up.IsEnemy(m_swap_vars.active->color)) return '.';
	if (up.p == Piece::KING) return 'K';
	int pinned_offset = m_swap_vars.active->color.IsWhite() ? 1 : 17;
	return char(int(m_pinned_direction[up.id - pinned_offset]) + '0');
}