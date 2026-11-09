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

    class SQLiteConnection
    {
    private:
        sqlite3 *_handle = nullptr;

    public:
        explicit SQLiteConnection(std::string_view fileName);
        ~SQLiteConnection();

        SQLiteConnection(const SQLiteConnection &) = delete;
        SQLiteConnection &operator=(const SQLiteConnection &) = delete;

        operator sqlite3 *() const noexcept { return _handle; }
    };

    class IDatabase
    {
    public:
        virtual ~IDatabase() = default;

        virtual void createSQLTabble() = 0;
        virtual long long addUser(std::string_view name, int age) = 0;
        virtual void deleteUser(int id) = 0;
        virtual void updateUserAge(std::string_view name, int newAge) = 0;
        virtual bool isUserExists(std::string_view name) = 0;
        virtual void printAllUsers() = 0;
        virtual std::optional<std::pair<std::string, int>> getUserById(int id) = 0;

        virtual void addUsersMassive(const std::vector<std::pair<std::string, int>> &users) = 0;
        virtual void searchUsersAdvanced(std::string_view partialName, std::function<void(const User &)> callback) = 0;
        virtual void printUsersPagedName(int pageSize, int pageNumber) = 0;
        virtual void printUsersPagedAge(int pageSize, int pageNumber) = 0;
    };

    class Database : public IDatabase
    {
    private:
        SQLiteConnection DB;
        char *errorMessage = nullptr;

    protected:
        explicit Database(std::string_view value = "example.db");

    public:
        ~Database() override = default;

        Database(const Database &) = delete;
        Database &operator=(const Database &) = delete;

        static Database &GetInstance(std::string_view value = "example.db");

        void createSQLTabble() override;
        long long addUser(std::string_view name, int age) override;
        void deleteUser(int id) override;
        void updateUserAge(std::string_view name, int newAge) override;
        bool isUserExists(std::string_view name) override;
        void printAllUsers() override;
        std::optional<std::pair<std::string, int>> getUserById(int id) override;

        void addUsersMassive(const std::vector<std::pair<std::string, int>> &users) override;
        void searchUsersAdvanced(std::string_view partialName, std::function<void(const User &)> callback) override;
        void printUsersPagedName(int pageSize, int pageNumber) override;
        void printUsersPagedAge(int pageSize, int pageNumber) override;
    };
}