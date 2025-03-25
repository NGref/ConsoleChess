#include "GameInterface.h"
#include "Game.h"

#include "gtest/gtest.h"

#include <memory>
#include <algorithm>

TEST(GameInterface, CloningIsDeepcopy) {
	std::unique_ptr<IGame> game = std::make_unique<Game>();
	game->move("a2a4");
	std::unique_ptr<IGame> game_clone = game->clone();
	game_clone->undo();
	EXPECT_NE(game->get_active_color().white, game_clone->get_active_color().white);
	EXPECT_NE(game->get_possible_moves(), game_clone->get_possible_moves());
}

TEST(GameInterface, PositionWrappers) {
	std::unique_ptr<IGame> game = std::make_unique<Game>();
	std::vector<TileI> vi = game->get_all_tiles_ind();
	std::vector<TileP> vp = game->get_all_tiles_pos();
	std::vector<TileP> vi_converted;
	vi_converted.reserve(vi.size());
	for (const TileI& tile : vi) {
		vi_converted.push_back(TileP{ bindex_to_position(tile.index), tile.piece, tile.color });
	}
	//std::sort(vi_converted.begin(), vi_converted.end(), [](TileP a, TileP b) {return position_to_bindex(a.position) > position_to_bindex(b.position); });
	//std::sort(vp.begin(), vp.end(), [](TileP a, TileP b) {return position_to_bindex(a.position) > position_to_bindex(b.position); });
	EXPECT_EQ(vi_converted, vp);


	std::vector<TileI> vinew = game->get_new_tiles_ind();
	std::vector<TileP> vpnew = game->get_new_tiles_pos();
	std::vector<TileP> vinew_converted;
	vinew_converted.reserve(vi.size());
	for (const TileI& tile : vinew) {
		vinew_converted.push_back(TileP{ bindex_to_position(tile.index), tile.piece, tile.color });
	}
	//std::sort(vinew_converted.begin(), vinew_converted.end(), [](TileP a, TileP b) {return position_to_bindex(a.position) > position_to_bindex(b.position); });
	//std::sort(vpnew.begin(), vpnew.end(), [](TileP a, TileP b) {return position_to_bindex(a.position) > position_to_bindex(b.position); });
	EXPECT_EQ(vinew_converted, vpnew);


	std::vector<TileI> vir = game->get_reverse_new_tiles_ind();
	std::vector<TileP> vpr = game->get_reverse_new_tiles_pos();
	std::vector<TileP> vir_converted;
	vir_converted.reserve(vir.size());
	for (const TileI& tile : vir) {
		vir_converted.push_back(TileP{ bindex_to_position(tile.index), tile.piece, tile.color });
	}
	//std::sort(vir_converted.begin(), vir_converted.end());
	//std::sort(vpr.begin(), vpr.end());
	EXPECT_EQ(vir_converted, vpr);
}