/* BL702 Driver*/
#include <bflb_platform.h>
/* USB STDIO */
#ifdef M0SENSE_USE_USBSTDIO
#include <usb_stdio.h>
#endif
#include "hal_adc.h"

#define CONFIG_GPIO14_FUNC GPIO_FUN_UART0_TX

//Redefine the CONFIG_GPIO15_FUNC in the bl702_iot/pinmux_config.h, which is located in sub-module
//#define CONFIG_GPIO15_FUNC GPIO_FUN_ANALOG


//According to the RM, the GPIO15 could use ADC_CH1
adc_channel_t posChList[] = { ADC_CHANNEL1 };
adc_channel_t negChList[] = { ADC_CHANNEL_GND };

adc_channel_val_t result_val;

struct device *adc_dev;

//0 ~ 127°C
#define NTC_ADC_MAX     (128)
static const uint16_t ADC_NTC[NTC_ADC_MAX] = 
{
    50056, 49459, 48850, 48230, 47598, 46956, 46303, 45641, 44970, 44290, 43603, 42908, 42207, 41500, 40788, 40072, 39352, 38629, 37904, 37177, 36450, 35723, 34997, 34272, 33550, 32768, 32115, 31403, 30696, 29995, 29300, 28612, 27931, 27257, 26592, 25935, 25287, 24648, 24020, 23401, 22792, 22194, 21606, 21029, 20463, 19909, 19346, 18819, 18303, 17799, 17306, 16824, 16354, 15895, 15448, 15011, 14585, 14171, 13767, 13373, 12990, 12617, 12254, 11902, 11559, 11225, 10901, 10586, 10280, 9982, 9693, 9413, 9141, 8876, 8620, 8371, 8129, 7895, 7667, 7446, 7233, 7025, 6824, 6628, 6439, 6255, 6077, 5904, 5737, 5663, 5503, 5349, 5200, 5055, 4914, 4778, 4645, 4517, 4392, 4271, 4154, 4041, 3930, 3823, 3719, 3618, 3521, 3426, 3334, 3244, 3158, 3074, 2992, 2913, 2836, 2761, 2689, 2619, 2551, 2484, 2420, 2358, 2297, 2238, 2181, 2125, 2071, 2019
};


static float get_temp_from_adc_val(uint16_t adc_val)
{
    uint32_t i;
    uint32_t detla;
    for (i = 0; i < NTC_ADC_MAX; i++)
    {
        if (adc_val >= ADC_NTC[i])
        {
            detla = (ADC_NTC[i] - ADC_NTC[i + 1]) / 10;
            //Approximately linear decrease
            return i + (1 - ((adc_val - ADC_NTC[i]) / detla) * 0.1);
        }
    }
    return -1;
}

int main(void)
{
    bflb_platform_init(0);

    adc_channel_cfg_t adc_channel_cfg;
    float temp;

    adc_channel_cfg.pos_channel = posChList;
    adc_channel_cfg.neg_channel = negChList;
    adc_channel_cfg.num = 1;

    adc_register(ADC0_INDEX, "adc");

    adc_dev = device_find("adc");

    if (adc_dev) {
        device_open(adc_dev, DEVICE_OFLAG_STREAM_RX);
        if (device_control(adc_dev, DEVICE_CTRL_ADC_CHANNEL_CONFIG, &adc_channel_cfg) == ERROR) {
            MSG("ADC channel config error , Please check the channel corresponding to IO is initial success by board system or Channel is invaild \r\n");
            while (1)
                ;
        }
        MSG("adc device find success\r\n");
    }

    while (1) {
        adc_channel_start(adc_dev);
        device_read(adc_dev, 0, (void *)&result_val, 1); /*max size is 32*/
        MSG("PosId = %d Value = %d Volt = %d mV ", result_val.posChan, result_val.value, (uint32_t)(result_val.volt * 1000));
        temp = get_temp_from_adc_val(result_val.value);
        if (temp != -1)
        {
            MSG("T = %.1f°C\r\n", temp);
        }
        else
        {
            MSG("Temperature is out of range!\n");
        }
        bflb_platform_delay_ms(500);
    }
}