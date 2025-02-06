#include "MCTS.hpp"

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
void MCTS(Node* root, int iterations) {
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
    bool usedPositions[BOARD_SIZE][BOARD_SIZE] = {false};  // 初始化棋盤的使用情況
    Node* nodePointer = node;
    while (nodePointer->parent != nullptr) {
        usedPositions[nodePointer->move.X][nodePointer->move.Y] = true;
        nodePointer = nodePointer->parent;
    }
    bool isFull = true;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (usedPositions[i][j] == 0) {
                isFull = false;
                break;
            }
        }
    }
    if (isFull) {
        return node;
    }
    int index = 0;
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (!usedPositions[i][j]) {  // 若該位置未被使用
                Node* newNode = new Node({i, j}, node);
                node->children[index++] = newNode;
            }
        }
    }
    return node->children[0];
}

int playout(Node* node) {                    // 在該node的回合開始遊戲，回傳值為node方的勝負關係：
                                             // 1 win, 0 draw, -1 lose
    int board[BOARD_SIZE][BOARD_SIZE] = {};  // 0: empty, 1: X, -1: O
    Node* nodePointer = node;
    bool startTurn = node->isXTurn, currentTurn = node->isXTurn;
    // 將節點的走步依序放入棋盤(前置作業)
    while (nodePointer->parent != nullptr) {
        board[nodePointer->move.X][nodePointer->move.Y] = nodePointer->isXTurn ? 1 : -1;
        nodePointer = nodePointer->parent;
    }
    if (checkWin(board,
                 startTurn)) {  // 如果節點的走步已經獲勝，也就是該路徑已有結果，設定isTerminal為true
        node->state = WIN;
        return 1;
    }
    // 建立可能的走步
    Position possibleMoves[9];  // 最大可能的走法數量為 9（3x3 棋盤）
    int moveCount = 0;          // 用來追蹤有效的走法數量
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] == 0) {
                possibleMoves[moveCount++] = {i, j};  // 儲存有效的走法
            }
        }
    }
    if (moveCount == 0) {  // 該node為最終節點，棋盤已滿，平手，並設置為isTerminal=true
        node->state = DRAW;
        return 0;
    }
    // 將節點的走步依序放入棋盤(後續作業)
    while (moveCount > 0) {
        // 隨機選擇一個走步
        currentTurn = !currentTurn;  // 換手
        uniform_int_distribution<int> distribution(0, moveCount - 1);
        int moveIndex = distribution(generator);
        Position move = possibleMoves[moveIndex];
        // 將選中的走步移到陣列最後，並減少 moveCount
        possibleMoves[moveIndex] = possibleMoves[moveCount - 1];
        moveCount--;                                   // 移除最後一個走步
        board[move.X][move.Y] = currentTurn ? 1 : -1;  // 將走步放入棋盤
        // 檢查是否有玩家獲勝
        if (moveCount <= 4 && checkWin(board,
                                       currentTurn)) {  // 空格在四個以下才有可能獲勝，只需檢查下棋那方是否獲勝
            if (currentTurn == startTurn) {             // 勝利方為開始下棋的那方=node方獲勝
                return 1;
            } else {  // 勝利方為非開始下棋的那方=node方輸
                return -1;
            }
        }
    }
    // 平手
    return 0;
}