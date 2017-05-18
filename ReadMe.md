Introduction
============
A 4x4 tic-tac-toe game with a GUI and AI, for an AI project.

Building
========

A makefile is provided for GNU Make.  It should work on any Unix-like system;
just run `make` from the same directory as the makefile and then run the executable
with `./main`.  Also, make sure you have the dependencies below pre-installed.

Dependencies
------------

  * C++14
  * libpthread
  * GLEW
  * SDL2

Files/Architecture
==================

Interface documentation is in `.hpp` files and implementation comments are in 
the corresponding `.cpp` files.

  * `GLEW.hpp`, `Shader.hpp`, `Graphics.hpp`/`Graphics.cpp`: boring, tedious, messy OpenGL stuff to build the GUI.
  * `Game.hpp`/`Game.cpp`: Defines the rules of the tic-tac-toe game.  Provides types for a game state, an action and a symbol (X or O).  Provides several convenience functions for things like iterating through the board line-by-line and checking who the winner is.
  * `AI.hpp`/`AI.cpp`: The interesting part.  Implements a couple of heuristic functions (the one specified in the assignment and an improved one).  Implements a minimax search with alpha-beta pruning.

Improved Heuristic Function
===========================

The AI uses a heuristic function other than the one specified in the assignment.  This improved function is based on the idea of *candidate lines.*  A candidate line is a line (a row, column or diagonal) that could potentially provide a win for the player in question.  A line counts as a candidate line for a player if the player's opponent has not occupied any space in that line.  The final heuristic score is the player's candidate line count minus the opponent's candidate line count.

In the original heuristic function's terms, the improved function could be expressed as `O_0 - X_0`.

The idea is that partially completed lines don't do us any good.  To win the game, we need to fill a line completely, and we can't do that if even a single space is occupied by the opponent.  Continuing to fill a line is pointless if the opponent has occupied a space in that line.  Also, even if the opponent hasn't occupied any spaces in that line, it still might be safer to occupy an entirely new line, because that instantly removes a way for the opponent to win.  This approximates the minimax strategy of minimizing maximum loss.
