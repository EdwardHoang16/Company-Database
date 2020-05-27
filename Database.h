/*
Edward Hoang
The question this code responds to is one asking for a company database storing
the ID, first name, last name, job, salary, and email address of all employees.
Access to the database requires logging in, and the information in the files themselves
is encrypted for security purposes.
Declares a class representing a database. The role of this class is to store all the
functionality of the database, including adding employees, searching for employees, showing
all employees' information at once, deleting an employee, sending an email, adding a user, deleting
a user, signing off, and exiting the program.
*/

#pragma once

#include <string>
#include <vector>

class Database {
private:
	const int WIDTH = 25;

	int input;
	bool keepGoing;
	bool searchKeepGoing;

	int id;
	std::string firstName;
	std::string lastName;
	std::string job;
	float salary;
	std::string emailAddress;

public:
	Database();
	~Database();

	bool promptForLogin();
	std::string maskPassword();
	long long encrypt(int character);
	int decrypt(long long character);
	int checkFromFile(std::string attempt, std::string fileName);
	int checkFromFilePassword(std::string passwordAttempt, int usernameID, std::string fileName);

	int getInput();
	void readInput();
	void mainMenuLoop();

	std::string replaceChar(std::string job, char charBeingReplaced, char newChar);
	void buildCellInfo(int entryID, std::string fileName);
	void createDatabase();
	void clearDatabase();
	void showAllEmployees();

	int getSearchInput();
	void readSearchInput();
	void searchByID();
	void searchByFirstName();
	void searchByLastName();
	void searchByJob();
	void searchBySalary();
	void searchByEmailAddress();

	void saveToFile(std::string input, std::string fileName, int id);
	bool fileExists(std::string fileName);
	bool isEmpty(std::string fileName);
	int getLastID(std::string fileName);
	void addEmployeeDetails();
	void addEmployee();

	int getEditInput();
	void readEditInput(int idBeingSought);
	//void editEmployeeDetails(int idBeingSought);
	void editEmployee(int idBeingSought);
	void searchForEmployeeToEdit();

	std::string generateTempFileName();
	void copyFromTempFile(std::string inputFileName, std::string tempFileName);
	void findLinesToDelete(int matchID, std::string fileName);
	void deleteCell(std::string fileName, int currentLinesCount, int totalLinesCount);
	void deleteEmployee();

	void addUser();

	int getUserEditInput();
	void readUserEditInput(std::string username, std::string confirmatonPassword);
	void editUser(std::string username, std::string confirmationPassword);
	void searchForUserToEdit();

	void deleteUser();

	void buildEmail();
	void sendEmail(std::vector<std::string> vectorOfInitialRecipients, std::string subject,
		std::string fullMessage);

	void signOut();
	bool exitProgram();
};