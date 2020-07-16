#ifndef __SYSTICK_H__
#define __SYSTICK_H__

#define STCTRL_ENABLE           0x00000001u
#define STCTRL_INTEN            0x00000002u
#define STCTRL_CLKSRC           0x00000004u
#define STCTRL_COUNT            0x00010000u

#define STRELOAD_MASK           0x00FFFFFFu
#define STCURRENT_MASK          0x00FFFFFFu

#define SYSTICK_2_IRQS_PER_SEC  2u
#define SYSTICK_4_IRQS_PER_SEC  4u
#define SYSTICK_8_IRQS_PER_SEC  8u
#define SYSTICK_16_IRQS_PER_SEC 16u

void systick_enable(void);
void systick_disable(void);
void systick_irq_enable(void);
void systick_irq_disable(void);
void systick_set_period_ms(uint32_t millisec);

#endif /* __SYSTICK_H__ */
