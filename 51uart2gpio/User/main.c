#include "stc89.h"

#define BUFFER_SIZE 20

// 17361@40MHz 6个12T触发一次
void Serial_Init() {
    SCON = 0x50;  // 设置为模式1，8位数据，可变波特率
    TMOD |= 0x20; // 使用定时器1作为波特率发生器
    TH1 = 0xFA;   // 设置波特率为9600
    TL1 = 0xFA;   // 设置波特率为9600
    TR1 = 1;      // 启动定时器1
    TI = 1;       // 设置TI，使能串口发送
    REN = 1;      // 允许串口接收
}


void Uartsend(unsigned char byte)//发送
{
	SBUF=byte;//把数据写入发送缓冲区SBUF
	//数据发送完成的标志是TI=1；所以等待数据传送完
	while(TI==0);
	TI=0;//软件清零
}

// void Uartsendstring(char *str) {
//     while (*str) {
//         Uartsend(*str++);
//     }
// }

// void Delay1s(void)	//@40MHz
// {
// 	unsigned char data i, j, k;

// 	i = 26;
// 	j = 85;
// 	k = 248;
// 	do
// 	{
// 		do
// 		{
// 			while (--k);
// 		} while (--j);
// 	} while (--i);
// }

 
int main()
{
    unsigned char receivedByte;
    unsigned char ioStatus;
    // sfr tmp;
    // unsigned char i;

    //初始化默认所有IO口为0
    P0 = 0x00;
    P1 = 0x00;
    P2 = 0x00;
    // P3 = 0x00;
    

	Serial_Init();
    Uartsend('a');
    while (1) {
        if (RI) { // 如果接收到数据
            RI = 0; // 清除接收中断标志
            receivedByte = SBUF; // 读取接收到的数据

            //将输入的数据二进制转换为字符串
            // Uartsendstring("receivedByte: ");
            // for (i = 0; i < 8; i++) {
            //     if (receivedByte & (0x80 >> i)) {
            //         Uartsend('1');
            //     } else {
            //         Uartsend('0');
            //     }
            // }
            // Uartsend('\n');
            

            // 解析命令
            if ((receivedByte & 0x80) == 0x80) { // 检查是否为IO命令
                unsigned char group = (receivedByte & 0x60) >> 5;
                unsigned char ioNumber = (receivedByte & 0x1C) >> 2;
                // unsigned char command = receivedByte & 0x03;

                if (!(receivedByte & 0x02)) { // 输出命令
                    ioStatus = receivedByte & 0x01;
                    // 根据group和ioNumber设置IO状态

                    //设置值
                    switch (group)
                    {
                    case 0:
                        if (ioStatus) {
                            P0 |= (1 << ioNumber);
                        } else {
                            P0 &= ~(1 << ioNumber);
                        }
                        // Uartsendstring("set P0");
                        break;
                    case 1:
                        if (ioStatus) {
                            P1 |= (1 << ioNumber);
                        } else {
                            P1 &= ~(1 << ioNumber);
                        }
                        // Uartsendstring("set P1");
                        break;
                    case 2:
                        if (ioStatus) {
                            P2 |= (1 << ioNumber);
                        } else {
                            P2 &= ~(1 << ioNumber);
                        }
                        // Uartsendstring("set P2");
                        break;
                    case 3:
                        if (ioStatus) {
                            P3 |= (1 << ioNumber);
                        } else {
                            P3 &= ~(1 << ioNumber);
                        }
                        // Uartsendstring("set P3");
                        break;
                    default:
                        break;
                    }
                    Uartsend('s');

                } else if (!(receivedByte & 0x01)) { // 读取命令
                    // 读取IO状态
                    //设置值
                    switch (group)
                    {
                    case 0:
                        ioStatus = P0 & (1 << ioNumber);
                        break;
                    case 1:
                        ioStatus = P1 & (1 << ioNumber);
                        break;
                    case 2:
                        ioStatus = P2 & (1 << ioNumber);
                        break;
                    case 3:
                        ioStatus = P3 & (1 << ioNumber);
                        break;
                    default:
                        break;
                    }

                    if (ioStatus) {
                        ioStatus = 0xFF;
                    } else {
                        ioStatus = 0x00;
                    }

                    // 发送读取到的IO状态回主机（此处假设接收命令后立即发送状态）
                    Uartsend(ioStatus);
                }
            }
        }
    }
}
