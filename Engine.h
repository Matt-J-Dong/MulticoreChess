//
// Created by Capks on 10/22/2024.
//

#ifndef ENGINE_H
#define ENGINE_H
#include <limits>

#include "Backend/Board.h"
#include "Backend/Type/Move.h"
#include "Backend/Type/Color.h"
#include "Evaluation.h"
#include <utility>
// #include <omp.h>


#include "SimplifiedMoveList.h"

class Engine {
    private:
        Evaluation evaluation;
        int numThreads = 1;
        float mateScore = 20000;

    public:
        //minimax implementation
        std::pair<Move, float> minimax(StockDory::Board &chessBoard, int depth) {
            // Local variables
            Move bestMove;
            float bestScore;

            // Debugging: Print current depth and player
            //std::cout << "Minimax called at depth " << depth << " for player "
            //        << (chessBoard.ColorToMove() == White ? "White" : "Black") << std::endl;

            // Base case
            if (depth == 0) {
                float score = evaluation.eval(chessBoard);
                // Debugging: Print evaluation score
                //std::cout << "Depth 0 reached. Evaluation score: " << score << std::endl;
                return std::make_pair(Move(), score);
            }
            // White's turn
            else if (chessBoard.ColorToMove() == White) {
                bestScore = -std::numeric_limits<float>::infinity();
                const StockDory::SimplifiedMoveList<White> moveList(chessBoard);

                // Add check for no legal moves
                if (moveList.Count() == 0) {
                    // No legal moves
                    float score = evaluation.eval(chessBoard);
                    std::cout << "No legal moves for White at depth " << depth << ". Evaluation score: " << score << std::endl;
                    return std::make_pair(Move(), score);
                }

                for (uint8_t i = 0; i < moveList.Count(); i++) {
                    Move nextMove = moveList[i];
                    Square from = nextMove.From();
                    Square to = nextMove.To();
                    // Debugging: Print move being considered
                    //std::cout << "White considering move: " << nextMove.ToString() << std::endl;

                    // Perform move
                    PreviousState prevState = chessBoard.Move<0>(from, to);
                    std::pair<Move, float> result = minimax(chessBoard, depth-1);
                    // Update best score
                    if (bestScore < result.second) {
                        bestScore = result.second;
                        bestMove = nextMove;
                    }
                    // Undo move
                    chessBoard.UndoMove<0>(prevState, from, to);
                }
            }
            // Black's turn
            else {
                bestScore = std::numeric_limits<float>::infinity();
                const StockDory::SimplifiedMoveList<Black> moveList(chessBoard);

                // Add check for no legal moves
                if (moveList.Count() == 0) {
                    // No legal moves
                    float score = evaluation.eval(chessBoard);
                    std::cout << "No legal moves for Black at depth " << depth << ". Evaluation score: " << score << std::endl;
                    return std::make_pair(Move(), score);
                }

                for (uint8_t i = 0; i < moveList.Count(); i++) {
                    Move nextMove = moveList[i];
                    Square from = nextMove.From();
                    Square to = nextMove.To();
                    // Debugging: Print move being considered
                    //std::cout << "Black considering move: " << nextMove.ToString() << std::endl;

                    // Perform move
                    PreviousState prevState = chessBoard.Move<0>(from, to);
                    std::pair<Move, float> result = minimax(chessBoard, depth-1);
                    // Update best score
                    if (bestScore > result.second) {
                        bestScore = result.second;
                        bestMove = nextMove;
                    }
                    // Undo move
                    chessBoard.UndoMove<0>(prevState, from, to);
                }
            }

            // Debugging: Print best move and score at current depth
            //std::cout << "Best move at depth " << depth << ": " << bestMove.ToString()
            //        << " with score " << bestScore << std::endl;

            return std::make_pair(bestMove, bestScore);
        }

        std::pair<Move, float> alphaBeta(StockDory::Board &chessBoard, float alpha, float beta, int depth) {
            //local variable of best move and best score
            Move bestMove;
            float bestScore;

            //base-case -> when depth is 0, we evaluate the position score and return a default move (which will be overrided in the parent call)
            if (depth == 0) {
                float score = evaluation.eval(chessBoard);
                return std::make_pair(Move(), score);
            }
            //Minimax on white turn -> try to score as high as possible
            else if (chessBoard.ColorToMove() == White) {
                //set best score to negative infinity at start
                bestScore = -std::numeric_limits<float>::infinity();
                //create move list for white
                const StockDory::SimplifiedMoveList<White> moveList(chessBoard);
                //iterate through the moves and calculate the best score that can be reached from the next position
                for (uint8_t i = 0; i < moveList.Count(); i++) {
                    Move nextMove = moveList[i];
                    Square from = nextMove.From();
                    Square to = nextMove.To();
                    //Perform move
                    PreviousState prevState = chessBoard.Move<0>(from, to);
                    std::pair<Move, float> result = alphaBeta(chessBoard, alpha, beta, depth-1);
                    //update if we found a better move for white
                    if (bestScore < result.second) {
                        bestScore = result.second;
                        bestMove = nextMove;
                    }
                    //Undo move
                    chessBoard.UndoMove<0>(prevState, from, to);
                    //alpha check
                    alpha = std::max(alpha, result.second);
                    if (beta <= alpha) {
                        break;
                    }

                }
            }
            //Minimax on black turn -> try to score as low as possible
            else {
                //set best score to positive infinity
                bestScore = std::numeric_limits<float>::infinity();
                //calculate possible moves for black
                const StockDory::SimplifiedMoveList<Black> moveList(chessBoard);
                //repeat the same thing as white but for black instead
                for (uint8_t i = 0; i < moveList.Count(); i++) {
                    Move nextMove = moveList[i];
                    Square from = nextMove.From();
                    Square to = nextMove.To();
                    Piece promotion = nextMove.Promotion();
                    //Perform move
                    PreviousState prevState = chessBoard.Move<0>(from, to, promotion);
                    std::pair<Move, float> result = alphaBeta(chessBoard, alpha, beta, depth-1);
                    if (bestScore > result.second) {
                        bestScore = result.second;
                        bestMove = nextMove;
                    }
                    //Undo move
                    chessBoard.UndoMove<0>(prevState, from, to);
                    //beta check
                    beta = std::min(beta, result.second);
                    if (beta <= alpha) {
                        break;
                    }
                }
            }
            return std::make_pair(bestMove, bestScore);
        }

        template<Color color, int maxDepth>
        std::pair<std::array<Move, maxDepth>, float> alphaBetaNega(StockDory::Board &chessBoard, float alpha, float beta, int depth) {
            //local variable of best line and best score
            float bestScore;
            std::array<Move, maxDepth> bestLine;
            int bestLineSize;
            //create move list for player
            const StockDory::SimplifiedMoveList<color> moveList(chessBoard);
            //check for mate
            if (moveList.Count() == 0 and chessBoard.Checked<color>()) {
                return std::make_pair(std::array<Move, maxDepth>(), -mateScore-depth);
            }
            //stalemate
            else if (moveList.Count() == 0){
                return std::make_pair(std::array<Move, maxDepth>(), 0);
            }
            //base-case -> when depth is 0, we evaluate the position score and return a default move (which will be overrided in the parent call)
            if (depth == 0) {
                float score = evaluation.eval(chessBoard);
                //flip the score for black since we are maximizing
                if (color == Black) {
                    score *= -1;
                }
                return std::make_pair(std::array<Move, maxDepth>(), score);
            }
            constexpr enum Color Ocolor = Opposite(color);
            //Assume from one perspective they are always the maximizer
            //Set best score to negative infinity at start
            bestScore = -std::numeric_limits<float>::infinity();
            //iterate through the moves and calculate the best score that can be reached from the next position
            for (uint8_t i = 0; i < moveList.Count(); i++) {
                Move nextMove = moveList[i];
                Square from = nextMove.From();
                Square to = nextMove.To();
                Piece promotion = nextMove.Promotion();
                //Perform move
                PreviousState prevState = chessBoard.Move<0>(from, to, promotion);
                std::pair<std::array<Move, maxDepth>, float> result = alphaBetaNega<Ocolor, maxDepth>(chessBoard, -beta, -alpha, depth-1);
                //update if we found a better move for white
                result.second = -result.second;
                if (bestScore < result.second) {
                    //create chess line with the current move as the head
                    bestLine[0] = nextMove;
                    //append result line to the current move
                    bestLineSize = 1;
                    for (int j = 0; j < depth - 1; j++) {
                        bestLine[bestLineSize++] = result.first[j];
                    }
                    bestScore = result.second;
                }
                //Undo move
                chessBoard.UndoMove<0>(prevState, from, to);
                //alpha check
                alpha = std::max(alpha, result.second);
                if (beta <= alpha) {
                    break;
                }
            }

            return std::make_pair(bestLine, bestScore);
        }

        template<Color color, int maxDepth>
        std::pair<std::array<Move, maxDepth>, float> YBWC(StockDory::Board chessBoard, float alpha, float beta, int depth) {
            std::array<Move, maxDepth> bestLine;
            float bestScore;
            int bestLineSize;
            const StockDory::SimplifiedMoveList<color> moveList(chessBoard);
            //checkmate check
            if (moveList.Count() == 0 and chessBoard.Checked<color>()) {
                return std::make_pair(std::array<Move, maxDepth>(), -mateScore-depth);
            }
            else if (moveList.Count() == 0){
                return std::make_pair(std::array<Move, maxDepth>(), 0);
            }
            if (depth == 0) {
                float score = evaluation.eval(chessBoard);
                if (color == Black) {
                    score *= -1;
                }
                return std::make_pair(std::array<Move, maxDepth>(), score);
            }
            constexpr enum Color Ocolor = Opposite(color);
            bestScore = -std::numeric_limits<float>::infinity();
            //check the leftmost child first (PV)
            Move PV = moveList[0];
            Square from = PV.From();
            Square to = PV.To();
            PreviousState prevState = chessBoard.Move<0>(from, to);
            std::pair<std::array<Move, maxDepth>, float> result = YBWC<Ocolor, maxDepth>(chessBoard, -beta, -alpha, depth-1);
            result.second = -result.second;
            if (bestScore < result.second) {
                bestScore = result.second;
                bestLine[0] = PV;
                //append result line to the current move
                bestLineSize = 1;
                for (int j = 0; j < depth - 1; j++) {
                    bestLine[bestLineSize++] = result.first[j];
                }
            }
            chessBoard.UndoMove<0>(prevState, from, to);
            alpha = std::max(alpha, result.second);
            if (beta <= alpha) {
                return std::make_pair(bestLine, bestScore);
            }
            //run parallel threads to check the rest of the children
#pragma omp parallel for num_threads(32) default(none) private(chessBoard, nextMove, from, to, prevState, result) shared(Ocolor, bestScore, bestLine, alpha, beta, bestLineSize)
            for (uint8_t i = 1; i < moveList.Count(); i++) {
                Move nextMove = moveList[i];
                Square from = nextMove.From();
                Square to = nextMove.To();
                Piece promotion = nextMove.Promotion();
                PreviousState prevState = chessBoard.Move<0>(from, to, promotion);
                //Call YBWC here. We let left most child run sequentially always for YBWC.
                std::pair<std::array<Move, maxDepth>, float> result = YBWC<Ocolor, maxDepth>(chessBoard, -beta, -alpha, depth-1);
                result.second = -result.second;
                chessBoard.UndoMove<0>(prevState, from, to);
#pragma omp critical
                {
                if (bestScore < result.second) {
                    bestScore = result.second;
                    bestLine[0] = nextMove;
                    //append result line to the current move
                    bestLineSize = 1;
                    for (int j = 0; j < depth - 1; j++) {
                        bestLine[bestLineSize++] = result.first[j];
                    }
                }
                if (result.second > alpha) {
                    alpha = result.second;
                    if (beta <= alpha) {
                        break;
                    }
                }
                }
            }
             return std::make_pair(bestLine, bestScore);
        }

        template <Color color, int maxDepth>
        std::pair<std::array<Move, maxDepth>, float> PVS(StockDory::Board chessBoard, float alpha, float beta, int depth) {
            std::array<Move, maxDepth> bestLine;
            float bestScore;
            int bestLineSize;
            //create move list for player
            const StockDory::SimplifiedMoveList<color> moveList(chessBoard);
            //check for mate
            if (moveList.Count() == 0 and chessBoard.Checked<color>()) {
                return std::make_pair(std::array<Move, maxDepth>(), -mateScore-depth);
            }
            //stalemate
            else if (moveList.Count() == 0){
                return std::make_pair(std::array<Move, maxDepth>(), 0);
            }
            if (depth == 0) {
                float score = evaluation.eval(chessBoard);
                if (color == Black) {
                    score *= -1;
                }
                return std::make_pair(std::array<Move, maxDepth>(), score);
            }

            constexpr enum Color Ocolor = Opposite(color);
            bestScore = -std::numeric_limits<float>::infinity();

            //Check principal value (left child or best line) like YBWC
            Move PV = moveList[0];
            Square from = PV.From();
            Square to = PV.To();
            PreviousState prevState = chessBoard.Move<0>(from, to);
            std::pair<std::array<Move, maxDepth>, float> result = PVS<Ocolor, maxDepth>(chessBoard, -beta, -alpha, depth-1);
            result.second = -result.second;
            if (bestScore < result.second) {
                bestScore = result.second;
                bestLine[0] = PV;
                //append result line to the current move
                bestLineSize = 1;
                for (int j = 0; j < depth - 1; j++) {
                    bestLine[bestLineSize++] = result.first[j];
                }
            }
            chessBoard.UndoMove<0>(prevState, from, to);
            alpha = std::max(alpha, result.second);
            if (beta <= alpha) {
                return std::make_pair(bestLine, bestScore);
            }
            //run the rest of the checks in parallel
#pragma omp parallel for num_threads(32) default(none) private(chessBoard, nextMove, from, to, prevState, result) shared(Ocolor, bestScore, bestLine, bestLineSize, alpha, beta)
            for (uint8_t i = 1; i < moveList.Count(); i++) {
                Move nextMove = moveList[i];
                Square from = nextMove.From();
                Square to = nextMove.To();
                Piece promotion = nextMove.Promotion();
                PreviousState prevState = chessBoard.Move<0>(from, to, promotion);
                //call parallelAlphaBetaNega rather than PVS, we don't evaluate left most child for PVS other than PV line
                std::pair<std::array<Move, maxDepth>, float> result = parallelAlphaBetaNega<Ocolor, maxDepth>(chessBoard, -beta, -alpha, depth-1);
                result.second = -result.second;
                chessBoard.UndoMove<0>(prevState, from, to);
#pragma omp critical
                {
                if (bestScore < result.second) {
                    bestScore = result.second;
                    bestLine[0] = nextMove;
                    //append result line to the current move
                    bestLineSize = 1;
                    for (int j = 0; j < depth - 1; j++) {
                        bestLine[bestLineSize++] = result.first[j];
                    }
                }
                if (result.second > alpha) {
                    alpha = result.second;
                    if (beta <= alpha) {
                        break;
                    }
                }
                }
            }
             return std::make_pair(bestLine, bestScore);
        }
        //basic parallelAlphaBetaNega used for PVS
        template <Color color, int maxDepth>
        std::pair<std::array<Move, maxDepth>, float> parallelAlphaBetaNega(StockDory::Board chessBoard, float alpha, float beta, int depth) {
            std::array<Move, maxDepth> bestLine;
            float bestScore;
            int bestLineSize;
            //create move list for player
            const StockDory::SimplifiedMoveList<color> moveList(chessBoard);
            //check for mate
            if (moveList.Count() == 0 and chessBoard.Checked<color>()) {
                return std::make_pair(std::array<Move, maxDepth>(), -mateScore-depth);
            }
            //stalemate
            else if (moveList.Count() == 0){
                return std::make_pair(std::array<Move, maxDepth>(), 0);
            }
            if (depth == 0) {
                float score = evaluation.eval(chessBoard);
                if (color == Black) {
                    score *= -1;
                }
                return std::make_pair(std::array<Move, maxDepth>(), score);
            }

            constexpr enum Color Ocolor = Opposite(color);
            bestScore = -std::numeric_limits<float>::infinity();

#pragma omp parallel for num_threads(32) default(none) private(chessBoard, nextMove, from, to, prevState, result) shared(Ocolor, bestScore, bestLine, alpha, beta)
            for (uint8_t i = 0; i < moveList.Count(); i++) {
                Move nextMove = moveList[i];
                Square from = nextMove.From();
                Square to = nextMove.To();
                Piece promotion = nextMove.Promotion();
                PreviousState prevState = chessBoard.Move<0>(from, to, promotion);
                std::pair<std::array<Move, maxDepth>, float> result = parallelAlphaBetaNega<Ocolor, maxDepth>(chessBoard, -beta, -alpha, depth-1);
                result.second = -result.second;
                chessBoard.UndoMove<0>(prevState, from, to);
#pragma omp critical
                {
                if (bestScore < result.second) {
                    bestScore = result.second;
                    bestLine[0] = nextMove;
                    //append result line to the current move
                    bestLineSize = 1;
                    for (int j = 0; j < depth - 1; j++) {
                        bestLine[bestLineSize++] = result.first[j];
                    }
                }
                if (result.second > alpha) {
                    alpha = result.second;
                    if (beta <= alpha) {
                        break;
                    }
                }
                }
            }
             return std::make_pair(bestLine, bestScore);
        }
};

#endif //ENGINE_H
