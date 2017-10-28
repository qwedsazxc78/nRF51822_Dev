#ifndef __DHT11_H__
#define __DHT11_H__
#include <stdint.h>

#define DATA_PIN    11   //温湿度传感器DHT11检测管脚
#define PIN_DATA_OUT   (nrf_gpio_cfg_output(DATA_PIN));   //P0.07设置为输入
#define PIN_DATA_IN    (nrf_gpio_cfg_input(DATA_PIN,NRF_GPIO_PIN_PULLUP));    //P0.07设置为输出


#define PIN_DATA_SET    (nrf_gpio_pin_set(DATA_PIN)); 
#define PIN_DATA_CLEAR  (nrf_gpio_pin_clear(DATA_PIN));


typedef struct DHT11_VALUE 
{
    uint16_t tempH;  //温度高字节（十位）
    uint8_t  tempL;  //温度低位节（个位）
    uint8_t  humyH;  //湿度高字节（十位）
    uint8_t  humyL;  //湿度高字节（个位）

}DHT11VALUE;

extern   DHT11VALUE DHT11Value;


extern void COM(void);          // 温湿写入
extern void DHT11(void);        //温湿度传感器启动

extern uint8_t temp[2]; 
extern uint8_t temp1[5];
extern uint8_t humidity[2];
extern uint8_t humidity1[9];

#endif 
