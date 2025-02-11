#ifndef MCTS_HPP
#define MCTS_HPP
#include <cstdint>
struct xoshiro256ss {
    uint64_t s[4];

    xoshiro256ss(uint64_t seed) {
        s[0] = seed;
        s[1] = seed ^ 0x9E3779B97F4A7C15;
        s[2] = seed ^ 0xBB67AE8584C2D8E7;
        s[3] = seed ^ 0xC2B2AE3D27D4EB4F;
    }

    uint64_t next() {
        uint64_t result = rotl64(s[0] + s[3], 23) + s[0];
        uint64_t t = s[1] << 17;
        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];
        s[2] ^= t;
        return result;
    }

   private:
    static uint64_t rotl64(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
};
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
inline Node* selection(Node* node);

/**
 * @brief MCTS 回傳階段 (Backpropagation)
 *
 * @param node 當前節點
 * @param endNode 結束節點
 * @param isXTurn 是否是 X 的回合
 * @param win 模擬結果，表示勝利或敗北
 */
inline void backpropagation(Node* node, Node* endNode, bool isXTurn, double win);

/**
 * @brief MCTS 模擬階段 (Playout)
 *
 * @param node 當前節點
 * @return int 模擬的結果 (1 表示勝利，0 表示平局，-1 表示失敗)
 */
inline int playout(Node* node);

#endif  // MCTS_HPP
