#include "MCTS.hpp"

#include <chrono>
#include <cmath>
#include <iostream>
#include <limits>
#include <random>
#include <set>

#include "Game.hpp"
#include "Node.hpp"

using namespace std;
random_device rd;         // 取得硬體隨機數
mt19937 generator(rd());  // 初始化隨機數生成器
double UCBCalculation(int ParentVisits, int NodeVisits, int NodeWins) {
    if (NodeVisits == 0) {
        return static_cast<double>(INT_MAX);
    }
    return static_cast<double>(NodeWins) / static_cast<double>(NodeVisits) +
           1.41 * sqrt(log(static_cast<double>(ParentVisits)) /
                       static_cast<double>(NodeVisits));
}
void MCTS(Node* root, int iterations) {
    auto start = std::chrono::high_resolution_clock::now();
    // 記錄開始時間
    for (int i = 0; i < iterations; i++) {
        if (root->isTerminal) {
            break;
        }
        Node* SelectedNode = Selection(root);  // 選擇best leaf node
        if (SelectedNode->Visits != 0) {
            SelectedNode = Expansion(SelectedNode);
        }
        int playoutResult =
            Playout(SelectedNode);  // 模擬遊戲 1 = selected node win, 0 = draw,
                                    // -1 = selected node lose
        Backpropagation(SelectedNode, SelectedNode->isXTurn,
                        playoutResult);  // 傳遞結果並更新節點資訊
    }
    auto end = std::chrono::high_resolution_clock::now();  // 記錄結束時間
    auto duration =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "MCTS completed in " << duration.count() << " milliseconds."
              << std::endl;
}

Node* Selection(Node* node) {              // select the best leaf node
    while (true) {                         // 一直選到leaf node
        if (node->Children.empty()) {      // 當前節點沒有子節點時，該節點為leaf
            return node;                   // node，返回該節點
        } else {                           // 如果當前節點有子節點
            Node* bestChild = nullptr;     // 紀錄最佳子節點
            double bestValue = -100000.0;  // 紀錄最佳 UCB 值
            for (auto child : node->Children) {  // 遍歷所有子節點
                if (!child->isTerminal) {        // 如果子節點不是終結節點
                    double ucbValue = UCBCalculation(
                        node->Visits, child->Visits, child->Wins);
                    // 更新最佳節點
                    if (ucbValue > bestValue) {
                        bestValue = ucbValue;
                        bestChild = child;
                    }
                }
            }
            // 如果至少一個子節點不是terminal，向下移動到該節點，繼續進行while
            // loop
            if (bestChild) {
                node = bestChild;

            } else {  // 如果所有子節點都是terminal，回溯到父節點繼續尋找，並將該節點設為terminal

                node->isTerminal = true;
                if (node->Parent == nullptr) {  // 如果是根節點，直接返回
                    return node;

                } else {
                    // 如果不是根節點，回溯到父節點
                    node = node->Parent;
                }
            }
        }
    }
}

void Backpropagation(
    Node* node, bool isXTurn,
    int win) {  // leaf node; leaf node's turn; win, lose, or draw
    while (node != nullptr) {
        node->Visits++;
        // 如果與 leaf node 的 turn 相符且贏，或者與 leaf node
        // 相反且對方輸，則增加勝利次數
        if ((node->isXTurn == isXTurn && win == 1) ||
            (node->isXTurn != isXTurn && win == -1)) {
            node->Wins += 2;
        } else if (win == 0) {  // 平手 for test
            node->Wins += 1;
        } else {
            node->Wins += 0;
        }
        node = node->Parent;
    }
}

Node* Expansion(Node* node) {
    set<Position> usedMoves;  // 紀錄已經使用過的走步
    Node* TempNode = node;
    while (TempNode->Parent != nullptr) {
        usedMoves.insert(TempNode->Move);
        TempNode = TempNode->Parent;
    }
    if (usedMoves.size() ==
        9) {  // 最後一個節點不進行任何操作，isTerminal=True等到Playout再做操作
        return node;
    }
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (usedMoves.find({i, j}) == usedMoves.end()) {
                Node* newNode = new Node({i, j}, node);
                node->Children.push_back(newNode);
            }
        }
    }
    return node->Children[0];
}

int Playout(Node* node) {  // 在該node的回合開始遊戲，回傳值為node方的勝負關係：
                           // 1 win, 0 draw, -1 lose
    vector<vector<int>> board(3, vector<int>(3, 0));  // 0: empty, 1: X, -1: O
    Node* TempNode = node;
    bool StartTurn = node->isXTurn, CurrentTurn = node->isXTurn;
    // 將節點的走步依序放入棋盤(前置作業)
    while (TempNode->Parent != nullptr) {
        board[TempNode->Move.X][TempNode->Move.Y] = TempNode->isXTurn ? 1 : -1;
        TempNode = TempNode->Parent;
    }
    if (CheckWin(
            board,
            StartTurn)) {  // 如果節點的走步已經獲勝，也就是該路徑已有結果，設定isTerminal為true
        node->isTerminal = true;
        return 1;
    }
    // 建立可能的走步
    vector<Position> possibleMoves;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == 0) {
                possibleMoves.push_back({i, j});
            }
        }
    }
    if (possibleMoves
            .empty()) {  // 該node為最終節點，棋盤已滿，平手，並設置為isTerminal=true
        node->isTerminal = true;
        return 0;
    }
    // 將節點的走步依序放入棋盤(後續作業)
    while (!possibleMoves.empty()) {
        // 隨機選擇一個走步
        CurrentTurn = !CurrentTurn;  // 換手
        uniform_int_distribution<int> distribution(0, possibleMoves.size() - 1);
        int moveIndex = distribution(generator);
        Position move = possibleMoves[moveIndex];
        possibleMoves.erase(possibleMoves.begin() + moveIndex);
        board[move.X][move.Y] = CurrentTurn ? 1 : -1;  // 將走步放入棋盤
        // 檢查是否有玩家獲勝
        if (possibleMoves.size() <= 4 &&
            CheckWin(
                board,
                CurrentTurn)) {  // 空格在四個以下才有可能獲勝，只需檢查下棋那方是否獲勝
            if (CurrentTurn ==
                StartTurn) {  // 勝利方為開始下棋的那方=node方獲勝
                return 1;
            } else {  // 勝利方為非開始下棋的那方=node方輸
                return -1;
            }
        }
    }
    // 平手
    return 0;
}