#pragma once

#include "engine/Game.h"

#include "ui/CharDisplay.h"
#include "ui/CharBoard.h"
#include "ui/CharMoveList.h"
#include "ui/CharBorder.h"
#include "ui/CharMessageBox.h"

#include <memory>

enum class COMMAND_TYPE {
    UNKNOWN = -1,
    QUIT = 0,
    NEW_GAME,
    MOVE,
    UNDO,
    FIND,
    SAVE,
    RESIZE,
    HELP
};

struct Command {
    COMMAND_TYPE type = COMMAND_TYPE::UNKNOWN;
    std::string arg = "";
};

Command parse_command(const std::string& str);

class App
{
public:
	App();
	void run();

private:
    void exec_cmd(const std::string& cmd);
    
    void exec_cmd_move(const std::string& arg);
    void exec_cmd_new_game(const std::string& arg);
    void exec_cmd_undo();
    void exec_cmd_find(const std::string& arg);
    void exec_cmd_save(const std::string& arg);
    void exec_cmd_help(const std::string& arg);
    void exec_cmd_resize(const std::string& arg);

    void push_game_end_message(GameEndState state);
private:
    std::unique_ptr<IGame> m_game;
    std::unique_ptr<CharDisplay> m_display;
    std::unique_ptr<CharBoard> m_board;
    std::unique_ptr<CharBorder> m_board_border;
    std::unique_ptr<ICharMoveList> m_movelist_h;
    std::unique_ptr<ICharMoveList> m_movelist_v;
    std::unique_ptr<CharMessageBox> m_iobox;
    ICharMoveList* m_active_mlist;
    BoardSize m_size;
    bool m_quit;
};

