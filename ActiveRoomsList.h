#ifndef ACTIVE_ROOMS_LIST_H
#define ACTIVE_ROOMS_LIST_H

#include "Room.h"
#include "wet1util.h" // For StatusType and output_t

class ActiveRoomsList {
private:
    Room* head_m;
    Room* tail_m;
    Room* nextRoomToClean_m;

public:
    ActiveRoomsList() : 
        head_m(nullptr), tail_m(nullptr), nextRoomToClean_m(nullptr) {}

    // Rule of Zero: No dynamic allocation in this class, raw pointers are non-owning.
    ~ActiveRoomsList() = default;

    ActiveRoomsList(const ActiveRoomsList&) = delete;
    ActiveRoomsList& operator=(const ActiveRoomsList&) = delete;

    // Insert a new room after the given predecessor. 
    // If predecessor is nullptr, insert at the head.
    void insertRoom(Room* newRoom, Room* predecessor) {
        if (!newRoom) {
            return;
        }

        if (!head_m) {
            // List is empty
            head_m = newRoom;
            tail_m = newRoom;
            newRoom->setNext(nullptr);
            newRoom->setPrev(nullptr);
            nextRoomToClean_m = newRoom; 
        } else if (!predecessor) {
            // Insert at head
            newRoom->setNext(head_m);
            newRoom->setPrev(nullptr);
            head_m->setPrev(newRoom);
            head_m = newRoom;
        } else {
            // Insert after predecessor
            Room* successor = predecessor->getNext();
            newRoom->setNext(successor);
            newRoom->setPrev(predecessor);
            predecessor->setNext(newRoom);
            
            if (successor) {
                successor->setPrev(newRoom);
            } else {
                // newRoom is the new tail
                tail_m = newRoom;
            }
        }
        
        // Ensure nextRoomToClean_m is valid
        if (!nextRoomToClean_m) {
            nextRoomToClean_m = head_m;
        }
    }

    // Remove a room and handle the nextRoomToClean_m pointer carefully
    void removeRoom(Room* roomToRemove) {
        if (!roomToRemove) {
            return;
        }

        // If we are removing the room that is next to be cleaned, advance the pointer
        if (nextRoomToClean_m == roomToRemove) {
            nextRoomToClean_m = nextRoomToClean_m->getNext();
            if (!nextRoomToClean_m && head_m && head_m != roomToRemove) {
                // If we reached the end, wrap around to head
                nextRoomToClean_m = head_m; 
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
        
        // If the list is now empty
        if (!head_m) {
            nextRoomToClean_m = nullptr;
        }
    }

    // O(1) operation to return the next room and advance the pointer
    output_t<int> cleanNext() {
        if (!nextRoomToClean_m) {
            return output_t<int>(StatusType::FAILURE);
        }

        int roomToReturn = nextRoomToClean_m->getRoomNum();
        
        nextRoomToClean_m = nextRoomToClean_m->getNext();
        if (!nextRoomToClean_m) {
            nextRoomToClean_m = head_m; // Wrap around to the beginning
        }

        return output_t<int>(roomToReturn);
    }
};

#endif // ACTIVE_ROOMS_LIST_H