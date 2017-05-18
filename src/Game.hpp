#ifndef GAME_HPP_INCLUDED
#define GAME_HPP_INCLUDED

#include <cstddef>
#include <array>
#include <vector>
#include <iostream>

enum class Symbol
{
	EMPTY,
	X,
	O
};

std::ostream& operator<<(std::ostream& output, const Symbol symbol);

struct Action
{
	Symbol symbol = Symbol::EMPTY;
	std::size_t place = 0;
};

std::ostream& operator<<(std::ostream& ostream, const Action action);

struct GameState
{
	// A left-to-right, top-to-bottom list of tiles.
	std::array<Symbol, 16> symbols = {};
	
	// Returns a list of possible moves for the given symbol.
	std::vector<Action> possibleActionsFor(Symbol) const;
	
	// Returns the state after applying an action.
	GameState apply(Action) const;
	
	// Returns the 4 row lines.
	std::array<std::array<Symbol, 4>, 4> rows() const;
	
	// Returns the 4 column lines.
	std::array<std::array<Symbol, 4>, 4> columns() const;
	
	// Returns the 2 diagonal lines.
	std::array<std::array<Symbol, 4>, 2> diagonals() const;
	
	// Returns all rows, columns and diagonals.
	std::array<std::array<Symbol, 4>, 10> lines() const;
	
	// Returns the symbol that won the game, or EMPTY if neither has won (yet).
	// If there is more than one current winner, the results are undefined.
	Symbol winner() const;
	
	// Returns true if there is a winner, or if there are no spaces left.
	bool terminal() const;
};

// Returns O for X, X for O and EMPTY for EMPTY.
Symbol opponentOf(Symbol);

#endif
