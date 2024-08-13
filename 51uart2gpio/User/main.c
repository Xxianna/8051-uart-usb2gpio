#include "stc89.h"

#define BUFFER_SIZE 20

// 17361@40MHz 6��12T����һ��
void Serial_Init() {
    SCON = 0x50;  // ����Ϊģʽ1��8λ���ݣ��ɱ䲨����
    TMOD |= 0x20; // ʹ�ö�ʱ��1��Ϊ�����ʷ�����
    TH1 = 0xFA;   // ���ò�����Ϊ9600
    TL1 = 0xFA;   // ���ò�����Ϊ9600
    TR1 = 1;      // ������ʱ��1
    TI = 1;       // ����TI��ʹ�ܴ��ڷ���
    REN = 1;      // �����ڽ���
}


void Uartsend(unsigned char byte)//����
{
	SBUF=byte;//������д�뷢�ͻ�����SBUF
	//���ݷ�����ɵı�־��TI=1�����Եȴ����ݴ�����
	while(TI==0);
	TI=0;//�������
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

    //��ʼ��Ĭ������IO��Ϊ0
    P0 = 0x00;
    P1 = 0x00;
    P2 = 0x00;
    // P3 = 0x00;
    

	Serial_Init();
    Uartsend('a');
    while (1) {
        if (RI) { // ������յ�����
            RI = 0; // ��������жϱ�־
            receivedByte = SBUF; // ��ȡ���յ�������

            //����������ݶ�����ת��Ϊ�ַ���
            // Uartsendstring("receivedByte: ");
            // for (i = 0; i < 8; i++) {
            //     if (receivedByte & (0x80 >> i)) {
            //         Uartsend('1');
            //     } else {
            //         Uartsend('0');
            //     }
            // }
            // Uartsend('\n');
            

            // ��������
            if ((receivedByte & 0x80) == 0x80) { // ����Ƿ�ΪIO����
                unsigned char group = (receivedByte & 0x60) >> 5;
                unsigned char ioNumber = (receivedByte & 0x1C) >> 2;
                // unsigned char command = receivedByte & 0x03;

                if (!(receivedByte & 0x02)) { // �������
                    ioStatus = receivedByte & 0x01;
                    // ����group��ioNumber����IO״̬

                    //����ֵ
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

                } else if (!(receivedByte & 0x01)) { // ��ȡ����
                    // ��ȡIO״̬
                    //����ֵ
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

                    // ���Ͷ�ȡ����IO״̬���������˴���������������������״̬��
                    Uartsend(ioStatus);
                }
            }
        }
    }
}
