#ifndef OTP_H
#define OTP_H
#include <string>
#include <curl/curl.h>

// Khai báo hàm tạo và gửi OTP qua email
std::string createOTP(const std::string& to);

// Khai báo hàm callback cho cURL
size_t read_callback(char* buffer, size_t size, size_t nitems, void* userdata);
#endif