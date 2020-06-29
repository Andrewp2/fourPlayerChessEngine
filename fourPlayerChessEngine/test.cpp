#include "Test.h"

#include<iostream>

void print_out_all_moves(Board& b) {
    std::vector<std::pair<unsigned char, unsigned char>> validMoves = b.generate_valid_moves('R');
    std::cout << "\n\n";
    for (auto ptr = validMoves.begin(); ptr < validMoves.end(); ++ptr) {
        std::cout << index_to_coordinate(ptr->first) << "-" << index_to_coordinate(ptr->second) << "\n";
    }

    validMoves = b.generate_valid_moves('B');
    std::cout << "\n\n";
    if (!validMoves.empty()) {
        std::cout << "Blue can still move!!!" << std::endl;
    }
    for (auto ptr = validMoves.begin(); ptr < validMoves.end(); ++ptr) {
        std::cout << index_to_coordinate(ptr->first) << "-" << index_to_coordinate(ptr->second) << "\n";
    }

    validMoves = b.generate_valid_moves('Y');
    std::cout << "\n\n";
    for (auto ptr = validMoves.begin(); ptr < validMoves.end(); ++ptr) {
        std::cout << index_to_coordinate(ptr->first) << "-" << index_to_coordinate(ptr->second) << "\n";
    }

    validMoves = b.generate_valid_moves('G');
    std::cout << "\n\n";
    for (auto ptr = validMoves.begin(); ptr < validMoves.end(); ++ptr) {
        std::cout << index_to_coordinate(ptr->first) << "-" << index_to_coordinate(ptr->second) << "\n";
    }
}

bool check_moves_valid(Board& b, std::vector<std::string> moves) {
    int ply = 0;
    auto ptr = moves.begin();
    while (ptr < moves.end()) {
        std::string from = *ptr;
        ptr++;
        std::string to = *ptr;
        std::vector<std::pair<unsigned char, unsigned char>> validMoves = b.generate_valid_moves(TEAM_NUMBER_TO_COLOR.at(ply % 4));
        bool validMove = false;
        for (auto ptr2 = validMoves.begin(); ptr2 < validMoves.end(); ++ptr2) {
            unsigned char first = ptr2->first;
            unsigned char second = ptr2->second;
            if (coordinate_to_index(from) == first && coordinate_to_index(to) == second) {
                validMove = true;
            }
        }
        if (!validMove) {
            std::cerr << "This was not considered a valid move!!!" << std::endl;
            return false;
        }
        b.make_move(from, to);
        std::cout << "\n" << std::to_string(ply) << "\n" << b << "\n";
        ptr++;
        ply++;
    }
    return true;
}

void print_out_coordinates() {
    for (int i = 0; i < NUM_SQUARES; ++i) {
        if (i % 14 == 0) {
            std::cout << "\n";
        }
        std::cout << index_to_coordinate(i) << "\t";
        std::cout << std::to_string(coordinate_to_index(index_to_coordinate(i))) << "\t";
    }
}
