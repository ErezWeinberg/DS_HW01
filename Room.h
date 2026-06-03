#ifndef ROOM_H
#define ROOM_H

class Room {
private:
    int roomNum_m;
    Room* next_m;
    Room* prev_m;

public:
    explicit Room(int num) : roomNum_m(num), next_m(nullptr), prev_m(nullptr) {}

    int getRoomNum() const { return roomNum_m; }
    Room* getNext() const { return next_m; }
    Room* getPrev() const { return prev_m; }

    void setNext(Room* nextRoom) { next_m = nextRoom; }
    void setPrev(Room* prevRoom) { prev_m = prevRoom; }

    bool operator<(const Room& other) const {
        return this->roomNum_m < other.roomNum_m;
    }

    bool operator==(const Room& other) const {
        return this->roomNum_m == other.roomNum_m;
    }
};

#endif // ROOM_H