#include <iostream>
#include <string>

using namespace std;

struct Room {
    int id;
    string type;
    string status;
    Room* next;

    Room(int i, string t) {
        id = i;
        type = t;
        status = "Ready";
        next = NULL;
    }
};

struct Floor {
    int floorNumber;
    Room* rooms;
    Floor* next;

    Floor(int f) {
        floorNumber = f;
        rooms = NULL;
        next = NULL;
    }
};

struct Booking {
    string customerName;
    string roomType;
    int stayDuration;
    bool isHighPriority;
};

struct QueueNode {
    Booking data;
    QueueNode* next;
};

struct StackNode {
    string customerName;
    int roomNumber;
    string roomType;
    int stayDuration;
    StackNode* next;
};

class HotelSystem {
private:
    Floor* hotel;
    QueueNode* highPriorityFront;
    QueueNode* highPriorityRear;
    QueueNode* regularFront;
    QueueNode* regularRear;
    StackNode* historyTop;

    void clearInputBuffer() {
        cin.clear();
        while (cin.get() != '\n');
    }

public:
    HotelSystem() {
        hotel = NULL;
        highPriorityFront = highPriorityRear = NULL;
        regularFront = regularRear = NULL;
        historyTop = NULL;
        initializeHotelLayout();
    }

    void initializeHotelLayout() {
        for (int i = 5; i >= 1; i--) {
            Floor* f = new Floor(i);
            f->next = hotel;
            hotel = f;

            for (int r = 10; r >= 1; r--) {
                string assignedType;
                if (r <= 5)
                    assignedType = "Single";
                else if (r <= 8)
                    assignedType = "Double";
                else
                    assignedType = "Suite";

                Room* newRoom = new Room(i * 100 + r, assignedType);
                newRoom->next = f->rooms;
                f->rooms = newRoom;
            }
        }
    }

    void addBookingRequest() {
        string name, type;
        int typeChoice, nights, priorityChoice;

        cout << "\n--- New Booking Request ---\n";
        cout << "Customer Name: ";
        cin >> name;

        cout << "Select Room Type (1.Single 2.Double 3.Suite): ";
        while (!(cin >> typeChoice) || typeChoice < 1 || typeChoice > 3) {
            cout << "Invalid choice. Enter again: ";
            clearInputBuffer();
        }

        if (typeChoice == 1)
            type = "Single";
        else if (typeChoice == 2)
            type = "Double";
        else
            type = "Suite";

        cout << "Nights: ";
        while (!(cin >> nights) || nights <= 0) {
            cout << "Invalid nights. Enter again: ";
            clearInputBuffer();
        }

        cout << "Priority (1=High, 0=Regular): ";
        while (!(cin >> priorityChoice) || (priorityChoice != 0 && priorityChoice != 1)) {
            cout << "Invalid priority. Enter again: ";
            clearInputBuffer();
        }

        QueueNode* newNode = new QueueNode;
        newNode->data.customerName = name;
        newNode->data.roomType = type;
        newNode->data.stayDuration = nights;
        if (priorityChoice == 1)
            newNode->data.isHighPriority = true;
        else
            newNode->data.isHighPriority = false;
        newNode->next = NULL;

        if (newNode->data.isHighPriority) {
            if (!highPriorityRear)
                highPriorityFront = highPriorityRear = newNode;
            else {
                highPriorityRear->next = newNode;
                highPriorityRear = newNode;
            }
        } else {
            if (!regularRear)
                regularFront = regularRear = newNode;
            else {
                regularRear->next = newNode;
                regularRear = newNode;
            }
        }

        cout << "Booking request added successfully.\n";
    }

    Room* findAvailableRoom(string type) {
        Floor* f = hotel;
        while (f) {
            Room* r = f->rooms;
            while (r) {
                if (r->type == type && r->status == "Ready")
                    return r;
                r = r->next;
            }
            f = f->next;
        }
        return NULL;
    }

    void executeBooking(Booking b) {
        Room* r = findAvailableRoom(b.roomType);

        if (r) {
            r->status = "Booked";
            cout << "SUCCESS: Room " << r->id << " assigned to " << b.customerName << endl;

            StackNode* node = new StackNode;
            node->customerName = b.customerName;
            node->roomNumber = r->id;
            node->roomType = r->type;
            node->stayDuration = b.stayDuration;
            node->next = historyTop;
            historyTop = node;
        } else {
            cout << "FAILED: No " << b.roomType << " room available for " << b.customerName << endl;
        }
    }

    void processQueues() {
        if (!highPriorityFront && !regularFront) {
            cout << "No pending bookings.\n";
            return;
        }

        while (highPriorityFront) {
            executeBooking(highPriorityFront->data);
            QueueNode* temp = highPriorityFront;
            highPriorityFront = highPriorityFront->next;
            delete temp;
        }

        while (regularFront) {
            executeBooking(regularFront->data);
            QueueNode* temp = regularFront;
            regularFront = regularFront->next;
            delete temp;
        }
    }

    void showBookingHistory() {
        if (!historyTop) {
            cout << "No booking history.\n";
            return;
        }

        StackNode* curr = historyTop;
        while (curr) {
            cout << curr->customerName << " | Room "
                 << curr->roomNumber << " | "
                 << curr->roomType << " | "
                 << curr->stayDuration << " Nights\n";
            curr = curr->next;
        }
    }

    void undoLastBooking() {
        if (!historyTop) {
            cout << "Nothing to undo.\n";
            return;
        }

        int roomID = historyTop->roomNumber;

        Floor* f = hotel;
        while (f) {
            Room* r = f->rooms;
            while (r) {
                if (r->id == roomID)
                    r->status = "Ready";
                r = r->next;
            }
            f = f->next;
        }

        StackNode* temp = historyTop;
        historyTop = historyTop->next;
        delete temp;

        cout << "Last booking undone successfully.\n";
    }

    void displayHotelMap() {
        Floor* f = hotel;
        while (f) {
            cout << "Floor " << f->floorNumber << " | ";
            Room* r = f->rooms;
            while (r) {
                cout << "[" << r->id << ":" << r->type[0] << ":";
                if (r->status == "Ready")
                    cout << "R";
                else
                    cout << "B";
                cout << "] ";
                r = r->next;
            }
            cout << endl;
            f = f->next;
        }
    }
};

int main() {
    HotelSystem galaxyHotel;
    int choice;

    while (true) {
        cout << "\n===== GALAXY HOTEL SYSTEM =====\n";
        cout << "1. Add Booking\n2. Process Bookings\n3. Show History\n";
        cout << "4. Undo Booking\n5. Hotel Map\n6. Exit\nChoice: ";
        cin >> choice;

        switch (choice) {
            case 1: galaxyHotel.addBookingRequest(); break;
            case 2: galaxyHotel.processQueues(); break;
            case 3: galaxyHotel.showBookingHistory(); break;
            case 4: galaxyHotel.undoLastBooking(); break;
            case 5: galaxyHotel.displayHotelMap(); break;
            case 6: return 0;
            default: cout << "Invalid option.\n";
        }
    }
}







