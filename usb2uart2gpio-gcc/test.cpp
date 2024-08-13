#include <windows.h>
#include <iostream>

bool ListAndOpenSerialPort(std::string& portName) {
    // 枚举串口设备
    char portNameBuffer[50];
    for (int i = 1; i < 10; ++i) { // 通常COM口编号不会超过10
        sprintf(portNameBuffer, "COM%d", i);
        HANDLE hSerial = CreateFileA(
            portNameBuffer,
            GENERIC_READ | GENERIC_WRITE,
            0,
            0,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            0
        );

        if (hSerial != INVALID_HANDLE_VALUE) {
            std::cout << "Serial port " << portNameBuffer << " opened successfully." << std::endl;
            portName = portNameBuffer; // 保存成功打开的串口名称
            return true; // 成功打开串口
        } else {
            // 打印错误信息（可选）
            DWORD error = GetLastError();
            if (error == ERROR_FILE_NOT_FOUND) {
                // 串口不存在，可以忽略
            } else {
                // 其他错误，可能需要处理
                std::cerr << "Error opening serial port " << portNameBuffer << ": " << error << std::endl;
            }
        }
    }
    return false; // 所有串口尝试失败
}

int main() {
    std::string portName;
    if (ListAndOpenSerialPort(portName)) {
        // 使用串口进行通信
    } else {
        std::cerr << "No serial ports available or all failed to open." << std::endl;
        return 1;
    }
    // ...
}
