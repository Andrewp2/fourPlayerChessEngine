#pragma once

#include <sys/types.h>
#include <array>
#include <string>
#include <iostream>
#include <vector>
#include <utility>
#include <map>

static const std::map<char, unsigned char> PIECE_LETTER_TO_NUMBER = { {'K',1}, {'Q', 2}, {'P', 3}, {'R', 4}, {'B', 5}, {'N', 6} };
static const std::map<char, unsigned char> TEAM_COLOR_TO_NUMBER = { {'R', 0},{'B', 1},{'Y', 2},{'G', 3} };
static const std::map<unsigned char, char> TEAM_NUMBER_TO_COLOR = { {0, 'R'},{1, 'B'},{2, 'Y'},{3, 'G'} };
constexpr unsigned char RED_K = 1;
constexpr unsigned char RED_Q = 2;
constexpr unsigned char RED_P = 3;
constexpr unsigned char RED_R = 4;
constexpr unsigned char RED_B = 5;
constexpr unsigned char RED_N = 6;

constexpr unsigned char BLU_K = 7;
constexpr unsigned char BLU_Q = 8;
constexpr unsigned char BLU_P = 9;
constexpr unsigned char BLU_R = 10;
constexpr unsigned char BLU_B = 11;
constexpr unsigned char BLU_N = 12;

constexpr unsigned char YEL_K = 13;
constexpr unsigned char YEL_Q = 14;
constexpr unsigned char YEL_P = 15;
constexpr unsigned char YEL_R = 16;
constexpr unsigned char YEL_B = 17;
constexpr unsigned char YEL_N = 18;

constexpr unsigned char GRE_K = 19;
constexpr unsigned char GRE_Q = 20;
constexpr unsigned char GRE_P = 21;
constexpr unsigned char GRE_R = 22;
constexpr unsigned char GRE_B = 23;
constexpr unsigned char GRE_N = 24;

constexpr unsigned char EMPTY = 100;
constexpr unsigned char CORNR = 101;

constexpr unsigned char NUM_SQUARES = 196;

constexpr std::pair<char, char> team1 = std::pair<char, char>('R', 'Y');
constexpr std::pair<char, char> team2 = std::pair<char, char>('B', 'G');

// K, Q, P, R, B, N * 4 = 24 pieces, plus 1 for empty square, plus 1 for non-accessible square
struct Square
{
	unsigned char pieceType;
};

bool is_number(const std::string& s);
char square_to_letter(Square s);

unsigned char coordinate_to_index(std::string coordinate);

std::string index_to_coordinate(unsigned char index);

class Board
{
public:

	void initialize_board();

	void make_move(std::string from, std::string to);

	std::vector<std::pair<unsigned char, unsigned char>> generate_valid_moves(char color);
	std::vector<std::pair<unsigned char, unsigned char>> generate_valid_moves(char color, char pieceType);
	std::vector<unsigned char> generate_possible_moves(unsigned char location, char color, char pieceType);


	std::vector<std::pair<unsigned char, unsigned char>> generate_valid_moves_ignore_check(char color);
	std::vector<std::pair<unsigned char, unsigned char>> generate_valid_moves_ignore_check(char color, char pieceType);
	std::vector<unsigned char> generate_possible_moves_ignore_check(unsigned char location, char color, char pieceType);


	bool left_in_check_and_cant_take(char color, std::pair<unsigned char, unsigned char> move);

	int check_can_move(
		unsigned char location,
		char color,
		const std::pair<unsigned char, unsigned char>& friendly1,
		const std::pair<unsigned char, unsigned char>& friendly2,
		const std::pair<unsigned char, unsigned char>& enemy1,
		const std::pair<unsigned char, unsigned char>& enemy2,
		std::vector<unsigned char>& possibleMoves,
		unsigned char deltaX,
		unsigned char deltaY
	);

	unsigned char relative_square(unsigned char location, char color, unsigned char deltaX, unsigned char deltaY);

	bool pawn_on_starting_square(unsigned char location, char color);

	bool contains_piece_in_bounds(unsigned char location, std::pair<unsigned char, unsigned char> bound1, std::pair<unsigned char, unsigned char> bound2);
	bool enemy_can_attack(unsigned char location, const std::vector<std::pair<unsigned char, unsigned char>>& enemyMoves);
	
	int evaluate(int depth);
	int evaluate();

	int find_number_of_piece(char color, unsigned char pieceType);
	int find_difference_of_piece(std::pair<char, char> friendlyTeam, std::pair<char, char> enemyTeam, char pieceType);

	bool in_check();

	void parse_fen(std::vector<std::string> fen);

private:
	std::vector<std::pair<bool, bool>> castlingRights = 
		{std::pair<bool, bool>(true, true), std::pair<bool, bool>(true, true), std::pair<bool, bool>(true, true), std::pair<bool, bool>(true, true) };
	int alpha_beta_max(int alpha, int beta, int depthLeft);
	int alpha_beta_min(int alpha, int beta, int depthLeft);
	int ply = 0;
	char sideToMove = 'R';
	friend std::ostream& operator<<(std::ostream& o, const Board& b);
	friend std::istream& operator>>(std::istream& input, Board& b);
	std::array<Square, NUM_SQUARES> squares;
};


