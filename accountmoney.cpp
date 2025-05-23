// #include <iostream>
// #include "../include/Database.h"

// using namespace std;

// int main() {
//     Database db;
//     if (!db.connect("data/taikhoan.db")) {
//         cerr << "Cannot connect to csdl" << endl;
//         return 1;
//     }

//     // Đảm bảo bảng đã được tạo
//     if (!db.createViTienTable()) {
//         cerr << "Cannot create ViTien table." << endl;
//         return 1;
//     }

//     // Tạo ví cho user "alice"
//     string user = "alice";
//     int soDuBanDau = 500000;

//     string sql = "INSERT INTO ViTien (TenNguoiDung, SoDu) VALUES ('" + user + "', " + to_string(soDuBanDau) + ");";

//     if (db.execute(sql)) {
//         cout << "Wallet " << user << " created successfully.\n";
//     } else {
//         cout << "Error created for " << user << ".\n";
//     }

//     db.disconnect();
//     return 0;
// }
