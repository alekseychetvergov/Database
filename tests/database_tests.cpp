#include <algorithm>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include <app/requester_database.hpp>
#include <database/database.hpp>

namespace
{

DataCore::IDatabase &database()
{
    static DataCore::IDatabase &instance = DataCore::Database::GetInstance(":memory:");
    return instance;
}

class DatabaseTests : public ::testing::Test
{
protected:
    static void SetUpTestSuite()
    {
        database().createSQLTabble();
    }
};

TEST_F(DatabaseTests, AddsAndRetrievesUser)
{
    constexpr std::string_view name = "gtest_add_retrieve";
    constexpr int age = 37;

    const long long id = database().addUser(name, age);

    ASSERT_GT(id, 0);
    const auto user = database().getUserById(static_cast<int>(id));
    ASSERT_TRUE(user.has_value());
    EXPECT_EQ(user->first, name);
    EXPECT_EQ(user->second, age);
    EXPECT_TRUE(database().isUserExists(name));
}

TEST_F(DatabaseTests, UpdatesUserAge)
{
    constexpr std::string_view name = "gtest_update";
    const long long id = database().addUser(name, 24);

    ASSERT_GT(id, 0);
    database().updateUserAge(name, 25);

    const auto user = database().getUserById(static_cast<int>(id));
    ASSERT_TRUE(user.has_value());
    EXPECT_EQ(user->second, 25);
}

TEST_F(DatabaseTests, DeletesUser)
{
    constexpr std::string_view name = "gtest_delete";
    const long long id = database().addUser(name, 19);

    ASSERT_GT(id, 0);
    database().deleteUser(static_cast<int>(id));

    EXPECT_FALSE(database().isUserExists(name));
    EXPECT_FALSE(database().getUserById(static_cast<int>(id)).has_value());
}

TEST_F(DatabaseTests, AddsUsersInBulkAndSearchesByPartialName)
{
    const std::vector<std::pair<std::string, int>> users = {
        {"gtest_bulk_alex", 28},
        {"gtest_bulk_sam", 31}};

    database().addUsersMassive(users);

    std::vector<DataCore::User> foundUsers;
    database().searchUsersAdvanced("gtest_bulk_", [&foundUsers](const DataCore::User &user)
                                   { foundUsers.push_back(user); });

    ASSERT_EQ(foundUsers.size(), users.size());
    for (const auto &expected : users)
    {
        const auto found = std::find_if(foundUsers.begin(), foundUsers.end(), [&expected](const DataCore::User &user)
                                        { return user.name == expected.first; });
        ASSERT_NE(found, foundUsers.end());
        EXPECT_EQ(found->age, expected.second);
    }
}

TEST_F(DatabaseTests, ParsesCommandsCaseInsensitively)
{
    reqesterDatabase requester;

    EXPECT_EQ(requester.request("add"), Command::Add);
    EXPECT_EQ(requester.request("ADD"), Command::Add);
    EXPECT_EQ(requester.request("2"), Command::Add);
    EXPECT_EQ(requester.request("not-a-command"), Command::Unknown);
}

}
