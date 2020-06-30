#pragma once

#include <string>
#include "board.h"

//https://www.chess.com/4-player-chess?g=3930117-4
static std::vector<std::string> moves1 = {
    "h2", "h3",
    "b9", "c9",
    "g13", "g12",
    "n5", "l6",
    "g1" , "j4",
    "b8" , "c8",
    "h14", "e11",
    "n10", "l9",
    "k2", "k4",
    "a5", "c4",
    "e11", "d11",
    "m8", "k8",
    "j4", "f4",
    "b10", "c10",
    "d11", "l11",
    "n7", "m8",
    "f4", "a9" };

void print_out_all_moves(Board& b);

bool check_moves_valid(Board& b, std::vector<std::string> moves);

void print_out_coordinates();