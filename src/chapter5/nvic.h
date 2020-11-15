#ifndef __NVIC_H__
#define __NVIC_H__

#define NUM_EXCEPTIONS      60u
#define NUM_IRQS            44u

/* CORTEX-M3 exceptions and interrupts mapped to their vector numbers */

#define FAULT_NMI           2u          // Non-Maskable Interrupt(NMI)
#define FAULT_HARD          3u          // Hard Fault
#define FAULT_MEM_MGMT      4u          // Memory Management Fault
#define FAULT_BUS           5u          // Bus Fault 
#define FAULT_USAGE         6u          // Usage Fault
#define FAULT_SVCALL        11u         // Supervisor Call
#define FAULT_DEBUG         12u         // Debug Monitor
#define FAULT_PENDSV        14u         // Pendable Request Service
#define FAULT_SYS_TICK      15u         // System Tick Timer

#define IRQ_GPIOA           16u         // GPIO Port A
#define IRQ_GPIOB           17u         // GPIO Port B
#define IRQ_GPIOC           18u         // GPIO Port C
#define IRQ_GPIOD           19u         // GPIO Port D
#define IRQ_GPIOE           20u         // GPIO Port E
#define IRQ_UART0           21u         // UART0
#define IRQ_UART1           22u         // UART1
#define IRQ_SSI0            23u         // SSI0
#define IRQ_I2C0            24u         // I2C0
#define IRQ_PWM_FAULT       25u         // PWM Fault
#define IRQ_PWM0            26u         // PWM Generator 0
#define IRQ_PWM1            27u         // PWM Generator 1
#define IRQ_PWM2            28u         // PWM Generator 2
#define IRQ_QEI0            29u         // QEI0
#define IRQ_ADC0_SEQ0       30u         // ADC0 Sequence 0
#define IRQ_ADC0_SEQ1       31u         // ADC0 Sequence 1
#define IRQ_ADC0_SEQ2       32u         // ADC0 Sequence 2
#define IRQ_ADC0_SEQ3       33u         // ADC0 Sequence 3
#define IRQ_WATCHDOG0       34u         // Watcdog Timer 0
#define IRQ_TIMER0A         35u         // Timer 0A
#define IRQ_TIMER0B         36u         // Timer 0B
#define IRQ_TIMER1A         37u         // Timer 1A
#define IRQ_TIMER1B         38u         // Timer 1B
#define IRQ_TIMER2A         39u         // Timer 2A
#define IRQ_TIMER2B         40u         // TImer 2B
#define IRQ_ACOMP0          41u         // Analog Comparator 0
#define IRQ_ACOMP1          42u         // Analog Comparator 1
#define IRQ_SYS_CTL         44u         // System Control
#define IRQ_FLASH_CTL       45u         // Flash Memory Control
#define IRQ_GPIOF           46u         // GPIO Port F
#define IRQ_GPIOG           47u         // GPIO Port G
#define IRQ_UART2           49u         // UART2
#define IRQ_TIMER3A         51u         // Timer 3A
#define IRQ_TIMER3B         52u         // Timer 3B
#define IRQ_I2C1            53u         // I2C1
#define IRQ_QEI1            54u         // QEI1
#define IRQ_ETH             58u         // Ethernet Controller
#define IRQ_HIBERNATE       59u         // Hibernate Module

void nvic_irq_enable(uint32_t vector_num);
void nvic_irq_disable(uint32_t vector_num);

#endif /* __NVIC_H__ */
