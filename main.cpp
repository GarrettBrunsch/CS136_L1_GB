// Garrett, Brunsch
// Lab #1 Files - Refresher S25
// CS_136_14728SP25P
// Due 2/23/25

// The aim of this program is to modularized, menu-driven program to read an unknown number of car records from a file, 
// validate the records, and store the valid records in an array of structs/objects.

#include <iostream>
#include <string>
#include <cctype>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <limits>
using namespace std;

enum MenuChoice { PRINT_UNSORTED_INVENTORY = 1, PRINT_INVALID_RECORDS, QUIT };

void displayMenu();
void clearInputError();

class CarRecord
{
private:
    static const int MAX_RECORDS = 25;
    static CarRecord inventory[MAX_RECORDS];
    static int recordCount;
    static const string INPUT_FILENAME;
    static const string ERROR_FILENAME;

    string carId = "Unknown";
    string carModel = "Unknown";
    string carManufacturer = "Unknown";
    int carQuantity = 0;
    double carPrice = 0.0;

    bool validationStatus = true;
    string errorMessage = "";

    static const int MAX_ID_LENGTH = 8;
    static const int MIN_MODEL_LENGTH = 3;
    static const double MIN_PRICE; // C++ does not allow double or floats to be directly initialized inside a class so it is a global const instead

    void validateRecord();
    bool isValidId(const string& carId, string& errorMessage);
    bool isValidCarModel(const string& carModel, string& errorMessage);
    bool isValidCarPrice(const double& carPrice, string& errorMessage);
    bool isValidQuantity(const int& carQuantity, string& errorMessage);

public:
    CarRecord();
    CarRecord(const string& carId, const string& carModel, const string& carManufacturer, int carQuantity, double carPrice);

    string toString() const;

    static void printHeader(bool includeErrorColumn = false);
    static bool readInventory();
    static void printInventory();
    static void printErrorFile();

};

int main()
{
    int userChoice;

    CarRecord::readInventory();

    do
    {
        displayMenu();
        cin >> userChoice;

        switch (userChoice)
        {
        case PRINT_UNSORTED_INVENTORY:
            CarRecord::printInventory();
            break;
        case PRINT_INVALID_RECORDS:
            CarRecord::printErrorFile();
            break;
        case QUIT:
            cout << "Now exiting program...\n\n";
            break;
        default:
            clearInputError();
            break;
        }
    } while (userChoice != QUIT);

    return 0;
}

void displayMenu()
{
    cout << "\n\n --- Main Menu ---\n"
        "1. Print Unsorted Inventory\n"
        "2. Print Invalid Records\n"
        "3. Quit\n"
        "Enter choice: ";
}

void clearInputError()
{
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "Invalid entry. Please double check the input and try again\n";
}

CarRecord CarRecord::inventory[MAX_RECORDS];
int CarRecord::recordCount = 0;
const string CarRecord::INPUT_FILENAME = "inventory.txt";
const string CarRecord::ERROR_FILENAME = "errors.txt";
const double CarRecord::MIN_PRICE = 12999.00;

CarRecord::CarRecord() : carId(""), carModel(""), carManufacturer(""), carQuantity(0), carPrice(0.0) {}
CarRecord::CarRecord(const string& id, const string& model, const string& manufacturer, int quantity, double price)
    :carId(id), carModel(model), carManufacturer(manufacturer), carQuantity(quantity), carPrice(price) {}

string CarRecord::toString() const
{
    //All numerical values must be right aligned to adhere to assignment instructions
    stringstream ss;
    ss << left << setw(10) << carId
        << left << setw(20) << carModel
        << left << setw(20) << carManufacturer
        << right << setw(8) << carQuantity
        << right << setw(12) << fixed << setprecision(2) << carPrice;
    return ss.str();
}

void CarRecord::printHeader(bool includeErrorColumn)
{
    cout << left << setw(10) << "Car ID"
        << left << setw(20) << "Model"
        << left << setw(20) << "Manufacturer"
        << right << setw(8) << "Qty"
        << right << setw(12) << "Price";

    if (includeErrorColumn)
    {
        cout << left << setw(30) << "   Error Message" // Three spaces added here to compensate for " - " format in table rows
            << "\n" << string(100, '-') << "\n";
    }
    else
    {
        cout << "\n" << string(70, '-') << "\n";
    }

}

bool CarRecord::readInventory()
{
    bool success = true;
    ifstream inFile(INPUT_FILENAME);
    ofstream errorFile(ERROR_FILENAME);
    string tempId, tempModel, tempManufacturer;
    int tempQuantity;
    double tempPrice;

    if (!inFile)
    {
        cout << "Error opening " << INPUT_FILENAME <<
            "\nDouble check the file exists in the proper directory and try again\n"
            "Program now exiting..." << endl;
        exit(EXIT_FAILURE); //Exit_failure preferred in instructions 
    }
    if (!errorFile)
    {
        inFile.close();
        cout << "Error opening " << ERROR_FILENAME <<
            "\nDouble check the file exists in the proper directory and try again\n"
            "Program now exiting..." << endl;
        exit(EXIT_FAILURE);
    }
    if (inFile.peek() == EOF)
    {
        cout << "Input file is empty\n";
        success = false;
    }
    else
    {
        recordCount = 0;
        while (success && recordCount < MAX_RECORDS && inFile >> tempId >> tempModel >> tempManufacturer >> tempQuantity >> tempPrice)
        {
            CarRecord temp(tempId, tempModel, tempManufacturer, tempQuantity, tempPrice);
            temp.validateRecord();

            if (temp.validationStatus)
            {
                inventory[recordCount] = temp;
                recordCount++;
            }
            else
            {
                errorFile << temp.toString() << " - " << temp.errorMessage << "\n";
            }
        }
    }

    if (recordCount >= MAX_RECORDS && inFile >> tempId)
    {
        cout << "Max record limit of " << MAX_RECORDS << " has been reached\n";
    }

    inFile.close();
    errorFile.close();

    return success;
}

void CarRecord::printInventory()
{
    cout << "\nCurrenent Inventory:\n";
    printHeader();

    for (int i = 0; i < recordCount; i++)
    {
        cout << inventory[i].toString() << "\n";
    }
}

void CarRecord::printErrorFile()
{
    bool hasErrors = false;
    ifstream errorFile(ERROR_FILENAME);
    string line;

    cout << "\nInvalid Records:\n";
    printHeader(true);

    while (getline(errorFile, line))
    {
        cout << line << "\n";
        hasErrors = true;
    }

    if (!hasErrors)
    {
        cout << "No invalid records found\n";
    }
}

void CarRecord::validateRecord()
{
    validationStatus = true;
    errorMessage.clear();

    isValidId(carId, errorMessage);
    isValidCarModel(carModel, errorMessage);
    isValidCarPrice(carPrice, errorMessage);
    isValidQuantity(carQuantity, errorMessage);
}

bool CarRecord::isValidId(const string& carId, string& errorMessage)
{
    if (carId.length() != MAX_ID_LENGTH)
    {
        errorMessage += "ID must be 8 characters; ";
        validationStatus = false;
    }

    for (int i = 0; i < (MAX_ID_LENGTH / 2); i++)
    {
        if (!isalpha(carId[i]) || carId[i] == 'O' || carId[i] == 'o')
        {
            errorMessage += "First 4 characters must be alpha only (excluding O); ";
            validationStatus = false;
        }
    }

    for (int i = (MAX_ID_LENGTH / 2); i < MAX_ID_LENGTH; i++)
    {
        if (carId[i] < '1' || carId[i] > '9')
        {
            errorMessage += "Second set of 4 characters must be 1-9; ";
            validationStatus = false;
        }
    }
    return validationStatus;
}

bool CarRecord::isValidCarModel(const string& carModel, string& errorMessage)
{
    if (carModel.length() < MIN_MODEL_LENGTH)
    {
        errorMessage += "Model contains less than minimum required characters; ";
        validationStatus = false;
    }
    if (!isalpha(carModel[0]))
    {
        errorMessage += "Model does not start with an alpha character; ";
        validationStatus = false;
    }
    for (int i = 0; i < carModel.length(); i++)
    {
        if (!isalnum(carModel[i]) && carModel[i] != '_')
        {
            errorMessage += "Model contains non-alphanumeric character; ";
            validationStatus = false;
        }
    }
    return validationStatus;
}

bool CarRecord::isValidCarPrice(const double& carPrice, string& errorMessage)
{
    if (carPrice < MIN_PRICE)
    {
        errorMessage += "Price must be above minimum price; ";
        validationStatus = false;
    }
    return validationStatus;
}
bool CarRecord::isValidQuantity(const int& carQuantity, string& errorMessage)
{
    if (carQuantity < 0)
    {
        errorMessage += "Quantity must be at least 0; ";
        validationStatus = false;
    }
    return validationStatus;
}

/*
                                            PROGRAM RUN 1 (Missing input file)

Error opening inventory.txt
Double check the file exists in the proper directory and try again
Program now exiting...

                                            PROGRAM RUN 2 (All valid entries/no invalid entries)


 --- Main Menu ---
1. Print Unsorted Inventory
2. Print Invalid Records
3. Quit
Enter choice: 1

Currenent Inventory:
Car ID    Model               Manufacturer             Qty       Price
----------------------------------------------------------------------
ABCD1234  Camry               Toyota                     5    13000.00
WXYZ5678  Civic               Honda                      3    15000.00


 --- Main Menu ---
1. Print Unsorted Inventory
2. Print Invalid Records
3. Quit
Enter choice: 2

Invalid Records:
Car ID    Model               Manufacturer             Qty       Price   Error Message
----------------------------------------------------------------------------------------------------
No invalid records found


 --- Main Menu ---
1. Print Unsorted Inventory
2. Print Invalid Records
3. Quit
Enter choice: 3
Now exiting program...

                                            PROGRAM RUN 3 (Mix of valid entries and all invalid bounds checks from inventory.txt)
 --- Main Menu ---
1. Print Unsorted Inventory
2. Print Invalid Records
3. Quit
Enter choice: 1

Currenent Inventory:
Car ID    Model               Manufacturer             Qty       Price
----------------------------------------------------------------------
ABCD1234  Model_A             Manufacturer_A             5    13000.00
MPKZ1265  Tucker48            Tucker_Corp               51  1663000.00
BPAZ3478  RX5                 Lexus                     10    45999.99


 --- Main Menu ---
1. Print Unsorted Inventory
2. Print Invalid Records
3. Quit
Enter choice: 2

Invalid Records:
Car ID    Model               Manufacturer             Qty       Price   Error Message
----------------------------------------------------------------------------------------------------
XYZO1234  Camry               Toyota                     3    15000.00 - First 4 characters must be alpha only (excluding O);
ABCD0234  Civic               Honda                      5    14000.00 - Second set of 4 characters must be 1-9;
ABC1234   Accord              Honda                      2    13500.00 - ID must be 8 characters; First 4 characters must be alpha only (excluding O); Second set of 4 characters must be 1-9;
ABCDEF12  M                   BMW                        1    15000.00 - Second set of 4 characters must be 1-9; Second set of 4 characters must be 1-9; Model contains less than minimum required characters;
WXYZ1234  3Series             BMW                        4    16000.00 - Model does not start with an alpha character;
DEFG1234  Corolla             Toyota                    -1    20000.00 - Quantity must be at least 0;
HIJK1234  Altima              Nissan                     5    12998.00 - Price must be above minimum price;


 --- Main Menu ---
1. Print Unsorted Inventory
2. Print Invalid Records
3. Quit
Enter choice: g
Invalid entry. Please double check the input and try again


 --- Main Menu ---
1. Print Unsorted Inventory
2. Print Invalid Records
3. Quit
Enter choice: 3
Now exiting program...
*/
