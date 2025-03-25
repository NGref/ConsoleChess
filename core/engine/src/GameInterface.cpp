#include "GameInterface.h"

std::vector<GameMove> IGame::get_possible_moves(Position from_pos) const
{
    return get_possible_moves(position_to_bindex(from_pos));
}

std::vector<Position> IGame::get_possible_moves_pos(Position from_pos) const
{
    const std::vector<int>& vind = get_possible_moves_ind(position_to_bindex(from_pos));
    std::vector<Position> vpos;
    vpos.reserve(vind.size());
    for (int index : vind) vpos.push_back(bindex_to_position(index));
    return vpos;
}

std::vector<TileP> IGame::get_all_tiles_pos() const
{
    const std::vector<TileI>& vind = get_all_tiles_ind();
    std::vector<TileP> vpos;
    vpos.reserve(vind.size());
    for (const TileI& ti : vind) vpos.emplace_back(bindex_to_position(ti.index),ti.piece, ti.color);
    return vpos;
}

std::vector<TileP> IGame::get_new_tiles_pos() const
{
    const std::vector<TileI>& vind = get_new_tiles_ind();
    std::vector<TileP> vpos;
    vpos.reserve(vind.size());
    for (const TileI& ti : vind) vpos.emplace_back(bindex_to_position(ti.index), ti.piece, ti.color);
    return vpos;
}

std::vector<TileP> IGame::get_reverse_new_tiles_pos() const
{
    const std::vector<TileI>& vind = get_reverse_new_tiles_ind();
    std::vector<TileP> vpos;
    vpos.reserve(vind.size());
    for (const TileI& ti : vind) vpos.emplace_back(bindex_to_position(ti.index), ti.piece, ti.color);
    return vpos;
}
