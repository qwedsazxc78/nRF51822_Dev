#ifndef __DHT11_H__
#define __DHT11_H__
#include <stdint.h>

#define DATA_PIN    11   //��ʪ�ȴ�����DHT11���ܽ�
#define PIN_DATA_OUT   (nrf_gpio_cfg_output(DATA_PIN));   //P0.07����Ϊ����
#define PIN_DATA_IN    (nrf_gpio_cfg_input(DATA_PIN,NRF_GPIO_PIN_PULLUP));    //P0.07����Ϊ���


#define PIN_DATA_SET    (nrf_gpio_pin_set(DATA_PIN)); 
#define PIN_DATA_CLEAR  (nrf_gpio_pin_clear(DATA_PIN));


typedef struct DHT11_VALUE 
{
    uint16_t tempH;  //�¶ȸ��ֽڣ�ʮλ��
    uint8_t  tempL;  //�¶ȵ�λ�ڣ���λ��
    uint8_t  humyH;  //ʪ�ȸ��ֽڣ�ʮλ��
    uint8_t  humyL;  //ʪ�ȸ��ֽڣ���λ��

}DHT11VALUE;

extern   DHT11VALUE DHT11Value;


extern void COM(void);          // ��ʪд��
extern void DHT11(void);        //��ʪ�ȴ���������

extern uint8_t temp[2]; 
extern uint8_t temp1[5];
extern uint8_t humidity[2];
extern uint8_t humidity1[9];

#endif 
