/**************************************************************************************/
// File:	Menu.h
// Summary:	Represents a menu that can be changed according to choice.
// -------------------------------------------
// Author:	Janty Azmat
// -------------------------------------------
// Log:		
// -------------------------------------------
// Notes:	- Most of the comments are in the 'Console.cpp' file
/**************************************************************************************/

#ifndef MENU_TOKEN
#define MENU_TOKEN

class Console; // Forward declaration

class Menu {
public:
	Menu(Console *theCon);
	~Menu();

	void assignChoices(const char *menuTitle, char const *const *choicesArr, int choicesCount);
	int select(bool isClearPage = true); // I would call it 'displayChoices' but lab instructions wants 'select'

private:
	char *_menuTitle; // The menu title
	char **_choiceArr; // The array of choices to be displayed
	int _choicCount; // The number of choices
	Console *_theCon; // A pointer to 'Console' object to display to the console

	static const char *_MSG_ERR_NO_CHOICES; // A no-choices error message

	void deleteChoices(); // Deletes the choices array
};

#endif // !MENU_TOKEN