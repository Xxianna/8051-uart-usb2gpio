
#include "USBgpioBurner.h"
#include <bitset>
#include <iostream>
#include <Windows.h>

// #define debug00

USBgpioBurner::USBgpioBurner(const std::string& settingsFile) {
    // SerialPort* serialPort = new SerialPort();
    // data = serialPort;
    // ��COM4�˿�
    DCB dcbSerialParams = {0};
    COMMTIMEOUTS timeouts = {0};
    
    data = CreateFileA("COM4",
                         GENERIC_READ | GENERIC_WRITE,
                         0,
                         0,
                         OPEN_EXISTING,
                         FILE_ATTRIBUTE_NORMAL,
                         0);

    if (data == INVALID_HANDLE_VALUE) {
        std::cout << "connect failed." << std::endl;
        exit(1);
    }

    // ���ô��ڲ���
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (!GetCommState(data, &dcbSerialParams)) {
        std::cout << "connect failed." << std::endl;
        CloseHandle(data);
        exit(1);
    }

    dcbSerialParams.BaudRate = 17361;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;

    if (!SetCommState(data, &dcbSerialParams)) {
        std::cout << "connect failed." << std::endl;
        CloseHandle(data);
        exit(1);
    }

    // ���ó�ʱ
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;
    timeouts.WriteTotalTimeoutConstant = 50;
    timeouts.WriteTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(data, &timeouts)) {
        std::cout << "connect failed." << std::endl;
        CloseHandle(data);
        exit(1);
    }
}

USBgpioBurner::~USBgpioBurner() {
    CloseHandle(data);
    data = nullptr;
}

bool USBgpioBurner::writeIO(unsigned char group, unsigned char ioNumber, bool outputStatus, bool isPWM) {

    //�Ϸ��Լ�飺�����3-1��3-0�����ش���
    if (group == 3 && (ioNumber == 0 || ioNumber == 1)) {
        std::cout << "Invalid IO number." << std::endl;
        return 2;
    }
    read_init[group] &= ~(1 << ioNumber);
    // Construct command byte
    unsigned char commandByte = 0x00;
    commandByte |= 0x80; // Set the first bit to indicate single IO command
    commandByte |= (group << 5) & 0x60; // Set bits 6 and 7 for the group
    commandByte |= (ioNumber << 2) & 0x1C; // Set bits 3, 4, and 5 for the io number
    commandByte |= (outputStatus | isPWM) | (isPWM << 1); // ���һλ�����ֵ�������ڶ�λ��ָ�ǲ���PWM�����趨PWM���ʱ�����һλ��1

#ifdef debug00
    //�Զ����ƴ�ӡ��������
    std::cout << "Command data: " << std::bitset<8>(commandByte) << std::endl;
    return 0;
#endif
    mtx.lock();
    bool result = WriteFile(data, &commandByte, 1, NULL, NULL);

        //ȷ���Ѿ��յ�
    unsigned char readByte;
    DWORD bytesRead;
    if (!ReadFile(data, &readByte, 1, &bytesRead, NULL)) {
        std::cout << "init_readIO failed." << std::endl;
        return 2;
    }

    
    mtx.unlock();
    return result;
    // return 0;
}

char USBgpioBurner::init_readIO(unsigned char group, unsigned char ioNumber) {
    //���Ϸ���
    if (group == 3 && (ioNumber == 0 || ioNumber == 1)) {
        std::cout << "Invalid IO number." << std::endl;
        return 2;
    }
    read_init[group] |= (1 << ioNumber);
    //��ȡ�Ķ˿���Ҫ����Ϊ�����ȡģʽ��дһ��1����
    // Construct command byte
    unsigned char commandByte = 0x00;
    commandByte |= 0x80; // Set the first bit to indicate single IO command
    commandByte |= (group << 5) & 0x60; // Set bits 6 and 7 for the group
    commandByte |= (ioNumber << 2) & 0x1C; // Set bits 3, 4, and 5 for the io number
    commandByte |= 0x01;

#ifdef debug00
    //�Զ����ƴ�ӡ��������
    std::cout << "Command data: " << std::bitset<8>(commandByte) << std::endl;
    return 0;
#endif
    mtx.lock();
    bool result = WriteFile(data, &commandByte, 1, NULL, NULL);

    //ȷ���Ѿ��յ�
    unsigned char readByte;
    DWORD bytesRead;
    if (!ReadFile(data, &readByte, 1, &bytesRead, NULL)) {
        std::cout << "init_readIO failed." << std::endl;
        return 2;
    }

    mtx.unlock();
    return result;
}

char USBgpioBurner::readIO(unsigned char group, unsigned char ioNumber) {

    //�Ϸ��Լ�飺�����3-1��3-0�����ش���
    if (group == 3 && (ioNumber == 0 || ioNumber == 1)) {
        std::cout << "Invalid IO number." << std::endl;
        return 2;
    }
    if((read_init[group] & (1 << ioNumber)) == 0){
        std::cout << "IO not initialized for reading." << std::endl;
        return 2;
    }

    // Construct command byte for read
    unsigned char commandByte = 0x00;
    commandByte |= 0x80; // Set the first bit to indicate single IO command
    commandByte |= (group << 5) & 0x60; // Set bits 6 and 7 for the group
    commandByte |= (ioNumber << 2) & 0x1C; // Set bits 3, 4, and 5 for the io number
    commandByte |= 0x02; //���趨PWM���ʱ�����һλ��0ʱ����ʾ��ȡio

    mtx.lock();
    if (!WriteFile(data, &commandByte, 1, NULL, NULL)) {
        std::cout << "read failed." << std::endl;
        return 2;
    }


#ifdef debug00
    //�Զ����ƴ�ӡ��������
    std::cout << "Command data: " << std::bitset<8>(commandByte) << std::endl;
#endif
    unsigned char readByte;
    DWORD bytesRead;
    if (!ReadFile(data, &readByte, 1, &bytesRead, NULL)) {
        std::cout << "read failed." << std::endl;
        return 2;
    }
    mtx.unlock();

    // return readByte;
    //����00/FF
    if(readByte == 0x00){
        return 0;
    }else if(readByte == 0xFF){
        return 1;
    }
    printf("read error\n");
    return 2;
}

