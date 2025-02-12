#ifndef NODE_HPP
#define NODE_HPP

#include <stdint.h>

#include <array>
#include <vector>
using std::array;

const int MAX_CHILDREN = 9;  ///< 每個節點最多的子節點數量（對應 3x3 棋盤）

enum class BoardState {
    PLAYING = 2,  ///< 遊戲進行中
    DRAW = 0,     ///< 平局
    WIN = 1,      ///< 有玩家獲勝
    LOSE = -1     ///< 有玩家輸了
};

/**
 * @brief 表示遊戲節點的結構體，用於蒙特卡洛樹搜索 (MCTS)
 *
 * 這個結構體代表遊戲樹中的每個節點，包含棋盤狀態、勝利次數、訪問次數等資訊。
 * 每個節點可以有多個子節點，並且每個節點有對應的父節點，這些節點構成了整個蒙特卡洛樹。
 * 節點使用位棋盤 (bitboard) 來表示當前的棋盤狀態，其中：
 * - `boardX` 記錄 X 玩家的落子位置
 * - `boardO` 記錄 O 玩家的落子位置
 * - `isXTurn` 記錄當前是否輪到 X 玩家
 */
struct Node {
    double wins;                   ///< 該節點的獲勝次數
    int visits;                    ///< 該節點的訪問次數
    BoardState state;              ///< 棋盤的狀態（勝利、平局或遊戲進行中）
    uint16_t boardX;               ///< 棋盤狀態（X 玩家的位棋盤）
    uint16_t boardO;               ///< 棋盤狀態（O 玩家的位棋盤）
    bool isXTurn;                  ///< 是否是 X 玩家的回合
    Node* parent;                  ///< 指向父節點的指標
    Node* children[MAX_CHILDREN];  ///< 指向子節點的指標陣列

    /**
     * @brief 預設構造函數，初始化根節點
     *
     * 該構造函數用於初始化根節點：
     * - 勝利次數 (`wins`) 設為 0
     * - 訪問次數 (`visits`) 設為 0
     * - 棋盤狀態 (`boardX` 和 `boardO`) 設為 0（表示棋盤為空）
     * - `isXTurn` 設為 `false`（假設 O 先手）
     * - 父節點 (`parent`) 設為 `nullptr`
     * - 所有子節點 (`children`) 初始化為空指標
     */
    Node() : wins(0), visits(0), parent(nullptr), boardX(0), boardO(0), isXTurn(false), state(BoardState::PLAYING) {
        std::fill(std::begin(children), std::end(children), nullptr);
    }

    /**
     * @brief 構造函數，用於創建子節點
     *
     * 這個構造函數基於父節點創建子節點：
     * - `move` 表示該子節點對應的棋盤移動位置
     * - 新節點會繼承父節點的棋盤狀態 (`boardX` 和 `boardO`)
     * - `isXTurn` 會與父節點相反，表示輪流落子
     * - 根據 `isXTurn`，將 `move` 位置標記到對應的棋盤 (`boardX` 或 `boardO`)
     *
     * @param move 該節點對應的棋盤移動位置 (0~8)，表示當前玩家落子的格子
     * @param parent 指向父節點的指標，表示該子節點由哪個父節點衍生
     */
    Node(uint16_t move, Node* parent)
        : wins(0), visits(0), parent(parent), isXTurn(!parent->isXTurn), state(BoardState::PLAYING) {
        boardX = parent->boardX;
        boardO = parent->boardO;

        // 根據當前輪次的玩家更新棋盤
        if (isXTurn) {
            boardX |= (1 << move);
        } else {
            boardO |= (1 << move);
        }

        std::fill(std::begin(children), std::end(children), nullptr);  // 初始化所有子節點為空指標
    }
};

/**
 * @brief 刪除整棵樹，釋放所有節點的記憶體
 *
 * @param node 根節點
 */
inline void deleteTree(Node* node) {
    if (node == nullptr) return;

    for (int i = 0; i < MAX_CHILDREN; i++) {
        if (node->children[i] != nullptr) {
            deleteTree(node->children[i]);
            node->children[i] = nullptr;  // 防止重複刪除
        }
    }

    delete node;
}

#endif  // NODE_HPP
