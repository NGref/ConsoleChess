#include "CharMessageBox.h"

#include "gtest/gtest.h"

TEST(CharMessageBox, Constructor) {
	CharMessageBox cmb(0,0,10,5,10);
	EXPECT_EQ(cmb.get_xpos(), 0);
	EXPECT_EQ(cmb.get_ypos(), 0);
	EXPECT_EQ(cmb.get_width(), 10);
	EXPECT_EQ(cmb.get_height(), 5);
	EXPECT_EQ(cmb.get_capacity(), 10);
}

TEST(CharMessageBox, PushAndDraw) {
	CharDisplay cd(10, 3);
	CharMessageBox cmb(0, 0, 10, 3, 3);
	cmb.draw(&cd);
	// box empty with no messages
	for (int x = 0; x < 10; x++) {
		for (int y = 0; y < 3; y++) {
			EXPECT_EQ(cd.get_buffer(x, y), ' ');
		}
	}
	cmb.push("test");
	cmb.draw(&cd);
	EXPECT_EQ(cd.get_buffer(0, 2), 't');
	EXPECT_EQ(cd.get_buffer(1, 2), 'e');
	EXPECT_EQ(cd.get_buffer(2, 2), 's');
	EXPECT_EQ(cd.get_buffer(3, 2), 't');
	EXPECT_EQ(cd.get_buffer(4, 2), ' ');
	EXPECT_EQ(cd.get_buffer(0, 1), ' ');
	EXPECT_EQ(cd.get_buffer(0, 0), ' ');

	cmb.push("asdf");
	cmb.draw(&cd);
	EXPECT_EQ(cd.get_buffer(0, 2), 'a');
	EXPECT_EQ(cd.get_buffer(1, 2), 's');
	EXPECT_EQ(cd.get_buffer(2, 2), 'd');
	EXPECT_EQ(cd.get_buffer(3, 2), 'f');
	EXPECT_EQ(cd.get_buffer(4, 2), ' ');
	EXPECT_EQ(cd.get_buffer(0, 1), 't');
	EXPECT_EQ(cd.get_buffer(1, 1), 'e');
	EXPECT_EQ(cd.get_buffer(2, 1), 's');
	EXPECT_EQ(cd.get_buffer(3, 1), 't');
	EXPECT_EQ(cd.get_buffer(4, 1), ' ');
	EXPECT_EQ(cd.get_buffer(0, 0), ' ');

	cmb.push("1");
	cmb.draw(&cd);
	EXPECT_EQ(cd.get_buffer(0, 2), '1');
	EXPECT_EQ(cd.get_buffer(0, 1), 'a');
	EXPECT_EQ(cd.get_buffer(0, 0), 't');

	cmb.push("o");
	cmb.draw(&cd);
	EXPECT_EQ(cd.get_buffer(0, 2), 'o');
	EXPECT_EQ(cd.get_buffer(0, 1), '1');
	EXPECT_EQ(cd.get_buffer(0, 0), 'a');
}

TEST(CharMessageBox, ResizeAndMove) {
	CharDisplay cd(4, 4);
	CharMessageBox cmb(0, 0, 4, 4, 4);
	cmb.push("3");
	cmb.push("2");
	cmb.push("1");
	cmb.push("0");
	cmb.set_xpos(1);
	cmb.set_ypos(1);
	cmb.set_width(2);
	cmb.set_height(2);
	cmb.draw(&cd);

	// buffer should look like this:
	// ....\n
	// .1..\n
	// .0..\n
	// ....\n

	EXPECT_EQ(cd.get_buffer(0, 0), ' ');
	EXPECT_EQ(cd.get_buffer(0, 1), ' ');
	EXPECT_EQ(cd.get_buffer(1, 0), ' ');
	EXPECT_EQ(cd.get_buffer(1, 1), '1');
	EXPECT_EQ(cd.get_buffer(1, 2), '0');
	EXPECT_EQ(cd.get_buffer(2, 1), ' ');
	EXPECT_EQ(cd.get_buffer(2, 2), ' ');
	EXPECT_EQ(cd.get_buffer(1, 3), ' ');
}

TEST(CharMessageBox, MessageBiggerThanWidth) {
	CharDisplay cd(4, 2);
	CharMessageBox cmb(0, 0, 4, 2, 2);
	cmb.push("bigger4");
	cmb.draw(&cd);

	EXPECT_EQ(cd.get_buffer(0, 0), 'b');
	EXPECT_EQ(cd.get_buffer(1, 0), 'i');
	EXPECT_EQ(cd.get_buffer(2, 0), 'g');
	EXPECT_EQ(cd.get_buffer(3, 0), 'g');
	EXPECT_EQ(cd.get_buffer(0, 1), 'e');
	EXPECT_EQ(cd.get_buffer(1, 1), 'r');
	EXPECT_EQ(cd.get_buffer(2, 1), '4');
	EXPECT_EQ(cd.get_buffer(3, 1), ' ');
}

TEST(CharMessageBox, PartialDraw) {
	CharDisplay cd(4, 4);
	CharMessageBox cmb1(-1, -1, 3, 3, 3);
	CharMessageBox cmb2(2, 2, 3, 3, 3);

	cmb1.push("123");
	cmb1.push("456");
	cmb1.push("789");
	cmb1.draw(&cd);

	cmb2.push("abc");
	cmb2.push("def");
	cmb2.push("ghi");
	cmb2.draw(&cd);

	// buffer should look like this:
	// 56..\n
	// 89..\n
	// ..ab\n
	// ..de\n

	EXPECT_EQ(cd.get_buffer(0, 0), '5');
	EXPECT_EQ(cd.get_buffer(1, 0), '6');
	EXPECT_EQ(cd.get_buffer(2, 0), ' ');
	EXPECT_EQ(cd.get_buffer(3, 0), ' ');
	EXPECT_EQ(cd.get_buffer_unsafe(4, 0), '\n');

	EXPECT_EQ(cd.get_buffer(0, 1), '8');
	EXPECT_EQ(cd.get_buffer(1, 1), '9');
	EXPECT_EQ(cd.get_buffer(2, 1), ' ');
	EXPECT_EQ(cd.get_buffer(3, 1), ' ');
	EXPECT_EQ(cd.get_buffer_unsafe(4, 1), '\n');

	EXPECT_EQ(cd.get_buffer(0, 2), ' ');
	EXPECT_EQ(cd.get_buffer(1, 2), ' ');
	EXPECT_EQ(cd.get_buffer(2, 2), 'a');
	EXPECT_EQ(cd.get_buffer(3, 2), 'b');
	EXPECT_EQ(cd.get_buffer_unsafe(4, 2), '\n');

	EXPECT_EQ(cd.get_buffer(0, 3), ' ');
	EXPECT_EQ(cd.get_buffer(1, 3), ' ');
	EXPECT_EQ(cd.get_buffer(2, 3), 'd');
	EXPECT_EQ(cd.get_buffer(3, 3), 'e');
	EXPECT_EQ(cd.get_buffer_unsafe(4, 3), '\n');
}