#include <stdint.h>
#include <stdbool.h>
#include "lm3s6965_memmap.h"
#include "uart_drv.h"
#include "sysctl.h"

/* UART register map structure.
 * Refer: http://www.ti.com/lit/ds/symlink/lm3s6965.pdf Table 12-3.
 */
typedef struct __attribute__ ((packed)){
    uint32_t DR;                // 0x00 UART Data Register
    uint32_t RSRECR;            // 0x04 UART Recieve Status/Error Clear Register
    uint32_t reserved0[4];      // 0x08-0x14 reserved
    const uint32_t FR;          // 0x18 UART Flag Register
    uint32_t reserved1;         // 0x1C reserved
    uint32_t ILPR;              // 0x20 UART IrDA Low-Power Register Register
    uint32_t IBRD;              // 0x24 UART Integer Baud-Rate Divisor Register
    uint32_t FBRD;              // 0x28 UART Fractional Baud-Rate Divisor Register
    uint32_t LCRH;              // 0x2C UART Line Control Register
    uint32_t CTL;               // 0x30 UART Control Register
    uint32_t IFLS;              // 0x34 UART Interrupt FIFO Level Select Register
    uint32_t IM;                // 0x38 UART Interrupt Mask Register
    const uint32_t RIS;         // 0x3C UART Raw Interrupt Status Register
    const uint32_t MIS;         // 0x40 UART Masked Interrupt Status Register
    uint32_t ICR;               // 0x44 UART Interrupt Clear Register
}uart_regs;

/* The MISRA-C:2004, Rule 10.6 
 * Also, Refer: http://www.ti.com/lit/ds/symlink/lm3s6965.pdf Table 2-4.
 * UART0 is one of 3 UART interfaces on LM3S6965 mapped to the adderss shown below.
 */
static volatile uart_regs *uart0 = (uart_regs*)UART0_BASE;

/* Enable the uart
 * TXE and RXE - transmit and recieve enable bits
 * are enabled out of reset - hence we don't set them here.
 */
static void uart_enable(void)
{
    /* set UARTEN in UARTCTL */
    uart0->CTL |= UARTCTL_UARTEN;
}


/* Disable the uart */
static void uart_disable(void)
{
    /* clear UARTEN in UARTCTL*/
    uart0->CTL &= ~UARTCTL_UARTEN;

    /* Allow any ongoing transfers to finish */
    while(uart0->FR & UARTFR_BUSY);
    
    /* Flush the FIFOs by (disabling) clearing FEN */
    uart0->LCRH &= ~UARTLCRH_FEN;
}

/* Set the uart baudrate of the uart device*/
static void uart_set_baudrate(uint32_t baudrate)
{
    uint32_t sysclk, brdi, brdf, dvsr, remd;

    /* Refer http://www.ti.com/lit/ds/symlink/lm3s6965.pdf 12.3.2 */
    sysclk = sysctl_getclk();

    dvsr = (baudrate * 16u);

    /* integer part of the baud rate */
    brdi = sysclk/dvsr;

    /* fractional part of the baudrate */
    remd = sysclk - dvsr * brdi;
    brdf = ((remd << 6u) + (dvsr >> 1))/dvsr;

    uart0->IBRD = (uint16_t)(brdi & 0xffffu);
    uart0->FBRD = (uint8_t)(brdf & 0x3ffu);
}

/* Set the line uart controls as in the example
 * example settings: 8-bit word length, no parity, one stop bit, FIFOs disabled
 */
static void uart_set_example_line_ctrls(void)
{
    uart0->LCRH = UARTLCRH_EXAMPLE;
}

/* Enable the said uart interface to generate interrupts
 * depending on the flag set - conditions for interrupt generation.
 * This interrupt is then sent to the interrupt controller.
 */
static void uart_irq_enable(uint32_t irq_flags)
{
    uart0->IM |= irq_flags;
}

/* Clear the interrupt for said uart interface
 * depending on the flag set.
 */
static void uart_irq_clear(uint32_t irq_flags)
{
    uart0->ICR = irq_flags;
}

/* Get the current interrupt status of the said uart interface
 * depending of whether is_masked is true or false, it returns
 * the masked or raw interrupt status.
 */
static uint32_t uart_irq_status(bool is_masked)
{
    if(is_masked)
        return (uart0->MIS);
    else
        return (uart0->RIS);
}

/* Initialize the uart device 
 * This is assumed to be called once following reset.
 * NOTE: Since uart0 is being used, we don't have to turn on
 * the corresponding AFSEL bits in GPIOAFSEL for PortA. 
 * These pins are set to uart0 rx/tx by default.
 * Refer http://www.ti.com/lit/ds/symlink/lm3s6965.pdf 12.2
 */
void uart_init(uint32_t baudrate)
{

    uart_disable();
    sysctl_periph_clk_enable(UART0_BASE);
    uart_set_baudrate(baudrate);
    uart_set_example_line_ctrls();
    uart_irq_enable(UART_RX_IRQ);
    uart_enable();
}

/* Output a byte of data */
void uart_tx_byte(uint8_t byte)
{
    /* if tx register is full, wait until it isn't */
    while(uart0->FR & UARTFR_TXFF);
    
    uart0->DR = (uint32_t)byte;
}

/* Poll for an input byte of data */
uart_err uart_rx_byte(uint8_t* byte)
{
    /* if the rx register is empty, reply 
     * indicating that there is no data
     */
    if(uart0->FR & UARTFR_RXFE)
    {
        return UART_NO_DATA;
    }

    /* Received data is 12-bits in length, with the first 4-bits
     * representing the error flags and the last 8-bits, the data. 
     */
    *byte = uart0->DR & UARTDR_DATA_MASK;
    
    if(uart0->RSRECR & UARTRSRECR_ERR_MASK)
    {
        /* the received data had an error
         * write to ECR to clear the error flags
         */
        uart0->RSRECR &= UARTRSRECR_ERR_MASK;
        return UART_RX_ERR;
    }
    
    return UART_OK;
}

void uart0_irq_handler(void)
{
    uint32_t irq_status;
    char c;

    irq_status = uart_irq_status(true);

    uart_irq_clear(irq_status);

    if(irq_status & UART_RX_IRQ)
    {
        c = uart0->DR & UARTDR_DATA_MASK;
        
        if(c == '\r')
        {
            uart_tx_byte('\n');
        }
        else if(c == '\b')
        {
            // TODO: Figure out how to handle backspace
            // for now send X (which isn't working)
            uart_tx_byte('X');
        }
        else
        {
            uart_tx_byte(c);
        }
    }
}

