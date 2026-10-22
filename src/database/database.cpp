#include <iostream>
#include <string>
#include <utility>

#include <database/database.hpp>

DataCore::Database::Database(std::string_view value) : _fileDataBaseName(value)
{
    int rc = sqlite3_open(_fileDataBaseName.c_str(), &DB);
    if (rc != SQLITE_OK)
    {
        // 1. Сначала сохраняем текст ошибки, пока DB еще жив
        std::string errorMsg = "Error opening database: " + _fileDataBaseName +
                               " | Error: " + sqlite3_errmsg(DB);
        // 2. Закрываем дескриптор (SQLite требует этого даже при ошибке открытия)
        if (DB)
        {
            sqlite3_close(DB);
            DB = nullptr;
        }
        // 3. Выбрасываем исключение
        throw ExceptDataBase(errorMsg);
    }
}
DataCore::Database::~Database()
{
    if (DB)
        sqlite3_close(DB);
}

DataCore::Database &DataCore::Database::GetInstance(std::string_view value)
{
    // Статический объект создается один раз при первом вызове
    static Database instance(value);
    return instance;
}

void DataCore::Database::createSQLTabble()
{
    std::string sql = "CREATE TABLE IF NOT EXISTS USERS("
                      "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                      "NAME TEXT NOT NULL, "
                      "AGE INT NOT NULL);";
    // 3. Выполнение запроса
    int exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &errorMessage);
    if (exit != SQLITE_OK)
    {
        std::string errorMsg = "Error create SQL Tabble: " + std::string(errorMessage);
        sqlite3_free(errorMessage);
        throw ExceptDataBase(errorMsg);
    }
    else
    {
        std::cout << "The table has been created or already exists." << std::endl;
    }
}
long long DataCore::Database::addUser(std::string_view name, int age)
{
    sqlite3_stmt *stmt;
    std::string sql = "INSERT INTO USERS (NAME, AGE) VALUES (?, ?);";
    long long lastId = -1;

    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, name.empty() ? "" : name.data(), static_cast<int>(name.size()), SQLITE_TRANSIENT);
        sqlite3_bind_int(stmt, 2, age);

        if (sqlite3_step(stmt) == SQLITE_DONE)
        {
            // Вот эта магическая функция:
            lastId = sqlite3_last_insert_rowid(DB);
            std::cout << "[Success] Added with user ID:" << lastId << std::endl;
        }
        sqlite3_finalize(stmt);
    }
    return lastId; // Возвращаем ID программе для дальнейшей работы
}
void DataCore::Database::deleteUser(int id)
{
    sqlite3_stmt *stmt;
    std::string sql = "DELETE FROM USERS WHERE ID = ?;";

    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, id);

        if (sqlite3_step(stmt) == SQLITE_DONE)
        {
            // sqlite3_changes возвращает количество измененных строк
            if (sqlite3_changes(DB) > 0)
            {
                std::cout << "[Success] User with ID " << id << " deleted." << std::endl;
            }
            else
            {
                std::cout << "[Warning] User with ID " << id << " not found." << std::endl;
            }
        }
        sqlite3_finalize(stmt);
    }
}
void DataCore::Database::updateUserAge(std::string_view name, int newAge)
{
    sqlite3_stmt *stmt;
    // Сначала идет то, ЧТО меняем, потом У КОГО меняем
    std::string sql = "UPDATE USERS SET AGE = ? WHERE NAME = ?;";

    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        // Привязываем параметры по порядку знаков '?'
        sqlite3_bind_int(stmt, 1, newAge);
        sqlite3_bind_text(stmt, 2, name.empty() ? "" : name.data(), static_cast<int>(name.size()), SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_DONE)
        {
            if (sqlite3_changes(DB) > 0)
            {
                std::cout << "[Success] Data " << name << " updated." << std::endl;
            }
            else
            {
                std::cout << "[Warning] User " << name << " not found." << std::endl;
            }
        }
        sqlite3_finalize(stmt);
    }
}
bool DataCore::Database::isUserExists(std::string_view name)
{
    sqlite3_stmt *stmt;
    std::string sql = "SELECT COUNT(*) FROM USERS WHERE NAME = ?;";
    bool exists = false;

    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_text(stmt, 1, name.empty() ? "" : name.data(), static_cast<int>(name.size()), SQLITE_TRANSIENT);

        // Выполняем шаг. Для COUNT(*) всегда возвращается одна строка
        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            int count = sqlite3_column_int(stmt, 0); // Получаем число из первой колонки
            exists = (count > 0);
        }
        sqlite3_finalize(stmt);
    }
    return exists;
}
void DataCore::Database::printAllUsers()
{
    sqlite3_stmt *stmt;
    std::string selectQuery = "SELECT ID, NAME, AGE FROM USERS;";
    sqlite3_prepare_v2(DB, selectQuery.c_str(), -1, &stmt, NULL); // sqlite3_prepare_v2: Компилирует SQL-запрос во внутренний байт-код SQLite.

    std::cout << "\nList of users in the database:" << std::endl;

    // 5.2. Проходим по строкам в цикле
    while (sqlite3_step(stmt) == SQLITE_ROW)
    { // sqlite3_step: Выполняет один шаг запроса. Если он возвращает SQLITE_ROW, значит, найдена строка с данными.
        int id = sqlite3_column_int(stmt, 0);
        const unsigned char *name = sqlite3_column_text(stmt, 1); // sqlite3_column_*: Извлекает данные из конкретной колонки текущей строки (считаем с нуля).
        int age = sqlite3_column_int(stmt, 2);
        std::cout << "ID: " << id << " | Name: " << name << " | Age: " << age << std::endl;
    }
    // 5.3. Очищаем память за запросом
    sqlite3_finalize(stmt); // sqlite3_finalize: Обязательно удаляет объект запроса, чтобы не было утечки памяти.
}
std::optional<std::pair<std::string, int>> DataCore::Database::getUserById(int id)
{
    sqlite3_stmt *stmt = nullptr;
    std::string sql = "SELECT NAME, AGE FROM USERS WHERE ID = ?;";

    // 1. Создаем объект, который может быть пустым
    std::optional<std::pair<std::string, int>> result = std::nullopt;

    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, id);

        if (sqlite3_step(stmt) == SQLITE_ROW)
        {
            const unsigned char *namePtr = sqlite3_column_text(stmt, 0);
            int age = sqlite3_column_int(stmt, 1);

            std::string name = namePtr ? reinterpret_cast<const char *>(namePtr) : "Unknown";
            // Записываем результат в optional
            result = std::make_pair(name, age);
        }
    }
    // 2. Всегда финализируем в самом конце, один раз
    if (stmt)
        sqlite3_finalize(stmt);

    return result;
}

void DataCore::Database::addUsersMassive(const std::vector<std::pair<std::string, int>> &users)
{
    char *errorMsg = nullptr;

    // 1. Открываем транзакцию
    sqlite3_exec(DB, "BEGIN TRANSACTION;", nullptr, nullptr, &errorMsg);

    try
    {
        sqlite3_stmt *stmt;
        std::string sql = "INSERT INTO USERS (NAME, AGE) VALUES (?, ?);";
        sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, nullptr);

        // addUsersMassive(all_users | std::views::filter([](const auto& u) { return u.second >= 18; }));
        for (const auto &user : users)
        {
            sqlite3_bind_text(stmt, 1, user.first.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 2, user.second);
            sqlite3_step(stmt);
            sqlite3_reset(stmt);
        }
        /*/for (const auto& user : users) {
            sqlite3_bind_text(stmt, 1, user.first.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 2, user.second);

            sqlite3_step(stmt);
            sqlite3_reset(stmt); // Очищаем stmt для следующей итерации, но не удаляем его
        }*/
        sqlite3_finalize(stmt);

        // 2. Подтверждаем изменения
        sqlite3_exec(DB, "COMMIT;", nullptr, nullptr, &errorMsg);
        std::cout << "[Transaction] Successfully added " << users.size() << " records." << std::endl;
    }
    catch (...)
    {
        // 3. Если что-то пошло не так, отменяем всё
        sqlite3_exec(DB, "ROLLBACK;", nullptr, nullptr, &errorMsg);
        std::cerr << "[Error] Transaction cancelled." << std::endl;
    }
}
void DataCore::Database::searchUsersAdvanced(std::string_view partialName, std::function<void(const User &)> callback)
{
    sqlite3_stmt *stmt;
    std::string sql = "SELECT ID, NAME, AGE FROM USERS WHERE NAME LIKE ?;";

    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        std::string query = "%";
        query.append(partialName);
        query.push_back('%');
        sqlite3_bind_text(stmt, 1, query.c_str(), -1, SQLITE_TRANSIENT);

        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            // Создаем объект User прямо "на лету"
            User u;
            u.id = sqlite3_column_int(stmt, 0);
            const char *namePtr = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            u.name = namePtr ? namePtr : "";
            u.age = sqlite3_column_int(stmt, 2);

            // Вызываем нашу функцию-обработчик для каждого найденного юзера
            callback(u);
        }
        sqlite3_finalize(stmt);
    }
}
void DataCore::Database::printUsersPagedName(int pageSize, int pageNumber)
{
    sqlite3_stmt *stmt;
    // Считаем пропуск: если 2-я страница при размере 5, нужно пропустить 5 записей
    int offset = (pageNumber - 1) * pageSize;

    // Сортируем по имени (ASC - по возрастанию)
    std::string sql = "SELECT ID, NAME, AGE FROM USERS ORDER BY NAME ASC LIMIT ? OFFSET ?;";

    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, pageSize);
        sqlite3_bind_int(stmt, 2, offset);

        std::cout << "\n--- Page " << pageNumber << " (size " << pageSize << ") ---" << std::endl;

        bool hasData = false;
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            hasData = true;
            int id = sqlite3_column_int(stmt, 0);
            const char *name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            int age = sqlite3_column_int(stmt, 2);
            std::cout << "ID: " << id << " | Name: " << name << " | Age: " << age << std::endl;
        }
        if (!hasData)
            std::cout << "This page is empty." << std::endl;

        sqlite3_finalize(stmt);
    }
}
void DataCore::Database::printUsersPagedAge(int pageSize, int pageNumber)
{
    sqlite3_stmt *stmt;
    // Считаем пропуск: если 2-я страница при размере 5, нужно пропустить 5 записей
    int offset = (pageNumber - 1) * pageSize;

    // Сортируем по имени (ASC - по возрастанию)
    std::string sql = "SELECT ID, NAME, AGE FROM USERS ORDER BY AGE ASC LIMIT ? OFFSET ?;";

    if (sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK)
    {
        sqlite3_bind_int(stmt, 1, pageSize);
        sqlite3_bind_int(stmt, 2, offset);

        std::cout << "\n--- Page " << pageNumber << " (size " << pageSize << ") ---" << std::endl;

        bool hasData = false;
        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            hasData = true;
            int id = sqlite3_column_int(stmt, 0);
            const char *name = reinterpret_cast<const char *>(sqlite3_column_text(stmt, 1));
            int age = sqlite3_column_int(stmt, 2);
            std::cout << "ID: " << id << " | Name: " << name << " | Age: " << age << std::endl;
        }
        if (!hasData)
            std::cout << "This page is empty." << std::endl;

        sqlite3_finalize(stmt);
    }
}
