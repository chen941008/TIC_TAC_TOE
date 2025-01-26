#include "Game.hpp"

#include <iostream>

#include "MCTS.hpp"
#include "Node.hpp"

using namespace std;
void startGame() {
    int playerOrder, currentOrder = 0, aiMode, simulationTimes;
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
                cin >> simulationTimes;
                if (simulationTimes > 10) {
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
    Node* currentNode =
        root;  // CurrentNode為當前棋盤最後一個子的節點，會去選擇他的子節點來下棋
    int board[3][3] = {0};
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
                if (board[X][Y] != 0) {
                    cout << "This position is already taken" << endl;
                    continue;
                }
                break;
            }
            board[X][Y] = playerOrder == 0 ? 1 : -1;
            if (checkWin(board, playerOrder == 0)) {
                cout << "You win" << endl;
                printBoard(board);
                break;
            }
            for (int i = 0; i < 9 && currentNode->children[i] != nullptr; i++) {
                if (currentNode->children[i]->move.X == X &&
                    currentNode->children[i]->move.Y == Y) {
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
                    cin >> simulationTimes;
                    if (simulationTimes <= 10) {
                        cout << "Please input a number greater than 10."
                             << endl;
                    }
                } while (simulationTimes <= 10);
            }
            MCTS(currentNode, simulationTimes);
            Node* bestChild = nullptr;
            double bestScore = -std::numeric_limits<double>::infinity();

            for (int i = 0; i < 9 && currentNode->children[i] != nullptr; ++i) {
                Node* child = currentNode->children[i];

                // 計算綜合分數：考慮勝率、訪問次數和路徑長度
                double winRate = static_cast<double>(child->wins) /
                                 static_cast<double>(child->visits);
                double visitRatio = static_cast<double>(child->visits) /
                                    static_cast<double>(currentNode->visits);
                double score = 1.0 * winRate + 0.0 * visitRatio;  // 可調整權重

                if (score > bestScore) {
                    bestScore = score;
                    bestChild = child;
                }
            }

            cout << "AI choose " << bestChild->move.X << " "
                 << bestChild->move.Y << endl;
            board[bestChild->move.X][bestChild->move.Y] =
                playerOrder == 0 ? -1 : 1;
            currentNode = bestChild;
            if (checkWin(board, playerOrder == 1)) {
                cout << "AI win" << endl;
                printBoard(board);
                break;
            }
        }
        currentOrder++;
    }
    delete root;
}

bool checkWin(int board[3][3], bool playTurn) {
    int player = playTurn ? 1 : -1;
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == player && board[i][1] == player &&
            board[i][2] == player) {
            return true;
        }
        if (board[0][i] == player && board[1][i] == player &&
            board[2][i] == player) {
            return true;
        }
    }
    if (board[0][0] == player && board[1][1] == player &&
        board[2][2] == player) {
        return true;
    }
    if (board[0][2] == player && board[1][1] == player &&
        board[2][0] == player) {
        return true;
    }
    return false;
}

void printBoard(int board[3][3]) {
    cout << endl;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == 1) {
                cout << " X ";
            } else if (board[i][j] == -1) {
                cout << " O ";
            } else {
                cout << "   ";  // 空格使用 " "，讓棋盤看起來更清爽
            }
            if (j < 2) cout << "|";  // 每一行列之間加上分隔線
        }
        cout << endl;
        if (i < 2) cout << "-----------" << endl;  // 每一行之後加上分隔線
    }
    cout << endl;
}