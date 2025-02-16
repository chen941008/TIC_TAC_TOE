#include "Game.hpp"

#include <stdint.h>

#include <iostream>

#include "MCTS.hpp"
#include "Node.hpp"

using namespace std;

void startGame() {
    Node* root = new Node();
    generateFullTree(root);
    Node* currentNode = root;  // CurrentNode為當前棋盤最後一個子的節點，會去選擇他的子節點來下棋
    int playerOrder, currentOrder = 0, aiMode, iterationTimes, simulationTimes;
    cout << "Input stimulation times." << endl;
    cin >> simulationTimes;
    MCTS ai(simulationTimes);
    cout << "Choose AI simulation mode: 1 = fixed simulation times, 2 = "
            "variable simulation times"
         << endl;
    while (true) {
        cin >> aiMode;
        if (aiMode == aiMode::FIXED_SIMULATION_TIMES) {
            while (true) {
                cout << "Input how many iteration you want to run (must be "
                        "greater than 9)."
                     << endl;
                cin >> iterationTimes;
                if (iterationTimes > MAX_CHILDREN) {
                    break;
                }
                cout << "Please input a number greater than 9." << endl;
            }
            break;
        }
        if (aiMode == aiMode::VARIABLE_SIMULATION_TIMES) {
            break;
        }
        cout << "Please input 1 or 2" << endl;
    }
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
        if (currentOrder == MAX_CHILDREN) {
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
                moveIndex = X * BOARD_SIZE + Y;
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
            if (currentOrder >= CHECKWIN_THRESHOLD && checkWin(boardX, boardO, playerOrder == 0)) {
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
            if (aiMode == aiMode::VARIABLE_SIMULATION_TIMES) {
                cout << "Input how many iteration you want to run (must be "
                        "greater than 9)."
                     << endl;
                do {
                    cin >> iterationTimes;
                    if (iterationTimes <= MAX_CHILDREN) {
                        cout << "Please input a number greater than 9." << endl;
                    }
                } while (iterationTimes <= MAX_CHILDREN);
            }
            uint16_t prevBoardX = boardX;
            uint16_t prevBoardO = boardO;
            ai.run(currentNode, iterationTimes);
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
                for (int idx = 0; idx < MAX_CHILDREN; idx++) {
                    if (diff & (1 << idx)) {
                        moveIndex = idx;
                        break;
                    }
                }
            } else {
                // 當前回合為 O 時，差異應出現在 boardO
                uint16_t diff = bestChild->boardO & ~(prevBoardO);
                for (int idx = 0; idx < MAX_CHILDREN; idx++) {
                    if (diff & (1 << idx)) {
                        moveIndex = idx;
                        break;
                    }
                }
            }
            cout << "AI choose " << moveIndex / BOARD_SIZE << " " << moveIndex % BOARD_SIZE << endl;
            // 更新本地棋盤狀態以及 currentNode
            boardX = bestChild->boardX;
            boardO = bestChild->boardO;
            currentNode = bestChild;
            if (currentOrder >= CHECKWIN_THRESHOLD && checkWin(boardX, boardO, (currentOrder % 2 == 0))) {
                cout << "AI win" << endl;
                printBoard(boardX, boardO);
                break;
            }
        }
        currentOrder++;
    }
    delete root;
}

void printBoard(uint16_t boardX, uint16_t boardO) {
    cout << endl;
    for (int i = 0; i < BOARD_SIZE; i++) {
        int temp = i * BOARD_SIZE;
        for (int j = 0; j < BOARD_SIZE; j++) {
            int index = temp + j;         // 將 2D 轉換為 1D 索引
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

// 遞迴生成完整遊戲樹的函式
void generateFullTree(Node* node) {
    // 取得當前棋盤已使用的位置
    uint16_t usedPositions = node->boardX | node->boardO;

    // 如果棋盤已滿，或者當前局面已經分出勝負，就不再展開
    if (usedPositions == 0b111111111) {
        node->state = BoardState::DRAW;
        return;
    } else if (checkWin(node->boardX, node->boardO, node->isXTurn)) {
        node->state = BoardState::WIN;
        return;
    }

    int childIndex = 0;
    // 對於棋盤上的每個位置，若未被使用，則產生一個新節點
    for (int i = 0; i < MAX_CHILDREN; i++) {
        if (!(usedPositions & (1 << i))) {      // 若位置 i 未被使用
            Node* newNode = new Node(i, node);  // 依據父節點狀態及這步 move 建立新節點
            node->children[childIndex++] = newNode;
        }
    }

    // 遞迴產生每個子節點的遊戲樹
    for (int i = 0; i < childIndex; i++) {
        generateFullTree(node->children[i]);
    }
}
