#ifndef MCTS_HPP
#define MCTS_HPP
#include <cstdint>

// 前向聲明 Node 結構體，避免需要包含整個定義
struct Node;
extern int simulationTimes;

/**
 * @brief 執行 MCTS (Monte Carlo Tree Search) 演算法
 *
 * @param root MCTS 演算法的根節點，表示當前搜尋的起點
 * @param iterations 演算法的迭代次數，即 MCTS 在該節點執行多少次模擬
 */
void MCTS(Node* root, int iterations);

/**
 * @brief MCTS 選擇階段 (Selection)
 *
 * @param node 當前節點，表示目前 MCTS 所處的節點
 * @return Node* 選擇的最佳子節點，根據 UCT (Upper Confidence Bound for Trees) 選擇
 */
inline Node* selection(Node* node);

/**
 * @brief MCTS 回傳階段 (Backpropagation)
 *
 * @param node 當前節點，表示從模擬回溯的起點
 * @param endNode 模擬結束時的節點，代表遊戲最終狀態
 * @param isXTurn 是否是 X 的回合，`true` 表示 X 玩家回合，`false` 表示 O 玩家回合
 * @param win 模擬結果，`1` 表示該玩家勝利，`0` 表示平局，`-1` 表示該玩家敗北
 */
inline void backpropagation(Node* node, Node* endNode, bool isXTurn, double win);

/**
 * @brief MCTS 模擬階段 (Playout)
 *
 * @param node 當前節點，表示從該節點開始進行隨機模擬
 * @return int 模擬的結果，`1` 表示勝利，`0` 表示平局，`-1` 表示失敗
 */
inline int playout(Node* node);

#endif  // MCTS_HPP
