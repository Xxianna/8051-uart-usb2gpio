all: main.cpp; 
	g++ -static -o uart.exe main.cpp  -lUSBgpioBurner -L./ -O3 -std=c++17

debug:
	g++ -o uart.exe main.cpp SerialPort.cpp USBgpioBurner.cpp -std=c++17 -g -O0

#静态链接方法
lib: 
	g++ -c USBgpioBurner.cpp -o tmp/USBgpioBurner.o
	ar rcs libUSBgpioBurner.a tmp/USBgpioBurner.o


#以下针对单个dll的情况，不和任何lib接口搭配使用。用不到
dll: SerialPort.cpp; 
	g++ -shared -o SerialPort.dll SerialPort.cpp -std=c++17 -fPIC

fromdll: main.cpp; 
	g++ -o uart.exe SerialPort.dll main.cpp -O3 -std=c++17
	

test: test.cpp; 
	g++ -static -o test.exe test.cpp