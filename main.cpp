#include <app/console_database.hpp>
#include <database/database.hpp>

int main()
{
    try
    {
        auto &db = DataCore::Database::GetInstance();
        ConsoleIO console;
        CommandParser parser;
        DatabaseCommandHandler handler;
        Command currentCmd = Command::Unknown;
        while (currentCmd != Command::Exit)
        {
            currentCmd = parser.parse(console.readCommand());
            handler.execute(currentCmd, db, console);
        }
    }
    catch (const DataCore::ExceptDataBase &e)
    {
        std::cerr << "Database Exception: " << e.what() << std::endl;
    }
    return 0;
}