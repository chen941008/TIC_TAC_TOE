#include <chrono>
#include <iostream>

#include "Game.hpp"
#include "MCTS.hpp"
#include "Node.hpp"

using namespace std;
int main() {
    int GameTimes;
    cout << "Input how many game you want to play." << endl;
    cin >> GameTimes;
    for (int i = 0; i < GameTimes; i++) {
        Game();
    }
    return 0;
}