#include <string>
#include <mutex>


class USBgpioBurner {
public:
    USBgpioBurner(const std::string& settingsFile);

    ~USBgpioBurner();

    //io组0~3，io号0~7，输出状态，是否PWM输出.3-1、3-0不可使用
    // Write IO with specific group, io number, output status, and output type (0 for digital, 1 for PWM)
    bool writeIO(unsigned char group, unsigned char ioNumber, bool outputStatus, bool isPWM = false);

    //被读取的端口需要先初始化为允许读取模式，否则可能烧毁控制器
    char init_readIO(unsigned char group, unsigned char ioNumber);

    // Read IO with specific group and io number, the callback will be called when data is received
    // 0/1 : IO status 2: Error
    char readIO(unsigned char group, unsigned char ioNumber);

private:
    void* data;
    std::mutex mtx;
    unsigned char read_init[4] = {0};
};
