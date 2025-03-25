#include "CharBorder.h"

#include "gtest/gtest.h"


TEST(CharBorder, Constructor) {
	CharDisplay cd(10, 10);
	CharBorder cb(0, 0, 10, 10);
	cb.draw(&cd);
	//check corners
	EXPECT_EQ(cd.get_buffer(0, 0), '+');
	EXPECT_EQ(cd.get_buffer(0, 9), '+');
	EXPECT_EQ(cd.get_buffer(9, 0), '+');
	EXPECT_EQ(cd.get_buffer(9, 9), '+');

	//check edges
	for (int x = 1; x < 9; x++) {
		EXPECT_EQ(cd.get_buffer(x, 0), '-');
		EXPECT_EQ(cd.get_buffer(x, 9), '-');
	}
	for (int y = 1; y < 9; y++) {
		EXPECT_EQ(cd.get_buffer(0, y), '|');
		EXPECT_EQ(cd.get_buffer(9, y), '|');
	}
}

TEST(CharBorder, MoveAndResize) {
	CharDisplay cd(10, 10);
	CharBorder cb(0, 0, 10, 10);
	cb.set_height(3);
	cb.set_width(3);
	cb.set_xpos(1);
	cb.set_ypos(1);
	cb.draw(&cd);
	//check corners
	EXPECT_EQ(cd.get_buffer(1, 1), '+');
	EXPECT_EQ(cd.get_buffer(1, 3), '+');
	EXPECT_EQ(cd.get_buffer(3, 1), '+');
	EXPECT_EQ(cd.get_buffer(3, 3), '+');
	//check corners
	EXPECT_EQ(cd.get_buffer(2, 1), '-');
	EXPECT_EQ(cd.get_buffer(2, 3), '-');
	EXPECT_EQ(cd.get_buffer(1, 2), '|');
	EXPECT_EQ(cd.get_buffer(3, 2), '|');
	
}

TEST(CharBorder, OutOfBounds) {
	CharDisplay cd(4, 4);
	CharBorder cb1(-1, -1, 3, 3);
	CharBorder cb2(2, 2, 3, 3);
	cb1.draw(&cd);
	cb2.draw(&cd);
	//buffer should be:
	//.|..\n
	//-+..\n
	//..+-\n
	//..|.\n
	EXPECT_EQ(cd.get_buffer(0, 0), ' ');
	EXPECT_EQ(cd.get_buffer(1, 0), '|');
	EXPECT_EQ(cd.get_buffer(2, 0), ' ');
	EXPECT_EQ(cd.get_buffer(3, 0), ' ');
	EXPECT_EQ(cd.get_buffer_unsafe(4, 0), '\n');

	EXPECT_EQ(cd.get_buffer(0, 1), '-');
	EXPECT_EQ(cd.get_buffer(1, 1), '+');
	EXPECT_EQ(cd.get_buffer(2, 1), ' ');
	EXPECT_EQ(cd.get_buffer(3, 1), ' ');
	EXPECT_EQ(cd.get_buffer_unsafe(4, 1), '\n');

	EXPECT_EQ(cd.get_buffer(0, 2), ' ');
	EXPECT_EQ(cd.get_buffer(1, 2), ' ');
	EXPECT_EQ(cd.get_buffer(2, 2), '+');
	EXPECT_EQ(cd.get_buffer(3, 2), '-');
	EXPECT_EQ(cd.get_buffer_unsafe(4, 2), '\n');

	EXPECT_EQ(cd.get_buffer(0, 3), ' ');
	EXPECT_EQ(cd.get_buffer(1, 3), ' ');
	EXPECT_EQ(cd.get_buffer(2, 3), '|');
	EXPECT_EQ(cd.get_buffer(3, 3), ' ');
	EXPECT_EQ(cd.get_buffer_unsafe(4, 3), '\n');
}
