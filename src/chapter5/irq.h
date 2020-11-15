#ifndef __IRQ_H__
#define __IRQ_H__

/* To enable all interrupts with programmable priority.
 * Refer: Refer http://www.ti.com/lit/ds/symlink/lm3s6965.pdf
 * Table 2-13 and Section 2-3-4
 */
static inline void irq_master_enable(void)
{
    /* https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html
     * asm statements that have no output operands, including asm goto statements, 
     * are implicitly volatile.asm statements that have no output operands, 
     * including asm goto statements, are implicitly volatile.
     * Included the __volatile__ qualifier anyway to put in this note.
     */
    __asm__ __volatile__ ("cpsie i");
}

/* To disable all interrupts with programmable priority.
 * Refer: Refer http://www.ti.com/lit/ds/symlink/lm3s6965.pdf
 * Table 2-13 and Section 2-3-4
 */
static inline void irq_master_disable(void)
{
    __asm__ __volatile__ ("cpsid i");
}

#endif /* __IRQ_H__ */
