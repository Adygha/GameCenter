/**************************************************************************************/
// File:	Menu.cpp
// Summary:	Represents a menu that can be changed according to choice.
// -------------------------------------------
// Author:	Janty Azmat
// -------------------------------------------
// Log:		
// -------------------------------------------
// Notes:	
/**************************************************************************************/

#include <string.h>
#include "Console.h"
#include "Menu.h"
#include <stdexcept>

const unsigned BUF_LEN = 512u; // For a general purpose c-strings lengths

const char *Menu::_MSG_ERR_NO_CHOICES = "Please provide choices before displaying them.";

/**************************************************************************************/
// A constructor with a 'Console' pointer as parameter.
// -------------------------------------------
// Parameters:
//	- theCon:			a pointer to the 'Console' object used as user interface.
/**************************************************************************************/
Menu::Menu(Console * theCon) : _theCon(theCon), _menuTitle(new char[BUF_LEN]),
												_choiceArr(NULL), _choicCount(0) {
}

Menu::~Menu() {
	this->deleteChoices(); // Delete the choices array
	delete[] this->_menuTitle;
	this->_menuTitle = NULL;
	// Will not delete '_theCon' because it is provided to this object from
	// outside user, and it might be needed outside this object (plus, the
	// user of this object is responsible for deleting the Console object
	// if it was dynamically created -if it was already dynamically accocated-)
}

void Menu::assignChoices(const char *menuTitle, char const *const *choicesArr, int choicesCount) {

	if (this->_choicCount)
		this->deleteChoices(); // Delete previous choices

	this->_choicCount = choicesCount;
	this->_choiceArr = new char *[choicesCount]; // Start all-over with new choices-array

	for (int count = 0; count < choicesCount; count++) { // Initialize every choice
		this->_choiceArr[count] = new char[BUF_LEN] {};
#ifdef _WIN32
		strcpy_s(this->_choiceArr[count], BUF_LEN, choicesArr[count]);
#else
		strcpy(this->_choiceArr[count], choicesArr[count]);
#endif // _WIN32
	}

#ifdef _WIN32
	strcpy_s(this->_menuTitle, BUF_LEN, menuTitle);
#else
	strcpy(this->_menuTitle, menuTitle);
#endif // _WIN32
}

/**************************************************************************************/
// Displays the menu title and choices, and asks the user to enter a proper choice.
// -------------------------------------------
// Return:	returns an acceptable integer choice entered by the user between the
//			available choices this object has.
/**************************************************************************************/
int Menu::select(bool isClearPage) {

	if (!this->_choicCount) // Check if choices already assigned
		throw std::runtime_error(Menu::_MSG_ERR_NO_CHOICES);

	if (isClearPage) // Clear the page if requested
		this->_theCon->clearPage();

	this->_theCon->displayMessage(this->_menuTitle, true);
	this->_theCon->displayMessage("\nMENU\n----\n", true);

	for (int count = 0; count < this->_choicCount; count++) { // Display choices
		this->_theCon->displayNumber(count + 1);
		this->_theCon->displayMessage(". ");
		this->_theCon->displayMessage(this->_choiceArr[count], true);
	}

	return this->_theCon->readInteger(
		"\nEnter a choice number from the list and press ENTER: ", 1, this->_choicCount);
}

/**************************************************************************************/
// A private method that deletes the choices array (made to avoid code duplication).
/**************************************************************************************/
void Menu::deleteChoices() {

	for (int count = 0; count < this->_choicCount; count++) { // Delete every choice
		delete[] this->_choiceArr[count];
		this->_choiceArr[count] = NULL; // Maybe unnecessary extra here??
	}

	delete[] this->_choiceArr; // Delete the array of choices itself
	this->_choiceArr = NULL;
	this->_choicCount = 0; // Reset the counter
}
