#pragma once

#include <iostream>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

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

class CommandParser
{
public:
    Command parse(std::string_view request) const;
};

class ConsoleIO
{
private:
    std::istream *_input;
    std::ostream *_output;

public:
    ConsoleIO(std::istream &input, std::ostream &output) noexcept;
    ConsoleIO() noexcept;

    std::string readCommand();
    void printInstruction();

    template <typename T>
    T readValue(std::string_view prompt, std::string_view errorMessage)
    {
        T value{};
        *_output << prompt;
        if (!(*_input >> value))
            throw std::ios_base::failure(std::string(errorMessage));
        return value;
    }

    template <typename... Values>
    void write(Values &&...values)
    {
        ((*_output << std::forward<Values>(values)), ...);
    }

    template <typename... Values>
    void writeLine(Values &&...values)
    {
        write(std::forward<Values>(values)...);
        *_output << std::endl;
    }
};

class DatabaseCommandHandler
{
public:
    void execute(Command command, DataCore::IDatabase &database, ConsoleIO &console) const;
};
