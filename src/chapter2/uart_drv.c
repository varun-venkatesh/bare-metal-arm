#include "uart_drv.h"

/* The MISRA-C:2004, Rule 10.6 
 * Also, Refer: http://www.ti.com/lit/ds/symlink/lm3s6965.pdf Table 2-4.
 * UART0 is one of 3 UART interfaces on LM3S6965 mapped to the adderss shown below.
 */
static volatile uart_regs *uart0 = (uart_regs*)0x4000C000u;

/* UART baud rate is determined in terms of the systemclock.
 * The default setting of the system clock on LM3S6965 is 12MHz.
 * Refer: http://www.ti.com/lit/ds/symlink/lm3s6965.pdf Table 5-5.
 * Also, when you run qemu with lm3s6965, a peek into Regsiter RCC (0x400FE060)
 * and RCC2 (which overrides RCC if USERCC2 is set) show that the value of SYSDIV/SYSDIV2 fields 
 * respectively is 16 ==> the sysclock frequency is 12.5 MHz
 */
#define UART_DFLT_SYSCLK 12500000u 

/* Refer: http://www.ti.com/lit/ds/symlink/lm3s6965.pdf 12.4
 * to use UART0, the UART0 bit in RCGC1 should be set.
 * Check page 220 of the above manual for 
 * the address of RCGC1 and its description
 * TODO: this is a hack to get the uart working. It will be placed in
 * it's own file.
 */
static void set_clk_uart0(void)
{
    uint32_t *pRCGC1 = (uint32_t*)0x400FE104u;

    *pRCGC1 |= 0x00000001u;
}

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
    uint32_t brdi, brdf, dvsr, remd;

    /* Refer http://www.ti.com/lit/ds/symlink/lm3s6965.pdf 12.3.2 */
    
    dvsr = (baudrate * 16u);

    /* integer part of the baud rate */
    brdi = UART_DFLT_SYSCLK/dvsr;

    /* fractional part of the baudrate */
    remd = UART_DFLT_SYSCLK - dvsr * brdi;
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
    set_clk_uart0();
    uart_set_baudrate(baudrate);
    uart_set_example_line_ctrls();
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
