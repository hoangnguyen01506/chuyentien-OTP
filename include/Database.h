#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include "../sql/sqlite3.h"
#include <vector>
#include <tuple>

class Database {
private:
    sqlite3* db;

public:
    Database();
    ~Database();

    bool connect(const std::string& filename);
    void disconnect();
    bool execute(const std::string &sql);

    // Table creation
    bool createUserTable();
    bool createViTienTable();
    bool createTransactionHistoryTable();
    std::string getAccountNumber(const std::string& username);
    std::string getUsernameByAccount(const std::string& accountNumber);
    std::string getUserEmail(Database& db, const std::string& username);
    std::string getFullname(Database& db, const std::string& username);


    // User operations
    bool userExists(const std::string& username);

    // 🔧 Đã sửa: thêm accountNumber vào insertUser
    bool insertUser(const std::string& username,
                    const std::string& hashedPassword,
                    bool isAuto,
                    const std::string& email,
                    const std::string& name,
                    const std::string& accountNumber);

    bool validateUser(const std::string& username, const std::string& hashedPassword, int& isAuto);
    bool updatePassword(const std::string& username, const std::string& newHashedPassword);
    sqlite3* getDB();

    // Wallet operations
    bool initViTien();
    bool showBalance(const std::string& username);
    bool createWalletForUser(const std::string& username);
    double getBalance(const std::string& username);
    bool transferMoney(const std::string& fromUser, const std::string& toUser, double amount);

    // Transaction history
    bool addTransaction(const std::string& username, const std::string& type, double amount, const std::string& partner);
    std::vector<std::tuple<std::string, std::string, double, std::string>> getTransactionHistory(const std::string& username);
    bool updateIsAuto(const std::string& username, int isAuto);
    bool updateName(const std::string& username, const std::string& newName);
};

#endif // DATABASE_H
