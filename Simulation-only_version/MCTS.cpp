#include "MCTS.hpp"

#include <stdint.h>

#include <atomic>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <set>
#include <thread>
#include <vector>

#include "Game.hpp"
#include "Node.hpp"

using namespace std;
const double COEFFICIENT = 1.41;  // UCB1算法的參數
int simulationTimes;              // 實際定義變量
random_device rd;                 // 取得硬體隨機數
mt19937 generator(rd());          // 初始化隨機數生成器

int MCTS(Node* root, int iterations) {
    auto start = std::chrono::high_resolution_clock::now();
    // 記錄開始時間
    for (int i = 0; i < iterations; i++) {
        Node* selectedNode = selection(root);  // 選擇best leaf node
        if (selectedNode->state == BoardState::WIN) {
            backpropagation(selectedNode, root->parent, selectedNode->isXTurn, static_cast<double>(BoardState::WIN));
            continue;
        }
        if (selectedNode->state == BoardState::DRAW) {
            backpropagation(selectedNode, root->parent, selectedNode->isXTurn, static_cast<double>(BoardState::DRAW));
            continue;
        }
        double playoutResult = 0.0;
        for (int i = 0; i < simulationTimes; i++) {
            playoutResult += playout(selectedNode);
        }
        playoutResult /= simulationTimes;
        backpropagation(selectedNode, root->parent, selectedNode->isXTurn,
                        playoutResult);  // 傳遞結果並更新節點資訊
    }
    auto end = std::chrono::high_resolution_clock::now();  // 記錄結束時間
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    return duration.count();
}

inline Node* selection(Node* node) {  // select the best leaf node
    while (true) {
        Node* bestChild = nullptr;                                     // 一直選到leaf node
        if (node->children[0] == nullptr) {                            // 當前節點沒有子節點時，該節點為leaf node
            return node;                                               // node，返回該節點
        } else {                                                       // 如果當前節點有子節點
            bestChild = nullptr;                                       // 紀錄最佳子節點
            double bestValue = std::numeric_limits<double>::lowest();  // 紀錄最佳 UCB 值
            double logParent = log(node->visits);                      // 避免每次都重算 log(node->visits)
            for (int i = 0; i < MAX_CHILDREN && node->children[i] != nullptr; i++) {
                Node* child = node->children[i];
                if (child->visits == 0) {
                    return child;
                }
                double ucbValue = (child->wins / child->visits) + COEFFICIENT * sqrt(logParent / child->visits);
                if (ucbValue > bestValue) {
                    bestValue = ucbValue;
                    bestChild = child;
                }
            }
        }
        node = bestChild;  // 選擇最佳子節點
    }
}

inline void backpropagation(Node* node, Node* endNode, bool isXTurn,
                            double win) {  // leaf node; leaf node's turn; win, lose, or draw
    while (node != endNode) {
        node->visits++;
        if (isXTurn == node->isXTurn) {
            node->wins += win;
        } else {
            node->wins -= win;
        }
        node = node->parent;
    }
}

inline int playout(Node* node) {
    uint16_t boardX = node->boardX;
    uint16_t boardO = node->boardO;
    bool startTurn = node->isXTurn;
    bool currentTurn = startTurn;

    // 建立可能的走步
    uint16_t usedPositions = boardX | boardO;                // 紀錄已使用的位置
    uint16_t availableMoves = ~usedPositions & 0b111111111;  // 取得可用位置

    int possibleMoves[MAX_CHILDREN];
    int count = 0;

    // 取得所有可用位置
    for (int i = 0; i < MAX_CHILDREN; i++) {
        if (availableMoves & (1 << i)) {
            possibleMoves[count++] = i;
        }
    }

    // 事先隨機打亂 possibleMoves 陣列
    for (int i = count - 1; i > 0; --i) {
        int j = generator() % (i + 1);
        swap(possibleMoves[i], possibleMoves[j]);  // 交換 i 和 j
    }

    // 依序模擬遊戲進行
    for (int i = count - 1; i >= 0; i--) {
        currentTurn = !currentTurn;  // 換手
        int move = possibleMoves[i];

        if (currentTurn) {
            boardX |= (1 << move);
        } else {
            boardO |= (1 << move);
        }

        if (i <= CHECKWIN_THRESHOLD && checkWin(boardX, boardO, currentTurn)) {
            return (currentTurn == startTurn) ? static_cast<int>(BoardState::WIN) : static_cast<int>(BoardState::LOSE);
        }
    }

    return static_cast<int>(BoardState::DRAW);  // 平手
}
