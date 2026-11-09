#include <algorithm>
#include <cctype>
#include <ranges>
#include <string>
#include <unordered_map>

#include <app/console_database.hpp>

namespace
{

bool iequals(std::string_view lhs, std::string_view rhs)
{
    return std::ranges::equal(lhs, rhs, [](unsigned char left, unsigned char right)
                              { return std::tolower(left) == std::tolower(right); });
}

const std::unordered_map<std::string, Command> commandMap = {
    {"createtable", Command::CreateTable}, {"1", Command::CreateTable},
    {"add", Command::Add}, {"2", Command::Add},
    {"delete", Command::Delete}, {"3", Command::Delete},
    {"update", Command::Update}, {"4", Command::Update},
    {"isuser", Command::IsUser}, {"5", Command::IsUser},
    {"printusers", Command::PrintUsers}, {"6", Command::PrintUsers},
    {"getuser", Command::GetUser}, {"7", Command::GetUser},
    {"addusersmassive", Command::AddMassive}, {"8", Command::AddMassive},
    {"searchusersname", Command::SearchName}, {"9", Command::SearchName},
    {"printusrpagename", Command::PrintPageName}, {"10", Command::PrintPageName},
    {"printusrpageage", Command::PrintPageAge}, {"11", Command::PrintPageAge},
    {"exit", Command::Exit}, {"-1", Command::Exit}};

} // namespace

Command CommandParser::parse(std::string_view request) const
{
    const auto it = commandMap.find(std::string(request));
    if (it != commandMap.end())
        return it->second;

    for (const auto &[key, command] : commandMap)
    {
        if (iequals(request, key))
            return command;
    }

    return Command::Unknown;
}

ConsoleIO::ConsoleIO(std::istream &input, std::ostream &output) noexcept
    : _input(&input), _output(&output)
{
}

ConsoleIO::ConsoleIO() noexcept : ConsoleIO(std::cin, std::cout)
{
}

std::string ConsoleIO::readCommand()
{
    printInstruction();
    return readValue<std::string>("\033[32m\033[1mInput command request: \033[0m",
                                  "Incorrect input format");
}

void ConsoleIO::printInstruction()
{
    writeLine("\033[32m**************************************************************************************************\033[0m");
    writeLine("1.) Input \033[31m\"createtable\"\033[0m or \033[31m\"CREATETABLE\"\033[0m or \033[31m\"1\"\033[0m for \033[33mCreate Table\033[0m ");
    writeLine("2.) Input \033[31m\"add\"\033[0m or \033[31m\"ADD\"\033[0m or \033[31m\"2\"\033[0m for \033[33m addUser\033[0m ");
    writeLine("3.) Input \033[31m\"delete\"\033[0m or \033[31m\"DELETE\"\033[0m or \033[31m\"3\"\033[0m for \033[33m delete User\033[0m ");
    writeLine("4.) Input \033[31m\"update\"\033[0m or \033[31m\"UPDATE\"\033[0m or \033[31m\"4\"\033[0m for \033[33m update User Age\033[0m ");
    writeLine("5.) Input \033[31m\"isUser\"\033[0m or \033[31m\"ISUSER\"\033[0m or \033[31m\"5\"\033[0m for \033[33m is User Exists\033[0m ");
    writeLine("6.) Input \033[31m\"printUsers\"\033[0m or \033[31m\"PRINTUSERS\"\033[0m or \033[31m\"6\"\033[0m for \033[33m print All Users\033[0m ");
    writeLine("7.) Input \033[31m\"getUser\"\033[0m or \033[31m\"GETUSER\"\033[0m or \033[31m\"7\"\033[0m for \033[33m get User ById\033[0m ");
    writeLine("8.) Input \033[31m\"addUsersMassive\"\033[0m or \033[31m\"ADDUSERSMASSIVE\"\033[0m or \033[31m\"8\"\033[0m for \033[33m add Users Massive\033[0m ");
    writeLine("9.) Input \033[31m\"searchUsersName\"\033[0m or \033[31m\"SEARCHUSERSNAME\"\033[0m or \033[31m\"9\"\033[0m for \033[33m search Users by Name\033[0m ");
    writeLine("10.) Input \033[31m\"printUsrPageName\"\033[0m or \033[31m\"PRINTUSRPAGENAME\"\033[0m or \033[31m\"10\"\033[0m for \033[33m print Users Paged Name\033[0m ");
    writeLine("11.) Input \033[31m\"printUsrPageAge\"\033[0m or \033[31m\"PRINTUSRPAGEAGE\"\033[0m or \033[31m\"11\"\033[0m for \033[33m print Users Paged Age\033[0m ");
    writeLine("\n-1.) Input \033[31m\"exit\"\033[0m or \033[31m\"EXIT\"\033[0m or \033[31m\"-1\"\033[0m for \033[33m Exit out dataBase\033[0m ");
    writeLine("\033[32m**************************************************************************************************\033[0m");
}

void DatabaseCommandHandler::execute(Command command, DataCore::IDatabase &db, ConsoleIO &console) const
{
    switch (command)
    {
    case Command::CreateTable:
        db.createSQLTabble();
        break;
    case Command::Add:
    {
        const auto name = console.readValue<std::string>("Enter your name and age:", "Incorrect input your name and age");
        const auto age = console.readValue<int>("", "Incorrect input your name and age");
        const long long newId = db.addUser(name, age);
        console.writeLine("The system has registered you under number:", newId);
        break;
    }
    case Command::Delete:
    {
        const int id = console.readValue<int>("Enter ID to delete:", "Incorrect input ID to delete");
        db.deleteUser(id);
        break;
    }
    case Command::Update:
    {
        const auto name = console.readValue<std::string>("Enter your username and new age:", "Incorrect username or new age");
        const auto age = console.readValue<int>("", "Incorrect username or new age");
        db.updateUserAge(name, age);
        break;
    }
    case Command::IsUser:
    {
        const auto name = console.readValue<std::string>("Enter a name to verify:", "Incorrect name to verify");
        if (db.isUserExists(name))
            console.writeLine("User [", name, "] is in the database.");
        else
            console.writeLine("User [", name, "] not found.");
        break;
    }
    case Command::PrintUsers:
        db.printAllUsers();
        break;
    case Command::GetUser:
    {
        const int id = console.readValue<int>("Enter a id for get User:", "Incorrect id for get User");
        const auto user = db.getUserById(id);
        if (user.has_value())
            console.writeLine("[Found] ID: ", id, " | Name: ", user->first, " | Age: ", user->second);
        else
            console.writeLine("[Error] User with ID ", id, " not found.");
        break;
    }
    case Command::AddMassive:
    {
        const int count = console.readValue<int>("Enter how many users you want to add: ", "Incorrect quantity users");
        if (count < 0)
            throw std::ios_base::failure("Incorrect quantity users");
        std::vector<std::pair<std::string, int>> users;
        users.reserve(static_cast<std::size_t>(count));
        for (int i = 0; i < count; ++i)
        {
            const auto name = console.readValue<std::string>("Enter name and age (left " + std::to_string(count - i) + "): ",
                                                             "Incorrect input your name and age");
            const auto age = console.readValue<int>("", "Incorrect input your name and age");
            users.emplace_back(name, age);
        }
        db.addUsersMassive(users);
        break;
    }
    case Command::SearchName:
    {
        const auto query = console.readValue<std::string>("Enter part of the name to search:", "Incorrect part of the name to search");
        db.searchUsersAdvanced(query, [&console](const DataCore::User &user)
                               { console.writeLine("ID: ", user.id, " | Name: ", user.name, " | Age: ", user.age); });
        break;
    }
    case Command::PrintPageName:
    case Command::PrintPageAge:
    {
        const int size = console.readValue<int>("How many people are on the page?", "Incorrect page size");
        const int page = console.readValue<int>("Which page should I open?", "Incorrect page number");
        if (command == Command::PrintPageName)
            db.printUsersPagedName(size, page);
        else
            db.printUsersPagedAge(size, page);
        break;
    }
    case Command::Exit:
        console.writeLine("Exiting the program...");
        break;
    default:
        console.writeLine("The command is not recognized or has not yet been implemented.");
        break;
    }
}
