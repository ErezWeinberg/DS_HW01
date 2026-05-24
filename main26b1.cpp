// 
// 234218 Data Structures 1.
// Semester: 2026B (Spring).
// Wet Exercise #1.
// 
// The following main file is necessary to link and run your code.
// This file is READ ONLY: even if you submit something else, the compiler will use our file.
// 

#include "SegmentationResort26b1.h"
#include <string>
#include <iostream>

using namespace std;

void print(string cmd, StatusType res);
void print(string cmd, output_t<int> res);
void print(string cmd, output_t<bool> res);

int main()
{
    
    int d1, d2;

    // Init
    SegmentationResort *obj = new SegmentationResort();
    
    // Execute all commands in file
    string op;
    while (cin >> op)
    {
        if (!op.compare("checkIn")) {
            cin >> d1 >> d2;
            print(op, obj->checkIn(d1, d2));
        } else if (!op.compare("checkOut")) {
            cin >> d1;
            print(op, obj->checkOut(d1));
        } else if (!op.compare("addTable")) {
            cin >> d1 >> d2;
            print(op, obj->addTable(d1, d2));
        } else if (!op.compare("removeTable")) {
            cin >> d1;
            print(op, obj->removeTable(d1));
        } else if (!op.compare("enterDiningRoom")) {
            cin >> d1 >> d2;
            print(op, obj->enterDiningRoom(d1, d2));
        } else if (!op.compare("leaveDiningRoom")) {
            cin >> d1 >> d2;
            print(op, obj->leaveDiningRoom(d1, d2));
        } else if (!op.compare("reheatFood")) {
            print(op, obj->reheatFood());
        } else if (!op.compare("joinTables")) {
            cin >> d1 >> d2;
            print(op, obj->joinTables(d1, d2));
        } else if (!op.compare("joinFriend")) {
            cin >> d1 >> d2;
            print(op, obj->joinFriend(d1, d2));
        } else if (!op.compare("cleanNextRoom")) {
            print(op, obj->cleanNextRoom());

        } else {
            cout << "Unknown command: " << op << endl;
            return -1;
        }
        // Verify no faults
        if (cin.fail()){
            cout << "Invalid input format" << endl;
            return -1;
        }
    }

    // Quit 
    delete obj;
    return 0;
}

// Helpers
static const char *StatusTypeStr[] =
{
    "SUCCESS",
    "ALLOCATION_ERROR",
    "INVALID_INPUT",
    "FAILURE"
};

void print(string cmd, StatusType res) 
{
    cout << cmd << ": " << StatusTypeStr[(int) res] << endl;
}

void print(string cmd, output_t<int> res)
{
    if (res.status() == StatusType::SUCCESS) {
        cout << cmd << ": " << StatusTypeStr[(int) res.status()] << ", " << res.ans() << endl;
    } else {
        cout << cmd << ": " << StatusTypeStr[(int) res.status()] << endl;
    }
}

void print(string cmd, output_t<bool> res)
{
    if (res.status() == StatusType::SUCCESS) {
        cout << cmd << ": " << StatusTypeStr[(int) res.status()] << ", " << (res.ans() ? "True" : "False") << endl;
    } else {
        cout << cmd << ": " << StatusTypeStr[(int) res.status()] << endl;
    }
}
