#include "Game.hpp"

#include <stdint.h>

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
    // 用 bitboard 表示棋盤，初始皆為 0
    uint16_t boardX = 0;
    uint16_t boardO = 0;
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
            printBoard(boardX, boardO);
            break;
        }
        printBoard(boardX, boardO);
        if (currentOrder % 2 == playerOrder) {  // Player turn
            cout << "Your turn" << endl;
            int X, Y, moveIndex;
            cout << "input X Y 0~2" << endl;
            while (true) {
                cin >> X >> Y;
                if (X < 0 || X > 2 || Y < 0 || Y > 2) {
                    cout << "Please input 0~2" << endl;
                    continue;
                }
                moveIndex = X * 3 + Y;
                // 檢查該位置是否已被佔用
                if ((boardX | boardO) & (1 << moveIndex)) {
                    cout << "This position is already taken" << endl;
                    continue;
                }
                break;
            }
            if (currentOrder % 2 == 0) {
                boardX |= (1 << moveIndex);
            } else {
                boardO |= (1 << moveIndex);
            }
            if (currentOrder >= 4 && checkWin(boardX, boardO, playerOrder == 0)) {
                cout << "You win" << endl;
                printBoard(boardX, boardO);
                break;
            }
            for (int i = 0; i < MAX_CHILDREN && currentNode->children[i] != nullptr; i++) {
                if (currentNode->children[i]->boardX == boardX && currentNode->children[i]->boardO == boardO) {
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
            uint16_t prevBoardX = boardX;
            uint16_t prevBoardO = boardO;
            MCTS(currentNode, iterationTimes);
            Node* bestChild = nullptr;
            int mostVisit = 0;
            for (int i = 0; i < MAX_CHILDREN && currentNode->children[i] != nullptr; ++i) {
                Node* child = currentNode->children[i];
                if (child->visits > mostVisit) {
                    mostVisit = child->visits;
                    bestChild = child;
                }
            }
            int moveIndex = -1;
            if (currentOrder % 2 == 0) {
                // 當前回合為 X 時，差異應出現在 boardX
                uint16_t diff = bestChild->boardX & ~(prevBoardX);
                for (int idx = 0; idx < 9; idx++) {
                    if (diff & (1 << idx)) {
                        moveIndex = idx;
                        break;
                    }
                }
            } else {
                // 當前回合為 O 時，差異應出現在 boardO
                uint16_t diff = bestChild->boardO & ~(prevBoardO);
                for (int idx = 0; idx < 9; idx++) {
                    if (diff & (1 << idx)) {
                        moveIndex = idx;
                        break;
                    }
                }
            }
            cout << "AI choose " << moveIndex / 3 << " " << moveIndex % 3 << endl;
            // 更新本地棋盤狀態以及 currentNode
            boardX = bestChild->boardX;
            boardO = bestChild->boardO;
            currentNode = bestChild;
            if (currentOrder >= 4 && checkWin(boardX, boardO, (currentOrder % 2 == 0))) {
                cout << "AI win" << endl;
                printBoard(boardX, boardO);
                break;
            }
        }
        currentOrder++;
    }
    delete root;
}

bool checkWin(uint16_t boardX, uint16_t boardO, bool playTurn) {
    uint16_t const winPattern[8] = {0b111000000, 0b000111000, 0b000000111, 0b100100100,
                                    0b010010010, 0b001001001, 0b100010001, 0b001010100};
    uint16_t playerBoard = playTurn ? boardX : boardO;
    for (int i = 0; i < 8; i++) {
        if ((playerBoard & winPattern[i]) == winPattern[i]) {
            return true;
        }
    }
    return false;
}

void printBoard(uint16_t boardX, uint16_t boardO) {
    cout << endl;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int index = i * 3 + j;        // 將 2D 轉換為 1D 索引
            if (boardX & (1 << index)) {  // 檢查 X 的棋子
                cout << " X ";
            } else if (boardO & (1 << index)) {  // 檢查 O 的棋子
                cout << " O ";
            } else {
                cout << "   ";  // 空位置
            }
            if (j < 2) cout << "|";  // 每列之間加上分隔線
        }
        cout << endl;
        if (i < 2) cout << "-----------" << endl;  // 每行之後加上分隔線
    }
    cout << endl;
}
