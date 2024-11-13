#include <iostream>
#include <chrono>
#include <ctime>
#include <map>
#include <string>
#include <vector>

using namespace std;
using namespace std::chrono;

// Base Class: Vehicle
class Vehicle
{
protected:
    string licensePlate;
    system_clock::time_point entryTime;

public:
    Vehicle(string licensePlate) : licensePlate(licensePlate), entryTime(system_clock::now()) {}
    virtual void displayDetails() const
    {
        time_t entryTimeT = system_clock::to_time_t(entryTime);
        cout << "License Plate: " << licensePlate << endl;
        cout << "Entry Time: " << ctime(&entryTimeT);
    }
};

// Derived Class (Single Inheritance): Car
class Car : public Vehicle
{
private:
    int token;
    system_clock::time_point exitTime;
    bool hasExited;
    static int tokenNum;

public:
    // Constructor for Car, initializing base class and unique members
    Car(string licensePlate) : Vehicle(licensePlate), token(tokenNum++), exitTime(system_clock::time_point()), hasExited(false) {}

    // Overloaded Constructor with a custom token number
    Car(string licensePlate, int customToken) : Vehicle(licensePlate), token(customToken), exitTime(system_clock::time_point()), hasExited(false) {}

    // Destructor to display message when Car object is destroyed
    ~Car()
    {
        cout << "Destructor called for Car with License Plate: " << licensePlate << endl;
    }

    int getTokenNum() const { return token; }
    bool isExited() const { return hasExited; }
    void setExitTime(system_clock::time_point time) { exitTime = time; }
    void setHasExited(bool status) { hasExited = status; }

    // Method to handle car exit and record exit time
    void exitCar()
    {
        if (!hasExited)
        {
            exitTime = system_clock::now();
            time_t exitTimeT = system_clock::to_time_t(exitTime);
            cout << "Car with " << licensePlate << " exited at " << ctime(&exitTimeT) << endl;
            hasExited = true;
        }
        else
        {
            cout << "Car with " << licensePlate << " has already exited." << endl;
        }
    }

    // Overridden method to display car details including token number
    void displayDetails() const override
    {
        Vehicle::displayDetails();
        cout << "Token Number: " << token << endl;
    }
};

int Car::tokenNum = 0; // Static member initialization

// Base Class for Hierarchical Inheritance: ParkingBase
class ParkingBase
{
public:
    virtual void displayStatus() const = 0; // Pure virtual function for displaying status
};

// Derived Class (Hierarchical Inheritance): ParkingSpot
class ParkingSpot : public ParkingBase
{
private:
    bool isAvailable;
    Car *assignedCar;
    int spotNumber;

public:
    // Constructor to initialize ParkingSpot
    ParkingSpot(int number) : isAvailable(true), assignedCar(nullptr), spotNumber(number) {}

    // Destructor to clean up dynamically allocated Car object
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

    // Method to assign a car to the spot if available
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

    // Method to remove the car from the spot
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

    // Method to display spot status
    void displayStatus() const override
    {
        cout << "Spot Number: " << spotNumber << ", Availability: " << (isAvailable ? "Yes" : "No") << endl;
    }
};

// Derived Class (Hierarchical Inheritance): ParkingLot
class ParkingLot : public ParkingBase
{
private:
    vector<ParkingSpot *> spots;
    static int totalCarAssigned;

public:
    // Constructor to create specified number of ParkingSpot objects
    ParkingLot(int totalSpots)
    {
        for (int i = 0; i < totalSpots; ++i)
        {
            spots.push_back(new ParkingSpot(i + 1));
        }
    }

    // Destructor to clean up dynamically allocated ParkingSpot objects
    ~ParkingLot()
    {
        for (auto &spot : spots)
        {
            delete spot;
        }
    }

    // Method to park a car in the first available spot
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

    // Method to remove a car from the lot based on license plate
    void removeCar(const string &licensePlate)
    {
        for (auto &spot : spots)
        {
            if (!spot->getAvailability() && spot->getAssignedCar()->licensePlate == licensePlate)
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

    // Method to display the number of available spots
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

    // Method to display all parked cars
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
                isParked = true;
            }
        }
        if (!isParked)
        {
            cout << "No cars are parked." << endl;
        }
    }

    // Static method to display the total number of assigned cars
    static void getTotalAssignedCars()
    {
        cout << "Total parked cars: " << totalCarAssigned << endl;
    }

    // Method to display the status of all parking spots
    void displayStatus() const override
    {
        cout << "ParkingLot Status:" << endl;
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
