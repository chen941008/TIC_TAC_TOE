#include <vector>
using namespace std;
struct Position {
    int X, Y;
    bool operator<(const Position& other) const {
        if (X == other.X) {
            return Y < other.Y;
        }
        return X < other.X;
    }
};

struct Node {
    int Wins, Visits;
    Node* Parent;
    vector<Node*> Children;
    Position Move;
    bool isTerminal, isXTurn;

    // root node
    Node()
        : Wins(0),
          Visits(0),
          Parent(nullptr),
          isTerminal(false),
          isXTurn(false) {
        Move = Position{-1, -1};
    }

    // child node
    Node(Position move, Node* parent)
        : Wins(0),
          Visits(0),
          Parent(parent),
          Move(move),
          isTerminal(false),
          isXTurn(!parent->isXTurn) {}
    ~Node() {
        for (Node* child : Children) {
            delete child;
        }
        Children.clear();
        Parent = nullptr;  // 清除指向父節點的指標
    }
};

double UCBCalculation(int ParentVisits, int NodeVisits, int NodeWins);
void MCTS(Node* root, int iterations);
void printBoard(vector<vector<int>>& board);
Node* Selection(Node* node);
void Backpropagation(Node* node, bool isXTurn, int win);
Node* Expansion(Node* node);
int Playout(Node* node);
bool CheckWin(vector<vector<int>>& board, bool PlayTurn);
void Game();