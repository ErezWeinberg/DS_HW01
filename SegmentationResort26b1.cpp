#include "SegmentationResort26b1.h"

SegmentationResort::SegmentationResort() : currentMealId_m(0) {
}

SegmentationResort::~SegmentationResort() {
}

StatusType SegmentationResort::checkIn(int guestId, int roomNum) {
    if (guestId <= 0 || roomNum <= 0) return StatusType::INVALID_INPUT;

    try {
        try {
            guestsTree_m.find(guestId);
            return StatusType::FAILURE;
        } catch (const ElementNotFoundException&) {}

        try {
            activeRoomsTree_m.find(roomNum);
            return StatusType::FAILURE;
        } catch (const ElementNotFoundException&) {}

        auto predPtr = activeRoomsTree_m.findPredecessor(roomNum);
        Room* predecessor = predPtr ? predPtr->get() : nullptr;

        auto newGuest = std::make_shared<Guest>(guestId, roomNum);
        auto newRoom = std::make_shared<Room>(roomNum);

        guestsTree_m.insert(guestId, newGuest);
        activeRoomsTree_m.insert(roomNum, newRoom);

        activeRoomsList_m.insertRoom(newRoom.get(), predecessor);

    } catch (const std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }

    return StatusType::SUCCESS;
}

StatusType SegmentationResort::checkOut(int guestId) {
    if (guestId <= 0) return StatusType::INVALID_INPUT;

    try {
        auto guest = guestsTree_m.find(guestId);
        if (guest->getCurrentTable() != nullptr) {
            return StatusType::FAILURE;
        }

        int roomNum = guest->getRoomNum();
        auto room = activeRoomsTree_m.find(roomNum);

        activeRoomsList_m.removeRoom(room.get());
        activeRoomsTree_m.remove(roomNum);
        guestsTree_m.remove(guestId);

    } catch (const ElementNotFoundException&) {
        return StatusType::FAILURE;
    } catch (const std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }

    return StatusType::SUCCESS;
}

StatusType SegmentationResort::addTable(int tableId, int capacity) {
    if (tableId <= 0 || capacity <= 0) return StatusType::INVALID_INPUT;

    try {
        try {
            tablesTree_m.find(tableId);
            return StatusType::FAILURE;
        } catch (const ElementNotFoundException&) {}

        auto newTable = std::make_shared<Table>(tableId, capacity);
        tablesTree_m.insert(tableId, newTable);

    } catch (const std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }

    return StatusType::SUCCESS;
}

StatusType SegmentationResort::removeTable(int tableId) {
    if (tableId <= 0) return StatusType::INVALID_INPUT;

    try {
        auto table = tablesTree_m.find(tableId);
        if (table->getCurrentNumOfGuests() > 0) {
            return StatusType::FAILURE;
        }
        tablesTree_m.remove(tableId);
    } catch (const ElementNotFoundException&) {
        return StatusType::FAILURE;
    }

    return StatusType::SUCCESS;
}

StatusType SegmentationResort::enterDiningRoom(int guestId, int tableId) {
    if (guestId <= 0 || tableId <= 0) return StatusType::INVALID_INPUT;

    try {
        auto guest = guestsTree_m.find(guestId);
        auto table = tablesTree_m.find(tableId);

        if (guest->getCurrentTable() != nullptr) return StatusType::FAILURE;
        if (table->getCurrentNumOfGuests() >= table->getCapacity()) return StatusType::FAILURE;
        if (guest->getLastMealId() == currentMealId_m) return StatusType::FAILURE;

        guest->setLastMealId(currentMealId_m);
        guest->setCurrentTable(table.get());
        table->addGuest();
        table->getGuestsTree().insert(guestId, guest.get());

    } catch (const ElementNotFoundException&) {
        return StatusType::FAILURE;
    } catch (const std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }

    return StatusType::SUCCESS;
}

StatusType SegmentationResort::leaveDiningRoom(int guestId, int tableId) {
    if (guestId <= 0 || tableId <= 0) return StatusType::INVALID_INPUT;

    try {
        auto table = tablesTree_m.find(tableId);
        Guest* guestPtr = table->getGuestsTree().find(guestId);

        // התיקון הלוגי: אם האורח ישב בשולחן בזמן שהאוכל חומם (reheatFood),
        // הוא נחשב כמי שאכל את הארוחה החדשה! לכן צריך לעדכן לו את תעודת הזהות של הארוחה
        // ממש רגע לפני שהוא קם מהשולחן, כדי שלא יוכל להיכנס שוב באותה ארוחה.
        guestPtr->setLastMealId(currentMealId_m);

        guestPtr->setCurrentTable(nullptr);
        table->removeGuest();
        table->getGuestsTree().remove(guestId);

    } catch (const ElementNotFoundException&) {
        return StatusType::FAILURE;
    }

    return StatusType::SUCCESS;
}

StatusType SegmentationResort::reheatFood() {
    currentMealId_m++;
    return StatusType::SUCCESS;
}

StatusType SegmentationResort::joinTables(int tableId1, int tableId2) {
    if (tableId1 <= 0 || tableId2 <= 0 || tableId1 == tableId2) return StatusType::INVALID_INPUT;

    try {
        auto table1 = tablesTree_m.find(tableId1);
        auto table2 = tablesTree_m.find(tableId2);

        int size1 = table1->getGuestsTree().getSize();
        int size2 = table2->getGuestsTree().getSize();
        int totalSize = size1 + size2;

        using KVPair = AVLTree<Guest*, int>::KeyValuePair;
        auto arr1 = std::unique_ptr<KVPair[]>(new KVPair[size1 > 0 ? size1 : 1]);
        auto arr2 = std::unique_ptr<KVPair[]>(new KVPair[size2 > 0 ? size2 : 1]);
        auto mergedArr = std::unique_ptr<KVPair[]>(new KVPair[totalSize > 0 ? totalSize : 1]);

        table1->getGuestsTree().storeInArray(arr1.get());
        table2->getGuestsTree().storeInArray(arr2.get());

        int i = 0, j = 0, k = 0;
        while (i < size1 && j < size2) {
            if (arr1[i].key_m < arr2[j].key_m) {
                mergedArr[k++] = arr1[i++];
            } else {
                arr2[j].data_m->setCurrentTable(table1.get());
                mergedArr[k++] = arr2[j++];
            }
        }
        while (i < size1) mergedArr[k++] = arr1[i++];
        while (j < size2) {
            arr2[j].data_m->setCurrentTable(table1.get());
            mergedArr[k++] = arr2[j++];
        }

        table1->getGuestsTree().buildFromArray(mergedArr.get(), totalSize);
        table1->setCapacity(table1->getCapacity() + table2->getCapacity());
        table1->setCurrentNumOfGuests(totalSize);

        table2->getGuestsTree().clear();
        tablesTree_m.remove(tableId2);

    } catch (const ElementNotFoundException&) {
        return StatusType::FAILURE;
    } catch (const std::bad_alloc&) {
        return StatusType::ALLOCATION_ERROR;
    }

    return StatusType::SUCCESS;
}

output_t<int> SegmentationResort::joinFriend(int guestId1, int guestId2) {
    if (guestId1 <= 0 || guestId2 <= 0 || guestId1 == guestId2) return output_t<int>(StatusType::INVALID_INPUT);

    try {
        auto guest1 = guestsTree_m.find(guestId1);
        auto guest2 = guestsTree_m.find(guestId2);

        Table* targetTable = guest2->getCurrentTable();

        if (targetTable == nullptr) return output_t<int>(StatusType::FAILURE);
        if (guest1->getCurrentTable() != nullptr) return output_t<int>(StatusType::FAILURE);
        if (targetTable->getCurrentNumOfGuests() >= targetTable->getCapacity()) return output_t<int>(StatusType::FAILURE);
        if (guest1->getLastMealId() == currentMealId_m) return output_t<int>(StatusType::FAILURE);

        guest1->setLastMealId(currentMealId_m);
        guest1->setCurrentTable(targetTable);

        targetTable->addGuest();
        targetTable->getGuestsTree().insert(guest1->getGuestId(), guest1.get());

        return output_t<int>(targetTable->getTableId());

    } catch (const ElementNotFoundException&) {
        return output_t<int>(StatusType::FAILURE);
    } catch (const std::bad_alloc&) {
        return output_t<int>(StatusType::ALLOCATION_ERROR);
    }
}

output_t<int> SegmentationResort::cleanNextRoom() {
    return activeRoomsList_m.cleanNext();
}