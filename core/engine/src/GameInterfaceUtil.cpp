#include "GameInterfaceUtil.h"

GameMove::GameMove() :
	from(0),to(0),promotion(Piece::EMPTY)
{
}

GameMove::GameMove(int from_, int to_) :
	from(from_), to(to_), promotion(Piece::EMPTY)
{
}

GameMove::GameMove(Position from_, Position to_) :
	from(position_to_bindex(from_)), to(position_to_bindex(to_)), promotion(Piece::EMPTY)
{
}

GameMove::GameMove(int from_, int to_, Piece promotion_) :
	from(from_), to(to_), promotion(promotion_)
{
}

GameMove::GameMove(Position from_, Position to_, Piece promotion_) :
	from(position_to_bindex(from_)), to(position_to_bindex(to_)), promotion(promotion_)
{
}

bool operator==(const TileP& lhs, const TileP& rhs)
{
	if (lhs.position.x != rhs.position.x) return false;
	if (lhs.position.y != rhs.position.y) return false;
	if (lhs.piece != rhs.piece) return false;
	if (lhs.color.IsWhite() != rhs.color.IsWhite()) return false;
	return true;
}

bool operator==(const GameMove& lhs, const GameMove& rhs)
{
	if (lhs.from != rhs.from) return false;
	if (lhs.to != rhs.to) return false;
	if (lhs.promotion != rhs.promotion) return false;
	return true;
}

int position_to_bindex(Position pos)
{
	return pos.y * GAME_WIDTH + pos.x;
}

Position bindex_to_position(int bindex)
{
	return {bindex % GAME_WIDTH , bindex / GAME_WIDTH };
}
