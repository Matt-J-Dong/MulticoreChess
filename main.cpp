// main.cpp
#include <iostream>
#include <limits>
#include <string>
#include <cstdlib> // For std::atoi
#include "Backend/Board.h"         // Include Board.h for chess board representation
#include "Backend/Type/Square.h"   // Include Square.h to use the Square enum
#include "SimplifiedMoveList.h"    // Include your SimplifiedMoveList class
#include "Backend/Type/Color.h"
#include "Engine.h"                // Include your Engine class

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
    std::cout << "1. Young Brothers Wait Concept (YBWC)\n";
    std::cout << "2. Principal Variation Search (PVS)\n";
    std::cout << "3. Parallel Alpha-Beta Nega\n";
    std::cout << "Enter your choice (1, 2, or 3): ";
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
            std::cerr << "Invalid input. Please enter 1, 2, or 3.\n";
            continue;
        }

        if (algorithmChoice == 1 || algorithmChoice == 2 || algorithmChoice == 3) {
            break; // Valid choice
        } else {
            std::cerr << "Invalid choice: " << algorithmChoice << ". Please enter 1, 2, or 3.\n";
        }
    }

    // Clear the newline character left in the input buffer
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    // Map the choice to algorithm name
    std::string algorithmName;
    switch (algorithmChoice) {
        case 1:
            algorithmName = "Young Brothers Wait Concept (YBWC)";
            break;
        case 2:
            algorithmName = "Principal Variation Search (PVS)";
            break;
        case 3:
            algorithmName = "Parallel Alpha-Beta Nega";
            break;
        default:
            // This case should never occur due to the earlier validation
            algorithmName = "Unknown Algorithm";
            break;
    }

    std::cout << "Starting " << algorithmName << " with depth: " << depth << "\n\n";

    // Initialize the chess board with the standard starting position
    StockDory::Board chessBoard;

    // Initialize the engine
    Engine engine;

    // Main game loop
    while (true) {
        // Display the current board state
        std::cout << "Current FEN: " << chessBoard.Fen() << "\n";

        // Determine which color is to move
        Color currentPlayer = chessBoard.ColorToMove();

        std::cout << "Current Player: " 
                  << (currentPlayer == White ? "White" : "Black") << "\n";

        // Define a pair to hold the result (best move sequence and its score)
        std::pair<std::array<Move, maxDepth>, float> result;

        // Execute the appropriate search algorithm based on the user's choice and current player
        if (algorithmChoice == 1) { // YBWC
            if (currentPlayer == White) {
                std::cout << "Performing YBWC for White...\n";
                // Perform YBWC for White
                result = engine.YBWC<White, maxDepth>(
                    chessBoard,
                    -std::numeric_limits<float>::infinity(),
                    std::numeric_limits<float>::infinity(),
                    depth
                );

                // Check if there is at least one move in the sequence
                // Since std::array doesn't have an empty() method, we assume the first move is valid
                Move bestMove = result.first[0];
                std::cout << "White's Best Move (YBWC): "
                          << squareToString(bestMove.From()) << " to "
                          << squareToString(bestMove.To())
                          << " with score " << result.second << "\n";

                // Print the entire sequence of moves (best line)
                std::cout << "Best Line: ";
                for (const Move &move : result.first) {
                    // Assuming that an invalid move has From() == To()
                    if (move.From() == move.To()) {
                        break; // Stop printing further moves
                    }
                    std::cout << squareToString(move.From()) << " to "
                              << squareToString(move.To()) << ", ";
                }
                std::cout << "\n";

                // Execute the move on the board using the existing Move<0> method
                try {
                    chessBoard.Move<0>(bestMove.From(), bestMove.To());
                    std::cout << "Move executed successfully.\n";
                }
                catch (const std::exception& e) {
                    std::cerr << "Error executing move: " << e.what() << "\n";
                    return 1;
                }
            }
            else if (currentPlayer == Black) {
                std::cout << "Performing YBWC for Black...\n";
                // Perform YBWC for Black
                result = engine.YBWC<Black, maxDepth>(
                    chessBoard,
                    -std::numeric_limits<float>::infinity(),
                    std::numeric_limits<float>::infinity(),
                    depth
                );

                // Check if there is at least one move in the sequence
                Move bestMove = result.first[0];
                std::cout << "Black's Best Move (YBWC): "
                          << squareToString(bestMove.From()) << " to "
                          << squareToString(bestMove.To())
                          << " with score " << result.second << "\n";

                // Print the entire sequence of moves (best line)
                std::cout << "Best Line: ";
                for (const Move &move : result.first) {
                    if (move.From() == move.To()) {
                        break;
                    }
                    std::cout << squareToString(move.From()) << " to "
                              << squareToString(move.To()) << ", ";
                }
                std::cout << "\n";

                // Execute the move on the board using the existing Move<0> method
                try {
                    chessBoard.Move<0>(bestMove.From(), bestMove.To());
                    std::cout << "Move executed successfully.\n";
                }
                catch (const std::exception& e) {
                    std::cerr << "Error executing move: " << e.what() << "\n";
                    return 1;
                }
            }
        }
        else if (algorithmChoice == 2) { // PVS
            if (currentPlayer == White) {
                std::cout << "Performing PVS for White...\n";
                // Perform PVS for White
                result = engine.PVS<White, maxDepth>(
                    chessBoard,
                    -std::numeric_limits<float>::infinity(),
                    std::numeric_limits<float>::infinity(),
                    depth
                );

                Move bestMove = result.first[0];
                std::cout << "White's Best Move (PVS): "
                          << squareToString(bestMove.From()) << " to "
                          << squareToString(bestMove.To())
                          << " with score " << result.second << "\n";

                // Print the entire sequence of moves (best line)
                std::cout << "Best Line: ";
                for (const Move &move : result.first) {
                    if (move.From() == move.To()) {
                        break;
                    }
                    std::cout << squareToString(move.From()) << " to "
                              << squareToString(move.To()) << ", ";
                }
                std::cout << "\n";

                // Execute the move on the board using the existing Move<0> method
                try {
                    chessBoard.Move<0>(bestMove.From(), bestMove.To());
                    std::cout << "Move executed successfully.\n";
                }
                catch (const std::exception& e) {
                    std::cerr << "Error executing move: " << e.what() << "\n";
                    return 1;
                }
            }
            else if (currentPlayer == Black) {
                std::cout << "Performing PVS for Black...\n";
                // Perform PVS for Black
                result = engine.PVS<Black, maxDepth>(
                    chessBoard,
                    -std::numeric_limits<float>::infinity(),
                    std::numeric_limits<float>::infinity(),
                    depth
                );

                Move bestMove = result.first[0];
                std::cout << "Black's Best Move (PVS): "
                          << squareToString(bestMove.From()) << " to "
                          << squareToString(bestMove.To())
                          << " with score " << result.second << "\n";

                // Print the entire sequence of moves (best line)
                std::cout << "Best Line: ";
                for (const Move &move : result.first) {
                    if (move.From() == move.To()) {
                        break;
                    }
                    std::cout << squareToString(move.From()) << " to "
                              << squareToString(move.To()) << ", ";
                }
                std::cout << "\n";

                // Execute the move on the board using the existing Move<0> method
                try {
                    chessBoard.Move<0>(bestMove.From(), bestMove.To());
                    std::cout << "Move executed successfully.\n";
                }
                catch (const std::exception& e) {
                    std::cerr << "Error executing move: " << e.what() << "\n";
                    return 1;
                }
            }
        }
        else if (algorithmChoice == 3) { // Parallel Alpha-Beta Nega
            if (currentPlayer == White) {
                std::cout << "Performing Parallel Alpha-Beta Nega for White...\n";
                // Perform Parallel Alpha-Beta Nega for White
                result = engine.parallelAlphaBetaNega<White, maxDepth>(
                    chessBoard,
                    -std::numeric_limits<float>::infinity(),
                    std::numeric_limits<float>::infinity(),
                    depth
                );

                Move bestMove = result.first[0];
                std::cout << "White's Best Move (Parallel Alpha-Beta Nega): "
                          << squareToString(bestMove.From()) << " to "
                          << squareToString(bestMove.To())
                          << " with score " << result.second << "\n";

                // Print the entire sequence of moves (best line)
                std::cout << "Best Line: ";
                for (const Move &move : result.first) {
                    if (move.From() == move.To()) {
                        break;
                    }
                    std::cout << squareToString(move.From()) << " to "
                              << squareToString(move.To()) << ", ";
                }
                std::cout << "\n";

                // Execute the move on the board using the existing Move<0> method
                try {
                    chessBoard.Move<0>(bestMove.From(), bestMove.To());
                    std::cout << "Move executed successfully.\n";
                }
                catch (const std::exception& e) {
                    std::cerr << "Error executing move: " << e.what() << "\n";
                    return 1;
                }
            }
            else if (currentPlayer == Black) {
                std::cout << "Performing Parallel Alpha-Beta Nega for Black...\n";
                // Perform Parallel Alpha-Beta Nega for Black
                result = engine.parallelAlphaBetaNega<Black, maxDepth>(
                    chessBoard,
                    -std::numeric_limits<float>::infinity(),
                    std::numeric_limits<float>::infinity(),
                    depth
                );

                Move bestMove = result.first[0];
                std::cout << "Black's Best Move (Parallel Alpha-Beta Nega): "
                          << squareToString(bestMove.From()) << " to "
                          << squareToString(bestMove.To())
                          << " with score " << result.second << "\n";

                // Print the entire sequence of moves (best line)
                std::cout << "Best Line: ";
                for (const Move &move : result.first) {
                    if (move.From() == move.To()) {
                        break;
                    }
                    std::cout << squareToString(move.From()) << " to "
                              << squareToString(move.To()) << ", ";
                }
                std::cout << "\n";

                // Execute the move on the board using the existing Move<0> method
                try {
                    chessBoard.Move<0>(bestMove.From(), bestMove.To());
                    std::cout << "Move executed successfully.\n";
                }
                catch (const std::exception& e) {
                    std::cerr << "Error executing move: " << e.what() << "\n";
                    return 1;
                }
            }
        }

        // Display the updated board state
        std::cout << "\nUpdated FEN: " << chessBoard.Fen() << "\n";

        // Prompt the user for a new FEN input
        while (true) {
            std::cout << "\nEnter a new FEN string to update the board (or type 'exit' to quit): ";
            std::string userFEN;
            std::getline(std::cin, userFEN);

            // Check if the user wants to exit
            if (userFEN == "exit") {
                std::cout << "Exiting the chess engine.\n";
                return 0;
            }

            // Attempt to set the new FEN
            try {
                chessBoard.SetFEN(userFEN);
                std::cout << "Board updated successfully.\n\n";
                break; // Exit the FEN input loop and continue with the main game loop
            }
            catch (const std::exception& e) {
                std::cerr << "Failed to set FEN: " << e.what() << "\n";
                std::cout << "Please try again.\n\n";
                // Continue to prompt the user for a valid FEN
            }
        }

        // Continue the main game loop
    }

    return 0;
}

// int main() {
//     StockDory::Board chessBoard;
//     Engine engine;
//     int depth = 1;
//     const StockDory::SimplifiedMoveList<White> moveList(chessBoard);
//
//     std::cout << "White's possible moves:\n";
//     for (uint8_t i = 0; i < moveList.Count(); i++) {
//         Move nextMove = moveList[i];
//         std::cout << "Move " << static_cast<int>(i + 1) << ": "
//                   << squareToString(nextMove.From()) << " to "
//                   << squareToString(nextMove.To()) << "\n";
//     }
//
//     if (chessBoard.ColorToMove() == White) {
//         std::pair<std::array<Move, maxDepth>, float> result3 = engine.PVS<White, maxDepth>(chessBoard, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), depth);
//
//         // Check if there is at least one move in the sequence
//         if (!result3.first.empty()) {
//             Move bestMove = result3.first.front();
//             std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
//                       << " with score " << result3.second << "\n";
//
//             // Print the entire sequence of moves
//             std::cout << "Best line: ";
//             for (int i = 0; i < depth; i++) {
//                 Move move = result3.first[i];
//                 std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
//             }
//             std::cout << "\n";
//         } else {
//             std::cout << "No moves available for White.\n";
//         }
//     }
//     else if (chessBoard.ColorToMove() == Black) {
//         std::pair<std::array<Move, maxDepth>, float> result3 = engine.YBWC<Black, maxDepth>(chessBoard, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), depth);
//
//         // Check if there is at least one move in the sequence
//         if (!result3.first.empty()) {
//             Move bestMove = result3.first.front();
//             std::cout << "Black Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
//                       << " with score " << result3.second << "\n";
//
//             // Print the entire sequence of moves
//             std::cout << "Best line: ";
//             for (const Move &move : result3.first) {
//                 std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
//             }
//             std::cout << "\n";
//         } else {
//             std::cout << "No moves available for Black.\n";
//         }
//     }
//     return 0;
// }


    /*
    // The following sections are commented out but can be enabled for alternative algorithms
    // Example: YBWC and PVS algorithms with varying depths
    // Uncomment and modify as needed
    */

    /*
    if (chessBoard.ColorToMove() == White) {
        std::pair<std::array<Move, maxDepth>, float> result3 = engine.YBWC<White, maxDepth>(
            chessBoard, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), 7);

        // Check if there is at least one move in the sequence
        if (!result3.first.empty()) {
            Move bestMove = result3.first.front();
            std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
                      << " with score " << result3.second << "\n";

            // Print the entire sequence of moves
            std::cout << "Best line: ";
            for (const Move &move : result3.first) {
                std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
            }
            std::cout << "\n";
        } else {
            std::cout << "No moves available for White.\n";
        }
    }
    else if (chessBoard.ColorToMove() == Black) {
        std::pair<std::array<Move, maxDepth>, float> result3 = engine.YBWC<Black, maxDepth>(
            chessBoard, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), 2);

        // Check if there is at least one move in the sequence
        if (!result3.first.empty()) {
            Move bestMove = result3.first.front();
            std::cout << "Black Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
                      << " with score " << result3.second << "\n";

            // Print the entire sequence of moves
            std::cout << "Best line: ";
            for (const Move &move : result3.first) {
                std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
            }
            std::cout << "\n";
        } else {
            std::cout << "No moves available for Black.\n";
        }
    }

    if (chessBoard.ColorToMove() == White) {
        std::pair<std::array<Move, maxDepth>, float> result3 = engine.PVS<White, maxDepth>(
            chessBoard, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), 7);

        // Check if there is at least one move in the sequence
        if (!result3.first.empty()) {
            Move bestMove = result3.first.front();
            std::cout << "White Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
                      << " with score " << result3.second << "\n";

            // Print the entire sequence of moves
            std::cout << "Best line: ";
            for (const Move &move : result3.first) {
                std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
            }
            std::cout << "\n";
        } else {
            std::cout << "No moves available for White.\n";
        }
    }
    else if (chessBoard.ColorToMove() == Black) {
        std::pair<std::array<Move, maxDepth>, float> result3 = engine.PVS<Black, maxDepth>(
            chessBoard, -std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), 2);

        // Check if there is at least one move in the sequence
        if (!result3.first.empty()) {
            Move bestMove = result3.first.front();
            std::cout << "Black Result is: " << squareToString(bestMove.From()) << " to " << squareToString(bestMove.To())
                      << " with score " << result3.second << "\n";

            // Print the entire sequence of moves
            std::cout << "Best line: ";
            for (const Move &move : result3.first) {
                std::cout << squareToString(move.From()) << " to " << squareToString(move.To()) << ", ";
            }
            std::cout << "\n";
        } else {
            std::cout << "No moves available for Black.\n";
        }
    }
    */

//     return 0;
// }
