#ifndef NODE_HPP
#define NODE_HPP

#include <array>
#include <vector>

using std::array;

/**
 * @brief 表示遊戲的終局狀態
 *
 * 此列舉類型用於描述遊戲在某個節點的結束狀態，包括以下三種：
 * - WIN: 表示該節點對應的玩家勝利。
 * - DRAW: 表示該節點對應的遊戲平局。
 * - NonTerminal: 表示該節點對應的遊戲未結束。
 */
enum terminalState {
    WIN = 1,         ///< 玩家勝利
    DRAW = 0,        ///< 遊戲平局
    NonTerminal = 2  ///< 遊戲未結束
};
/**
 * @brief 表示遊戲節點的結構體，用於蒙地卡羅樹搜索
 */
const int MAX_CHILDREN = 9;  ///< 每個節點最多的子節點數量
struct Node {
    double wins;                   ///< 該節點的獲勝次數
    int visits;                    ///< 該節點的訪問次數
    Node* parent;                  ///< 指向父節點的指標
    Node* children[MAX_CHILDREN];  ///< 儲存該節點的所有子節點
    int move;                      ///< 該節點對應的棋盤移動
    terminalState state;           ///< 該節點是否為終局狀態
    bool isXTurn;                  ///< 是否輪到 X 玩家行動

    /**
     * @brief 預設構造函數，初始化根節點
     */
    Node() : wins(0), visits(0), parent(nullptr), state(NonTerminal), isXTurn(false), move(-1) {
        std::fill(std::begin(children), std::end(children), nullptr);
    }

    /**
     * @brief 構造函數，用於創建子節點
     *
     * @param move 該節點對應的棋盤移動
     * @param parent 指向父節點的指標
     */
    Node(int move, Node* parent)
        : wins(0), visits(0), parent(parent), move(move), state(NonTerminal), isXTurn(!(parent->isXTurn)) {
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
