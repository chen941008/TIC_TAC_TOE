#include "MCTS.hpp"

#include <stdint.h>

#include <atomic>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <set>
#include <thread>
#include <vector>

#include "Game.hpp"
#include "Node.hpp"

using namespace std;

MCTS::MCTS(int simTimes) : simulationTimes(simTimes), generator(std::random_device{}()) {}

void MCTS::run(Node* root, int iterations) {
    for (int i = 0; i < iterations; i++) {
        Node* selectedNode = selection(root);
        if (selectedNode->state == BoardState::WIN) {
            backpropagation(selectedNode, root->parent, selectedNode->isXTurn, static_cast<double>(BoardState::WIN));
            continue;
        }
        if (selectedNode->state == BoardState::DRAW) {
            backpropagation(selectedNode, root->parent, selectedNode->isXTurn, static_cast<double>(BoardState::DRAW));
            continue;
        }
        double playoutResult = 0.0;
        for (int i = 0; i < simulationTimes; i++) {
            playoutResult += playout(selectedNode);
        }
        playoutResult /= simulationTimes;
        backpropagation(selectedNode, root->parent, selectedNode->isXTurn, playoutResult);
    }
}

Node* MCTS::selection(Node* node) {
    while (true) {
        if (node->children[0] == nullptr) {
            return node;
        } else {
            Node* bestChild = nullptr;
            double bestValue = std::numeric_limits<double>::lowest();
            double logParent = log(node->visits);
            for (int i = 0; i < MAX_CHILDREN && node->children[i] != nullptr; i++) {
                Node* child = node->children[i];
                if (child->visits == 0) {
                    return child;
                }
                double ucbValue = (child->wins / child->visits) + COEFFICIENT * sqrt(logParent / child->visits);
                if (ucbValue > bestValue) {
                    bestValue = ucbValue;
                    bestChild = child;
                }
            }
            node = bestChild;
        }
    }
}

void MCTS::backpropagation(Node* node, Node* endNode, bool isXTurn, double win) {
    while (node != endNode) {
        node->visits++;
        if (isXTurn == node->isXTurn) {
            node->wins += win;
        } else {
            node->wins -= win;
        }
        node = node->parent;
    }
}

int MCTS::playout(Node* node) {
    uint16_t boardX = node->boardX;
    uint16_t boardO = node->boardO;
    bool startTurn = node->isXTurn;
    bool currentTurn = startTurn;

    uint16_t usedPositions = boardX | boardO;
    uint16_t availableMoves = ~usedPositions & 0b111111111;

    int possibleMoves[MAX_CHILDREN];
    int count = 0;
    for (int i = 0; i < MAX_CHILDREN; i++) {
        if (availableMoves & (1 << i)) {
            possibleMoves[count++] = i;
        }
    }

    for (int i = count - 1; i > 0; --i) {
        int j = generator() % (i + 1);
        std::swap(possibleMoves[i], possibleMoves[j]);
    }

    for (int i = count - 1; i >= 0; i--) {
        currentTurn = !currentTurn;
        int move = possibleMoves[i];

        if (currentTurn) {
            boardX |= (1 << move);
        } else {
            boardO |= (1 << move);
        }

        if (i <= CHECKWIN_THRESHOLD && Game::checkWin(boardX, boardO, currentTurn)) {
            return (currentTurn == startTurn) ? static_cast<int>(BoardState::WIN) : static_cast<int>(BoardState::LOSE);
        }
    }
    return static_cast<int>(BoardState::DRAW);
}
