// fourPlayerChessEngine.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "board.h"
#include "Test.h"

#include <gtkmm.h>
#include "fourPlayerChessEngine.h"

int main(int argc, char *argv[])
{
    Board board;
    board.initialize_board();

    /*std::cout << "Begin evaluations" << std::endl;
    for (int i = 0; i < 10; i++) {
        std::cout << "\n" << "Depth: " << std::to_string(i) <<  " Evaluation: " << std::to_string((double)board.evaluate(i)/100.0) << std::endl;
    }*/

    std::cout << "\n" << board << "\n";
    print_out_coordinates();
    //https://www.chess.com/4-player-chess?g=3930117-4
    std::vector<std::string> moves = {
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
    //check_moves_valid(board, moves);
    //print_out_all_moves(board);



    /*auto app = Gtk::Application::create(argc, argv, "Four Player Chess Engine");
    auto builder = Gtk::Builder::create_from_file("chess.glade");
    Gtk::Window* window = nullptr;
    builder->get_widget("mainWindow", window);
    window->set_size_request(1080, 720);
    app->run(*window);
    delete window;*/
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
