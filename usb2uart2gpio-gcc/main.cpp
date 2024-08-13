// main.cpp
#include "USBgpioBurner.h"
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;

int main() {

    USBgpioBurner myio("settings.ini");

    //创建计时器，用于统计时间
    // Create a timer to measure time
    auto start = chrono::high_resolution_clock::now();

    char tmp = 0;
    for(int i = 0; i < 100; i++){
        myio.writeIO(1, 1, tmp);
        tmp = !tmp;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    //创建计时器，用于统计时间
    // Create a timer to measure time
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    cout << "Time elapsed: " << elapsed.count() << "s" << endl;
    
    
    return 0;


    //把1组3组io写1，2组0组初始化为读取模式，然后读取2组4组的状态
    // Write 1, 2, 4 of group 1 to 1, 0, 1, then initialize 2, 4 of group 2 to read mode, and read the status of 2, 4
    for (int i = 0; i < 8; i++) {
        myio.writeIO(1, i, true);
        myio.writeIO(3, i, true);
        myio.init_readIO(2, i);
        myio.init_readIO(4, i);
    }


    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    getchar();

    //读取所有io的状态并打印
    // Read the status of all IOs and print
    int status;
    for (int i = 0; i < 8; i++) {
        status = myio.readIO(2, i);
        if(status == 2){
            cout << "Error reading IO 2-" << i << endl;
        }else{
            cout << "Group 2, IO " << i << " status: " << status << endl;
        }
        status = myio.readIO(4, i);
        if(status == 2){
            cout << "Error reading IO 4-" << i << endl;
        }else{
            cout << "Group 4, IO " << i << " status: " << status << endl;
        }
        //13也读取
        // Also read 13
        status = myio.readIO(3, i);
        if (status == 2) {
            cout << "Error reading IO 3-" << i << endl;
        } else {
            cout << "Group 3, IO " << i << " status: " << status << endl;
        }
        status = myio.readIO(1, i);
        if (status == 2) {
            cout << "Error reading IO 1-" << i << endl;
        } else {
            cout << "Group 1, IO " << i << " status: " << status << endl;
        }
    }

    //关闭所有io
    // Close all IOs
    for (int i = 0; i < 8; i++) {
        myio.writeIO(1, i, false);
        myio.writeIO(3, i, false);
        myio.writeIO(0, i, false);
        myio.writeIO(2, i, false);
    }


    return 0;
}
