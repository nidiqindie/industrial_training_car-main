#ifndef __OLED_H
#define __OLED_H
#define RCC_APB2Periph_OLED_GPIO RCC_APB2Periph_GPIOA
#define OLED_GPIO GPIOA
#define OLED_SCL GPIO_Pin_4
#define OLED_SDA GPIO_Pin_5 
void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowFloatNum(uint8_t x, uint8_t y, float data, uint8_t num_int, uint8_t num_float);
#endif
