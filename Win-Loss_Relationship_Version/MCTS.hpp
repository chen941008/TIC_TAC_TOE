#ifndef MCTS_HPP
#define MCTS_HPP

#include <vector>

// 前向聲明 Node 結構體，避免需要包含整個定義
struct Node;

/**
 * @brief 計算 UCB (Upper Confidence Bound) 值
 *
 * @param ParentVisits 父節點訪問次數
 * @param NodeVisits 當前節點訪問次數
 * @param NodeWins 當前節點獲勝次數
 * @return double 計算出的 UCB 值
 */
double UCBCalculation(int ParentVisits, int NodeVisits, long long int NodeWins);

/**
 * @brief 執行 MCTS (Monte Carlo Tree Search) 演算法
 *
 * @param root MCTS 的根節點
 * @param iterations 演算法迭代次數
 */
void MCTS(Node* root, int iterations);

/**
 * @brief MCTS 選擇階段
 *
 * @param node 當前節點
 * @return Node* 選擇的子節點
 */
Node* Selection(Node* node);

/**
 * @brief MCTS 回傳階段 (Backpropagation)
 *
 * @param node 當前節點
 * @param isXTurn 是否為 X 的回合
 * @param win 獲勝與否
 */
void Backpropagation(Node* node, bool isXTurn, int win);

/**
 * @brief MCTS 擴展階段 (Expansion)
 *
 * @param node 當前節點
 * @return Node* 新增的子節點
 */
Node* Expansion(Node* node);

/**
 * @brief MCTS 模擬階段 (Playout)
 *
 * @param node 當前節點
 * @return int 模擬結果 (例如：勝利方或平局)
 */
int Playout(Node* node);

#endif  // MCTS_HPP
