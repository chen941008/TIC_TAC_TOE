#include "Game.hpp"

#include <iostream>

#include "MCTS.hpp"
#include "Node.hpp"

using namespace std;
void startGame() {
    const int iterationTimes = 550000;
    int playerOrder, currentOrder = 0;
    Node* root = new Node();
    expansion(root);
    Node* currentNode = root;  // CurrentNode為當前棋盤最後一個子的節點，會去選擇他的子節點來下棋
    int board[BOARD_SIZE][BOARD_SIZE] = {0};
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
            if (currentOrder >= 4 && checkWin(board, playerOrder == 0)) {
                cout << "You win" << endl;
                printBoard(board);
                break;
            }
            for (int i = 0; i < MAX_CHILDREN && currentNode->children[i] != nullptr; i++) {
                if (currentNode->children[i]->move.X == X && currentNode->children[i]->move.Y == Y) {
                    currentNode = currentNode->children[i];
                    break;
                }
            }
        } else {  // AI turn
            cout << "AI turn" << endl;
            MCTS(currentNode, iterationTimes);
            Node* bestChild = currentNode->children[0];
            for (int i = 1; i < MAX_CHILDREN && currentNode->children[i] != nullptr; ++i) {
                Node* child = currentNode->children[i];
                // 更新 `bestChild`，根據終局狀態優先順序 WIN > DRAW > LOSE
                if (child->state > bestChild->state) {
                    bestChild = child;
                }
            }
            cout << "AI choose " << bestChild->move.X << " " << bestChild->move.Y << endl;
            board[bestChild->move.X][bestChild->move.Y] = playerOrder == 0 ? -1 : 1;
            currentNode = bestChild;
            if (currentOrder >= 4 && checkWin(board, playerOrder == 1)) {
                cout << "AI win" << endl;
                printBoard(board);
                break;
            }
        }
        currentOrder++;
    }
    deleteTree(root);
}

bool checkWin(int board[BOARD_SIZE][BOARD_SIZE], bool playTurn) {
    int player = playTurn ? 1 : -1;
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (board[i][0] == player && board[i][1] == player && board[i][2] == player) {
            return true;
        }
        if (board[0][i] == player && board[1][i] == player && board[2][i] == player) {
            return true;
        }
    }
    if (board[0][0] == player && board[1][1] == player && board[2][2] == player) {
        return true;
    }
    if (board[0][2] == player && board[1][1] == player && board[2][0] == player) {
        return true;
    }
    return false;
}

void printBoard(int board[BOARD_SIZE][BOARD_SIZE]) {
    cout << endl;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
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