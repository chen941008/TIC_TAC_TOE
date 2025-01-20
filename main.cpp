#include <iostream>
#include "MCTS.hpp"
using namespace std;
int main() {
    int RunTimes;
    cin >> RunTimes;
    Node* root = new Node();
    MCTS(root, RunTimes);
    return 0;
}