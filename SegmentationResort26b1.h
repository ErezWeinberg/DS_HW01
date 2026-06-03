#ifndef SegmentationResort26SPRING_H_
#define SegmentationResort26SPRING_H_

#include "wet1util.h"
#include "Guest.h"
#include "Table.h"
#include "Room.h"
#include "AVLTree.h"
#include "ActiveRoomsList.h"
#include <memory>

class SegmentationResort {
private:
    // Using shared_ptr prevents memory shifting during AVL rotations/removals
    AVLTree<std::shared_ptr<Guest>, int> guestsTree_m;
    AVLTree<std::shared_ptr<Table>, int> tablesTree_m;
    AVLTree<std::shared_ptr<Room>, int> activeRoomsTree_m;
    ActiveRoomsList activeRoomsList_m;
    
    int currentMealId_m;

public:
    // <DO-NOT-MODIFY> {
    SegmentationResort();
    virtual ~SegmentationResort();
    StatusType checkIn(int guestId, int roomNum);
    StatusType checkOut(int guestId);
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