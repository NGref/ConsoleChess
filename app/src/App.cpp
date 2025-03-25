#include "App.h"

#include <iostream>
#include <sstream>

COMMAND_TYPE CHAR_TO_CMD_TYPE(const char c)
{
	switch (c) {
	case 'h': return COMMAND_TYPE::HELP;
	case 'q': return COMMAND_TYPE::QUIT;
	case 'n': return COMMAND_TYPE::NEW_GAME;
	case 'm': return COMMAND_TYPE::MOVE;
	case 'f': return COMMAND_TYPE::FIND;
	case 's': return COMMAND_TYPE::SAVE;
	case 'u': return COMMAND_TYPE::UNDO;
	case 'r': return COMMAND_TYPE::RESIZE;
	default: return COMMAND_TYPE::UNKNOWN;
	}
}

App::App() :
    m_game(nullptr), m_display(nullptr), m_board(nullptr), m_board_border(nullptr), m_movelist_h(nullptr), m_movelist_v(nullptr), m_iobox(nullptr), m_quit(false),
	m_active_mlist(nullptr), m_size(BoardSize::SMALL)
{
    m_game = std::make_unique<Game>();
    if (!m_game->get_init_ok()) {
        std::cout << "Game failed to initialize" << std::endl;
        m_quit = true;
        return;
    }
    // Init widgets
    m_board = std::make_unique<CharBoard>(1, 1, m_size, "../res", false, true, true);
	m_board_border = std::make_unique<CharBorder>(0,0,m_board->get_width()+2, m_board->get_height()+2);
	m_movelist_h = std::make_unique<CharMoveListH>(m_board_border->get_width(), 0, 4);
	m_active_mlist = m_movelist_h.get();
    m_iobox = std::make_unique<CharMessageBox>(m_board_border->get_width(), m_movelist_h->get_height(), m_movelist_h->get_width(), 3, 5);

	m_movelist_v = std::make_unique<CharMoveListV>(m_board_border->get_width(), 1, 4);

	m_display = std::make_unique<CharDisplay>(m_board_border->get_width() + m_movelist_h->get_width(), m_board_border->get_height(), '.');

    // Initial draw call
    m_board->draw(m_display.get(), m_game->get_all_tiles_pos());
	m_board_border->draw(m_display.get());
    m_movelist_h->draw(m_display.get());
    m_iobox->push("Chess v2.2. type a move (uci) or -h for a list of commands");
    m_iobox->draw(m_display.get());
    m_display->render();
}

void App::run()
{
    std::string cmd;
    while (!m_quit) {
		//move cursor to input location
		std::cout << "\33[1A"; // 1 up
		std::cout << "\33[" << m_iobox->get_xpos() << "C"; //right to iobox
		std::cout << '>';
        std::getline(std::cin, cmd);
        exec_cmd(cmd);
		m_display->render();
    }
    return;
}

void App::exec_cmd(const std::string& line)
{
	m_iobox->push(line);
	Command cmd = parse_command(line);
	switch (cmd.type) {

	case COMMAND_TYPE::UNKNOWN:
		m_iobox->push("Invalid Command");
		m_iobox->draw(m_display.get());
		break;

	case COMMAND_TYPE::QUIT:
		m_quit = true;
		break;

	case COMMAND_TYPE::NEW_GAME:
		exec_cmd_new_game(cmd.arg);
		break;

	case COMMAND_TYPE::MOVE:
		exec_cmd_move(cmd.arg);
		break;

	case COMMAND_TYPE::FIND:
		exec_cmd_find(cmd.arg);
		break;

	case COMMAND_TYPE::SAVE:
		exec_cmd_save(cmd.arg);
		break;
	case COMMAND_TYPE::UNDO:
		exec_cmd_undo();
		break;

	case COMMAND_TYPE::HELP:
		exec_cmd_help(cmd.arg);
		break;

	case COMMAND_TYPE::RESIZE:
		exec_cmd_resize(cmd.arg);
		break;

	default:
		m_iobox->push("Invalid Command");
		m_iobox->draw(m_display.get());
		break;
	}
	return;
}

void App::exec_cmd_move(const std::string& arg)
{
	if (m_game->get_game_has_ended()) {
		m_iobox->push("Game has ended! Start a new game with -n");
		return;
	}

	GameState gs = m_game->move(arg);
	switch (gs)
	{
	case GameState::INVALID_MOVE:
		m_iobox->push("Invalid move: " + arg);
		break;

	case GameState::VALID_MOVE:
		m_board->draw_delta(m_display.get(), m_game->get_new_tiles_pos());
		m_active_mlist->draw_push(m_display.get(), m_game->get_last_move_str());
		break;

	case GameState::GAME_HAS_ENDED:
		m_board->draw_delta(m_display.get(), m_game->get_new_tiles_pos());
		m_active_mlist->draw_push(m_display.get(), m_game->get_last_move_str());
		push_game_end_message(m_game->get_ending_game_state());
		break;
	}
	m_iobox->draw(m_display.get());
	return;
}

void App::exec_cmd_new_game(const std::string& arg)
{
	m_active_mlist->clear();
	m_iobox->clear();
	
	m_game->new_game(arg);
	if (m_game->get_init_ok()) {
		m_iobox->push("Successfully created new game!");
		m_board->draw(m_display.get(),m_game->get_all_tiles_pos());
	}
	else {
		m_iobox->push("Creating new game failed!");
	}
	if (m_game->get_active_color().IsBlack()) {
		m_active_mlist->push("");
	}
	m_active_mlist->draw(m_display.get());
	m_iobox->draw(m_display.get());
	return;
}

void App::exec_cmd_undo()
{
	const std::vector<TileP> v = m_game->get_reverse_new_tiles_pos();
	if (v.empty()) {
		m_iobox->push("Undo not available!");
		m_iobox->draw(m_display.get());
		return;
	}
	m_game->undo();
	m_board->draw_delta(m_display.get(), v);
	m_active_mlist->draw_pop(m_display.get());
	m_iobox->push("undo successful!");
	m_iobox->draw(m_display.get());
}

void App::exec_cmd_find(const std::string& arg)
{
	if (arg.size() == 0) {
		const std::vector<GameMove>& moves = m_game->get_possible_moves();
		if (moves.size() > 0) {
			std::ostringstream ss;
			for (int i = 0; i < moves.size() - 1; i++) {
				const Position from = bindex_to_position(moves[i].from);
				const Position to = bindex_to_position(moves[i].to);
				const Piece promo = moves[i].promotion;
				ss << 'a' + from.x << '1' + from.y << 'a' + to.x << '1' + to.y;
				if (promo != Piece::EMPTY) ss << '=' << piece_to_char(promo);
				ss << ", ";
			}
			const Position from = bindex_to_position(moves.back().from);
			const Position to = bindex_to_position(moves.back().to);
			const Piece promo = moves.back().promotion;
			ss << 'a' + from.x << '1' + from.y << 'a' + to.x << '1' + to.y << ", ";
			if (promo != Piece::EMPTY) ss << '=' << piece_to_char(promo);
			m_iobox->push(ss.str());
		}
	}
	else if (arg.size() == 2) {
		Position from{ arg[0] - 'a', arg[1] - '1' };
		const std::vector<Position>& vto = m_game->get_possible_moves_pos(from);
		if (vto.size() > 0) {
			std::ostringstream ss;
			for (int i = 0; i < vto.size() - 1; i++) {
				ss << char('a' + from.x) << char('1' + from.y) << char('a' + vto[i].x) << char('1' + vto[i].y) << ", ";
			}
			ss << char('a' + from.x) << char('1' + from.y) << char('a' + vto.back().x) << char('1' + vto.back().y);
			m_iobox->push(ss.str());
		}
	}
	else m_iobox->push("not a valid position to search!");

	m_iobox->draw(m_display.get());
	return;
}

void App::exec_cmd_save(const std::string& fname)
{
	std::ofstream file(fname);
	if (!file.is_open()) {
		m_iobox->push("Could not open file " + fname);
		m_iobox->draw(m_display.get());
		return;
	}
	//TODO
	m_iobox->push("Not implemented!");
	file.close();
	m_iobox->draw(m_display.get());
	return;
}

void App::exec_cmd_help(const std::string& arg)
{
	if (arg.size() == 0) {
		m_iobox->push("Commands: -m move,-u undo, -f find, -n new game, -s save, -q quit");
		m_iobox->push("-h <Command> for detailed help");
		m_iobox->draw(m_display.get());
	}
	else if (arg.size() == 1) {
		COMMAND_TYPE ct = CHAR_TO_CMD_TYPE(arg[0]);
		switch (ct)
		{
		case COMMAND_TYPE::UNKNOWN:
			m_iobox->push("Invalid Command");
			break;

		case COMMAND_TYPE::QUIT:
			m_iobox->push("-q: quit the application");
			break;

		case COMMAND_TYPE::NEW_GAME:
			m_iobox->push("-n [<fen>]: creates a new game");
			break;

		case COMMAND_TYPE::MOVE:
			m_iobox->push("[-m ]<uci>: enter a move in uci format. Command prefix is optional");
			break;

		case COMMAND_TYPE::FIND:
			m_iobox->push("-f <pos>: get all possible moves for a piece on pos");
			break;

		case COMMAND_TYPE::SAVE:
			m_iobox->push("-s <fname>: saves game to file");
			break;

		case COMMAND_TYPE::HELP:
			m_iobox->push("-h <cmd>: shows help message");
			break;
		case COMMAND_TYPE::RESIZE:
			m_iobox->push("-r <size>: resizes the game. size={s,m,l}");
			break;
		default:
			m_iobox->push("Invalid Command");
			break;
		}
		m_iobox->draw(m_display.get());
	}
	else {
		m_iobox->push("Invalid Command");
		m_iobox->draw(m_display.get());
	}
}

void App::exec_cmd_resize(const std::string& arg)
{
	if (arg.size() == 0) return;
	BoardSize board_size;
	const char board_size_char = arg[0];
	if (board_size_char == 's' || board_size_char == 'S') board_size = BoardSize::SMALL;
	else if (board_size_char == 'm' || board_size_char == 'M') board_size = BoardSize::MEDIUM;
	else if (board_size_char == 'l' || board_size_char == 'L') board_size = BoardSize::LARGE;
	else return;
	if (m_size == board_size) return;
	m_board->set_size(board_size);
	if (board_size == BoardSize::SMALL) {
		m_board->set_grid(false, true);
		m_board->set_xpos(1);
		m_board->set_ypos(1);
		m_board_border->set_xpos(0);
		m_board_border->set_ypos(0);

		m_active_mlist = m_movelist_h.get();
		m_movelist_h->set_move_list(m_movelist_v->get_move_list());

		m_iobox->set_height(m_board_border->get_height() - m_active_mlist->get_height() - 1);
		m_iobox->set_xpos(m_board_border->get_width());
		m_display->set_size(m_board_border->get_width() + m_active_mlist->get_width(), m_board_border->get_height());
	}
	else {
		m_board->set_grid(true, true);
		m_board->set_xpos(0);
		m_board->set_ypos(0);

		m_board_border->set_xpos(-100);
		m_board_border->set_ypos(-100);

		if (m_size == BoardSize::SMALL) {
			m_active_mlist = m_movelist_v.get();
			m_movelist_v->set_move_list(m_movelist_h->get_move_list());
		}
		m_movelist_v->set_xpos(m_board->get_width());
		m_movelist_v->set_number_of_moves(m_board->get_height() / 3);

		m_iobox->set_height(m_board->get_height() - m_active_mlist->get_height() - 2);
		m_iobox->set_xpos(m_board->get_width());
		m_display->set_size(m_board->get_width() + m_active_mlist->get_width(), m_board->get_height());
	}

	
	m_iobox->set_ypos(m_active_mlist->get_ypos() +  m_active_mlist->get_height());
	m_iobox->set_width(m_active_mlist->get_width());
	
	m_size = board_size;
	
	// Resize draw call
	m_display->flush();

	m_board->draw(m_display.get(), m_game->get_all_tiles_pos());
	m_board_border->draw(m_display.get());
	m_active_mlist->draw(m_display.get());
	m_iobox->push("Resized!");
	m_iobox->draw(m_display.get());

	
	m_display->render();
}

void App::push_game_end_message(GameEndState ges)
{
	switch (ges)
	{
	case GameEndState::WHITE_WIN_CM:
		m_iobox->push("Checkmate! White wins.");
		break;
	case GameEndState::WHITE_WIN_FF:
		m_iobox->push("Black forfeit! White wins.");
		break;
	case GameEndState::WHITE_WIN_TIME:
		m_iobox->push("Black lost on time! White wins.");
		break;
	case GameEndState::WHITE_WIN_REP_INV_MOVE:
		m_iobox->push("Black made too many invalid moves! White wins");
		break;
	case GameEndState::BLACK_WIN_CM:
		m_iobox->push("Checkmate! Black wins.");
		break;
	case GameEndState::BLACK_WIN_FF:
		m_iobox->push("White forfeit! Black wins.");
		break;
	case GameEndState::BLACK_WIN_TIME:
		m_iobox->push("White lost on time! Black wins.");
		break;
	case GameEndState::BLACK_WIN_REP_INV_MOVE:
		m_iobox->push("Black made too many invalid moves! Black wins");
		break;
	case GameEndState::END_DRAW_STALEMATE:
		m_iobox->push("Stalemate! Draw");
		break;
	case GameEndState::END_DRAW_OFFER:
		m_iobox->push("Draw offer accepted");
		break;
	case GameEndState::END_DRAW_3FOLD:
		m_iobox->push("Draw via three fold repetition!");
		break;
	case GameEndState::END_DRAW_MAX_TURNS:
		m_iobox->push("Draw - max number of turns reached!");
		break;
	case GameEndState::END_DRAW_MAX_HALF_TURNS:
		m_iobox->push("Draw - max half turns reached");
		break;
	default:
		break;
	}
}

Command parse_command(const std::string& line)
{
	Command cmd;
	
	if (line[0] == '-') {
		cmd.type = CHAR_TO_CMD_TYPE(line[1]);
		try {
			cmd.arg = line.substr(3, line.size() - 3);
		}
		catch (std::out_of_range) {
			cmd.arg = "";
		}
		
	}
	else if (line[0] == '?') {
		cmd.type = COMMAND_TYPE::FIND;
		cmd.arg = line.substr(1, line.size() - 1);
	}
	else {
		cmd.type = COMMAND_TYPE::MOVE;
		cmd.arg = line;
	}
	return cmd;
}