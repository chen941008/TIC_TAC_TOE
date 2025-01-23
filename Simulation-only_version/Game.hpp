#ifndef GAME_HPP
#define GAME_HPP

#include <vector>

using std::vector;

/**
 * @brief 遊戲主邏輯控制
 */
void Game();

/**
 * @brief 檢查當前棋盤是否有玩家獲勝
 *
 * @param board 棋盤的二維向量表示
 * @param PlayTurn 當前玩家 (true 表示 X 玩家, false 表示 O 玩家)
 * @return true 如果當前玩家獲勝
 * @return false 如果當前玩家未獲勝
 */
bool CheckWin(int board[3][3], bool PlayTurn);

/**
 * @brief 輸出棋盤的當前狀態
 *
 * @param board 棋盤的二維向量表示
 */
void printBoard(int board[3][3]);

#endif  // GAME_HPP
