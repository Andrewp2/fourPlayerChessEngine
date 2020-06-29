#pragma once

#include <string>
#include "board.h"
void print_out_all_moves(Board& b);

bool check_moves_valid(Board& b, std::vector<std::string> moves);

void print_out_coordinates();