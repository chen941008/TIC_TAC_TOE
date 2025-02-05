#include <chrono>
#include <fstream>
#include <iostream>

#include "Game.hpp"
#include "MCTS.hpp"
#include "Node.hpp"

using namespace std;
int main() {
    int gameTimes;
    cout << "Input how many game you want to play." << endl;
    cin >> gameTimes;
    for (int i = 0; i < gameTimes; i++) {
        startGame();
    }
    return 0;
}