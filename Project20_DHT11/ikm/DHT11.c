#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "DHT11.h"

//��ʪ�ȶ���
uint8_t U8FLAG,uchartemp;
uint8_t u8T_data_H,u8T_data_L,u8RH_data_H,u8RH_data_L,u8checkdata;
uint8_t u8T_data_H_temp,u8T_data_L_temp,u8RH_data_H_temp,uRH_data_L_temp,u8checkdata_temp;
uint8_t ucharcomdata;

DHT11VALUE DHT11Value;
/******************************************************************************
 * ��  �� : 10 us��ʱ
 * ��  �� : ��
 * ����ֵ : ��
 *****************************************************************************/
void Delay_10us() 
{
  nrf_delay_us(1);
  nrf_delay_us(1);
  nrf_delay_us(1);
  nrf_delay_us(1);
  nrf_delay_us(1);
  nrf_delay_us(1);
  nrf_delay_us(1);
  nrf_delay_us(1);
  nrf_delay_us(1);
  nrf_delay_us(1);   
}

uint8_t GetPinValue(void)
{
  if((nrf_gpio_pin_read(DATA_PIN) && (1<<DATA_PIN)) != 0 ) return 1;
	else return 0;
}
/******************************************************************************
 * ��  �� : ��ʪ�ȴ���д������д��1���ֽ�
 * ��  �� : ��
 * ����ֵ : ��
 *****************************************************************************/
void COM(void)    // ��ʪд��
{     
    uint8_t i;         
    for(i=0;i<8;i++)    
    {
        U8FLAG=2; 
        while((!GetPinValue())&&U8FLAG++);
        Delay_10us();
        Delay_10us();
        Delay_10us();
        uchartemp=0;
        if(GetPinValue())uchartemp=1;
        U8FLAG=2;
        while((GetPinValue())&&U8FLAG++);   
        if(U8FLAG==1)break;    
        ucharcomdata<<=1;
        ucharcomdata|=uchartemp; 
    }    
}

/******************************************************************************
 * ��  �� : Ds18b20д������д��1���ֽ�
 * ��  �� : ��
 * ����ֵ : ��
 *****************************************************************************/
void DHT11(void)   //��ʪ��������
{
    PIN_DATA_OUT
	  PIN_DATA_CLEAR;
    nrf_delay_ms(19);  //>18MS
    PIN_DATA_SET; 
    PIN_DATA_IN;  //����P0.2Ϊ����
    Delay_10us();
    Delay_10us();                        
    Delay_10us();
    Delay_10us();  
    if(!GetPinValue()) 
    {
        U8FLAG=2; 
        while((!GetPinValue())&&U8FLAG++);
        U8FLAG=2;
        while((GetPinValue())&&U8FLAG++); 
        COM();
        u8RH_data_H_temp=ucharcomdata;
        COM();
        uRH_data_L_temp=ucharcomdata;
        COM();
        u8T_data_H_temp=ucharcomdata;
        COM();
        u8T_data_L_temp=ucharcomdata;
        COM();
        u8checkdata_temp=ucharcomdata;
        //DATA_PIN=1; 
        uchartemp=(u8T_data_H_temp+u8T_data_L_temp+u8RH_data_H_temp+uRH_data_L_temp);
        if(uchartemp==u8checkdata_temp)
        {
            u8RH_data_H=u8RH_data_H_temp;
            u8RH_data_L=uRH_data_L_temp;
            u8T_data_H=u8T_data_H_temp;
            u8T_data_L=u8T_data_L_temp;
            u8checkdata=u8checkdata_temp;
        }
        DHT11Value.tempH = u8T_data_H/10; 
        DHT11Value.tempL = u8T_data_H%10;
        
        DHT11Value.humyH = u8RH_data_H/10; 
        DHT11Value.humyL = u8RH_data_H%10;        
    } 
    else //û�óɹ���ȡ������0
    {
        DHT11Value.tempH = 0; 
        DHT11Value.tempL = 0;
        
        DHT11Value.humyH = 0; 
        DHT11Value.humyL = 0;  
    } 
    
    PIN_DATA_OUT; //����P0.7Ϊ��� 
}
/********************************************END FILE*******************************************/
