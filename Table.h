#ifndef TABLE_H
#define TABLE_H

#include "AVLTree.h"

class Guest; // Forward declaration

class Table {
private:
    int tableId_m;
    int capacity_m;
    int currentNumOfGuests_m;
    AVLTree<Guest*, int> guestsInTable_m;

public:
    Table(int id, int cap) :
        tableId_m(id), capacity_m(cap), currentNumOfGuests_m(0) {}

    int getTableId() const { return tableId_m; }
    int getCapacity() const { return capacity_m; }
    int getCurrentNumOfGuests() const { return currentNumOfGuests_m; }

    AVLTree<Guest*, int>& getGuestsTree() { return guestsInTable_m; }

    void addGuest() { currentNumOfGuests_m++; }
    void removeGuest() { currentNumOfGuests_m--; }

    void setCapacity(int cap) { capacity_m = cap; }
    void setCurrentNumOfGuests(int num) { currentNumOfGuests_m = num; }

    bool operator<(const Table& other) const {
        return this->tableId_m < other.tableId_m;
    }
    
    bool operator==(const Table& other) const {
        return this->tableId_m == other.tableId_m;
    }
};

#endif // TABLE_H