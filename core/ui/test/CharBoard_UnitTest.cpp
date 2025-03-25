#include "CharBoard.h"

#include "gtest/gtest.h"


TEST(CharBoard, Constructor) {
	CharBoard b(0, 0, BoardSize::SMALL, "../../res", false, true, true);

	EXPECT_EQ(b.get_xpos(), 0);
	EXPECT_EQ(b.get_ypos(), 0);
	EXPECT_EQ(b.get_width(), 19);
	EXPECT_EQ(b.get_height(), 9);
	ASSERT_TRUE(b.get_init_ok());

	CharDisplay cd(b.get_width(), b.get_height());
	b.draw(&cd,std::vector<TileP>());

	const std::string buffer =
		"   A B C D E F G H \n"
		"8  - - - - - - - - \n"
		"7  - - - - - - - - \n"
		"6  - - - - - - - - \n"
		"5  - - - - - - - - \n"
		"4  - - - - - - - - \n"
		"3  - - - - - - - - \n"
		"2  - - - - - - - - \n"
		"1  - - - - - - - - \n";

	EXPECT_EQ(buffer, cd.get_buffer_str());
}

//TODO setters, partial draw , delta draw