#ifndef ACTIVE_ROOMS_LIST_H
#define ACTIVE_ROOMS_LIST_H

#include "Room.h"
#include "wet1util.h"

class ActiveRoomsList {
private:
    Room* head_m;
    Room* tail_m;
    Room* nextRoomToClean_m;
    int lastCleaned_m; // שומר את המזהה של החדר האחרון שנוקה

    // פונקציית העזר הקסומה: בודקת אם חדר a צריך להיות מנוקה לפני חדר b
    bool isBetter(int a, int b) {
        bool a_after = (a > lastCleaned_m);
        bool b_after = (b > lastCleaned_m);

        if (a_after && !b_after) return true;
        if (!a_after && b_after) return false;
        return a < b;
    }

public:
    ActiveRoomsList() :
        head_m(nullptr), tail_m(nullptr), nextRoomToClean_m(nullptr), lastCleaned_m(-1) {}

    ~ActiveRoomsList() = default;

    ActiveRoomsList(const ActiveRoomsList&) = delete;
    ActiveRoomsList& operator=(const ActiveRoomsList&) = delete;

    void insertRoom(Room* newRoom, Room* predecessor) {
        if (!newRoom) {
            return;
        }

        // --- הכנסה רגילה לרשימה המקושרת ---
        if (!head_m) {
            head_m = newRoom;
            tail_m = newRoom;
            newRoom->setNext(nullptr);
            newRoom->setPrev(nullptr);
        } else if (!predecessor) {
            newRoom->setNext(head_m);
            newRoom->setPrev(nullptr);
            head_m->setPrev(newRoom);
            head_m = newRoom;
        } else {
            Room* successor = predecessor->getNext();
            newRoom->setNext(successor);
            newRoom->setPrev(predecessor);
            predecessor->setNext(newRoom);

            if (successor) {
                successor->setPrev(newRoom);
            } else {
                tail_m = newRoom;
            }
        }

        // --- עדכון חכם של המצביע לניקיון ---
        if (!nextRoomToClean_m) {
            nextRoomToClean_m = newRoom;
        } else {
            // אם החדר החדש "עוקף בתור" לפי חוקי המעגל, המצביע יעבור אליו!
            if (isBetter(newRoom->getRoomNum(), nextRoomToClean_m->getRoomNum())) {
                nextRoomToClean_m = newRoom;
            }
        }
    }

    void removeRoom(Room* roomToRemove) {
        if (!roomToRemove) {
            return;
        }

        if (nextRoomToClean_m == roomToRemove) {
            nextRoomToClean_m = nextRoomToClean_m->getNext();
            // אם החדר שהוסר היה האחרון ברשימה, צריך לקפוץ להתחלה
            if (!nextRoomToClean_m && head_m && head_m != roomToRemove) {
                nextRoomToClean_m = head_m;
            } else if (!nextRoomToClean_m) {
                nextRoomToClean_m = nullptr;
            }
        }

        Room* prevRoom = roomToRemove->getPrev();
        Room* nextRoom = roomToRemove->getNext();

        if (prevRoom) {
            prevRoom->setNext(nextRoom);
        } else {
            head_m = nextRoom;
        }

        if (nextRoom) {
            nextRoom->setPrev(prevRoom);
        } else {
            tail_m = prevRoom;
        }

        if (!head_m) {
            nextRoomToClean_m = nullptr;
        }
    }

    output_t<int> cleanNext() {
        if (!nextRoomToClean_m) {
            return output_t<int>(StatusType::FAILURE);
        }

        int roomToReturn = nextRoomToClean_m->getRoomNum();

        // מעדכנים מהו החדר האחרון שנוקה
        lastCleaned_m = roomToReturn;

        nextRoomToClean_m = nextRoomToClean_m->getNext();
        if (!nextRoomToClean_m) {
            nextRoomToClean_m = head_m;
        }

        return output_t<int>(roomToReturn);
    }
};

#endif // ACTIVE_ROOMS_LIST_H