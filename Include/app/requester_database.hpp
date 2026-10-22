#pragma once

#include <string_view>

#include <database/database.hpp>

enum class Command
{
    CreateTable = 1,
    Add,
    Delete,
    Update,
    IsUser,
    PrintUsers,
    GetUser,
    AddMassive,
    SearchName,
    PrintPageName,
    PrintPageAge,
    Exit = -1,
    Unknown = 100
};

class reqesterDatabase
{
private:
    struct string_hash
    {
        using is_transparent = void; // Включает гетерогенный поиск
        size_t operator()(std::string_view txt) const
        {
            return std::hash<std::string_view>{}(txt);
        }
    };
    static const std::unordered_map<std::string, Command, string_hash, std::equal_to<>> commandMap;

public:
    Command request(std::string_view reqest);
    std::string printInstruction();
    void dateRequest(const Command command, DataCore::Database &db);
};
