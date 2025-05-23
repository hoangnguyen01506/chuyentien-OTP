#include "../include/Database.h"
#include <iostream>
#include <string>

using namespace std;

Database::Database() : db(nullptr) {}

Database::~Database() {
    if (db) sqlite3_close(db);
}

bool Database::connect(const string& filename) {
    int exit = sqlite3_open(filename.c_str(), &db);
    if (exit) {
        cerr << "Error open DB: " << sqlite3_errmsg(db) << endl;
        return false;
    }
    if (!createUserTable()) {
        cerr << "Failed to create USERS table." << endl;
        return false;
    }

    return true;
}

void Database::disconnect() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

bool Database::execute(const std::string& sql) {
    char* errMsg = nullptr;
    int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool Database::createWalletForUser(const std::string& username) {
    std::string sql = "INSERT INTO ViTien (ten, soDu) VALUES (?, 100);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

bool Database::createUserTable() {
    string sql = "CREATE TABLE IF NOT EXISTS USERS("
                 "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                 "FULLNAME TEXT NOT NULL, "
                 "USERNAME TEXT NOT NULL UNIQUE, "
                 "PASSWORD TEXT NOT NULL, "
                 "EMAIL TEXT, "
                 "IS_AUTO INTEGER NOT NULL);";
    return execute(sql);
}

bool Database::createViTienTable() {
    string sql = R"(CREATE TABLE IF NOT EXISTS ViTien (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        ten TEXT NOT NULL,
        soDu REAL NOT NULL
    );)";
    return execute(sql);
}

bool Database::initViTien() {
    return createViTienTable();
}

bool Database::showBalance(const std::string& username) {
    std::string sql = "SELECT soDu FROM ViTien WHERE ten = ?;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare SQL: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        double soDu = sqlite3_column_double(stmt, 0);
        std::cout << "Your current balance is: " << soDu << " VND" << std::endl;
    } else {
        std::cout << "Wallet not found for this user." << std::endl;
    }

    sqlite3_finalize(stmt);
    return true;
}

double Database::getBalance(const std::string& username) {
    string sql = "SELECT soDu FROM ViTien WHERE ten = ?;";
    sqlite3_stmt* stmt;
    double result = 0.0;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            result = sqlite3_column_double(stmt, 0);
        }
        sqlite3_finalize(stmt);
    }

    return result;
}


bool Database::transferMoney(const std::string& fromUser, const std::string& toUser, double amount) {
    if (amount <= 0) {
        std::cerr << "[ERROR] Số tiền không hợp lệ.\n";
        return false;
    }

    // 1. Lấy số dư người gửi
    std::string queryFrom = "SELECT soDu FROM ViTien WHERE ten = ?";
    sqlite3_stmt* stmtFrom;
    if (sqlite3_prepare_v2(db, queryFrom.c_str(), -1, &stmtFrom, nullptr) != SQLITE_OK) {
        std::cerr << "[ERROR] Prepare stmtFrom failed: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmtFrom, 1, fromUser.c_str(), -1, SQLITE_STATIC);

    double senderBalance = -1;
    if (sqlite3_step(stmtFrom) == SQLITE_ROW) {
        senderBalance = sqlite3_column_double(stmtFrom, 0);
    } else {
        std::cerr << "[ERROR] Không tìm thấy người gửi: " << fromUser << std::endl;
        sqlite3_finalize(stmtFrom);
        return false;
    }
    sqlite3_finalize(stmtFrom);

    if (senderBalance < amount) {
        std::cerr << "[ERROR] Không đủ tiền để chuyển. Số dư hiện tại: " << senderBalance << std::endl;
        return false;
    }

    // 2. Kiểm tra người nhận tồn tại
    std::string queryTo = "SELECT COUNT(*) FROM ViTien WHERE ten = ?";
    sqlite3_stmt* stmtTo;
    if (sqlite3_prepare_v2(db, queryTo.c_str(), -1, &stmtTo, nullptr) != SQLITE_OK) {
        std::cerr << "[ERROR] Prepare stmtTo failed: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmtTo, 1, toUser.c_str(), -1, SQLITE_STATIC);

    int count = 0;
    if (sqlite3_step(stmtTo) == SQLITE_ROW) {
        count = sqlite3_column_int(stmtTo, 0);
    }
    sqlite3_finalize(stmtTo);

    if (count == 0) {
        std::cerr << "[ERROR] Người nhận không tồn tại: " << toUser << std::endl;
        return false;
    }

    // 3. Trừ tiền người gửi
    std::string sql1 = "UPDATE ViTien SET soDu = soDu - ? WHERE ten = ?";
    sqlite3_stmt* stmtUpdateFrom;
    if (sqlite3_prepare_v2(db, sql1.c_str(), -1, &stmtUpdateFrom, nullptr) != SQLITE_OK) {
        std::cerr << "[ERROR] Prepare updateFrom failed: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_double(stmtUpdateFrom, 1, amount);
    sqlite3_bind_text(stmtUpdateFrom, 2, fromUser.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmtUpdateFrom) != SQLITE_DONE) {
        std::cerr << "[ERROR] Trừ tiền người gửi thất bại.\n";
        sqlite3_finalize(stmtUpdateFrom);
        return false;
    }
    sqlite3_finalize(stmtUpdateFrom);

    // 4. Cộng tiền người nhận
    std::string sql2 = "UPDATE ViTien SET soDu = soDu + ? WHERE ten = ?";
    sqlite3_stmt* stmtUpdateTo;
    if (sqlite3_prepare_v2(db, sql2.c_str(), -1, &stmtUpdateTo, nullptr) != SQLITE_OK) {
        std::cerr << "[ERROR] Prepare updateTo failed: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_double(stmtUpdateTo, 1, amount);
    sqlite3_bind_text(stmtUpdateTo, 2, toUser.c_str(), -1, SQLITE_STATIC);

    if (sqlite3_step(stmtUpdateTo) != SQLITE_DONE) {
        std::cerr << "[ERROR] Cộng tiền người nhận thất bại.\n";
        sqlite3_finalize(stmtUpdateTo);
        return false;
    }
    sqlite3_finalize(stmtUpdateTo);

    std::cout << "[SUCCESS] Đã chuyển " << amount << " từ " << fromUser << " đến " << toUser << std::endl;
    return true;
}

bool Database::userExists(const string& username) {
    string sql = "SELECT 1 FROM USERS WHERE USERNAME = ? LIMIT 1;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    if (sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        std::cerr << "Bind failed: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        return false;
    }

    int step = sqlite3_step(stmt);
    bool exists = (step == SQLITE_ROW);

    sqlite3_finalize(stmt);
    return exists;
}

bool Database::insertUser(const string& username, const string& hashedPassword, bool isAuto, const string& email, const string& fullname) {
    string sql = "INSERT INTO USERS (FULLNAME, USERNAME, PASSWORD, IS_AUTO, EMAIL) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "SQLite prepare error: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, fullname.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, hashedPassword.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, isAuto ? 1 : 0);
    sqlite3_bind_text(stmt, 5, email.c_str(), -1, SQLITE_STATIC);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

bool Database::validateUser(const string& username, const string& hashedPassword, int& isAuto) {
    string sql = "SELECT IS_AUTO FROM USERS WHERE USERNAME = ? AND PASSWORD = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hashedPassword.c_str(), -1, SQLITE_STATIC);
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        isAuto = sqlite3_column_int(stmt, 0);
        sqlite3_finalize(stmt);
        return true;
    }
    sqlite3_finalize(stmt);
    return false;
}

bool Database::updatePassword(const string& username, const string& newHashedPassword) {
    string sql = "UPDATE USERS SET PASSWORD = ?, IS_AUTO = 0 WHERE USERNAME = ?;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(stmt, 1, newHashedPassword.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);
    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

sqlite3* Database::getDB() {
    return db;
}
