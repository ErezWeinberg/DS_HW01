// 
// 234218 Data Structures 1.
// Semester: 2026B (Spring).
// Wet Exercise #1.
// 
// The following header file contains all methods we expect you to implement.
// You MAY add private methods and fields of your own.
// DO NOT erase or modify the signatures of the public methods.
// DO NOT modify the preprocessors in this file.
// DO NOT use the preprocessors in your other code files.
// 

#ifndef SegmentationResort26SPRING_H_
#define SegmentationResort26SPRING_H_

#include "wet1util.h"
#include "Guest.h"
#include "Table.h"
#include "Room.h"
#include "AVLTree.h"
#include "ActiveRoomsList.h"

class SegmentationResort {
private:
    // Core data structures
    AVLTree<Guest, int> guestsTree_m;
    AVLTree<Table, int> tablesTree_m;
    AVLTree<Room, int> activeRoomsTree_m;
    ActiveRoomsList activeRoomsList_m;

    // Global state
    int currentMealId_m;

    
public:
    // <DO-NOT-MODIFY> {
    SegmentationResort();

    virtual ~SegmentationResort();

    StatusType checkIn(int geustId, int roomNum);

    StatusType checkOut(int geustId);

    StatusType addTable(int tableId, int capacity);

    StatusType removeTable(int tableId);

    StatusType enterDiningRoom(int guestId, int tableId);

    StatusType leaveDiningRoom(int guestId, int tableId);

    StatusType reheatFood();

    StatusType joinTables(int tableId1, int tableId2);
                    
    output_t<int> joinFriend(int guestId1, int guestId2);

    output_t<int> cleanNextRoom();

    // } </DO-NOT-MODIFY>
};

#endif // SegmentationResort26SPRING_H_
