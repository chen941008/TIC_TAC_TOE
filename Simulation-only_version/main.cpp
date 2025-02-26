#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>

#include "Game.hpp"
#include "MCTS.hpp"
#include "Node.hpp"
#include "ThreadPool.hpp"
using namespace std;
int main() {
    /*
    cout << "Welcome to Tic-Tac-Toe" << endl;
    int gameTimes;
    cout << "Input how many game you want to play." << endl;
    cin >> gameTimes;
    for (int i = 0; i < gameTimes; i++) {
        Game::startGame();
    }
        */
    int gameTimes = 30;
    long long totalTime = 0;
    ofstream outputFile("parallelPlayouts(2_core).csv");
    if (!outputFile.is_open()) {
        cerr << "Error: Unable to open output file!" << endl;
        return 1;
    }
    // 寫入 CSV 標題行
    outputFile << "core,SimulationTimes,AverageTime (ms)" << endl;
    // 模擬不同次數的情況
    for (int Thread = 1; Thread <= 8; Thread += 2) {
        for (int simulationTimes = 1; simulationTimes <= 1000000000; simulationTimes *= 10) {
            totalTime = 0;
            for (int i = 0; i < gameTimes; i++) {
                Node* root = new Node();           // 創建根節點
                Game::generateFullTree(root);      // 生成完整遊戲樹
                MCTS ai(simulationTimes, Thread);  // 創建 MCTS AI
                totalTime += ai.run(root, 10000);  // 執行 MCTS
                deleteTree(root);                  // 刪除樹
            }
            // 計算平均時間
            double average_time = totalTime / static_cast<double>(gameTimes);
            // 輸出到 CSV 檔案
            outputFile << Thread << "," << simulationTimes << "," << average_time << endl;
            // 同時輸出到控制台（可選）
            cout << "core = " << Thread << ", SimulationTimes = " << simulationTimes
                 << ", AverageTime = " << average_time << " ms" << endl;
        }
    }

    // 關閉檔案
    outputFile.close();
    return 0;
}