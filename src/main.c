#include "stm8s.h"
#include "milis.h"
#include "stm8_hd44780.h"
#include <stdio.h>

#include "adc_helper.h"

#define _ISOC99_SOURCE
#define _GNU_SOURCE

#define LED_PORT GPIOC
#define LED_PIN  GPIO_PIN_5
#define LED_HIGH   GPIO_WriteHigh(LED_PORT, LED_PIN)
#define LED_LOW  GPIO_WriteLow(LED_PORT, LED_PIN)
#define LED_REVERSE GPIO_WriteReverse(LED_PORT, LED_PIN)

#define BTN_PORT GPIOE
#define BTN_PIN  GPIO_PIN_4
#define BTN_PUSH (GPIO_ReadInputPin(BTN_PORT, BTN_PIN)==RESET) 


void setup(void)
{
    CLK_HSIPrescalerConfig(CLK_PRESCALER_HSIDIV1);      // taktovani MCU na 16MHz
    GPIO_Init(LED_PORT, LED_PIN, GPIO_MODE_OUT_PP_LOW_SLOW);
    GPIO_Init(BTN_PORT, BTN_PIN, GPIO_MODE_IN_FL_NO_IT);

    ADC2_SchmittTriggerConfig(ADC2_SCHMITTTRIG_CHANNEL4, DISABLE); //PB4
    ADC2_SchmittTriggerConfig(ADC2_SCHMITTTRIG_CHANNEL5, DISABLE); //PB5
    // nastavíme clock pro ADC (16MHz / 4 = 4MHz)
    ADC2_PrescalerConfig(ADC2_PRESSEL_FCPU_D4);
    // volíme zarovnání výsledku (typicky vpravo, jen vyjmečně je výhodné vlevo)
    ADC2_AlignConfig(ADC2_ALIGN_RIGHT);
    // nasatvíme multiplexer na některý ze vstupních kanálů
    ADC2_Select_Channel(ADC2_CHANNEL_4);
    // rozběhneme AD převodník
    ADC2_Cmd(ENABLE);
    // počkáme než se AD převodník rozběhne (~7us)
    ADC2_Startup_Wait();


    init_milis();
}

int main(void){
    uint32_t time = 0;
    uint8_t text [16];
    uint8_t text2 [16];
    uint16_t ADCx;
    uint16_t napeti;
    uint16_t teplota;

    setup();


    lcd_init();

    while (1) {

        if (milis() - time > 1000 /*&& BTN_PUSH*/) {
            LED_REVERSE; 
            time = milis();
            ADCx = ADC_get(ADC2_CHANNEL_4);
            napeti = (uint32_t)3300 * ADCx / 1024;
            teplota = ((uint32_t)3300 * ADCx / 1024);
            lcd_clear();
            lcd_gotoxy(0, 0);
            sprintf(text, "U=%dmV", napeti);
            lcd_puts(text);
            lcd_gotoxy(0, 1);            
            sprintf(text2, "Teplota=%d.%d", teplota/10, teplota%10);
            lcd_puts(text2);
            }  

    }
}

/*-------------------------------  Assert -----------------------------------*/
#include "__assert__.h"
