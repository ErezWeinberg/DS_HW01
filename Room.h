#ifndef ROOM_H
#define ROOM_H
class Room {
private:
    int roomNumber;
    Room* nextRoom;
    Room* previousRoom;

public:
    // בנאי
    explicit Room(int num):
    roomNumber(num),
    nextRoom(nullptr),
    previousRoom(nullptr){}
    // Getts
    int getRoomNumber()const {
        return roomNumber;
    }
    Room* getNextRoom()const {
        return nextRoom;
    }
    Room* getPreviousRoom()const {
        return previousRoom;
    }
    //Setts
    void setNextRoom(Room* nextRoom) {
        this->nextRoom = nextRoom;
    }
    void setPreviousRoom(Room* previousRoom) {
        this->previousRoom = previousRoom;
    }
    bool operator==(const Room& other)const {
        return this-> roomNumber == other.getRoomNumber();
    }
    bool operator<(const Room& other)const {
        return this->roomNumber < other.getRoomNumber();
    }
};
#endif //ROOM_H
