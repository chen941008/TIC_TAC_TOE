#ifndef MCTS_HPP
#define MCTS_HPP

// 前向聲明 Node 結構體，避免需要包含整個定義
struct Node;

/**
 * @brief 計算 UCB (Upper Confidence Bound) 值
 *
 * @param parentVisits 父節點的訪問次數
 * @param nodeVisits 當前節點的訪問次數
 * @param nodeWins 當前節點的勝利次數
 * @return double 計算得到的 UCB 值
 */
double calculateUCB(int parentVisits, int nodeVisits, double nodeWins);

/**
 * @brief 執行 MCTS (Monte Carlo Tree Search) 演算法
 *
 * @param root MCTS 演算法的根節點
 * @param iterations 演算法的迭代次數
 */
void MCTS(Node* root, int iterations);

/**
 * @brief MCTS 選擇階段
 *
 * @param node 當前節點
 * @return Node* 選擇的最佳子節點
 */
Node* selection(Node* node);

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

/**
 * @brief 根據節點子節點的終局狀態設置當前節點的終局狀態
 *
 * 該函數通過檢查節點的所有子節點的 `terminalState`，根據優先級
 * （LOSS > DRAW > WIN）選擇適當的狀態並設置給當前節點。
 * 如果存在子節點的狀態為 LOSS，則當前節點狀態設為 WIN；
 * 如果所有子節點都沒有 LOSS，但有 DRAW，則設為 DRAW；
 * 如果所有子節點均為 WIN，則設為 LOSS。
 *
 * @param node 指向需要設置終局狀態的節點指標
 */
void setTerminalState(Node* node);

#endif  // MCTS_HPP
