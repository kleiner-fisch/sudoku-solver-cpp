
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <algorithm>
#include <cassert>
#include <chrono>
#include <fstream>
#include <list>
#include <map>
#include <set>
#include <stdexcept>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <exception>

#include "Board.h"
#include "Entry.h"
#include "Solver.h"
namespace pt = boost::property_tree;


void Entry::setX(int x0) { x = x0; }
void Entry::setY(int y0) { y = y0; }
void Entry::setCandidates(std::set<int> candidates) {
    this->candidates.insert(candidates.begin(), candidates.end());
}

Entry::Entry(Entry *org) {
    this->x = org->x;
    this->y = org->y;
    this->candidates = org->candidates;
    this->highlight = org->highlight;
}

Entry::Entry() {}

/**
 * Sets this entries value. Returns whether the value was set successfully.
 * <p>Setting the value may fail, if the value is not contained in this entries
 * candidates. In this case we return false.</p>
 */
bool Entry::setValue(int value) {
    if ((this->candidates).count(value) == 1) {
        this->candidates = {value};
        return true;
    } else {
        this->candidates = {};
        return false;
    }
}
/**
 * returns a 3x3 block as a single string. First 3 chars represent first line
 * etc.
 *
 * If multiple numbers are possible, all are part of the string representation,
 * otherwise only the single number or a special string if no number is
 * possible.
 */
std::string Entry::to_string() {
    std::string result;
    if (this->candidates.size() == 1) {
        std::string number = std::to_string(*(this->candidates.begin()));
        result = "!!! " + number + " !!!";
    } else if (this->candidates.size() == 0) {
        result = "xxxxxxxxx";
    } else {
        for (int i = 1; i <= 9; i++) {
            if (this->candidates.count(i) == 1) {
                result += std::to_string(i);
            } else {
                result += " ";
            }
        }
    }
    return result;
}

bool operator==(Entry &a, Entry &b) { return a.x == b.x && a.y == b.y; }

bool operator!=(Entry &a, Entry &b) { return a.x != b.x || a.y != b.y; }

Board Board::copy() {
    Board result;
    for (int i; i < 81; i++) {
        result.entries[i] = Entry((&this->entries[i]));
    }
    return result;
}

Board::Board() {
    // First we set the coordinates
    for (int y = 0; y < 9; y++) {
        for (int x = 0; x < 9; x++) {
            getEntry(x, y).setX(x);
            getEntry(x, y).setY(y);
            getEntry(x, y).setCandidates({1, 2, 3, 4, 5, 6, 7, 8, 9});
        }
    }
}

Entry &Board::getEntry(int x, int y) { return entries[y * 9 + x]; }

/**
 * returns all entries that share constraints with the given coord/entry
 * (including the entry with the given coordinates itself)
 */
void Board::getAreaAll(int x, int y, std::vector<Entry *> *output) {
    output->clear();
    getVerticalArea(x, output, false);
    getHorizontalArea(y, output, false);
    getSquareArea(x, y, output, false);
    std::sort(output->begin(), output->end());
    output->erase(std::unique(output->begin(), output->end()), output->end());
    // we expect to collect exactly 21 entries
    assert(output->size() == 21);
}

/**
 * returns all entries that share constraints with the given coord/entry (except
 * the entry with the given coordinates itself)
 * */
void Board::getAreaNoSelf(int x, int y, std::vector<Entry *> *output) {
    getAreaAll(x, y, output);
    output->erase(std::remove(output->begin(), output->end(), &getEntry(x, y)),
                  output->end());
    // we expect to collect exactly 20 entries
    assert(output->size() == 20);
}

void Board::getVerticalArea(int x, std::vector<Entry *> *output,
                            bool clearOutput) {
    if (clearOutput) {
        output->clear();
    }
    for (int yCounter = 0; yCounter < 9; yCounter++) {
        output->push_back(&getEntry(x, yCounter));
    }
}

void Board::getHorizontalArea(int y, std::vector<Entry *> *output,
                              bool clearOutput) {
    if (clearOutput) {
        output->clear();
    }
    for (int xCounter = 0; xCounter < 9; xCounter++) {
        output->push_back(&getEntry(xCounter, y));
    }
}

void Board::getSquareArea(int x, int y, std::vector<Entry *> *output,
                          bool clearOutput) {
    if (clearOutput) {
        output->clear();
    }  // determines whether (x0, y0) is in the same 3x3 sudoku square as (x, y)
    auto isInSquare = [x, y](int x0, int y0) {
        auto const tmpX = std::div(x, 3);
        auto const tmpY = std::div(y, 3);
        return tmpX.quot * 3 <= x0 && tmpY.quot * 3 <= y0 &&
               x0 < tmpX.quot * 3 + 3 && y0 < tmpY.quot * 3 + 3;
    };
    int index = 0;
    for (int yCounter = 0; yCounter < 9; yCounter++) {
        for (int xCounter = 0; xCounter < 9; xCounter++) {
            if (isInSquare(xCounter, yCounter)) {
                output->push_back(&getEntry(xCounter, yCounter));
            }
        }
    }
}

std::vector<std::vector<Entry *>> Board::getAllSmallAreas() {
    std::vector<std::vector<Entry *>> areas;
    for (int i = 0; i < 9; i++) {
        std::vector<Entry *> hArea;
        getHorizontalArea(i, &hArea);
        areas.push_back(hArea);
        std::vector<Entry *> vArea;
        getVerticalArea(i, &vArea);
        areas.push_back(vArea);
    }
    for (int yCounter : {1, 4, 7}) {
        for (int xCounter : {1, 4, 7}) {
            std::vector<Entry *> sArea;
            getSquareArea(xCounter, yCounter, &sArea);
            areas.push_back(sArea);
        }
    }
    return areas;
}

void Board::resetHighlightedEntries() {
    for (int x = 0; x < 9; x++) {
        for (int y = 0; y < 9; y++) {
            Entry *entry = &getEntry(x, y);
            entry->highlight = false;
        }
    }
}

void Board::toString() {
    std::string single_line(55, '-');
    std::string double_line(55, '#');
    std::string result = "";
    std::string vsep;
    std::string hsep;
    for (int y = 0; y < 9; y++) {
        std::string top = "";
        std::string mid = "";
        std::string bot = "";
        y == 2 || y == 5 ? hsep = double_line : hsep = single_line;
        // Each entry consists of 3 rows and 3 columns.
        // We concatenate the top, middle and bottom row of all entries
        //      and then append the vertical separator.
        for (int x = 0; x < 9; x++) {
            x % 3 == 0 &&x > 0 ? vsep = " # " : vsep = " | ";
            Entry *entry = &getEntry(x, y);
            std::string string_rep = entry->to_string();
            std::string highlightPrefix = "";
            std::string highlightSuffix = "";
            if (entry->highlight) {
                highlightPrefix = "\033[36m";
                highlightSuffix = "\033[0m";
            }
            top += vsep + highlightPrefix + string_rep.substr(0, 3) +
                   highlightSuffix;
            mid += vsep + highlightPrefix + string_rep.substr(3, 3) +
                   highlightSuffix;
            bot += vsep + highlightPrefix + string_rep.substr(6, 3) +
                   highlightSuffix;
        }
        result += top + " |\n" + mid + " |\n" + bot + " |\n" + hsep + "\n";
    }
    std::cout << result << std::endl;
}

Solver::Solver(std::vector<int> *startingBoard) {
    currentBoard.resetHighlightedEntries();
    initializeBoard(startingBoard);
    /*if (startingBoard->size() >= 1) {
        for (Entry &entry : this->currentBoard.entries) {
            if (entry.candidates.size() == 1) {
                this->fixedValues.push_back(&entry);
            }
        }
    }*/
}

/**
 * The format is x0, y0, value0, x1, y1, value1, ... with (0, 0) being the top
 * left entry.
 */
void Solver::initializeBoard(std::vector<int> *startingBoard) {
    int x, y, value;
    for (size_t i = 0; i < startingBoard->size() / 3; i++) {
        x = startingBoard->at(i * 3), y = startingBoard->at((i * 3) + 1),
        value = startingBoard->at((i * 3) + 2);
        Entry *currentEntry = &(this->currentBoard.getEntry(x, y));
        currentEntry->setValue(value);
        currentEntry->highlight = true;
    }
}

bool Solver::step() {
    // this->moves.push_back(this->currentBoard.copy());
    bool changedBoard = false;
    currentBoard.resetHighlightedEntries();
    if (!changedBoard) {
        changedBoard = propagateFixedValue();
    }
    if (!changedBoard) {
        changedBoard = propagateSmallAreaCandidates();
    }
    if (!changedBoard) {
        changedBoard = propagateSmallAreaConstraints();
    }
    return changedBoard;
}

bool Solver::propagateSmallAreaCandidates() {
    bool changedBoard = false;
    for (std::vector<Entry *> area : currentBoard.getAllSmallAreas()) {
        for (int i = 0; i < 9; i++) {
            for (int k = i + 1; k < 9; k++) {
                if (area[i]->candidates.size() == 2 &&
                    area[i]->candidates == area[k]->candidates) {
                    // We remove the two canditates <it> and <nx>
                    // that in this area are reserved for the entries at
                    // area location <i> and <k>
                    auto it = area[i]->candidates.begin();
                    auto nx = std::next(it, 1);
                    for (Entry *entry : area) {
                        if (entry != area[i] && entry != area[k]) {
                            int oldSize = entry->candidates.size();
                            entry->candidates.erase(*it);
                            entry->candidates.erase(*nx);
                            // If the candidates were changed, we wish to
                            // highlight the entry, and remember the board was
                            // changed.
                            //      Otherwise, we only remember the previous
                            //      state of whether the board was changed
                            entry->highlight =
                                oldSize != entry->candidates.size();
                            // oldSize == entry->candidates.size() ?
                            // entry->highlight = true : entry->highlight =
                            // false ;
                            changedBoard = changedBoard || entry->highlight;
                        }
                    }
                }
            }
        }
    }
    return changedBoard;
}

bool Solver::propagateSmallAreaConstraints() {
    bool changedBoard = false;
    for (std::vector<Entry *> area : currentBoard.getAllSmallAreas()) {
        for (int i = 1; i <= 9; i++) {
            Entry *storedEntry;
            int number;
            int counter = 0;
            for (Entry *entry : area) {
                // We are only interested in non-fixed entries yet ..
                if (entry->candidates.count(i) == 1) {
                    counter++;
                    storedEntry = entry;
                    number = i;
                }
                // ... and the candidate should occur exactly once
                if (counter >= 2) {
                    break;
                }
            }
            if (counter == 1 && storedEntry->candidates.size() > 1) {
                storedEntry->setValue(number);
                changedBoard = true;
                storedEntry->highlight = true;
            }
        }
    }
    return changedBoard;
}

bool Solver::propagateFixedValue() {
    bool changedBoard = false;
    for (int x = 0; x < 9; x++) {
        for (int y = 0; y < 9; y++) {
            Entry *entry = &(currentBoard.getEntry(x, y));
            if (entry->candidates.size() == 1) {
                int value = *(entry->candidates.begin());
                std::vector<Entry *> area;
                currentBoard.getAreaNoSelf(entry->x, entry->y, &area);
                for (Entry *otherEntry : area) {
                    if (otherEntry->candidates.count(value) >= 1) {
                        changedBoard = true;
                        otherEntry->highlight = true;
                        otherEntry->candidates.erase(value);
                    }
                }
            }
        }
    }

    return changedBoard;
}


void load(const std::string &filename, std::vector<int> *values)
{
    // Create empty property tree object
    pt::ptree tree;

    // Parse the XML into the property tree.
    pt::read_json(filename, tree);

    for(pt::ptree::value_type &v : tree){
        assert("" == v.first);
        pt::ptree entry = v.second;
        values->push_back(entry.get<int>("entry.x"));
        values->push_back(entry.get<int>("entry.y"));
        values->push_back(entry.get<int>("entry.value"));
    }

}

int main() {
    auto startTime = std::chrono::high_resolution_clock::now();
    Board currentBoard;

    std::vector<int> start;
    load("sudoku-example1.json", &start);

    Solver solver = Solver(&start);
    solver.currentBoard.toString();

    bool changedBoard = true;
    while (changedBoard) {
        std::cout << " !!!!!!!!!!!!!!!!!!!!!!!!! " << std::endl;
        changedBoard = solver.step();
        solver.currentBoard.toString();
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(end - startTime);
    std::cout << "Running time: " << duration.count() << std::endl;
    return 0;
}
