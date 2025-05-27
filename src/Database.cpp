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
    std::string sql = "CREATE TABLE IF NOT EXISTS USERS ("
                      "USERNAME TEXT PRIMARY KEY,"
                      "PASSWORD TEXT NOT NULL,"
                      "IS_MANAGER INTEGER NOT NULL,"
                      "EMAIL TEXT,"
                      "NAME TEXT,"
                      "ACCOUNT_NUMBER TEXT);";
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
bool Database::createTransactionHistoryTable() {
    const char* sql =
    "CREATE TABLE IF NOT EXISTS TransactionHistory ("
    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
    "username TEXT,"
    "type TEXT,"
    "amount REAL,"
    "partner TEXT,"
    "datetime TEXT"
    ");";
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
    addTransaction(fromUser, "Transfer Out", amount, toUser);
    addTransaction(toUser, "Transfer In", amount, fromUser);
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

bool Database::insertUser(const std::string& username,
                          const std::string& hashedPassword,
                          bool isAuto,
                          const std::string& email,
                          const std::string& name,
                          const std::string& accountNumber) {
    std::string sql = "INSERT INTO USERS (USERNAME, PASSWORD, IS_MANAGER, EMAIL, NAME, ACCOUNT_NUMBER) VALUES (?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare insertUser statement: " << sqlite3_errmsg(db) << std::endl;
        return false;
    }

    sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, hashedPassword.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 3, isAuto ? 1 : 0);
    sqlite3_bind_text(stmt, 4, email.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 6, accountNumber.c_str(), -1, SQLITE_STATIC);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

bool Database::validateUser(const string& username, const string& hashedPassword, int& isAuto) {
    string sql = "SELECT IS_MANAGER FROM USERS WHERE USERNAME = ? AND PASSWORD = ?;";
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
    string sql = "UPDATE USERS SET PASSWORD = ?, IS_MANAGER = 0 WHERE USERNAME = ?;";
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
bool Database::addTransaction(const string& username, const string& type, double amount, const string& partner) {
    time_t now = time(nullptr);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", localtime(&now));

    string sql = "INSERT INTO TransactionHistory (username, type, amount, partner, datetime) VALUES ('" + 
                 username + "', '" + type + "', " + to_string(amount) + ", '" + partner + "', '" + buf + "');";

    return execute(sql);
}
string Database::getAccountNumber(const string& username) {
    string sql = "SELECT ACCOUNT_NUMBER FROM USERS WHERE USERNAME = ?;";
    sqlite3_stmt* stmt;
    string result;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            result = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        }
        sqlite3_finalize(stmt);
    }
    return result;
}

string Database::getUsernameByAccount(const string& accountNumber) {
    string sql = "SELECT USERNAME FROM USERS WHERE ACCOUNT_NUMBER = ?;";
    sqlite3_stmt* stmt;
    string result;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, accountNumber.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            result = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        }
        sqlite3_finalize(stmt);
    }
    return result;
}



vector<tuple<string, string, double, string>> Database::getTransactionHistory(const string& username) {
    vector<tuple<string, string, double, string>> history;
    string sql = "SELECT datetime, type, amount, partner FROM TransactionHistory WHERE username='" + username + "' ORDER BY datetime DESC LIMIT 20;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            string datetime = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            string type = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            double amount = sqlite3_column_double(stmt, 2);
            string partner = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            history.emplace_back(datetime, type, amount, partner);
        }
    }
    sqlite3_finalize(stmt);
    return history;
}

bool Database::updateIsAuto(const string& username, int isManager) {
    string sql = "UPDATE USERS SET IS_MANAGER = ? WHERE USERNAME = ?;";
    sqlite3_stmt* stmt;

    // Chuẩn bị câu lệnh SQL
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Prepare failed: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    // Gán giá trị vào câu lệnh
    sqlite3_bind_int(stmt, 1, isManager);
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);

    // Thực thi lệnh
    int result = sqlite3_step(stmt);
    int changes = sqlite3_changes(db); // <- số dòng thực sự bị thay đổi

    sqlite3_finalize(stmt);

    // Kiểm tra kết quả
    if (result != SQLITE_DONE) {
        cerr << "Step failed: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    if (changes == 0) {
        cerr << "No rows were updated. Có thể USERNAME không tồn tại hoặc IS_MANAGER đã là giá trị này rồi." << endl;
        return false;
    }

    return true;
}

bool Database::updateName(const string& username, const string& newName) {
    // Kiểm tra đầu vào
    if (username.empty() || newName.empty()) {
        cerr << "Lỗi: Username hoặc newName rỗng" << endl;
        return false;
    }

    // Kiểm tra username tồn tại
    if (!userExists(username)) {
        cerr << "Lỗi: Username '" << username << "' không tồn tại trong cơ sở dữ liệu" << endl;
        return false;
    }

    string sql = "UPDATE USERS SET NAME = ? WHERE USERNAME = ?;";
    sqlite3_stmt* stmt;

    // Chuẩn bị câu lệnh SQL
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        cerr << "Chuẩn bị thất bại: " << sqlite3_errmsg(db) << endl;
        return false;
    }

    // Gán giá trị
    sqlite3_bind_text(stmt, 1, newName.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, username.c_str(), -1, SQLITE_STATIC);

    // Thực thi câu lệnh
    int result = sqlite3_step(stmt);
    int changes = sqlite3_changes(db); // Kiểm tra số hàng bị ảnh hưởng

    if (result != SQLITE_DONE) {
        cerr << "Thực thi thất bại: " << sqlite3_errmsg(db) << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    if (changes == 0) {
        cerr << "Không có hàng nào được cập nhật: Username '" << username << "' không tồn tại hoặc FULLNAME đã là giá trị này" << endl;
        sqlite3_finalize(stmt);
        return false;
    }

    sqlite3_finalize(stmt);


    return true;
}
