/*
Edward Hoang
The question this code responds to is one asking for a company database storing
the ID, first name, last name, job, salary, and email address of all employees.
Access to the database requires logging in, and the information in the files themselves
is encrypted for security purposes.
Defines a class representing a database.
*/

#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
#include <limits>
#include <vector>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <conio.h>
#include <Poco\Net\AcceptCertificateHandler.h>
#include <Poco\Net\FilePartSource.h>
#include <Poco\Net\InvalidCertificateHandler.h>
#include <Poco\Net\MailMessage.h>
#include <Poco\Net\NetException.h>
#include <Poco\Net\SecureSMTPClientSession.h>
#include <Poco\Net\SSLManager.h>
#include "Database.h"

#undef max

using namespace std;

//This is the constructor. It starts off the program by asking for a user login.
//If the login is successful, all of the options available to the user will be shown.
Database::Database() {
	if (promptForLogin() == true) {
		mainMenuLoop();
	}
}

//This is the destructor.
Database::~Database() {
}

//This method repeatedly asks for a user to log in until it is successful.
bool Database::promptForLogin() {
	bool loginSuccessful = false;

	while (loginSuccessful == false) {
		string usernameAttempt;
		string passwordAttempt;
		int usernameID;
		int passwordID;

		cout << "Company Database Login" << endl;
		cout << "Username: ";
		getline(cin, usernameAttempt);
		cout << "Password: ";
		passwordAttempt = maskPassword();

		usernameID = checkFromFile(usernameAttempt, "usernames.txt");
		passwordID = checkFromFilePassword(passwordAttempt, usernameID, "passwords.txt");

		if (usernameID == passwordID && usernameID != 0) {
			cout << "Access Granted." << endl << endl;
			loginSuccessful = true;
			return true;
		}
		else {
			cout << "Access Denied." << endl << endl;
		}
	}
}

//This method masks the password a user enters in, replacing the actual characters with asterisks.
string Database::maskPassword() {
	int character;
	string password;

	do {
		character = _getch();
		switch (character) {
		case 0:
			_getch();
			break;
		case '\b':
			if (password.length()) {
				cout << "\b \b";
				password.erase(password.end() - 1);
			}
			break;
		default:
			if (isalnum(character) || ispunct(character)) {
				cout << "*";
				password.push_back(character);
			}
		}
	} while (character != '\r');

	cout << endl << endl;
	return password;
}

//This method encrypts a character by performing a series of mathematical operations on it. 
long long Database::encrypt(int character) {
	return powf(character, 5) * 6 - 17;
}

//This method decrypts a character by undoing each step of the series of mathematical operations
//performed on it to encrypt it. 
int Database::decrypt(long long character) {
	return powf((character + 17) / 6, 1 / 5.f);
}

//This method checks if the inputted attempt exists in a file named fileName in encrypted form.
//If so, it returns the ID associated with the attempt. If not, it returns 0.
int Database::checkFromFile(string attempt, string fileName) {
	string decryptedLine;
	string currentLine;
	long long encryptedChar;

	ifstream file;
	file.open(fileName);

	while (file >> currentLine) {
		if (currentLine.find("ID:") != string::npos) {
			if (attempt == decryptedLine) {
				int id;
				file.close();
				currentLine.erase(0, 3);
				istringstream(currentLine) >> id;
				return id;
			}
			else {
				decryptedLine.erase(decryptedLine.begin(), decryptedLine.end());
			}
		}
		else {
			istringstream(currentLine) >> encryptedChar;
			decryptedLine += (char)decrypt(encryptedChar);
			currentLine = "";
		}
	}

	return 0;
}

//This method checks that the inputted passwordAttempt exists in a file named fileName in encrypted form,
//and that the ID found to be associated with the passwordAttempt matches the ID associated with the 
//username.
int Database::checkFromFilePassword(string passwordAttempt, int usernameID, string fileName) {
	string line;
	string currentLine;
	long long encryptedChar;

	ifstream file;
	file.open(fileName);

	while (file >> currentLine) {
		if (currentLine.find("ID:") != string::npos) {
			if (passwordAttempt == line) {
				int id;
				currentLine.erase(0, 3);
				istringstream(currentLine) >> id;
				if (id == usernameID) {
					file.close();
					return id;
				}
			}
			line.erase(line.begin(), line.end());
			continue;
		}
		else {
			istringstream(currentLine) >> encryptedChar;
			line += (char)decrypt(encryptedChar);
			currentLine = "";
		}
	}

	return 0;
}

//This method presents to the users the list of options he or she can take and then takes as input
//what option he or she wants to take.
int Database::getInput() {
	cout << "Company Database" << endl;
	cout << "Here are your commands:" << endl;
	cout << "1 - Show all employees" << endl;
	cout << "2 - Search for employee" << endl;
	cout << "3 - Add employee" << endl;
	cout << "4 - Edit employee" << endl;
	cout << "5 - Delete employee" << endl;
	cout << "6 - Add user" << endl;
	cout << "7 - Edit user" << endl;
	cout << "8 - Delete user" << endl;
	cout << "9 - Send email" << endl;
	cout << "10 - Sign out" << endl;
	cout << "11 - Exit program" << endl;
	cout << endl;

	cout << "Enter your command: ";
	cin >> input;

	return input;
}

//This method calls a method based on what the user entered as input.
void Database::readInput() {
	if (input == 1) {
		showAllEmployees();
	}
	else if (input == 2) {
		readSearchInput();
	}
	else if (input == 3) {
		addEmployee();
	}
	else if (input == 4) {
		searchForEmployeeToEdit();
	}
	else if (input == 5) {
		deleteEmployee();
	}
	else if (input == 6) {
		addUser();
	}
	else if (input == 7) {
		searchForUserToEdit();
	}
	else if (input == 8) {
		deleteUser();
	}
	else if (input == 9) {
		buildEmail();
	}
	else if (input == 10) {
		signOut();
	}
	else if (input == 11) {
		keepGoing = exitProgram();
	}
	else {
		cout << "Invalid input. Try again." << endl;
		cout << endl;
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
}

//This method keeps the Company Database menu continually showing up until the user exits the program.
void Database::mainMenuLoop() {
	keepGoing = true;

	while (keepGoing == true) {
		input = getInput();
		cout << endl;
		readInput();
	}
}

//This method takes a string, replaces all instances of one character with another, and returns
//the resulting string.
string Database::replaceChar(string str, char charBeingReplaced, char newChar) {
	for (int index = 0; index < str.size(); index++) {
		if (str[index] == charBeingReplaced) {
			str[index] = newChar;
		}
	}

	return str;
}

//This method, for each entryID, moves the information stored in a file named fileName from that file to 
//the database file. 
void Database::buildCellInfo(int entryID, string fileName) {
	string cellInfo;
	string currentLine;
	string strToFind = "ID:";
	strToFind.append(to_string(entryID));
	long long encryptedChar;

	ofstream databaseFile;
	databaseFile.open("Database.txt", ios_base::app);
	ifstream buildingFile;
	buildingFile.open(fileName);

	while (buildingFile >> currentLine) {
		if (currentLine.find("ID:") != string::npos) {
			if (currentLine.find(strToFind) != string::npos) {
				databaseFile << cellInfo << ' ';
				databaseFile.close();
				buildingFile.close();
				return;
			}
			else {
				cellInfo = "";
			}
		}
		else {
			istringstream(currentLine) >> encryptedChar;
			cellInfo += (char)decrypt(encryptedChar);
			currentLine = "";
		}
	}

	databaseFile.close();
	buildingFile.close();
}

//This method decrypts all information about all employees and stores it into database.txt.
void Database::createDatabase() {
	string currentLine;

	for (int index = 1; index <= getLastID("id.txt"); index++) {
		string strToFind = "ID:";
		strToFind.append(to_string(index));
		ifstream readFile;
		readFile.open("id.txt");

		while (readFile >> currentLine) {
			if (currentLine.find(strToFind)) {
				readFile.close();

				buildCellInfo(index, "id.txt");
				buildCellInfo(index, "firstName.txt");
				buildCellInfo(index, "lastName.txt");
				buildCellInfo(index, "job.txt");
				buildCellInfo(index, "salary.txt");
				buildCellInfo(index, "emailAddress.txt");
				ofstream databaseFile;
				databaseFile.open("Database.txt", ios_base::app);
				databaseFile << "\n";
				databaseFile.close();
			}
			else {
				currentLine = "";
			}
		}
	}
}

//This method erases all information in database.txt.
void Database::clearDatabase() {
	ofstream databaseFile;
	databaseFile.open("Database.txt", ofstream::out | ofstream::trunc);
	databaseFile.close();
}

//This method shows all information about all employees.
void Database::showAllEmployees() {
	createDatabase();
	ifstream databaseFile;
	databaseFile.open("Database.txt");

	cout << left << setw(WIDTH) << "ID" << setw(WIDTH) << "First Name" << setw(WIDTH) << "Last Name" << setw(WIDTH) <<
		"Job" << setw(WIDTH) << "Salary" << setw(WIDTH) << "Email Address" << endl;

	while (databaseFile >> id >> firstName >> lastName >> job >> salary >> emailAddress) {
		job = replaceChar(job, '_', ' ');

		cout << left << fixed << setprecision(0) << setw(WIDTH) << id << setw(WIDTH) << firstName << setw(WIDTH)
			<< lastName << setw(WIDTH) << job << '$' << setw(WIDTH) << fixed << setprecision(2) << setw(WIDTH - 1)
			<< salary << setw(WIDTH) << emailAddress << endl;
	}

	clearDatabase();
	databaseFile.close();
	cout << endl;
}

//This method presents to the user the different ways to search for an employee and then takes as input
//which method he or she wants to take.
int Database::getSearchInput() {
	int searchInput;

	cout << "Search By:" << endl;
	cout << "1 - ID" << endl;
	cout << "2 - First Name" << endl;
	cout << "3 - Last Name" << endl;
	cout << "4 - Job" << endl;
	cout << "5 - Salary" << endl;
	cout << "6 - Email Address" << endl;
	cout << endl;

	cout << "Enter your choice: ";
	cin >> searchInput;

	return searchInput;
}

//This method calls a method of searching for an employee based on what the user entered as input
//in getSearchInput().
void Database::readSearchInput() {
	int searchInput = getSearchInput();
	cout << endl;

	if (searchInput == 1) {
		searchByID();
	}
	else if (searchInput == 2) {
		searchByFirstName();
	}
	else if (searchInput == 3) {
		searchByLastName();
	}
	else if (searchInput == 4) {
		searchByJob();
	}
	else if (searchInput == 5) {
		searchBySalary();
	}
	else if (searchInput == 6) {
		searchByEmailAddress();
	}
	else {
		cout << "Invalid input." << endl;
		cout << endl;
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
}

//This method searches for an employee based on his or her ID.
void Database::searchByID() {
	int idBeingSought;
	char employeeFound = 'N';

	cout << "Enter employee's ID: ";
	cin >> idBeingSought;
	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	cout << endl;

	createDatabase();
	ifstream readFile;
	readFile.open("Database.txt");

	while (readFile >> id >> firstName >> lastName >> job >> salary >> emailAddress) {
		job = replaceChar(job, '_', ' ');

		if (id == idBeingSought) {
			cout << left << setw(WIDTH) << "ID" << setw(WIDTH) << "First Name" << setw(WIDTH) << "Last Name" << setw(WIDTH) <<
				"Job" << setw(WIDTH) << "Salary" << setw(WIDTH) << "Email Address" << endl;
			cout << left << fixed << setprecision(0) << setw(WIDTH) << id << setw(WIDTH) << firstName << setw(WIDTH)
				<< lastName << setw(WIDTH) << job << '$' << setw(WIDTH) << fixed << setprecision(2) << setw(WIDTH - 1)
				<< salary << setw(WIDTH) << emailAddress << endl;
			cout << endl;

			cout << "Employee found. Is this the employee you're looking for? Enter N if not or anything else if so: ";
			cin >> employeeFound;

			if (employeeFound == 'N' || employeeFound == 'n') {
				cout << endl;
				cout << "Continuing search..." << endl;
				continue;
			}
			else {
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << endl;
				searchKeepGoing = false;
				break;
			}
		}
	}

	if (employeeFound == 'N' || employeeFound == 'n') {
		cout << "No employee matching that ID found." << endl;
		searchKeepGoing = false;
		cout << endl;
	}

	clearDatabase();
	readFile.close();
}

//This method searches for an employee based on his or her first name.
void Database::searchByFirstName() {
	string firstNameBeingSought;
	char employeeFound = 'N';

	cout << "Enter employee's first name: ";
	cin >> firstNameBeingSought;
	transform(firstNameBeingSought.begin(), firstNameBeingSought.end(), firstNameBeingSought.begin(), toupper);
	cout << endl;

	createDatabase();
	ifstream readFile;
	readFile.open("Database.txt");

	while (readFile >> id >> firstName >> lastName >> job >> salary >> emailAddress) {
		string firstNameCopy = firstName;
		transform(firstNameCopy.begin(), firstNameCopy.end(), firstNameCopy.begin(), toupper);
		job = replaceChar(job, '_', ' ');

		if (firstNameCopy == firstNameBeingSought) {
			cout << left << setw(WIDTH) << "ID" << setw(WIDTH) << "First Name" << setw(WIDTH) << "Last Name" << setw(WIDTH) <<
				"Job" << setw(WIDTH) << "Salary" << setw(WIDTH) << "Email Address" << endl;
			cout << left << fixed << setprecision(0) << setw(WIDTH) << id << setw(WIDTH) << firstName << setw(WIDTH) << lastName << setw(WIDTH) << job <<
				'$' << setw(WIDTH) << fixed << setprecision(2) << setw(WIDTH - 1) << salary << setw(WIDTH) << emailAddress <<
				endl;
			cout << endl;

			cout << "Employee found. Is this the employee you're looking for? Enter N if not or anything else if so: ";
			cin >> employeeFound;

			if (employeeFound == 'N' || employeeFound == 'n') {
				cout << endl;
				cout << "Continuing search..." << endl;
				continue;
			}
			else {
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << endl;
				searchKeepGoing = false;
				break;
			}
		}
	}

	if (employeeFound == 'N' || employeeFound == 'n') {
		cout << "No employee matching that first name found." << endl;
		searchKeepGoing = false;
		cout << endl;
	}

	clearDatabase();
	readFile.close();
}

//This method searches for an employee based on his or her last name.
void Database::searchByLastName() {
	string lastNameBeingSought;
	char employeeFound = 'N';

	cout << "Enter employee's last name: ";
	cin >> lastNameBeingSought;
	transform(lastNameBeingSought.begin(), lastNameBeingSought.end(), lastNameBeingSought.begin(), toupper);
	cout << endl;

	createDatabase();
	ifstream readFile;
	readFile.open("Database.txt");

	while (readFile >> id >> firstName >> lastName >> job >> salary >> emailAddress) {
		string lastNameCopy = lastName;
		transform(lastNameCopy.begin(), lastNameCopy.end(), lastNameCopy.begin(), toupper);
		job = replaceChar(job, '_', ' ');

		if (lastNameCopy == lastNameBeingSought) {
			cout << left << setw(WIDTH) << "ID" << setw(WIDTH) << "First Name" << setw(WIDTH) << "Last Name" << setw(WIDTH) <<
				"Job" << setw(WIDTH) << "Salary" << setw(WIDTH) << "Email Address" << endl;
			cout << left << fixed << setprecision(0) << setw(WIDTH) << id << setw(WIDTH) << firstName << setw(WIDTH) << lastName << setw(WIDTH) << job <<
				'$' << setw(WIDTH) << fixed << setprecision(2) << setw(WIDTH - 1) << salary << setw(WIDTH) << emailAddress <<
				endl;
			cout << endl;

			cout << "Employee found. Is this the employee you're looking for? Enter N if not or anything else if so: ";
			cin >> employeeFound;

			if (employeeFound == 'N' || employeeFound == 'n') {
				cout << endl;
				cout << "Continuing search..." << endl;
				continue;
			}
			else {
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << endl;
				searchKeepGoing = false;
				break;
			}
		}
	}

	if (employeeFound == 'N' || employeeFound == 'n') {
		cout << "No employee matching that last name found." << endl;
		searchKeepGoing = false;
		cout << endl;
	}

	clearDatabase();
	readFile.close();
}

//This method searches for an employee based on his or her job.
void Database::searchByJob() {
	string jobBeingSought;
	char employeeFound = 'N';

	cout << "Enter employee's job: ";
	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	getline(cin, jobBeingSought);
	transform(jobBeingSought.begin(), jobBeingSought.end(), jobBeingSought.begin(), toupper);
	cout << endl;

	createDatabase();
	ifstream readFile;
	readFile.open("Database.txt");

	while (readFile >> id >> firstName >> lastName >> job >> salary >> emailAddress) {
		string jobCopy;
		job = replaceChar(job, '_', ' ');
		jobCopy = job;
		transform(jobCopy.begin(), jobCopy.end(), jobCopy.begin(), toupper);

		if (jobCopy == jobBeingSought) {
			cout << left << setw(WIDTH) << "ID" << setw(WIDTH) << "First Name" << setw(WIDTH) << "Last Name" << setw(WIDTH) <<
				"Job" << setw(WIDTH) << "Salary" << setw(WIDTH) << "Email Address" << endl;
			cout << left << fixed << setprecision(0) << setw(WIDTH) << id << setw(WIDTH) << firstName << setw(WIDTH) << lastName << setw(WIDTH) << job <<
				'$' << setw(WIDTH) << fixed << setprecision(2) << setw(WIDTH - 1) << salary << setw(WIDTH) << emailAddress <<
				endl;
			cout << endl;

			cout << "Employee found. Is this the employee you're looking for? Enter N if not or anything else if so: ";
			cin >> employeeFound;

			if (employeeFound == 'N' || employeeFound == 'n') {
				cout << endl;
				cout << "Continuing search..." << endl;
				continue;
			}
			else {
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << endl;
				searchKeepGoing = false;
				break;
			}
		}
	}

	if (employeeFound == 'N' || employeeFound == 'n') {
		cout << "No employee matching that job found." << endl;
		searchKeepGoing = false;
		cout << endl;
	}

	clearDatabase();
	readFile.close();
}

//This method searches for an employee based on his or her salary.
void Database::searchBySalary() {
	float salaryBeingSought;
	char employeeFound = 'N';

	cout << "Enter employee's salary: $";
	cin >> salaryBeingSought;
	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	cout << endl;

	createDatabase();
	ifstream readFile;
	readFile.open("Database.txt");

	while (readFile >> id >> firstName >> lastName >> job >> salary >> emailAddress) {
		job = replaceChar(job, '_', ' ');

		if (salary == salaryBeingSought) {
			cout << left << setw(WIDTH) << "ID" << setw(WIDTH) << "First Name" << setw(WIDTH) << "Last Name" << setw(WIDTH) <<
				"Job" << setw(WIDTH) << "Salary" << setw(WIDTH) << "Email Address" << endl;
			cout << left << fixed << setprecision(0) << setw(WIDTH) << id << setw(WIDTH) << firstName << setw(WIDTH) << lastName << setw(WIDTH) << job <<
				'$' << setw(WIDTH) << fixed << setprecision(2) << setw(WIDTH - 1) << salary << setw(WIDTH) << emailAddress <<
				endl;
			cout << endl;

			cout << "Employee found. Is this the employee you're looking for? Enter N if not or anything else if so: ";
			cin >> employeeFound;

			if (employeeFound == 'N' || employeeFound == 'n') {
				cout << endl;
				cout << "Continuing search..." << endl;
				continue;
			}
			else {
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << endl;
				searchKeepGoing = false;
				break;
			}
		}
	}

	if (employeeFound == 'N' || employeeFound == 'n') {
		cout << "No employee matching that salary found." << endl;
		searchKeepGoing = false;
		cout << endl;
	}

	clearDatabase();
	readFile.close();
}

//This method searches for an employee based on his or her email address.
void Database::searchByEmailAddress() {
	string emailAddressBeingSought;
	char employeeFound = 'N';

	cout << "Enter employee's email address: ";
	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	getline(cin, emailAddressBeingSought);
	transform(emailAddressBeingSought.begin(), emailAddressBeingSought.end(), emailAddressBeingSought.begin(), toupper);
	cout << endl;

	createDatabase();
	ifstream readFile;
	readFile.open("Database.txt");

	while (readFile >> id >> firstName >> lastName >> job >> salary >> emailAddress) {
		string emailAddressCopy = emailAddress;
		transform(emailAddressCopy.begin(), emailAddressCopy.end(), emailAddressCopy.begin(), toupper);
		job = replaceChar(job, '_', ' ');

		if (emailAddressCopy == emailAddressBeingSought) {
			cout << left << setw(WIDTH) << "ID" << setw(WIDTH) << "First Name" << setw(WIDTH) << "Last Name" << setw(WIDTH) <<
				"Job" << setw(WIDTH) << "Salary" << setw(WIDTH) << "Email Address" << endl;
			cout << left << fixed << setprecision(0) << setw(WIDTH) << id << setw(WIDTH) << firstName << setw(WIDTH) << lastName << setw(WIDTH) << job <<
				'$' << setw(WIDTH) << fixed << setprecision(2) << setw(WIDTH - 1) << salary << setw(WIDTH) << emailAddress <<
				endl;
			cout << endl;

			cout << "Employee found. Is this the employee you're looking for? Enter N if not or anything else if so: ";
			cin >> employeeFound;

			if (employeeFound == 'N' || employeeFound == 'n') {
				cout << endl;
				cout << "Continuing search..." << endl;
				continue;
			}
			else {
				cin.clear();
				cin.ignore(numeric_limits<streamsize>::max(), '\n');
				cout << endl;
				searchKeepGoing = false;
				break;
			}
		}
	}

	if (employeeFound == 'N' || employeeFound == 'n') {
		cout << "No employee matching that email address found." << endl;
		searchKeepGoing = false;
		cout << endl;
	}

	clearDatabase();
	readFile.close();
}

//This method saves to a file named fileName an encrypted form of the input along with the associated ID.
void Database::saveToFile(string input, string fileName, int id) {
	fstream file;
	file.open(fileName, ios::app);
	file.seekg(0, ios::end);

	if (file.tellg() != 0) {
		file << "\n";
	}

	file.seekg(0, ios::beg);

	for (int index = 0; index < input.length(); index++) {
		file << encrypt(input[index]);
		file << "\n";
	}

	file << "ID:" << id;
	file.close();
}

//This method checks if a file of the input fileName actually exists.
bool Database::fileExists(string fileName) {
	ifstream inFile(fileName);
	return inFile.good();
}

//This method checks if a file of the input fileName is empty.
bool Database::isEmpty(string fileName) {
	if (fileExists(fileName) == false || filesystem::file_size(fileName) == 0) {
		return true;
	}
	return false;
}

//This method returns the last ID found in a file named fileName.
int Database::getLastID(string fileName) {
	int lastID;
	string unprocessedID;

	ifstream file;
	file.open(fileName);
	file.seekg(0, ios::end);

	if (isEmpty(fileName) == true) {
		return 0;
	}

	for (int index = -1; unprocessedID.find("I") == string::npos; index--) {
		file.seekg(index, ios::end);
		file >> unprocessedID;
	}

	file.close();
	unprocessedID.erase(0, 3);
	istringstream(unprocessedID) >> lastID;
	return lastID;
}

//This method prompts the user to input the details of the employee being added.
void Database::addEmployeeDetails() {
	cout << "Enter the following:" << endl;

	cout << "ID: ";
	cin >> id;
	while (cin.fail()) {
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "Invalid input. Try again." << endl;
		cout << "ID: ";
		cin >> id;
	}

	cout << "First name: ";
	cin >> firstName;

	cout << "Last name: ";
	cin >> lastName;

	cout << "Job: ";
	cin.ignore();
	getline(cin, job);
	job = replaceChar(job, ' ', '_');

	cout << "Salary: $";
	cin >> salary;
	while (cin.fail()) {
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "Invalid input. Try again." << endl;
		cout << "Salary: $";
		cin >> salary;
	}

	cout << "Email address: ";
	cin >> emailAddress;
}

//This method adds an employee to the database.
void Database::addEmployee() {
	int matchID;
	addEmployeeDetails();
	matchID = 1 + getLastID("id.txt");

	saveToFile(to_string(id), "id.txt", matchID);
	saveToFile(firstName, "firstName.txt", matchID);
	saveToFile(lastName, "lastName.txt", matchID);
	saveToFile(job, "job.txt", matchID);
	saveToFile(to_string(salary), "salary.txt", matchID);
	saveToFile(emailAddress, "emailAddress.txt", matchID);

	cout << endl << "Employee added." << endl << endl;
}

//This method presents to the user the different attributes to edit regarding an employee and then takes
//as input which attribute he or she wants to edit.
int Database::getEditInput() {
	int editInput;

	cout << "Edit:" << endl;
	cout << "1 - ID" << endl;
	cout << "2 - First Name" << endl;
	cout << "3 - Last Name" << endl;
	cout << "4 - Job" << endl;
	cout << "5 - Salary" << endl;
	cout << "6 - Email Address" << endl;
	cout << endl;

	cout << "Enter your choice: ";
	cin >> editInput;

	return editInput;
}

//This method prompts the user to edit an attribute of an employee based on what the user entered 
//as input in getEditInput(). 
void Database::readEditInput(int idBeingSought) {
	int editInput = getEditInput();

	if (editInput == 1) {
		cout << "ID: ";
		cin >> id;
		while (cin.fail()) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Invalid input. Try again." << endl;
			cout << "ID: ";
			cin >> id;
		}
		editEmployee(idBeingSought);
	}
	else if (editInput == 2) {
		cout << "First name: ";
		cin >> firstName;
		editEmployee(idBeingSought);
	}
	else if (editInput == 3) {
		cout << "Last name: ";
		cin >> lastName;
		editEmployee(idBeingSought);
	}
	else if (editInput == 4) {
		cout << "Job: ";
		cin.ignore();
		getline(cin, job);
		job = replaceChar(job, ' ', '_');
		editEmployee(idBeingSought);
	}
	else if (editInput == 5) {
		cout << "Salary: $";
		cin >> salary;
		while (cin.fail()) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cout << "Invalid input. Try again." << endl;
			cout << "Salary: $";
			cin >> salary;
		}
		editEmployee(idBeingSought);
	}
	else if (editInput == 6) {
		cout << "Email address: ";
		cin >> emailAddress;
		editEmployee(idBeingSought);
	}
	else {
		cout << endl << "Invalid input." << endl;
		cout << endl;
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		clearDatabase();
	}
}

//This method edits an employee.
void Database::editEmployee(int idBeingSought) {
	int matchID;
	matchID = checkFromFile(to_string(idBeingSought), "id.txt");

	if (matchID != 0) {
		findLinesToDelete(matchID, "id.txt");
		findLinesToDelete(matchID, "firstName.txt");
		findLinesToDelete(matchID, "lastName.txt");
		findLinesToDelete(matchID, "job.txt");
		findLinesToDelete(matchID, "salary.txt");
		findLinesToDelete(matchID, "emailAddress.txt");

		matchID = 1 + getLastID("id.txt");

		saveToFile(to_string(id), "id.txt", matchID);
		saveToFile(firstName, "firstName.txt", matchID);
		saveToFile(lastName, "lastName.txt", matchID);
		saveToFile(job, "job.txt", matchID);
		saveToFile(to_string(salary), "salary.txt", matchID);
		saveToFile(emailAddress, "emailAddress.txt", matchID);

		clearDatabase();
		cout << endl << "Employee edited." << endl;
	}

	cout << endl;
}

//This method takes as input the employee to be edited and searches for that employee in the 
//database.
void Database::searchForEmployeeToEdit() {
	int idBeingSought;
	bool employeeFound = false;

	cout << "Enter employee's ID: ";
	cin >> idBeingSought;
	cin.clear();
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	cout << endl;

	createDatabase();
	ifstream readFile;
	readFile.open("Database.txt");

	while (readFile >> id >> firstName >> lastName >> job >> salary >> emailAddress) {
		if (id == idBeingSought) {
			employeeFound = true;
			readEditInput(idBeingSought);
		}
	}

	if (employeeFound == false) {
		cout << "Employee not found." << endl << endl;
		clearDatabase();
	}
}

//This method generates a name for the temporary file to be used, starting from "TempFile" and appending
//additional random letters to the file name until a name that isn't already used is found.
string Database::generateTempFileName() {
	const int MAX = 26;
	string tempFileName = "TempFile";
	char alphabet[MAX] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g',
						  'h', 'i', 'j', 'k', 'l', 'm', 'n',
						  'o', 'p', 'q', 'r', 's', 't', 'u',
						  'v', 'w', 'x', 'y', 'z' };

	if (fileExists(tempFileName) == false) {
		return tempFileName;
	}
	else {
		while (fileExists(tempFileName) == true) {
			tempFileName = tempFileName + alphabet[rand() % MAX];
			if (fileExists(tempFileName) == false) {
				return tempFileName;
			}
		}
	}
}

//This method copies the contents of the temporary file back into the original file.
void Database::copyFromTempFile(string inputFileName, string tempFileName) {
	const int REMOVE_WHITE_SPACE = 1;
	const char* tempFileNameAsChar = tempFileName.c_str();
	string content = "";

	ofstream newInputFile;
	newInputFile.open(inputFileName, ofstream::out | ofstream::trunc);
	ifstream newTempFile;
	newTempFile.open(tempFileName);

	for (int index = 0; newTempFile.eof() != true; index++) {
		content += newTempFile.get();
	}

	content.erase(content.end() - REMOVE_WHITE_SPACE);

	newInputFile << content;
	newInputFile.close();
	newTempFile.close();
	std::remove(tempFileNameAsChar);
}

//This method looks for which lines need to be deleted in a file to remove only the information
//about the employee or user being deleted.
void Database::findLinesToDelete(int matchID, string fileName) {
	string currentLine;
	string cellInfo;
	int currentLinesCount = 0;
	int totalLinesCount = 0;
	long long encryptedChar;
	string strToFind = "ID:";
	strToFind.append(to_string(matchID));

	ifstream readFile(fileName);

	while (readFile >> currentLine) {
		currentLinesCount++;
		totalLinesCount++;
		if (currentLine.find("ID:") != string::npos) {
			if (currentLine.find(strToFind) != string::npos) {
				readFile.close();
				break;
			}
			else {
				cellInfo.erase(cellInfo.begin(), cellInfo.end());
				currentLinesCount = 0;
				continue;
			}
		}
		else {
			istringstream(currentLine) >> encryptedChar;
			cellInfo += (char)decrypt(encryptedChar);
			currentLine = "";
		}
	}

	readFile.close();
	deleteCell(fileName, currentLinesCount, totalLinesCount);
}

//This method deletes the lines marked for deletion in findLinesToDelete().
void Database::deleteCell(string fileName, int currentLinesCount, int totalLinesCount) {
	const int FIRST_LINE = 1;
	int secondTotalLinesCount = 0;
	string currentLine;

	ifstream inputFile(fileName);
	string tempFileName = generateTempFileName();
	ofstream tempFile(tempFileName);

	while (inputFile >> currentLine) {
		stringstream lineOfText(currentLine);
		secondTotalLinesCount++;
		int startIntervalToIgnore = totalLinesCount - currentLinesCount + 1;
		int endIntervalToIgnore = totalLinesCount;
		if (secondTotalLinesCount < startIntervalToIgnore || secondTotalLinesCount >= endIntervalToIgnore + 1) {
			while (lineOfText >> currentLine) {
				tempFile << "\n";
				tempFile << currentLine;
			}
		}
	}

	inputFile.close();
	tempFile.close();
	copyFromTempFile(fileName, tempFileName);
}

//This method deletes all information about an employee.
void Database::deleteEmployee() {
	int idBeingSought;
	int matchID;

	cout << "Enter Employee's ID: ";
	cin >> idBeingSought;
	cout << endl;

	matchID = checkFromFile(to_string(idBeingSought), "id.txt");

	if (matchID != 0) {
		findLinesToDelete(matchID, "id.txt");
		findLinesToDelete(matchID, "firstName.txt");
		findLinesToDelete(matchID, "lastName.txt");
		findLinesToDelete(matchID, "job.txt");
		findLinesToDelete(matchID, "salary.txt");
		findLinesToDelete(matchID, "emailAddress.txt");

		cout << "Employee deleted." << endl << endl;
	}
	else {
		cout << "Employee not found." << endl << endl;
	}
}

//This method adds a user capable of logging into the database.
void Database::addUser() {
	string username;
	string password;
	int id;

	cout << "Enter the following:" << endl;
	cout << "Username: ";
	cin >> username;
	cout << "Password: ";
	password = maskPassword();

	if (checkFromFile(username, "usernames.txt") != 0) {
		cout << "That username is not availble. Please try another." << endl << endl;
		return;
	}

	id = 1 + getLastID("usernames.txt");
	saveToFile(username, "usernames.txt", id);
	saveToFile(password, "passwords.txt", id);
	cout << "User added." << endl << endl;
}

//This method presents to the user the different parts of a user's login information and then takes
//as input which one he or she wants to edit.
int Database::getUserEditInput() {
	int userEditInput;

	cout << "Edit:" << endl;
	cout << "1 - Username" << endl;
	cout << "2 - Password" << endl;

	cout << "Enter your choice: ";
	cin >> userEditInput;

	return userEditInput;
}

//This method prompts the user to edit a part of a user's login information based on what the user entered 
//as input in getUserEditInput().
void Database::readUserEditInput(string username, string confirmationPassword) {
	int userEditInput = getUserEditInput();

	if (userEditInput == 1) {
		string newUsername;
		cout << "Username: ";
		cin >> newUsername;
		editUser(newUsername, confirmationPassword);
		cout << endl;
	}
	else if (userEditInput == 2) {
		string newPassword;
		cout << "Password: ";
		newPassword = maskPassword();
		editUser(username, newPassword);
	}
}

//This method edits a user's login information.
void Database::editUser(string username, string confirmationPassword) {
	id = 1 + getLastID("usernames.txt");

	saveToFile(username, "usernames.txt", id);
	saveToFile(confirmationPassword, "passwords.txt", id);

	cout << "User edited." << endl << endl;
}

//This method takes as input the user whose login information is to be edited and searches for that user 
//in the user database.
void Database::searchForUserToEdit() {
	string usernameBeingSought;
	string confirmationPassword;
	int matchID;

	cout << "Enter User Username: ";
	cin >> usernameBeingSought;
	cout << "Enter User Password to confirm: ";
	confirmationPassword = maskPassword();

	matchID = checkFromFile(usernameBeingSought, "usernames.txt");

	if (matchID != 0) {
		int usernameID = matchID;
		int passwordID = checkFromFilePassword(confirmationPassword, usernameID, "passwords.txt");

		if (usernameID == passwordID && usernameID != 0) {
			readUserEditInput(usernameBeingSought, confirmationPassword);
			findLinesToDelete(matchID, "usernames.txt");
			findLinesToDelete(matchID, "passwords.txt");
		}
		else {
			cout << "Confirmation password is incorrect." << endl << endl;
		}
	}
	else {
		cout << "User not found." << endl;
		cout << endl;
	}
}

//This method deletes all information about a user, removing that user's ability to
//log in to the database.
void Database::deleteUser() {
	string usernameBeingSought;
	string confirmationPassword;
	int matchID;

	cout << "Enter User Username: ";
	cin >> usernameBeingSought;
	cout << "Enter User Password to confirm: ";
	confirmationPassword = maskPassword();

	matchID = checkFromFile(usernameBeingSought, "usernames.txt");

	if (matchID != 0) {
		int usernameID = matchID;
		int passwordID = checkFromFilePassword(confirmationPassword, usernameID, "passwords.txt");

		if (usernameID == passwordID && usernameID != 0) {
			findLinesToDelete(matchID, "usernames.txt");
			findLinesToDelete(matchID, "passwords.txt");
			cout << "User deleted." << endl << endl;
		}
		else {
			cout << "Confirmation password is incorrect." << endl << endl;
		}
	}
	else {
		cout << "User not found." << endl;
		cout << endl;
	}
}

//This method allows the user to construct an email to be sent.
void Database::buildEmail() {
	int numberOfRecipients;
	vector<string> vectorOfInitialRecipients(0);
	string subject;
	bool continueMessage = true;
	string message;
	string fullMessage;

	cout << "How many recipients will you send this email to? ";
	cin >> numberOfRecipients;
	while (cin.fail()) {
		cin.clear();
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
		cout << "Invalid input. Try again." << endl;
		cout << "How many recipients will you send this email to? ";
		cin >> numberOfRecipients;
	}
	cout << endl;

	while (numberOfRecipients > vectorOfInitialRecipients.size()) {
		cout << "Enter recipient's email address: ";
		string initialRecipient;
		cin >> initialRecipient;
		vectorOfInitialRecipients.push_back(initialRecipient);
	}

	cout << "Enter subject: ";
	cin.ignore();
	getline(cin, subject);

	cout << "Enter message (Enter /END on separate line to end message): " << endl;
	while (continueMessage == true) {
		getline(cin, message);

		if (message == "/END") {
			continueMessage = false;
		}
		else {
			fullMessage.append(message);
			fullMessage.append("\n");
		}
	}

	sendEmail(vectorOfInitialRecipients, subject, fullMessage);
}

//This method sends the email constructed in buildEmail() to the recipient(s).
void Database::sendEmail(vector<string> vectorOfInitialRecipients, string subject, string fullMessage) {
	Poco::Net::MailMessage mailMessage;

	mailMessage.setSender("Company Database <databasetester16@gmail.com>");

	for (int index = 0; index < vectorOfInitialRecipients.size(); index++) {
		mailMessage.addRecipient(Poco::Net::MailRecipient(Poco::Net::MailRecipient::PRIMARY_RECIPIENT,
			vectorOfInitialRecipients[index]));
	}

	mailMessage.setSubject(subject);
	mailMessage.setContent(fullMessage);

	Poco::Net::SecureSMTPClientSession session("smtp.gmail.com", 587);
	session.open();
	Poco::Net::initializeSSL();

	Poco::SharedPtr<Poco::Net::InvalidCertificateHandler> ptrHandler = new Poco::Net::AcceptCertificateHandler(false);
	Poco::Net::Context::Ptr ptrContext = new Poco::Net::Context(Poco::Net::Context::CLIENT_USE, "", "", "",
		Poco::Net::Context::VERIFY_RELAXED, 9, true, "ALL:!ADH:!LOW:!EXP:!MD5:@STRENGTH");
	Poco::Net::SSLManager::instance().initializeClient(NULL, ptrHandler, ptrContext);

	try {
		session.login();
		cout << endl;
		cout << "Sending email . . . " << endl;
		if (session.startTLS(ptrContext)) {
			session.login(Poco::Net::SecureSMTPClientSession::AUTH_LOGIN, "databasetester16@gmail.com", "testingdatabase16");
			session.sendMessage(mailMessage);
		}
		cout << "Email was sent successfully." << endl;
		session.close();
		Poco::Net::uninitializeSSL();
	}
	catch (Poco::Net::SMTPException& error) {
		cout << error.message() << endl;
		session.close();
		Poco::Net::uninitializeSSL();
	}
	cout << endl;
}

//This method signs the user out of their account, bringing him or her back to
//the login menu.
void Database::signOut() {
	cout << "You have signed out." << endl << endl;
	cin.ignore();

	if (promptForLogin() == true) {
		mainMenuLoop();
	}
}

//This method ends the program.
bool Database::exitProgram() {
	cout << "You have exited the program." << endl;
	return false;
}