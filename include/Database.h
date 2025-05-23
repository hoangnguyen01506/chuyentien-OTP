#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include "../sql/sqlite3.h"
#include <vector>
#include<tuple>
class Database {
private:
    sqlite3* db;

public:
    Database();
    ~Database();

    bool connect(const std::string& filename);
    void disconnect();
    bool createUserTable();
    bool userExists(const std::string& username);
    bool insertUser(const std::string& username, const std::string& hashedPassword, bool isAuto, const std::string& email, const std::string& name);
    bool validateUser(const std::string& username, const std::string& hashedPassword, int& isAuto);
    bool updatePassword(const std::string& username, const std::string& newHashedPassword);
    sqlite3* getDB();

    bool execute(const std::string &sql);
    bool createViTienTable();
    bool initViTien();
    bool transferMoney(const std::string& fromUser, const std::string& toUser, double amount);
    bool showBalance(const std::string& username);
    bool createWalletForUser(const std::string& username);
    double getBalance(const std::string& username);
    bool addTransaction(const std::string& username, const std::string& type, double amount, const std::string& partner);
    std::vector<std::tuple<std::string, std::string, double, std::string>> getTransactionHistory(const std::string& username);
    bool createTransactionHistoryTable();
};

#endif
