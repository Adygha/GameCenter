/**************************************************************************************/
// File:	Console.cpp
// Summary:	Represents the console, and provides few methods that makes it easy to
//			achive the lab work functions.
// -------------------------------------------
// Author:	Janty Azmat
// -------------------------------------------
// Log:		
// -------------------------------------------
// Notes:	
/**************************************************************************************/

#include <iostream>
#include <iomanip>
#include "Console.h"

const unsigned BUF_LEN = 128; // Length of string buffer
const char *MSG_INVALID_INPUT = "Invalid input.. Try again..\n";

/**************************************************************************************/
// Displays a waiting message and waits for the user to press 'Enter' key. This method
// will ignore other input than 'Enter' key.
// -------------------------------------------
// Parameters:
//	- waitMessage:	the waiting message to bedisplayed.
/**************************************************************************************/
void Console::readEnter(const char *waitMessage) {
	std::cout << waitMessage;
	this->clearCin();
}

/**************************************************************************************/
// Displays a waiting message and waits for the user to enter an integer. This method
// will ignore other input than an integer. Additionally it can be asked to specify an
// accepted (inclusive) range.
// -------------------------------------------
// Parameters:
//	- waitMessage:	the waiting message to be displayed.
//	- rangeMin:		the accepted minimum range (inclusive).
//	- rangeMax:		the accepted maximum range (inclusive).
/**************************************************************************************/
int Console::readInteger(const char *waitMessage, int rangeMin, int rangeMax) {
	int outInt;
	bool tmpIs1st = true;

	do { // Loop until a valid input (integer between waitMessage and rangeMax inclusive)

		if (tmpIs1st) {
			tmpIs1st = false;
		} else {
			std::cout << MSG_INVALID_INPUT;
		}
		std::cout << waitMessage;

		if (!std::cin)
			this->clearCin();

	} while (!(std::cin >> outInt) || outInt < rangeMin || outInt > rangeMax);

	this->clearCin();

	return outInt;
}

/**************************************************************************************/
// Displays a waiting message and waits for the user to enter a double. This method
// will ignore other input than an double. Additionally it can be asked to specify an
// accepted (inclusive) range.
// -------------------------------------------
// Parameters:
//	- waitMessage:	the waiting message to be displayed.
//	- rangeMin:		the accepted minimum range (inclusive).
//	- rangeMax:		the accepted maximum range (inclusive).
/**************************************************************************************/
double Console::readDouble(const char *waitMessage, double rangeMin, double rangeMax) {
	double outDbl;
	bool tmpIs1st = true;

	do { // Loop until a valid input (double between waitMessage and rangeMax inclusive)

		if (tmpIs1st) {
			tmpIs1st = false;
		} else {
			std::cout << MSG_INVALID_INPUT;
		}
		std::cout << waitMessage;

		if (!std::cin)
			this->clearCin();

	} while (!(std::cin >> outDbl) || outDbl < rangeMin || outDbl > rangeMax);

	this->clearCin();

	return outDbl;
}

/**************************************************************************************/
// Displays a waiting message and waits for the user to enter a char. This method
// will ignore other input than an char. Additionally it can be asked to specify an
// accepted (inclusive) range.
// -------------------------------------------
// Parameters:
//	- waitMessage:	the waiting message to be displayed.
//	- rangeMin:		the accepted minimum range (inclusive).
//	- rangeMax:		the accepted maximum range (inclusive).
//	- toUpper:		'true' to get the return in upper case.
/**************************************************************************************/
char Console::readChar(const char *waitMessage, char rangeMin, char rangeMax, bool toUpper) {
	char outCh;
	bool tmpIs1st = true;

	if (toUpper) { // If the output needed in upper case the range also should be
		rangeMin = (char)toupper(rangeMin);
		rangeMax = (char)toupper(rangeMax);
	}

	do { // Loop until a valid input

		if (tmpIs1st) {
			tmpIs1st = false;
		} else {
			std::cout << MSG_INVALID_INPUT;
		}
		std::cout << waitMessage;

		if (!std::cin)
			this->clearCin();

		std::cin.get(outCh);
		this->clearCin();

		if (toUpper) // To upper case if needed
			outCh = (char)toupper(outCh);

	} while (outCh < rangeMin || outCh > rangeMax);

	return outCh;
}

/**************************************************************************************/
// Displays a waiting message and waits for the user to enter a line of text (a string
// with spaces). Additionally it gives the option to refuse an empty string.
// -------------------------------------------
// Parameters:
//	- waitMessage:	the waiting message to be displayed.
//	- rangeMin:		the accepted minimum range (inclusive).
//	- rangeMax:		the accepted maximum range (inclusive).
/**************************************************************************************/
const char *Console::readString(const char *waitMessage, bool acceptEmpty) {
	static char outStr[BUF_LEN]{};
	bool tmpIs1st = true;

	do { // Loop until not empty (if specified)

		if (tmpIs1st) {
			tmpIs1st = false;
		} else {
			std::cout << MSG_INVALID_INPUT;
		}
		std::cout << waitMessage;

		if (!std::cin)
			this->clearCin();

		std::cin.getline(outStr, BUF_LEN);
		std::cin.clear();
		std::cin.sync(); // Just in case (recommended by some sites with 'cin.get???()')
	} while (!(acceptEmpty || outStr[0])); // Loop only when empty and 'acceptEmpty == false'

	return outStr;
}

/**************************************************************************************/
// Displays a message to the user with an option to terminate with new line.
// -------------------------------------------
// Parameters:
//	- theMessage:		the message to be displayed.
//	- withEndLine:	'true' to terminate the message with new line.
/**************************************************************************************/
void Console::displayMessage(const char * theMessage, bool withEndLine) {
	std::cout << theMessage << (withEndLine ? "\n" : "");
}

/**************************************************************************************/
// Displays a number (integer) to the user with an option to terminate with new line.
// -------------------------------------------
// Parameters:
//	- theNum:			the integer to be displayed.
//	- withEndLine:	'true' to terminate the integer with new line.
/**************************************************************************************/
void Console::displayNumber(int theNum, bool withEndLine) {
	std::cout << theNum << (withEndLine ? "\n" : "");
}

/**************************************************************************************/
// Displays a number (double) to the user with an option to terminate with new line,
// and an option to specify the precision to display in.
// -------------------------------------------
// Parameters:
//	- theNum:			the double to be displayed.
//	- numPrecision:	the desired precision.
//	- withEndLine:	'true' to terminate the integer with new line.
/**************************************************************************************/
void Console::displayNumber(double theNum, int numPrecision, bool withEndLine) {
	std::cout << std::fixed << std::setprecision(numPrecision) <<
		theNum << (withEndLine ? "\n" : "");
}

///**************************************************************************************/
//// Displays an array of numbers (doubles) to the user in the form of a table,
//// providing options for the display
//// -------------------------------------------
//// Parameters:
////	- numArr:			the pointer to the double array to be displayed.
////	- arrLength:		the length of 'numArr' array.
////	- colCount:		the desired column count.
////	- colWidth:		the desired column width.
////	- numPrecision:	the desired precision.
///**************************************************************************************/
//void Console::displayNumbersInTable(const double * numArr, int arrLength, int colCount,
//	int colWidth, int numPrecision) {
//
//	for (int count = 0; count < arrLength; count++) {
//
//		if (count % colCount == 0) // Split the rows
//			std::cout << std::endl;
//
//		std::cout << std::fixed << std::setprecision(numPrecision) << std::setw(colWidth);
//		std::cout << numArr[count];
//	}
//	std::cout << std::endl;
//}

/**************************************************************************************/
// Clear the user page (the console in our case here). Supposed to work on Windows OS
// and other *NIX OSs.
/**************************************************************************************/
void Console::clearPage() {
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif // _WIN32
}

/**************************************************************************************/
// Clear flags and drop the rest of the console input stream.
/**************************************************************************************/
void Console::clearCin() {
	std::cin.clear();
	std::cin.ignore(INT_MAX, '\n');
}
