#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <iostream>
#include <ranges>

#include <app/requester_database.hpp>
#include <database/database.hpp>

const std::unordered_map<std::string, Command, reqesterDatabase::string_hash, std::equal_to<>> reqesterDatabase::commandMap = {
    {"createtable", Command::CreateTable}, {"1", Command::CreateTable}, {"add", Command::Add}, {"2", Command::Add}, {"delete", Command::Delete}, {"3", Command::Delete}, {"update", Command::Update}, {"4", Command::Update}, {"isuser", Command::IsUser}, {"5", Command::IsUser}, {"printusers", Command::PrintUsers}, {"6", Command::PrintUsers}, {"getuser", Command::GetUser}, {"7", Command::GetUser}, {"addusersmassive", Command::AddMassive}, {"8", Command::AddMassive}, {"searchusersname", Command::SearchName}, {"9", Command::SearchName}, {"printusrpagename", Command::PrintPageName}, {"10", Command::PrintPageName}, {"printusrpageage", Command::PrintPageAge}, {"11", Command::PrintPageAge}, {"exit", Command::Exit}, {"-1", Command::Exit}};

bool iequals(std::string_view lhs, std::string_view rhs)
{
    return (
        std::ranges::equal(lhs, rhs, [](unsigned char l, unsigned char r)
                           { return std::tolower(l) == std::tolower(r); }));
}

Command reqesterDatabase::request(std::string_view reqest)
{
    auto it = commandMap.find(reqest);
    if (it != commandMap.end())
    {
        return it->second;
    }

    // 2. Если точное совпадение не найдено (ввели "ADD"), делаем чуть более дорогой поиск без учета регистра
    for (const auto &[key, command] : commandMap)
    {
        if (iequals(reqest, key))
        {
            return command;
        }
    }

    return Command::Unknown;
}
std::string reqesterDatabase::printInstruction()
{
    std::string reqest;
    std::cout << "\033[32m**************************************************************************************************\033[0m\n";
    std::cout << "1.) Input \033[31m\"createtable\"\033[0m or \033[31m\"CREATETABLE\"\033[0m or \033[31m\"1\"\033[0m for \033[33mCreate Table\033[0m \n";
    std::cout << "2.) Input \033[31m\"add\"\033[0m or \033[31m\"ADD\"\033[0m or \033[31m\"2\"\033[0m for \033[33m addUser\033[0m \n";
    std::cout << "3.) Input \033[31m\"delete\"\033[0m or \033[31m\"DELETE\"\033[0m or \033[31m\"3\"\033[0m for \033[33m delete User\033[0m \n";
    std::cout << "4.) Input \033[31m\"update\"\033[0m or \033[31m\"UPDATE\"\033[0m or \033[31m\"4\"\033[0m for \033[33m update User Age\033[0m \n";
    std::cout << "5.) Input \033[31m\"isUser\"\033[0m or \033[31m\"ISUSER\"\033[0m or \033[31m\"5\"\033[0m for \033[33m is User Exists\033[0m \n";
    std::cout << "6.) Input \033[31m\"printUsers\"\033[0m or \033[31m\"PRINTUSERS\"\033[0m or \033[31m\"6\"\033[0m for \033[33m print All Users\033[0m \n";
    std::cout << "7.) Input \033[31m\"getUser\"\033[0m or \033[31m\"GETUSER\"\033[0m or \033[31m\"7\"\033[0m for \033[33m get User ById\033[0m \n";
    std::cout << "8.) Input \033[31m\"addUsersMassive\"\033[0m or \033[31m\"ADDUSERSMASSIVE\"\033[0m or \033[31m\"8\"\033[0m for \033[33m add Users Massive\033[0m \n";
    std::cout << "9.) Input \033[31m\"searchUsersName\"\033[0m or \033[31m\"SEARCHUSERSNAME\"\033[0m or \033[31m\"9\"\033[0m for \033[33m search Users by Name\033[0m \n";
    std::cout << "10.) Input \033[31m\"printUsrPageName\"\033[0m or \033[31m\"PRINTUSRPAGENAME\"\033[0m or \033[31m\"10\"\033[0m for \033[33m print Users Paged Name\033[0m \n";
    std::cout << "11.) Input \033[31m\"printUsrPageAge\"\033[0m or \033[31m\"PRINTUSRPAGEAGE\"\033[0m or \033[31m\"11\"\033[0m for \033[33m print Users Paged Age\033[0m \n";

    std::cout << "\n-1.) Input \033[31m\"exit\"\033[0m or \033[31m\"EXIT\"\033[0m or \033[31m\"-1\"\033[0m for \033[33m Exit out dataBase\033[0m \n";
    std::cout << "\033[32m**************************************************************************************************\033[0m\n";

    std::cout << "\033[32m\033[1mInput command request: \033[0m";
    if (std::cin >> reqest)
        return reqest;
    else
        throw std::ios_base::failure("Incorrect input format");
}
void reqesterDatabase::dateRequest(const Command command, DataCore::Database &db)
{
    switch (command)
    {
    case Command::CreateTable:
        db.createSQLTabble();
        break;
    case Command::Add:
    { // ADD
        std::string name;
        int age;
        std::cout << "Enter your name and age:";
        if (std::cin >> name >> age)
        {
            long long newId = db.addUser(name, age);
            std::cout << "The system has registered you under number:" << newId << std::endl;
        }
        else
            throw std::ios_base::failure("Incorrect input your name and age");
        break;
    }
    case Command::Delete:
    { // DELETE
        int id;
        std::cout << "Enter ID to delete:";
        if (std::cin >> id)
            db.deleteUser(id);
        else
            throw std::ios_base::failure("Incorrect input ID to delete");
        break;
    }
    case Command::Update:
    { // UPDATE
        std::string name;
        int newAge;
        std::cout << "Enter your username and new age:";
        if (std::cin >> name >> newAge)
            db.updateUserAge(name, newAge);
        else
            throw std::ios_base::failure("Incorrect username or new age");
        break;
    }
    case Command::IsUser:
    { // IS USER EXISTS
        std::string name;
        std::cout << "Enter a name to verify:";
        if (std::cin >> name)
        {
            if (db.isUserExists(name))
            {
                std::cout << "User [" << name << "] is in the database." << std::endl;
            }
            else
            {
                std::cout << "User [" << name << "] not found." << std::endl;
            }
        }
        else
            throw std::ios_base::failure("Incorrect name to verify");
        break;
    }
    case Command::PrintUsers:
    { // PRINT ALL
        db.printAllUsers();
        break;
    }
    case Command::GetUser:
    {
        int id;
        std::cout << "Enter a id for get User:";
        if (std::cin >> id)
        {
            auto user = db.getUserById(id);
            if (user.has_value())
            {
                std::cout << "[Found] ID: " << id << " | Name: " << user.value().first << " | Age: " << user.value().second << std::endl;
            }
            else
                std::cout << "[Error] User with ID " << id << " not found." << std::endl;
        }
        else
            throw std::ios_base::failure("Incorrect id for get User");
        break;
    }
    case Command::AddMassive:
    {
        int countUsers = 0;
        std::cout << "Enter how many users you want to add: ";
        if (std::cin >> countUsers)
        {
            std::vector<std::pair<std::string, int>> users(countUsers);
            std::ranges::generate(users, [left = countUsers]() mutable
                                  {
                    std::string name; int age;
                    std::cout << "Enter name and age (left " << left-- << "): ";
                    if (!(std::cin >> name >> age)) throw std::runtime_error("Incorrect input your name and age");
                    return std::make_pair(name, age); });
            /*std::ranges::transform(users, std::begin(users),[left = countUsers](auto) mutable ->std::pair<std::string, int> {
                if (left > 0) {
                    std::string name; int age;
                    std::cout << "Enter your name and age (left " << left << "): ";
                    if (std::cin >> name >> age) {
                        left--;
                        return std::make_pair(name, age);
                    }
                    else
                        throw std::ios_base::failure("Incorrect input your name and age");
                }
            });*/
            db.addUsersMassive(users);
        }
        else
            throw std::ios_base::failure("Incorrect quantity users");
        break;
    }
    case Command::SearchName:
    {
        std::string query;
        std::cout << "Enter part of the name to search:";
        if (std::cin >> query)
        {
            db.searchUsersAdvanced(query, [](const DataCore::User &u)
                                   { std::cout << "ID: " << u.id << " | Name: " << u.name << " | Age: " << u.age << std::endl; });
        }
        else
            throw std::ios_base::failure("Incorrect part of the name to search");
        break;
    }
    case Command::PrintPageName:
    { // Вывод страницами
        int size, page;
        std::cout << "How many people are on the page?";
        std::cin >> size;
        std::cout << "Which page should I open?";
        std::cin >> page;
        db.printUsersPagedName(size, page);
        break;
    }
    case Command::PrintPageAge:
    { // Вывод страницами
        int size, page;
        std::cout << "How many people are on the page?";
        std::cin >> size;
        std::cout << "Which page should I open?";
        std::cin >> page;
        db.printUsersPagedAge(size, page);
        break;
    }
    case Command::Exit:
    {
        std::cout << "Exiting the program..." << std::endl;
        break;
    }
    default:
        std::cout << "The command is not recognized or has not yet been implemented." << std::endl;
        break;
    }
}