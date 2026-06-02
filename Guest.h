#ifndef GUEST_H
#define GUEST_H

class Table; // Forward declaration

class Guest {
private:
    int guestId_m;
    int roomNum_m;
    int lastMealId_m;
    Table* currentTable_m; // Non-owning raw pointer to avoid cyclic dependencies

public:
    Guest(int id, int room) : 
        guestId_m(id), roomNum_m(room), lastMealId_m(0), currentTable_m(nullptr) {
    }

    // Getters
    int getGuestId() const {
        return guestId_m;
    }
    
    int getRoomNum() const {
        return roomNum_m;
    }
    
    int getLastMealId() const {
        return lastMealId_m;
    }
    
    Table* getCurrentTable() const {
        return currentTable_m;
    }

    // Setters
    void setLastMealId(int mealId) {
        lastMealId_m = mealId;
    }
    
    void setCurrentTable(Table* table) {
        currentTable_m = table;
    }

    // Explicit comparison operator for AVL tree
    bool operator<(const Guest& other) const {
        return this->guestId_m < other.guestId_m;
    }
    
    bool operator==(const Guest& other) const {
        return this->guestId_m == other.guestId_m;
    }
};

#endif // GUEST_H