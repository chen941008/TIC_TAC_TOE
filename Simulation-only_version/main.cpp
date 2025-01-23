#include <chrono>
#include <fstream>
#include <iostream>

#include "Game.hpp"
#include "MCTS.hpp"
#include "Node.hpp"

using namespace std;
int main() {
    /*
    int GameTimes;
    cout << "Input how many game you want to play." << endl;
    cin >> GameTimes;
    for (int i = 0; i < GameTimes; i++) {
        Game();
    }
    */
    int GameTimes = 1, total_time = 0;

    // 創建一個 CSV 檔案來保存結果
    ofstream outputFile("simulation_results.csv");
    if (!outputFile.is_open()) {
        cerr << "Error: Unable to open output file!" << endl;
        return 1;
    }

    // 寫入 CSV 標題行
    outputFile << "SimulationTimes,AverageTime (ms)" << endl;

    // 模擬不同次數的情況
    for (simulationTimes = 1; simulationTimes <= 1; simulationTimes++) {
        total_time = 0;
        for (int i = 0; i < GameTimes; i++) {
            Node* root = new Node();  // 創建根節點
            total_time += MCTS(root, 1000000);
            delete root;
        }

        // 計算平均時間
        double average_time = total_time / static_cast<double>(GameTimes);

        // 輸出到 CSV 檔案
        outputFile << simulationTimes << "," << average_time << endl;

        // 同時輸出到控制台（可選）
        cout << "SimulationTimes = " << simulationTimes
             << ", AverageTime = " << average_time << " ms" << endl;
    }

    // 關閉檔案
    outputFile.close();
    return 0;
}