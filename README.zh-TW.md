# MCTS Tic-Tac-Toe 👉 [用英文閱讀](README.md)
這是一個基於蒙地卡羅樹搜尋（MCTS）算法的井字遊戲實作，旨在展示如何應用 MCTS 進行決策和遊戲智能。

## 功能
- 基於 MCTS 演算法進行遊戲決策
- 支援 X 和 O 玩家對戰
- 支援遊戲狀態檢查與終局判定

## 安裝
1. 克隆此專案：
   ```bash
   git clone https://github.com/chen941008/TIC_TAC_TOE
   ```

## 編譯與設置

這個專案使用 CMake 來進行編譯。請遵循以下步驟來設置和編譯專案：

1. 確保已安裝 CMake（版本 3.10 或以上）。
2. 在專案的根目錄下創建一個編譯目錄：
    ```bash
   mkdir build
   cd build
   ```
3. 使用 CMake 配置專案：
    ```bash
    cmake ..
    ```
4. 編譯專案：
    ```bash
    cmake --build .
    ```
5. 執行專案： 編譯完成後，你應該會在 build 目錄下看到可執行檔案（例如 Simulation-only_version）。你可以直接執行它：
    ```bash
    ./Simulation-only_version
    ```

## 範例
假設玩家 X 是先手，遊戲過程可能如下：

Input how many game you want to play.
1
Choose AI simulation mode: 1 = fixed simulation times, 2 = variable simulation times
1
Input how many iteration you want to run (must be greater than 10).
600000
Choose first or second player, input 1 or 2 

1

|   |   |   |
|---|---|---|
|   |   |   |
|   |   |   |

Your turn
input X Y 0~2  

0 2

|   |   | X |
|---|---|---|
|   |   |   |
|   |   |   |

AI turn  

AI choose 1 1  

|   |   | X |
|---|---|---|
|   | O |   |
|   |   |   |

Your turn  

input X Y 0~2  

1 2  

|   |   | X |
|---|---|---|
|   | O | X |
|   |   |   |

AI turn  

AI choose 2 2  

|   |   | X |
|---|---|---|
|   | O | X |
|   |   | O |

Your turn  

input X Y 0~2  

0 0  

| X |   | X |
|---|---|---|
|   | O | X |
|   |   | O |

AI turn  

AI choose 0 1  

| X | O | X |
|---|---|---|
|   | O | X |
|   |   | O |  

Your turn  

input X Y 0~2  

2 1  

| X | O | X |
|---|---|---|
|   | O | X |
|   | X | O |

AI turn  

AI choose 1 0  

| X | O | X |
|---|---|---|
| O | O | X |
|   | X | O |

Your turn  

input X Y 0~2  

2 0  

Draw  

| X | O | X |
|---|---|---|
| O | O | X |
| X | X | O |

## 貢獻
如果您希望為此專案做出貢獻，請按照以下步驟：
1. Fork 此專案
2. 創建一個新的分支 (`git checkout -b feature-branch`)
3. 提交更改 (`git commit -am 'Add new feature'`)
4. 推送到分支 (`git push origin feature-branch`)
5. 提交 Pull Request

## 聯絡
如果您有任何問題，請聯絡 [jack20051008@gmail.com](mailto:jack20051008@gmail.com)
