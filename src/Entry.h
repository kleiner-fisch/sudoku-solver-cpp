/*
 * Entry.h
 *
 *  Created on: Feb 26, 2023
 *      Author: heinrich
 */

#include <set>
#include <string>
#include <vector>



#ifndef ENTRY_H_
#define ENTRY_H_

struct Entry{
    std::set<int> candidates;
    int x;
    int y;
    bool highlight;

    Entry(Entry *org);

    Entry();


    bool setValue(int value);
    std::string to_string();
    void setX(int x0);
    void setY(int y0);
    void setCandidates(std::set<int> candidates);

    friend bool operator==(const Entry& lhs, const Entry& rhs);
    friend bool operator!=(const Entry& lhs, const Entry& rhs);

};




#endif /* ENTRY_H_ */
