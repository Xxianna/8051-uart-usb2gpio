// main.cpp
#include "SerialPort.h"
#include <iostream>

using namespace std;

class MySerialPortHandler {
public:
    virtual void onReceive(const std::string& data) {
        // std::cout << "Received: " << data << std::endl;
        //如果不以换行符结尾，加到缓存中
        this->data += data;
        //如果以换行符结尾，处理缓存中的数据
        if (data.back() == '\n') {
            std::cout << "Received: " << this->data;
            this->data.clear();
        }
    }
private:
    string data;
};

int main() {


    SerialPort serialPort;
    if (!serialPort.init("settings.ini")) {
        std::cout << "Failed to initialize serial port." << std::endl;
        return 1;
    }

    MySerialPortHandler handler;
    serialPort.setReceiveCallback([&handler](const std::string& data) {
        handler.onReceive(data);
    });

    std::cout << "Serial port initialized. Press Enter to exit." << std::endl;

    // Example: Send data
    serialPort.send("Hello, Serial Port!");

    std::cin.get(); // Wait for Enter key

    serialPort.close();
    return 0;
}
