/*
Price Calculator.cpp
Author: Matt Presley
Created: 27 Oct 2011
Last Revised: 5 June 2012

Purpose: Calculates item prices for Tractor Parts Express.

Version history:
1.0001  - original build
1.0002  - eliminated all price overlap when moving between tiers,
        - program version is now displayed to user
        - now checks for prices ending in 0.99 and subtracts 1
1.0003  - console now clears when beginning new calculation
        - cleaned up code using function calls
1.0004  - resized/moved application window
1.0005  - in weight calculation, used ceil() function to round up to next pound
        - cleaned up comments
1.06    - updated price calculations per Steve on 18 Jan 2012
1.07    - reduced size of title display and window
2.00    - Added menu to allow user to see algorithm and patch notes
2.01    - Updated pricing model: From $4.00 to $82.50, add 50% to wholesale price
2.02    - Program now asks user to identify vendor used for price calculation
2.03    - Resized window and shortened title
        - Increased prices by 10% per Steve on 05 June 2012
        - For wholesale prices greater than or equal to $100.00, use "Engine Kit" price model
        - Removed "secondary price model" for non-TISCO parts, as we use TISCO almost exclusively now
*/


#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <math.h>
#define _WIN32_WINNT 0x0500
#include <windows.h>
using namespace std;

//*******************************
//***** FUNCTION PROTOTYPES *****
//*******************************
void setWindowSize();
void displayTitle();
int mainMenu();
int subMenu();
void calculatePrices();
int displayAlgorithm();
int displayPatchNotes();
float getPrices();
float getWeights();
int getVendor();
float tiscoPriceModel(float);
float otherPriceModel(float);
float checkPrices(float, float);
void displayOurPrice(float);


int main()
{
    setWindowSize();
    int choiceMain = 0;
    int choiceSub = 0;

    // display menu to user
    choiceMain = mainMenu();
    while(choiceMain != 1)
    {
        if(choiceMain == 2)
            choiceSub = displayAlgorithm();
        if(choiceMain == 3)
            choiceSub = displayPatchNotes();
        if(choiceMain == 4)
            return 0;

        if(choiceSub == 1)
            choiceMain = mainMenu();
        if(choiceSub == 4)
            return 0;
    }

    calculatePrices();

    system("PAUSE");
    return 0;
}


//************************************
//*                                  *
//*     MENU / UTILITY FUNCTIONS     *
//*                                  *
//************************************


// setWindowSize()
// sets the application window size and location on screen
void setWindowSize()
{
    HWND console = GetConsoleWindow();
    RECT r;
    GetWindowRect(console, &r);
    //MoveWindow(window_handle, x, y, width, height, redraw_window);
    MoveWindow(console, 0, 685, 240, 300, TRUE);
}


// displayTitle()
// displays title and program version to screen
void displayTitle()
{
    system("cls");
    cout << fixed << setprecision(2);
    cout << "**************************" << endl;
    cout << "     Price Calculator     " << endl;
    cout << "       version 2.03       " << endl;
    cout << "**************************" << endl;
    cout << endl;
}


// mainMenu()
// Gives user four options:
//  1) Begin using sort function
//  2) View algorithm
//  3) View patch notes
//  4) Exit program
//  Returns a menu choice to parent function
int mainMenu()
{
    setWindowSize();
    displayTitle();

    int choice;
    cout << "MAIN MENU:" << endl;
    cout << "----------" << endl;
    cout << "1) Perform calculations." << endl;
    cout << "2) View the algorithm." << endl;
    cout << "3) View version history." << endl;
    cout << "4) Exit the program." << endl;
    cout << endl << "Menu choice: ";
    cin >> choice;
    // Input validation. If not 1, 2, or 3, display error and repeat.
    while(choice != 1 && choice != 2 && choice != 3 && choice != 4)
    {
        cout << endl << "Invalid choice. Please enter 1, 2, 3, or 4: ";
        cin >> choice;
    }
    return choice;
}


// subMenu()
// Gives user two options:
// 1) Return to main menu
// 2) Exit program
// Returns a menu choice to parent function
int subMenu()
{
    int choice;
    cout << endl << endl << endl << "MENU OPTIONS:" << endl;
    cout << "-------------" << endl;
    cout << "1) Return to the main menu." << endl;
    cout << "4) Exit the program." << endl;
    cout << endl << "Menu choice: ";
    cin >> choice;
    // Input validation. If not 1 or 4, display error and repeat.
    while(choice != 1 && choice != 4)
    {
        cout << endl << "Invalid choice. Please enter 1 or 4: ";
        cin >> choice;
    }
    return choice;
}


//*********************************
//*                               *
//*     CALCULATION FUNCTIONS     *
//*                               *
//*********************************


// calculatePrices()
// Gets wholesale price and weight using getPrices() and getWeights()
// Calculates prices using tiscoPrices(), tiscoPrices(), and checkPrices()
// Displays output using displayOurPrice()
void calculatePrices()
{
    int repeat = 0;
    while(repeat == 0) // repeat calculator program until user closes
    {
        float rate = 0.0;           // rate to use in calculation
        float wholesalePrice = 0.0; // wholesale price, input by user
        float weight = 0.0;         // weight of item, input by user
        float ourPrice = 0.0;       // final price

        // when updating overall price change, multiply by the coefficient below
        const float CHANGE_ALL_PRICES = 1.10;   // raise prices by 10% on 05 June 2012

        //*********************
        //***** GET INPUT *****
        //*********************

        displayTitle();
        wholesalePrice = getPrices();
        weight = getWeights();


        //********************************
        //***** PERFORM CALCULATIONS *****
        //********************************

        // rounds up weight to the nearest whole number if 5 pounds or more
        if(weight >= 5.0)  //add weight into calculation if >= 5.0 lb.
            weight = ceil(weight);   // rounds up to next whole number
        else
            weight = 0; // if less than 5 pounds, ignore weight in calculation

        // if wholesale price is less than $100, add 65 cents per pound after coefficient
        if(wholesalePrice <= 100.00)
        {
            weight = weight * 0.65;  // add $0.65/pound (after multiplying wholesale by its coefficient)
            ourPrice = tiscoPriceModel(wholesalePrice);     // multiply wholesale by a coefficient
            if(wholesalePrice >= 4.00)   // update price as needed for items using coefficients
                ourPrice = ourPrice * CHANGE_ALL_PRICES;
            ourPrice = static_cast<int>(ourPrice + weight) + 0.99; // final price
        }

        // if wholesale price is $100 or more, multiply weight by 0.70 and add before coefficient
        else
        {
            weight = weight * 0.70; // add $0.70/pound (before multiplying wholesale by its coefficient)
            ourPrice = (wholesalePrice + weight) * 1.23;
            if(wholesalePrice >= 4.00)   // update price as needed for items using coefficients
                ourPrice = ourPrice * CHANGE_ALL_PRICES;
            ourPrice = static_cast<int>(ourPrice) + 0.99;
        }

        // if the calculated price ends in "0.99", subtract $1
        ourPrice = checkPrices(ourPrice, wholesalePrice);


        // ****************************************
        // ***** DISCPLAY FINAL PRICE TO USER *****
        // ****************************************

        displayOurPrice(ourPrice);
        system("PAUSE");
        system("cls");
    }
}


// getPrices()
// asks user to input wholesale price
// if something other than a number is entered, asks for input again
// returns wholesale price to parent function
float getPrices()
{
    float getPrice = 0.0;
    cout << "Wholesale price: $";
    cin >> getPrice;
    while(!cin || getPrice == 0.0) //input validation for price
    {
        getPrice = 0.0;
        cout << "Invalid input." << endl;
        cout << "Value must be $0.01 or greater." << endl << endl;
        cin.clear();
        cout << "Wholesale price: $";
        cin >> getPrice;
    }
    return getPrice;
}


// getWeights()
// asks user to input the item weight
// if something other than a number is entered, asks for input again
// returns item weight to parent function
float getWeights()
{
    float getWeight = 0.0;
    cout << "Item weight: ";
    cin >> getWeight;
    while(!cin || getWeight < 0.0) //input validation for weight
    {
        getWeight = 0.0;
        cout << "Invalid input." << endl;
        cout << "Value must be 0 or greater." << endl << endl;
        cin.clear();
        cout << "Item weight: ";
        cin >> getWeight;
    }
    return getWeight;
}


// tiscoPriceModel()
// Calculates the price of items purcharsed from Tisco
// Accepts the wholesale price from parent function
// Either uses a static price or multiplies by a coefficient based on wholesale
// Returns the calculated price to parent function
float tiscoPriceModel(float tiscoPrice)
{
    float calcPrice = 0.0;

    // Between $0.01 and $3.99 wholesale, use a static price
    if(tiscoPrice > 0.00 && tiscoPrice < 0.50)
        calcPrice = 1.99;
    else if(tiscoPrice < 1.00)
        calcPrice = 2.99;
    else if(tiscoPrice < 2.00)
        calcPrice = 4.99;
    else if(tiscoPrice < 3.00)
        calcPrice = 6.99;
    else if(tiscoPrice < 4.00)
        calcPrice = 8.99;

    // For $4.00 wholesale and up, multiply by a coefficient
    else if(tiscoPrice <= 82.50)
    {
        calcPrice = tiscoPrice * 1.50;

        // For lowest values, add a static value to smooth transition
        if(tiscoPrice < 5.46)     // from 4.00 to 5.45, add $1.40
            calcPrice = calcPrice + 1.40;
    }
    else if(tiscoPrice <= 85.00)
        calcPrice = tiscoPrice * 1.49;
    else if(tiscoPrice <= 87.50)
        calcPrice = tiscoPrice * 1.48;
    else if(tiscoPrice <= 90.00)
        calcPrice = tiscoPrice * 1.46;
    else if(tiscoPrice <= 92.50)
        calcPrice = tiscoPrice * 1.45;
    else if(tiscoPrice <= 95.00)
        calcPrice = tiscoPrice * 1.43;
    else if(tiscoPrice <= 97.50)
        calcPrice = tiscoPrice * 1.42;
    else if(tiscoPrice <= 100.00)
        calcPrice = tiscoPrice * 1.41;

    return calcPrice;
}


// checkPrices()
// accepts final price and wholesale price from main()
// if the price ends with 0.99 ($10.99, $20.99, etc.), subtract 1.00 from
//   the final price to appeal more to customer
// returns final price to main()
float checkPrices(float returnPrice, float wholesale)
{
    int checkPrice = 0;     //checks for prices ending in 0.99
    int checkLastDigit = 0; //checks for prices ending in 0.99

    checkPrice =  static_cast<int>(returnPrice);
    checkLastDigit = static_cast<int>(returnPrice) % 10;
    if(wholesale > 0 && checkLastDigit == 0)    //if price ends in 0.99, subtract 1.00
        returnPrice--;

    return returnPrice;
}


// displayOurPrice()
// accepts the final price from main()
// displays final price to user
void displayOurPrice(float usePrice)
{
    cout << endl;
    cout << fixed << setprecision(2);
    cout << "Our price: $" << usePrice;
    cout << endl << endl << endl;
}


// displayAlgorithm()
// Increases window size and shows basic algorithm used to calculate prices
// Returns a menu choice to parent function
int displayAlgorithm()
{
    //enlarge window
    HWND console = GetConsoleWindow();
    RECT r;
    GetWindowRect(console, &r);
    //MoveWindow(window_handle, x, y, width, height, redraw_window);
    MoveWindow(console, 0, 0, 1000, 900, TRUE);

    displayTitle();

    cout << "PRICE CALCULATOR ALGORITHM:" << endl;
    cout << "---------------------------" << endl;
    cout << "1) Get wholesale price and weight from the user" << endl << endl;
    cout << "2) Determine under which range the wholesale price falls" << endl << endl;
    cout << "3) Multiply by a coefficient and add weight based on wholesale price:" << endl << endl;

    cout << "If the wholesale price is $3.99 or less, final price is a static value:" << endl;
    cout << " 0.01 - 0.49 -> sell for 1.99" << endl;
    cout << " 0.50 - 0.99 -> sell for 2.99" << endl;
    cout << " 1.00 - 1.99 -> sell for 4.99" << endl;
    cout << " 2.00 - 2.99 -> sell for 6.99" << endl;
    cout << " 3.00 - 3.99 -> sell for 8.99" << endl << endl;

    cout << "If the wholesale price is between $4.00 and $100.00, increase it by a" << endl;
    cout << "percentage, then add 65 cents per pound (if 5 pounds or greater):" << endl;
    cout << " 4.00 - 82.50 -> add 50%" << endl;
    cout << " 82.51 - 85.00 -> add 49%" << endl;
    cout << " 85.01 - 87.50 -> add 48%" << endl;
    cout << " 87.51 - 90.00 -> add 46%" << endl;
    cout << " 90.01 - 92.50 -> add 45%" << endl;
    cout << " 92.51 - 95.00 -> add 43%" << endl;
    cout << " 95.01 - 97.50 -> add 42%" << endl;
    cout << " 97.51 - 100.00 -> add 41%" << endl;
    cout << " To smooth over the transition from static to percentage values," << endl;
    cout << " if wholesale is between $4.00 and $5.45, add $1.40 after calculation" << endl << endl;

    cout << "If the wholesale price is greater than $100.00, add 70 cents per pound." << endl;
    cout << "Then, increase the total price (wholesale + weight) by 23%" << endl << endl;

    cout << "4) For any price that started with a wholesale value of $4.00 or more, " << endl;
    cout << "   raise the total price by 10% (price increase per Steve on 5 June 2012)" << endl << endl;

    cout << "5) \"Round up\" the price to the next 99 cents" << endl << endl;

    cout << "6) If the final price ends in \"0.99\" (such as 20.99 or 30.99), subtract $1.00" << endl;

    int choice = subMenu();
    return choice;
}


// displayPatchNotes()
// Increases window size and displays version history to user
// Returns a menu choice to parent function
int displayPatchNotes()
{
    //enlarge window
    HWND console = GetConsoleWindow();
    RECT r;
    GetWindowRect(console, &r);
    //MoveWindow(window_handle, x, y, width, height, redraw_window);
    MoveWindow(console, 0, 0, 1000, 900, TRUE);

    displayTitle();
    cout << "VERSION HISTORY:" << endl;
    cout << "----------------" << endl;
    cout << "Version 1.0001 " << endl;
    cout << "- Release date: 01 Nov 2011" << endl;
    cout << "- Original build. Created to allow Tractor Parts Express employees to more" << endl;
    cout << "  quickly calculate retail prices based on wholesale price and weight of items." << endl << endl;

    cout << "Version 1.0002" << endl;
    cout << "- Release date: 03 Nov 2011" << endl;
    cout << "- Eliminated all price overlap when moving between tiers" << endl;
    cout << "- Version of program is now displayed to user" << endl;
    cout << "- Now checks for prices ending in \"0.99\" and subtracts $1.00" << endl << endl;

    cout << "Version 1.0003 " << endl;
    cout << "- Release date: 17 Nov 2011" << endl;
    cout << "- Console now clears when beginning new calculation" << endl;
    cout << "- Cleaned up code using function calls" << endl << endl;

    cout << "Version 1.0004" << endl;
    cout << "- Release date: 17 Nov 2011" << endl;
    cout << "- resized/moved application window for ease of use" << endl << endl;

    cout << "Version 1.0005" << endl;
    cout << "- Release date: 22 Nov" << endl;
    cout << "- In weight calculation, used ceil() function to round up to next pound" << endl << endl;

    cout << "Version 1.06" << endl;
    cout << "- Release date: 18 Jan 2012" << endl;
    cout << "- Updated pricing algorithm: smaller price brackets and lowered weight cost" << endl << endl;

    cout << "Version 1.07" << endl;
    cout << "- Release date: 20 Jan 2012" << endl;
    cout << "- Reduced size of window and title display for ease of use" << endl << endl;

    cout << "Version 2.00" << endl;
    cout << "- Release date: 06 Feb 2012" << endl;
    cout << "- Added menu to allow user to view pricing algorithm and patch notes" << endl << endl;

    cout << "Version 2.01" << endl;
    cout << "Release date: 17 Feb 2012" << endl;
    cout << "- Updated pricing model: from $4.00 to $82.50 wholesale, add 50% to price" << endl << endl;

    cout << "Version 2.02" << endl;
    cout << "- Release date: 20 Feb 2012" << endl;
    cout << "- Program now asks user to identify vendor used for price calculation," << endl;
    cout << "  and uses a different price model for vendors other than Tisco" << endl << endl;

    cout << "Version 2.03" << endl;
    cout << "- Resized window and shortened title" << endl;
    cout << "- Release date: 05 June 2012" << endl;
    cout << "- Increased prices by 10% per Steve on 05 June 2012" << endl;
    cout << "- For wholesale prices of $100.00 or more, use \"Engine Kit\" price model" << endl;
    cout << "- Removed \"secondary price model\" for non-TISCO parts, " << endl;
    cout << "  as we now use TISCO almost exclusively" << endl;

    cout << endl;
    int choice = subMenu();
    return choice;
}
