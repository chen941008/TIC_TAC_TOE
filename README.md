# MCTS Tic-Tac-Toe 👉 [Read this in 中文](README.zh-TW.md)
This is an implementation of the Tic-Tac-Toe game based on the Monte Carlo Tree Search (MCTS) algorithm, designed to demonstrate how to apply MCTS for decision-making and game intelligence.

## Features
- Game decision-making based on the MCTS algorithm
- Support for X and O players to compete
- Support for game state checking and endgame determination

## Installation
1. Clone this project:
   ```bash
   git clone https://github.com/chen941008/TIC_TAC_TOE
   ```

## Build and Setup

This project uses CMake for compilation. Please follow the steps below to set up and build the project:

1. Make sure you have CMake installed (version 3.10 or above).
2. Create a build directory in the root of the project:
   ```bash
   mkdir build
   cd build
   ```
3. Configure the project using CMake:
   ```bash
   cmake ..
   ```
4. Build the project:
   ```bash
   cmake --build .
   ```
5. Run the project: After the build is completed, you should find the executable (e.g., `Simulation-only_version`) in the build directory. You can run it directly:
   ```bash
   ./Simulation-only_version
   ```

## Example
Assuming player X is the first player, the game process might look like this:

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


## Contributing
If you would like to contribute to this project, please follow these steps:
1. Fork this project
2. Create a new branch (`git checkout -b feature-branch`)
3. Commit your changes (`git commit -am 'Add new feature'`)
4. Push to the branch (`git push origin feature-branch`)
5. Submit a Pull Request

## Contact
If you have any questions, please contact [jack20051008@gmail.com](mailto:jack20051008@gmail.com)

