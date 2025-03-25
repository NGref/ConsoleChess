#pragma once
#include "GameUtils.h"
#include "GameInterface.h"

#include <string>


/// <summary>
/// Implementation of a Chess game with the methods defined in IGame.
/// Additional functionalities are added for testing.
/// 
/// Implementation details:
/// This class stores the current board state, a list of game-deltas, and a list of legal moves.
/// If a legal move is used as input int the move method, the board is changed and a new game-delta is pushed.
/// A undo call pops a game-delta and reverses to board to the previous state.
/// After both cases the legal moves for the next player are reevaluated.
/// 
/// Why precalculate legal moves?
/// Legal moves are moves that are possible by the moving rules of the piece (called pseudo leagal) and do not result in your own king being checked.
/// Looking at the worst case, the user inputs only legal moves and does not make use of the get_legal_moves methods.
/// Then in a naive implementation the computational cost of precalculating would be #pseudo_legal (usually <40) times cost of checking legal.
/// However in a more refined implementation, one can make use of the fact that past moves deemed legal usually are still legal in the future.
/// This is done via a coverage map with O(#pieces x #board_size) space complexity. Checking for legality is O(1), Updating is comparable to a single naive check.
/// Therefore except for O(#pieces x #board_size) space there are no real drawbacks for precalculating.
/// </summary>
class Game : public IGame
{
public:
	// default constructor using standard fen
	Game(const std::string& fen = "", GameMoveStrFmt fmt = GameMoveStrFmt::UCI, uint8_t MAX_HALF_TURNS = 100);
	Game(const Game& other);
	std::unique_ptr<IGame> clone() const override;
	~Game() override;
	
	bool get_init_ok() const override;
	ChessColor get_active_color() const override;
	int get_turn_number() const override;
	std::vector<GameMove> get_possible_moves() const override;
	std::vector<std::string> get_possible_moves_str() const override;
	std::vector<GameMove> get_possible_moves(int from_ind) const override;
	std::vector<int> get_possible_moves_ind(int from_ind) const override;
	std::vector<std::string> get_possible_moves_str(const std::string& from_str) const override;
	std::vector<TileI> get_all_tiles_ind() const override;
	std::vector<TileI> get_new_tiles_ind() const override;
	std::vector<TileI> get_reverse_new_tiles_ind() const override;
	GameMove get_last_move() const override;
	std::string get_last_move_str(GameMoveStrFmt fmt = GameMoveStrFmt::DEFAULT) const override;
	std::vector<GameMove> get_all_moves() const override;
	std::vector<std::string> get_all_moves_str(GameMoveStrFmt fmt = GameMoveStrFmt::DEFAULT) const override;
	bool get_is_check() const override;
	bool get_game_has_ended() const override;
	GameEndState get_ending_game_state() const override;
	GameMoveStrFmt get_move_str_fmt() const override;

	GameState move(const std::string& move, GameMoveStrFmt fmt = GameMoveStrFmt::DEFAULT) override;
	GameState move(const GameMove& move) override;
	void undo() override;
	void new_game(const std::string& fen = "") override;
	void set_ending_game_state(GameEndState ges) override;
	void set_move_str_fmt(GameMoveStrFmt fmt) override;

public:
	// use for testing
	uint64_t perft(int);
	void perft_divide(int);
	void save_board_DEBUG();
	friend bool operator==(const Game& lhs, const Game& rhs);
private:
	bool is_en_passant(const GameMove& m);
	bool is_en_passant(const GameMoveInt& m);

	bool init_fen(const std::string& fen);
	bool init_fen_castles(const std::string& fen_castles_section);
	bool init_fen_p2_index(const std::string& fen_ep_section);

	void find_legal_moves();
	void find_pseudo_moves();
	void find_pinned_pieces();
	void find_pinned_direction(const Direction dir, const int king_bindex, const Piece dirp);
	void clear_pinned();

	void piece_moves(int id);
	void king_moves(int id);
	void queen_moves(int id);
	void bishop_moves(int id);
	void knight_moves(int id);
	void rook_moves(int id);
	void pawn_moves(int id);
	void move_and_append(const Direction dir, const int from_bindex);
	void ksc_append(int king_bindex);
	void qsc_append(int king_bindex);

	bool en_passant_is_self_check(int from_x, int from_y, int ep_x);
	
	void filter_pinned_moves();
	void filter_block_moves();
	void find_block_indices(int king_bindex, int check_bindex);
	void append_and_clear();
	void append_en_passant_if_resolves_check(int active_king_index, int check_id);

	bool move_is_legal(const GameMove& m) const;
	GameDelta legal_to_gd(const GameMove& move);

	GameMove string_to_gamemove(const std::string& s) const;
	GameMove uci_to_gamemove(const std::string& uci) const;
	GameMove san_to_gamemove(const std::string& san) const;
	GameMove lan_to_gamemove(const std::string& lan) const;

	std::string legal_to_string(const GameMoveInt& move) const;
	std::string legal_to_uci(const GameMoveInt& move) const;
	std::string legal_to_san(const GameMoveInt& move) const;
	std::string legal_to_lan(const GameMoveInt& move) const;

	std::string gd_to_string(const GameDelta& gd) const;
	std::string gd_to_uci(const GameDelta& gd) const;
	std::string gd_to_san(const GameDelta& gd) const;
	std::string gd_to_lan(const GameDelta& gd) const;

	void update_p2_index(const GameDelta& gd);
	void update_castles(const GameDelta& gd);
	void update_game_has_ended(bool is_check);
	
	void undo_update_p2_index(int p2_last);
	void undo_update_castles(PlayerCastles white_last, PlayerCastles black_last);

private:
	GameState perft_move(const GameMoveInt& m);
	void perft_undo();
	char bindex_to_pinned_dir_char_DEBUG(int bindex);
private:
	const std::string M_DEFAULT_FEN;
	ChessBoard m_board;
	SwapVars m_swap_vars;
	std::vector<GameDelta> m_gamedelta_list;
	std::vector<GameMoveInt> m_legal_moves;
	std::vector<GameMoveInt> m_pseudo_moves;
	std::vector<int> m_block_check_indices;
	
	std::array<Direction, GAME_MAX_ID> m_pinned_direction;
	GameEndState m_ending_gamestate;
	GameMoveStrFmt m_string_fmt;
	int  m_p2_index;
	bool m_fen_valid;
	bool m_game_has_ended;
	uint8_t m_turn_number;
	const uint8_t M_MAX_HALF_TURNS;
	uint8_t m_half_turn_number;
};