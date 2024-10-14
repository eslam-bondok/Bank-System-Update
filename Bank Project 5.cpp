
#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <vector>
#include <iomanip>
#include <cstdlib>

using namespace std;

struct stUsers
{
	string UserName;
	string Password;
	int Permissions;
	bool MarkForDelete = false;
};

struct stTransaction
{
	int TransactionNumber;
	string TransactionType;
	double TransactionAmount;
};

stUsers CurrentUser;

enum enWhatToShow { enShowDeposit = 1, enShowWithdraw = 2 };

enum enTransaction { enDeposit = 1, enWithdraw = 2, TotalBalances = 3, ShowTransactions = 4, MainMenu = 5 };

enum enWhatToDo { enShowClientList = 1, enAddNewClient = 2, enDeleteClient = 3, enUpdateClientInfo = 4, enFindClient = 5, enTransactions = 6, enManageUsers = 7, enLogout = 8 };

enum enManageusers { enListUsers = 1, enAddNewUser = 2, enDeleteUser = 3, enUpdateUser = 4, enFindUser = 5, enMainMenu = 6 };

enum enPermission {
	PAll = -1, PShowClientList = 1, PAddNewClient = 2,
	PDeleteClient = 4, PUpdateClient = 8, PFindClient = 16, PTransactions = 32, PManageUsers = 64
};

const string ClientsFile = "Clients.txt";
const string UsersFile = "Users.txt";
const string TransactionsFile = "Transactions.txt";

void PerformMainMenuOption(enWhatToDo WhatToDo);
void ShowTransactionsMenu();
void ShowManageUsersMenu();
void GobackToMainMenu();
void ShowTransactionsScreen();
void Login();

struct stClient
{
	string AccountNumber;
	string PinCode;
	string ClientName;
	string Phone;
	stTransaction Transaction;
	double AccountBalance;
	bool MarkForDelete = false;
};

void PrintTransactionRecord(stClient Client)
{
	cout << "| " << left << setw(15) << Client.AccountNumber;
	cout << "| " << left << setw(30) << Client.ClientName;
	cout << "| " << left << setw(20) << Client.Transaction.TransactionNumber;
	cout << "| " << left << setw(20) << Client.Transaction.TransactionType;
	cout << "| " << left << setw(20) << Client.Transaction.TransactionAmount;
	cout << "| " << left << setw(15) << Client.AccountBalance;
}

string ConvertTransactionRecordToLine(stClient& Client, string Separator = "#//#")
{
	string DataLine = "";

	DataLine += Client.AccountNumber + Separator;
	DataLine += Client.ClientName + Separator;
	DataLine += to_string(Client.Transaction.TransactionNumber) + Separator;
	DataLine += Client.Transaction.TransactionType + Separator;
	DataLine += to_string(Client.Transaction.TransactionAmount) + Separator;
	DataLine += to_string(Client.AccountBalance);

	return DataLine;
}

void SaveTransactionToFile(string FileName, stClient& Client)
{
	fstream MyFile;
	MyFile.open(FileName, ios::app);

	if (MyFile.is_open())
	{
		string DataLine = ConvertTransactionRecordToLine(Client);
		MyFile << DataLine << endl;
		MyFile.close();
	}
}

int GenerateTransactionNumber(int From, int To)
{
	int TransactionNumber = rand() % (To - From + 1) + From;
	return TransactionNumber;
}

string GetTransactionType(enWhatToShow WhatToShow)
{
	return ((WhatToShow == enWhatToShow::enShowDeposit) ? "Deposit" : "Withdraw");
}

enWhatToShow enTransactionType(double DepositAmount)
{
	return((DepositAmount >= 0) ? enWhatToShow::enShowDeposit : enWhatToShow::enShowWithdraw);
}

void SaveTransactionData(stClient& Client, double DepositAmount, enWhatToShow WhatToShow)
{
	Client.Transaction.TransactionType = GetTransactionType(WhatToShow);
	Client.Transaction.TransactionAmount = DepositAmount;
	Client.Transaction.TransactionNumber = GenerateTransactionNumber(1000, 10000);
}

void ResetScreen()
{
	system("color 0F");
	system("cls");
}

void PrintAccessDeniedScreen()
{
	ResetScreen();
	cout << "--------------------------------------" << endl;
	cout << "Access Denied,\nYou Don't have a permission to Do This,\nPlease Contact Your Admin." << endl;
	cout << "--------------------------------------" << endl;
}

bool CheckAccessPermission(enPermission Permission)
{
	if (CurrentUser.Permissions == enPermission::PAll)
		return true;

	if ((Permission & CurrentUser.Permissions) == Permission)
		return true;

	return false;
}

short ReadMainMenuOption()
{
	short TaskNumber;
	cout << "Choose what do you want to do? [1 to 8]? ";
	cin >> TaskNumber;

	return TaskNumber;
}

void ShowMainMenu()
{
	short TaskNumber;

	ResetScreen();

	cout << "===============================================" << endl;
	cout << "\t\tMain Menu Screen" << endl;
	cout << "===============================================" << endl;

	cout << "\t\t[1] Show Client List." << endl;
	cout << "\t\t[2] Add New Client." << endl;
	cout << "\t\t[3] Delete Client." << endl;
	cout << "\t\t[4] Update Client Info." << endl;
	cout << "\t\t[5] Find Client." << endl;
	cout << "\t\t[6] Transactions." << endl;
	cout << "\t\t[7] Manage Users." << endl;
	cout << "\t\t[8] Logout." << endl;

	cout << "===============================================" << endl;

	PerformMainMenuOption((enWhatToDo)ReadMainMenuOption());
}

void GobackToMainMenu()
{
	cout << "\n\nPress any key to go back to main menu......";
	system("Pause>0");
	ShowMainMenu();
}

void PrintClientRecord(stClient Client)
{
	cout << "| " << left << setw(15) << Client.AccountNumber;
	cout << "| " << left << setw(12) << Client.PinCode;
	cout << "| " << left << setw(40) << Client.ClientName;
	cout << "| " << left << setw(12) << Client.Phone;
	cout << "| " << left << setw(12) << Client.AccountBalance;
}

string ReadAccountNumber()
{
	string AccountNumber;
	cout << "\nEnter Account Number? ";
	cin >> AccountNumber;

	return AccountNumber;
}

vector<string> SplitString(string S1, string Delimiter)
{
	vector<string> vString;
	short Pos = 0;
	string sWord;

	while ((Pos = S1.find(Delimiter)) != std::string::npos)
	{
		sWord = S1.substr(0, Pos);

		if (sWord != "")
		{
			vString.push_back(sWord);
		}

		S1.erase(0, Pos + Delimiter.length());
	}

	if (S1 != "")
	{
		vString.push_back(S1);
	}

	return vString;
}

stClient ConvertLineToRecord(string Line, string Separator = "#//#")
{
	vector<string> vString = SplitString(Line, Separator);
	stClient Client;

	Client.AccountNumber = vString[0];
	Client.PinCode = vString[1];
	Client.ClientName = vString[2];
	Client.Phone = vString[3];
	Client.AccountBalance = stod(vString[4]);

	return Client;
}

vector<stClient> LoadClientsDataFromFile(string FileName)
{
	vector<stClient> vClients;

	fstream MyFile;
	MyFile.open(FileName, ios::in);

	string Line;
	stClient Client;

	if (MyFile.is_open())
	{
		while (getline(MyFile, Line))
		{
			Client = ConvertLineToRecord(Line);
			vClients.push_back(Client);
		}

		MyFile.close();
	}

	return vClients;
}

bool CheckIfClientExistsByAccountNumber(string AccountNumber, string FileName)
{
	fstream MyFile;
	MyFile.open(FileName, ios::in);

	string Line;
	stClient Client;

	if (MyFile.is_open())
	{
		while (getline(MyFile, Line))
		{
			Client = ConvertLineToRecord(Line);

			if (Client.AccountNumber == AccountNumber)
			{
				MyFile.close();
				return true;
			}
		}

		MyFile.close();
	}

	return false;
}

stClient ReadNewClient()
{
	stClient Client;

	cout << "\nEnter Account Number? ";
	getline(cin >> ws, Client.AccountNumber);

	while (CheckIfClientExistsByAccountNumber(Client.AccountNumber, ClientsFile))
	{
		cout << "\nClient With the Account Number [" << Client.AccountNumber << "] already exists, Enter another Account Number? ";
		getline(cin >> ws, Client.AccountNumber);
	}

	cout << "\nEnter PinCode? ";
	getline(cin >> ws, Client.PinCode);

	cout << "\nEnter Client Name? ";
	getline(cin, Client.ClientName);

	cout << "\nEnter Phone? ";
	getline(cin, Client.Phone);

	cout << "\nEnter Account Balance? ";
	cin >> Client.AccountBalance;

	return Client;
}

void AddDataLineToFile(string FileName, string Line)
{
	fstream MyFile;
	MyFile.open(FileName, ios::out | ios::app);

	if (MyFile.is_open())
	{
		MyFile << Line << endl;
		MyFile.close();
	}
}

string ConvertRecordToLine(stClient ClientRecord, string Separator = "#//#")
{
	string DataLine = "";

	DataLine += ClientRecord.AccountNumber + Separator;
	DataLine += ClientRecord.PinCode + Separator;
	DataLine += ClientRecord.ClientName + Separator;
	DataLine += ClientRecord.Phone + Separator;
	DataLine += to_string(ClientRecord.AccountBalance);

	return DataLine;
}

void SaveClientsDataToFile(string FileName, vector<stClient>& vClients)
{
	fstream MyFile;
	MyFile.open(FileName, ios::out);

	string DataLine;

	if (MyFile.is_open())
	{
		for (stClient& C : vClients)
		{
			if (C.MarkForDelete == false)
			{
				DataLine = ConvertRecordToLine(C);
				MyFile << DataLine << endl;
			}
		}

		MyFile.close();
	}
}

void ShowClientsList()
{
	if (!CheckAccessPermission(enPermission::PShowClientList))
	{
		PrintAccessDeniedScreen();
		GobackToMainMenu();
		return;
	}

	ResetScreen();

	vector<stClient> vClients = LoadClientsDataFromFile(ClientsFile);

	cout << "\t\t\t\tClient List (" << vClients.size() << ") Client(s)." << endl;
	cout << "________________________________________________________________________________________________\n\n";
	cout << "| " << left << setw(15) << "Account Number";
	cout << "| " << left << setw(12) << "Pin Code";
	cout << "| " << left << setw(40) << "Client Name";
	cout << "| " << left << setw(12) << "Phone";
	cout << "| " << left << setw(12) << "Balance";
	cout << "\n________________________________________________________________________________________________\n";

	for (stClient& Client : vClients)
	{
		PrintClientRecord(Client);
		cout << endl;
	}

	cout << "________________________________________________________________________________________________\n";
}

void AddNewClient()
{
	stClient Client;
	Client = ReadNewClient();
	AddDataLineToFile(ClientsFile, ConvertRecordToLine(Client));
}

void AddNewClients()
{
	char AddMoreClients = 'Y';
	do {
		cout << "Adding New Client : " << endl;

		AddNewClient();

		cout << "\nClient Added Successfully, do you want to add more clients? Y/N? ";
		cin >> AddMoreClients;

	} while (toupper(AddMoreClients) == 'Y');
}

void AddClients()
{
	ResetScreen();

	if (!CheckAccessPermission(enPermission::PAddNewClient))
	{
		PrintAccessDeniedScreen();
		GobackToMainMenu();
		return;
	}

	cout << "------------------------------------------\n";
	cout << "\tAdd New Clients Screen\n";
	cout << "------------------------------------------\n";

	AddNewClients();
}

bool FindClientByAccountNumber(string AccountNumber, vector<stClient> vClients, stClient& Client)
{
	for (stClient& C : vClients)
	{
		if (C.AccountNumber == AccountNumber)
		{
			Client = C;
			return true;
		}
	}

	return false;
}

bool MarkForDeleteByAccountNumber(string AccountNumber, vector<stClient>& vClients)
{
	for (stClient& C : vClients)
	{
		if (C.AccountNumber == AccountNumber)
		{
			C.MarkForDelete = true;
			return true;
		}
	}

	return false;
}

void PrintClientCard(stClient Client)
{
	cout << "\nThe Following are the client details : " << endl;
	cout << "------------------------------------------\n";
	cout << "\nAccount Number  : " << Client.AccountNumber;
	cout << "\nPin Code        : " << Client.PinCode;
	cout << "\nClient Name     : " << Client.ClientName;
	cout << "\nPhone           : " << Client.Phone;
	cout << "\nAccount Balance : " << Client.AccountBalance;
	cout << "\n------------------------------------------\n";
}

bool DeleteClientByAccountNumber(string AccountNumber, vector<stClient>& vClients)
{
	stClient Client;
	char DeleteClient = 'n';

	if (FindClientByAccountNumber(AccountNumber, vClients, Client))
	{
		PrintClientCard(Client);

		cout << "\nAre you sure you want to delete this client? Y/N? ";
		cin >> DeleteClient;

		if (DeleteClient == 'Y' || DeleteClient == 'y')
		{
			MarkForDeleteByAccountNumber(AccountNumber, vClients);
			SaveClientsDataToFile(ClientsFile, vClients);

			cout << "\nClient Deleted Successfully.\n\n";
			return true;
		}
	}
	else
	{
		cout << "\nThe Client with the Account Number (" << AccountNumber << ") is Not Found!\n\n";
		return false;
	}
}

void DeleteClient()
{
	ResetScreen();

	if (!CheckAccessPermission(enPermission::PDeleteClient))
	{
		PrintAccessDeniedScreen();
		GobackToMainMenu();
		return;
	}

	cout << "------------------------------------------\n";
	cout << "\tDelete Client Screen\n";
	cout << "------------------------------------------\n";

	string AccountNumber = ReadAccountNumber();
	vector<stClient> vClients = LoadClientsDataFromFile(ClientsFile);
	DeleteClientByAccountNumber(AccountNumber, vClients);
}

stClient ChangeClientRecord(string AccountNumber)
{
	stClient Client;

	Client.AccountNumber = AccountNumber;

	cout << "\nEnter Pin Code? ";
	getline(cin >> ws, Client.PinCode);

	cout << "\nEnter Client Name? ";
	getline(cin, Client.ClientName);

	cout << "\nEnter Phone? ";
	getline(cin, Client.Phone);

	cout << "\nEnter Account Balance? ";
	cin >> Client.AccountBalance;

	return Client;
}

bool UpdateClientByAccountNumber(string AccountNumber, vector<stClient>& vClients)
{
	stClient Client;
	char UpdateClient = 'Y';

	if (FindClientByAccountNumber(AccountNumber, vClients, Client))
	{
		PrintClientCard(Client);

		cout << "\nAre you sure you want to update this client? Y/N? ";
		cin >> UpdateClient;

		if (UpdateClient == 'Y' || UpdateClient == 'y')
		{
			for (stClient& Client : vClients)
			{
				if (Client.AccountNumber == AccountNumber)
				{
					Client = ChangeClientRecord(AccountNumber);
				}
			}

			SaveClientsDataToFile(ClientsFile, vClients);

			cout << "\nClient Updated Successfully.\n\n";
			return true;
		}
	}
	else
	{
		cout << "\nThe Client with the Account Number (" << AccountNumber << ") is Not Found!\n\n";
		return false;
	}
}

void UpdateClientInfo()
{
	ResetScreen();

	if (!CheckAccessPermission(enPermission::PUpdateClient))
	{
		PrintAccessDeniedScreen();
		GobackToMainMenu();
		return;
	}

	cout << "------------------------------------------\n";
	cout << "\tUpdate Client Info Screen\n";
	cout << "------------------------------------------\n";

	string AccountNumber = ReadAccountNumber();
	vector<stClient> vClients = LoadClientsDataFromFile(ClientsFile);
	UpdateClientByAccountNumber(AccountNumber, vClients);
}

void FindClient()
{
	ResetScreen();

	if (!CheckAccessPermission(enPermission::PFindClient))
	{
		PrintAccessDeniedScreen();
		GobackToMainMenu();
		return;
	}

	cout << "------------------------------------------\n";
	cout << "\tFind Client Screen\n";
	cout << "------------------------------------------\n";

	string AccountNumber = ReadAccountNumber();
	vector<stClient> vClients = LoadClientsDataFromFile(ClientsFile);
	stClient Client;

	if (FindClientByAccountNumber(AccountNumber, vClients, Client))
	{
		PrintClientCard(Client);
		cout << endl;
	}
	else
	{
		cout << "\nThe Client with the Account Number (" << AccountNumber << ") is Not Found!\n\n";
	}
}

void ExitProgram()
{
	ResetScreen();

	Login();
}

void PerformMainMenuOption(enWhatToDo WhatToDo)
{
	switch (WhatToDo)
	{
	case enWhatToDo::enShowClientList:
		ShowClientsList();
		GobackToMainMenu();
		break;

	case enWhatToDo::enAddNewClient:
		AddClients();
		GobackToMainMenu();
		break;

	case enWhatToDo::enDeleteClient:
		DeleteClient();
		GobackToMainMenu();
		break;

	case enWhatToDo::enUpdateClientInfo:
		UpdateClientInfo();
		GobackToMainMenu();
		break;

	case enWhatToDo::enFindClient:
		FindClient();
		GobackToMainMenu();
		break;

	case enWhatToDo::enTransactions:
		ShowTransactionsMenu();
		GobackToMainMenu();

	case enWhatToDo::enManageUsers:
		ShowManageUsersMenu();
		GobackToMainMenu();

	case enWhatToDo::enLogout:
		ExitProgram();
		break;
	}
}

bool AddDepositAmount(string AccountNumber, double DepositAmount, vector<stClient>& vClients)
{
	char Answer = 'n';
	cout << "\nAre you sure you want perform this transaction? Y/N? ";
	cin >> Answer;

	if (Answer == 'Y' || Answer == 'y')
	{
		for (stClient& Client : vClients)
		{
			if (Client.AccountNumber == AccountNumber)
			{
				Client.AccountBalance += DepositAmount;
				SaveClientsDataToFile(ClientsFile, vClients);
				SaveTransactionData(Client, DepositAmount, enTransactionType(DepositAmount));
				SaveTransactionToFile(TransactionsFile, Client);
				cout << "\n\nDone Successfully, New balance is : " << Client.AccountBalance;

				return true;
			}
		}

	}

	return false;
}

double GetDepositAmount()
{
	double DepositAmount;

	do {
		cout << "\nPlease enter deposit amount? " << endl;
		cin >> DepositAmount;

	} while (DepositAmount <= 0);

	return DepositAmount;
}

double GetWithdrawAmount(stClient Client)
{
	double WithdrawAmount;

	cout << "\nPlease enter withdraw amount? ";
	cin >> WithdrawAmount;

	while (WithdrawAmount >= Client.AccountBalance)
	{
		cout << "\n\nAmount Exceeds the balance, you can withdraw up to : " << Client.AccountBalance << endl;
		cout << "\nPlease enter another amount? ";
		cin >> WithdrawAmount;
	}

	return WithdrawAmount * -1;
}

void DepositByAccountNumber(string AccountNumber, vector<stClient>& vClients)
{
	stClient Client;

	while (!FindClientByAccountNumber(AccountNumber, vClients, Client))
	{
		cout << "\nClient with Account Number [" << AccountNumber << "] does not exist.\n";
		AccountNumber = ReadAccountNumber();
	}

	PrintClientCard(Client);

	double DepositAmount;
	DepositAmount = GetDepositAmount();

	AddDepositAmount(AccountNumber, DepositAmount, vClients);
}

void ShowDepositScreen()
{
	ResetScreen();

	cout << "------------------------------------------\n";
	cout << "\tDeposit Screen\n";
	cout << "------------------------------------------\n";

	string AccountNumber = ReadAccountNumber();
	vector <stClient> vClients = LoadClientsDataFromFile(ClientsFile);
	DepositByAccountNumber(AccountNumber, vClients);
}

void ShowTotalBalancesScreen()
{
	ResetScreen();

	vector<stClient> vClients = LoadClientsDataFromFile(ClientsFile);
	double TotalBalances = 0;

	cout << "\t\t\t\tBalances List (" << vClients.size() << ") Client(s)." << endl;
	cout << "________________________________________________________________________________________________\n\n";
	cout << "| " << left << setw(15) << "Account Number";
	cout << "| " << left << setw(12) << "Pin Code";
	cout << "| " << left << setw(40) << "Client Name";
	cout << "| " << left << setw(12) << "Phone";
	cout << "| " << left << setw(12) << "Balance";
	cout << "\n________________________________________________________________________________________________\n";

	if (vClients.size() == 0)
	{
		cout << "\n\t\t\t\t\tNo Clients Available In the System!" << endl;
	}
	else
	{
		for (stClient& Client : vClients)
		{
			PrintClientRecord(Client);
			TotalBalances += Client.AccountBalance;
			cout << endl;
		}
	}
	cout << "________________________________________________________________________________________________\n";

	cout << "\n\t\t\t\t\tTotal Balances = " << TotalBalances << "$" << endl;
}

void WithdrawByAccountNumber(string AccountNumber, vector<stClient>& vClients)
{
	stClient Client;

	while (!FindClientByAccountNumber(AccountNumber, vClients, Client))
	{
		cout << "\nClient with Account Number [" << AccountNumber << "] does not exist.\n";
		AccountNumber = ReadAccountNumber();
	}

	PrintClientCard(Client);

	double WithdrawAmount;
	char Answer = 'n';
	WithdrawAmount = GetWithdrawAmount(Client);

	AddDepositAmount(AccountNumber, WithdrawAmount, vClients);
}

void ShowWithdrawScreen()
{

	ResetScreen();

	cout << "------------------------------------------\n";
	cout << "\tWithdraw Screen\n";
	cout << "------------------------------------------\n";

	string AccountNumber = ReadAccountNumber();
	vector <stClient> vClients = LoadClientsDataFromFile(ClientsFile);
	WithdrawByAccountNumber(AccountNumber, vClients);
}

void GoBackToTransactionsMenu()
{
	cout << "\n\nPress any key to go back to transactions menu.....";
	system("pause>0");
	ShowTransactionsMenu();
}

void PerformTransaction(enTransaction Transaction)
{
	switch (Transaction)
	{
	case enTransaction::enDeposit:
		ShowDepositScreen();
		GoBackToTransactionsMenu();
		break;

	case enTransaction::enWithdraw:
		ShowWithdrawScreen();
		GoBackToTransactionsMenu();
		break;

	case enTransaction::TotalBalances:
		ShowTotalBalancesScreen();
		GoBackToTransactionsMenu();
		break;

	case enTransaction::ShowTransactions:
		ShowTransactionsScreen();
		GoBackToTransactionsMenu();
		break;

	case enTransaction::MainMenu:
		ShowMainMenu();
		break;
	}
}

short ReadTransactionMenuOption()
{
	short TaskNumber;
	cout << "Choose what do you want to do? [1 to 4]? ";
	cin >> TaskNumber;

	return TaskNumber;
}

void ShowTransactionsMenu()
{
	ResetScreen();

	if (!CheckAccessPermission(enPermission::PTransactions))
	{
		PrintAccessDeniedScreen();
		GobackToMainMenu();
		return;
	}

	cout << "===============================================" << endl;
	cout << "\t\tTransactions Menu Screen" << endl;
	cout << "===============================================" << endl;

	cout << "\t\t[1] Deposit." << endl;
	cout << "\t\t[2] Withdraw." << endl;
	cout << "\t\t[3] Total Balances." << endl;
	cout << "\t\t[4] Show Transactions" << endl;
	cout << "\t\t[5] Main Menu." << endl;

	cout << "===============================================" << endl;

	PerformTransaction((enTransaction)ReadTransactionMenuOption());
}

stUsers ConvertUsersLineToRecord(string Line, string Separator = "#//#")
{
	vector<string> vString = SplitString(Line, Separator);
	stUsers User;

	User.UserName = vString[0];
	User.Password = vString[1];
	User.Permissions = stoi(vString[2]);

	return User;
}

vector<stUsers> LoadUsersDataFromFile(string FileName)
{
	fstream MyFile;
	MyFile.open(FileName, ios::in);

	vector<stUsers> Users;
	string Line;
	stUsers User;

	if (MyFile.is_open())
	{
		while (getline(MyFile, Line))
		{
			User = ConvertUsersLineToRecord(Line);
			Users.push_back(User);
		}

		MyFile.close();
	}

	return Users;
}

bool FindUserByUsernameAndPassword(string UserName, string Password, stUsers& CurrentUser)
{
	vector<stUsers> CurrentUsers = LoadUsersDataFromFile(UsersFile);

	for (stUsers& U : CurrentUsers)
	{
		if (U.UserName == UserName && U.Password == Password)
		{
			CurrentUser = U;
			return true;
		}
	}

	return false;
}

void GoBackToManageUsersMenu()
{
	cout << "\nPress any key to go back to Manage Users Menu......" << endl;
	system("pause>0");
	ShowManageUsersMenu();
}

stClient ConvertFromLineToTransactionRecord(string Line, string Separator = "#//#")
{
	vector<string> vString = SplitString(Line, Separator);
	stClient Client;

	Client.AccountNumber = vString[0];
	Client.ClientName = vString[1];
	Client.Transaction.TransactionNumber = stod(vString[2]);
	Client.Transaction.TransactionType = vString[3];
	Client.Transaction.TransactionAmount = stod(vString[4]);
	Client.AccountBalance = stod(vString[5]);

	return Client;
}

vector<stClient> LoadTransactionsDataFromFile(string FileName)
{
	fstream MyFile;
	MyFile.open(FileName, ios::in);

	vector<stClient> vTransactions;

	stClient Client;
	string Line;

	if (MyFile.is_open())
	{
		while (getline(MyFile, Line))
		{
			Client = ConvertFromLineToTransactionRecord(Line);
			vTransactions.push_back(Client);
		}

		MyFile.close();
	}

	return vTransactions;
}

void ShowTransactionsScreen()
{
	ResetScreen();

	vector<stClient> vTransactions = LoadTransactionsDataFromFile(TransactionsFile);

	cout << "\t\t\t\t\t\tTransactions List (" << vTransactions.size() << ") Transaction(s)." << endl;
	cout << "_____________________________________________________________________________________________________________________________________\n\n";
	cout << "| " << left << setw(15) << "Account Number";
	cout << "| " << left << setw(30) << "Client Name";
	cout << "| " << left << setw(20) << "Transaction Number";
	cout << "| " << left << setw(20) << "Transaction Type";
	cout << "| " << left << setw(20) << "Transaction Amount";
	cout << "| " << left << setw(15) << "Account Balance";
	cout << "\n_____________________________________________________________________________________________________________________________________\n";

	for (stClient& Transaction : vTransactions)
	{
		PrintTransactionRecord(Transaction);
		cout << endl;
	}

	cout << "_____________________________________________________________________________________________________________________________________\n";
}

void PrintUserRecord(stUsers User)
{
	cout << "| " << left << setw(25) << User.UserName;
	cout << "| " << left << setw(25) << User.Password;
	cout << "| " << left << setw(40) << User.Permissions;
}

void ShowUsersListScreen()
{
	ResetScreen();

	vector<stUsers> Users = LoadUsersDataFromFile(UsersFile);

	cout << "\t\t\t\tUsers List (" << Users.size() << ") User(s).\n";
	cout << "___________________________________________________________________________________________\n\n";
	cout << "| " << left << setw(25) << "Username";
	cout << "| " << left << setw(25) << "Password";
	cout << "| " << left << setw(40) << "Permissions";
	cout << "\n___________________________________________________________________________________________\n\n";

	if (Users.size() == 0)
		cout << "\t\t\t\tNo Users Available In the System!" << endl;
	else
	{
		for (stUsers& User : Users)
		{
			PrintUserRecord(User);
			cout << endl;
		}
	}
	cout << "___________________________________________________________________________________________\n\n";
}

bool CheckIfUserExistsByUsername(string UserName, string FileName)
{
	fstream MyFile;
	MyFile.open(FileName, ios::in);

	string Line;
	stUsers User;

	if (MyFile.is_open())
	{
		while (getline(MyFile, Line))
		{
			User = ConvertUsersLineToRecord(Line);

			if (User.UserName == UserName)
			{
				MyFile.close();
				return true;
			}
		}

		MyFile.close();
	}

	return false;
}

int ReadPermissionsToSet()
{
	int Permissions = 0;
	char GiveAccess;

	cout << "\nDo You Want to give full access? Y/N? ";
	cin >> GiveAccess;

	if (toupper(GiveAccess) == 'Y')
	{
		Permissions = -1;
	}
	else
	{
		cout << "\nDo You Want to five access to : \n\n";

		cout << "Show Cient List? Y/N? ";
		cin >> GiveAccess;
		if (toupper(GiveAccess) == 'Y')
		{
			Permissions += enPermission::PShowClientList;
		}

		cout << "\nAdd New Client? Y/N? ";
		cin >> GiveAccess;
		if (toupper(GiveAccess) == 'Y')
		{
			Permissions += enPermission::PAddNewClient;
		}

		cout << "\nDelete Client? Y/N? ";
		cin >> GiveAccess;
		if (toupper(GiveAccess) == 'Y')
		{
			Permissions += enPermission::PDeleteClient;
		}

		cout << "\nUpdate Client? Y/N? ";
		cin >> GiveAccess;
		if (toupper(GiveAccess) == 'Y')
		{
			Permissions += enPermission::PUpdateClient;
		}

		cout << "\nFind Client? Y/N? ";
		cin >> GiveAccess;
		if (toupper(GiveAccess) == 'Y')
		{
			Permissions += enPermission::PFindClient;
		}

		cout << "\nTransactions? Y/N? ";
		cin >> GiveAccess;
		if (toupper(GiveAccess) == 'Y')
		{
			Permissions += enPermission::PTransactions;
		}

		cout << "\nManage Users? Y/N? ";
		cin >> GiveAccess;
		if (toupper(GiveAccess) == 'Y')
		{
			Permissions += enPermission::PManageUsers;
		}
	}

	return Permissions;
}

string ConvertUserRecordToline(stUsers User, string Separator = "#//#")
{
	string Line = "";

	Line += User.UserName + Separator;
	Line += User.Password + Separator;
	Line += to_string(User.Permissions);

	return Line;
}

stUsers ReadNewUser()
{
	stUsers User;

	cout << "Enter Username? ";
	getline(cin >> ws, User.UserName);

	while (CheckIfUserExistsByUsername(User.UserName, UsersFile))
	{
		cout << "\nUser with the Username [" << User.UserName << "] already exists, Enter another Username? ";
		getline(cin >> ws, User.UserName);
	}

	cout << "\nEnter Password? ";
	getline(cin, User.Password);

	User.Permissions = ReadPermissionsToSet();

	return User;
}

void AddNewUser()
{
	stUsers User;
	User = ReadNewUser();
	AddDataLineToFile(UsersFile, ConvertUserRecordToline(User));
}

void AddNewUsers()
{
	char AddMoreusers = 'Y';

	while (toupper(AddMoreusers) == 'Y')
	{
		cout << "Adding New User : \n\n";

		AddNewUser();

		cout << "\nUser Added Successfully, Do you want to add more Users? Y/N? ";
		cin >> AddMoreusers;
	}
}

void AddUsers()
{
	ResetScreen();

	cout << "------------------------------------------\n";
	cout << "\tAdd New User Screen\n";
	cout << "------------------------------------------\n";

	AddNewUsers();
}

string ReadUsername()
{
	string Username;
	cout << "Please Enter Username? ";
	cin >> Username;

	return Username;
}

void PrintUserData(stUsers User)
{
	cout << "\nThe Following are the user details : " << endl;
	cout << "----------------------------------------------------" << endl;
	cout << "Username    : " << User.UserName << endl;
	cout << "Password    : " << User.Password << endl;
	cout << "Permissions : " << User.Permissions << endl;
	cout << "----------------------------------------------------" << endl;
}

bool FindUserByUsername(string Username, vector<stUsers> Users, stUsers& User)
{
	for (stUsers& U : Users)
	{
		if (U.UserName == Username)
		{
			User = U;
			return true;
		}
	}

	return false;
}

bool MarkUserForDelete(string UserName, vector<stUsers>& Users)
{
	for (stUsers& User : Users)
	{
		if (User.UserName == UserName)
		{
			User.MarkForDelete = true;
			return true;
		}
	}

	return false;
}

void SaveUsersDataToFile(vector<stUsers>& Users, string FileName)
{
	fstream MyFile;
	MyFile.open(FileName, ios::out);
	string Line;

	if (MyFile.is_open())
	{
		for (stUsers& User : Users)
		{
			Line = ConvertUserRecordToline(User);

			if (User.MarkForDelete == false)
			{
				MyFile << Line << endl;
			}
		}

		MyFile.close();
	}
}

bool DeleteUserByUsername(string Username, vector<stUsers> Users)
{
	stUsers User;
	char DeleteUser;

	if (FindUserByUsername(Username, Users, User))
	{
		PrintUserData(User);

		cout << "\nAre You sure you want to delete this User? Y/N? ";
		cin >> DeleteUser;

		if (toupper(DeleteUser) == 'Y')
		{
			MarkUserForDelete(Username, Users);
			SaveUsersDataToFile(Users, UsersFile);
		}

		cout << "\nUser Deleted Successfully." << endl;
		return true;
	}
	else
	{
		cout << "\nUser with the Username [" << Username << "] is Not Found!" << endl;
		return false;
	}
}

void DeleteUserScreen()
{
	ResetScreen();

	cout << "------------------------------------------\n";
	cout << "\tDelete User Screen\n";
	cout << "------------------------------------------\n";

	vector<stUsers> Users = LoadUsersDataFromFile(UsersFile);
	string Username = ReadUsername();

	if (Username == "Admin")
	{
		cout << "\nYou Cannot Delete This User." << endl;
		GoBackToManageUsersMenu();
	}
	else
	{
		DeleteUserByUsername(Username, Users);
	}
}

stUsers ChangeUserRecord(string UserName)
{
	stUsers User;

	User.UserName = UserName;

	cout << "\nEnter Password? ";
	getline(cin >> ws, User.Password);

	User.Permissions = ReadPermissionsToSet();

	return User;
}

bool UpdateUserByUsername(string Username, vector<stUsers> Users)
{
	stUsers User;
	char UpdateUser;

	if (FindUserByUsername(Username, Users, User))
	{
		PrintUserData(User);

		cout << "\nAre you sure you want to update this User? Y/N? ";
		cin >> UpdateUser;

		if (toupper(UpdateUser) == 'Y')
		{
			for (stUsers& User : Users)
			{
				if (User.UserName == Username)
				{
					User = ChangeUserRecord(Username);
				}
			}

			SaveUsersDataToFile(Users, UsersFile);
			cout << "\nUser Updated Successfully." << endl;
			return true;
		}
	}
	else
	{
		cout << "\nUser with the Username [" << Username << "] is Not Found!" << endl;
		return false;
	}
}

void UpdateUserScreen()
{
	ResetScreen();

	cout << "------------------------------------------\n";
	cout << "\tUpdate User Screen\n";
	cout << "------------------------------------------\n";
	vector<stUsers> Users = LoadUsersDataFromFile(UsersFile);
	string Username = ReadUsername();
	UpdateUserByUsername(Username, Users);
}

void FindUserScreen()
{
	ResetScreen();

	cout << "------------------------------------------\n";
	cout << "\tFind User Screen\n";
	cout << "------------------------------------------\n";

	vector<stUsers> Users = LoadUsersDataFromFile(UsersFile);
	string Username = ReadUsername();
	stUsers User;

	if (FindUserByUsername(Username, Users, User))
	{
		PrintUserData(User);
	}
	else
	{
		cout << "\nUser with the Username [" << Username << "] is Not Found!" << endl;
	}
}

void PerformManageUsersMenuOption(enManageusers ManageUsersOption)
{
	switch (ManageUsersOption)
	{
	case enManageusers::enListUsers:
		ShowUsersListScreen();
		GoBackToManageUsersMenu();
		break;

	case enManageusers::enAddNewUser:
		AddUsers();
		GoBackToManageUsersMenu();
		break;

	case enManageusers::enDeleteUser:
		DeleteUserScreen();
		GoBackToManageUsersMenu();
		break;

	case enManageusers::enUpdateUser:
		UpdateUserScreen();
		GoBackToManageUsersMenu();
		break;

	case enManageusers::enFindUser:
		FindUserScreen();
		GoBackToManageUsersMenu();
		break;

	case enManageusers::enMainMenu:
		ShowMainMenu();
		break;
	}
}

short ReadManageUsersMenuOption()
{
	short TaskNumber;
	cout << "Choose what do you want to do? [1 to 6]? ";
	cin >> TaskNumber;

	return TaskNumber;
}

void ShowManageUsersMenu()
{
	ResetScreen();

	if (!CheckAccessPermission(enPermission::PManageUsers))
	{
		PrintAccessDeniedScreen();
		GobackToMainMenu();
		return;
	}

	cout << "===============================================" << endl;
	cout << "\t\tManage Users Menu Screen" << endl;
	cout << "===============================================" << endl;
	cout << "\t\t[1] List Users." << endl;
	cout << "\t\t[2] Add New User." << endl;
	cout << "\t\t[3] Delete User." << endl;
	cout << "\t\t[4] Update User." << endl;
	cout << "\t\t[5] Find User." << endl;
	cout << "\t\t[6] Main Menu." << endl;
	cout << "===============================================" << endl;

	PerformManageUsersMenuOption((enManageusers)ReadManageUsersMenuOption());
}

bool LoadUserInfo(string Username, string Password)
{
	if (FindUserByUsernameAndPassword(Username, Password, CurrentUser))
		return true;
	else
		return false;
}

void Login()
{
	bool LoginFailed = false;
	string Username, Password;

	do {

		ResetScreen();
		cout << "------------------------------------------\n";
		cout << "\tLogin Screen\n";
		cout << "------------------------------------------\n";

		if (LoginFailed)
		{
			cout << "Invalid Username/Password!" << endl;
		}

		cout << "Please Enter Username? ";
		cin >> Username;

		cout << "Please Enter Password? ";
		cin >> Password;

		LoginFailed = !LoadUserInfo(Username, Password);

	} while (LoginFailed);

	ShowMainMenu();
}

int main()
{
	srand((unsigned)time(NULL));

	Login();

	return 0;
}