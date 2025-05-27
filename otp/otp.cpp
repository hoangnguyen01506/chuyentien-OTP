#include <iostream>
#include <string>
#include <ctime>
#include <cstring>
#include <cstdlib>
#include <curl/curl.h>
#include "otp.h"

// Hàm callback để đọc dữ liệu gửi qua cURL
size_t read_callback(char* buffer, size_t size, size_t nitems, void* userdata) {
    std::string* payload = static_cast<std::string*>(userdata);
    size_t len = size * nitems;
    if (payload->empty()) {
        return 0;
    }
    size_t copy_len = std::min(payload->length(), len);
    std::memcpy(buffer, payload->c_str(), copy_len);
    payload->erase(0, copy_len);
    return copy_len;
}

// Hàm tạo và gửi OTP qua email
std::string createOTP(const std::string& to) {
    // Khởi tạo seed cho số ngẫu nhiên
    srand((unsigned)time(0));

    // Thông tin email
    const std::string from = "tonythephong@gmail.com";
    const std::string username = "tonythephong@gmail.com";
    const std::string app_password = "xigwelwjkylfywtj";

    // Tạo OTP gồm 6 chữ số
    std::string otp;
    for (int i = 0; i < 6; ++i) {
        otp += std::to_string(rand() % 10);
    }
    

    // Tạo payload cho email
    std::string payload =
        "To: " + to + "\r\n" +
        "From: " + from + "\r\n" +
        "Subject: Mã OTP xác thực\r\n" +
        "\r\n" +
        "Mã OTP của bạn là: " + otp + "\r\n" +
        "Vui lòng sử dụng mã này để xác thực.\r\n";

    std::cout << "Đang thiết lập cURL..." << std::endl;

    // Khởi tạo cURL
    CURLcode res = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (res != CURLE_OK) {
        std::cerr << "Khởi tạo cURL thất bại: " << curl_easy_strerror(res) << std::endl;
        return "";
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Không thể khởi tạo cURL." << std::endl;
        curl_global_cleanup();
        return "";
    }

    std::cout << "cURL đã được khởi tạo." << std::endl;

    // Thiết lập danh sách người nhận
    struct curl_slist* recipients = nullptr;
    recipients = curl_slist_append(nullptr, to.c_str());
    if (!recipients) {
        std::cerr << "Không thể thêm địa chỉ nhận." << std::endl;
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return "";
    }

    // Thiết lập các tùy chọn cURL
    res = curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465");
    if (res != CURLE_OK) std::cerr << "Lỗi CURLOPT_URL: " << curl_easy_strerror(res) << std::endl;

    res = curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
    if (res != CURLE_OK) std::cerr << "Lỗi CURLOPT_USE_SSL: " << curl_easy_strerror(res) << std::endl;

    res = curl_easy_setopt(curl, CURLOPT_CAINFO, "cacert.pem");

    if (res != CURLE_OK) std::cerr << "Lỗi CURLOPT_CAINFO: " << curl_easy_strerror(res) << std::endl;

    res = curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
    if (res != CURLE_OK) std::cerr << "Lỗi CURLOPT_USERNAME: " << curl_easy_strerror(res) << std::endl;

    res = curl_easy_setopt(curl, CURLOPT_PASSWORD, app_password.c_str());
    if (res != CURLE_OK) std::cerr << "Lỗi CURLOPT_PASSWORD: " << curl_easy_strerror(res) << std::endl;

    res = curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from.c_str());
    if (res != CURLE_OK) std::cerr << "Lỗi CURLOPT_MAIL_FROM: " << curl_easy_strerror(res) << std::endl;

    res = curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);
    if (res != CURLE_OK) std::cerr << "Lỗi CURLOPT_MAIL_RCPT: " << curl_easy_strerror(res) << std::endl;

    res = curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
    if (res != CURLE_OK) std::cerr << "Lỗi CURLOPT_READFUNCTION: " << curl_easy_strerror(res) << std::endl;

    res = curl_easy_setopt(curl, CURLOPT_READDATA, &payload);
    if (res != CURLE_OK) std::cerr << "Lỗi CURLOPT_READDATA: " << curl_easy_strerror(res) << std::endl;

    res = curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    if (res != CURLE_OK) std::cerr << "Lỗi CURLOPT_UPLOAD: " << curl_easy_strerror(res) << std::endl;

    // Bỏ dòng này nếu không cần log chi tiết cURL:
    // res = curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

    if (res != CURLE_OK) std::cerr << "Lỗi CURLOPT_VERBOSE: " << curl_easy_strerror(res) << std::endl;

    res = curl_easy_setopt(curl, CURLOPT_TIMEOUT, 60L);
    if (res != CURLE_OK) std::cerr << "Lỗi CURLOPT_TIMEOUT: " << curl_easy_strerror(res) << std::endl;

    res = curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 30L);
    if (res != CURLE_OK) std::cerr << "Lỗi CURLOPT_CONNECTTIMEOUT: " << curl_easy_strerror(res) << std::endl;

    // Gửi email
    std::cout << "Đang gửi email..." << std::endl;
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        std::cerr << "Gửi mail thất bại: " << curl_easy_strerror(res) << std::endl;
        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
        curl_global_cleanup();
        return "";
    }

    // Dọn dẹp
    curl_slist_free_all(recipients);
    curl_easy_cleanup(curl);
    curl_global_cleanup();

    return otp;
}