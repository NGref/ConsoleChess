#pragma once
#include <array>
#include <vector>
#include  <memory>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <bitset>


#include "GameInterfaceUtil.h"

#define GAME_MAX_COLOR_ID 16
#define GAME_MAX_ID 2*GAME_MAX_COLOR_ID
#define GAME_BLACK_ID_OFFSET GAME_MAX_COLOR_ID


enum Direction {
    NONE = 0,
    N,
    E,
    S,
    W,
    NE,
    SE,
    SW,
    NW,
    NNE,
    ENE,
    ESE,
    SSE,
    SSW,
    WSW,
    WNW,
    NNW
};

#define NUMBER_OF_DIRECTIONS 17

inline Direction operator++(Direction& x) { return x = (Direction)(((int)(x)+1)); }


int get_bindex_delta(const Direction dir);
Direction get_direction(const int delta_bindex);
bool is_hvd(const Direction& dir);
Direction get_hvd(const int from_bindex, const int to_bindex);
Direction get_opposite_direction(const Direction dir);

// number of steps on the board from bindex in direction. Knight #steps always in {0,1}
int8_t GetOOBSteps(int bindex, Direction direction);


/// <summary>
/// Piece on the board, which is registered with id. Color can be deduced by id.
/// UniquePiece is empty if piece is empty.
/// </summary>
struct UniquePiece {
    int id;
    Piece p;

    bool IsEmpty() const;
    bool IsWhite() const;
    bool IsBlack() const;
    ChessColor GetColor() const;
    bool IsEnemy(ChessColor cc) const;
    bool IsEnemy(const UniquePiece& other) const;
    bool IsAlly(ChessColor cc) const;
    bool IsAlly(const UniquePiece& other) const;
    friend bool operator==(const UniquePiece& lhs, const UniquePiece& rhs);
    friend bool operator!=(const UniquePiece& lhs, const UniquePiece& rhs);
};

/// <summary>
/// A memory efficient variant of GameMove struct. Only used inside the engine
/// first 6 bits: from index
/// next bit: promotion flag
/// next 2 bits: promotion piece
/// next bit: empty/en passant takes flag
/// last 6 bits: to index
/// </summary>
class GameMoveInt
{
public:
    GameMoveInt() : m_data(0) {}
    GameMoveInt(int from, int to);
    GameMoveInt(int from, int to, Piece promotion);

    int get_from() const;
    int get_to() const;
    Piece get_promotion() const;
    bool is_promotion() const;
    bool is_null() const;

    void set_from(int from);
    void set_to(int to);
    void set_promotion(Piece promotion);
    void set_null();

    //debug
    friend inline bool operator==(const GameMoveInt& lhs, const GameMoveInt& rhs) { return lhs.m_data == rhs.m_data; }
    friend inline bool operator!=(const GameMoveInt& lhs, const GameMoveInt& rhs) { return !(lhs == rhs); }
private:
    uint16_t m_data;
};

GameMoveInt gm_to_gmi(const GameMove& m);
GameMove gmi_to_gm(const GameMoveInt& gmi);
bool operator==(const GameMove& lhs, const GameMoveInt& rhs);


struct PlayerCastles {
    bool kscastle = true;
    bool qscastle = true;

    bool CanCastle();
    void SetFalse();
    friend bool operator==(const PlayerCastles& lhs, const PlayerCastles& rhs);
};

/// <summary>
/// A more detailed variant of GameMove struct.
/// </summary>
class GameDelta {
public:
    GameDelta(const GameMove& move_);
    GameDelta(
        GameMove move_, UniquePiece takes_,
        PlayerCastles white_castle_, PlayerCastles black_castle_,
        uint8_t half_turns_, int8_t p2_index_,
        bool ksc_, bool qsc_, bool ep_, bool check_
    );
    bool IsTakes() const;
    bool IsPromotion() const;
    bool IsCastle() const;
    bool IsQSCastle() const;
    bool IsKSCastle() const;
    bool IsEnPassant() const;
    bool IsCheck() const;
    friend bool operator==(const GameDelta& lhs, const GameDelta& rhs);
public:
    GameMove move;
    UniquePiece takes = UniquePiece();
    PlayerCastles white_castle = PlayerCastles{false, false};
    PlayerCastles black_castle = PlayerCastles{ false, false };
    uint8_t half_turns = 0;
    int8_t p2_index = -1;
    bool ksc = false;
    bool qsc = false;
    bool ep = false;
    bool check = false;
};

class ChessBoard 
{
public:
    ChessBoard();

    bool new_board(const std::string& fen_board_section);

    void apply_gamedelta(const GameDelta& gd);
    void undo_gamedelta(const GameDelta& gd);

    int get_bindex(int id) const;
    int get_id(int bindex) const;
    Piece get_piece_from_bindex(int bindex) const;
    Piece get_piece_from_id(int id) const;
    UniquePiece get_up(int bindex) const;

    bool empty(int bindex);

    bool is_covered(int index)  const;
    bool is_covered_color(int index, int color_off)  const;
    int get_cover_count(int index) const;
    int get_cover_count_color(int index, int color_off) const;
    // returns id of first piece that covers
    int get_first_cover_id(int index) const;
    // returns id of first piece with same color as color_off that covers
    int get_first_cover_id_color(int index, int color_off) const;
    friend bool operator==(const ChessBoard& lhs, const ChessBoard& rhs);
private:
    bool init_from_fen(const std::string& fen);
    void init_coverage();
    
    void clear();

    void register_up(int bindex, int id, Piece p);

    void reset_coverage(int id);
    void set_coverage_single(int id, int index, bool value);

    void piece_covers(int id);
    void king_covers(int id, int bindex);
    void queen_covers(int id, int bindex);
    void bishop_covers(int id, int bindex);
    void knight_covers(int id, int bindex);
    void rook_covers(int id, int bindex);
    void white_pawn_covers(int id, int bindex);
    void black_pawn_covers(int id, int bindex);
    void cover_and_append(const Direction, const int, const int);

    void update_coverage();
private:
    std::vector<int> m_coverage_delta_indices;
    std::array<int, GAME_BOARD_SIZE> m_bindex_to_id;
    std::array<Piece, GAME_BOARD_SIZE> m_bindex_to_piece;
    std::array<int, GAME_MAX_ID> m_id_to_bindex;
    std::array<Piece, GAME_MAX_ID> m_id_to_piece;
    std::array<std::array<bool, GAME_BOARD_SIZE>, GAME_MAX_ID> m_coverage;
};


class PlayerVars {
public:
    PlayerVars(bool w, int king_id_, int pawn_forward_, int pawn_start_y_, int pawn_promo_y_);
    friend bool operator==(const PlayerVars& lhs, const PlayerVars& rhs);
public:
    PlayerCastles castles;
    const ChessColor color;
    const int king_id;
    const int color_offset;
    const int pawn_forward;
    const int pawn_start_y;
    const int pawn_promo_y;
};

class SwapVars {
    
public:
    SwapVars();
    SwapVars(const SwapVars& other);
    void Swap();
    friend bool operator==(const SwapVars& lhs, const SwapVars& rhs);
public:
    PlayerVars white;
    PlayerVars black;
    PlayerVars* active;
    PlayerVars* passive;
};

bool operator==(const SwapVars& lhs, const SwapVars& rhs);

Piece char_to_piece(char c);
char piece_to_char(const Piece p);

std::string PositionToString(Position pos);