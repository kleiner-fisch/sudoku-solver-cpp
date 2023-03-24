/*
 * Board.h
 *
 *  Created on: Feb 26, 2023
 *      Author: heinrich
 */

#include "Entry.h"


#ifndef BOARD_H_
#define BOARD_H_


struct Board
{
    Entry entries[81];

    Board();
    Entry &getEntry(int x, int y);
    void getAreaAll(int x, int y, std::vector<Entry*> *output);
    void getAreaNoSelf(int x, int y, std::vector<Entry*> *output);
    void toString();

    Board copy();

    void getVerticalArea(int x, std::vector<Entry*> *output, bool clearOutput=true);
    void getHorizontalArea(int y, std::vector<Entry*> *output, bool clearOutput=true);
    void getSquareArea(int x, int y, std::vector<Entry*> *output, bool clearOutput=true);
    std::vector<std::vector<Entry*>> getAllSmallAreas();

    /**
     * Sets the highlight parameter of all entries to false
     */
    void resetHighlightedEntries();


};


#endif /* BOARD_H_ */
