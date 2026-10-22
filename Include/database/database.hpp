#pragma once

#include <sqlite3.h>
#include <functional>
#include <optional>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <database/user.hpp>

namespace DataCore
{

    class ExceptDataBase : public std::runtime_error
    {
    public:
        explicit ExceptDataBase(std::string_view message) : std::runtime_error(std::string(message)) {}
    };

    class Database
    {
    private:
        inline static sqlite3 *DB = nullptr;
        std::string _fileDataBaseName;
        char *errorMessage = nullptr;

    protected:
        Database(std::string_view value = "example.db");
        ~Database();

    public:
        Database(Database &other) = delete;
        void operator=(const Database &) = delete;

        static Database &GetInstance(std::string_view value = "example.db");

        void createSQLTabble();
        long long addUser(std::string_view name, int age);
        void deleteUser(int id);
        void updateUserAge(std::string_view name, int newAge);
        bool isUserExists(std::string_view name);
        void printAllUsers();
        std::optional<std::pair<std::string, int>> getUserById(int id);

        void addUsersMassive(const std::vector<std::pair<std::string, int>> &users);
        void searchUsersAdvanced(std::string_view partialName, std::function<void(const User &)> callback);
        void printUsersPagedName(int pageSize, int pageNumber);
        void printUsersPagedAge(int pageSize, int pageNumber);
    };
}