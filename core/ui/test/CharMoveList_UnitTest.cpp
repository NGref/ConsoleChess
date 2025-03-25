#include "CharMoveList.h"

#include "gtest/gtest.h"

#include <memory>

TEST(CharMoveList, Constructor) {
	//horizontal
	std::unique_ptr<ICharMoveList> h = std::make_unique<CharMoveListH>(0, 0, 1);
	CharDisplay cd(h->get_width(),h->get_height());
	h->draw(&cd);
	const std::string hbuffer =
		"+---------+---------+\n"
		"|  Move   | 1       |\n"
		"+---------+---------+\n"
		"| >White  |         |\n"
		"+---------+---------+\n"
		"|  Black  |         |\n"
		"+---------+---------+\n";
	EXPECT_EQ(hbuffer, cd.get_buffer_str());

	//vertical
	std::unique_ptr<ICharMoveList> v = std::make_unique<CharMoveListV>(0, 0, 1);
	cd.set_size(v->get_width(), v->get_height());
	v->draw(&cd);

	std::string vbuffer =
		"+------+---------+---------+\n"
		"| Move | >White  |  Black  |\n"
		"+------+---------+---------+\n"
		"| 1    |         |         |\n"
		"+------+---------+---------+\n";
	EXPECT_EQ(vbuffer, cd.get_buffer_str());
}

TEST(CharMoveList, Setters) {
	std::unique_ptr<ICharMoveList> h = std::make_unique<CharMoveListH>(0, 0, 1);
	h->set_xpos(1);
	h->set_ypos(1);
	h->set_number_of_moves(2);
	CharDisplay cd(h->get_width() + 1, h->get_height() + 1, '?');
	h->draw(&cd);
	const std::string hbuffer =
		"????????????????????????????????\n"
		"?+---------+---------+---------+\n"
		"?|  Move   | 1       | 2       |\n"
		"?+---------+---------+---------+\n"
		"?| >White  |         |         |\n"
		"?+---------+---------+---------+\n"
		"?|  Black  |         |         |\n"
		"?+---------+---------+---------+\n";
	EXPECT_EQ(hbuffer, cd.get_buffer_str());

	std::unique_ptr<ICharMoveList> v = std::make_unique<CharMoveListV>(0, 0, 1);
	v->set_xpos(1);
	v->set_ypos(1);
	v->set_number_of_moves(2);
	cd.set_size(v->get_width()+1, v->get_height()+1);
	v->draw(&cd);

	std::string vbuffer =
		"?????????????????????????????\n"
		"?+------+---------+---------+\n"
		"?| Move | >White  |  Black  |\n"
		"?+------+---------+---------+\n"
		"?| 1    |         |         |\n"
		"?+------+---------+---------+\n"
		"?| 2    |         |         |\n"
		"?+------+---------+---------+\n";
	EXPECT_EQ(vbuffer, cd.get_buffer_str());

}

TEST(CharMoveList, Push) {
	//horizontal
	std::unique_ptr<ICharMoveList> h = std::make_unique<CharMoveListH>(0, 0, 1);
	CharDisplay cd(h->get_width(), h->get_height());
	h->push("test");
	h->draw(&cd);
	const std::string hbuffer =
		"+---------+---------+\n"
		"|  Move   | 1       |\n"
		"+---------+---------+\n"
		"|  White  | test    |\n"
		"+---------+---------+\n"
		"| >Black  |         |\n"
		"+---------+---------+\n";
	EXPECT_EQ(hbuffer, cd.get_buffer_str());

	//vertical
	std::unique_ptr<ICharMoveList> v = std::make_unique<CharMoveListV>(0, 0, 1);
	cd.set_size(v->get_width(), v->get_height());
	v->push("test");
	v->draw(&cd);

	std::string vbuffer =
		"+------+---------+---------+\n"
		"| Move |  White  | >Black  |\n"
		"+------+---------+---------+\n"
		"| 1    | test    |         |\n"
		"+------+---------+---------+\n";
	EXPECT_EQ(vbuffer, cd.get_buffer_str());
}

TEST(CharMoveList, PushTooLong) {
	//horizontal
	std::unique_ptr<ICharMoveList> h = std::make_unique<CharMoveListH>(0, 0, 1);
	CharDisplay cd(h->get_width(), h->get_height());
	h->push("averylongtext");
	h->draw(&cd);
	const std::string hbuffer =
		"+---------+---------+\n"
		"|  Move   | 1       |\n"
		"+---------+---------+\n"
		"|  White  | averylo |\n"
		"+---------+---------+\n"
		"| >Black  |         |\n"
		"+---------+---------+\n";
	EXPECT_EQ(hbuffer, cd.get_buffer_str());

	//vertical
	std::unique_ptr<ICharMoveList> v = std::make_unique<CharMoveListV>(0, 0, 1);
	cd.set_size(v->get_width(), v->get_height());
	v->push("averylongtext");
	v->draw(&cd);

	std::string vbuffer =
		"+------+---------+---------+\n"
		"| Move |  White  | >Black  |\n"
		"+------+---------+---------+\n"
		"| 1    | averylo |         |\n"
		"+------+---------+---------+\n";
	EXPECT_EQ(vbuffer, cd.get_buffer_str());
}

TEST(CharMoveList, PushPageTurn) {
	//horizontal
	std::unique_ptr<ICharMoveList> h = std::make_unique<CharMoveListH>(0, 0, 1);
	CharDisplay cd(h->get_width(), h->get_height());
	h->push("1w");
	h->push("1w");
	h->push("2w");
	h->draw(&cd);
	const std::string hbuffer =
		"+---------+---------+\n"
		"|  Move   | 2       |\n"
		"+---------+---------+\n"
		"|  White  | 2w      |\n"
		"+---------+---------+\n"
		"| >Black  |         |\n"
		"+---------+---------+\n";
	EXPECT_EQ(hbuffer, cd.get_buffer_str());

	//vertical
	std::unique_ptr<ICharMoveList> v = std::make_unique<CharMoveListV>(0, 0, 1);
	cd.set_size(v->get_width(), v->get_height());
	v->push("1w");
	v->push("1w");
	v->push("2w");
	v->draw(&cd);

	std::string vbuffer =
		"+------+---------+---------+\n"
		"| Move |  White  | >Black  |\n"
		"+------+---------+---------+\n"
		"| 2    | 2w      |         |\n"
		"+------+---------+---------+\n";
	EXPECT_EQ(vbuffer, cd.get_buffer_str());
}

TEST(CharMoveList, PushDraw) {
	//horizontal
	std::unique_ptr<ICharMoveList> h = std::make_unique<CharMoveListH>(0, 0, 1);
	CharDisplay cd(h->get_width(), h->get_height(), '?');
	h->draw_push(&cd, "averylongtext");
	const std::string hbuffer =
		"?????????????????????\n"
		"?????????????????????\n"
		"?????????????????????\n"
		"?? ?????????averylo??\n"
		"?????????????????????\n"
		"??>??????????????????\n"
		"?????????????????????\n";
	EXPECT_EQ(hbuffer, cd.get_buffer_str());

	//vertical
	std::unique_ptr<ICharMoveList> v = std::make_unique<CharMoveListV>(0, 0, 1);
	cd.set_size(v->get_width(), v->get_height());
	v->draw_push(&cd, "averylongtext");

	std::string vbuffer =
		"????????????????????????????\n"
		"????????? ?????????>????????\n"
		"????????????????????????????\n"
		"?????????averylo????????????\n"
		"????????????????????????????\n";
	EXPECT_EQ(vbuffer, cd.get_buffer_str());
}



TEST(CharMoveList, PopDraw) {
	//horizontal
	std::unique_ptr<ICharMoveList> h = std::make_unique<CharMoveListH>(0, 0, 1);
	CharDisplay cd(h->get_width(), h->get_height(), '?');
	h->push("1w");
	h->push("1b");
	h->draw_pop(&cd);
	const std::string hbuffer =
		"?????????????????????\n"
		"?????????????????????\n"
		"?????????????????????\n"
		"?? ??????????????????\n"
		"?????????????????????\n"
		"??>?????????       ??\n"
		"?????????????????????\n";
	EXPECT_EQ(hbuffer, cd.get_buffer_str());

	std::unique_ptr<ICharMoveList> v = std::make_unique<CharMoveListV>(0, 0, 1);
	cd.set_size(v->get_width(), v->get_height());
	v->push("1w");
	v->push("1b");
	v->draw_pop(&cd);

	std::string vbuffer =
		"????????????????????????????\n"
		"????????? ?????????>????????\n"
		"????????????????????????????\n"
		"???????????????????       ??\n"
		"????????????????????????????\n";
	EXPECT_EQ(vbuffer, cd.get_buffer_str());
}

TEST(CharMoveList, PopPageTurn) {
	//horizontal
	std::unique_ptr<ICharMoveList> h = std::make_unique<CharMoveListH>(0, 0, 1);
	CharDisplay cd(h->get_width(), h->get_height(), '?');
	h->push("1w");
	h->push("1b");
	h->push("2w"); //page++
	h->pop();
	h->draw_pop(&cd); //page--
	const std::string hbuffer =
		"+---------+---------+\n"
		"|  Move   | 1       |\n"
		"+---------+---------+\n"
		"|  White  | 1w      |\n"
		"+---------+---------+\n"
		"| >Black  |         |\n"
		"+---------+---------+\n";
	EXPECT_EQ(hbuffer, cd.get_buffer_str());

	std::unique_ptr<ICharMoveList> v = std::make_unique<CharMoveListV>(0, 0, 1);
	cd.set_size(v->get_width(), v->get_height());
	v->push("1w");
	v->push("1b");
	v->push("2b"); //page++
	v->pop();
	v->draw_pop(&cd); //page--

	std::string vbuffer =
		"+------+---------+---------+\n"
		"| Move |  White  | >Black  |\n"
		"+------+---------+---------+\n"
		"| 1    | 1w      |         |\n"
		"+------+---------+---------+\n";
	EXPECT_EQ(vbuffer, cd.get_buffer_str());
}