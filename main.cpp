#include <iostream>
#include <sqlite3.h>
#include <string>
#include <mutex>
#include <exception>
#include <optional>
#include <functional>
#include <vector>
#include <iterator>

#include <ranges>
#include <algorithm>

#include <algorithms/min_element.hpp>
#include <app/requester_database.hpp>
#include <database/database.hpp>

int main()
{

    try
    {
        std::string name;
        int age;
        /*std::cout << "Input Name and Age to add: ";
        if (!(std::cin >> name >> age)) return 0;*/

        DataCore::Database &db = DataCore::Database::GetInstance();
        reqesterDatabase reqDb{};

        Command currentCmd = Command::Unknown;
        while (currentCmd != Command::Exit)
        {
            std::string input = reqDb.printInstruction();
            currentCmd = reqDb.request(input);
            reqDb.dateRequest(currentCmd, db);
        }
        // addUsersMassive(all_users | std::views::filter([](const auto& u) { return u.second >= 18; }));
    }
    catch (const DataCore::ExceptDataBase &e)
    {
        std::cerr << "Database Exception: " << e.what() << std::endl;
    }
    return 0;
}