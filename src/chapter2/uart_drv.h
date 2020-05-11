#ifndef UART_DRV_H
#define UART_DRV_H

#include <stdint.h>

/* UART register map structure.
 * Refer: http://www.ti.com/lit/ds/symlink/lm3s6965.pdf Table 12-3.
 */
typedef struct __attribute__ ((packed)){
    uint32_t DR;                // 0x00 UART Data Register
    uint32_t RSRECR;            // 0x04 UART Recieve Status/Error Clear Register
    uint32_t reserved0[4];      // 0x08-0x14 reserved
    uint32_t FR;                // 0x18 UART Flag Register
    uint32_t reserved1;         // 0x1C reserved
    uint32_t ILPR;              // 0x20 UART IrDA Low-Power Register Register
    uint32_t IBRD;              // 0x24 UART Integer Baud-Rate Divisor Register
    uint32_t FBRD;              // 0x28 UART Fractional Baud-Rate Divisor Register
    uint32_t LCRH;              // 0x2C UART Line Control Register
    uint32_t CTL;               // 0x30 UART Control Register
}uart_regs;

typedef enum{
    UART_OK = 0,
    UART_NO_DATA,
    UART_RX_ERR,
}uart_err;

/* Configured as in the example 
 * Refer http://www.ti.com/lit/ds/symlink/lm3s6965.pdf 12.4
 * and http://www.ti.com/lit/ds/symlink/lm3s6965.pdf for UARTLCRH description.
 */
#define UARTLCRH_EXAMPLE 0x00000060u

#define UARTCTL_UARTEN 0x00000001u
#define UARTFR_TXFF 0x00000020u
#define UARTFR_RXFE 0x00000010u
#define UARTDR_DATA_MASK 0x000000FFu
#define UARTRSRECR_ERR_MASK 0x0000000Fu
#define UARTFR_BUSY 0x00000008u
#define UARTLCRH_FEN 0x00000010

void uart_init(uint32_t baudrate);
void uart_tx_byte(uint8_t byte);
uart_err uart_rx_byte(uint8_t* byte);

#endif /* UART_DRV_H */
