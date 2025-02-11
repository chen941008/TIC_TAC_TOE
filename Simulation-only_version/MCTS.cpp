#include "MCTS.hpp"

#include <stdint.h>

#include <chrono>
#include <climits>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>
#include <set>

#include "Game.hpp"
#include "Node.hpp"

using namespace std;
const double COEFFICIENT = 1.41;  // UCB1算法的參數
int simulationTimes;              // 實際定義變量
random_device rd;                 // 取得硬體隨機數
mt19937 generator(rd());          // 初始化隨機數生成器
double calculateUCB(int parentVisits, int nodeVisits, double nodeWins) {
    if (nodeVisits == 0) {
        return static_cast<double>(INT_MAX);
    }
    return static_cast<double>(nodeWins) / static_cast<double>(nodeVisits) +
           COEFFICIENT * sqrt(log(static_cast<double>(parentVisits)) / static_cast<double>(nodeVisits));
}
int MCTS(Node* root, int iterations) {
    auto start = std::chrono::high_resolution_clock::now();
    // 記錄開始時間
    for (int i = 0; i < iterations; i++) {
        Node* selectedNode = selection(root);  // 選擇best leaf node
        if (selectedNode->state == WIN) {
            backpropagation(selectedNode, selectedNode->isXTurn, 1.0);
            continue;
        }
        if (selectedNode->state == DRAW) {
            backpropagation(selectedNode, selectedNode->isXTurn, 0.0);
            continue;
        }
        if (selectedNode->visits != 0) {
            selectedNode = expansion(selectedNode);
        }
        double playoutResult = 0.0;
        for (int j = 0; j < simulationTimes; j++) {
            playoutResult += playout(selectedNode);  // 模擬遊戲 1 = selected node win, 0 =
                                                     // draw, -1 = selected node lose
        }
        playoutResult /= simulationTimes;
        backpropagation(selectedNode, selectedNode->isXTurn,
                        playoutResult);  // 傳遞結果並更新節點資訊
    }
    auto end = std::chrono::high_resolution_clock::now();  // 記錄結束時間
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    return duration.count();
}

Node* selection(Node* node) {  // select the best leaf node
    while (true) {
        Node* bestChild = nullptr;                                     // 一直選到leaf node
        if (node->children[0] == nullptr) {                            // 當前節點沒有子節點時，該節點為leaf node
            return node;                                               // node，返回該節點
        } else {                                                       // 如果當前節點有子節點
            bestChild = nullptr;                                       // 紀錄最佳子節點
            double bestValue = std::numeric_limits<double>::lowest();  // 紀錄最佳 UCB 值
            for (int i = 0; i < MAX_CHILDREN && node->children[i] != nullptr; i++) {
                Node* child = node->children[i];
                double ucbValue = calculateUCB(node->visits, child->visits, child->wins);
                // 更新最佳節點
                if (ucbValue > bestValue) {
                    bestValue = ucbValue;
                    bestChild = child;
                }
            }
        }
        node = bestChild;  // 選擇最佳子節點
    }
}

void backpropagation(Node* node, bool isXTurn,
                     double win) {  // leaf node; leaf node's turn; win, lose, or draw
    while (node != nullptr) {
        node->visits++;
        if (isXTurn == node->isXTurn) {
            node->wins += win;
        } else {
            node->wins -= win;
        }
        node = node->parent;
    }
}

Node* expansion(Node* node) {
    uint16_t usedPositions = node->boardX | node->boardO;  // 獲取已使用的位置
    if (usedPositions == 0b111111111) {                    // 棋盤已滿
        return node;
    }
    int index = 0;
    for (int i = 0; i < 9; i++) {
        if (!(usedPositions & (1 << i))) {  // 如果該位置未被使用
            Node* newNode = new Node(i, node);
            node->children[index++] = newNode;
        }
    }
    return node->children[0];  // 返回第一個擴展的子節點
}

int playout(Node* node) {  // 在該 node 的回合開始遊戲，回傳值為 node 方的勝負關係：
                           // 1 win, 0 draw, -1 lose
    uint16_t boardX = node->boardX;
    uint16_t boardO = node->boardO;
    bool startTurn = node->isXTurn;
    bool currentTurn = startTurn;

    // 如果當前節點已經是一個勝利狀態，直接回傳勝利
    if (checkWin(boardX, boardO, startTurn)) {
        node->state = WIN;
        return 1;
    }

    // 建立可能的走步
    uint16_t usedPositions = boardX | boardO;                // 紀錄已使用的位置
    uint16_t availableMoves = ~usedPositions & 0b111111111;  // 取得可用位置
    if (availableMoves == 0) {                               // 棋盤已滿，平手
        node->state = DRAW;
        return 0;
    }

    int possibleMoves[9];
    int count = 0;

    // 取得所有可用位置
    for (int i = 0; i < 9; i++) {
        if (availableMoves & (1 << i)) {
            possibleMoves[count++] = i;
        }
    }

    std::uniform_int_distribution<int> distribution(0, count - 1);

    // 隨機模擬遊戲進行
    while (count > 0) {
        currentTurn = !currentTurn;  // 換手
        int moveIndex = distribution(generator);
        int move = possibleMoves[moveIndex];

        // 將已選位置移到陣列尾部並減少計數
        possibleMoves[moveIndex] = possibleMoves[--count];

        if (currentTurn) {
            boardX |= (1 << move);
        } else {
            boardO |= (1 << move);
        }

        // 只需檢查下棋的那方是否獲勝
        if (checkWin(boardX, boardO, currentTurn)) {
            return (currentTurn == startTurn) ? 1 : -1;
        }
    }
    return 0;  // 平手
}
