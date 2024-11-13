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

    virtual void displayDetails() const = 0; // Pure virtual function for displaying details
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

// Parking Base Class to demonstrate LSP
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
    Vehicle *assignedVehicle; // Polymorphism - We are assigning any vehicle, not just Car.
    int spotNumber;

public:
    ParkingSpot(int number) : isAvailable(true), assignedVehicle(nullptr), spotNumber(number) {}

    bool getAvailability() const { return isAvailable; }
    int getSpotNumber() const { return spotNumber; }
    Vehicle *getAssignedVehicle() const { return assignedVehicle; }

    bool assignVehicle(Vehicle *vehicle) // Using Vehicle pointer to accept any derived class (Car, Truck, etc.)
    {
        if (isAvailable)
        {
            isAvailable = false;
            assignedVehicle = vehicle;
            return true;
        }
        return false;
    }

    bool removeVehicle()
    {
        if (!isAvailable)
        {
            isAvailable = true;
            delete assignedVehicle;
            assignedVehicle = nullptr;
            return true;
        }
        return false;
    }

    void displayStatus() const override
    {
        Logger::log("Spot Number: " + to_string(spotNumber) + ", Availability: " + (isAvailable ? "Yes" : "No"));
    }
};

// ParkingLot class handling multiple spots
class ParkingLot : public ParkingBase
{
private:
    vector<ParkingSpot *> spots;
    static int totalVehicleAssigned;

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

    bool parkVehicle(Vehicle *vehicle)
    {
        for (auto &spot : spots)
        {
            if (spot->getAvailability())
            {
                if (spot->assignVehicle(vehicle))
                {
                    totalVehicleAssigned++;
                    Logger::log("Vehicle parked at spot: " + to_string(spot->getSpotNumber()));
                    return true;
                }
            }
        }
        Logger::log("No spots are available.");
        delete vehicle;
        return false;
    }

    void removeVehicle(const string &licensePlate)
    {
        for (auto &spot : spots)
        {
            if (!spot->getAvailability() && spot->getAssignedVehicle()->licensePlate == licensePlate)
            {
                spot->getAssignedVehicle()->displayDetails();
                spot->removeVehicle();
                totalVehicleAssigned--;
                Logger::log("Vehicle removed from spot: " + to_string(spot->getSpotNumber()));
                return;
            }
        }
        Logger::log("Vehicle with license plate " + licensePlate + " not found.");
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

    void displayAllVehicles() const
    {
        Logger::log("Currently parked vehicles:");
        bool isParked = false;
        for (const auto &spot : spots)
        {
            if (!spot->getAvailability())
            {
                spot->getAssignedVehicle()->displayDetails();
                Logger::log("Assigned to spot: " + to_string(spot->getSpotNumber()));
                isParked = true;
            }
        }
        if (!isParked)
        {
            Logger::log("No vehicles are parked.");
        }
    }

    static void getTotalAssignedVehicles()
    {
        Logger::log("Total parked vehicles: " + to_string(totalVehicleAssigned));
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

int ParkingLot::totalVehicleAssigned = 0;

int main()
{
    ParkingLot parkingLot1(30);
    bool running = true;

    while (running)
    {
        cout << "\n===== Parking Lot Management System =====" << endl;
        cout << "1. Add a new vehicle" << endl;
        cout << "2. Display all vehicles" << endl;
        cout << "3. Display the count of available spots." << endl;
        cout << "4. Exit the vehicle" << endl;
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
            cout << "Enter the vehicle's license plate: ";
            getline(cin, licensePlate);

            Vehicle *enteredVehicle;
            if (licensePlate.length() > 5)
            {
                enteredVehicle = new Car(licensePlate); // Can easily substitute Car with other Vehicle types
            }
            else
            {
                enteredVehicle = new Car(licensePlate); // Can easily be replaced with other vehicle types
            }

            if (parkingLot1.parkVehicle(enteredVehicle))
            {
                Logger::log("Vehicle added.");
            }
            break;
        }
        case 2:
            parkingLot1.displayAllVehicles();
            ParkingLot::getTotalAssignedVehicles();
            break;
        case 3:
            parkingLot1.displayAvailableSpots();
            break;
        case 4:
        {
            string licensePl;
            cout << "Enter the Vehicle License Plate number: ";
            getline(cin, licensePl);

            parkingLot1.removeVehicle(licensePl);
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
