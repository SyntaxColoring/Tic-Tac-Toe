#include "Game.hpp"

#include <algorithm>

std::ostream& operator<<(std::ostream& output, const Symbol symbol)
{
	switch (symbol)
	{
		case Symbol::EMPTY:
			output << '?';
			break;
		case Symbol::X:
			output << 'X';
			break;
		case Symbol::O:
			output << 'O';
			break;
		default:
			output << '!';
			break;
	}
	return output;
}

std::ostream& operator<<(std::ostream& output, const Action action)
{
	output << "[place " << action.symbol << " on " << action.place << "]";
	return output;
}

// If one symbol makes up the entirety of line, returns that symbol.  Else,
// returns Symbol::EMPTY.
static Symbol lineWinner(const std::array<Symbol, 4> line)
{
	for (std::size_t index = 1; index < line.size(); index++)
		if (line[index] != line[0]) return Symbol::EMPTY;
	return line[0];
}

std::vector<Action> GameState::possibleActionsFor(Symbol symbol) const
{
	std::vector<Action> result;
	result.reserve(symbols.size());
	for (std::size_t place = 0; place < symbols.size(); place++)
		if (symbols[place] == Symbol::EMPTY)
			result.push_back({symbol, place});
	return result;
}

GameState GameState::apply(const Action action) const
{
	GameState newState = *this;
	newState.symbols[action.place] = action.symbol;
	return newState;
}

std::array<std::array<Symbol, 4>, 4> GameState::rows() const
{
	std::array<std::array<Symbol, 4>, 4> rows;
	for (std::size_t row = 0; row < 4; row++)
		for (std::size_t column = 0; column < 4; column++)
			rows[row][column] = symbols[row*4+column];
	return rows;
}

std::array<std::array<Symbol, 4>, 4> GameState::columns() const
{
	std::array<std::array<Symbol, 4>, 4> columns;
	for (std::size_t row = 0; row < 4; row++)
		for (std::size_t column = 0; column < 4; column++)
			columns[column][row] = symbols[row*4+column];
	return columns;
}

std::array<std::array<Symbol, 4>, 2> GameState::diagonals() const
{
	// Tile layout:
	// 0  1  2  3
	// 4  5  6  7
	// 8  9  10 11
	// 12 13 14 15
	std::array<Symbol, 4> diagonal1 = {{symbols[0], symbols[5], symbols[10], symbols[15]}};
	std::array<Symbol, 4> diagonal2 = {{symbols[3], symbols[6], symbols[9], symbols[12]}};
	return {{diagonal1, diagonal2}};
}

std::array<std::array<Symbol, 4>, 10> GameState::lines() const
{
	std::array<std::array<Symbol, 4>, 10> lines;
	std::array<std::array<Symbol, 4>, 4> rows = this->rows();
	std::array<std::array<Symbol, 4>, 4> columns = this->columns();
	std::array<std::array<Symbol, 4>, 2> diagonals = this->diagonals();
	
	auto begin = lines.begin();
	begin = std::copy(rows.begin(), rows.end(), begin);
	begin = std::copy(columns.begin(), columns.end(), begin);
	begin = std::copy(diagonals.begin(), diagonals.end(), begin);
	return lines;
}

Symbol GameState::winner() const
{
	for (const auto& line: lines())
	{
		auto winner = lineWinner(line);
		if (winner != Symbol::EMPTY) return winner;
	}
	return Symbol::EMPTY;
}

bool GameState::terminal() const
{
	return winner() != Symbol::EMPTY || std::count(symbols.begin(), symbols.end(), Symbol::EMPTY) == 0;
}

Symbol opponentOf(Symbol symbol)
{
	switch (symbol)
	{
		case Symbol::O:
			return Symbol::X;
		case Symbol::X:
			return Symbol::O;
		default:
			return Symbol::EMPTY;
	}
}
