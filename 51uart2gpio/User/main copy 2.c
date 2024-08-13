#include "public.h"
#include "lcd1602.h"
#include "freq-duty.c"

void voidf(void){}
#define APP_NUM 3

typedef void (*void_func)(void);//���庯��ָ��,������ֲӦ�õ��жϺ���
void_func timer0_func=voidf;
void_func timer1_func=voidf;
//����Ӧ�ú����ṹ��
typedef struct{
    void_func app9;
    char ctrlc;
    void_func exit9;
}func_app;

//����Ӧ�ó�
func_app app_pool[APP_NUM];
char app_num=APP_NUM;

//Ӧ��1�������ź�ռ�ձ�&Ƶ�ʲⶨ
void freq_duty_init(){
    timer0_func=timer0_fd;
    timer1_func=timer1_fd;
    main_fd(&(app_pool[0].ctrlc));
    //����ֻ����ȡ��ַ����ʵ�ֿ���Ӧ��
}
void freq_duty_exit(void){
    timer0_func=voidf;
    timer1_func=voidf;
}
//Ӧ�óس�ʼ��
void app_pool_init(void)
{
    app_pool[0].app9=freq_duty_init;
    app_pool[0].ctrlc=0;
    app_pool[0].exit9=freq_duty_exit;
    app_pool[1].app9=voidf;
    app_pool[1].ctrlc=0;
    app_pool[1].exit9=voidf;
    app_pool[2].app9=voidf;
    app_pool[2].ctrlc=0;
    app_pool[2].exit9=voidf;
}

void main()
{	
	EA=1;//�����ж�
    //��ʼ���ⲿ�ж�0�������л�Ӧ��
	IT0=1;//�����ش�����ʽ���½��أ�
	EX0=1;//��INT1���ж�����
    lcd1602_init();
    app_pool_init();
	while(1)
	{
        //�л�Ӧ��
        if(app_num==APP_NUM){
            lcd1602_clear();
            LCD1602_V0=0;
            delay_ms(100);
        }
        else{
            lcd1602_clear();
            LCD1602_V0=!LCD1602_V0;
            app_pool[app_num].ctrlc=1;
            app_pool[app_num].app9();
        } 
	}
}
//�ⲿ�ж�0�����л�Ӧ��
void exti0() interrupt 0 
{
    //�л�Ӧ��
    if(app_num<APP_NUM)
    {
        app_pool[app_num].ctrlc=0;
        app_pool[app_num].exit9();
        //�˳�����������ʵ�֣������벻�У��������Ա����
        app_num++;
    }else app_num=0;
    //app_num=APP_NUM��ʾֹͣ
}

void timer1() interrupt 3
{
    timer1_func();
}
void timer0() interrupt 1//��ʱ��0�жϺ���
{
    timer0_func();
}
