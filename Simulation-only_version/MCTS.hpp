#ifndef MCTS_HPP
#define MCTS_HPP
#include <random>
struct Node;
class MCTS {
   public:
    MCTS(int simTimes, int numThreads);   // 構造函數聲明
    int run(Node* root, int iterations);  // run 方法聲明

   private:
    const double COEFFICIENT = 1.41;
    int simulationTimes;
    int numThreads;
    // ThreadPool threadPool;
    std::mt19937 generator;

    Node* selection(Node* node);
    void backpropagation(Node* node, Node* endNode, bool isXTurn, double win);
    int playout(Node* node);
    double parallelPlayouts(int thread, int simulationTimes, Node* node);
};

#endif