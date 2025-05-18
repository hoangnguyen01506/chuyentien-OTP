#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include "../sql/sqlite3.h"

class Database {
private:
    sqlite3* db;

public:
    Database();
    ~Database();

    bool connect(const std::string& filename);
    bool createUserTable();
    bool userExists(const std::string& username);
    bool insertUser(const std::string& username, const std::string& hashedPassword, bool isAuto, const std::string& email, const std::string& name);
    bool validateUser(const std::string& username, const std::string& hashedPassword, int& isAuto);
    bool updatePassword(const std::string& username, const std::string& newHashedPassword);
    sqlite3* getDB();
};

#endif
