#include "Game.hpp"

#include <iostream>

#include "MCTS.hpp"
#include "Node.hpp"

using namespace std;
void startGame() {
    int playerOrder, currentOrder = 0, aiMode, iterationTimes;
    cout << "Choose AI simulation mode: 1 = fixed simulation times, 2 = "
            "variable simulation times"
         << endl;
    while (true) {
        cin >> aiMode;
        if (aiMode == 1) {
            while (true) {
                cout << "Input how many iteration you want to run (must be "
                        "greater than 10)."
                     << endl;
                cin >> iterationTimes;
                if (iterationTimes > 10) {
                    break;
                }
                cout << "Please input a number greater than 10." << endl;
            }
            break;
        }
        if (aiMode == 2) {
            break;
        }
        cout << "Please input 1 or 2" << endl;
    }
    Node* root = new Node();
    expansion(root);
    Node* currentNode = root;  // CurrentNode為當前棋盤最後一個子的節點，會去選擇他的子節點來下棋
    int board[9] = {0};
    cout << "Choose first or second player, input 1 or 2" << endl;
    while (true) {  // 選擇先手或後手，防白痴crash程式
        cin >> playerOrder;
        if (playerOrder == 1 || playerOrder == 2) {
            break;
        }
        cout << "Please input 1 or 2" << endl;
    }
    playerOrder--;
    while (true) {
        if (currentOrder == 9) {
            cout << "Draw" << endl;
            printBoard(board);
            break;
        }
        printBoard(board);
        if (currentOrder % 2 == playerOrder) {  // Player turn
            cout << "Your turn" << endl;
            int X, Y;
            cout << "input X Y 0~2" << endl;
            while (true) {
                cin >> X >> Y;
                if (X < 0 || X > 2 || Y < 0 || Y > 2) {
                    cout << "Please input 0~2" << endl;
                    continue;
                }
                if (board[X * 3 + Y] != 0) {
                    cout << "This position is already taken" << endl;
                    continue;
                }
                break;
            }
            board[X * 3 + Y] = playerOrder == 0 ? 1 : -1;
            if (currentOrder >= 4 && checkWin(board, playerOrder == 0)) {
                cout << "You win" << endl;
                printBoard(board);
                break;
            }
            for (int i = 0; i < MAX_CHILDREN && currentNode->children[i] != nullptr; i++) {
                if (currentNode->children[i]->move == X * 3 + Y) {
                    currentNode = currentNode->children[i];
                    break;
                }
            }
        } else {  // AI turn
            cout << "AI turn" << endl;
            if (aiMode == 2) {
                cout << "Input how many iteration you want to run (must be "
                        "greater than 10)."
                     << endl;
                do {
                    cin >> iterationTimes;
                    if (iterationTimes <= 10) {
                        cout << "Please input a number greater than 10." << endl;
                    }
                } while (iterationTimes <= 10);
            }
            MCTS(currentNode, iterationTimes);
            Node* bestChild = nullptr;
            int mostVisit = 0;
            for (int i = 0; i < MAX_CHILDREN && currentNode->children[i] != nullptr; ++i) {
                Node* child = currentNode->children[i];
                // 計算綜合分數：考慮勝率、訪問次數和路徑長度
                if (child->visits > mostVisit) {
                    mostVisit = child->visits;
                    bestChild = child;
                }
            }
            cout << "AI choose " << bestChild->move / 3 << " " << bestChild->move % 3 << endl;
            board[bestChild->move] = playerOrder == 0 ? -1 : 1;
            currentNode = bestChild;
            if (currentOrder >= 4 && checkWin(board, playerOrder == 1)) {
                cout << "AI win" << endl;
                printBoard(board);
                break;
            }
        }
        currentOrder++;
    }
    delete root;
}

bool checkWin(int board[9], bool playTurn) {
    int player = playTurn ? 1 : -1;

    // 直接列出所有勝利的組合
    const int wins[8][3] = {
        {0, 1, 2}, {3, 4, 5}, {6, 7, 8},  // 橫向
        {0, 3, 6}, {1, 4, 7}, {2, 5, 8},  // 縱向
        {0, 4, 8}, {2, 4, 6}              // 對角線
    };

    for (int i = 0; i < 8; i++) {
        if (board[wins[i][0]] == player && board[wins[i][1]] == player && board[wins[i][2]] == player) {
            return true;
        }
    }
    return false;
}

void printBoard(int board[9]) {
    cout << endl;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int index = i * 3 + j;  // 將 2D 轉換為 1D 索引
            if (board[index] == 1) {
                cout << " X ";
            } else if (board[index] == -1) {
                cout << " O ";
            } else {
                cout << "   ";
            }
            if (j < 2) cout << "|";  // 每列之間加上分隔線
        }
        cout << endl;
        if (i < 2) cout << "-----------" << endl;  // 每行之後加上分隔線
    }
    cout << endl;
}
