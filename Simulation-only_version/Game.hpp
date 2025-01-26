#ifndef GAME_HPP
#define GAME_HPP

#include <vector>

using std::vector;

/**
 * @brief 遊戲的主邏輯控制函式
 */
void startGame();

/**
 * @brief 檢查當前棋盤是否有玩家獲勝
 *
 * @param board 棋盤的二維陣列表示
 * @param playTurn 當前玩家的回合 (true 表示 X 玩家, false 表示 O 玩家)
 * @return true 如果當前玩家獲勝
 * @return false 如果當前玩家未獲勝
 */
bool checkWin(int board[3][3], bool playTurn);

/**
 * @brief 輸出棋盤的當前狀態
 *
 * @param board 棋盤的二維陣列表示
 */
void printBoard(int board[3][3]);

#endif  // GAME_HPP
