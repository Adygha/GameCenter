/**************************************************************************************/
// File:	Console.h
// Summary:	Represents the console, and provides few methods that makes it easy to
//			achive the lab work functions.
// -------------------------------------------
// Author:	Janty Azmat
// -------------------------------------------
// Log:		
// -------------------------------------------
// Notes:	- Most of the comments are in the 'Console.cpp' file.
/**************************************************************************************/

#ifndef CONSOLE_TOKEN
#define CONSOLE_TOKEN

#include <climits>
#include <cfloat>

class Console {
public:
	void readEnter(const char *waitMessage);
	int readInteger(const char *waitMessage, int rangeMin = INT_MIN, int rangeMax = INT_MAX);
	double readDouble(const char *waitMessage, double rangeMin = -DBL_MAX, double rangeMax = DBL_MAX);
	char readChar(const char *waitMessage, char rangeMin = 1, char rangeMax = CHAR_MAX, bool toUpper = false);
	const char *readString(const char *waitMessage, bool acceptEmpty = false);
	void displayMessage(const char * theMessage, bool withEndLine = false);
	void displayNumber(int theNum, bool withEndLine = false);
	void displayNumber(double theNum, int numPrecision = 2, bool withEndLine = false);
	//void displayNumbersInTable(const double * numArr, int arrLength, int colCount = 6,
	//	int colWidth = 8, int numPrecision = 2); // Not needed here (only in 'Step1_1C')
	void clearPage();

private:
	void clearCin();
};

#endif // !CONSOLE_TOKEN