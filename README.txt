# Hệ thống giao dịch điểm thưởng

## Giới thiệu dự án

Dự án **Hệ thống giao dịch điểm thưởng** được phát triển bằng ngôn ngữ C++ nhằm xây dựng một hệ thống giúp người dùng và quản lý giao dịch, quản lý điểm thưởng dễ dàng và an toàn. Hệ thống cho phép người dùng đăng ký, đăng nhập, quản lý thông tin cá nhân, chuyển điểm thưởng, kiểm tra số dư và lịch sử giao dịch. Quản lý có thể tạo tài khoản người dùng, quản lý cơ sở dữ liệu và hỗ trợ thay đổi thông tin khi cần thiết.

Dự án sử dụng thư viện SQLite3 để quản lý cơ sở dữ liệu và thư viện cURL để gửi mã OTP qua email.

## Thành viên tham gia dự án

- **Đinh Thế Phong**  
  - Thiết kế mã nguồn đăng nhập, đăng ký  
  - Mã nguồn OTP  
  - Hệ thống quản lý  
  - Thay đổi thông tin người dùng  

- **Phạm Cao Gia Khang**  
  - Thiết kế mã nguồn cơ sở dữ liệu  
  - Mã nguồn hiển thị các truy vấn giao dịch  
  - Mã nguồn đổi mật khẩu khi quên 
  - Test chức năng, xử lý các bug 
  - Thay đổi thông tin người dùng    

- **Hồ Nguyên Sâm**  
  - Thiết kế đồ họa giao diện  
  - Mã nguồn hiển thị các truy vấn giao dịch  

- **Trần Hoàng Nguyên**  
  - Thiết kế mã nguồn chuyển đổi điểm thưởng  
  - Test chức năng, xử lý các bug  

## Đặc tả chức năng

### Người dùng

- Đăng nhập, đăng ký tài khoản với xác thực OTP qua email  
- Thay đổi mật khẩu khi quên  
- Kiểm tra số dư ví điểm thưởng  
- Chuyển khoản điểm thưởng cho người khác  
- Truy vấn các giao dịch và trạng thái giao dịch  
- Thay đổi mật khẩu chủ động (không quên)  
- Thay đổi họ tên cá nhân  

### Quản lý

- Tạo tài khoản cho người dùng  
- Quản lý cơ sở dữ liệu  
- Thay đổi thông tin người dùng theo yêu cầu  

### Quy trình thay đổi thông tin cá nhân

- Người dùng hoặc quản lý sẽ chọn đổi mật khẩu hoặc đổi họ tên (username không đổi được)  
- Yêu cầu nhập lại username để xác thực ban đầu  
- Hệ thống gửi mã OTP để xác thực tiếp  
- Sau khi xác thực thành công, hệ thống cập nhật thông tin mới  
- Đối với thay đổi mật khẩu do quản lý hỗ trợ (ví dụ khi quên mật khẩu), người dùng cần:  
  - Xác thực OTP  
  - Ở lần đăng nhập tiếp theo, phải đăng nhập lại mật khẩu hiện tại trước khi tạo mật khẩu mới để bảo mật  

## Hướng dẫn tải và cài đặt

1. Clone dự án từ GitHub:  

https://github.com/giakhangcd/Bai_tap_lon.git


2. Dự án sử dụng ngôn ngữ C++ cùng thư viện SQLite3 và cURL, cần đảm bảo các thư viện này đã được cài đặt trên máy.

## Hướng dẫn build và chạy

- Sử dụng `tasks.json` trong VS Code để build.  
- Nhấn tổ hợp phím `Ctrl + Shift + B` để build dự án.  
- Sau khi build thành công, chạy file app.exe để thực thi


## Cấu trúc tệp tin chính

- `main.cpp`: Mã nguồn chính điều khiển chương trình.  
- `user.cpp`: Các chức năng chính của hệ thống dành cho người dùng.  
- `ui.cpp`: Thiết kế giao diện người dùng.  
- `otp.cpp`: Mã nguồn tạo và gửi mã OTP qua email.  
- `database.cpp`: Mã nguồn xử lý đọc ghi dữ liệu trong hệ thống cơ sở dữ liệu SQLite3.  

## Hướng dẫn sử dụng chương trình

- Khi khởi động, chương trình hiển thị 4 lựa chọn:  
1. Đăng nhập người dùng  
2. Đăng ký người dùng  
3. Đăng nhập quản lý  
4. Tạo mật khẩu mới khi quên  

- Đăng ký người dùng yêu cầu nhập:  
- Username  
- Mật khẩu  
- Email  
- Họ tên  
- Sau đó hệ thống gửi mã OTP để xác thực email.  

- Sau khi đăng nhập thành công, người dùng có 5 lựa chọn:  
1. Đổi thông tin cá nhân (họ tên hoặc mật khẩu)  
2. Giao dịch chuyển điểm thưởng  
3. Kiểm tra số dư ví  
4. Kiểm tra lịch sử giao dịch  
5. Đăng xuất  

Quy trình thay đổi thông tin cá nhân
Khi người dùng hoặc quản lý muốn thay đổi thông tin cá nhân, hệ thống cung cấp 2 lựa chọn:

Thay đổi mật khẩu

Thay đổi họ tên
(Lưu ý: Username không thể thay đổi được)

Trước khi tiến hành thay đổi, hệ thống yêu cầu người dùng nhập lại username hiện tại để xác thực bước đầu.

Sau bước xác thực username, hệ thống sẽ gửi mã OTP đến email đã đăng ký của người dùng để tiếp tục xác thực.

Khi người dùng nhập chính xác mã OTP, hệ thống cho phép người dùng cập nhật thông tin tương ứng (mật khẩu mới hoặc họ tên mới).

Đặc biệt, trong trường hợp thay đổi mật khẩu do người quản lý hỗ trợ (ví dụ khi người dùng quên mật khẩu):

Người dùng chỉ cần xác thực bằng mã OTP.

Tuy nhiên, để đảm bảo an toàn, ở lần đăng nhập tiếp theo, người dùng sẽ phải đăng nhập lại với mật khẩu hiện tại một lần nữa trước khi được phép tạo mật khẩu mới.

*Quy trình này cũng tương tự với chức năng quên mật khẩu, nhằm giữ bảo mật cho tài khoản người dùng.


## Tài liệu tham khảo

- Không sử dụng tài liệu tham khảo ngoài trong quá trình phát triển.


