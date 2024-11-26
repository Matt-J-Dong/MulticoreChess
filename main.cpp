// main.cpp
#include <iostream>
#include <limits>
#include <string>
#include <cstdlib> // For std::atoi
#include "Backend/Board.h"
#include "Backend/Type/Square.h"
#include "SimplifiedMoveList.h"
#include "Backend/Type/Color.h"
#include "Engine.h"
#include <omp.h>
#include <fstream> // For file I/O
#include <iomanip> // For formatting output
#include <atomic>

constexpr int maxDepth = 25;

// Function to convert a Square enum to its string representation (e.g., E2 -> "e2")
std::string squareToString(Square square) {
    return std::string(1, File(square)) + std::string(1, Rank(square));
}

// Function to display usage instructions
void printUsage(const std::string &programName) {
    std::cerr << "Usage: " << programName << " <depth>\n";
    std::cerr << "  <depth> : Positive integer specifying the search depth.\n";
    std::cerr << "Example:\n";
    std::cerr << "  " << programName << " 4\n";
}

// Function to display the algorithm options list
void displayAlgorithmOptions() {
    std::cout << "Choose Search Algorithm:\n";
    std::cout << "1. Sequential Minimax\n";
    std::cout << "2. Parallel Minimax\n";
    std::cout << "3. Sequential Alpha Beta\n";
    std::cout << "4. Naive Parallel Alpha Beta\n";
    std::cout << "5. Naive Parallel Alpha Beta with PV\n";
    std::cout << "6. Young Brothers Wait Concept (YBWC)\n";
    std::cout << "7. Principal Variation Search (PVS)\n";
    std::cout << "8. testing function\n";
    std::cout << "Enter your choice (1 to 8): ";
}

int main(int argc, char* argv[]) {
    // Check if the depth argument is provided
    if (argc != 2) {
        std::cerr << "Error: Incorrect number of arguments.\n";
        printUsage(argv[0]);
        return 1;
    }

    // Parse the depth from the first command-line argument
    int depth = std::atoi(argv[1]);

    // Validate the depth
    if (depth <= 0) {
        std::cerr << "Invalid depth: " << depth << ". Depth must be a positive integer.\n";
        printUsage(argv[0]);
        return 1;
    }

    // Display algorithm options and get user choice
    int algorithmChoice = 0;
    while (true) {
        displayAlgorithmOptions();
        std::cin >> algorithmChoice;

        // Check for input failure (e.g., non-integer input)
        if (std::cin.fail()) {
            std::cin.clear(); // Clear the error flags
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Discard invalid input
            std::cerr << "Invalid input. Please enter 1, or 2.\n";
            continue;
        }

        if (algorithmChoice == 1 || algorithmChoice == 2 || algorithmChoice == 3 || algorithmChoice == 4 || algorithmChoice == 5 || algorithmChoice == 6 || algorithmChoice == 7 || algorithmChoice == 8) {
            break; // Valid choice
        } else {
            std::cerr << "Invalid choice: " << algorithmChoice << ". Please enter 1 to 8.\n";
        }
    }

    // Map the choice to algorithm name
    std::string algorithmName;
    switch (algorithmChoice) {
        case 1:
            algorithmName = "Sequential Minimax";
            break;
        case 2:
            algorithmName = "Parallel Minimax";
            break;
        case 3:
            algorithmName = "Sequential Alpha Beta";
            break;
        case 4:
            algorithmName = "Naive Parallel Alpha Beta";
            break;
        case 5:
            algorithmName = "Naive Parallel Alpha Beta with PV";
            break;
        case 6:
            algorithmName = "Young Brothers Wait Concept (YBWC)";
            break;
        case 7:
            algorithmName = "Principal Variation Search (PVS)";
            break;
        case 8:
            algorithmName = "Testing Function";
            break;
        default:
            // This case should never occur due to the earlier validation
                algorithmName = "Unknown Algorithm";
            break;
    }

    std::cout << "Starting " << algorithmName << " with depth: " << depth << "\n";

    // Initialize the chess board with the standard starting position
    std::string FEN = "";
    StockDory::Board chessBoard;
    int nThreads = 1;
    if (algorithmChoice != 8) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Input FEN (input 0 no FEN): ";
        std::getline(std::cin, FEN);
        if (FEN.compare("0") == 0) {
            chessBoard = StockDory::Board();
        }
        else {
            chessBoard = StockDory::Board(FEN);
        }
        std::cout << "Input number of threads (only applies to parallel algorithms, we tested betweeen 1-64): ";
        std::cin >> nThreads;
    }

    Engine engine;

    // Determine which color is to move
    Color currentPlayer = chessBoard.ColorToMove();

    omp_set_num_threads(nThreads);

    // Define a pair to hold the result (best move sequence and its score)
    std::pair<std::array<Move, maxDepth>, int> result;
    double tstart = 0.0, tend=0.0, ttaken;
    // Execute the appropriate search algorithm based on the user's choice and current player
    if (algorithmChoice == 1) { // seq minimax
        if (currentPlayer == White) {
            // Perform minimax for White
            tstart = omp_get_wtime();
            result = engine.minimax<White, maxDepth>(
                chessBoard,
                depth
            );
            tend = omp_get_wtime();
            ttaken = tend-tstart;
            printf("Time taken for main part: %f\n", ttaken);
            // Check if there is at least one move in the sequence
            if (!result.first.empty()) {
                Move bestMove = result.first.front();
                std::cout << "White's Best Move (seq minimax): "
                          << squareToString(bestMove.From()) << " to "
                          << squareToString(bestMove.To())
                          << " with score " << result.second << "\n";

                // Print the entire sequence of moves (best line)
                std::cout << "Best Line: ";
                for (const Move &move : result.first) {
                    std::cout << squareToString(move.From()) << " to "
                              << squareToString(move.To()) << ", ";
                }
                std::cout << "\n";
            } else {
                std::cout << "No moves available for White.\n";
            }
        }
        else if (currentPlayer == Black) {
            // Perform minimax for Black
            tstart = omp_get_wtime();
            result = engine.minimax<Black, maxDepth>(
                chessBoard,
                depth
            );
            tend = omp_get_wtime();
            ttaken = tend-tstart;
            printf("Time taken for main part: %f\n", ttaken);
            // Check if there is at least one move in the sequence
            if (!result.first.empty()) {
                Move bestMove = result.first.front();
                std::cout << "Black's Best Move (seq minimax): "
                          << squareToString(bestMove.From()) << " to "
                          << squareToString(bestMove.To())
                          << " with score " << result.second << "\n";

                // Print the entire sequence of moves (best line)
                std::cout << "Best Line: ";
                for (const Move &move : result.first) {
                    std::cout << squareToString(move.From()) << " to "
                              << squareToString(move.To()) << ", ";
                }
                std::cout << "\n";
            } else {
                std::cout << "No moves available for Black.\n";
            }
        }
    }
    else if (algorithmChoice == 2) { // para minimax
        if (currentPlayer == White) {
            tstart = omp_get_wtime();
            result = engine.parallelMinimax<White, maxDepth>(
                chessBoard,
                depth
            );
            tend = omp_get_wtime();
            ttaken = tend-tstart;
            printf("Time taken for main part: %f\n", ttaken);
            // Check if there is at least one move in the sequence
            if (!result.first.empty()) {
                Move bestMove = result.first.front();
                std::cout << "White's Best Move (para minimax): "
                          << squareToString(bestMove.From()) << " to "
                          << squareToString(bestMove.To())
                          << " with score " << result.second << "\n";

                // Print the entire sequence of moves (best line)
                std::cout << "Best Line: ";
                for (const Move &move : result.first) {
                    std::cout << squareToString(move.From()) << " to "
                              << squareToString(move.To()) << ", ";
                }
                std::cout << "\n";
            } else {
                std::cout << "No moves available for White.\n";
            }
        }
        else if (currentPlayer == Black) {
            tstart = omp_get_wtime();
            result = engine.parallelMinimax<Black, maxDepth>(
                chessBoard,
                depth
            );
            tend = omp_get_wtime();
            ttaken = tend-tstart;
            printf("Time taken for main part: %f\n", ttaken);
            // Check if there is at least one move in the sequence
            if (!result.first.empty()) {
                Move bestMove = result.first.front();
                std::cout << "Black's Best Move (seq minimax): "
                          << squareToString(bestMove.From()) << " to "
                          << squareToString(bestMove.To())
                          << " with score " << result.second << "\n";

                // Print the entire sequence of moves (best line)
                std::cout << "Best Line: ";
                for (const Move &move : result.first) {
                    std::cout << squareToString(move.From()) << " to "
                              << squareToString(move.To()) << ", ";
                }
                std::cout << "\n";
            } else {
                std::cout << "No moves available for Black.\n";
            }
        }
    }
    else if (algorithmChoice == 3) { // seq alpha beta
        if (currentPlayer == White) {
            tstart = omp_get_wtime();
            result = engine.alphaBetaNega<White, maxDepth>(
                chessBoard,
                -50000,
                50000,
                depth
            );
            tend = omp_get_wtime();
            ttaken = tend-tstart;
            printf("Time taken for main part: %f\n", ttaken);
            // Check if there is at least one move in the sequence
            if (!result.first.empty()) {
                Move bestMove = result.first.front();
                std::cout << "White's Best Move (seq AB): "
                          << squareToString(bestMove.From()) << " to "
                          << squareToString(bestMove.To())
                          << " with score " << result.second << "\n";

                // Print the entire sequence of moves (best line)
                std::cout << "Best Line: ";
                for (const Move &move : result.first) {
                    std::cout << squareToString(move.From()) << " to "
                              << squareToString(move.To()) << ", ";
                }
                std::cout << "\n";
            } else {
                std::cout << "No moves available for White.\n";
            }
        }
        else if (currentPlayer == Black) {
            tstart = omp_get_wtime();
            result = engine.alphaBetaNega<Black, maxDepth>(
                    chessBoard,
                    -50000,
                    50000,
                    depth
            );
            tend = omp_get_wtime();
            ttaken = tend-tstart;
            printf("Time taken for main part: %f\n", ttaken);
            // Check if there is at least one move in the sequence
            if (!result.first.empty()) {
                Move bestMove = result.first.front();
                std::cout << "Black's Best Move (seq AB): "
                          << squareToString(bestMove.From()) << " to "
                          << squareToString(bestMove.To())
                          << " with score " << result.second << "\n";

                // Print the entire sequence of moves (best line)
                std::cout << "Best Line: ";
                for (const Move &move : result.first) {
                    std::cout << squareToString(move.From()) << " to "
                              << squareToString(move.To()) << ", ";
                }
                std::cout << "\n";
            } else {
                std::cout << "No moves available for Black.\n";
            }
        }
    }
    else if (algorithmChoice == 4) { // naive parallel AB
        if (currentPlayer == White) {
            tstart = omp_get_wtime();
            result = engine.naiveParallelAlphaBeta<White, maxDepth>(
                chessBoard,
                -50000,
                50000,
                depth
            );
            tend = omp_get_wtime();
            ttaken = tend-tstart;
            printf("Time taken for main part: %f\n", ttaken);
            // Check if there is at least one move in the sequence
            if (!result.first.empty()) {
                Move bestMove = result.first.front();
                std::cout << "White's Best Move (naive parallel AB): "
                          << squareToString(bestMove.From()) << " to "
                          << squareToString(bestMove.To())
                          << " with score " << result.second << "\n";

                // Print the entire sequence of moves (best line)
                std::cout << "Best Line: ";
                for (const Move &move : result.first) {
                    std::cout << squareToString(move.From()) << " to "
                              << squareToString(move.To()) << ", ";
                }
                std::cout << "\n";
            } else {
                std::cout << "No moves available for White.\n";
            }
        }
        else if (currentPlayer == Black) {
            tstart = omp_get_wtime();
            result = engine.naiveParallelAlphaBeta<Black, maxDepth>(
                    chessBoard,
                    -50000,
                    50000,
                    depth
            );
            tend = omp_get_wtime();
            ttaken = tend-tstart;
            printf("Time taken for main part: %f\n", ttaken);
            // Check if there is at least one move in the sequence
            if (!result.first.empty()) {
                Move bestMove = result.first.front();
                std::cout << "Black's Best Move (naive parallel AB): "
                          << squareToString(bestMove.From()) << " to "
                          << squareToString(bestMove.To())
                          << " with score " << result.second << "\n";

                // Print the entire sequence of moves (best line)
                std::cout << "Best Line: ";
                for (const Move &move : result.first) {
                    std::cout << squareToString(move.From()) << " to "
                              << squareToString(move.To()) << ", ";
                }
                std::cout << "\n";
            } else {
                std::cout << "No moves available for Black.\n";
            }
        }
    }
    else if (algorithmChoice == 5) { // naive parallel AB with PV
        if (currentPlayer == White) {
            tstart = omp_get_wtime();
            result = engine.naiveParallelPVAlphaBeta<White, maxDepth>(
                chessBoard,
                -50000,
                50000,
                depth
            );
            tend = omp_get_wtime();
            ttaken = tend-tstart;
            printf("Time taken for main part: %f\n", ttaken);
            // Check if there is at least one move in the sequence
            if (!result.first.empty()) {
                Move bestMove = result.first.front();
                std::cout << "White's Best Move (naive parallel AB with PV): "
                          << squareToString(bestMove.From()) << " to "
                          << squareToString(bestMove.To())
                          << " with score " << result.second << "\n";

                // Print the entire sequence of moves (best line)
                std::cout << "Best Line: ";
                for (const Move &move : result.first) {
                    std::cout << squareToString(move.From()) << " to "
                              << squareToString(move.To()) << ", ";
                }
                std::cout << "\n";
            } else {
                std::cout << "No moves available for White.\n";
            }
        }
        else if (currentPlayer == Black) {
            tstart = omp_get_wtime();
            result = engine.naiveParallelPVAlphaBeta<Black, maxDepth>(
                    chessBoard,
                    -50000,
                    50000,
                    depth
            );
            tend = omp_get_wtime();
            ttaken = tend-tstart;
            printf("Time taken for main part: %f\n", ttaken);
            // Check if there is at least one move in the sequence
            if (!result.first.empty()) {
                Move bestMove = result.first.front();
                std::cout << "Black's Best Move (naive parallel AB with PV): "
                          << squareToString(bestMove.From()) << " to "
                          << squareToString(bestMove.To())
                          << " with score " << result.second << "\n";

                // Print the entire sequence of moves (best line)
                std::cout << "Best Line: ";
                for (const Move &move : result.first) {
                    std::cout << squareToString(move.From()) << " to "
                              << squareToString(move.To()) << ", ";
                }
                std::cout << "\n";
            } else {
                std::cout << "No moves available for Black.\n";
            }
        }
    }
    else if (algorithmChoice == 6) { // YBWC
        if (currentPlayer == White) {
            // Perform YBWC for White
            tstart = omp_get_wtime();
            result = engine.YBWC<White, maxDepth>(
                chessBoard,
                -50000,
                50000,
                depth
            );
            tend = omp_get_wtime();
            ttaken = tend-tstart;
            printf("Time taken for main part: %f\n", ttaken);
            // Check if there is at least one move in the sequence
            if (!result.first.empty()) {
                Move bestMove = result.first.front();
                std::cout << "White's Best Move (YBWC): "
                          << squareToString(bestMove.From()) << " to "
                          << squareToString(bestMove.To())
                          << " with score " << result.second << "\n";

                // Print the entire sequence of moves (best line)
                std::cout << "Best Line: ";
                for (const Move &move : result.first) {
                    std::cout << squareToString(move.From()) << " to "
                              << squareToString(move.To()) << ", ";
                }
                std::cout << "\n";
            } else {
                std::cout << "No moves available for White.\n";
            }
        }
        else if (currentPlayer == Black) {
            // Perform YBWC for Black
            tstart = omp_get_wtime();
            result = engine.YBWC<Black, maxDepth>(
                chessBoard,
                -50000,
                50000,
                depth
            );
            tend = omp_get_wtime();
            ttaken = tend-tstart;
            printf("Time taken for main part: %f\n", ttaken);
            // Check if there is at least one move in the sequence
            if (!result.first.empty()) {
                Move bestMove = result.first.front();
                std::cout << "Black's Best Move (YBWC): "
                          << squareToString(bestMove.From()) << " to "
                          << squareToString(bestMove.To())
                          << " with score " << result.second << "\n";

                // Print the entire sequence of moves (best line)
                std::cout << "Best Line: ";
                for (const Move &move : result.first) {
                    std::cout << squareToString(move.From()) << " to "
                              << squareToString(move.To()) << ", ";
                }
                std::cout << "\n";
            } else {
                std::cout << "No moves available for Black.\n";
            }
        }
    }
    else if (algorithmChoice == 7) { // PVS
        if (currentPlayer == White) {
            // Perform PVS for White
            tstart = omp_get_wtime();
            result = engine.PVS<White, maxDepth>(
                chessBoard,
                -50000,
                50000,
                depth
            );
            tend = omp_get_wtime();
            ttaken = tend-tstart;
            printf("Time taken for main part: %f\n", ttaken);
            // Check if there is at least one move in the sequence
            if (!result.first.empty()) {
                Move bestMove = result.first.front();
                std::cout << "White's Best Move (PVS): "
                          << squareToString(bestMove.From()) << " to "
                          << squareToString(bestMove.To())
                          << " with score " << result.second << "\n";

                // Print the entire sequence of moves (best line)
                std::cout << "Best Line: ";
                for (const Move &move : result.first) {
                    std::cout << squareToString(move.From()) << " to "
                              << squareToString(move.To()) << ", ";
                }
                std::cout << "\n";
            } else {
                std::cout << "No moves available for White.\n";
            }
        }
        else if (currentPlayer == Black) {
            // Perform PVS for Black
            tstart = omp_get_wtime();
            result = engine.PVS<Black, maxDepth>(
                chessBoard,
                -50000,
                50000,
                depth
            );
            tend = omp_get_wtime();
            ttaken = tend-tstart;
            printf("Time taken for main part: %f\n", ttaken);
            // Check if there is at least one move in the sequence
            if (!result.first.empty()) {
                Move bestMove = result.first.front();
                std::cout << "Black's Best Move (PVS): "
                          << squareToString(bestMove.From()) << " to "
                          << squareToString(bestMove.To())
                          << " with score " << result.second << "\n";

                // Print the entire sequence of moves (best line)
                std::cout << "Best Line: ";
                for (const Move &move : result.first) {
                    std::cout << squareToString(move.From()) << " to "
                              << squareToString(move.To()) << ", ";
                }
                std::cout << "\n";
            } else {
                std::cout << "No moves available for Black.\n";
            }
        }
    }
    else if (algorithmChoice == 8) {
        // Disable or enable the below line based on whether you wanted nested parallelism or not.
        // omp_set_nested(1);
        const char* mateIn3FENs[] = {
            "7k/8/3NK3/5BN1/8/8/8/8 w - - 0 1",
            "k7/3K4/3N4/2N5/8/3B4/8/8 w - - 0 1",
            "8/8/2K5/7r/6r1/8/6k1/8 b - - 0 1",
            "8/K7/7r/8/2k5/5bb1/8/8 b - - 0 1",
            "8/K7/P6r/8/2k5/5bb1/8/8 b - - 0 1",
            "8/8/8/8/k7/4Q3/3K4/8 w - - 0 1",
            "8/8/k7/2K5/8/2Q5/b1R5/n7 w - - 0 1",
            "8/8/k1K1b3/2n5/8/8/8/2R5 w - - 0 1",
            "8/7P/k1K1b3/2n5/8/8/8/2R5 w - - 0 1",
            "7k/7n/8/8/8/7B/7R/6RK w - - 0 1"
        };
        std::ofstream resultFile("results-m3.txt");
        if (!resultFile.is_open()) {
            std::cerr << "Error: Unable to open results.txt for writing\n";
            return 0;
        }
        std::cout << "Testing mate in 3 FENs\n";
        for (const char* fen : mateIn3FENs) {
            StockDory::Board chessBoard(fen);
            resultFile << "Current Fen: " << fen << "\n";
            std::cout << "Current Fen: " << fen << "\n" << std::endl;
            for (int depth = 5; depth < 7; depth++) {
                std::cout << "Testing depth: " << depth << "\n";
                if (chessBoard.ColorToMove() == White) {
                    int result = engine.minimaxMoveCounter<White>(
                        chessBoard,
                        depth
                    );
                    printf("Total number of moves in minimax: %d\n", result);
                    resultFile << "Minimax moves: " << result << "\n";
                }
                if (chessBoard.ColorToMove() == White) {
                    int count = 0;
                    std::pair<std::array<Move, maxDepth>, int> result = engine.alphaBetaNegaMoveCounter<White, maxDepth>(
                        chessBoard,
                        -50000,
                        50000,
                        depth,
                        count
                    );
                    printf("Total number of moves in alphaBeta: %d\n", count);
                    resultFile << "AlphaBeta moves: " << count << "\n";
                }
                if (chessBoard.ColorToMove() == Black) {
                    int result = engine.minimaxMoveCounter<Black>(
                        chessBoard,
                        depth
                    );
                    printf("Total number of moves in minimax: %d\n", result);
                    resultFile << "Minimax moves: " << result << "\n";
                }
                if (chessBoard.ColorToMove() == Black) {
                    int count = 0;
                    std::pair<std::array<Move, maxDepth>, int> result = engine.alphaBetaNegaMoveCounter<Black, maxDepth>(
                        chessBoard,
                        -50000,
                        50000,
                        depth,
                        count
                    );
                    printf("Total number of moves in alphaBeta: %d\n", count);
                    resultFile << "AlphaBeta moves: " << count << "\n";
                }
                std::cout << "Algorithm: Sequential Minimax\n" << std::endl;
                double totalTime = 0;
                for (int i = 0; i < 20; i++) {
                    if (chessBoard.ColorToMove() == White) {
                         tstart = omp_get_wtime();
                         std::pair<std::array<Move, maxDepth>, int> result = engine.minimax<White, maxDepth>(
                             chessBoard,
                             depth
                         );
                         tend = omp_get_wtime();
                         ttaken = tend - tstart;
                         totalTime += ttaken;

                         printf("Time taken for main part minimax: %f\n", ttaken);

                         if (!result.first.empty()) {
                             Move bestMove = result.first.front();
                             std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
                                       << " with score " << result.second << "\n";

                             std::cout << "Best line: ";
                             for (int i = 0; i < depth; i++) {
                                 Move move = result.first[i];
                                 std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
                             }
                             std::cout << "\n";
                         } else {
                             std::cout << "No moves available for White.\n";
                         }
                     }
                    else if (chessBoard.ColorToMove() == Black) {
                        tstart = omp_get_wtime();
                        std::pair<std::array<Move, maxDepth>, int> result = engine.minimax<Black, maxDepth>(
                            chessBoard,
                            depth
                        );
                        tend = omp_get_wtime();
                        ttaken = tend - tstart;
                        totalTime += ttaken;

                        printf("Time taken for main part minimax: %f\n", ttaken);

                        if (!result.first.empty()) {
                            Move bestMove = result.first.front();
                            std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
                                      << " with score " << result.second << "\n";

                            std::cout << "Best line: ";
                            for (int i = 0; i < depth; i++) {
                                Move move = result.first[i];
                                std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
                            }
                            std::cout << "\n";
                        } else {
                            std::cout << "No moves available for White.\n";
                        }
                    }
                }
                double averageTime = totalTime/20;
                std::cout << "Average time for minimax in 20 iterations is: " << averageTime << "\n";
                resultFile << "Sequential Minimax,1," << averageTime << "\n";

                int numThreads[] = {1, 2, 4, 8, 16, 32, 64};

                for (int threads : numThreads) {
                    omp_set_num_threads(threads);
                    std::cout << "Algorithm: Parallel Minimax\n" << std::endl;
                    std::cout << "Total number of threads: " << threads << "\n" << std::endl;
                    totalTime = 0;
                    for (int i = 0; i < 20; i++) {
                        if (chessBoard.ColorToMove() == White) {
                            tstart = omp_get_wtime();
                            std::pair<std::array<Move, maxDepth>, int> result = engine.parallelMinimax<White, maxDepth>(
                                chessBoard,
                                depth
                            );
                            tend = omp_get_wtime();
                            ttaken = tend - tstart;
                            totalTime += ttaken;

                            printf("Time taken for main part parallel minimax: %f\n", ttaken);

                            if (!result.first.empty()) {
                                Move bestMove = result.first.front();
                                std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
                                          << " with score " << result.second << "\n";

                                std::cout << "Best line: ";
                                for (int i = 0; i < depth; i++) {
                                    Move move = result.first[i];
                                    std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
                                }
                                std::cout << "\n";
                            } else {
                                std::cout << "No moves available for White.\n";
                            }
                        }
                        else if (chessBoard.ColorToMove() == Black) {
                            tstart = omp_get_wtime();
                            std::pair<std::array<Move, maxDepth>, int> result = engine.parallelMinimax<Black, maxDepth>(
                                chessBoard,
                                depth
                            );
                            tend = omp_get_wtime();
                            ttaken = tend - tstart;
                            totalTime += ttaken;

                            printf("Time taken for main part parallel minimax: %f\n", ttaken);

                            if (!result.first.empty()) {
                                Move bestMove = result.first.front();
                                std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
                                          << " with score " << result.second << "\n";

                                std::cout << "Best line: ";
                                for (int i = 0; i < depth; i++) {
                                    Move move = result.first[i];
                                    std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
                                }
                                std::cout << "\n";
                            } else {
                                std::cout << "No moves available for White.\n";
                            }
                        }
                    }
                    averageTime = totalTime/20;
                    std::cout << "Average time for parallel minimax in 20 iterations is: " << averageTime << " with " << threads << " threads" << "\n";
                    resultFile << "Parallel Minimax," << threads << "," << averageTime << "\n";
                }

                std::cout << "Algorithm: Sequential AlphaBeta\n" << std::endl;
                totalTime = 0;
                for (int i = 0; i < 20; i++) {
                    if (chessBoard.ColorToMove() == White) {
                        tstart = omp_get_wtime();
                        std::pair<std::array<Move, maxDepth>, int> result = engine.alphaBetaNega<White, maxDepth>(
                        chessBoard,
                        -50000,
                        50000,
                            depth
                        );
                        tend = omp_get_wtime();
                        ttaken = tend - tstart;
                        totalTime += ttaken;

                        printf("Time taken for main part sequential alpha beta: %f\n", ttaken);

                        if (!result.first.empty()) {
                            Move bestMove = result.first.front();
                            std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
                                      << " with score " << result.second << "\n";

                            std::cout << "Best line: ";
                            for (int i = 0; i < depth; i++) {
                                Move move = result.first[i];
                                std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
                            }
                            std::cout << "\n";
                        } else {
                            std::cout << "No moves available for White.\n";
                        }
                    }
                    else if (chessBoard.ColorToMove() == Black) {
                        tstart = omp_get_wtime();
                        std::pair<std::array<Move, maxDepth>, int> result = engine.alphaBetaNega<Black, maxDepth>(
                        chessBoard,
                        -50000,
                        50000,
                            depth
                        );
                        tend = omp_get_wtime();
                        ttaken = tend - tstart;
                        totalTime += ttaken;

                        printf("Time taken for main part sequential alpha beta: %f\n", ttaken);

                        if (!result.first.empty()) {
                            Move bestMove = result.first.front();
                            std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
                                      << " with score " << result.second << "\n";

                            std::cout << "Best line: ";
                            for (int i = 0; i < depth; i++) {
                                Move move = result.first[i];
                                std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
                            }
                            std::cout << "\n";
                        } else {
                            std::cout << "No moves available for White.\n";
                        }
                    }
                }
                averageTime = totalTime/20;
                std::cout << "Average time for sequential AlphaBeta in 20 iterations is: " << averageTime << "\n";
                resultFile << "Sequential AlphaBeta,1," << averageTime << "\n";

                for (int threads : numThreads) {
                    omp_set_num_threads(threads);
                    std::cout << "Algorithm: Naive Alpha Beta Parallel\n" << std::endl;

                    std::cout << "Total number of threads: " << threads << "\n" << std::endl;
                    totalTime = 0;
                    for (int i = 0; i < 20; i++) {
                        if (chessBoard.ColorToMove() == White) {
                            tstart = omp_get_wtime();
                            std::pair<std::array<Move, maxDepth>, int> result = engine.naiveParallelAlphaBeta<White, maxDepth>(
                                chessBoard,
                                -50000,
                                50000,
                                depth
                            );
                            tend = omp_get_wtime();
                            ttaken = tend - tstart;
                            totalTime += ttaken;

                            printf("Time taken for main part naive alpha beta parallel: %f\n", ttaken);

                            if (!result.first.empty()) {
                                Move bestMove = result.first.front();
                                std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
                                          << " with score " << result.second << "\n";

                                std::cout << "Best line: ";
                                for (int i = 0; i < depth; i++) {
                                    Move move = result.first[i];
                                    std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
                                }
                                std::cout << "\n";
                            } else {
                                std::cout << "No moves available for White.\n";
                            }
                        }
                        else if (chessBoard.ColorToMove() == Black) {
                            tstart = omp_get_wtime();
                            std::pair<std::array<Move, maxDepth>, int> result = engine.naiveParallelAlphaBeta<Black, maxDepth>(
                                chessBoard,
                                -50000,
                                50000,
                                depth
                            );
                            tend = omp_get_wtime();
                            ttaken = tend - tstart;
                            totalTime += ttaken;

                            printf("Time taken for main part naive alpha beta parallel: %f\n", ttaken);

                            if (!result.first.empty()) {
                                Move bestMove = result.first.front();
                                std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
                                          << " with score " << result.second << "\n";

                                std::cout << "Best line: ";
                                for (int i = 0; i < depth; i++) {
                                    Move move = result.first[i];
                                    std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
                                }
                                std::cout << "\n";
                            } else {
                                std::cout << "No moves available for White.\n";
                            }
                        }
                    }
                    averageTime = totalTime/20;
                    std::cout << "Average time for naive alpha beta parallel in 20 iterations is: " << averageTime << " with " << threads << " threads" << "\n";
                    resultFile << "Naive Parallel Alpha Beta," << threads << "," << averageTime << "\n";
                }

                for (int threads : numThreads) {
                    omp_set_num_threads(threads);
                    std::cout << "Algorithm: Naive Alpha Beta Parallel\n" << std::endl;

                    std::cout << "Total number of threads: " << threads << "\n" << std::endl;
                    totalTime = 0;
                    for (int i = 0; i < 20; i++) {
                        if (chessBoard.ColorToMove() == White) {
                            tstart = omp_get_wtime();
                            std::pair<std::array<Move, maxDepth>, int> result = engine.naiveParallelPVAlphaBeta<White, maxDepth>(
                                chessBoard,
                                -50000,
                                50000,
                                depth
                            );
                            tend = omp_get_wtime();
                            ttaken = tend - tstart;
                            totalTime += ttaken;

                            printf("Time taken for main part naive alpha beta parallel with PV combo: %f\n", ttaken);

                            if (!result.first.empty()) {
                                Move bestMove = result.first.front();
                                std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
                                          << " with score " << result.second << "\n";

                                std::cout << "Best line: ";
                                for (int i = 0; i < depth; i++) {
                                    Move move = result.first[i];
                                    std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
                                }
                                std::cout << "\n";
                            } else {
                                std::cout << "No moves available for White.\n";
                            }
                        }
                        else if (chessBoard.ColorToMove() == Black) {
                            tstart = omp_get_wtime();
                            std::pair<std::array<Move, maxDepth>, int> result = engine.naiveParallelPVAlphaBeta<Black, maxDepth>(
                                chessBoard,
                                -50000,
                                50000,
                                depth
                            );
                            tend = omp_get_wtime();
                            ttaken = tend - tstart;
                            totalTime += ttaken;

                            printf("Time taken for main part naive alpha beta parallel with PV combo: %f\n", ttaken);

                            if (!result.first.empty()) {
                                Move bestMove = result.first.front();
                                std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
                                          << " with score " << result.second << "\n";

                                std::cout << "Best line: ";
                                for (int i = 0; i < depth; i++) {
                                    Move move = result.first[i];
                                    std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
                                }
                                std::cout << "\n";
                            } else {
                                std::cout << "No moves available for White.\n";
                            }
                        }
                    }
                    averageTime = totalTime/20;
                    std::cout << "Average time for naive alpha beta parallel with YBWC combo in 20 iterations is: " << averageTime << " with " << threads << " threads" << "\n";
                    resultFile << "Naive Parallel Alpha Beta with PV," << threads << "," << averageTime << "\n";
                }

                for (int threads : numThreads) {
                    omp_set_num_threads(threads);
                    std::cout << "Algorithm: YBWC\n" << std::endl;

                    std::cout << "Total number of threads: " << threads << "\n" << std::endl;
                    totalTime = 0;
                    for (int i = 0; i < 20; i++) {
                        if (chessBoard.ColorToMove() == White) {
                            tstart = omp_get_wtime();
                            std::pair<std::array<Move, maxDepth>, int> result = engine.YBWC<White, maxDepth>(
                                chessBoard,
                                -50000,
                                50000,
                                depth
                            );
                            tend = omp_get_wtime();
                            ttaken = tend - tstart;
                            totalTime += ttaken;

                            printf("Time taken for main part YBWC: %f\n", ttaken);

                            if (!result.first.empty()) {
                                Move bestMove = result.first.front();
                                std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
                                          << " with score " << result.second << "\n";

                                std::cout << "Best line: ";
                                for (int i = 0; i < depth; i++) {
                                    Move move = result.first[i];
                                    std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
                                }
                                std::cout << "\n";
                            } else {
                                std::cout << "No moves available for White.\n";
                            }
                        }
                        else if (chessBoard.ColorToMove() == Black) {
                            tstart = omp_get_wtime();
                            std::pair<std::array<Move, maxDepth>, int> result = engine.YBWC<Black, maxDepth>(
                                chessBoard,
                                -50000,
                                50000,
                                depth
                            );
                            tend = omp_get_wtime();
                            ttaken = tend - tstart;
                            totalTime += ttaken;

                            printf("Time taken for main part YBWC: %f\n", ttaken);

                            if (!result.first.empty()) {
                                Move bestMove = result.first.front();
                                std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
                                          << " with score " << result.second << "\n";

                                std::cout << "Best line: ";
                                for (int i = 0; i < depth; i++) {
                                    Move move = result.first[i];
                                    std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
                                }
                                std::cout << "\n";
                            } else {
                                std::cout << "No moves available for White.\n";
                            }
                        }
                    }
                    averageTime = totalTime/20;
                    std::cout << "Average time for YBWC in 20 iterations is: " << averageTime << " with " << threads << " threads" << "\n";
                    resultFile << "YBWC," << threads << "," << averageTime << "\n";
                }

                for (int threads : numThreads) {
                    omp_set_num_threads(threads);
                    std::cout << "Algorithm: PVS\n" << std::endl;

                    std::cout << "Total number of threads: " << threads << "\n" << std::endl;
                    totalTime = 0;
                    for (int i = 0; i < 20; i++) {
                        if (chessBoard.ColorToMove() == White) {
                            tstart = omp_get_wtime();
                            std::pair<std::array<Move, maxDepth>, int> result = engine.PVS<White, maxDepth>(
                                chessBoard,
                                -50000,
                                50000,
                                depth
                            );
                            tend = omp_get_wtime();
                            ttaken = tend - tstart;
                            totalTime += ttaken;

                            printf("Time taken for main part PVS: %f\n", ttaken);

                            if (!result.first.empty()) {
                                Move bestMove = result.first.front();
                                std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
                                          << " with score " << result.second << "\n";

                                std::cout << "Best line: ";
                                for (int i = 0; i < depth; i++) {
                                    Move move = result.first[i];
                                    std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
                                }
                                std::cout << "\n";
                            } else {
                                std::cout << "No moves available for White.\n";
                            }
                        }
                        else if (chessBoard.ColorToMove() == Black) {
                            tstart = omp_get_wtime();
                            std::pair<std::array<Move, maxDepth>, int> result = engine.PVS<Black, maxDepth>(
                                chessBoard,
                                -50000,
                                50000,
                                depth
                            );
                            tend = omp_get_wtime();
                            ttaken = tend - tstart;
                            totalTime += ttaken;

                            printf("Time taken for main part PVS: %f\n", ttaken);

                            if (!result.first.empty()) {
                                Move bestMove = result.first.front();
                                std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
                                          << " with score " << result.second << "\n";

                                std::cout << "Best line: ";
                                for (int i = 0; i < depth; i++) {
                                    Move move = result.first[i];
                                    std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
                                }
                                std::cout << "\n";
                            } else {
                                std::cout << "No moves available for White.\n";
                            }
                        }
                    }
                    averageTime = totalTime/20;
                    std::cout << "Average time for PVS in 20 iterations is: " << averageTime << " with " << threads << " threads" << "\n";
                    resultFile << "PVS," << threads << "," << averageTime << "\n";
                }
            }

        }

        // const char* mateIn2FENs[] = {
        //     "8/8/8/8/8/1k6/2nb4/1K6 b - - 0 1",
        //     "1r6/8/8/8/8/1k6/8/K7 b - - 0 1",
        //     "1q6/8/8/8/8/1k6/8/K7 b - - 0 1",
        //     "1q6/8/8/8/8/1k6/PPP5/K7 b - - 0 1",
        //     "8/8/8/8/1k6/n2q4/PP6/K6R b - - 0 1",
        //     "5k2/4p2Q/8/5P2/b5R1/8/3P4/3KR3 w - - 0 1",
        //     "5k2/3p4/5K1P/8/8/8/8/8 w - - 0 1"
        // };
        //
        // std::ofstream resultFile2("results-m2.txt");
        // if (!resultFile2.is_open()) {
        //     std::cerr << "Error: Unable to open results.txt for writing\n";
        //     return 0;
        // }
        // std::cout << "Testing mate in 2 FENs\n";
        // for (const char* fen : mateIn2FENs) {
        //     StockDory::Board chessBoard(fen);
        //     resultFile2 << "Current Fen: " << fen << "\n";
        //     std::cout << "Current Fen: " << fen << "\n" << std::endl;
        //     for (int depth = 3; depth < 5; depth++) {
        //         std::cout << "Testing depth: " << depth << "\n";
        //         if (chessBoard.ColorToMove() == White) {
        //             int result = engine.minimaxMoveCounter<White>(
        //                 chessBoard,
        //                 depth
        //             );
        //             printf("Total number of moves in minimax: %d\n", result);
        //             resultFile2 << "Minimax moves: " << result << "\n";
        //         }
        //         if (chessBoard.ColorToMove() == White) {
        //             int count = 0;
        //             std::pair<std::array<Move, maxDepth>, int> result = engine.alphaBetaNegaMoveCounter<White, maxDepth>(
        //                 chessBoard,
        //                 -50000,
        //                 50000,
        //                 depth,
        //                 count
        //             );
        //             printf("Total number of moves in alphaBeta: %d\n", count);
        //             resultFile2 << "AlphaBeta moves: " << count << "\n";
        //         }
        //         if (chessBoard.ColorToMove() == Black) {
        //             int result = engine.minimaxMoveCounter<Black>(
        //                 chessBoard,
        //                 depth
        //             );
        //             printf("Total number of moves in minimax: %d\n", result);
        //             resultFile2 << "Minimax moves: " << result << "\n";
        //         }
        //         if (chessBoard.ColorToMove() == Black) {
        //             int count = 0;
        //             std::pair<std::array<Move, maxDepth>, int> result = engine.alphaBetaNegaMoveCounter<Black, maxDepth>(
        //                 chessBoard,
        //                 -50000,
        //                 50000,
        //                 depth,
        //                 count
        //             );
        //             printf("Total number of moves in alphaBeta: %d\n", count);
        //             resultFile2 << "AlphaBeta moves: " << count << "\n";
        //         }
        //         std::cout << "Algorithm: Sequential Minimax\n" << std::endl;
        //         double totalTime = 0;
        //         for (int i = 0; i < 20; i++) {
        //             if (chessBoard.ColorToMove() == White) {
        //                  tstart = omp_get_wtime();
        //                  std::pair<std::array<Move, maxDepth>, int> result = engine.minimax<White, maxDepth>(
        //                      chessBoard,
        //                      depth
        //                  );
        //                  tend = omp_get_wtime();
        //                  ttaken = tend - tstart;
        //                  totalTime += ttaken;
        //
        //                  printf("Time taken for main part minimax: %f\n", ttaken);
        //
        //                  if (!result.first.empty()) {
        //                      Move bestMove = result.first.front();
        //                      std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                                << " with score " << result.second << "\n";
        //
        //                      std::cout << "Best line: ";
        //                      for (int i = 0; i < depth; i++) {
        //                          Move move = result.first[i];
        //                          std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                      }
        //                      std::cout << "\n";
        //                  } else {
        //                      std::cout << "No moves available for White.\n";
        //                  }
        //              }
        //             else if (chessBoard.ColorToMove() == Black) {
        //                 tstart = omp_get_wtime();
        //                 std::pair<std::array<Move, maxDepth>, int> result = engine.minimax<Black, maxDepth>(
        //                     chessBoard,
        //                     depth
        //                 );
        //                 tend = omp_get_wtime();
        //                 ttaken = tend - tstart;
        //                 totalTime += ttaken;
        //
        //                 printf("Time taken for main part minimax: %f\n", ttaken);
        //
        //                 if (!result.first.empty()) {
        //                     Move bestMove = result.first.front();
        //                     std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                               << " with score " << result.second << "\n";
        //
        //                     std::cout << "Best line: ";
        //                     for (int i = 0; i < depth; i++) {
        //                         Move move = result.first[i];
        //                         std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                     }
        //                     std::cout << "\n";
        //                 } else {
        //                     std::cout << "No moves available for White.\n";
        //                 }
        //             }
        //         }
        //         double averageTime = totalTime/20;
        //         std::cout << "Average time for minimax in 20 iterations is: " << averageTime << "\n";
        //         resultFile2 << "Sequential Minimax,1," << averageTime << "\n";
        //
        //         int numThreads[] = {1, 2, 4, 8, 16, 32, 64};
        //
        //         for (int threads : numThreads) {
        //             omp_set_num_threads(threads);
        //             std::cout << "Algorithm: Parallel Minimax\n" << std::endl;
        //             std::cout << "Total number of threads: " << threads << "\n" << std::endl;
        //             totalTime = 0;
        //             for (int i = 0; i < 20; i++) {
        //                 if (chessBoard.ColorToMove() == White) {
        //                     tstart = omp_get_wtime();
        //                     std::pair<std::array<Move, maxDepth>, int> result = engine.parallelMinimax<White, maxDepth>(
        //                         chessBoard,
        //                         depth
        //                     );
        //                     tend = omp_get_wtime();
        //                     ttaken = tend - tstart;
        //                     totalTime += ttaken;
        //
        //                     printf("Time taken for main part parallel minimax: %f\n", ttaken);
        //
        //                     if (!result.first.empty()) {
        //                         Move bestMove = result.first.front();
        //                         std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                                   << " with score " << result.second << "\n";
        //
        //                         std::cout << "Best line: ";
        //                         for (int i = 0; i < depth; i++) {
        //                             Move move = result.first[i];
        //                             std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                         }
        //                         std::cout << "\n";
        //                     } else {
        //                         std::cout << "No moves available for White.\n";
        //                     }
        //                 }
        //                 else if (chessBoard.ColorToMove() == Black) {
        //                     tstart = omp_get_wtime();
        //                     std::pair<std::array<Move, maxDepth>, int> result = engine.parallelMinimax<Black, maxDepth>(
        //                         chessBoard,
        //                         depth
        //                     );
        //                     tend = omp_get_wtime();
        //                     ttaken = tend - tstart;
        //                     totalTime += ttaken;
        //
        //                     printf("Time taken for main part parallel minimax: %f\n", ttaken);
        //
        //                     if (!result.first.empty()) {
        //                         Move bestMove = result.first.front();
        //                         std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                                   << " with score " << result.second << "\n";
        //
        //                         std::cout << "Best line: ";
        //                         for (int i = 0; i < depth; i++) {
        //                             Move move = result.first[i];
        //                             std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                         }
        //                         std::cout << "\n";
        //                     } else {
        //                         std::cout << "No moves available for White.\n";
        //                     }
        //                 }
        //             }
        //             averageTime = totalTime/20;
        //             std::cout << "Average time for parallel minimax in 20 iterations is: " << averageTime << " with " << threads << " threads" << "\n";
        //             resultFile2 << "Parallel Minimax," << threads << "," << averageTime << "\n";
        //         }
        //
        //         std::cout << "Algorithm: Sequential AlphaBeta\n" << std::endl;
        //         totalTime = 0;
        //         for (int i = 0; i < 20; i++) {
        //             if (chessBoard.ColorToMove() == White) {
        //                 tstart = omp_get_wtime();
        //                 std::pair<std::array<Move, maxDepth>, int> result = engine.alphaBetaNega<White, maxDepth>(
        //                 chessBoard,
        //                 -50000,
        //                 50000,
        //                     depth
        //                 );
        //                 tend = omp_get_wtime();
        //                 ttaken = tend - tstart;
        //                 totalTime += ttaken;
        //
        //                 printf("Time taken for main part sequential alpha beta: %f\n", ttaken);
        //
        //                 if (!result.first.empty()) {
        //                     Move bestMove = result.first.front();
        //                     std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                               << " with score " << result.second << "\n";
        //
        //                     std::cout << "Best line: ";
        //                     for (int i = 0; i < depth; i++) {
        //                         Move move = result.first[i];
        //                         std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                     }
        //                     std::cout << "\n";
        //                 } else {
        //                     std::cout << "No moves available for White.\n";
        //                 }
        //             }
        //             else if (chessBoard.ColorToMove() == Black) {
        //                 tstart = omp_get_wtime();
        //                 std::pair<std::array<Move, maxDepth>, int> result = engine.alphaBetaNega<Black, maxDepth>(
        //                 chessBoard,
        //                 -50000,
        //                 50000,
        //                     depth
        //                 );
        //                 tend = omp_get_wtime();
        //                 ttaken = tend - tstart;
        //                 totalTime += ttaken;
        //
        //                 printf("Time taken for main part sequential alpha beta: %f\n", ttaken);
        //
        //                 if (!result.first.empty()) {
        //                     Move bestMove = result.first.front();
        //                     std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                               << " with score " << result.second << "\n";
        //
        //                     std::cout << "Best line: ";
        //                     for (int i = 0; i < depth; i++) {
        //                         Move move = result.first[i];
        //                         std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                     }
        //                     std::cout << "\n";
        //                 } else {
        //                     std::cout << "No moves available for White.\n";
        //                 }
        //             }
        //         }
        //         averageTime = totalTime/20;
        //         std::cout << "Average time for sequential AlphaBeta in 20 iterations is: " << averageTime << "\n";
        //         resultFile2 << "Sequential AlphaBeta,1," << averageTime << "\n";
        //
        //         for (int threads : numThreads) {
        //             omp_set_num_threads(threads);
        //             std::cout << "Algorithm: Naive Alpha Beta Parallel\n" << std::endl;
        //
        //             std::cout << "Total number of threads: " << threads << "\n" << std::endl;
        //             totalTime = 0;
        //             for (int i = 0; i < 20; i++) {
        //                 if (chessBoard.ColorToMove() == White) {
        //                     tstart = omp_get_wtime();
        //                     std::pair<std::array<Move, maxDepth>, int> result = engine.naiveParallelAlphaBeta<White, maxDepth>(
        //                         chessBoard,
        //                         -50000,
        //                         50000,
        //                         depth
        //                     );
        //                     tend = omp_get_wtime();
        //                     ttaken = tend - tstart;
        //                     totalTime += ttaken;
        //
        //                     printf("Time taken for main part naive alpha beta parallel: %f\n", ttaken);
        //
        //                     if (!result.first.empty()) {
        //                         Move bestMove = result.first.front();
        //                         std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                                   << " with score " << result.second << "\n";
        //
        //                         std::cout << "Best line: ";
        //                         for (int i = 0; i < depth; i++) {
        //                             Move move = result.first[i];
        //                             std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                         }
        //                         std::cout << "\n";
        //                     } else {
        //                         std::cout << "No moves available for White.\n";
        //                     }
        //                 }
        //                 else if (chessBoard.ColorToMove() == Black) {
        //                     tstart = omp_get_wtime();
        //                     std::pair<std::array<Move, maxDepth>, int> result = engine.naiveParallelAlphaBeta<Black, maxDepth>(
        //                         chessBoard,
        //                         -50000,
        //                         50000,
        //                         depth
        //                     );
        //                     tend = omp_get_wtime();
        //                     ttaken = tend - tstart;
        //                     totalTime += ttaken;
        //
        //                     printf("Time taken for main part naive alpha beta parallel: %f\n", ttaken);
        //
        //                     if (!result.first.empty()) {
        //                         Move bestMove = result.first.front();
        //                         std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                                   << " with score " << result.second << "\n";
        //
        //                         std::cout << "Best line: ";
        //                         for (int i = 0; i < depth; i++) {
        //                             Move move = result.first[i];
        //                             std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                         }
        //                         std::cout << "\n";
        //                     } else {
        //                         std::cout << "No moves available for White.\n";
        //                     }
        //                 }
        //             }
        //             averageTime = totalTime/20;
        //             std::cout << "Average time for naive alpha beta parallel in 20 iterations is: " << averageTime << " with " << threads << " threads" << "\n";
        //             resultFile2 << "Naive Parallel Alpha Beta," << threads << "," << averageTime << "\n";
        //         }
        //
        //         for (int threads : numThreads) {
        //             omp_set_num_threads(threads);
        //             std::cout << "Algorithm: Naive Alpha Beta Parallel\n" << std::endl;
        //
        //             std::cout << "Total number of threads: " << threads << "\n" << std::endl;
        //             totalTime = 0;
        //             for (int i = 0; i < 20; i++) {
        //                 if (chessBoard.ColorToMove() == White) {
        //                     tstart = omp_get_wtime();
        //                     std::pair<std::array<Move, maxDepth>, int> result = engine.naiveParallelPVAlphaBeta<White, maxDepth>(
        //                         chessBoard,
        //                         -50000,
        //                         50000,
        //                         depth
        //                     );
        //                     tend = omp_get_wtime();
        //                     ttaken = tend - tstart;
        //                     totalTime += ttaken;
        //
        //                     printf("Time taken for main part naive alpha beta parallel with PV combo: %f\n", ttaken);
        //
        //                     if (!result.first.empty()) {
        //                         Move bestMove = result.first.front();
        //                         std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                                   << " with score " << result.second << "\n";
        //
        //                         std::cout << "Best line: ";
        //                         for (int i = 0; i < depth; i++) {
        //                             Move move = result.first[i];
        //                             std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                         }
        //                         std::cout << "\n";
        //                     } else {
        //                         std::cout << "No moves available for White.\n";
        //                     }
        //                 }
        //                 else if (chessBoard.ColorToMove() == Black) {
        //                     tstart = omp_get_wtime();
        //                     std::pair<std::array<Move, maxDepth>, int> result = engine.naiveParallelPVAlphaBeta<Black, maxDepth>(
        //                         chessBoard,
        //                         -50000,
        //                         50000,
        //                         depth
        //                     );
        //                     tend = omp_get_wtime();
        //                     ttaken = tend - tstart;
        //                     totalTime += ttaken;
        //
        //                     printf("Time taken for main part naive alpha beta parallel with PV combo: %f\n", ttaken);
        //
        //                     if (!result.first.empty()) {
        //                         Move bestMove = result.first.front();
        //                         std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                                   << " with score " << result.second << "\n";
        //
        //                         std::cout << "Best line: ";
        //                         for (int i = 0; i < depth; i++) {
        //                             Move move = result.first[i];
        //                             std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                         }
        //                         std::cout << "\n";
        //                     } else {
        //                         std::cout << "No moves available for White.\n";
        //                     }
        //                 }
        //             }
        //             averageTime = totalTime/20;
        //             std::cout << "Average time for naive alpha beta parallel with YBWC combo in 20 iterations is: " << averageTime << " with " << threads << " threads" << "\n";
        //             resultFile2 << "Naive Parallel Alpha Beta with PV," << threads << "," << averageTime << "\n";
        //         }
        //
        //         for (int threads : numThreads) {
        //             omp_set_num_threads(threads);
        //             std::cout << "Algorithm: YBWC\n" << std::endl;
        //
        //             std::cout << "Total number of threads: " << threads << "\n" << std::endl;
        //             totalTime = 0;
        //             for (int i = 0; i < 20; i++) {
        //                 if (chessBoard.ColorToMove() == White) {
        //                     tstart = omp_get_wtime();
        //                     std::pair<std::array<Move, maxDepth>, int> result = engine.YBWC<White, maxDepth>(
        //                         chessBoard,
        //                         -50000,
        //                         50000,
        //                         depth
        //                     );
        //                     tend = omp_get_wtime();
        //                     ttaken = tend - tstart;
        //                     totalTime += ttaken;
        //
        //                     printf("Time taken for main part YBWC: %f\n", ttaken);
        //
        //                     if (!result.first.empty()) {
        //                         Move bestMove = result.first.front();
        //                         std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                                   << " with score " << result.second << "\n";
        //
        //                         std::cout << "Best line: ";
        //                         for (int i = 0; i < depth; i++) {
        //                             Move move = result.first[i];
        //                             std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                         }
        //                         std::cout << "\n";
        //                     } else {
        //                         std::cout << "No moves available for White.\n";
        //                     }
        //                 }
        //                 else if (chessBoard.ColorToMove() == Black) {
        //                     tstart = omp_get_wtime();
        //                     std::pair<std::array<Move, maxDepth>, int> result = engine.YBWC<Black, maxDepth>(
        //                         chessBoard,
        //                         -50000,
        //                         50000,
        //                         depth
        //                     );
        //                     tend = omp_get_wtime();
        //                     ttaken = tend - tstart;
        //                     totalTime += ttaken;
        //
        //                     printf("Time taken for main part YBWC: %f\n", ttaken);
        //
        //                     if (!result.first.empty()) {
        //                         Move bestMove = result.first.front();
        //                         std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                                   << " with score " << result.second << "\n";
        //
        //                         std::cout << "Best line: ";
        //                         for (int i = 0; i < depth; i++) {
        //                             Move move = result.first[i];
        //                             std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                         }
        //                         std::cout << "\n";
        //                     } else {
        //                         std::cout << "No moves available for White.\n";
        //                     }
        //                 }
        //             }
        //             averageTime = totalTime/20;
        //             std::cout << "Average time for YBWC in 20 iterations is: " << averageTime << " with " << threads << " threads" << "\n";
        //             resultFile2 << "YBWC," << threads << "," << averageTime << "\n";
        //         }
        //
        //         for (int threads : numThreads) {
        //             omp_set_num_threads(threads);
        //             std::cout << "Algorithm: PVS\n" << std::endl;
        //
        //             std::cout << "Total number of threads: " << threads << "\n" << std::endl;
        //             totalTime = 0;
        //             for (int i = 0; i < 20; i++) {
        //                 if (chessBoard.ColorToMove() == White) {
        //                     tstart = omp_get_wtime();
        //                     std::pair<std::array<Move, maxDepth>, int> result = engine.PVS<White, maxDepth>(
        //                         chessBoard,
        //                         -50000,
        //                         50000,
        //                         depth
        //                     );
        //                     tend = omp_get_wtime();
        //                     ttaken = tend - tstart;
        //                     totalTime += ttaken;
        //
        //                     printf("Time taken for main part PVS: %f\n", ttaken);
        //
        //                     if (!result.first.empty()) {
        //                         Move bestMove = result.first.front();
        //                         std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                                   << " with score " << result.second << "\n";
        //
        //                         std::cout << "Best line: ";
        //                         for (int i = 0; i < depth; i++) {
        //                             Move move = result.first[i];
        //                             std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                         }
        //                         std::cout << "\n";
        //                     } else {
        //                         std::cout << "No moves available for White.\n";
        //                     }
        //                 }
        //                 else if (chessBoard.ColorToMove() == Black) {
        //                     tstart = omp_get_wtime();
        //                     std::pair<std::array<Move, maxDepth>, int> result = engine.PVS<Black, maxDepth>(
        //                         chessBoard,
        //                         -50000,
        //                         50000,
        //                         depth
        //                     );
        //                     tend = omp_get_wtime();
        //                     ttaken = tend - tstart;
        //                     totalTime += ttaken;
        //
        //                     printf("Time taken for main part PVS: %f\n", ttaken);
        //
        //                     if (!result.first.empty()) {
        //                         Move bestMove = result.first.front();
        //                         std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                                   << " with score " << result.second << "\n";
        //
        //                         std::cout << "Best line: ";
        //                         for (int i = 0; i < depth; i++) {
        //                             Move move = result.first[i];
        //                             std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                         }
        //                         std::cout << "\n";
        //                     } else {
        //                         std::cout << "No moves available for White.\n";
        //                     }
        //                 }
        //             }
        //             averageTime = totalTime/20;
        //             std::cout << "Average time for PVS in 20 iterations is: " << averageTime << " with " << threads << " threads" << "\n";
        //             resultFile2 << "PVS," << threads << "," << averageTime << "\n";
        //         }
        //     }
        //
        // }
        //
        // const char* mateIn1FENs[] = {
        //     "3qk3/3pp1P1/8/8/8/8/8/4K3 w - - 0 1",
        //     "3qk3/3pp1Q1/8/8/8/8/8/4K3 w - - 0 1",
        //     "6rk/6pp/7N/8/8/8/8/4K3 w - - 0 1",
        //     "7k/8/8/8/8/7r/r7/4K3 b - - 0 1",
        //     "8/8/8/8/8/2n3k1/r4b2/5K2 b - - 0 1",
        //     "2q5/8/8/8/8/6k1/r7/4K3 b - - 0 1",
        //     "8/8/8/5q2/8/6k1/r7/4K3 b - - 0 1",
        //     "8/8/8/8/8/6k1/r2PP3/4K3 b - - 0 1",
        //     "8/8/8/8/8/nk6/3b4/K7 b - - 0 1"
        // };
        //
        // std::ofstream resultFile1("results-m1.txt");
        // if (!resultFile1.is_open()) {
        //     std::cerr << "Error: Unable to open results.txt for writing\n";
        //     return 0;
        // }
        // std::cout << "Testing mate in 1 FENs\n";
        // for (const char* fen : mateIn1FENs) {
        //     StockDory::Board chessBoard(fen);
        //     resultFile1 << "Current Fen: " << fen << "\n";
        //     std::cout << "Current Fen: " << fen << "\n" << std::endl;
        //     for (int depth = 1; depth < 3; depth++) {
        //         std::cout << "Testing depth: " << depth << "\n";
        //         if (chessBoard.ColorToMove() == White) {
        //             int result = engine.minimaxMoveCounter<White>(
        //                 chessBoard,
        //                 depth
        //             );
        //             printf("Total number of moves in minimax: %d\n", result);
        //             resultFile1 << "Minimax moves: " << result << "\n";
        //         }
        //         if (chessBoard.ColorToMove() == White) {
        //             int count = 0;
        //             std::pair<std::array<Move, maxDepth>, int> result = engine.alphaBetaNegaMoveCounter<White, maxDepth>(
        //                 chessBoard,
        //                 -50000,
        //                 50000,
        //                 depth,
        //                 count
        //             );
        //             printf("Total number of moves in alphaBeta: %d\n", count);
        //             resultFile1 << "AlphaBeta moves: " << count << "\n";
        //         }
        //         if (chessBoard.ColorToMove() == Black) {
        //             int result = engine.minimaxMoveCounter<Black>(
        //                 chessBoard,
        //                 depth
        //             );
        //             printf("Total number of moves in minimax: %d\n", result);
        //             resultFile1 << "Minimax moves: " << result << "\n";
        //         }
        //         if (chessBoard.ColorToMove() == Black) {
        //             int count = 0;
        //             std::pair<std::array<Move, maxDepth>, int> result = engine.alphaBetaNegaMoveCounter<Black, maxDepth>(
        //                 chessBoard,
        //                 -50000,
        //                 50000,
        //                 depth,
        //                 count
        //             );
        //             printf("Total number of moves in alphaBeta: %d\n", count);
        //             resultFile1 << "AlphaBeta moves: " << count << "\n";
        //         }
        //         std::cout << "Algorithm: Sequential Minimax\n" << std::endl;
        //         double totalTime = 0;
        //         for (int i = 0; i < 20; i++) {
        //             if (chessBoard.ColorToMove() == White) {
        //                  tstart = omp_get_wtime();
        //                  std::pair<std::array<Move, maxDepth>, int> result = engine.minimax<White, maxDepth>(
        //                      chessBoard,
        //                      depth
        //                  );
        //                  tend = omp_get_wtime();
        //                  ttaken = tend - tstart;
        //                  totalTime += ttaken;
        //
        //                  printf("Time taken for main part minimax: %f\n", ttaken);
        //
        //                  if (!result.first.empty()) {
        //                      Move bestMove = result.first.front();
        //                      std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                                << " with score " << result.second << "\n";
        //
        //                      std::cout << "Best line: ";
        //                      for (int i = 0; i < depth; i++) {
        //                          Move move = result.first[i];
        //                          std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                      }
        //                      std::cout << "\n";
        //                  } else {
        //                      std::cout << "No moves available for White.\n";
        //                  }
        //              }
        //             else if (chessBoard.ColorToMove() == Black) {
        //                 tstart = omp_get_wtime();
        //                 std::pair<std::array<Move, maxDepth>, int> result = engine.minimax<Black, maxDepth>(
        //                     chessBoard,
        //                     depth
        //                 );
        //                 tend = omp_get_wtime();
        //                 ttaken = tend - tstart;
        //                 totalTime += ttaken;
        //
        //                 printf("Time taken for main part minimax: %f\n", ttaken);
        //
        //                 if (!result.first.empty()) {
        //                     Move bestMove = result.first.front();
        //                     std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                               << " with score " << result.second << "\n";
        //
        //                     std::cout << "Best line: ";
        //                     for (int i = 0; i < depth; i++) {
        //                         Move move = result.first[i];
        //                         std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                     }
        //                     std::cout << "\n";
        //                 } else {
        //                     std::cout << "No moves available for White.\n";
        //                 }
        //             }
        //         }
        //         double averageTime = totalTime/20;
        //         std::cout << "Average time for minimax in 20 iterations is: " << averageTime << "\n";
        //         resultFile1 << "Sequential Minimax,1," << averageTime << "\n";
        //
        //         int numThreads[] = {1, 2, 4, 8, 16, 32, 64};
        //
        //         for (int threads : numThreads) {
        //             omp_set_num_threads(threads);
        //             std::cout << "Algorithm: Parallel Minimax\n" << std::endl;
        //             std::cout << "Total number of threads: " << threads << "\n" << std::endl;
        //             totalTime = 0;
        //             for (int i = 0; i < 20; i++) {
        //                 if (chessBoard.ColorToMove() == White) {
        //                     tstart = omp_get_wtime();
        //                     std::pair<std::array<Move, maxDepth>, int> result = engine.parallelMinimax<White, maxDepth>(
        //                         chessBoard,
        //                         depth
        //                     );
        //                     tend = omp_get_wtime();
        //                     ttaken = tend - tstart;
        //                     totalTime += ttaken;
        //
        //                     printf("Time taken for main part parallel minimax: %f\n", ttaken);
        //
        //                     if (!result.first.empty()) {
        //                         Move bestMove = result.first.front();
        //                         std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                                   << " with score " << result.second << "\n";
        //
        //                         std::cout << "Best line: ";
        //                         for (int i = 0; i < depth; i++) {
        //                             Move move = result.first[i];
        //                             std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                         }
        //                         std::cout << "\n";
        //                     } else {
        //                         std::cout << "No moves available for White.\n";
        //                     }
        //                 }
        //                 else if (chessBoard.ColorToMove() == Black) {
        //                     tstart = omp_get_wtime();
        //                     std::pair<std::array<Move, maxDepth>, int> result = engine.parallelMinimax<Black, maxDepth>(
        //                         chessBoard,
        //                         depth
        //                     );
        //                     tend = omp_get_wtime();
        //                     ttaken = tend - tstart;
        //                     totalTime += ttaken;
        //
        //                     printf("Time taken for main part parallel minimax: %f\n", ttaken);
        //
        //                     if (!result.first.empty()) {
        //                         Move bestMove = result.first.front();
        //                         std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                                   << " with score " << result.second << "\n";
        //
        //                         std::cout << "Best line: ";
        //                         for (int i = 0; i < depth; i++) {
        //                             Move move = result.first[i];
        //                             std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                         }
        //                         std::cout << "\n";
        //                     } else {
        //                         std::cout << "No moves available for White.\n";
        //                     }
        //                 }
        //             }
        //             averageTime = totalTime/20;
        //             std::cout << "Average time for parallel minimax in 20 iterations is: " << averageTime << " with " << threads << " threads" << "\n";
        //             resultFile1 << "Parallel Minimax," << threads << "," << averageTime << "\n";
        //         }
        //
        //         std::cout << "Algorithm: Sequential AlphaBeta\n" << std::endl;
        //         totalTime = 0;
        //         for (int i = 0; i < 20; i++) {
        //             if (chessBoard.ColorToMove() == White) {
        //                 tstart = omp_get_wtime();
        //                 std::pair<std::array<Move, maxDepth>, int> result = engine.alphaBetaNega<White, maxDepth>(
        //                 chessBoard,
        //                 -50000,
        //                 50000,
        //                     depth
        //                 );
        //                 tend = omp_get_wtime();
        //                 ttaken = tend - tstart;
        //                 totalTime += ttaken;
        //
        //                 printf("Time taken for main part sequential alpha beta: %f\n", ttaken);
        //
        //                 if (!result.first.empty()) {
        //                     Move bestMove = result.first.front();
        //                     std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                               << " with score " << result.second << "\n";
        //
        //                     std::cout << "Best line: ";
        //                     for (int i = 0; i < depth; i++) {
        //                         Move move = result.first[i];
        //                         std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                     }
        //                     std::cout << "\n";
        //                 } else {
        //                     std::cout << "No moves available for White.\n";
        //                 }
        //             }
        //             else if (chessBoard.ColorToMove() == Black) {
        //                 tstart = omp_get_wtime();
        //                 std::pair<std::array<Move, maxDepth>, int> result = engine.alphaBetaNega<Black, maxDepth>(
        //                 chessBoard,
        //                 -50000,
        //                 50000,
        //                     depth
        //                 );
        //                 tend = omp_get_wtime();
        //                 ttaken = tend - tstart;
        //                 totalTime += ttaken;
        //
        //                 printf("Time taken for main part sequential alpha beta: %f\n", ttaken);
        //
        //                 if (!result.first.empty()) {
        //                     Move bestMove = result.first.front();
        //                     std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                               << " with score " << result.second << "\n";
        //
        //                     std::cout << "Best line: ";
        //                     for (int i = 0; i < depth; i++) {
        //                         Move move = result.first[i];
        //                         std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                     }
        //                     std::cout << "\n";
        //                 } else {
        //                     std::cout << "No moves available for White.\n";
        //                 }
        //             }
        //         }
        //         averageTime = totalTime/20;
        //         std::cout << "Average time for sequential AlphaBeta in 20 iterations is: " << averageTime << "\n";
        //         resultFile1 << "Sequential AlphaBeta,1," << averageTime << "\n";
        //
        //         for (int threads : numThreads) {
        //             omp_set_num_threads(threads);
        //             std::cout << "Algorithm: Naive Alpha Beta Parallel\n" << std::endl;
        //
        //             std::cout << "Total number of threads: " << threads << "\n" << std::endl;
        //             totalTime = 0;
        //             for (int i = 0; i < 20; i++) {
        //                 if (chessBoard.ColorToMove() == White) {
        //                     tstart = omp_get_wtime();
        //                     std::pair<std::array<Move, maxDepth>, int> result = engine.naiveParallelAlphaBeta<White, maxDepth>(
        //                         chessBoard,
        //                         -50000,
        //                         50000,
        //                         depth
        //                     );
        //                     tend = omp_get_wtime();
        //                     ttaken = tend - tstart;
        //                     totalTime += ttaken;
        //
        //                     printf("Time taken for main part naive alpha beta parallel: %f\n", ttaken);
        //
        //                     if (!result.first.empty()) {
        //                         Move bestMove = result.first.front();
        //                         std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                                   << " with score " << result.second << "\n";
        //
        //                         std::cout << "Best line: ";
        //                         for (int i = 0; i < depth; i++) {
        //                             Move move = result.first[i];
        //                             std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                         }
        //                         std::cout << "\n";
        //                     } else {
        //                         std::cout << "No moves available for White.\n";
        //                     }
        //                 }
        //                 else if (chessBoard.ColorToMove() == Black) {
        //                     tstart = omp_get_wtime();
        //                     std::pair<std::array<Move, maxDepth>, int> result = engine.naiveParallelAlphaBeta<Black, maxDepth>(
        //                         chessBoard,
        //                         -50000,
        //                         50000,
        //                         depth
        //                     );
        //                     tend = omp_get_wtime();
        //                     ttaken = tend - tstart;
        //                     totalTime += ttaken;
        //
        //                     printf("Time taken for main part naive alpha beta parallel: %f\n", ttaken);
        //
        //                     if (!result.first.empty()) {
        //                         Move bestMove = result.first.front();
        //                         std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                                   << " with score " << result.second << "\n";
        //
        //                         std::cout << "Best line: ";
        //                         for (int i = 0; i < depth; i++) {
        //                             Move move = result.first[i];
        //                             std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                         }
        //                         std::cout << "\n";
        //                     } else {
        //                         std::cout << "No moves available for White.\n";
        //                     }
        //                 }
        //             }
        //             averageTime = totalTime/20;
        //             std::cout << "Average time for naive alpha beta parallel in 20 iterations is: " << averageTime << " with " << threads << " threads" << "\n";
        //             resultFile1 << "Naive Parallel Alpha Beta," << threads << "," << averageTime << "\n";
        //         }
        //
        //         for (int threads : numThreads) {
        //             omp_set_num_threads(threads);
        //             std::cout << "Algorithm: Naive Alpha Beta Parallel\n" << std::endl;
        //
        //             std::cout << "Total number of threads: " << threads << "\n" << std::endl;
        //             totalTime = 0;
        //             for (int i = 0; i < 20; i++) {
        //                 if (chessBoard.ColorToMove() == White) {
        //                     tstart = omp_get_wtime();
        //                     std::pair<std::array<Move, maxDepth>, int> result = engine.naiveParallelPVAlphaBeta<White, maxDepth>(
        //                         chessBoard,
        //                         -50000,
        //                         50000,
        //                         depth
        //                     );
        //                     tend = omp_get_wtime();
        //                     ttaken = tend - tstart;
        //                     totalTime += ttaken;
        //
        //                     printf("Time taken for main part naive alpha beta parallel with PV combo: %f\n", ttaken);
        //
        //                     if (!result.first.empty()) {
        //                         Move bestMove = result.first.front();
        //                         std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                                   << " with score " << result.second << "\n";
        //
        //                         std::cout << "Best line: ";
        //                         for (int i = 0; i < depth; i++) {
        //                             Move move = result.first[i];
        //                             std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                         }
        //                         std::cout << "\n";
        //                     } else {
        //                         std::cout << "No moves available for White.\n";
        //                     }
        //                 }
        //                 else if (chessBoard.ColorToMove() == Black) {
        //                     tstart = omp_get_wtime();
        //                     std::pair<std::array<Move, maxDepth>, int> result = engine.naiveParallelPVAlphaBeta<Black, maxDepth>(
        //                         chessBoard,
        //                         -50000,
        //                         50000,
        //                         depth
        //                     );
        //                     tend = omp_get_wtime();
        //                     ttaken = tend - tstart;
        //                     totalTime += ttaken;
        //
        //                     printf("Time taken for main part naive alpha beta parallel with PV combo: %f\n", ttaken);
        //
        //                     if (!result.first.empty()) {
        //                         Move bestMove = result.first.front();
        //                         std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                                   << " with score " << result.second << "\n";
        //
        //                         std::cout << "Best line: ";
        //                         for (int i = 0; i < depth; i++) {
        //                             Move move = result.first[i];
        //                             std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                         }
        //                         std::cout << "\n";
        //                     } else {
        //                         std::cout << "No moves available for White.\n";
        //                     }
        //                 }
        //             }
        //             averageTime = totalTime/20;
        //             std::cout << "Average time for naive alpha beta parallel with YBWC combo in 20 iterations is: " << averageTime << " with " << threads << " threads" << "\n";
        //             resultFile1 << "Naive Parallel Alpha Beta with PV," << threads << "," << averageTime << "\n";
        //         }
        //
        //         for (int threads : numThreads) {
        //             omp_set_num_threads(threads);
        //             std::cout << "Algorithm: YBWC\n" << std::endl;
        //
        //             std::cout << "Total number of threads: " << threads << "\n" << std::endl;
        //             totalTime = 0;
        //             for (int i = 0; i < 20; i++) {
        //                 if (chessBoard.ColorToMove() == White) {
        //                     tstart = omp_get_wtime();
        //                     std::pair<std::array<Move, maxDepth>, int> result = engine.YBWC<White, maxDepth>(
        //                         chessBoard,
        //                         -50000,
        //                         50000,
        //                         depth
        //                     );
        //                     tend = omp_get_wtime();
        //                     ttaken = tend - tstart;
        //                     totalTime += ttaken;
        //
        //                     printf("Time taken for main part YBWC: %f\n", ttaken);
        //
        //                     if (!result.first.empty()) {
        //                         Move bestMove = result.first.front();
        //                         std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                                   << " with score " << result.second << "\n";
        //
        //                         std::cout << "Best line: ";
        //                         for (int i = 0; i < depth; i++) {
        //                             Move move = result.first[i];
        //                             std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                         }
        //                         std::cout << "\n";
        //                     } else {
        //                         std::cout << "No moves available for White.\n";
        //                     }
        //                 }
        //                 else if (chessBoard.ColorToMove() == Black) {
        //                     tstart = omp_get_wtime();
        //                     std::pair<std::array<Move, maxDepth>, int> result = engine.YBWC<Black, maxDepth>(
        //                         chessBoard,
        //                         -50000,
        //                         50000,
        //                         depth
        //                     );
        //                     tend = omp_get_wtime();
        //                     ttaken = tend - tstart;
        //                     totalTime += ttaken;
        //
        //                     printf("Time taken for main part YBWC: %f\n", ttaken);
        //
        //                     if (!result.first.empty()) {
        //                         Move bestMove = result.first.front();
        //                         std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                                   << " with score " << result.second << "\n";
        //
        //                         std::cout << "Best line: ";
        //                         for (int i = 0; i < depth; i++) {
        //                             Move move = result.first[i];
        //                             std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                         }
        //                         std::cout << "\n";
        //                     } else {
        //                         std::cout << "No moves available for White.\n";
        //                     }
        //                 }
        //             }
        //             averageTime = totalTime/20;
        //             std::cout << "Average time for YBWC in 20 iterations is: " << averageTime << " with " << threads << " threads" << "\n";
        //             resultFile1 << "YBWC," << threads << "," << averageTime << "\n";
        //         }
        //
        //         for (int threads : numThreads) {
        //             omp_set_num_threads(threads);
        //             std::cout << "Algorithm: PVS\n" << std::endl;
        //
        //             std::cout << "Total number of threads: " << threads << "\n" << std::endl;
        //             totalTime = 0;
        //             for (int i = 0; i < 20; i++) {
        //                 if (chessBoard.ColorToMove() == White) {
        //                     tstart = omp_get_wtime();
        //                     std::pair<std::array<Move, maxDepth>, int> result = engine.PVS<White, maxDepth>(
        //                         chessBoard,
        //                         -50000,
        //                         50000,
        //                         depth
        //                     );
        //                     tend = omp_get_wtime();
        //                     ttaken = tend - tstart;
        //                     totalTime += ttaken;
        //
        //                     printf("Time taken for main part PVS: %f\n", ttaken);
        //
        //                     if (!result.first.empty()) {
        //                         Move bestMove = result.first.front();
        //                         std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                                   << " with score " << result.second << "\n";
        //
        //                         std::cout << "Best line: ";
        //                         for (int i = 0; i < depth; i++) {
        //                             Move move = result.first[i];
        //                             std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                         }
        //                         std::cout << "\n";
        //                     } else {
        //                         std::cout << "No moves available for White.\n";
        //                     }
        //                 }
        //                 else if (chessBoard.ColorToMove() == Black) {
        //                     tstart = omp_get_wtime();
        //                     std::pair<std::array<Move, maxDepth>, int> result = engine.PVS<Black, maxDepth>(
        //                         chessBoard,
        //                         -50000,
        //                         50000,
        //                         depth
        //                     );
        //                     tend = omp_get_wtime();
        //                     ttaken = tend - tstart;
        //                     totalTime += ttaken;
        //
        //                     printf("Time taken for main part PVS: %f\n", ttaken);
        //
        //                     if (!result.first.empty()) {
        //                         Move bestMove = result.first.front();
        //                         std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
        //                                   << " with score " << result.second << "\n";
        //
        //                         std::cout << "Best line: ";
        //                         for (int i = 0; i < depth; i++) {
        //                             Move move = result.first[i];
        //                             std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
        //                         }
        //                         std::cout << "\n";
        //                     } else {
        //                         std::cout << "No moves available for White.\n";
        //                     }
        //                 }
        //             }
        //             averageTime = totalTime/20;
        //             std::cout << "Average time for PVS in 20 iterations is: " << averageTime << " with " << threads << " threads" << "\n";
        //             resultFile1 << "PVS," << threads << "," << averageTime << "\n";
        //         }
        //     }
        //
        // }

    }

    return 0;
}

// int main() {
//     StockDory::Board chessBoard;
//     Engine engine;
//     int depth = 6;
//     double tstart = 0.0, tend=0.0, ttaken;
//
//     // Set up OpenMP
//     omp_set_nested(1);
//     omp_set_num_threads(8);
//     omp_set_max_active_levels(2);
//
// const char* mateIn3FENs[] = {
//             "7k/8/3NK3/5BN1/8/8/8/8 w - - 0 1",
//             "k7/3K4/3N4/2N5/8/3B4/8/8 w - - 0 1",
//             "8/8/2K5/7r/6r1/8/6k1/8 b - - 0 1",
//             "8/K7/7r/8/2k5/5bb1/8/8 b - - 0 1",
//             "8/K7/P6r/8/2k5/5bb1/8/8 b - - 0 1",
//             "8/8/8/8/k7/4Q3/3K4/8 w - - 0 1",
//             "8/8/k7/2K5/8/2Q5/b1R5/n7 w - - 0 1",
//             "8/8/k1K1b3/2n5/8/8/8/2R5 w - - 0 1",
//             "8/7P/k1K1b3/2n5/8/8/8/2R5 w - - 0 1",
//             "7k/7n/8/8/8/7B/7R/6RK w - - 0 1"
//         };
//         std::cout << "Testing mate in 3 FENs\n";
//         for (const char* fen : mateIn3FENs) {
//             StockDory::Board chessBoard(fen);
//             std::cout << "Current Fen: " << fen << "\n" << std::endl;
//             for (int depth = 5; depth < 7; depth++) {
//                 std::cout << "Testing depth: " << depth << "\n";
//                 if (chessBoard.ColorToMove() == White) {
//                     int result = engine.minimaxMoveCounter<White>(
//                         chessBoard,
//                         depth
//                     );
//                     printf("Total number of moves in minimax: %d\n", result);
//                 }
//                 if (chessBoard.ColorToMove() == White) {
//                     int count = 0;
//                     std::pair<std::array<Move, maxDepth>, int> result = engine.alphaBetaNegaMoveCounter<White, maxDepth>(
//                         chessBoard,
//                         -50000,
//                         50000,
//                         depth,
//                         count
//                     );
//                     printf("Total number of moves in alphaBeta: %d\n", count);
//                 }
//                 if (chessBoard.ColorToMove() == Black) {
//                     int result = engine.minimaxMoveCounter<Black>(
//                         chessBoard,
//                         depth
//                     );
//                     printf("Total number of moves in minimax: %d\n", result);
//                 }
//                 if (chessBoard.ColorToMove() == Black) {
//                     int count = 0;
//                     std::pair<std::array<Move, maxDepth>, int> result = engine.alphaBetaNegaMoveCounter<Black, maxDepth>(
//                         chessBoard,
//                         -50000,
//                         50000,
//                         depth,
//                         count
//                     );
//                     printf("Total number of moves in alphaBeta: %d\n", count);
//                 }
//                 std::cout << "Algorithm: Sequential Minimax\n" << std::endl;
//                 double totalTime = 0;
//                 for (int i = 0; i < 20; i++) {
//                     if (chessBoard.ColorToMove() == White) {
//                          tstart = omp_get_wtime();
//                          std::pair<std::array<Move, maxDepth>, int> result = engine.minimax<White, maxDepth>(
//                              chessBoard,
//                              depth
//                          );
//                          tend = omp_get_wtime();
//                          ttaken = tend - tstart;
//                          totalTime += ttaken;
//
//                          printf("Time taken for main part minimax: %f\n", ttaken);
//
//                          if (!result.first.empty()) {
//                              Move bestMove = result.first.front();
//                              std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
//                                        << " with score " << result.second << "\n";
//
//                              std::cout << "Best line: ";
//                              for (int i = 0; i < depth; i++) {
//                                  Move move = result.first[i];
//                                  std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
//                              }
//                              std::cout << "\n";
//                          } else {
//                              std::cout << "No moves available for White.\n";
//                          }
//                      }
//                     else if (chessBoard.ColorToMove() == Black) {
//                         tstart = omp_get_wtime();
//                         std::pair<std::array<Move, maxDepth>, int> result = engine.minimax<Black, maxDepth>(
//                             chessBoard,
//                             depth
//                         );
//                         tend = omp_get_wtime();
//                         ttaken = tend - tstart;
//                         totalTime += ttaken;
//
//                         printf("Time taken for main part minimax: %f\n", ttaken);
//
//                         if (!result.first.empty()) {
//                             Move bestMove = result.first.front();
//                             std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
//                                       << " with score " << result.second << "\n";
//
//                             std::cout << "Best line: ";
//                             for (int i = 0; i < depth; i++) {
//                                 Move move = result.first[i];
//                                 std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
//                             }
//                             std::cout << "\n";
//                         } else {
//                             std::cout << "No moves available for White.\n";
//                         }
//                     }
//                 }
//                 double averageTime = totalTime/20;
//                 std::cout << "Average time for minimax in 20 iterations is: " << averageTime << "\n";
//
//                 int numThreads[] = {1, 2, 4, 8, 16, 32, 64};
//
//                 for (int threads : numThreads) {
//                     omp_set_num_threads(threads);
//                     std::cout << "Algorithm: Parallel Minimax\n" << std::endl;
//                     std::cout << "Total number of threads: " << numThreads << "\n" << std::endl;
//                     totalTime = 0;
//                     for (int i = 0; i < 20; i++) {
//                         if (chessBoard.ColorToMove() == White) {
//                             tstart = omp_get_wtime();
//                             std::pair<std::array<Move, maxDepth>, int> result = engine.parallelMinimax<White, maxDepth>(
//                                 chessBoard,
//                                 depth
//                             );
//                             tend = omp_get_wtime();
//                             ttaken = tend - tstart;
//                             totalTime += ttaken;
//
//                             printf("Time taken for main part parallel minimax: %f\n", ttaken);
//
//                             if (!result.first.empty()) {
//                                 Move bestMove = result.first.front();
//                                 std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
//                                           << " with score " << result.second << "\n";
//
//                                 std::cout << "Best line: ";
//                                 for (int i = 0; i < depth; i++) {
//                                     Move move = result.first[i];
//                                     std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
//                                 }
//                                 std::cout << "\n";
//                             } else {
//                                 std::cout << "No moves available for White.\n";
//                             }
//                         }
//                         else if (chessBoard.ColorToMove() == Black) {
//                             tstart = omp_get_wtime();
//                             std::pair<std::array<Move, maxDepth>, int> result = engine.parallelMinimax<Black, maxDepth>(
//                                 chessBoard,
//                                 depth
//                             );
//                             tend = omp_get_wtime();
//                             ttaken = tend - tstart;
//                             totalTime += ttaken;
//
//                             printf("Time taken for main part parallel minimax: %f\n", ttaken);
//
//                             if (!result.first.empty()) {
//                                 Move bestMove = result.first.front();
//                                 std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
//                                           << " with score " << result.second << "\n";
//
//                                 std::cout << "Best line: ";
//                                 for (int i = 0; i < depth; i++) {
//                                     Move move = result.first[i];
//                                     std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
//                                 }
//                                 std::cout << "\n";
//                             } else {
//                                 std::cout << "No moves available for White.\n";
//                             }
//                         }
//                     }
//                     averageTime = totalTime/20;
//                     std::cout << "Average time for parallel minimax in 20 iterations is: " << averageTime << " with " << threads << " threads" << "\n";
//                 }
//
//                 std::cout << "Algorithm: Sequential AlphaBeta\n" << std::endl;
//                 totalTime = 0;
//                 for (int i = 0; i < 20; i++) {
//                     if (chessBoard.ColorToMove() == White) {
//                         tstart = omp_get_wtime();
//                         std::pair<std::array<Move, maxDepth>, int> result = engine.alphaBetaNega<White, maxDepth>(
//                         chessBoard,
//                         -50000,
//                         50000,
//                             depth
//                         );
//                         tend = omp_get_wtime();
//                         ttaken = tend - tstart;
//                         totalTime += ttaken;
//
//                         printf("Time taken for main part sequential alpha beta: %f\n", ttaken);
//
//                         if (!result.first.empty()) {
//                             Move bestMove = result.first.front();
//                             std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
//                                       << " with score " << result.second << "\n";
//
//                             std::cout << "Best line: ";
//                             for (int i = 0; i < depth; i++) {
//                                 Move move = result.first[i];
//                                 std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
//                             }
//                             std::cout << "\n";
//                         } else {
//                             std::cout << "No moves available for White.\n";
//                         }
//                     }
//                     else if (chessBoard.ColorToMove() == Black) {
//                         tstart = omp_get_wtime();
//                         std::pair<std::array<Move, maxDepth>, int> result = engine.alphaBetaNega<Black, maxDepth>(
//                         chessBoard,
//                         -50000,
//                         50000,
//                             depth
//                         );
//                         tend = omp_get_wtime();
//                         ttaken = tend - tstart;
//                         totalTime += ttaken;
//
//                         printf("Time taken for main part sequential alpha beta: %f\n", ttaken);
//
//                         if (!result.first.empty()) {
//                             Move bestMove = result.first.front();
//                             std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
//                                       << " with score " << result.second << "\n";
//
//                             std::cout << "Best line: ";
//                             for (int i = 0; i < depth; i++) {
//                                 Move move = result.first[i];
//                                 std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
//                             }
//                             std::cout << "\n";
//                         } else {
//                             std::cout << "No moves available for White.\n";
//                         }
//                     }
//                 }
//                 averageTime = totalTime/20;
//                 std::cout << "Average time for sequential AlphaBeta in 20 iterations is: " << averageTime << " with " << numThreads << "\n";
//
//                 for (int threads : numThreads) {
//                     omp_set_num_threads(threads);
//                     std::cout << "Algorithm: Naive Alpha Beta Parallel\n" << std::endl;
//
//                     std::cout << "Total number of threads: " << threads << "\n" << std::endl;
//                     totalTime = 0;
//                     for (int i = 0; i < 20; i++) {
//                         if (chessBoard.ColorToMove() == White) {
//                             tstart = omp_get_wtime();
//                             std::pair<std::array<Move, maxDepth>, int> result = engine.naiveParallel<White, maxDepth>(
//                                 chessBoard,
//                                 -50000,
//                                 50000,
//                                 depth
//                             );
//                             tend = omp_get_wtime();
//                             ttaken = tend - tstart;
//                             totalTime += ttaken;
//
//                             printf("Time taken for main part naive alpha beta parallel: %f\n", ttaken);
//
//                             if (!result.first.empty()) {
//                                 Move bestMove = result.first.front();
//                                 std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
//                                           << " with score " << result.second << "\n";
//
//                                 std::cout << "Best line: ";
//                                 for (int i = 0; i < depth; i++) {
//                                     Move move = result.first[i];
//                                     std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
//                                 }
//                                 std::cout << "\n";
//                             } else {
//                                 std::cout << "No moves available for White.\n";
//                             }
//                         }
//                         else if (chessBoard.ColorToMove() == Black) {
//                             tstart = omp_get_wtime();
//                             std::pair<std::array<Move, maxDepth>, int> result = engine.naiveParallel<Black, maxDepth>(
//                                 chessBoard,
//                                 -50000,
//                                 50000,
//                                 depth
//                             );
//                             tend = omp_get_wtime();
//                             ttaken = tend - tstart;
//                             totalTime += ttaken;
//
//                             printf("Time taken for main part naive alpha beta parallel: %f\n", ttaken);
//
//                             if (!result.first.empty()) {
//                                 Move bestMove = result.first.front();
//                                 std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
//                                           << " with score " << result.second << "\n";
//
//                                 std::cout << "Best line: ";
//                                 for (int i = 0; i < depth; i++) {
//                                     Move move = result.first[i];
//                                     std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
//                                 }
//                                 std::cout << "\n";
//                             } else {
//                                 std::cout << "No moves available for White.\n";
//                             }
//                         }
//                     }
//                     averageTime = totalTime/20;
//                     std::cout << "Average time for naive alpha beta parallel in 20 iterations is: " << averageTime << " with " << threads << " threads" << "\n";
//                 }
//
//                 for (int threads : numThreads) {
//                     omp_set_num_threads(threads);
//                     std::cout << "Algorithm: YBWC\n" << std::endl;
//
//                     std::cout << "Total number of threads: " << threads << "\n" << std::endl;
//                     totalTime = 0;
//                     for (int i = 0; i < 20; i++) {
//                         if (chessBoard.ColorToMove() == White) {
//                             tstart = omp_get_wtime();
//                             std::pair<std::array<Move, maxDepth>, int> result = engine.YBWC<White, maxDepth>(
//                                 chessBoard,
//                                 -50000,
//                                 50000,
//                                 depth
//                             );
//                             tend = omp_get_wtime();
//                             ttaken = tend - tstart;
//                             totalTime += ttaken;
//
//                             printf("Time taken for main part YBWC: %f\n", ttaken);
//
//                             if (!result.first.empty()) {
//                                 Move bestMove = result.first.front();
//                                 std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
//                                           << " with score " << result.second << "\n";
//
//                                 std::cout << "Best line: ";
//                                 for (int i = 0; i < depth; i++) {
//                                     Move move = result.first[i];
//                                     std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
//                                 }
//                                 std::cout << "\n";
//                             } else {
//                                 std::cout << "No moves available for White.\n";
//                             }
//                         }
//                         else if (chessBoard.ColorToMove() == Black) {
//                             tstart = omp_get_wtime();
//                             std::pair<std::array<Move, maxDepth>, int> result = engine.YBWC<Black, maxDepth>(
//                                 chessBoard,
//                                 -50000,
//                                 50000,
//                                 depth
//                             );
//                             tend = omp_get_wtime();
//                             ttaken = tend - tstart;
//                             totalTime += ttaken;
//
//                             printf("Time taken for main part YBWC: %f\n", ttaken);
//
//                             if (!result.first.empty()) {
//                                 Move bestMove = result.first.front();
//                                 std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
//                                           << " with score " << result.second << "\n";
//
//                                 std::cout << "Best line: ";
//                                 for (int i = 0; i < depth; i++) {
//                                     Move move = result.first[i];
//                                     std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
//                                 }
//                                 std::cout << "\n";
//                             } else {
//                                 std::cout << "No moves available for White.\n";
//                             }
//                         }
//                     }
//                     averageTime = totalTime/20;
//                     std::cout << "Average time for YBWC in 20 iterations is: " << averageTime << " with " << threads << " threads" << "\n";
//                 }
//
//                 for (int threads : numThreads) {
//                     omp_set_num_threads(threads);
//                     std::cout << "Algorithm: PVS\n" << std::endl;
//
//                     std::cout << "Total number of threads: " << threads << "\n" << std::endl;
//                     totalTime = 0;
//                     for (int i = 0; i < 20; i++) {
//                         if (chessBoard.ColorToMove() == White) {
//                             tstart = omp_get_wtime();
//                             std::pair<std::array<Move, maxDepth>, int> result = engine.PVS<White, maxDepth>(
//                                 chessBoard,
//                                 -50000,
//                                 50000,
//                                 depth
//                             );
//                             tend = omp_get_wtime();
//                             ttaken = tend - tstart;
//                             totalTime += ttaken;
//
//                             printf("Time taken for main part PVS: %f\n", ttaken);
//
//                             if (!result.first.empty()) {
//                                 Move bestMove = result.first.front();
//                                 std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
//                                           << " with score " << result.second << "\n";
//
//                                 std::cout << "Best line: ";
//                                 for (int i = 0; i < depth; i++) {
//                                     Move move = result.first[i];
//                                     std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
//                                 }
//                                 std::cout << "\n";
//                             } else {
//                                 std::cout << "No moves available for White.\n";
//                             }
//                         }
//                         else if (chessBoard.ColorToMove() == Black) {
//                             tstart = omp_get_wtime();
//                             std::pair<std::array<Move, maxDepth>, int> result = engine.PVS<Black, maxDepth>(
//                                 chessBoard,
//                                 -50000,
//                                 50000,
//                                 depth
//                             );
//                             tend = omp_get_wtime();
//                             ttaken = tend - tstart;
//                             totalTime += ttaken;
//
//                             printf("Time taken for main part PVS: %f\n", ttaken);
//
//                             if (!result.first.empty()) {
//                                 Move bestMove = result.first.front();
//                                 std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
//                                           << " with score " << result.second << "\n";
//
//                                 std::cout << "Best line: ";
//                                 for (int i = 0; i < depth; i++) {
//                                     Move move = result.first[i];
//                                     std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
//                                 }
//                                 std::cout << "\n";
//                             } else {
//                                 std::cout << "No moves available for White.\n";
//                             }
//                         }
//                     }
//                     averageTime = totalTime/20;
//                     std::cout << "Average time for PVS in 20 iterations is: " << averageTime << " with " << threads << " threads" << "\n";
//                 }
//             }
//
//         }
// //
// //     const StockDory::SimplifiedMoveList<White> moveList(chessBoard);
// //
// //     double tstart = 0.0, tend = 0.0, ttaken;
// //     double totalTime = 0;
// //
// //     for (int i = 0; i < 1; i++) {
// //         if (chessBoard.ColorToMove() == White) {
// //             tstart = omp_get_wtime();
// //             int result = engine.minimaxMoveCounter<White>(
// //                 chessBoard,
// //                 depth
// //             );
// //             tend = omp_get_wtime();
// //             ttaken = tend - tstart;
// //             totalTime += ttaken;
// //
// //             printf("Total number of moves in minimax: %d\n", result);
// //         }
// //         if (chessBoard.ColorToMove() == White) {
// //             int count = 0;
// //             tstart = omp_get_wtime();
// //             std::pair<std::array<Move, maxDepth>, int> result = engine.alphaBetaNegaMoveCounter<White, maxDepth>(
// //                 chessBoard,
// //                 -50000,
// //                 50000,
// //                 depth,
// //                 count
// //             );
// //             tend = omp_get_wtime();
// //             ttaken = tend - tstart;
// //             totalTime += ttaken;
// //
// //             printf("Total number of moves in alphaBeta: %d\n", count);
// //         }
// //         if (chessBoard.ColorToMove() == White) {
// //             tstart = omp_get_wtime();
// //             std::pair<std::array<Move, maxDepth>, int> result = engine.minimax<White, maxDepth>(
// //                 chessBoard,
// //                 depth
// //             );
// //             tend = omp_get_wtime();
// //             ttaken = tend - tstart;
// //             totalTime += ttaken;
// //
// //             printf("Time taken for main part minimax: %f\n", ttaken);
// //
// //             if (!result.first.empty()) {
// //                 Move bestMove = result.first.front();
// //                 std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
// //                           << " with score " << result.second << "\n";
// //
// //                 std::cout << "Best line: ";
// //                 for (int i = 0; i < depth; i++) {
// //                     Move move = result.first[i];
// //                     std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
// //                 }
// //                 std::cout << "\n";
// //             } else {
// //                 std::cout << "No moves available for White.\n";
// //             }
// //         }
// //         if (chessBoard.ColorToMove() == White) {
// //             tstart = omp_get_wtime();
// //             std::pair<std::array<Move, maxDepth>, int> result = engine.parallelMinimax<White, maxDepth>(
// //                 chessBoard,
// //                 depth
// //             );
// //             tend = omp_get_wtime();
// //             ttaken = tend - tstart;
// //             totalTime += ttaken;
// //
// //             printf("Time taken for main part parallel minimax: %f\n", ttaken);
// //
// //             if (!result.first.empty()) {
// //                 Move bestMove = result.first.front();
// //                 std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
// //                           << " with score " << result.second << "\n";
// //
// //                 std::cout << "Best line: ";
// //                 for (int i = 0; i < depth; i++) {
// //                     Move move = result.first[i];
// //                     std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
// //                 }
// //                 std::cout << "\n";
// //             } else {
// //                 std::cout << "No moves available for White.\n";
// //             }
// //         }
// //         if (chessBoard.ColorToMove() == White) {
// //             tstart = omp_get_wtime();
// //             std::pair<std::array<Move, maxDepth>, int> result = engine.alphaBetaNega<White, maxDepth>(
// //                 chessBoard,
// //                 -50000,
// //                 50000,
// //                 depth
// //             );
// //             tend = omp_get_wtime();
// //             ttaken = tend - tstart;
// //             totalTime += ttaken;
// //
// //             printf("Time taken for main part: %f\n", ttaken);
// //
// //             if (!result.first.empty()) {
// //                 Move bestMove = result.first.front();
// //                 std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
// //                           << " with score " << result.second << "\n";
// //
// //                 std::cout << "Best line: ";
// //                 for (int i = 0; i < depth; i++) {
// //                     Move move = result.first[i];
// //                     std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
// //                 }
// //                 std::cout << "\n";
// //             } else {
// //                 std::cout << "No moves available for White.\n";
// //             }
// //         }
// //         if (chessBoard.ColorToMove() == White) {
// //             tstart = omp_get_wtime();
// //             std::pair<std::array<Move, maxDepth>, int> result = engine.naiveParallel<White, maxDepth>(
// //                 chessBoard,
// //                 -50000,
// //                 50000,
// //                 depth
// //             );
// //             tend = omp_get_wtime();
// //             ttaken = tend - tstart;
// //             totalTime += ttaken;
// //
// //             printf("Time taken for main part: %f\n", ttaken);
// //
// //             if (!result.first.empty()) {
// //                 Move bestMove = result.first.front();
// //                 std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
// //                           << " with score " << result.second << "\n";
// //
// //                 std::cout << "Best line: ";
// //                 for (int i = 0; i < depth; i++) {
// //                     Move move = result.first[i];
// //                     std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
// //                 }
// //                 std::cout << "\n";
// //             } else {
// //                 std::cout << "No moves available for White.\n";
// //             }
// //         }
// //         if (chessBoard.ColorToMove() == White) {
// //             tstart = omp_get_wtime();
// //             std::pair<std::array<Move, maxDepth>, int> result = engine.YBWC<White, maxDepth>(
// //                 chessBoard,
// //                 -50000,
// //                 50000,
// //                 depth
// //             );
// //             tend = omp_get_wtime();
// //             ttaken = tend - tstart;
// //             totalTime += ttaken;
// //
// //             printf("Time taken for main part: %f\n", ttaken);
// //
// //             if (!result.first.empty()) {
// //                 Move bestMove = result.first.front();
// //                 std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
// //                           << " with score " << result.second << "\n";
// //
// //                 std::cout << "Best line: ";
// //                 for (int i = 0; i < depth; i++) {
// //                     Move move = result.first[i];
// //                     std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
// //                 }
// //                 std::cout << "\n";
// //             } else {
// //                 std::cout << "No moves available for White.\n";
// //             }
// //         }
// //         if (chessBoard.ColorToMove() == White) {
// //             tstart = omp_get_wtime();
// //             std::pair<std::array<Move, maxDepth>, int> result = engine.PVS<White, maxDepth>(
// //                 chessBoard,
// //                 -50000,
// //                 50000,
// //                 depth
// //             );
// //             tend = omp_get_wtime();
// //             ttaken = tend - tstart;
// //             totalTime += ttaken;
// //
// //             printf("Time taken for main part: %f\n", ttaken);
// //
// //             if (!result.first.empty()) {
// //                 Move bestMove = result.first.front();
// //                 std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
// //                           << " with score " << result.second << "\n";
// //
// //                 std::cout << "Best line: ";
// //                 for (int i = 0; i < depth; i++) {
// //                     Move move = result.first[i];
// //                     std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
// //                 }
// //                 std::cout << "\n";
// //             } else {
// //                 std::cout << "No moves available for White.\n";
// //             }
// //         }
// //
// //     //     else if (chessBoard.ColorToMove() == Black) {
// //     //     std::pair<std::array<Move, maxDepth>, int> result3 = engine.YBWC<Black, maxDepth>(chessBoard, -50000, 50000, depth);
// //     //
// //     //     // Check if there is at least one move in the sequence
// //     //     if (!result3.first.empty()) {
// //     //         Move bestMove = result3.first.front();
// //     //         std::cout << "Black Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
// //     //                   << " with score " << result3.second << "\n";
// //     //
// //     //         // Print the entire sequence of moves
// //     //         std::cout << "Best line: ";
// //     //         for (const Move &move : result3.first) {
// //     //             std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
// //     //         }
// //     //         std::cout << "\n";
// //     //     } else {
// //     //         std::cout << "No moves available for Black.\n";
// //     //     }
// //     // }
// //     }
// //     double averageTime = totalTime/20;
// //     printf("The average time in 20 iterations is %f\n", averageTime);
// //
//     return 0;
// }

//Local test to see the differences between PVS and YBWC
// int main() {
//     const char* mateIn3FENs[] = {
//         "7k/8/3NK3/5BN1/8/8/8/8 w - - 0 1",
//         "k7/3K4/3N4/2N5/8/3B4/8/8 w - - 0 1",
//         "8/8/2K5/7r/6r1/8/6k1/8 b - - 0 1",
//         "8/8/k1K1b3/2n5/8/8/8/2R5 w - - 0 1",
//         "8/7P/k1K1b3/2n5/8/8/8/2R5 w - - 0 1",
//         "7k/7n/8/8/8/7B/7R/6RK w - - 0 1"
//     };
//     Engine engine;
//     std::cout << "Testing mate in 3 FENs\n";
//     int numThreads[] = {64};
//     for (const int threads: numThreads) {
//         omp_set_num_threads(threads);
//         printf("Threads: %d\n", threads);
//
//         int timesYBWCMoreMove = 0;
//         int timesPVSMoreMove = 0;
//         int timesYBWCMoreCrit = 0;
//         int timesPVSMoreCrit = 0;
//         int averageMoreMoves = 0;
//         int averageMoreCrit = 0;
//         int fasterYBWC = 0;
//         int fasterPVS = 0;
//         double averageYBWCTime = 0;
//         double averagePVSTime = 0;
//         for (const char* fen : mateIn3FENs) {
//
//             StockDory::Board chessBoard(fen);
//             std::atomic<int> moveCount(0);
//             std::atomic<int> critCount(0);
//             double tstart = omp_get_wtime();
//             auto result = engine.YBWCTest<White, 10>(chessBoard, -50000, 50000, 10, moveCount, critCount);
//             double tend = omp_get_wtime();
//             double ttaken = tend - tstart;
//             printf("Time taken for main part: %f\n", ttaken);
//
//             std::atomic<int> moveCount2(0);
//             std::atomic<int> critCount2(0);
//             double tstart1 = omp_get_wtime();
//             auto result2 = engine.PVSTest<White, 10>(chessBoard, -50000, 50000, 10, moveCount2, critCount2);
//             double tend1 = omp_get_wtime();
//             double ttaken1 = tend1 - tstart1;
//             printf("Time taken for main part: %f\n", ttaken1);
//             std::cout << "Total move YBWC count: " << moveCount.load() << std::endl;
//             std::cout << "Total move PVS count: " << moveCount2.load() << std::endl;
//             std::cout << "Total crit YBWC count: " << critCount.load() << std::endl;
//             std::cout << "Total crit PVS count: " << critCount2.load() << std::endl;
//             if (moveCount.load() < moveCount2.load()) {
//                 timesPVSMoreMove++;
//             }
//             else if (moveCount.load() > moveCount2.load()) {
//                 timesYBWCMoreMove++;
//             }
//             if (critCount.load() < critCount2.load()) {
//                 timesPVSMoreCrit++;
//             }
//             else if (critCount.load() > critCount2.load()) {
//                 timesYBWCMoreCrit++;
//             }
//
//             if (ttaken < ttaken1) {
//                 fasterYBWC++;
//             }
//             else if (ttaken > ttaken1) {
//                 fasterPVS++;
//             }
//             averageMoreMoves += moveCount2.load() - moveCount.load();
//             averageMoreCrit += critCount2.load() - critCount.load();
//             averagePVSTime += ttaken1;
//             averageYBWCTime += ttaken;
//         }
//         averageMoreMoves = averageMoreMoves/10;
//         averageMoreCrit = averageMoreCrit/10;
//         averagePVSTime = averagePVSTime/10;
//         averageYBWCTime = averageYBWCTime/10;
//         std::cout << "timesYBWCMoreMove: " << timesYBWCMoreMove << std::endl;
//         std::cout << "timesPVSMoreMove: " << timesPVSMoreMove << std::endl;
//         std::cout << "timesYBWCMoreCrit: " << timesYBWCMoreCrit << std::endl;
//         std::cout << "timesPVSMoreCrit: " << timesPVSMoreCrit << std::endl;
//         std::cout << "averageMoreMoves: " << averageMoreMoves << std::endl;
//         std::cout << "averageMoreCrit: " << averageMoreCrit << std::endl;
//         std::cout << "averageYBWCTime: " << averageYBWCTime << std::endl;
//         std::cout << "averagePVSTime: " << averagePVSTime << std::endl;
//         std::cout << "Faster YBWC: " << fasterYBWC << std::endl;
//         std::cout << "Faster PVS: " << fasterPVS << std::endl;
//     }
//
// }
