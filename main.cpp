#include <iostream>
#include <chrono>
#include <ctime>
#include <map>
#include <string>

using namespace std;
using namespace std::chrono;

class Car
{
private:
    string licensePlate;
    system_clock::time_point entryTime;
    int token;
    system_clock::time_point exitTime;

public:
    static int tokenNum;
    Car() : licensePlate(""), entryTime(system_clock::now()), token(tokenNum++), exitTime(system_clock::time_point()) {};
    Car(string licensePlate) : licensePlate(licensePlate), entryTime(system_clock::now()), token(tokenNum++), exitTime(system_clock::time_point()) {};
    void displayDetails() const
    {
        time_t entryTimeT = system_clock::to_time_t(entryTime);
        cout << "License Plate: " << licensePlate << endl;
        cout << "Entry Time: " << ctime(&entryTimeT);
    }
    void enterCar()
    {
        cout << "Car entered" << endl;
    }
    void exitCar()
    {
        exitTime = system_clock::now();
        time_t exitTimeT = system_clock::to_time_t(exitTime);
        cout << "Car with " << licensePlate << " exited at " << ctime(&exitTimeT) << endl;
    }
};

class ParkingLot
{
private:
    int availableSpots;
    int totalSpots;

public:
    ParkingLot(int totalSpots) : availableSpots(totalSpots), totalSpots(totalSpots) {};
    bool parkCar()
    {
        if (availableSpots > 0)
        {
            availableSpots--;
            return true;
        }
        else
        {
            cout << "No spots are available." << endl;
            return false;
        }
    }
    void removeCar()
    {
        if (availableSpots > totalSpots)
        {
            availableSpots++;
        }
    }
    void displayAvailableSpots()
    {
        cout << "Total Available Spots: " << availableSpots << endl;
    }
};
int Car::tokenNum = 0;
map<string, Car> CarsEntered;

void displayAllCars()
{
    if (CarsEntered.empty())
    {
        cout << "No cars entered yet." << endl;
        return;
    }

    cout << "Entered cars till now:" << endl;
    for (const auto &pair : CarsEntered)
    {
        pair.second.displayDetails();
        cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
    }
}

int main()
{
    ParkingLot parkingLot1(30);
    bool running = true;
    while (running)
    {
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
            Car enteredCar(licensePlate);
            CarsEntered[licensePlate] = enteredCar;
            parkingLot1.parkCar();

            cout << "Car added." << endl;
            break;
        }
        case 2:
            displayAllCars();
            break;
        case 3:
        {
            parkingLot1.displayAvailableSpots();
            break;
        }
        case 4:
        {
            string licensePl;
            cout << "Enter the Car License Plate number: ";
            getline(cin, licensePl);

            if (CarsEntered.find(licensePl) != CarsEntered.end())
            {
                CarsEntered[licensePl].exitCar();
                CarsEntered.erase(licensePl);
                parkingLot1.removeCar();
            }
            else
            {
                cout << "Car not found" << endl;
            }
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
