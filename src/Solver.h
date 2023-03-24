/*
 * Solver.h
 *
 *  Created on: Feb 26, 2023
 *      Author: heinrich
 */

#include "Board.h"


#ifndef SOLVER_H_
#define SOLVER_H_


class Solver
{
public:
	  Solver(std::vector<int> *startingBoard);

	  void initializeBoard(std::vector<int> *startingBoard);

	  /**
	   * After fixing a value we remove the fixed value from the candidates of the entries area
	   */
	  bool propagateFixedValue();


	  std::vector<Board> moves;
	  Board currentBoard;

	  /**
	   * performs a deduction step. Returns true iff a deduction was made, and false otherwise.
	   * The deduction is performed on the current board, and the old board is added to the sequence of moves.
	   */
	  bool step();

	/**
	 * For all small areas checks whether there are n entries with exactly n equal candidates.
	 * If there are, these n candidates from all other entries of this small area.
	 * We only check for n=2.
	 */
	  bool propagateSmallAreaCandidates();

	/**
	 * Propagates constraints from an area (e.g. when in an area only a single entry possible can have a given) to the entries.
	 */
	bool propagateSmallAreaConstraints();

};


#endif /* SOLVER_H_ */
