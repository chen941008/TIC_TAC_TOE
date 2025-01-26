#include "Game.hpp"

#include <iostream>

#include "MCTS.hpp"
#include "Node.hpp"

using namespace std;
void Game() {
    int PlayerOrder, CurrentOrder = 0, AIMode, simulationTimes;
    cout << "Choose AI simulation mode: 1 = fixed simulation times, 2 = "
            "variable simulation times"
         << endl;
    while (true) {
        cin >> AIMode;
        if (AIMode == 1) {
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
        if (AIMode == 2) {
            break;
        }
        cout << "Please input 1 or 2" << endl;
    }
    Node* root = new Node();
    Expansion(root);
    Node* CurrentNode =
        root;  // CurrentNode為當前棋盤最後一個子的節點，會去選擇他的子節點來下棋
    int board[3][3] = {0};
    cout << "Choose first or second player, input 1 or 2" << endl;
    while (true) {  // 選擇先手或後手，防白痴crash程式
        cin >> PlayerOrder;
        if (PlayerOrder == 1 || PlayerOrder == 2) {
            break;
        }
        cout << "Please input 1 or 2" << endl;
    }
    PlayerOrder--;
    while (true) {
        if (CurrentOrder == 9) {
            cout << "Draw" << endl;
            printBoard(board);
            break;
        }
        printBoard(board);
        if (CurrentOrder % 2 == PlayerOrder) {  // Player turn
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
            board[X][Y] = PlayerOrder == 0 ? 1 : -1;
            if (CheckWin(board, PlayerOrder == 0)) {
                cout << "You win" << endl;
                printBoard(board);
                break;
            }
            for (int i = 0; i < 9 && CurrentNode->Children[i] != nullptr; i++) {
                if (CurrentNode->Children[i]->Move.X == X &&
                    CurrentNode->Children[i]->Move.Y == Y) {
                    CurrentNode = CurrentNode->Children[i];
                    break;
                }
            }
        } else {  // AI turn
            cout << "AI turn" << endl;
            if (AIMode == 2) {
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
            MCTS(CurrentNode, simulationTimes);
            Node* bestChild = nullptr;
            double bestScore = -std::numeric_limits<double>::infinity();

            for (int i = 0; i < 9 && CurrentNode->Children[i] != nullptr; ++i) {
                Node* child = CurrentNode->Children[i];

                // 計算綜合分數：考慮勝率、訪問次數和路徑長度
                double winRate = static_cast<double>(child->Wins) /
                                 static_cast<double>(child->Visits);
                double visitRatio = static_cast<double>(child->Visits) /
                                    static_cast<double>(CurrentNode->Visits);
                double score = 1.0 * winRate + 0.0 * visitRatio;  // 可調整權重

                if (score > bestScore) {
                    bestScore = score;
                    bestChild = child;
                }
            }

            cout << "AI choose " << bestChild->Move.X << " "
                 << bestChild->Move.Y << endl;
            board[bestChild->Move.X][bestChild->Move.Y] =
                PlayerOrder == 0 ? -1 : 1;
            CurrentNode = bestChild;
            if (CheckWin(board, PlayerOrder == 1)) {
                cout << "AI win" << endl;
                printBoard(board);
                break;
            }
        }
        CurrentOrder++;
    }
    delete root;
}

bool CheckWin(int board[3][3], bool PlayTurn) {
    int player = PlayTurn ? 1 : -1;
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