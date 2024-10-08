#include <iostream>
#include <chrono>
#include <ctime>
#include <map>
#include <string>
#include <vector>

using namespace std;
using namespace std::chrono;

class Car
{
private:
    // Private data members
    string licensePlate;
    system_clock::time_point entryTime;
    int token;
    system_clock::time_point exitTime;
    bool hasExited;
    static int tokenNum;

public:
    // public methods
    // Default and Parameterized Constructors
    Car() : licensePlate(""), entryTime(system_clock::now()), token(tokenNum++), exitTime(system_clock::time_point()), hasExited(false) {}
    Car(string licensePlate) : licensePlate(licensePlate), entryTime(system_clock::now()), token(tokenNum++), exitTime(system_clock::time_point()), hasExited(false) {}

    // Destructor
    ~Car()
    {
        cout << "Destructor called for Car with License Plate: " << licensePlate << endl;
    }

    // Accessors(Provide read-only access to private data)
    string getLicensePlate() const { return licensePlate; }
    system_clock::time_point getEntryTime() const { return entryTime; }
    int getTokenNum() const { return token; }
    system_clock::time_point getExitTime() const { return exitTime; }
    bool isExited() const { return hasExited; }

    // Mutators(Provide controlled write access to private data)
    void setLicensePlate(const string &plate) { licensePlate = plate; }
    void setExitTime(system_clock::time_point time) { exitTime = time; }
    void setHasExited(bool status) { hasExited = status; }

    // Other Methods
    void displayDetails() const
    {
        time_t entryTimeT = system_clock::to_time_t(entryTime);
        cout << "License Plate: " << this->licensePlate << endl;
        cout << "Entry Time: " << ctime(&entryTimeT);
        cout << "Token Number: " << this->token << endl;
    }

    void exitCar()
    {
        if (!this->hasExited)
        {
            this->exitTime = system_clock::now();
            time_t exitTimeT = system_clock::to_time_t(this->exitTime);
            cout << "Car with " << this->licensePlate << " exited at " << ctime(&exitTimeT) << endl;
            this->hasExited = true;
        }
        else
        {
            cout << "Car with " << this->licensePlate << " has already exited." << endl;
        }
    }
};

int Car::tokenNum = 0;

class ParkingSpot
{
private:
//private data members
    bool isAvailable;
    Car *assignedCar;
    int spotNumber;

public:
//public methods
    // Constructor
    ParkingSpot(int number) : isAvailable(true), assignedCar(nullptr), spotNumber(number) {}

    // Destructor
    ~ParkingSpot()
    {
        if (assignedCar)
        {
            cout << "Destructor called for ParkingSpot " << spotNumber << " with assigned car: " << assignedCar->getLicensePlate() << endl;
            delete assignedCar;
        }
        else
        {
            cout << "Destructor called for ParkingSpot " << spotNumber << " with no car assigned." << endl;
        }
    }

    // Accessors(Provide read-only access to private data)
    bool getAvailability() const { return isAvailable; }
    int getSpotNumber() const { return spotNumber; }
    Car *getAssignedCar() const { return assignedCar; }

    // Mutators(Provide controlled write access to private data)
    void setAvailability(bool availability) { isAvailable = availability; }
    void setAssignedCar(Car *car) { assignedCar = car; }

    // Other Methods
    bool assignCar(Car *car)
    {
        if (isAvailable)
        {
            isAvailable = false;
            assignedCar = car;
            return true;
        }
        return false;
    }

    bool removeCar()
    {
        if (!isAvailable)
        {
            isAvailable = true;
            delete assignedCar;
            assignedCar = nullptr;
            return true;
        }
        return false;
    }
};

class ParkingLot
{
private:
    vector<ParkingSpot *> spots;
    static int totalCarAssigned;

public:
    ParkingLot(int totalSpots)
    {
        for (int i = 0; i < totalSpots; ++i)
        {
            spots.push_back(new ParkingSpot(i + 1));
        }
    }

    bool parkCar(Car *car)
    {
        for (auto &spot : spots)
        {
            if (spot->getAvailability())
            {
                if (spot->assignCar(car))
                {
                    totalCarAssigned++;
                    cout << "Car parked at spot: " << spot->getSpotNumber() << endl;
                    return true;
                }
            }
        }
        cout << "No spots are available." << endl;
        delete car;
        return false;
    }

    void removeCar(const string &licensePlate)
    {
        for (auto &spot : spots)
        {
            if (!spot->getAvailability() && spot->getAssignedCar()->getLicensePlate() == licensePlate)
            {
                spot->getAssignedCar()->exitCar();
                totalCarAssigned--;
                spot->removeCar();
                cout << "Car removed from spot: " << spot->getSpotNumber() << endl;
                return;
            }
        }
        cout << "Car with license plate " << licensePlate << " not found." << endl;
    }

    void displayAvailableSpots() const
    {
        int availableSpots = 0;
        for (const auto &spot : spots)
        {
            if (spot->getAvailability())
            {
                availableSpots++;
            }
        }
        cout << "Total Available Spots: " << availableSpots << endl;
    }

    void displayAllCars() const
    {
        cout << "Currently parked cars:" << endl;
        bool isParked = false;
        for (const auto &spot : spots)
        {
            if (!spot->getAvailability())
            {
                spot->getAssignedCar()->displayDetails();
                cout << "Assigned to spot: " << spot->getSpotNumber() << endl;
                cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
                isParked = true;
            }
        }
        if (!isParked)
        {
            cout << "No cars are parked." << endl;
        }
    }

    static void getTotalAssignedCars()
    {
        cout << "Total parked cars: " << totalCarAssigned << endl;
    }
};

int ParkingLot::totalCarAssigned = 0;

int main()
{
    ParkingLot parkingLot1(30);
    bool running = true;

    while (running)
    {
        cout << "\n===== Parking Lot Management System =====" << endl;
        cout << "1. Add a new car" << endl;
        cout << "2. Display all cars" << endl;
        cout << "3. Display the count of available spots." << endl;
        cout << "4. Exit the car" << endl;
        cout << "5. Exit the terminal" << endl;
        cout << "Choose an option: ";

        int option;
        cin >> option;
        cin.ignore();

        switch (option)
        {
        case 1:
        {
            string licensePlate;
            cout << "Enter the car's license plate: ";
            getline(cin, licensePlate);

            Car *enteredCar = new Car(licensePlate);
            if (parkingLot1.parkCar(enteredCar))
            {
                cout << "Car added." << endl;
            }
            break;
        }
        case 2:
            parkingLot1.displayAllCars();
            ParkingLot::getTotalAssignedCars();
            break;
        case 3:
            parkingLot1.displayAvailableSpots();
            break;
        case 4:
        {
            string licensePl;
            cout << "Enter the Car License Plate number: ";
            getline(cin, licensePl);

            parkingLot1.removeCar(licensePl);
            break;
        }
        case 5:
            running = false;
            cout << "Exiting the system." << endl;
            break;
        default:
            cout << "Invalid option. Please try again." << endl;
            break;
        }
    }

    return 0;
}
