#include "GameUtils.h"
#include "GameUtils.h"

#define GAME_DELTA_DIR_N GAME_WIDTH
#define GAME_DELTA_DIR_E 1
#define GAME_DELTA_DIR_S -GAME_WIDTH
#define GAME_DELTA_DIR_W -1
#define GAME_DELTA_DIR_NE GAME_WIDTH + 1 //9
#define GAME_DELTA_DIR_SE -GAME_WIDTH + 1 //-7
#define GAME_DELTA_DIR_SW -GAME_WIDTH - 1 //-9
#define GAME_DELTA_DIR_NW GAME_WIDTH - 1 //7
#define GAME_DELTA_DIR_NNE 2*GAME_WIDTH + 1 //17
#define GAME_DELTA_DIR_ENE GAME_WIDTH + 2 //10
#define GAME_DELTA_DIR_ESE -GAME_WIDTH + 2 //-6
#define GAME_DELTA_DIR_SSE -2 * GAME_WIDTH + 1 //-15
#define GAME_DELTA_DIR_SSW -2 * GAME_WIDTH - 1 //-17
#define GAME_DELTA_DIR_WSW -GAME_WIDTH - 2 //-10
#define GAME_DELTA_DIR_WNW GAME_WIDTH - 2 //6
#define GAME_DELTA_DIR_NNW 2 * GAME_WIDTH - 1 //15





int get_bindex_delta(const Direction dir)
{
	switch (dir) {
	case Direction::NONE: return 0;
	case Direction::N   : return GAME_DELTA_DIR_N;
	case Direction::E   : return GAME_DELTA_DIR_E;
	case Direction::S   : return GAME_DELTA_DIR_S;
	case Direction::W   : return GAME_DELTA_DIR_W;
	case Direction::NE  : return GAME_DELTA_DIR_NE;
	case Direction::SE  : return GAME_DELTA_DIR_SE;
	case Direction::SW  : return GAME_DELTA_DIR_SW;
	case Direction::NW  : return GAME_DELTA_DIR_NW;
	case Direction::NNE : return GAME_DELTA_DIR_NNE;
	case Direction::ENE : return GAME_DELTA_DIR_ENE;
	case Direction::ESE : return GAME_DELTA_DIR_ESE;
	case Direction::SSE : return GAME_DELTA_DIR_SSE;
	case Direction::SSW : return GAME_DELTA_DIR_SSW;
	case Direction::WSW : return GAME_DELTA_DIR_WSW;
	case Direction::WNW : return GAME_DELTA_DIR_WNW;
	case Direction::NNW : return GAME_DELTA_DIR_NNW;
	}
}


Direction get_direction(const int delta_bindex)
{
	switch (delta_bindex) {
	case GAME_DELTA_DIR_N	: return Direction::N;
	case GAME_DELTA_DIR_E	: return Direction::E;
	case GAME_DELTA_DIR_S	: return Direction::S;
	case GAME_DELTA_DIR_W	: return Direction::W;
	case GAME_DELTA_DIR_NE	: return Direction::NE;
	case GAME_DELTA_DIR_SE	: return Direction::SE;
	case GAME_DELTA_DIR_SW	: return Direction::SW;
	case GAME_DELTA_DIR_NW	: return Direction::NW;
	case GAME_DELTA_DIR_NNE	: return Direction::NNE;
	case GAME_DELTA_DIR_ENE	: return Direction::ENE;
	case GAME_DELTA_DIR_ESE	: return Direction::ESE;
	case GAME_DELTA_DIR_SSE	: return Direction::SSE;
	case GAME_DELTA_DIR_SSW	: return Direction::SSW;
	case GAME_DELTA_DIR_WSW	: return Direction::WSW;
	case GAME_DELTA_DIR_WNW	: return Direction::WNW;
	case GAME_DELTA_DIR_NNW	: return Direction::NNW;
	default: return Direction::NONE;
	}
}

#undef GAME_DELTA_DIR_N
#undef GAME_DELTA_DIR_E
#undef GAME_DELTA_DIR_S
#undef GAME_DELTA_DIR_W
#undef GAME_DELTA_DIR_NE
#undef GAME_DELTA_DIR_SE
#undef GAME_DELTA_DIR_SW
#undef GAME_DELTA_DIR_NW
#undef GAME_DELTA_DIR_NNE
#undef GAME_DELTA_DIR_ENE
#undef GAME_DELTA_DIR_ESE
#undef GAME_DELTA_DIR_SSE
#undef GAME_DELTA_DIR_SSW
#undef GAME_DELTA_DIR_WSW
#undef GAME_DELTA_DIR_WNW
#undef GAME_DELTA_DIR_NNW

bool is_hvd(const Direction& dir)
{
	return dir>Direction::NONE && dir<Direction::NNE;
}

Direction get_hvd(const int from_bindex, const int to_bindex)
{
	Position from = bindex_to_position(from_bindex);
	Position to = bindex_to_position(to_bindex);

	const int dx = to.x - from.x;
	const int dy = to.y - from.y;

	if (dx == 0) {
		if (dy == 0) return Direction::NONE;
		if (dy > 0) return Direction::N;
		return Direction::S;
	}
	if (dx > 0) {
		if (dy == 0) return Direction::E;
		if (dy == dx) return Direction::NE;
		if (dy == -dx) return Direction::SE;
		return Direction::NONE;
	}
	else {
		if (dy == 0) return Direction::W;
		if (dy == dx) return Direction::SW;
		if (dy == -dx) return Direction::NW;
		return Direction::NONE;
	}
	return Direction::NONE;
}

Direction get_opposite_direction(const Direction dir)
{
	switch (dir) {
	case Direction::NONE: return Direction::NONE;
	case Direction::N: return Direction::S;
	case Direction::E: return Direction::W;
	case Direction::S: return Direction::N;
	case Direction::W: return Direction::E;
	case Direction::NE: return Direction::SW;
	case Direction::SE: return Direction::NW;
	case Direction::SW: return Direction::NE;
	case Direction::NW: return Direction::SE;
	case Direction::NNE: return Direction::SSW;
	case Direction::ENE: return Direction::WSW;
	case Direction::ESE: return Direction::WNW;
	case Direction::SSE: return Direction::NNW;
	case Direction::SSW: return Direction::NNE;
	case Direction::WSW: return Direction::ENE;
	case Direction::WNW: return Direction::ESE;
	case Direction::NNW: return Direction::SSE;
	}
}



struct DirStepCount {
	int8_t hvd_cnt[8] = {};
	bool n_cnt[8] = {};
};

std::array<DirStepCount, GAME_BOARD_SIZE> InitOOB()
{
	std::array<DirStepCount, GAME_BOARD_SIZE> oob_ret;
	for (int8_t x = 0; x < GAME_WIDTH; x++) {
		for (int8_t y = 0; y < GAME_HEIGHT; y++) {
			int8_t index = y*GAME_WIDTH + x;
			oob_ret[index].hvd_cnt[0] = GAME_HEIGHT - 1 - y ;
			oob_ret[index].hvd_cnt[1] = GAME_WIDTH - 1 - x;
			oob_ret[index].hvd_cnt[2] = y;
			oob_ret[index].hvd_cnt[3] = x;
			oob_ret[index].hvd_cnt[4] = std::min(GAME_HEIGHT - 1 - y, GAME_WIDTH - 1 - x);
			oob_ret[index].hvd_cnt[5] = std::min(y, int8_t(GAME_WIDTH - 1 - x));
			oob_ret[index].hvd_cnt[6] = std::min(y, x);
			oob_ret[index].hvd_cnt[7] = std::min(int8_t(GAME_HEIGHT - 1 - y), x);
			oob_ret[index].n_cnt[0] = (GAME_HEIGHT - 1 - y) > 1 && (GAME_WIDTH - 1 - x) > 0;
			oob_ret[index].n_cnt[1] = (GAME_HEIGHT - 1 - y) > 0 && (GAME_WIDTH - 1 - x) > 1;
			oob_ret[index].n_cnt[2] = y > 0 && (GAME_WIDTH - 1 - x) > 1;
			oob_ret[index].n_cnt[3] = y > 1 && (GAME_WIDTH - 1 - x) > 0;
			oob_ret[index].n_cnt[4] = y > 1 &&  x > 0;
			oob_ret[index].n_cnt[5] = y > 0 && x > 1;
			oob_ret[index].n_cnt[6] = (GAME_HEIGHT - 1 - y) > 0 && x > 1;
			oob_ret[index].n_cnt[7] = (GAME_HEIGHT - 1 - y) > 1 && x > 0;
		}
	}
	return oob_ret;
}

const std::array<DirStepCount, GAME_BOARD_SIZE> BOARD_OOB = InitOOB();

int8_t GetCount(const DirStepCount& dsc, const Direction& dir)
{
	const int oob_index = int(dir);
	if (dir == Direction::NONE) return 0;
	else if (oob_index <= int(Direction::NW)) return dsc.hvd_cnt[oob_index - 1];
	else  return int8_t(dsc.n_cnt[oob_index - 9]);
}

int8_t GetOOBSteps(int bindex, Direction direction)
{
	return GetCount(BOARD_OOB[bindex], direction); 
}


bool UniquePiece::IsEmpty() const
{
	return p == Piece::EMPTY;
}

bool UniquePiece::IsWhite() const
{
	{ return IsEmpty() ? false : id < GAME_BLACK_ID_OFFSET; }
}

bool UniquePiece::IsBlack() const
{
	return IsEmpty() ? false : id >= GAME_BLACK_ID_OFFSET;
}

ChessColor UniquePiece::GetColor() const
{
	return ChessColor{ id < GAME_BLACK_ID_OFFSET };
}

bool UniquePiece::IsEnemy(ChessColor cc) const
{
	return IsEmpty() ? false : (id < GAME_MAX_COLOR_ID) != cc.white;
}

bool UniquePiece::IsEnemy(const UniquePiece& other) const
{
	if (IsEmpty() || other.IsEmpty()) return false;
	return (id < GAME_BLACK_ID_OFFSET) != (other.id < GAME_BLACK_ID_OFFSET);
}

bool UniquePiece::IsAlly(ChessColor cc) const
{
	{ return IsEmpty() ? false : (id < GAME_MAX_COLOR_ID) == cc.white; };
}

bool UniquePiece::IsAlly(const UniquePiece& other) const
{
	if (IsEmpty() || other.IsEmpty()) return false;
	return (id < GAME_BLACK_ID_OFFSET) == (other.id < GAME_BLACK_ID_OFFSET);
}

bool operator==(const UniquePiece& lhs, const UniquePiece& rhs)
{
	if (lhs.IsEmpty() && rhs.IsEmpty()) return true;
	if (lhs.id != rhs.id) return false;
	if (lhs.p != rhs.p) return false;
	return true;
}

bool operator!=(const UniquePiece& lhs, const UniquePiece& rhs)
{
	return !(lhs == rhs);
}


GameMoveInt::GameMoveInt(int from, int to) : m_data(static_cast<uint16_t>(to) + (static_cast<uint16_t>(from) << 10))
{
}

GameMoveInt::GameMoveInt(int from, int to, Piece promo)
{
	m_data = to + (from << 10);
	switch (promo) {
	case Piece::EMPTY: break;
	case Piece::KING: break;
	case Piece::QUEEN: 
		m_data += 0b0000001000000000; 
		break;
	case Piece::BISHOP:
		m_data += 0b0000001100000000;
		break;
	case Piece::KNIGHT: 
		m_data += 0b0000001110000000; 
		break;
	case Piece::ROOK:
		m_data += 0b0000001010000000;
		break;
	
	case Piece::PAWN: break;
	}
}

int GameMoveInt::get_from() const
{
	return int(m_data >> 10);
}

int GameMoveInt::get_to() const
{
	return int(m_data & 0b0000000000111111);
}

Piece GameMoveInt::get_promotion() const 
{
	const uint16_t promo = (m_data & 0b0000001110000000)  >> 7;
	switch (promo) {
	case 0: return Piece::EMPTY;
	case 1: return Piece::EMPTY;
	case 2: return Piece::EMPTY;
	case 3: return Piece::EMPTY;
	case 4: return Piece::QUEEN;
	case 5: return Piece::ROOK;
	case 6: return Piece::BISHOP;
	case 7: return Piece::KNIGHT;
	default: return Piece::EMPTY;
	}
}

bool GameMoveInt::is_promotion() const
{
	return bool(m_data & 0b0000001000000000);
}

bool GameMoveInt::is_null() const
{
	return m_data == 0;
}

void GameMoveInt::set_from(int from)
{
	m_data = (uint8_t(from) << 10) + (m_data & 0b0000001111111111);
}

void GameMoveInt::set_to(int to)
{
	m_data = uint8_t(to) + (m_data & 0b1111111111000000);
}

void GameMoveInt::set_promotion(Piece promo)
{
	m_data &= 0b1111110000111111;
	switch (promo) {
	case Piece::EMPTY: break;
	case Piece::KING: break;
	case Piece::QUEEN:
		m_data += 0b0000001000000000;
		break;
	case Piece::BISHOP:
		m_data += 0b0000001100000000;
		break;
	case Piece::KNIGHT:
		m_data += 0b0000001110000000;
		break;
	case Piece::ROOK:
		m_data += 0b0000001010000000;
		break;

	case Piece::PAWN: break;
	}
}

void GameMoveInt::set_null()
{
	m_data = 0;
}


GameMoveInt gm_to_gmi(const GameMove& m)
{
	return GameMoveInt(m.from, m.to, m.promotion);
}

GameMove gmi_to_gm(const GameMoveInt& gmi)
{
	return GameMove(gmi.get_from(), gmi.get_to(), gmi.get_promotion());
}

bool operator==(const GameMove& lhs, const GameMoveInt& rhs)
{
	if (lhs.from != rhs.get_from()) return false;
	if (lhs.to != rhs.get_to()) return false;
	if (lhs.promotion != rhs.get_promotion()) return false;
	return true;
}




bool PlayerCastles::CanCastle()
{
	return kscastle || qscastle;
}

void PlayerCastles::SetFalse()
{
	kscastle = false; 
	qscastle = false;
}

bool operator==(const PlayerCastles& lhs, const PlayerCastles& rhs)
{
	if (lhs.kscastle != rhs.kscastle) return false;
	if (lhs.qscastle != rhs.qscastle) return false;
	return true;
}


GameDelta::GameDelta(const GameMove& move_) : move(move_)
{
}

GameDelta::GameDelta(GameMove move_, UniquePiece takes_,
	PlayerCastles white_castle_, PlayerCastles black_castle_,
	uint8_t half_turns_, int8_t p2_index_,
	bool ksc_, bool qsc_, bool ep_, bool check_) :
	move(move_), takes(takes_),
	white_castle(white_castle_), black_castle(black_castle_),
	half_turns(half_turns_), p2_index(p2_index_),
	ksc(ksc_), qsc(qsc_), ep(ep_), check(check_)
{
}

bool GameDelta::IsTakes() const
{
	return takes.p != Piece::EMPTY;
}

bool GameDelta::IsPromotion() const
{
	return move.promotion != Piece::EMPTY;
}

bool GameDelta::IsCastle() const
{
	return qsc || ksc;
}

bool GameDelta::IsQSCastle() const
{
	return qsc;
}

bool GameDelta::IsKSCastle() const
{
	return ksc;
}

bool GameDelta::IsEnPassant() const
{
	return ep;
}

bool GameDelta::IsCheck() const
{
	return check;
}

bool operator==(const GameDelta& lhs, const GameDelta& rhs)
{
	if (lhs.move != rhs.move) return false;
	if (lhs.takes != rhs.takes) return false;
	if (lhs.white_castle != rhs.white_castle) return false;
	if (lhs.black_castle != rhs.black_castle) return false;
	if (lhs.half_turns != rhs.half_turns) return false;
	if (lhs.ksc != rhs.ksc) return false;
	if (lhs.qsc != rhs.qsc) return false;
	if (lhs.ep != rhs.ep) return false;
	if (lhs.check != rhs.check) return false;
	return true;
}


ChessBoard::ChessBoard() : m_coverage_delta_indices(), m_bindex_to_id{}, m_bindex_to_piece{}, m_id_to_bindex{}, m_id_to_piece{}, m_coverage{}
{
	init_from_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
}

bool ChessBoard::new_board(const std::string& fen_board_section)
{
	clear();
	return init_from_fen(fen_board_section);
}

void ChessBoard::register_up(int bindex, int id, Piece p)
{
	m_bindex_to_id[bindex] = id;
	m_bindex_to_piece[bindex] = p;
	m_id_to_bindex[id] = bindex;
	m_id_to_piece[id] = p;
}

void ChessBoard::apply_gamedelta(const GameDelta& gd)
{
	const UniquePiece up_from = get_up(gd.move.from);
	const UniquePiece up_to = get_up(gd.move.to);

	//normal case
	//set from bindex empty
	m_bindex_to_id[gd.move.from] = 0;
	m_bindex_to_piece[gd.move.from] = Piece::EMPTY;

	//set to bindex to from
	m_bindex_to_id[gd.move.to] = up_from.id;
	m_bindex_to_piece[gd.move.to] = up_from.p;

	// set from id map
	m_id_to_bindex[up_from.id] = gd.move.to;

	//push coverage edit
	m_coverage_delta_indices.push_back(gd.move.from);
	m_coverage_delta_indices.push_back(gd.move.to);
	set_coverage_single(up_from.id, gd.move.from, true); //flag from id

	//check for castle move
	if (gd.IsCastle()) {
		const int rook_bindex = gd.move.from + (gd.IsKSCastle() ? 3 : -4);
		const int king_adjacent_bindex = gd.move.from + (gd.IsKSCastle() ? 1 : -1);
		const int rook_id = m_bindex_to_id[rook_bindex];

		m_bindex_to_id[rook_bindex] = 0;
		m_bindex_to_piece[rook_bindex] = Piece::EMPTY;

		m_bindex_to_id[king_adjacent_bindex] = rook_id;
		m_bindex_to_piece[king_adjacent_bindex] = Piece::ROOK;

		m_id_to_bindex[rook_id] = king_adjacent_bindex;

		m_coverage_delta_indices.push_back(rook_bindex);
		m_coverage_delta_indices.push_back(king_adjacent_bindex);
		set_coverage_single(rook_id, rook_bindex, true); //flag rook id
	}

	//check for promotion
	if (gd.IsPromotion()) {
		m_id_to_piece[up_from.id] = gd.move.promotion;
		m_bindex_to_piece[gd.move.to] = gd.move.promotion;
	}

	// check for takes
	if (gd.IsTakes()) {
		//probably broken together with enpassant
		m_id_to_piece[gd.takes.id] = Piece::EMPTY;
		m_id_to_bindex[gd.takes.id] = 0;

		reset_coverage(gd.takes.id);
	}

	if (gd.IsEnPassant()) {
		m_bindex_to_id[gd.p2_index] = 0;
		m_bindex_to_piece[gd.p2_index] = Piece::EMPTY;
		m_coverage_delta_indices.push_back(gd.p2_index);
	}

	update_coverage();
	m_coverage_delta_indices.clear();
	return;
}

void ChessBoard::undo_gamedelta(const GameDelta& gd)
{
	const UniquePiece up_board_to = get_up(gd.move.to);

	m_bindex_to_id[gd.move.from] = up_board_to.id;
	m_bindex_to_piece[gd.move.from] = up_board_to.p;
	m_id_to_bindex[up_board_to.id] = gd.move.from;

	m_bindex_to_id[gd.move.to] = 0;
	m_bindex_to_piece[gd.move.to] = Piece::EMPTY;

	m_coverage_delta_indices.push_back(gd.move.from);
	m_coverage_delta_indices.push_back(gd.move.to);
	set_coverage_single(up_board_to.id, gd.move.from, true);

	//check for castle move
	if (gd.IsCastle()) {
		const int corner_bindex = gd.move.from + (gd.IsKSCastle() ? 3 : -4);
		const int king_adjacent_bindex = gd.move.from + (gd.IsKSCastle() ? 1 : -1);
		const int rook_id = m_bindex_to_id[king_adjacent_bindex];

		m_bindex_to_id[corner_bindex] = rook_id;
		m_bindex_to_piece[corner_bindex] = Piece::ROOK;

		m_bindex_to_id[king_adjacent_bindex] = 0;
		m_bindex_to_piece[king_adjacent_bindex] = Piece::EMPTY;

		m_id_to_bindex[rook_id] = corner_bindex;

		m_coverage_delta_indices.push_back(corner_bindex);
		m_coverage_delta_indices.push_back(king_adjacent_bindex);
		set_coverage_single(rook_id, corner_bindex, true);
	}

	//undo promotion
	if (gd.IsPromotion()) {
		m_id_to_piece[up_board_to.id] = Piece::PAWN;
		m_bindex_to_piece[gd.move.from] = Piece::PAWN;
	}

	//undo takes
	if (gd.IsTakes() && !gd.IsEnPassant()) {
		m_bindex_to_id[gd.move.to] = gd.takes.id;
		m_bindex_to_piece[gd.move.to] = gd.takes.p;
		m_id_to_piece[gd.takes.id] = gd.takes.p;
		m_id_to_bindex[gd.takes.id] = gd.move.to;

		set_coverage_single(gd.takes.id, gd.move.to, true);
	}

	if (gd.IsEnPassant()) {
		m_bindex_to_id[gd.p2_index] = gd.takes.id;
		m_bindex_to_piece[gd.p2_index] = gd.takes.p;
		m_id_to_piece[gd.takes.id] = gd.takes.p;
		m_id_to_bindex[gd.takes.id] = gd.p2_index;

		set_coverage_single(gd.takes.id, gd.p2_index, true);
		m_coverage_delta_indices.push_back(gd.p2_index);
	}

	update_coverage();
	m_coverage_delta_indices.clear();
	return;
}

int ChessBoard::get_bindex(int id) const
{
	return m_id_to_bindex[id];
}

int ChessBoard::get_id(int bindex) const
{
	return m_bindex_to_id[bindex];
}

Piece ChessBoard::get_piece_from_bindex(int bindex) const
{
	return m_bindex_to_piece[bindex];
}

Piece ChessBoard::get_piece_from_id(int id) const
{
	return m_id_to_piece[id];
}

UniquePiece ChessBoard::get_up(int bindex) const
{
	return UniquePiece(m_bindex_to_id[bindex],m_bindex_to_piece[bindex]);
}

bool ChessBoard::empty(int bindex)
{
	return m_bindex_to_piece[bindex] == Piece::EMPTY;
}

bool ChessBoard::is_covered(int index) const
{
	for (const std::array<bool, GAME_BOARD_SIZE>& idboard : m_coverage) {
		if (idboard[index]) return true;
	}
	return false;
}

bool ChessBoard::is_covered_color(int index, int color_off) const
{
	const int id_end = color_off + GAME_MAX_COLOR_ID;
	for (int id = color_off; id < id_end; id++) {
		if (m_coverage[id][index]) return true;
	}
	return false;
}

int ChessBoard::get_cover_count(int index) const
{
	int cnt = 0;
	for (const std::array<bool, GAME_BOARD_SIZE>& idboard : m_coverage) {
		if (idboard[index]) cnt++;
	}
	return cnt;
}

int ChessBoard::get_cover_count_color(int index, int color_off) const
{
	const int id_end = color_off + GAME_MAX_COLOR_ID;
	int cnt = 0;
	for (int id = color_off; id < id_end; id++) {
		if (m_coverage[id][index]) cnt++;
	}
	return cnt;
}

int ChessBoard::get_first_cover_id(int index) const
{
	for (int id = 0; id < GAME_MAX_ID; id++) {
		if (m_coverage[id][index]) return id;
	}
	return -1;
}

int ChessBoard::get_first_cover_id_color(int index, int color_off) const
{
	const int id_end = color_off + GAME_MAX_COLOR_ID;
	for (int id = color_off; id < id_end; id++) {
		if (m_coverage[id][index]) return id;
	}
	return -1;
}

/// <summary>
/// registers pieces on the board according to board section of a fen string.
/// checks if the following criteria are met:
///		- all pieces in fen can be registered (max 16 per color)
///		- there is only one king per team
/// </summary>
/// <param name="fen_board_section"></param>
/// <returns>
/// true if board section was valid else false
/// </returns>
bool ChessBoard::init_from_fen(const std::string& fen_board_section)
{
	int parse_section = 1;
	int x = 0;
	int y = GAME_HEIGHT - 1;
	int digit_stack = 0;
	int white_id = 1;
	int black_id = GAME_MAX_COLOR_ID + 1;
	bool last_was_digit = false;
	bool received_rank = false;
	int white_king_count = 0;
	int black_king_count = 0;
	for (const char c : fen_board_section) {
		if (isdigit(c)) {
			if (last_was_digit || c == '9') return false;
			last_was_digit = true;
			x += int(c - '0');
			if (x > GAME_WIDTH) return false;
		}
		else if (c == '/') {
			x = 0;
			y -= 1;
			last_was_digit = false;
		}
		else {
			if (x > GAME_WIDTH - 1) return false;
			if (y < 0) return false;
			Piece p = char_to_piece(c);
			if (p==Piece::EMPTY) return false;
			bool is_white = isupper(c);
			int index = position_to_bindex({ x, y });
			if (p == Piece::KING) {
				if (is_white) {
					register_up(index, 0, Piece::KING);
					white_king_count++;
				}
				else {
					register_up(index, GAME_MAX_COLOR_ID, Piece::KING);
					black_king_count++;
				}
			}
			else {
				if (is_white) {
					if (white_id >= GAME_MAX_COLOR_ID) return false;
					register_up(index, white_id, p);
					white_id++;
				}

				else {
					if (black_id >= GAME_MAX_ID) return false;
					register_up(index, black_id, p);
					black_id++;
				}
			}
			x += 1;
			last_was_digit = false;
		}
	}
	if (white_king_count != 1 || black_king_count != 1) return false;

	init_coverage();
	return true;
}

void ChessBoard::init_coverage()
{
	for (int id = 0; id < GAME_MAX_ID; id++) piece_covers(id);
}

void ChessBoard::clear()
{
	m_coverage_delta_indices.clear();
	m_bindex_to_id.fill(0);
	m_bindex_to_piece.fill(Piece::EMPTY);
	m_id_to_bindex.fill(0);
	m_id_to_piece.fill(Piece::EMPTY);
	for (std::array<bool,GAME_BOARD_SIZE>& id_coverage : m_coverage) id_coverage.fill(false);
}


void ChessBoard::reset_coverage(int id)
{
	for (int bindex = 0; bindex < GAME_BOARD_SIZE; bindex++) m_coverage[id][bindex] = false;
	return;
}

void ChessBoard::set_coverage_single(int id, int index, bool value)
{
	 m_coverage[id][index] = value;
}

void ChessBoard::piece_covers(int id)
{
	const int bindex = m_id_to_bindex[id];
	const UniquePiece up = UniquePiece(id,m_id_to_piece[id]);
	switch (up.p) {
	case (Piece::KING):   return king_covers(id, bindex);
	case (Piece::QUEEN):  return queen_covers(id, bindex);
	case (Piece::BISHOP): return bishop_covers(id, bindex);
	case(Piece::KNIGHT):  return knight_covers(id, bindex);
	case(Piece::ROOK):    return rook_covers(id, bindex);
	case(Piece::PAWN):    return up.IsWhite() ? white_pawn_covers(id, bindex) : black_pawn_covers(id, bindex);
	default:              return;
	}
}

void ChessBoard::king_covers(int id, int bindex)
{
	for (Direction dir = Direction::N; dir <= Direction::NW; ++dir) {
		int8_t nsteps = GetOOBSteps(bindex, dir);
		if (nsteps > 0) {
			int to_bindex = bindex + get_bindex_delta(dir);
			m_coverage[id][to_bindex] = true;
		}
	}
}

void ChessBoard::queen_covers(int id, int bindex)
{
	bishop_covers(id, bindex);
	rook_covers(id, bindex);
}

void ChessBoard::bishop_covers(int id, int bindex)
{
	for (Direction dir = Direction::NE; dir <= Direction::NW; ++dir) {
		cover_and_append(dir, id, bindex);
	}
}

void ChessBoard::knight_covers(int id, int bindex)
{
	for (Direction dir = Direction::NNE; dir <= Direction::NNW; ++dir) {
		cover_and_append(dir, id, bindex);
	}
}

void ChessBoard::rook_covers(int id, int bindex)
{
	for (Direction dir = Direction::N; dir <= Direction::W; ++dir) {
		cover_and_append(dir, id, bindex);
	}
}

void ChessBoard::white_pawn_covers(int id, int bindex)
{
	const int forward = 8;
	Position from_pos = bindex_to_position(bindex);

	//takes_left
	if (from_pos.x > 0) {
		int to_index = bindex + forward - 1;
		m_coverage[id][to_index] = true;
	}

	//takes_right
	if (from_pos.x < GAME_WIDTH - 1) {
		int to_index = bindex + forward + 1;
		m_coverage[id][to_index] = true;
	}

	// TODO ep cover ... but do i really need it?
}

void ChessBoard::black_pawn_covers(int id, int bindex)
{
	const int forward = -8;
	Position from_pos = bindex_to_position(bindex);

	//takes_left
	if (from_pos.x > 0) {
		int to_index = bindex + forward - 1;
		m_coverage[id][to_index] = true;
	}

	//takes_right
	if (from_pos.x < GAME_WIDTH - 1) {
		int to_index = bindex + forward + 1;
		m_coverage[id][to_index] = true;
	}
}

void ChessBoard::cover_and_append(const Direction dir, const int id, const int bindex)
{
	int8_t nsteps = GetOOBSteps(bindex, dir);
	int d = get_bindex_delta(dir);
	int to_index = bindex;
	int enemy_king_id = id<GAME_MAX_COLOR_ID ? GAME_MAX_COLOR_ID : 0;
	for (int8_t n = 1; n <= nsteps; n++) {
		to_index += d;
		m_coverage[id][to_index] = true;
		if (!empty(to_index) && m_bindex_to_id[to_index]!=enemy_king_id) return;
	}
	return;
}

void ChessBoard::update_coverage()
{
	bool id_needs_update[32]{};
	for (const int bindex : m_coverage_delta_indices) {
		for (int id = 0; id < 32; id++) {
			id_needs_update[id] |= m_coverage[id][bindex];
		}
	}
	for (int id = 0; id < 32; id++) {
		if (id_needs_update[id]) {
			reset_coverage(id);
			piece_covers(id);
		}
	}
}

bool operator==(const ChessBoard& lhs, const ChessBoard& rhs)
{
    if (lhs.m_bindex_to_id != rhs.m_bindex_to_id) return false;
	if (lhs.m_bindex_to_piece != rhs.m_bindex_to_piece) return false;
    if (lhs.m_id_to_bindex != rhs.m_id_to_bindex) return false;
    if (lhs.m_id_to_piece != rhs.m_id_to_piece) return false;
	if (lhs.m_coverage != rhs.m_coverage) return false;
    return true;
}


PlayerVars::PlayerVars(bool w, int king_id_, int pawn_forward_, int pawn_start_y_, int pawn_promo_y_)
	:
	castles(), color{ w }, king_id(king_id_), color_offset(king_id_), pawn_forward(pawn_forward_),
	pawn_start_y(pawn_start_y_), pawn_promo_y(pawn_promo_y_) 
{
}

bool operator==(const PlayerVars& lhs, const PlayerVars& rhs)
{
	if (lhs.castles != rhs.castles) return false;
	if (lhs.color.white != rhs.color.white) return false;
	if (lhs.king_id != rhs.king_id) return false;
	if (lhs.color_offset != rhs.color_offset) return false;
	if (lhs.pawn_forward != rhs.pawn_forward) return false;
	if (lhs.pawn_start_y != rhs.pawn_start_y) return false;
	if (lhs.pawn_promo_y != rhs.pawn_promo_y) return false;
	return true;
}


SwapVars::SwapVars() :
	white(true, 0, GAME_WIDTH, 1, GAME_HEIGHT - 2),
	black(false, GAME_BLACK_ID_OFFSET, -GAME_WIDTH, GAME_HEIGHT - 2, 1),
	active(&white),
	passive(&black)
{
}

SwapVars::SwapVars(const SwapVars& other) :
	white(other.white), black(other.black), active(&white), passive(&black)
{
	if (other.active != &other.white) Swap();
}

void SwapVars::Swap()
{
	std::swap(active, passive);
}

bool operator==(const SwapVars& lhs, const SwapVars& rhs)
{
	if (lhs.white != rhs.white) return false;
	if (lhs.black != rhs.black) return false;
	if ((lhs.active == &lhs.white) != (rhs.active == &rhs.white)) return false;
	if ((lhs.passive == &lhs.white) != (rhs.passive == &rhs.white)) return false;
	return true;
}


Piece char_to_piece(char c)
{
	c = std::tolower(c);
	switch (c) {
	case 'k': return Piece::KING;
	case 'q': return Piece::QUEEN;
	case 'b': return Piece::BISHOP;
	case 'n': return Piece::KNIGHT;
	case 'r': return Piece::ROOK;
	case 'p': return Piece::PAWN;
	default: return Piece::EMPTY;
	}
}

char piece_to_char(Piece p)
{
	switch (p) {
	case Piece::EMPTY: return '?';
	case Piece::KING: return 'k';
	case Piece::QUEEN: return 'q';
	case Piece::BISHOP: return 'b';
	case Piece::KNIGHT: return 'n';
	case Piece::ROOK: return 'r';
	case Piece::PAWN: return 'p';
	default: return '.';
	}

}

std::string PositionToString(Position pos)
{
	std::string s_ret("a1");
	s_ret[0] = char(pos.x - 'a');
	s_ret[1] = char(pos.y - '1');
	return s_ret;
}

