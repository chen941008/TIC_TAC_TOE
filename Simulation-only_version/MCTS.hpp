#ifndef MCTS_HPP
#define MCTS_HPP

// 前向聲明 Node 結構體，避免需要包含整個定義
struct Node;
extern int simulationTimes;

/**
 * @brief 執行 MCTS (Monte Carlo Tree Search) 演算法
 *
 * @param root MCTS 演算法的根節點
 * @param iterations 演算法的迭代次數
 * @return int 演算法執行所花費的時間（毫秒）
 */
int MCTS(Node* root, int iterations);

/**
 * @brief MCTS 選擇階段
 *
 * @param node 當前節點
 * @return Node* 選擇的最佳子節點
 */
Node* selection(Node* node);

/**
 * @brief MCTS 回傳階段 (Backpropagation)
 *
 * @param node 當前節點
 * @param isXTurn 是否是 X 的回合
 * @param win 模擬結果，表示勝利或敗北
 */
void backpropagation(Node* node, bool isXTurn, double win);

/**
 * @brief MCTS 擴展階段 (Expansion)
 *
 * @param node 當前節點
 * @return Node* 新增的子節點
 */
Node* expansion(Node* node);

/**
 * @brief MCTS 模擬階段 (Playout)
 *
 * @param node 當前節點
 * @return int 模擬的結果 (1 表示勝利，0 表示平局，-1 表示失敗)
 */
int playout(Node* node);

#endif  // MCTS_HPP
