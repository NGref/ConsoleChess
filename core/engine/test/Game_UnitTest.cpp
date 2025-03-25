#include "Game.h"

#include "gtest/gtest.h"

#include <fstream>
#include <vector>
#include <string>
#include <algorithm>


TEST(GameTest, DefaultConstructor) {
	Game game{};
	// test fen init
	ASSERT_TRUE(game.get_init_ok()) << "Default fen init failed";
	
	// test some getters
	EXPECT_TRUE(game.get_active_color().IsWhite());
	EXPECT_FALSE(game.get_game_has_ended());
	EXPECT_EQ(game.get_move_str_fmt(), GameMoveStrFmt::UCI);
	EXPECT_EQ(game.get_turn_number(),1);
	EXPECT_FALSE(game.get_is_check());

	//test the board
	const std::vector<TileI> occupied_tiles = game.get_all_tiles_ind();
	ASSERT_EQ(occupied_tiles.size(), 32) << "Missing Pieces on board";
	
	EXPECT_EQ(occupied_tiles[0].index, 0);
	EXPECT_EQ(occupied_tiles[0].piece, Piece::ROOK);
	EXPECT_TRUE(occupied_tiles[0].color.IsWhite());

	// test legal move generation
	//test piece with no legal moves
	const int white_rook_init_ind = 0;
	const std::vector<int> legal_rook_moves = game.get_possible_moves_ind(white_rook_init_ind);
	EXPECT_EQ(legal_rook_moves.size(), 0) << "Rook cannot move initially";
	//test piece with legal moves
	const int white_knight_init_ind = 1;
	const std::vector<int> legal_knight_moves = game.get_possible_moves_ind(white_knight_init_ind);
	ASSERT_EQ(legal_knight_moves.size(), 2) << "Knight has wrong number of legal moves!";
	EXPECT_NE(std::find(legal_knight_moves.begin(), legal_knight_moves.end(), 16), legal_knight_moves.end());
	EXPECT_NE(std::find(legal_knight_moves.begin(), legal_knight_moves.end(), 18), legal_knight_moves.end());

	// no delta yet
	EXPECT_EQ(game.get_new_tiles_ind().size(), 0);
	EXPECT_EQ(game.get_reverse_new_tiles_ind().size(), 0);
}

TEST(GameTest, ConstructorWithParam) {
	// some other position and formats
	Game game2("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1", GameMoveStrFmt::SAN);
	EXPECT_EQ(game2.get_move_str_fmt(), GameMoveStrFmt::SAN);
	EXPECT_TRUE(game2.get_init_ok()) << "Default fen init failed";

	Game game3("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1", GameMoveStrFmt::LAN);
	EXPECT_EQ(game3.get_move_str_fmt(), GameMoveStrFmt::LAN);
	EXPECT_TRUE(game3.get_init_ok()) << "Default fen init failed";

	Game game4("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1", GameMoveStrFmt::DEFAULT);
	EXPECT_EQ(game4.get_move_str_fmt(), GameMoveStrFmt::UCI);
	EXPECT_TRUE(game4.get_init_ok()) << "Default fen init failed";

	Game game5("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8", GameMoveStrFmt::UCI);
	EXPECT_EQ(game5.get_move_str_fmt(), GameMoveStrFmt::UCI);
	EXPECT_TRUE(game5.get_init_ok()) << "Default fen init failed";

	Game game6("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
	EXPECT_EQ(game6.get_move_str_fmt(), GameMoveStrFmt::UCI);
	EXPECT_TRUE(game6.get_init_ok()) << "Default fen init failed";
}

TEST(GameTest, CopyConstructor) {
	Game game{};
	Game game_copy(game);
	EXPECT_EQ(game, game_copy);
}

TEST(GameTest, LegalMoves) {
	std::ifstream dataset("test/legal_data.csv");
	ASSERT_TRUE(dataset.is_open());
	std::string line;
	while (std::getline(dataset, line)) {
		// parse data
		int line_off = 0;
		std::string name = line.substr(line_off,line.find(',', line_off)- line_off);
		
		line_off += name.size() + 1;
		std::string fen = line.substr(line_off, line.find(',', line_off) - line_off);
		line_off += fen.size() + 1;
		std::vector<std::string> dataset_legal_moves;
		std::string ucis_size = line.substr(line_off, line.find(',', line_off) - line_off);
		//std::cout << name << "; " << fen << "; " << ucis_size << '\n';
		dataset_legal_moves.reserve(std::stoi(ucis_size));
		line_off += ucis_size.size() + 1;
		while (line_off < line.size()) {
			const std::string uci = line.substr(line_off, line.find(',', line_off)-line_off);
			dataset_legal_moves.push_back(uci);
			line_off += uci.size() + 1;
		}
		Game game(fen);
		ASSERT_TRUE(game.get_init_ok()) << name << ": fen init failed";

		std::vector<std::string> game_legal_moves = game.get_possible_moves_str();

		std::sort(game_legal_moves.begin(), game_legal_moves.end());
		std::sort(dataset_legal_moves.begin(), dataset_legal_moves.end());
		EXPECT_EQ(game_legal_moves, dataset_legal_moves) << name << " uci mismatch!";

	}
}

TEST(GameTest, MoveArgument) {
	Game game{};
	EXPECT_EQ(game.move(GameMove(0, 3)), GameState::INVALID_MOVE);
	EXPECT_EQ(game.move(GameMove(2, 10)), GameState::INVALID_MOVE);
	EXPECT_EQ(game.move(GameMove(8, 16,Piece::QUEEN)), GameState::INVALID_MOVE);
	EXPECT_EQ(game.move(GameMove(-1, 10000)), GameState::INVALID_MOVE);
	EXPECT_EQ(game.move(GameMove(10000, -1)), GameState::INVALID_MOVE);
	EXPECT_EQ(game.move(GameMove(18, 19)), GameState::INVALID_MOVE);
	EXPECT_EQ(game.move(GameMove(62, 55)), GameState::INVALID_MOVE);

	EXPECT_EQ(game.move(""), GameState::INVALID_MOVE);
	EXPECT_EQ(game.move("aaaaaaaaaaa"), GameState::INVALID_MOVE);
	EXPECT_EQ(game.move("12345678"), GameState::INVALID_MOVE);
	EXPECT_EQ(game.move("d5d5"), GameState::INVALID_MOVE);
	EXPECT_EQ(game.move("a12a"), GameState::INVALID_MOVE);
	EXPECT_EQ(game.move("a1b1"), GameState::INVALID_MOVE);
	EXPECT_EQ(game.move("x1b1"), GameState::INVALID_MOVE);
	EXPECT_EQ(game.move("a1b9"), GameState::INVALID_MOVE);
}

TEST(GameTest, MoveUndoInvariance) {
	std::ifstream dataset("test/legal_data.csv");
	ASSERT_TRUE(dataset.is_open());
	std::string line;
	while (std::getline(dataset, line)) {
		// parse data
		int line_off = 0;
		std::string name = line.substr(line_off, line.find(',', line_off) - line_off);

		line_off += name.size() + 1;
		std::string fen = line.substr(line_off, line.find(',', line_off) - line_off);
		line_off += fen.size() + 1;
		std::vector<std::string> dataset_legal_moves;
		std::string ucis_size = line.substr(line_off, line.find(',', line_off) - line_off);
		//std::cout << name << "; " << fen << "; " << ucis_size << '\n';
		dataset_legal_moves.reserve(std::stoi(ucis_size));
		line_off += ucis_size.size() + 1;
		while (line_off < line.size()) {
			const std::string uci = line.substr(line_off, line.find(',', line_off) - line_off);
			dataset_legal_moves.push_back(uci);
			line_off += uci.size() + 1;
		}
		Game game(fen);
		ASSERT_TRUE(game.get_init_ok()) << name << ": fen init failed";
		Game game_copy(game);
		ASSERT_TRUE(game_copy.get_init_ok()) << name << ": fen init failed";

		for (const std::string& move : dataset_legal_moves) {
			EXPECT_NE(game.move(move), GameState::INVALID_MOVE) << name << ": move " << move << " should be valid!";
			game.move(move);
			game.undo();
			ASSERT_EQ(game, game_copy) << "Equality failed: " << name << ", " << move;
		}
		
	}
}

TEST(GameTest, NewGame) {
	Game game;
	Game game2("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
	ASSERT_TRUE(game2.get_init_ok());
	game2.new_game();
	EXPECT_EQ(game, game2);

	Game game3("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
	game2.new_game("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
	EXPECT_EQ(game2, game3);
}


TEST(GameTest, Perft) {
	Game game{};
	ASSERT_TRUE(game.get_init_ok()) << "Default fen init failed";
	EXPECT_EQ(game.perft(1), 20ULL);
	EXPECT_EQ(game.perft(2), 400ULL);
	EXPECT_EQ(game.perft(3), 8902ULL);
	EXPECT_EQ(game.perft(4), 197281ULL);
	EXPECT_EQ(game.perft(5), 4865609ULL);
	//EXPECT_EQ(game.perft(6), 119060324ULL);

	Game game2("r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1");
	ASSERT_TRUE(game2.get_init_ok()) << "Default fen init failed";
	EXPECT_EQ(game2.perft(1), 48ULL);
	EXPECT_EQ(game2.perft(2), 2039ULL);
	EXPECT_EQ(game2.perft(3), 97862ULL);
	EXPECT_EQ(game2.perft(4), 4085603ULL);
	//EXPECT_EQ(game2.perft(5), 193690690ULL);

	Game game3("8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1");
	EXPECT_TRUE(game3.get_init_ok()) << "Default fen init failed";
	EXPECT_EQ(game3.perft(1), 14ULL);
	EXPECT_EQ(game3.perft(2), 191ULL);
	EXPECT_EQ(game3.perft(3), 2812ULL);
	EXPECT_EQ(game3.perft(4), 43238ULL);
	EXPECT_EQ(game3.perft(5), 674624ULL);
	//EXPECT_EQ(game3.perft(6), 11030083ULL);

	Game game4("r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1");
	EXPECT_TRUE(game4.get_init_ok()) << "Default fen init failed";
	EXPECT_EQ(game4.perft(1), 6ULL);
	EXPECT_EQ(game4.perft(2), 264ULL);
	EXPECT_EQ(game4.perft(3), 9467ULL);
	EXPECT_EQ(game4.perft(4), 422333ULL);
	//EXPECT_EQ(game4.perft(5), 15833292ULL);

	Game game5("rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8");
	EXPECT_TRUE(game5.get_init_ok()) << "Default fen init failed";
	EXPECT_EQ(game5.perft(1), 44ULL);
	EXPECT_EQ(game5.perft(2), 1486ULL);
	EXPECT_EQ(game5.perft(3), 62379ULL);
	EXPECT_EQ(game5.perft(4), 2103487ULL);
	//EXPECT_EQ(game5.perft(5), 89941194ULL);

	Game game6("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
	EXPECT_EQ(game6.get_move_str_fmt(), GameMoveStrFmt::UCI);
	EXPECT_TRUE(game6.get_init_ok()) << "Default fen init failed";
	EXPECT_EQ(game6.perft(1), 46ULL);
	EXPECT_EQ(game6.perft(2), 2079ULL);
	EXPECT_EQ(game6.perft(3), 89890ULL);
	EXPECT_EQ(game6.perft(4), 3894594ULL);
	//EXPECT_EQ(game6.perft(5), 164075551ULL);
}
