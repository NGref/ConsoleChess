#pragma once

// changeing this might break alot
#define GAME_WIDTH 8
#define GAME_HEIGHT 8
#define GAME_BOARD_SIZE GAME_WIDTH*GAME_HEIGHT

struct Position {
	int x; // [0, GAMGE_WIDTH - 1]
	int y; // [0, GAMGE_HEIGHT - 1]
};

struct ChessColor {
	bool white;
	bool IsWhite() const { return white; }
	bool IsBlack() const { return !white; }
};

enum class GameState {
	INVALID_MOVE = 0,
	VALID_MOVE = 1,
	GAME_HAS_ENDED = 2
};

enum class GameEndState {
	WHITE_WIN_CM = 0,
	WHITE_WIN_FF = 1,
	WHITE_WIN_TIME = 2,
	WHITE_WIN_REP_INV_MOVE = 3,

	BLACK_WIN_CM = 10,
	BLACK_WIN_FF = 11,
	BLACK_WIN_TIME = 12,
	BLACK_WIN_REP_INV_MOVE = 13,

	END_DRAW_STALEMATE = 20,
	END_DRAW_OFFER = 21,
	END_DRAW_3FOLD = 22,
	END_DRAW_MAX_TURNS = 23,
	END_DRAW_MAX_HALF_TURNS = 24,
};

enum class GameMoveStrFmt {
	DEFAULT,
	UCI,
	SAN,
	LAN
};

enum class Piece {
	EMPTY = 0,
	KING,
	QUEEN,
	BISHOP,
	KNIGHT,
	ROOK,
	PAWN
};

struct TileP {
	Position position;
	Piece piece;
	ChessColor color;
};

bool operator==(const TileP& lhs, const TileP& rhs);

struct TileI {
	int index;
	Piece piece;
	ChessColor color;
};



class GameMove {
public:
	int from;
	int to;
	Piece promotion;
	GameMove();
	GameMove(int from_, int to_);
	GameMove(Position from_, Position to_);
	GameMove(int from_, int to_, Piece promotion_);
	GameMove(Position from_, Position to_, Piece promotion_);
	friend bool operator==(const GameMove& lhs, const GameMove& rhs);
};

int position_to_bindex(Position pos);
Position bindex_to_position(int bindex);