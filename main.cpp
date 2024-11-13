#include <iostream>
#include <chrono>
#include <ctime>
#include <map>
#include <string>
#include <vector>

using namespace std;
using namespace std::chrono;

// Logger Class: Handles all logging and output operations
class Logger
{
public:
    static void log(const string &message)
    {
        cout << message << endl;
    }
};

// Abstract Base Class: Vehicle
class Vehicle
{
protected:
    string licensePlate;
    system_clock::time_point entryTime;

public:
    Vehicle(string licensePlate) : licensePlate(licensePlate), entryTime(system_clock::now()) {}

    // Pure virtual function making Vehicle an abstract class
    virtual void displayDetails() const = 0;
    virtual ~Vehicle() {}
};

// Derived Class: Car
class Car : public Vehicle
{
private:
    int token;
    system_clock::time_point exitTime;
    bool hasExited;
    static int tokenNum;

public:
    Car(string licensePlate) : Vehicle(licensePlate), token(tokenNum++), exitTime(system_clock::time_point()), hasExited(false) {}
    Car(string licensePlate, int customToken) : Vehicle(licensePlate), token(customToken), exitTime(system_clock::time_point()), hasExited(false) {}

    ~Car()
    {
        Logger::log("Destructor called for Car with License Plate: " + licensePlate);
    }

    int getTokenNum() const { return token; }
    bool isExited() const { return hasExited; }
    void setExitTime(system_clock::time_point time) { exitTime = time; }
    void setHasExited(bool status) { hasExited = status; }

    void exitCar()
    {
        if (!hasExited)
        {
            exitTime = system_clock::now();
            time_t exitTimeT = system_clock::to_time_t(exitTime);
            Logger::log("Car with " + licensePlate + " exited at " + string(ctime(&exitTimeT)));
            hasExited = true;
        }
        else
        {
            Logger::log("Car with " + licensePlate + " has already exited.");
        }
    }

    void displayDetails() const override
    {
        time_t entryTimeT = system_clock::to_time_t(entryTime);
        Logger::log("License Plate: " + licensePlate);
        Logger::log("Entry Time: " + string(ctime(&entryTimeT)));
        Logger::log("Token Number: " + to_string(token));
    }
};

int Car::tokenNum = 0;

// Base Class for Hierarchical Inheritance: ParkingBase
class ParkingBase
{
public:
    virtual void displayStatus() const = 0;
};

// Derived Class: ParkingSpot
class ParkingSpot : public ParkingBase
{
private:
    bool isAvailable;
    Car *assignedCar;
    int spotNumber;

public:
    ParkingSpot(int number) : isAvailable(true), assignedCar(nullptr), spotNumber(number) {}
    ~ParkingSpot()
    {
        if (assignedCar)
        {
            delete assignedCar;
        }
    }

    bool getAvailability() const { return isAvailable; }
    int getSpotNumber() const { return spotNumber; }
    Car *getAssignedCar() const { return assignedCar; }

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

    void displayStatus() const override
    {
        Logger::log("Spot Number: " + to_string(spotNumber) + ", Availability: " + (isAvailable ? "Yes" : "No"));
    }
};

// Derived Class: ParkingLot
class ParkingLot : public ParkingBase
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

    ~ParkingLot()
    {
        for (auto &spot : spots)
        {
            delete spot;
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
                    Logger::log("Car parked at spot: " + to_string(spot->getSpotNumber()));
                    return true;
                }
            }
        }
        Logger::log("No spots are available.");
        delete car;
        return false;
    }

    void removeCar(const string &licensePlate)
    {
        for (auto &spot : spots)
        {
            if (!spot->getAvailability() && spot->getAssignedCar()->licensePlate == licensePlate)
            {
                spot->getAssignedCar()->exitCar();
                totalCarAssigned--;
                spot->removeCar();
                Logger::log("Car removed from spot: " + to_string(spot->getSpotNumber()));
                return;
            }
        }
        Logger::log("Car with license plate " + licensePlate + " not found.");
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
        Logger::log("Total Available Spots: " + to_string(availableSpots));
    }

    void displayAllCars() const
    {
        Logger::log("Currently parked cars:");
        bool isParked = false;
        for (const auto &spot : spots)
        {
            if (!spot->getAvailability())
            {
                spot->getAssignedCar()->displayDetails();
                Logger::log("Assigned to spot: " + to_string(spot->getSpotNumber()));
                isParked = true;
            }
        }
        if (!isParked)
        {
            Logger::log("No cars are parked.");
        }
    }

    static void getTotalAssignedCars()
    {
        Logger::log("Total parked cars: " + to_string(totalCarAssigned));
    }

    void displayStatus() const override
    {
        Logger::log("ParkingLot Status:");
        for (const auto &spot : spots)
        {
            spot->displayStatus();
        }
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

            Car *enteredCar;
            if (licensePlate.length() > 5)
            {
                enteredCar = new Car(licensePlate, 999); // Using overloaded constructor
            }
            else
            {
                enteredCar = new Car(licensePlate); // Using default constructor
            }

            if (parkingLot1.parkCar(enteredCar))
            {
                Logger::log("Car added.");
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
            Logger::log("Exiting...");
            break;
        default:
            Logger::log("Invalid option.");
            break;
        }
    }

    return 0;
}
