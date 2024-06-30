/* BL702 Driver*/
#include <bflb_platform.h>
/* USB STDIO */
#ifdef M0SENSE_USE_USBSTDIO
#include <usb_stdio.h>
#endif

#define BSP_UART_CLOCK_SOURCE  ROOT_CLOCK_SOURCE_PLL_96M
#define BSP_UART_CLOCK_DIV  0

#define CONFIG_GPIO14_FUNC GPIO_FUN_UART0_TX
#define CONFIG_GPIO15_FUNC GPIO_FUN_UART0_RX
#define BSP_USING_UART0

#if defined(BSP_USING_UART0)
#ifndef UART0_CONFIG
#define UART0_CONFIG \
{   \
.id = 0, \
.baudrate = 2000000,\
.databits = UART_DATA_LEN_8, \
.stopbits = UART_STOP_ONE, \
.parity = UART_PAR_NONE, \
.fifo_threshold = 1, \
}
#endif
#endif

int main(void)
{
    bflb_platform_init(0);
    MSG_DBG(
        "Now can use MSG_xxx, LOG_xxx and bflb_platform_printf on uart.\r\n");  // just appear on uart unless use printf

#ifdef M0SENSE_USE_USBSTDIO
    usb_stdio_init();                                        // MUST be called before any call to printf or puts
    printf("Now can use printf, puts on usb_cdc_acm.\r\n");  // on usb, ttyACMx on Linux or COMx on Windows.
#endif

    while (1) {
        printf("hello, world\r\n");
        MSG_DBG("Test!\n");
        bflb_platform_delay_ms(2000);
    }
}