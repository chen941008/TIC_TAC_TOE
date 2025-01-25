#ifndef NODE_HPP
#define NODE_HPP

#include <array>
#include <vector>

using std::array;

/// @brief 定義棋盤上的位置結構體
///
/// 用來表示棋盤上的一個位置，由 `X` 和 `Y` 兩個整數構成，
/// 其中 `X` 代表行，`Y` 代表列。此結構體還包含了自訂的 `<` 比較運算子，
/// 用來在需要排序或插入集合（例如 `std::set`）時使用。
struct Position {
    int X, Y;  ///< 棋盤上的行和列，`X` 代表行，`Y` 代表列

    /// @brief 比較運算符，用於比較兩個 `Position` 的大小
    ///
    /// 根據 `X` 值進行比較，如果 `X` 值相等，則比較 `Y` 值。
    /// 適用於排序容器（如 `std::set`）中的位置排序。
    /// @param other 另一個要比較的 `Position`
    /// @return 如果當前 `Position` 小於 `other`，則返回 `true`
    bool operator<(const Position& other) const {
        if (X == other.X) {
            return Y < other.Y;  ///< 如果 `X` 相等，則比較 `Y`
        }
        return X < other.X;  ///< 否則，根據 `X` 進行比較
    }
};

/**
 * @brief 表示遊戲的節點，用於蒙地卡羅樹搜索
 */
const int MAX_CHILDREN = 9;  ///< 每個節點最多的子節點數量
struct Node {
    double Wins;                   ///< 該節點的獲勝次數
    int Visits;                    ///< 該節點的訪問次數
    Node* Parent;                  ///< 指向父節點的指標
    Node* Children[MAX_CHILDREN];  ///< 儲存所有子節點
    Position Move;                 ///< 該節點代表的棋盤上的移動
    bool isTerminal;               ///< 該節點是否為終局狀態
    bool isXTurn;                  ///< 是否輪到 X 玩家行動

    /**
     * @brief 預設構造函數，用於創建根節點
     */
    Node()
        : Wins(0),
          Visits(0),
          Parent(nullptr),
          isTerminal(false),
          isXTurn(false) {
        std::fill(std::begin(Children), std::end(Children), nullptr);
        Move = Position{-1, -1};
    }

    /**
     * @brief 構造函數，用於創建子節點
     *
     * @param move 該節點代表的移動
     * @param parent 指向父節點的指標
     */
    Node(Position move, Node* parent)
        : Wins(0),
          Visits(0),
          Parent(parent),
          Move(move),
          isTerminal(false),
          isXTurn(!parent->isXTurn) {
        std::fill(std::begin(Children), std::end(Children),
                  nullptr);  // 初始化所有子節點為空指標
    }
};
inline void deleteTree(Node* node) {
    if (node == nullptr) return;

    // 先遞歸刪除子節點
    for (Node* child : node->Children) {
        if (child != nullptr) {
            deleteTree(child);
        }
    }

    // 最後刪除當前節點
    delete node;
}
#endif  // NODE_HPP
