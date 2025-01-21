#include "test.hpp"

#include <algorithm>
#include <climits>
#include <cmath>
#include <iostream>
#include <queue>
#include <random>
#include <set>
#include <vector>
int K = 1;  // 調整分數的比例係數
int dealt;
double Ratio = 0.05;      // 調整勝率和訪問次數的比例
random_device rd;         // 取得硬體隨機數
mt19937 generator(rd());  // 初始化隨機數生成器
double UCBCalculation(int ParentVisits, int NodeVisits,
                      long long int NodeWins) {
    if (NodeVisits == 0) {
        return static_cast<double>(INT_MAX);
    }
    return static_cast<double>(NodeWins) / static_cast<double>(NodeVisits) +
           1.41 * sqrt(log(static_cast<double>(ParentVisits)) /
                       static_cast<double>(NodeVisits));
}

void printBoard(vector<vector<int>>& board) {
    cout << endl;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == 1) {
                cout << " X ";
            } else if (board[i][j] == -1) {
                cout << " O ";
            } else {
                cout << "   ";  // 空格使用 " "，讓棋盤看起來更清爽
            }
            if (j < 2) cout << "|";  // 每一行列之間加上分隔線
        }
        cout << endl;
        if (i < 2) cout << "-----------" << endl;  // 每一行之後加上分隔線
    }
    cout << endl;
}

void MCTS(Node* root, int iterations) {
    for (int i = 0; i < iterations; i++) {
        Node* SelectedNode = Selection(root);  // 選擇best leaf node
        if (root->isTerminal) {
            // cout << "root is terminal" << endl;
            break;
        }
        if (SelectedNode->Visits != 0) {
            SelectedNode = Expansion(SelectedNode);
        }
        int playoutResult =
            Playout(SelectedNode);  // 模擬遊戲  >0  selected node win, 0 =
                                    // draw, <0 selected node lose
        Backpropagation(SelectedNode, SelectedNode->isXTurn,
                        playoutResult);  // 傳遞結果並更新節點資訊
    }
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
        if ((node->isXTurn == isXTurn && win > 0) ||
            (node->isXTurn != isXTurn && win < 0)) {
            node->Wins += abs(win);
        } else if (win == 0) {  // 平手 for test
            node->Wins += K;
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
bool CheckWin(vector<vector<int>>& board, bool PlayTurn) {
    int player = PlayTurn ? 1 : -1;
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == player && board[i][1] == player &&
            board[i][2] == player) {
            return true;
        }
        if (board[0][i] == player && board[1][i] == player &&
            board[2][i] == player) {
            return true;
        }
    }
    if (board[0][0] == player && board[1][1] == player &&
        board[2][2] == player) {
        return true;
    }
    if (board[0][2] == player && board[1][1] == player &&
        board[2][0] == player) {
        return true;
    }
    return false;
}
int calculateReward(
    int result,
    int currentRound) {  // 調整輸贏分數，越早獲勝分數越高，越早輸分數越低
    return K * (currentRound + 1) * result;
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

    // 建立可能的走步
    vector<Position> possibleMoves;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == 0) {
                possibleMoves.push_back({i, j});
            }
        }
    }
    if (CheckWin(
            board,
            StartTurn)) {  // 如果節點的走步已經獲勝，也就是該路徑已有結果，設定isTerminal為true
        node->isTerminal = true;
        return calculateReward(1, possibleMoves.size());
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
                return calculateReward(1, possibleMoves.size());
            } else {  // 勝利方為非開始下棋的那方=node方輸
                return calculateReward(-1, possibleMoves.size());
            }
        }
    }
    // 平手
    return 0;
}
void Game(Node* root, int GameTimes) {
    for (int i = 0; i < GameTimes; i++) {
        int PlayerOrder, CurrentOrder = 0;
        Node* CurrentNode =
            root;  // CurrentNode為當前棋盤最後一個子的節點，會去選擇他的子節點來下棋
        vector<vector<int>> board(3, vector<int>(3, 0));
        cout << "Choose first or second player, input 1 or 2" << endl;
        while (true) {  // 選擇先手或後手，防白痴crash程式
            cin >> PlayerOrder;
            if (PlayerOrder == 1 || PlayerOrder == 2) {
                break;
            }
            cout << "Please input 1 or 2" << endl;
        }
        PlayerOrder--;
        while (true) {
            if (CurrentOrder == 9) {
                cout << "Draw" << endl;
                printBoard(board);
                break;
            }
            printBoard(board);
            if (CurrentOrder % 2 == PlayerOrder) {  // Player turn
                cout << "Your turn" << endl;
                int X, Y;
                cout << "input X Y 0~2" << endl;
                while (true) {
                    cin >> X >> Y;
                    if (X < 0 || X > 2 || Y < 0 || Y > 2) {
                        cout << "Please input 0~2" << endl;
                        continue;
                    }
                    if (board[X][Y] != 0) {
                        cout << "This position is already taken" << endl;
                        continue;
                    }
                    break;
                }
                board[X][Y] = PlayerOrder == 0 ? 1 : -1;
                if (CheckWin(board, PlayerOrder == 0)) {
                    cout << "You win" << endl;
                    printBoard(board);
                    break;
                }

                if (!CurrentNode->Children
                         .empty()) {  // 如果有子節點，選擇玩家所下的節點，否則直接跳過=無法選擇
                    for (auto child :
                         CurrentNode->Children) {  // 選擇玩家所下的節點
                        if (child->Move.X == X && child->Move.Y == Y) {
                            CurrentNode = child;
                            break;
                        }
                    }
                }
            } else {  // AI turn

                Node* bestChild = nullptr;  // 紀錄最佳子節點
                double bestWinRate =
                    -std::numeric_limits<double>::infinity();  // 紀錄最佳
                                                               // win rate
                for (auto child : CurrentNode->Children) {     // 遍歷所有子節點
                    double winRate = static_cast<double>(child->Wins) /
                                     static_cast<double>(child->Visits);
                    // 更新最佳節點
                    if (winRate > bestWinRate) {
                        bestWinRate = winRate;
                        bestChild = child;
                    }
                }
                cout << "AI choose " << bestChild->Move.X << " "
                     << bestChild->Move.Y << endl;
                board[bestChild->Move.X][bestChild->Move.Y] =
                    PlayerOrder == 0 ? -1 : 1;
                CurrentNode = bestChild;
                if (CheckWin(board, PlayerOrder == 1)) {
                    cout << "AI win" << endl;
                    printBoard(board);
                    break;
                }
            }
            CurrentOrder++;
        }
        cout << "Game " << i + 1 << " finish" << endl;
    }
}
int showWinRate(Node* root, Position currectMove) {
    vector<vector<int>> board(3, vector<int>(3, 0));
    Node* TempNode = root;
    while (TempNode->Parent != nullptr) {
        board[TempNode->Move.X][TempNode->Move.Y] = TempNode->isXTurn ? 1 : -1;
        TempNode = TempNode->Parent;
    }
    // printBoard(board);
    Node* bestChild = nullptr;
    double bestScore = -std::numeric_limits<double>::infinity();

    for (auto child : root->Children) {
        // 計算綜合分數：考慮勝率、訪問次數和路徑長度
        double winRate = static_cast<double>(child->Wins) /
                         static_cast<double>(child->Visits);
        double visitRatio = static_cast<double>(child->Visits) /
                            static_cast<double>(root->Visits);
        double score =
            Ratio * winRate + (1 - Ratio) * visitRatio;  // 可調整權重

        if (score > bestScore) {
            bestScore = score;
            bestChild = child;
        }
    }

    // cout << "Best Move " << bestChild->Move.X << " " << bestChild->Move.Y
    //      << " bestScore: " << bestScore << endl;

    if (bestChild->Move.X == currectMove.X &&
        bestChild->Move.Y == currectMove.Y) {
        return 1;
    }
    return 0;
}
int showWinRate2(Node* root) {
    Node* bestChild = nullptr;
    double bestScore = -std::numeric_limits<double>::infinity();
    for (auto child : root->Children) {
        // 計算綜合分數：考慮勝率、訪問次數和路徑長度
        double winRate = static_cast<double>(child->Wins) /
                         static_cast<double>(child->Visits);
        double visitRatio = static_cast<double>(child->Visits) /
                            static_cast<double>(root->Visits);
        double score =
            Ratio * winRate + (1 - Ratio) * visitRatio;  // 可調整權重

        if (score > bestScore) {
            bestScore = score;
            bestChild = child;
        }
    }
    if (bestChild->Move.X == 0 && bestChild->Move.Y == 1) {
        return 1;
    }
    if (bestChild->Move.X == 1 && bestChild->Move.Y == 0) {
        return 1;
    }
    if (bestChild->Move.X == 1 && bestChild->Move.Y == 2) {
        return 1;
    }
    if (bestChild->Move.X == 2 && bestChild->Move.Y == 1) {
        return 1;
    }
    return 0;
}
struct ans {
    int dealt, k;
    double ratio;
};
int main() {
    /*
    int RunTimes;
    cout << "input MCTS simulation times" << endl;
    cin >> RunTimes;
    Node* root = new Node();
    MCTS(root, RunTimes);
    */
    vector<ans> ansList;
    Node* root = new Node();
    MCTS(root, 550000);
    for (dealt = 1; dealt < 50; dealt += 1) {
        for (K = 1; K < 50; K += 1) {
            for (Ratio = 0.05; Ratio < 0.51; Ratio += 0.05) {
                int currectAns = 0;
                for (int i = 0; i < 1; i++) {
                    currectAns += showWinRate(root->Children[4]
                                                  ->Children[0]
                                                  ->Children[1]
                                                  ->Children[3]
                                                  ->Children[1],
                                              {1, 2});
                    // cout << "currect move is 1 2" << endl;
                    currectAns += showWinRate(root->Children[4]
                                                  ->Children[0]
                                                  ->Children[4]
                                                  ->Children[1]
                                                  ->Children[0],
                                              {2, 1});
                    // cout << "currect move is 2 1" << endl;
                    currectAns += showWinRate(root->Children[4]
                                                  ->Children[2]
                                                  ->Children[0]
                                                  ->Children[5]
                                                  ->Children[2],
                                              {1, 0});
                    // cout << "currect move is 1 0" << endl;
                    currectAns += showWinRate(root->Children[4]
                                                  ->Children[2]
                                                  ->Children[6]
                                                  ->Children[0]
                                                  ->Children[0],
                                              {2, 1});
                    // cout << "currect move is 2 1" << endl;
                    currectAns += showWinRate(root->Children[4]
                                                  ->Children[5]
                                                  ->Children[0]
                                                  ->Children[5]
                                                  ->Children[4],
                                              {0, 1});
                    // cout << "currect move is 0 1" << endl;
                    currectAns += showWinRate(root->Children[4]
                                                  ->Children[5]
                                                  ->Children[6]
                                                  ->Children[0]
                                                  ->Children[2],
                                              {1, 2});
                    // cout << "currect move is 1 2" << endl;
                    currectAns += showWinRate(root->Children[4]
                                                  ->Children[7]
                                                  ->Children[2]
                                                  ->Children[4]
                                                  ->Children[4],
                                              {0, 1});
                    // cout << "currect move is 0 1" << endl;
                    currectAns += showWinRate(root->Children[4]
                                                  ->Children[7]
                                                  ->Children[5]
                                                  ->Children[2]
                                                  ->Children[3],
                                              {1, 0});
                    // cout << "currect move is 1 0" << endl;
                    currectAns += showWinRate2(
                        root->Children[0]->Children[3]->Children[6]);
                    currectAns += showWinRate2(
                        root->Children[8]->Children[4]->Children[0]);
                    currectAns += showWinRate2(
                        root->Children[2]->Children[3]->Children[4]);
                    currectAns += showWinRate2(
                        root->Children[6]->Children[4]->Children[2]);
                }
                if (currectAns == 60) {
                    ansList.push_back({dealt, K, Ratio});
                }
                cout << "K = " << K << ", Dealt = " << dealt
                     << ", ratio = " << Ratio << ", currect rate is "
                     << static_cast<double>(currectAns) / 60.0 << endl;
            }
        }
    }
    cout << "正確率為1的有:" << endl;
    for (auto i : ansList) {
        cout << "K = " << i.k << ", Dealt = " << i.dealt
             << ", ratio = " << i.ratio << endl;
    }
    /*
    cout << "input how many games you want to play" << endl;
    int GameTimes;
    cin >> GameTimes;
    Game(root, GameTimes);
    cout << "finish" << endl;
    */
    return 0;
}