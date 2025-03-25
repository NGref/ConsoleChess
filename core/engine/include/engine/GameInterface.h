#pragma once

#include "GameInterfaceUtil.h"

#include <vector>
#include <string>
#include <memory>


/// <summary>
/// Interface of the game class.
/// Defines core functionality of game class and adds additional type support
/// 
/// New games can be created via FEN-strings using the constructor or new_game method.
/// The game can be progressed by inputing valid chess-moves to the move method (either in string format SAN/LAN/UCI or GameMove struct)
/// The move method returns the current Game-state after processing the input move.
/// The game has ended (check-mate/draw...) if the move method returns the respective state.
/// The specific type of ending (check-mate/draw...) can be obtained via get_ending_game_state.
/// 
/// Other getter methods return information about the board state or legal/past moves
/// 
/// </summary>
class IGame
{
public:
	virtual ~IGame() {}

	// to be defined in game class

	// used for deepcopy
	virtual std::unique_ptr<IGame> clone() const = 0;
	
	// call after constructor/new_game
	virtual bool get_init_ok() const = 0;

	virtual ChessColor get_active_color() const = 0;
	virtual int get_turn_number() const = 0;
	virtual GameMoveStrFmt get_move_str_fmt() const = 0;
	
	// returns all legal moves for current active player
	virtual std::vector<GameMove> get_possible_moves() const = 0;
	virtual std::vector<std::string> get_possible_moves_str() const = 0;
	// returns all legal moves for piece at from_id
	virtual std::vector<GameMove> get_possible_moves(int from_ind) const = 0;
	// returns all legal to_ind for piece at from_id
	virtual std::vector<int> get_possible_moves_ind(int from_ind) const = 0;
	// returns all legal moves for piece at from_str
	virtual std::vector<std::string> get_possible_moves_str(const std::string& from_str) const = 0;

	// returns each occupied tile on the chessboard
	virtual std::vector<TileI> get_all_tiles_ind() const = 0;
	// returns all tiles that changed after last move including empty ones
	virtual std::vector<TileI> get_new_tiles_ind() const = 0;
	// can be called before undo.
	virtual std::vector<TileI> get_reverse_new_tiles_ind() const  = 0;

	virtual GameMove get_last_move() const = 0;
	virtual std::string get_last_move_str(GameMoveStrFmt fmt = GameMoveStrFmt::DEFAULT) const = 0;
	// order: last move at back
	virtual std::vector<GameMove> get_all_moves() const = 0;
	virtual std::vector<std::string> get_all_moves_str(GameMoveStrFmt fmt = GameMoveStrFmt::DEFAULT) const = 0;
	

	virtual bool get_is_check() const = 0;
	virtual bool get_game_has_ended() const = 0;
	virtual GameEndState get_ending_game_state() const = 0;

	// try a move; returns {valid, invalid, game_ended}
	virtual GameState move(const GameMove& move) = 0;
	virtual GameState move(const std::string& move, GameMoveStrFmt fmt = GameMoveStrFmt::DEFAULT) = 0;
	
	virtual void undo() = 0;
	// old game will be overwritten. Make sure to save before.
	virtual void new_game(const std::string& fen = "") = 0;
	
	virtual void set_move_str_fmt(GameMoveStrFmt fmt) = 0;

	// can be used for forfeit and time implementations
	virtual void set_ending_game_state(GameEndState ges) = 0;
	
	// Function overloading to support position in-/outputs
	std::vector<GameMove> get_possible_moves(Position from_pos) const;
	std::vector<Position> get_possible_moves_pos(Position from_pos) const;
	std::vector<TileP> get_all_tiles_pos() const;
	std::vector<TileP> get_new_tiles_pos() const;
	virtual std::vector<TileP> get_reverse_new_tiles_pos() const;
};