// You can edit anything you want in this file.
// However you need to implement all public StudentCourseManager function, as provided below as a template

#include "SegmentationResort26b1.h"


SegmentationResort::SegmentationResort()
{
    
}

SegmentationResort::~SegmentationResort()
{
    
}

StatusType SegmentationResort::checkIn(int guestId, int roomNum) {
    if (guestId <= 0 || roomNum <= 0) {
        return StatusType::INVALID_INPUT;
    }

    try {
        // Step 1: Verify guest doesn't exist
        try {
            guestsTree_m.find(guestId);
            return StatusType::FAILURE;
        } catch (const ElementNotFoundException&) {
            // Expected flow - guest does not exist
        }

        // Step 2: Verify room is not occupied
        try {
            activeRoomsTree_m.find(roomNum);
            return StatusType::FAILURE;
        } catch (const ElementNotFoundException&) {
            // Expected flow - room is empty
        }

        // Step 3: Find predecessor BEFORE inserting to easily link in the list
        Room* predecessor = activeRoomsTree_m.findPredecessor(roomNum);

        // Step 4: Insert to our AVL trees
        guestsTree_m.insert(guestId, Guest(guestId, roomNum));
        activeRoomsTree_m.insert(roomNum, Room(roomNum));

        // Step 5: Get stable pointer from the tree and link it in the list
        Room* newRoomPtr = &(activeRoomsTree_m.find(roomNum));
        activeRoomsList_m.insertRoom(newRoomPtr, predecessor);

    } catch (const std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }

    return StatusType::SUCCESS;
}

StatusType SegmentationResort::checkOut(int guestId) {
    if (guestId <= 0) {
        return StatusType::INVALID_INPUT;
    }

    try {
        // Step 1: Find guest
        Guest& guest = guestsTree_m.find(guestId);

        // Step 2: Ensure guest is not currently in the dining room
        if (guest.getCurrentTable() != nullptr) {
            return StatusType::FAILURE;
        }

        int roomNum = guest.getRoomNum();

        // Step 3: Get the room from tree and remove from linked list
        Room& room = activeRoomsTree_m.find(roomNum);
        activeRoomsList_m.removeRoom(&room);

        // Step 4: Remove from trees
        activeRoomsTree_m.remove(roomNum);
        guestsTree_m.remove(guestId);

    } catch (const ElementNotFoundException&) {
        // Guest not found in the tree
        return StatusType::FAILURE;
    } catch (const std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }

    return StatusType::SUCCESS;
}

StatusType SegmentationResort::addTable(int tableId, int capacity) {
    if (tableId <= 0 || capacity <= 0) {
        return StatusType::INVALID_INPUT;
    }

    try {
        // Verify table doesn't already exist
        try {
            tablesTree_m.find(tableId);
            return StatusType::FAILURE;
        } catch (const ElementNotFoundException&) {
            // Expected flow - table does not exist
        }

        tablesTree_m.insert(tableId, Table(tableId, capacity));

    } catch (const std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }

    return StatusType::SUCCESS;
}

StatusType SegmentationResort::removeTable(int tableId) {
    if (tableId <= 0) {
        return StatusType::INVALID_INPUT;
    }

    try {
        Table& table = tablesTree_m.find(tableId);

        // Cannot remove a table that has guests currently sitting at it
        if (table.getCurrentNumOfGuests() > 0) {
            return StatusType::FAILURE;
        }

        tablesTree_m.remove(tableId);

    } catch (const ElementNotFoundException&) {
        return StatusType::FAILURE;
    }

    return StatusType::SUCCESS;
}

StatusType SegmentationResort::enterDiningRoom(int guestId, int tableId) {
    if (guestId <= 0 || tableId <= 0) {
        return StatusType::INVALID_INPUT;
    }

    try {
        Guest& guest = guestsTree_m.find(guestId);
        Table& table = tablesTree_m.find(tableId);

        // Verify guest is not already at a table
        if (guest.getCurrentTable() != nullptr) {
            return StatusType::FAILURE;
        }

        // Verify table has capacity
        if (table.getCurrentNumOfGuests() == table.getCapacity()) {
            return StatusType::FAILURE;
        }

        // Verify guest hasn't already participated in the current meal
        if (guest.getLastMealId() == currentMealId_m) {
            return StatusType::FAILURE;
        }

        // Update states
        guest.setLastMealId(currentMealId_m);
        guest.setCurrentTable(&table);

        table.addGuest();
        table.getGuestsTree().insert(guestId, &guest);

    } catch (const ElementNotFoundException&) {
        return StatusType::FAILURE;
    } catch (const std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }

    return StatusType::SUCCESS;
}

StatusType SegmentationResort::leaveDiningRoom(int guestId, int tableId) {
    if (guestId <= 0 || tableId <= 0) {
        return StatusType::INVALID_INPUT;
    }

    try {
        Table& table = tablesTree_m.find(tableId);

        // Find the guest strictly in the internal table tree to ensure O(log(n_tableId))
        Guest* guestPtr = table.getGuestsTree().find(guestId);

        // Disconnect guest from table
        guestPtr->setCurrentTable(nullptr);

        // Update table state
        table.removeGuest();
        table.getGuestsTree().remove(guestId);

    } catch (const ElementNotFoundException&) {
        // Either table doesn't exist, or guest is not at this specific table
        return StatusType::FAILURE;
    }

    return StatusType::SUCCESS;
}
StatusType SegmentationResort::reheatFood() {
    currentMealId_m++;
    return StatusType::SUCCESS;
}


StatusType SegmentationResort::joinTables(int tableId1, int tableId2) {
    if (tableId1 <= 0 || tableId2 <= 0 || tableId1 == tableId2) {
        return StatusType::INVALID_INPUT;
    }

    try {
        Table& table1 = tablesTree_m.find(tableId1);
        Table& table2 = tablesTree_m.find(tableId2);

        int size1 = table1.getGuestsTree().getSize();
        int size2 = table2.getGuestsTree().getSize();
        int totalSize = size1 + size2;

        // Use smart pointers for dynamic arrays to guarantee no memory leaks
        using KVPair = AVLTree<Guest*, int>::KeyValuePair;
        auto arr1 = std::unique_ptr<KVPair[]>(new KVPair[size1 > 0 ? size1 : 1]);
        auto arr2 = std::unique_ptr<KVPair[]>(new KVPair[size2 > 0 ? size2 : 1]);
        auto mergedArr = std::unique_ptr<KVPair[]>(new KVPair[totalSize > 0 ? totalSize : 1]);

        table1.getGuestsTree().storeInArray(arr1.get());
        table2.getGuestsTree().storeInArray(arr2.get());

        // Merge the two sorted arrays (MergeSort logic)
        int i = 0, j = 0, k = 0;
        while (i < size1 && j < size2) {
            if (arr1[i].key_m < arr2[j].key_m) {
                mergedArr[k++] = arr1[i++];
            } else {
                // Update table pointer for guests moving from table2 to table1
                arr2[j].data_m->setCurrentTable(&table1);
                mergedArr[k++] = arr2[j++];
            }
        }

        while (i < size1) {
            mergedArr[k++] = arr1[i++];
        }

        while (j < size2) {
            arr2[j].data_m->setCurrentTable(&table1);
            mergedArr[k++] = arr2[j++];
        }

        // Build the new perfectly balanced tree in table1
        table1.getGuestsTree().buildFromArray(mergedArr.get(), totalSize);

        // Update table1 attributes
        table1.setCapacity(table1.getCapacity() + table2.getCapacity());
        table1.setCurrentNumOfGuests(totalSize);

        // Safely clear table2's tree nodes (Guest objects remain safe in global tree) and remove table
        table2.getGuestsTree().clear();
        tablesTree_m.remove(tableId2);

    } catch (const ElementNotFoundException&) {
        return StatusType::FAILURE;
    } catch (const std::bad_alloc&) {
        // Critical memory error routing
        std::cerr << "Critical Error: Memory allocation failed during joinTables." << std::endl;
        return StatusType::ALLOCATION_ERROR;
    }

    return StatusType::SUCCESS;
}

output_t<int> SegmentationResort::joinFriend(int guestId1, int guestId2) {
    if (guestId1 <= 0 || guestId2 <= 0 || guestId1 == guestId2) {
        return output_t<int>(StatusType::INVALID_INPUT);
    }

    try {
        Guest& guest1 = guestsTree_m.find(guestId1);
        Guest& guest2 = guestsTree_m.find(guestId2);

        Table* targetTable = guest2.getCurrentTable();

        if (targetTable == nullptr) {
            return output_t<int>(StatusType::FAILURE);
        }

        if (guest1.getCurrentTable() != nullptr) {
            return output_t<int>(StatusType::FAILURE);
        }

        if (targetTable->getCurrentNumOfGuests() == targetTable->getCapacity()) {
            return output_t<int>(StatusType::FAILURE);
        }

        if (guest1.getLastMealId() == currentMealId_m) {
            return output_t<int>(StatusType::FAILURE);
        }

        // Apply state changes cleanly
        guest1.setLastMealId(currentMealId_m);
        guest1.setCurrentTable(targetTable);

        targetTable->addGuest();
        targetTable->getGuestsTree().insert(guest1.getGuestId(), &guest1);

        return output_t<int>(targetTable->getTableId());

    } catch (const ElementNotFoundException&) {
        return output_t<int>(StatusType::FAILURE);
    } catch (const std::bad_alloc&) {
        std::cerr << "Critical Error: Memory allocation failed during joinFriend." << std::endl;
        return output_t<int>(StatusType::ALLOCATION_ERROR);
    }
}
output_t<int> SegmentationResort::cleanNextRoom() {
    return activeRoomsList_m.cleanNext();
}