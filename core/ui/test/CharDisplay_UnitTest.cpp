#include "CharDisplay.h"

#include "gtest/gtest.h"

TEST(CharDisplay, ConstructorAndGetters) {
	CharDisplay cd{ 100,20 };
	EXPECT_EQ(cd.get_width(), 100);
	EXPECT_EQ(cd.get_height(), 20);
	for (int x = 0; x < 100; x++) {
		for (int y = 0; y < 20; y++) {
			EXPECT_EQ(cd.get_buffer(x,y), ' ');
		}
	}
	EXPECT_EQ(cd.get_buffer(-1, -1), '\0');
	CharDisplay cd2{ 0, 0 };
	EXPECT_EQ(cd2.get_width(), 0);
	EXPECT_EQ(cd2.get_height(), 0);
	EXPECT_EQ(cd2.get_buffer(0, 0), '\0');
	CharDisplay cd3{ -1,-1 };
	EXPECT_EQ(cd3.get_width(), 0);
	EXPECT_EQ(cd3.get_height(), 0);
	EXPECT_EQ(cd3.get_buffer(0, 0), '\0');
}

TEST(CharDisplay, SettingTheBuffer) {
	CharDisplay cd{ 1,1};
	cd.set_buffer(0, 0, '!');
	EXPECT_EQ(cd.get_buffer(0, 0), '!') << "buffer not set correctly";

	cd.set_buffer(1, 0, '!');
	EXPECT_EQ(cd.get_buffer_unsafe(1, 0), '\n') << "safe method did overwrite newline in buffer";
}