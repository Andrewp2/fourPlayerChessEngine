#include "board.h"

#include <sstream>
#include <regex>

long nodesSearched = 0;

unsigned char coordinate_to_index(std::string coordinate)
{
	if (coordinate.length() < 2) {
		return 255;
	}
	if (coordinate.length() > 3) {
		return 255;
	}
	char letter = coordinate[0];
	int number = 14 - std::stoi(coordinate.substr(1));
	if (letter < 'a' || letter > 'n') {
		return 255;
	}
	if (number < 0 || number > 13) {
		return 255;
	}
	return number * 14 + (letter - 'a');
}

std::string index_to_coordinate(unsigned char index)
{
	if (index > NUM_SQUARES || index < 0) {
		return "error";
	}
	int row = 14 - ((index / 14));
	int col = index % 14;
	char letter = 'a' + col;
	return std::string(1, letter) + std::to_string(row);
}

void Board::initialize_board()
{

	squares =
	{
		CORNR, CORNR, CORNR, YEL_R, YEL_N, YEL_B, YEL_K, YEL_Q, YEL_B, YEL_N, YEL_R, CORNR, CORNR, CORNR,
		CORNR, CORNR, CORNR, YEL_P, YEL_P, YEL_P, YEL_P, YEL_P, YEL_P, YEL_P, YEL_P, CORNR, CORNR, CORNR,
		CORNR, CORNR, CORNR, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, CORNR, CORNR, CORNR,
		BLU_R, BLU_P, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, GRE_P, GRE_R,
		BLU_N, BLU_P, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, GRE_P, GRE_N,
		BLU_B, BLU_P, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, GRE_P, GRE_B,
		BLU_K, BLU_P, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, GRE_P, GRE_Q,
		BLU_Q, BLU_P, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, GRE_P, GRE_K,
		BLU_B, BLU_P, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, GRE_P, GRE_B,
		BLU_N, BLU_P, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, GRE_P, GRE_N,
		BLU_R, BLU_P, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, GRE_P, GRE_R,
		CORNR, CORNR, CORNR, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, CORNR, CORNR, CORNR,
		CORNR, CORNR, CORNR, RED_P, RED_P, RED_P, RED_P, RED_P, RED_P, RED_P, RED_P, CORNR, CORNR, CORNR,
		CORNR, CORNR, CORNR, RED_R, RED_N, RED_B, RED_Q, RED_K, RED_B, RED_N, RED_R, CORNR, CORNR, CORNR,
	};
}

void Board::make_move(std::string from, std::string to)
{
	unsigned char first = coordinate_to_index(from);
	unsigned char second = coordinate_to_index(to);
	//TODO: handle castling stuff
	ply++;
	sideToMove = TEAM_NUMBER_TO_COLOR.at(ply);
	this->squares[second].pieceType = this->squares[first].pieceType;
	this->squares[first].pieceType = EMPTY;
}

std::vector<std::pair<unsigned char, unsigned char>> Board::generate_valid_moves(char color)
{
	std::vector<std::pair<unsigned char, unsigned char>> moves;
	auto pawnMoves = this->generate_valid_moves(color, 'P');
	auto queenMoves = this->generate_valid_moves(color, 'Q');
	auto kingMoves = this->generate_valid_moves(color, 'K');
	auto rookMoves = this->generate_valid_moves(color, 'R');
	auto bishopMoves = this->generate_valid_moves(color, 'B');
	auto knightMoves = this->generate_valid_moves(color, 'N');
	moves.insert(moves.end(), pawnMoves.begin(), pawnMoves.end());
	moves.insert(moves.end(), queenMoves.begin(), queenMoves.end());
	moves.insert(moves.end(), kingMoves.begin(), kingMoves.end());
	moves.insert(moves.end(), rookMoves.begin(), rookMoves.end());
	moves.insert(moves.end(), bishopMoves.begin(), bishopMoves.end());
	moves.insert(moves.end(), knightMoves.begin(), knightMoves.end());
	return moves;
}

std::vector<std::pair<unsigned char, unsigned char>> Board::generate_valid_moves(char color, char pieceType)
{
	std::vector<std::pair<unsigned char, unsigned char>> moves;
	unsigned char pieceNumber = (TEAM_COLOR_TO_NUMBER.at(color) * 6) + PIECE_LETTER_TO_NUMBER.at(pieceType);
	std::vector<unsigned char> startingSquares;
	for (unsigned char i = 0; i < NUM_SQUARES; ++i) {
		if (this->squares[i].pieceType == pieceNumber) {
			startingSquares.push_back(i);
		}
	}
	for (auto ptr = startingSquares.begin(); ptr < startingSquares.end(); ++ptr) {
		std::vector<unsigned char> possibleMoves = generate_possible_moves(*ptr, color, pieceType);
		for (auto ptr2 = possibleMoves.begin(); ptr2 < possibleMoves.end(); ++ptr2) {
			moves.push_back(std::pair<unsigned char, unsigned char>(*ptr, *ptr2));
		}
	}
	for (auto iter = moves.begin(); iter < moves.end();) {
		if (left_in_check_and_cant_take(color, *iter)) {
			iter = moves.erase(iter);
		}
		else {
			++iter;
		}
	}
	return moves;
}

std::vector<unsigned char> Board::generate_possible_moves(unsigned char location, char color, char pieceType)
{
	std::vector<unsigned char> possibleMoves;
	std::pair<char, char> friendlyTeam;
	std::pair<char, char> enemyTeam;
	if (team1.first == color || team1.second == color) {
		friendlyTeam = team1;
		enemyTeam = team2;
	}
	else {
		friendlyTeam = team2;
		enemyTeam = team1;
	}
	auto const enemyTeam1Bounds = std::pair<unsigned char, unsigned char>(TEAM_COLOR_TO_NUMBER.at(enemyTeam.first) * 6 + 1, TEAM_COLOR_TO_NUMBER.at(enemyTeam.first) * 6 + 6);
	auto const enemyTeam2Bounds = std::pair<unsigned char, unsigned char>(TEAM_COLOR_TO_NUMBER.at(enemyTeam.second) * 6 + 1, TEAM_COLOR_TO_NUMBER.at(enemyTeam.second) * 6 + 6);
	auto const friendlyTeam1Bounds = std::pair<unsigned char, unsigned char>(TEAM_COLOR_TO_NUMBER.at(friendlyTeam.first) * 6 + 1, TEAM_COLOR_TO_NUMBER.at(friendlyTeam.first) * 6 + 6);
	auto const friendlyTeam2Bounds = std::pair<unsigned char, unsigned char>(TEAM_COLOR_TO_NUMBER.at(friendlyTeam.second) * 6 + 1, TEAM_COLOR_TO_NUMBER.at(friendlyTeam.second) * 6 + 6);
	if (pieceType == 'P') {
		//Can't handle differences in promotion here, deal with later
		//Also not handling enpassant yet
		//Moving forward 1 square
		auto forwardSquare = relative_square(location, color, 0, 1);
		if (!contains_piece_in_bounds(this->squares[forwardSquare].pieceType, enemyTeam1Bounds, enemyTeam2Bounds)) {
			possibleMoves.push_back(forwardSquare);
		}
		//Moving forward 2 squares
		auto forward2Square = relative_square(location, color, 0, 2);
		if (pawn_on_starting_square(location, color) &&
			!contains_piece_in_bounds(this->squares[forwardSquare].pieceType, enemyTeam1Bounds, enemyTeam2Bounds) &&
			!contains_piece_in_bounds(this->squares[forward2Square].pieceType, enemyTeam1Bounds, enemyTeam2Bounds)) {
			possibleMoves.push_back(forward2Square);
		}
		//Taking on the right and left squares
		auto rightSquare = relative_square(location, color, 1, 1);
		auto leftSquare = relative_square(location, color, -1, 1);
		auto rightSquarePiece = this->squares[rightSquare].pieceType;
		auto leftSquarePiece = this->squares[leftSquare].pieceType;
		if (contains_piece_in_bounds(rightSquarePiece, enemyTeam1Bounds, enemyTeam2Bounds)) {
			possibleMoves.push_back(rightSquare);
		}
		if (contains_piece_in_bounds(leftSquarePiece, enemyTeam1Bounds, enemyTeam2Bounds)) {
			possibleMoves.push_back(leftSquare);
		}
	}
	else if (pieceType == 'Q') {
		int result;
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, i, i);
			if (result != 1) {
				break;
			}
		}
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, i, -i);
			if (result != 1) {
				break;
			}
		}
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, -i, i);
			if (result != 1) {
				break;
			}
		}
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, -i, -i);
			if (result != 1) {
				break;
			}
		}
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, 0, i);
			if (result != 1) {
				break;
			}
		}
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, 0, -i);
			if (result != 1) {
				break;
			}
		}
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, i, 0);
			if (result != 1) {
				break;
			}
		}
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, -i, 0);
			if (result != 1) {
				break;
			}
		}
	}
	else if (pieceType == 'K') {
		std::pair<bool, bool> castling = this->castlingRights[TEAM_COLOR_TO_NUMBER.at(color)];
		//Rules for castling: 2 squares on kingside, 3 on queenside must be empty
		//The 2 squares adjacent to the king must not be under check
		//The king itself must not be checked
		//The king and rooks must have not moved before

		//stack overflow - generate_valid_moves_ignore_check calls generate_possible_moves
		std::vector<std::pair<unsigned char, unsigned char>> enemyPlayer1Moves = this->generate_valid_moves_ignore_check(enemyTeam.first);
		std::vector<std::pair<unsigned char, unsigned char>> enemyPlayer2Moves = this->generate_valid_moves_ignore_check(enemyTeam.second);
		std::vector<std::pair<unsigned char, unsigned char>> enemyMoves;
		enemyMoves.reserve(enemyPlayer1Moves.size() + enemyPlayer2Moves.size());
		enemyMoves.insert(enemyMoves.end(), enemyPlayer1Moves.begin(), enemyPlayer1Moves.end());
		enemyMoves.insert(enemyMoves.end(), enemyPlayer2Moves.begin(), enemyPlayer2Moves.end());
		if (castling.first) {
			//kingside
			//Note that even if you somehow have checked your opponent, 
			//you still cannot castle if they are hitting a square you need to use to castle.
			//An enemy player could not castle into check to prevent you from castling.
			auto bishopSquare = relative_square(location, color, 1, 0);
			auto knightSquare = relative_square(location, color, 2, 0);
			if (squares[bishopSquare].pieceType == EMPTY &&
				squares[knightSquare].pieceType == EMPTY &&
				!enemy_can_attack(bishopSquare, enemyMoves) &&
				!enemy_can_attack(knightSquare, enemyMoves)) {
				possibleMoves.push_back(knightSquare);
			}


		}
		if (castling.second) {
			auto queenSquare = relative_square(location, color, -1, 0);
			auto leftBishopSquare = relative_square(location, color, -2, 0);
			auto leftKnightSquare = relative_square(location, color, -3, 0);
			if (squares[leftBishopSquare].pieceType == EMPTY &&
				squares[leftKnightSquare].pieceType == EMPTY &&
				!enemy_can_attack(leftBishopSquare, enemyMoves) &&
				!enemy_can_attack(leftKnightSquare, enemyMoves)) {
				possibleMoves.push_back(leftKnightSquare);
			}

		}
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, 0, 1);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, 1, 1);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, 1, 0);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, -1, 0);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, -1, 1);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, -1, -1);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, 0, -1);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, 1, -1);
	}
	else if (pieceType == 'R') {
		//TODO: Castling
		int result;
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, 0, i);
			if (result != 1) {
				break;
			}
		}
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, 0, -i);
			if (result != 1) {
				break;
			}
		}
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, i, 0);
			if (result != 1) {
				break;
			}
		}
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, -i, 0);
			if (result != 1) {
				break;
			}
		}
	}
	else if (pieceType == 'B') {
		int result;
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, i, i);
			if (result != 1) {
				break;
			}
		}
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, i, -i);
			if (result != 1) {
				break;
			}
		}
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, -i, i);
			if (result != 1) {
				break;
			}
		}
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, -i, -i);
			if (result != 1) {
				break;
			}
		}
	}
	else if (pieceType == 'N') {
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, 1, 2);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, 2, 1);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, -1, 2);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, -2, 1);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, 1, -2);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, 2, -1);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, -1, -2);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, -2, -1);
	}
	return possibleMoves;
}

std::vector<std::pair<unsigned char, unsigned char>> Board::generate_valid_moves_ignore_check(char color)
{
	std::vector<std::pair<unsigned char, unsigned char>> moves;
	auto pawnMoves = this->generate_valid_moves_ignore_check(color, 'P');
	auto queenMoves = this->generate_valid_moves_ignore_check(color, 'Q');
	auto kingMoves = this->generate_valid_moves_ignore_check(color, 'K');
	auto rookMoves = this->generate_valid_moves_ignore_check(color, 'R');
	auto bishopMoves = this->generate_valid_moves_ignore_check(color, 'B');
	auto knightMoves = this->generate_valid_moves_ignore_check(color, 'N');
	moves.insert(moves.end(), pawnMoves.begin(), pawnMoves.end());
	moves.insert(moves.end(), queenMoves.begin(), queenMoves.end());
	moves.insert(moves.end(), kingMoves.begin(), kingMoves.end());
	moves.insert(moves.end(), rookMoves.begin(), rookMoves.end());
	moves.insert(moves.end(), bishopMoves.begin(), bishopMoves.end());
	moves.insert(moves.end(), knightMoves.begin(), knightMoves.end());
	return moves;
}

std::vector<std::pair<unsigned char, unsigned char>> Board::generate_valid_moves_ignore_check(char color, char pieceType)
{
	std::vector<std::pair<unsigned char, unsigned char>> moves;
	unsigned char pieceNumber = (TEAM_COLOR_TO_NUMBER.at(color) * 6) + PIECE_LETTER_TO_NUMBER.at(pieceType);
	std::vector<unsigned char> startingSquares;
	for (unsigned char i = 0; i < NUM_SQUARES; ++i) {
		if (this->squares[i].pieceType == pieceNumber) {
			startingSquares.push_back(i);
		}
	}
	for (auto ptr = startingSquares.begin(); ptr < startingSquares.end(); ++ptr) {
		std::vector<unsigned char> possibleMoves = generate_possible_moves_ignore_check(*ptr, color, pieceType);
		for (auto ptr2 = possibleMoves.begin(); ptr2 < possibleMoves.end(); ++ptr2) {
			moves.push_back(std::pair<unsigned char, unsigned char>(*ptr, *ptr2));
		}
	}
	return moves;
}

std::vector<unsigned char> Board::generate_possible_moves_ignore_check(unsigned char location, char color, char pieceType)
{
	std::vector<unsigned char> possibleMoves;
	std::pair<char, char> friendlyTeam;
	std::pair<char, char> enemyTeam;
	if (team1.first == color || team1.second == color) {
		friendlyTeam = team1;
		enemyTeam = team2;
	}
	else {
		friendlyTeam = team2;
		enemyTeam = team1;
	}
	auto const enemyTeam1Bounds = std::pair<unsigned char, unsigned char>(TEAM_COLOR_TO_NUMBER.at(enemyTeam.first) * 6 + 1, TEAM_COLOR_TO_NUMBER.at(enemyTeam.first) * 6 + 6);
	auto const enemyTeam2Bounds = std::pair<unsigned char, unsigned char>(TEAM_COLOR_TO_NUMBER.at(enemyTeam.second) * 6 + 1, TEAM_COLOR_TO_NUMBER.at(enemyTeam.second) * 6 + 6);
	auto const friendlyTeam1Bounds = std::pair<unsigned char, unsigned char>(TEAM_COLOR_TO_NUMBER.at(friendlyTeam.first) * 6 + 1, TEAM_COLOR_TO_NUMBER.at(friendlyTeam.first) * 6 + 6);
	auto const friendlyTeam2Bounds = std::pair<unsigned char, unsigned char>(TEAM_COLOR_TO_NUMBER.at(friendlyTeam.second) * 6 + 1, TEAM_COLOR_TO_NUMBER.at(friendlyTeam.second) * 6 + 6);
	if (pieceType == 'P') {
		//Can't handle differences in promotion here, deal with later
		//Also not handling enpassant yet
		//Moving forward 1 square
		auto forwardSquare = relative_square(location, color, 0, 1);
		if (!contains_piece_in_bounds(this->squares[forwardSquare].pieceType, enemyTeam1Bounds, enemyTeam2Bounds)) {
			possibleMoves.push_back(forwardSquare);
		}
		//Moving forward 2 squares
		auto forward2Square = relative_square(location, color, 0, 2);
		if (pawn_on_starting_square(location, color) &&
			!contains_piece_in_bounds(this->squares[forwardSquare].pieceType, enemyTeam1Bounds, enemyTeam2Bounds) &&
			!contains_piece_in_bounds(this->squares[forward2Square].pieceType, enemyTeam1Bounds, enemyTeam2Bounds)) {
			possibleMoves.push_back(forward2Square);
		}
		//Taking on the right and left squares
		auto rightSquare = relative_square(location, color, 1, 1);
		auto leftSquare = relative_square(location, color, -1, 1);
		auto rightSquarePiece = this->squares[rightSquare].pieceType;
		auto leftSquarePiece = this->squares[leftSquare].pieceType;
		if (contains_piece_in_bounds(rightSquarePiece, enemyTeam1Bounds, enemyTeam2Bounds)) {
			possibleMoves.push_back(rightSquare);
		}
		if (contains_piece_in_bounds(leftSquarePiece, enemyTeam1Bounds, enemyTeam2Bounds)) {
			possibleMoves.push_back(leftSquare);
		}
	}
	else if (pieceType == 'Q') {
		int result;
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, i, i);
			if (result != 1) {
				break;
			}
		}
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, i, -i);
			if (result != 1) {
				break;
			}
		}
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, -i, i);
			if (result != 1) {
				break;
			}
		}
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, -i, -i);
			if (result != 1) {
				break;
			}
		}
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, 0, i);
			if (result != 1) {
				break;
			}
		}
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, 0, -i);
			if (result != 1) {
				break;
			}
		}
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, i, 0);
			if (result != 1) {
				break;
			}
		}
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, -i, 0);
			if (result != 1) {
				break;
			}
		}
	}
	else if (pieceType == 'K') {
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, 0, 1);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, 1, 1);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, 1, 0);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, -1, 0);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, -1, 1);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, -1, -1);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, 0, -1);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, 1, -1);
	}
	else if (pieceType == 'R') {
		//TODO: Castling
		int result;
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, 0, i);
			if (result != 1) {
				break;
			}
		}
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, 0, -i);
			if (result != 1) {
				break;
			}
		}
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, i, 0);
			if (result != 1) {
				break;
			}
		}
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, -i, 0);
			if (result != 1) {
				break;
			}
		}
	}
	else if (pieceType == 'B') {
		int result;
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, i, i);
			if (result != 1) {
				break;
			}
		}
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, i, -i);
			if (result != 1) {
				break;
			}
		}
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, -i, i);
			if (result != 1) {
				break;
			}
		}
		for (int i = 1; i < 14; ++i) {
			result = check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, -i, -i);
			if (result != 1) {
				break;
			}
		}
	}
	else if (pieceType == 'N') {
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, 1, 2);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, 2, 1);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, -1, 2);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, -2, 1);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, 1, -2);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, 2, -1);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, -1, -2);
		check_can_move(location, color, friendlyTeam1Bounds, friendlyTeam2Bounds, enemyTeam1Bounds, enemyTeam2Bounds, possibleMoves, -2, -1);
	}
	return possibleMoves;
}

bool Board::left_in_check_and_cant_take(char color, std::pair<unsigned char, unsigned char> move) {
	std::pair<char, char> friendlyTeam;
	std::pair<char, char> enemyTeam;
	if (team1.first == color || team1.second == color) {
		friendlyTeam = team1;
		enemyTeam = team2;
	}
	else {
		friendlyTeam = team2;
		enemyTeam = team1;
	}
	Board check;
	std::array<Square, NUM_SQUARES> checkSquares = squares;
	check.squares = checkSquares;
	Square previousOccupier = check.squares[move.second];
	check.squares[move.second] = check.squares[move.first];
	Square empty;
	empty.pieceType = EMPTY;
	check.squares[move.first] = empty;
	//Note you CAN take a king even IF it puts you into check.
	std::vector<std::pair<unsigned char, unsigned char>> enemyPlayer1Moves = check.generate_valid_moves_ignore_check(enemyTeam.first);
	std::vector<std::pair<unsigned char, unsigned char>> enemyPlayer2Moves = check.generate_valid_moves_ignore_check(enemyTeam.second);
	bool player1Attacking = false;
	bool player2Attacking = false;
	unsigned char kingLocation;
	for (int i = 0; i < NUM_SQUARES; ++i) {
		if (check.squares[i].pieceType == TEAM_COLOR_TO_NUMBER.at(color) * 6 + PIECE_LETTER_TO_NUMBER.at('K')) {
			kingLocation = i;
		}
	}
	for (auto ptr = enemyPlayer1Moves.begin(); ptr < enemyPlayer1Moves.end(); ++ptr) {
		if (ptr->second == kingLocation) {
			player1Attacking = true;
		}
	}
	for (auto ptr = enemyPlayer2Moves.begin(); ptr < enemyPlayer2Moves.end(); ++ptr) {
		if (ptr->second == kingLocation) {
			player2Attacking = true;
		}
	}
	//Check if we're taking the king of the same color that is attacking us.
	if (previousOccupier.pieceType == TEAM_COLOR_TO_NUMBER.at(enemyTeam.first) * 6 + PIECE_LETTER_TO_NUMBER.at('K')) {
		return player2Attacking;
	}
	else if (previousOccupier.pieceType == TEAM_COLOR_TO_NUMBER.at(enemyTeam.second) * 6 + PIECE_LETTER_TO_NUMBER.at('K')) {
		return player1Attacking;
	}
	else {
		return player1Attacking || player2Attacking;
	}
}

//If the square contained nothing: 1
//If the square is OOB: 2
//If the square is a corner: 3
//If the square contains a friendly piece: 4
//If the square contains an enemy piece: 5
int Board::check_can_move(
	unsigned char location,
	char color,
	const std::pair<unsigned char, unsigned char>& friendly1,
	const std::pair<unsigned char, unsigned char>& friendly2,
	const std::pair<unsigned char, unsigned char>& enemy1,
	const std::pair<unsigned char, unsigned char>& enemy2,
	std::vector<unsigned char>& possibleMoves,
	unsigned char deltaX,
	unsigned char deltaY
)
{
	auto square1 = relative_square(location, color, deltaX, deltaY);
	if (square1 == 255) {
		return 2;
	}
	if (this->squares[square1].pieceType == 101) {
		return 3;
	}
	if (contains_piece_in_bounds(this->squares[square1].pieceType, friendly1, friendly2)) {
		return 4;
	}
	possibleMoves.push_back(square1);
	if (contains_piece_in_bounds(this->squares[square1].pieceType, enemy1, enemy2)) {
		return 5;
	}
	return 1;
}

unsigned char Board::relative_square(unsigned char location, char color, unsigned char deltaX, unsigned char deltaY) {
	unsigned char row = location / 14;
	unsigned char col = location % 14;
	unsigned char realRow = -1;
	unsigned char realCol = -1;
	if (color == 'R') {
		realRow = row - deltaY;
		realCol = col + deltaX;
	}
	else if (color == 'B') {
		realRow = row - deltaX;
		realCol = col + deltaY;
	}
	else if (color == 'Y') {
		realRow = row + deltaY;
		realCol = col - deltaX;
	}
	else if (color == 'G') {
		realRow = row + deltaX;
		realCol = col - deltaY;
	}
	else {
		return 255;
	}
	if (realRow < 0 || realRow >= 14 || realCol < 0 || realCol >= 14) {
		return 255;
	}
	return realRow * 14 + realCol;
}

bool Board::pawn_on_starting_square(unsigned char location, char color) {
	unsigned char row = location / 14;
	unsigned char col = location % 14;
	if (color == 'R') {
		return row == 12;
	}
	else if (color == 'B') {
		return col == 1;
	}
	else if (color == 'Y') {
		return row == 1;
	}
	else if (color == 'G') {
		return col == 12;
	}
	return false;

}

bool Board::contains_piece_in_bounds(unsigned char pieceType, std::pair<unsigned char, unsigned char> bound1, std::pair<unsigned char, unsigned char> bound2) {
	if (pieceType >= bound1.first && pieceType <= bound1.second) {
		return true;
	}
	if (pieceType >= bound2.first && pieceType <= bound2.second) {
		return true;
	}
	return false;
}

bool Board::enemy_can_attack(unsigned char location, const std::vector<std::pair<unsigned char, unsigned char>>& enemyMoves) {
	for (auto ptr = enemyMoves.begin(); ptr != enemyMoves.end(); ++ptr) {
		if (location == ptr->second) {
			return true;
		}
	}
	return false;
}

int Board::evaluate(int depth) {
	if (ply % 2 == 0) {
		return alpha_beta_max(INT_MIN, INT_MAX, depth);
	}
	else {
		return alpha_beta_min(INT_MIN, INT_MAX, depth);
	}
}

int Board::evaluate()
{
	nodesSearched++;
	std::pair<char, char> friendlyTeam;
	std::pair<char, char> enemyTeam;
	char color = TEAM_NUMBER_TO_COLOR.at(ply % 4);
	if (team1.first == color || team1.second == color) {
		friendlyTeam = team1;
		enemyTeam = team2;
	}
	else {
		friendlyTeam = team2;
		enemyTeam = team1;
	}
	//Note you CAN take a king even IF it puts you into check.

	std::vector<std::pair<unsigned char, unsigned char>> friendlyPlayer1Moves = this->generate_valid_moves(friendlyTeam.first);
	std::vector<std::pair<unsigned char, unsigned char>> friendlyPlayer2Moves = this->generate_valid_moves(friendlyTeam.second);
	std::vector<std::pair<unsigned char, unsigned char>> enemyPlayer1Moves = this->generate_valid_moves(enemyTeam.first);
	std::vector<std::pair<unsigned char, unsigned char>> enemyPlayer2Moves = this->generate_valid_moves(enemyTeam.second);
	int enemyMobilityScore = enemyPlayer1Moves.size() + enemyPlayer2Moves.size();
	int friendlyMobilityScore = friendlyPlayer1Moves.size() + friendlyPlayer2Moves.size();

	int kingFactor = 200000;
	int queenFactor = 1100;
	int pawnFactor = 100;
	int rookFactor = 500;
	int bishopFactor = 550;
	int knightFactor = 330;
	int moveFactor = 10;

	int moveDifference = friendlyMobilityScore - enemyMobilityScore;
	int kingDifference = find_difference_of_piece(friendlyTeam, enemyTeam, 'K');
	int queenDifference = find_difference_of_piece(friendlyTeam, enemyTeam, 'Q');;
	int pawnDifference = find_difference_of_piece(friendlyTeam, enemyTeam, 'P');;
	int rookDifference = find_difference_of_piece(friendlyTeam, enemyTeam, 'R');;
	int bishopDifference = find_difference_of_piece(friendlyTeam, enemyTeam, 'B');;
	int knightDifference = find_difference_of_piece(friendlyTeam, enemyTeam, 'N');;
	//int turn = (((ply % 2) * 2) - 1) * -1;

	std::cout << std::to_string(moveDifference) << std::endl;

	return (
		moveFactor * moveDifference +
		kingFactor * kingDifference +
		kingFactor * kingDifference +
		queenFactor * queenDifference +
		pawnFactor * pawnDifference +
		rookFactor * rookDifference +
		bishopFactor * bishopDifference +
		knightFactor * knightDifference);
}

int Board::find_number_of_piece(char color, unsigned char pieceType) {
	std::vector<unsigned char> startingSquares;
	unsigned char pieceNumber = TEAM_COLOR_TO_NUMBER.at(color) * 6 + pieceType;
	for (unsigned char i = 0; i < NUM_SQUARES; ++i) {
		if (this->squares[i].pieceType == pieceNumber) {
			startingSquares.push_back(i);
		}
	}
	return startingSquares.size();
}

int Board::find_difference_of_piece(std::pair<char, char> friendlyTeam, std::pair<char, char> enemyTeam, char pieceType) {
	return find_number_of_piece(friendlyTeam.first, PIECE_LETTER_TO_NUMBER.at(pieceType)) +
		find_number_of_piece(friendlyTeam.second, PIECE_LETTER_TO_NUMBER.at(pieceType)) -
		(find_number_of_piece(enemyTeam.first, PIECE_LETTER_TO_NUMBER.at(pieceType)) + find_number_of_piece(enemyTeam.second, PIECE_LETTER_TO_NUMBER.at(pieceType)));
}

bool Board::in_check() {
	std::pair<char, char> friendlyTeam;
	std::pair<char, char> enemyTeam;
	char color = TEAM_NUMBER_TO_COLOR.at(ply % 4);
	if (team1.first == color || team1.second == color) {
		friendlyTeam = team1;
		enemyTeam = team2;
	}
	else {
		friendlyTeam = team2;
		enemyTeam = team1;
	}
	//Note you CAN take a king even IF it puts you into check.
	std::vector<std::pair<unsigned char, unsigned char>> enemyPlayer1Moves = this->generate_valid_moves_ignore_check(enemyTeam.first);
	std::vector<std::pair<unsigned char, unsigned char>> enemyPlayer2Moves = this->generate_valid_moves_ignore_check(enemyTeam.second);
	bool player1Attacking = false;
	bool player2Attacking = false;
	unsigned char kingLocation;
	for (int i = 0; i < NUM_SQUARES; ++i) {
		if (this->squares[i].pieceType == TEAM_COLOR_TO_NUMBER.at(color) * 6 + PIECE_LETTER_TO_NUMBER.at('K')) {
			kingLocation = i;
		}
	}
	for (auto ptr = enemyPlayer1Moves.begin(); ptr < enemyPlayer1Moves.end(); ++ptr) {
		if (ptr->second == kingLocation) {
			return true;
		}
	}
	for (auto ptr = enemyPlayer2Moves.begin(); ptr < enemyPlayer2Moves.end(); ++ptr) {
		if (ptr->second == kingLocation) {
			return true;
		}
	}
	return false;
}

int Board::alpha_beta_max(int alpha, int beta, int depthLeft) {
	if (depthLeft == 0) {
		return this->evaluate();
	}
	std::vector<std::pair<unsigned char, unsigned char>> moves = this->generate_valid_moves(TEAM_NUMBER_TO_COLOR.at(this->ply % 4));
	if (moves.empty()) {
		if (this->in_check()) {
			return INT_MIN;
		}
		else {
			return 0;
		}
	}
	for (auto ptr = moves.begin(); ptr != moves.end(); ++ptr) {
		Board copy = *this;
		copy.make_move(index_to_coordinate(ptr->first), index_to_coordinate(ptr->second));
		int score = copy.alpha_beta_min(alpha, beta, depthLeft - 1);
		if (score >= beta) {
			return beta;
		}
		if (score > alpha) {
			alpha = score;
		}
	}
	return alpha;
}

int Board::alpha_beta_min(int alpha, int beta, int depthLeft) {
	if (depthLeft == 0) {
		return -this->evaluate();
	}
	std::vector<std::pair<unsigned char, unsigned char>> moves = this->generate_valid_moves(TEAM_NUMBER_TO_COLOR.at(this->ply % 4));
	if (moves.empty()) {
		if (this->in_check()) {
			return INT_MAX;
		}
		else {
			return 0;
		}
	}
	for (auto ptr = moves.begin(); ptr != moves.end(); ++ptr) {
		Board copy = *this;
		copy.make_move(index_to_coordinate(ptr->first), index_to_coordinate(ptr->second));
		int score = copy.alpha_beta_max(alpha, beta, depthLeft - 1);
		if (score <= alpha) {
			return alpha;
		}
		if (score < beta) {
			beta = score;
		}
	}
	return beta;
}

void Board::parse_fen(std::vector<std::string> fen) {
	if (fen.size() != 15) {
		std::cerr << "Invalid FEN4 string" << std::endl;
		return;
	}
	std::string first = fen[0];
	auto re = std::regex(R"(-)");
	auto const line = std::vector<std::string>(
		std::sregex_token_iterator{ begin(first), end(first), re, -1 },
		std::sregex_token_iterator{}
	);
	this->sideToMove = line[0][0];
	//technically incorrect, but it'll have to do until we implement 50 move rule
	re = std::regex(R"(,)");
	auto vec = std::vector<std::string>(
		std::sregex_token_iterator{ begin(line[1]), end(line[1]), re, -1 },
		std::sregex_token_iterator{}
	);
	for (int i = 0; i < 4; ++i) {
		//ignore players eliminated
	}

	std::vector<std::pair<bool, bool>> castling =
	{ std::pair<bool, bool>(true, true), std::pair<bool, bool>(true, true), std::pair<bool, bool>(true, true), std::pair<bool, bool>(true, true) };
	vec = std::vector<std::string>(
		std::sregex_token_iterator{ begin(line[2]), end(line[2]), re, -1 },
		std::sregex_token_iterator{}
	);
	for (int i = 0; i < 4; ++i) {
		//kingside castling
		int canCastle = std::stoi(vec[i]);
		castling[i].first = (canCastle == 1);
	}
	vec = std::vector<std::string>(
		std::sregex_token_iterator{ begin(line[3]), end(line[3]), re, -1 },
		std::sregex_token_iterator{}
	);
	for (int i = 0; i < 4; ++i) {
		//queenside castling
		int canCastle = std::stoi(vec[i]);
		castling[i].second = (canCastle == 1);
	}
	vec = std::vector<std::string>(
		std::sregex_token_iterator{ begin(line[4]), end(line[4]), re, -1 },
		std::sregex_token_iterator{}
	);
	for (int i = 0; i < 4; ++i) {
		//ignore points
	}
	int consecutivePawnMoves = std::stoi(line[5]);
	this->ply = TEAM_COLOR_TO_NUMBER.at(sideToMove);
	re = std::regex(R"(,)");
	int j = 0;
	for (int i = 1; i < 15; ++i) {
		vec = std::vector<std::string>(
			std::sregex_token_iterator{ begin(fen[i]), end(fen[i]), re, -1 },
			std::sregex_token_iterator{}
		);
		auto ptr = vec.begin();
		while (ptr != vec.end()) {
			if (ptr->back() == '\/') {
				ptr->pop_back();
			}
			if (is_number(*ptr)) {
				int skip = std::stoi(*ptr);
				for (int k = 0; k < skip; ++k) {
					this->squares[j].pieceType = EMPTY;
					j++;
				}
			}
			else {
				std::string piece = *ptr;
				char first = piece[0];
				char second = piece[1];
				first = toupper(first);
				this->squares[j].pieceType = TEAM_COLOR_TO_NUMBER.at(first) * 6 + PIECE_LETTER_TO_NUMBER.at(second);
				j++;
			}
			++ptr;
		}
	}
	// remember to reset the corners to CORNR
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			this->squares[i * 14 + j].pieceType = CORNR;
			this->squares[(13 - i) * 14 + j].pieceType = CORNR;
			this->squares[i * 14 + (13 - j)].pieceType = CORNR;
			this->squares[(13 - i) * 14 + (13 - j)].pieceType = CORNR;
		}
	}
}

bool is_number(const std::string& s) {
	return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

char square_to_letter(Square s)
{
	//red blue yellow green
	//empty 100, corner 101
	switch (s.pieceType) {
	case RED_K:
		return 'K';
	case RED_Q:
		return 'Q';
	case RED_P:
		return 'P';
	case RED_R:
		return 'R';
	case RED_B:
		return 'B';
	case RED_N:
		return 'N';

	case BLU_K:
		return 'K';
	case BLU_Q:
		return 'Q';
	case BLU_P:
		return 'P';
	case BLU_R:
		return 'R';
	case BLU_B:
		return 'B';
	case BLU_N:
		return 'N';

	case YEL_K:
		return 'K';
	case YEL_Q:
		return 'Q';
	case YEL_P:
		return 'P';
	case YEL_R:
		return 'R';
	case YEL_B:
		return 'B';
	case YEL_N:
		return 'N';

	case GRE_K:
		return 'K';
	case GRE_Q:
		return 'Q';
	case GRE_P:
		return 'P';
	case GRE_R:
		return 'R';
	case GRE_B:
		return 'B';
	case GRE_N:
		return 'N';

	case EMPTY:
		return ' ';
	case CORNR:
		return '#';
	default:
		return '!';
	}
	return 0;
}

std::ostream& operator<<(std::ostream& o, const Board& b)
{
	for (int i = 0; i < NUM_SQUARES; ++i) {
		if (i % 14 == 0 && i != 0) {
			o << "\n";
		}
		o << square_to_letter(b.squares[i]);
	}
	return o;
}

//Order: Player to move, Player eliminated, Castling availability kingside, queenside, Points, number of consecutive non-pawn moves(used for 50 rule draw), 
//piece placement from red's perspective.
//Starts from 14th rank and goes back to first rank, each rank is described by comma separated line. Algebraic notation.
//Takes in a FEN4 string and produces a board
std::istream& operator>>(std::istream& input, Board& b) {
	std::vector<std::string> full;
	std::string line;
	while (std::getline(input, line)) {
		if (line.empty()) {
			break;
		}
		full.push_back(line);
	}
	b.parse_fen(full);
	return input;
}
