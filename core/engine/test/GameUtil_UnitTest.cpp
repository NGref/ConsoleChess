#include "GameUtils.h"

#include "gtest/gtest.h"



TEST(GameUtil, DirectionFunctionality) {
	Direction dir = Direction::N;
	++dir;
	EXPECT_EQ(dir, Direction::E);
	dir = Direction::NW;
	++dir;
	EXPECT_EQ(dir, Direction::NNE);
	EXPECT_FALSE(is_hvd(dir));
	EXPECT_TRUE(is_hvd(Direction::N));
	EXPECT_TRUE(is_hvd(Direction::SW));
	EXPECT_EQ(get_opposite_direction(Direction::N), Direction::S);
	EXPECT_EQ(get_opposite_direction(Direction::SW), Direction::NE);
	EXPECT_EQ(get_opposite_direction(Direction::SSW), Direction::NNE);
	EXPECT_EQ(get_opposite_direction(Direction::ENE), Direction::WSW);
}

TEST(GameUtil, BoardBoundsCheck) {
	// number of steps of piece at a1 in each hvd direction
	EXPECT_EQ(GetOOBSteps(0, Direction::N), 7);
	EXPECT_EQ(GetOOBSteps(0, Direction::NE), 7);
	EXPECT_EQ(GetOOBSteps(0, Direction::E), 7);
	EXPECT_EQ(GetOOBSteps(0, Direction::SE), 0);
	EXPECT_EQ(GetOOBSteps(0, Direction::S), 0);
	EXPECT_EQ(GetOOBSteps(0, Direction::SW), 0);
	EXPECT_EQ(GetOOBSteps(0, Direction::W), 0);
	EXPECT_EQ(GetOOBSteps(0, Direction::NW), 0);
	// knight
	EXPECT_EQ(GetOOBSteps(0, Direction::NNE), 1);
	EXPECT_EQ(GetOOBSteps(0, Direction::ENE), 1);
	for (Direction dir = Direction::ESE; dir <= Direction::NNW; ++dir) {
		EXPECT_EQ(GetOOBSteps(0, dir), 0);
	}

	//mirrored
	// number of steps of piece at a1 in each hvd direction
	EXPECT_EQ(GetOOBSteps(63, Direction::S), 7);
	EXPECT_EQ(GetOOBSteps(63, Direction::SW), 7);
	EXPECT_EQ(GetOOBSteps(63, Direction::W), 7);
	EXPECT_EQ(GetOOBSteps(63, Direction::N), 0);
	EXPECT_EQ(GetOOBSteps(63, Direction::NE), 0);
	EXPECT_EQ(GetOOBSteps(63, Direction::E), 0);
	EXPECT_EQ(GetOOBSteps(63, Direction::SE), 0);
	EXPECT_EQ(GetOOBSteps(63, Direction::NW), 0);
	// knight
	EXPECT_EQ(GetOOBSteps(63, Direction::SSW), 1);
	EXPECT_EQ(GetOOBSteps(63, Direction::WSW), 1);
	
	EXPECT_EQ(GetOOBSteps(63, Direction::SSE), 0);
	EXPECT_EQ(GetOOBSteps(63, Direction::WNW), 0);
}

TEST(GameUtil, GameMoveConversion) {
	const GameMove move{ 0,1 };
	const GameMoveInt move_int{ 0,1 };
	EXPECT_EQ(gm_to_gmi(move), move_int);
	EXPECT_EQ(gmi_to_gm(move_int), move);
	//with promotion
	const GameMove promo{ 0,1 ,Piece::QUEEN};
	const GameMoveInt promo_int{ 0,1 ,Piece::QUEEN};
	EXPECT_EQ(gm_to_gmi(promo),promo_int);
	EXPECT_EQ(gmi_to_gm(promo_int),promo);
}

TEST(GameUtil, ChessBoardDefaultConstructor) {
	ChessBoard board;
	EXPECT_EQ(board.get_bindex(0), 4); //white king starts at e1
	EXPECT_EQ(board.get_bindex(GAME_BLACK_ID_OFFSET), 60); //black king starts at e8
	EXPECT_EQ(board.get_piece_from_bindex(0), Piece::ROOK); //rook at a1
	EXPECT_EQ(board.get_piece_from_bindex(GAME_BOARD_SIZE - 1), Piece::ROOK); //rook at h8
}

TEST(GameUtil, ChessBoardNewGame) {
	const ChessBoard board;
	ChessBoard board2;
	board2.new_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
	EXPECT_EQ(board, board2);
}


TEST(GameUtil, IndexIDPieceLookup) {
	ChessBoard board;
	for (int id = 0; id < GAME_MAX_ID; id++) {
		const Piece p = board.get_piece_from_id(id);
		const int bindex = board.get_bindex(id);
		if (p != Piece::EMPTY) EXPECT_EQ(p, board.get_piece_from_bindex(bindex));
	}
	for (int bindex = 0; bindex < GAME_BOARD_SIZE; bindex++) {
		const UniquePiece up = board.get_up(bindex);
		if (!up.IsEmpty()) {
			EXPECT_EQ(up.p, board.get_piece_from_bindex(bindex));
			EXPECT_EQ(up.id, board.get_id(bindex));
		}
	}
}

TEST(GameUtil, CoverageGetters) {
	ChessBoard board;
	//white rook at a1
	EXPECT_EQ(board.get_cover_count(0),0);
	EXPECT_FALSE(board.is_covered(0));
	//white knight at b1 (is covered by rook at a1)
	EXPECT_EQ(board.get_cover_count(1), 1); 
	EXPECT_TRUE(board.is_covered(1));
	EXPECT_EQ(board.get_cover_count_color(1,0), 1);
	EXPECT_EQ(board.get_cover_count_color(1, GAME_BLACK_ID_OFFSET), 0);
	EXPECT_EQ(board.get_bindex(board.get_first_cover_id(1)),0);
	//black pawn at d7
	const int bp_bindex = position_to_bindex({ 3,6 });
	EXPECT_EQ(board.get_cover_count(bp_bindex), 4);
	EXPECT_TRUE(board.is_covered(bp_bindex));
	EXPECT_EQ(board.get_cover_count_color(bp_bindex, 0), 0);
	EXPECT_EQ(board.get_cover_count_color(bp_bindex, GAME_BLACK_ID_OFFSET), 4);
}

TEST(GameUtil, ChessBoardApplyUndoInvariance) {
	const ChessBoard board;
	ChessBoard board2;
	GameDelta gd(GameMove{ 8,16 });
	board2.apply_gamedelta(gd);
	board2.undo_gamedelta(gd);
	EXPECT_EQ(board, board2);
}