#ifndef AI_HPP_INCLUDED
#define AI_HPP_INCLUDED

#include "Game.hpp"

typedef int Score;
constexpr Score SCORE_MAX = 1000; // SCORE_MIN is just -SCORE_MAX.  :)

// An evaluation function is expected to return a value from -1000 to 1000,
// where 1000 is a win for symbol and -1000 is a loss for symbol.
typedef Score Evaluator(const GameState& state, Symbol symbol);

Evaluator defaultEvaluator; // Follows the formula specified in the assignment.
Evaluator improvedEvaluator; // Evaluates by counting "candidate lines."  See readme for justification.

// Returned by calls to minimax().  We need a struct to hold all the metadata
// that it comes back with.
struct MinimaxResult
{
	// The worst-case score for the examined node, assuming both players play their best.
	Score score = 0;
	
	// Whether or not the depth limit prevented any nodes from being generated.
	bool cutOff = false;
	
	// The maximum depth reached.  The root node is at depth 0.
	unsigned int maximumDepth = 0;
	
	// The total number of nodes generated, not counting the root.
	unsigned int nodeCount = 0;
	
	// The number of subtrees pruned by the maximizer.
	unsigned int prunedCount = 0;
	
	// The number of subtrees pruned by the minimizer.
	unsigned int opponentPrunedCount = 0;
};

// Finds the value of the given node via the minimax algorithm with alpha-beta pruning.
// state is the root node, evaluate is a heuristic function, symbol is the player that
// the root node belongs to, maximumDepth is the number of tree layers below the root
// node that are allowed to be generated, and minimum and maximum correspond to alpha
// and beta.  The algorithm will not generate a subtree that it knows will fall outside
// [minimum, maximum].
MinimaxResult minimax(const GameState& state, Evaluator evaluate, Symbol symbol, unsigned int maximumDepth, Score minimum, Score maximum);

// Returns the best action for symbol to do, starting from state.
Action findBestAction(const GameState& state, Evaluator evaluate, Symbol symbol, unsigned int maximumDepth);

#endif
