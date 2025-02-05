#ifndef NODE_HPP
#define NODE_HPP

/// @brief 定義棋盤上位置的結構體
///
/// 用來表示棋盤上的一個位置，包含 `X`（行）和 `Y`（列）。
/// 此結構體也自訂了 `<` 比較運算子，適用於需要排序或插入集合（如
/// `std::set`）的情況。
struct Position {
    int X, Y;  ///< 棋盤位置，`X` 代表行，`Y` 代表列

    /// @brief 比較運算符，用於比較兩個 `Position` 的大小
    ///
    /// 先根據 `X` 值比較，若 `X` 值相等，再比較 `Y` 值。
    /// 適用於排序容器（如 `std::set`）中的位置排序。
    /// @param other 另一個 `Position` 物件
    /// @return 如果當前 `Position` 小於 `other`，則返回 `true`
    bool operator<(const Position& other) const {
        if (X == other.X) {
            return Y < other.Y;  ///< `X` 相等時比較 `Y`
        }
        return X < other.X;  ///< 根據 `X` 比較
    }
};

/**
 * @brief 表示遊戲的終局狀態
 *
 * 此列舉類型用於描述遊戲在某個節點的結束狀態，包括以下三種：
 * - WIN: 表示該節點對應的玩家勝利。
 * - LOSE: 表示該節點對應的玩家失敗。
 * - DRAW: 表示該節點對應的遊戲平局。
 */
enum terminalState {
    WIN = 1,    ///< 玩家勝利
    LOSE = -1,  ///< 玩家失敗
    DRAW = 0,    ///< 遊戲平局
    NonTerminal = 2 ///< 遊戲未結束
};

/**
 * @brief 表示遊戲節點的結構體，用於蒙地卡羅樹搜索
 */
const int MAX_CHILDREN = 9;  ///< 每個節點最多的子節點數量
struct Node {
    double wins;                   ///< 該節點的獲勝次數
    terminalState state;           ///< 該節點的終局狀態
    int visits;                    ///< 該節點的訪問次數
    Node* parent;                  ///< 指向父節點的指標
    Node* children[MAX_CHILDREN];  ///< 儲存該節點的所有子節點
    Position move;                 ///< 該節點對應的棋盤移動
    bool isTerminal;               ///< 該節點是否為終局狀態
    bool isXTurn;                  ///< 是否輪到 X 玩家行動

    /**
     * @brief 預設構造函數，初始化根節點
     */
    Node()
        : wins(0),
          state(NonTerminal),
          visits(0),
          parent(nullptr),
          isTerminal(false),
          isXTurn(false) {
        std::fill(std::begin(children), std::end(children), nullptr);
        move = Position{-1, -1};
    }

    /**
     * @brief 構造函數，用於創建子節點
     *
     * @param move 該節點對應的棋盤移動
     * @param parent 指向父節點的指標
     */
    Node(Position move, Node* parent)
        : wins(0),
          state(NonTerminal),
          visits(0),
          parent(parent),
          move(move),
          isTerminal(false),
          isXTurn(!(parent->isXTurn)) {
        std::fill(std::begin(children), std::end(children),
                  nullptr);  // 初始化所有子節點為空指標
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
