#include "AI.hpp"
#include <algorithm>
#include <stdexcept>
#include <limits>

Score defaultEvaluator(const GameState& gameState, const Symbol symbol)
{
	Score score = 0;
	
	const auto lines = gameState.lines();
	for (const auto& line: lines)
	{
		const auto count = std::count(line.begin(), line.end(), symbol);
		const auto opponentCount = std::count(line.begin(), line.end(), opponentOf(symbol));
		
		// Win/lose check.
		if (count == 4) return SCORE_MAX;
		else if (opponentCount == 4) return -SCORE_MAX;
		
		else // score = 6X_3 + 3X_2 + X_1 - (6O_3 + 3O_2 + O_1)
		{
			if (count == 3) score += 6;
			else if (count == 2) score += 3;
			else if (count == 1) score += 1;
			
			if (opponentCount == 3) score -= 6;
			else if (opponentCount == 2) score -= 3;
			else if (opponentCount == 1) score -= 1;
		}
	}
	
	return score;
}

Score improvedEvaluator(const GameState& gameState, const Symbol symbol)
{
	Score score = 0;
	const auto lines = gameState.lines();
	for (const auto& line: lines)
	{
		const auto count = std::count(line.begin(), line.end(), symbol);
		const auto opponentCount = std::count(line.begin(), line.end(), opponentOf(symbol));
		
		if (count == 4) return SCORE_MAX;
		else if (opponentCount == 4) return -SCORE_MAX;
		else
		{
			if (opponentCount == 0) score++;
			else if (count == 0) score--;
		}
	}
	return score;
}

MinimaxResult minimax(const GameState& state,
                      Evaluator evaluate,
                      const Symbol symbol,
                      const unsigned int maximumDepth,
                      const Score minimum,
                      const Score maximum)
{
	MinimaxResult result;
	
	const auto ourActions = state.possibleActionsFor(symbol);
	
	if (maximumDepth == 0 || ourActions.empty()) // This is either a leaf node or we've reached the cutoff point.
	{
		result.score = evaluate(state, symbol);
		result.cutOff = !ourActions.empty(); // It doesn't count as a cutoff if there are no child nodes, anyway.
	}
	
	else
	{
		result.score = -10000;
		
		for (const auto& ourAction: ourActions)
		{
			const GameState ourResult = state.apply(ourAction);
			result.nodeCount++;
			
			MinimaxResult opponentResult = minimax(ourResult, evaluate, opponentOf(symbol), maximumDepth-1, -maximum, -result.score);
			
			result.score = std::max(result.score, -opponentResult.score);
			
			result.cutOff |= opponentResult.cutOff;
			result.maximumDepth = std::max(result.maximumDepth, opponentResult.maximumDepth+1);
			result.nodeCount += opponentResult.nodeCount;
			result.prunedCount += opponentResult.opponentPrunedCount;
			result.opponentPrunedCount += opponentResult.prunedCount;
			
			if (result.score > maximum)
			{
				result.prunedCount++;
				break;
			}
		}
	}
	
	return result;
}

Action findBestAction(const GameState& state, Evaluator evaluate, const Symbol symbol, const unsigned int maximumDepth)
{
	std::cout << "Thinking for player " << symbol << " (max depth = " << maximumDepth << ")..." << std::flush;
	
	const auto actions = state.possibleActionsFor(symbol);
	if (actions.empty())
		throw std::runtime_error("findBestAction() called on terminal node.");
	else
	{
		Action bestAction;
		Score score = -SCORE_MAX - 1;
		bool cutOff = false;
		unsigned int maximumDepthReached = 0;
		unsigned int nodeCount = 1 + actions.size(); // The +1 is for the root node.
		unsigned int prunedCount = 0;
		unsigned int opponentPrunedCount = 0;
		
		for (const auto& candidateAction: actions)
		{
			const GameState candidateState = state.apply(candidateAction);
			MinimaxResult candidateResult = minimax(candidateState, evaluate, opponentOf(symbol), maximumDepth, -SCORE_MAX, SCORE_MAX);
			candidateResult.score *= -1;
			
			cutOff |= candidateResult.cutOff;
			maximumDepthReached = std::max(maximumDepth, candidateResult.maximumDepth+1);
			nodeCount += candidateResult.nodeCount;
			prunedCount += candidateResult.prunedCount;
			opponentPrunedCount = candidateResult.opponentPrunedCount;
			if (candidateResult.score > score)
			{
				score = candidateResult.score;
				bestAction = candidateAction;
			}
		}
		
		std::cout << "selecting " << bestAction << ".  "
		          << "cut off: " << std::boolalpha << cutOff << ", "
		          << "maximum depth: " << maximumDepthReached << ", "
		          << "generated nodes: " << nodeCount << ", "
		          << "pruned subtrees: " << prunedCount << ", "
		          << "opponent's pruned subtrees: " << opponentPrunedCount << std::endl;
		return bestAction;
	}
}
