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
int main(void)
{
    bflb_platform_init(0);

    adc_channel_cfg_t adc_channel_cfg;

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
        MSG("PosId = %d Value = %d Volt = %d mV \r\n", result_val.posChan, result_val.value, (uint32_t)(result_val.volt * 1000));
        bflb_platform_delay_ms(500);
    }
}