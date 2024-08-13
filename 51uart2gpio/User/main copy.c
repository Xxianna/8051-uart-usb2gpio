#include "public.h"
#include "lcd1602.h"
//#include "freq-duty.c"

sbit LCD1602_V0=P2^4;//����pwm

//���尴��
sbit CHOICE=P1^1;
sbit STOP=P1^2;

sbit Tdt=P3^5;

u8 show_opt=3;
long Freq=121212;
long freq_s;
u16 Duty=0;
u8 start=1;
long up=0,total=0;

//��ʾռ�ձ�
void prtduty(u16 duty)
{
	static u16 dutyt=0;
	static u8 show[5]={'0','0','.','0','0'};
	//��ʾռ�ձ�
	if(dutyt==duty)return;
	show[0]=duty/1000+'0';
	show[1]=duty/100%10+'0';
	show[3]=duty/10%10+'0';
	show[4]=duty%10+'0';
	dutyt=duty;
	lcd1602_show_string(7,1,show);
}
//��ʾ����
void prtnum(long num) reentrant
{
	static long numc=0;
	static u8 flag,show[6];//��¼��ʾ�ĸ������֣���Ϊ�˳�����������������½�
	if(num==numc){
		return;
	}else{
		numc=num;flag=0;
		show[5]='0'+num%10;
		num/=10;if(!num)flag=1;
		show[4]=flag?' ':num%10+'0';
		num/=10;if(!flag&&!num)flag=1;
		show[3]=flag?' ':num%10+'0';
		num/=10;if(!flag&&!num)flag=1;
		show[2]=flag?' ':num%10+'0';
		num/=10;if(!flag&&!num)flag=1;
		show[1]=flag?' ':num%10+'0';
		num/=10;if(!flag&&!num)flag=1;
		show[0]=flag?' ':num%10+'0';
	}
	lcd1602_show_string(5,0,show);
}
/*
void exti1_init(void)
{
	IT1=1;//�����ش�����ʽ���½��أ�
	EX1=1;//��INT1���ж�����
	EA=1;//�����ж�
}*/
void time0_init(void)
{
	TMOD|=0X01;//ѡ��Ϊ��ʱ��0ģʽ��������ʽ1
	TH0=0XF2;	
	TL0=0XFA;	
	ET0=1;//�򿪶�ʱ��0�ж�����
	TR0=0;//��ͣ��ʱ��		
}
void time1_init(void)
{
	TMOD|=0X50;
	TH1=0;
	TL1=0;		
	TR1=0;//�򿪼�����	
	ET1=0;
}
void main()
{	
	bit choice_state=0;
    bit stop_state=0;
	EA=1;//�����ж�
	//exti1_init();//���֡�ʵʱ�л������жϡ�
	//��һ������Ϊ�ж�0�ڰ��ϻ�������������,ʹ�ò��ȶ������Բ����жϡ�
	time1_init();
	time0_init();
	lcd1602_init();//LCD1602��ʼ��
	lcd1602_show_string(0,0,"Freq:      Hz");
	lcd1602_show_string(0,1,"dt:    00.00%");
	
	while(1)
	{
		LCD1602_V0=!LCD1602_V0;//�������

        if(STOP==0){
			if(stop_state==0){
			delay_10us(100);//����
			if(STOP==0){
				show_opt^=1;//�Ƿ�ֹͣ
				freq_s=Freq;
				stop_state=1;
			}}
		}else{
			if(stop_state)stop_state=0;
		}

		if(CHOICE==0){//��������
		delay_10us(100);//��ʱһ��ʱ�䣬�ȴ������ȶ�
		if(CHOICE==0){
			if(choice_state==0){//����״̬Ϊ0��������״̬
			}else{//����״̬Ϊ1�����ɿ�״̬
				choice_state=0;
				show_opt^=2;//Ƶ�ʡ�ռ�ձ�
				if(show_opt==0||show_opt==1){//ռ�ձ�
					ET0=0;//�رն�ʱ��0�ж�
					TR0=0;//��ͣ��ʱ��0
					TMOD=0x20;
					TH1=0x80;
					TL1=0X80;
					TR1=1;//������ʱ��1
					ET1=1;
					total=0;up=0;
				}else{
					TMOD=0;//��ʼ��
					time1_init();//Ƶ��
					time0_init();
					start=1;
				}
			}
		}}else{
			if(choice_state==0){//�����ɿ�
				choice_state=1;
			}
		}
        if(start==1&&show_opt==3||show_opt==2){
			TR0=1;
			TR1=1;
			start=0;
		}
		if(show_opt==2){//��ʾ�̶�Ƶ��
			prtnum(freq_s);
		}else if(show_opt==0){//��ʾ�̶�ռ�ձ�
			prtduty(Duty);
		}else if(show_opt==3){//��ʾʵʱƵ��
			prtnum(Freq);
		}else if(show_opt==1){//��ʾʵʱռ�ձ�
			//����ռ�ձ�
			if((total>20000&&Freq>10000)||total>80000){
				ET1=0;//�رն�ʱ��1�ж�
				Duty=10000*up/total;
				total=0;
				up=0;
				ET1=1;//������ʱ��1�ж�
			}
			prtduty(Duty);
		}
        /*
		if(start==1&&show_opt==2){//��ʾ�̶�Ƶ��
			TR0=1;
			TR1=1;
			prtnum(freq_s);
			start=0;
		}else if(show_opt==0){//��ʾ�̶�ռ�ձ�
			prtduty(Duty);
		}else if(start==1&&show_opt==3){//��ʾʵʱƵ��
			TR0=1;
			TR1=1;
			prtnum(Freq);
			start=0;
		}else if(show_opt==1){//��ʾʵʱռ�ձ�
			//����ռ�ձ�
			if((total>20000&&Freq>10000)||total>80000){
				ET1=0;//�رն�ʱ��1�ж�
				Duty=10000*up/total;
				total=0;
				up=0;
				ET1=1;//������ʱ��1�ж�
			}
			prtduty(Duty);
		}*/
	}
		
}
/*
void exti1() interrupt 2 //�ⲿ�ж�1�жϺ���
{
	delay_10us(1000);//����
	if(STOP==0){
		show_opt^=1;//�Ƿ�ֹͣ
		freq_s=Freq;
	}

}
*/
void timer1() interrupt 3
{
	total++;
	up+=(Tdt==1);
}
void timer0() interrupt 1//��ʱ��0�жϺ���
{
	static int time;
	TH0=0XF2;	//����ʱ������ֵ����ʱ1ms
	TL0=0XFA;
	time++;
	if(time==247&&TH1>50){//0.25s�迨�����Ƶ�ʹ���ֱ�ӽضϣ�����T1���
		time=0;
		TR0=0;
		TR1=0;
		Freq=(TH1*256+TL1);
		Freq=4*Freq;//-Freq/150;//��freq��ˣ���Ϊ����long
		TH1=0;
		TL1=0;
		start=1;
	}else if(time==985){//ģ��1sУ׼ֵ������ʵ���������
		time=0;
		TR1=0;    //ֹͣ����
		TR0=0;          //ֹͣ��ʱ
		Freq=(TH1*256+TL1);   //���Ƶ��ֵ   ����1�����������
		//Freq+=Freq/3000*(Freq/5000);//����ƫ��
		//ע�⣺�ϸ�Ƶ������ƫ���Ϊ���������������46k���½�Ϊ׼ȷ
		TH1=0;          //����ֵ����
		TL1=0;
		start=1;  
	}
}
