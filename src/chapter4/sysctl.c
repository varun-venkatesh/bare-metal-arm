#include <stdint.h>
#include "lm3s6965_memmap.h"
#include "sysctl.h"

#define FREQ_12MHZ 12000000u
#define FREQ_30KHZ 30000u
#define FREQ_32KHZ 32000u

/* System Control register map structure.
 * Refer: http://www.ti.com/lit/ds/symlink/lm3s6965.pdf Table 5-7
 */
typedef struct __attribute__((packed)){
    uint32_t DID0;              // 0x000 Device Identification 0
    uint32_t DID1;              // 0x004 Device Identification 1
    uint32_t DC0;               // 0x008 Device Capabilities 0
    uint32_t reserved1;         // 0x00C-0x00F reserved
    uint32_t DC1;               // 0x010 Device Capabilities 1
    uint32_t DC2;               // 0x014 Device Capabilities 2
    uint32_t DC3;               // 0x018 Device Capabilities 3
    uint32_t DC4;               // 0x01C Device Capabilities 4
    uint32_t reserved2[4];      // 0x020-0x02F reserved
    uint32_t PBORCTL;           // 0x030 Brown-Out Reset Control
    uint32_t LDOPCTL;           // 0x034 LDO Power Control
    uint32_t reserved3[2];      // 0x038-0x3F reserved
    uint32_t SRCR0;             // 0x040 Software Reset Control 0
    uint32_t SRCR1;             // 0x044 Software Reset Control 1
    uint32_t SRCR2;             // 0x048 Software Reset Control 2
    uint32_t reserved4;         // 0x04C-0x04F reserved
    uint32_t RIS;               // 0x050 Raw Interrupt Status
    uint32_t IMC;               // 0x054 Interrupt Mask Control
    uint32_t MISC;              // 0x058 Masked Interrupt Status and Clear
    uint32_t RESC;              // 0x05C Reset Cause
    uint32_t RCC;               // 0x060 Run-Mode Clock Configuration
    uint32_t PLLCFG;            // 0x064 XTAL to PLL Translation
    uint32_t reserved5[2];      // 0x068-0x06F reserved
    uint32_t RCC2;              // 0x070 Run-Mode Clock Configuration 2
    uint32_t reserved6[35];     // 0x074-0x0FF reserved
    uint32_t RCGC0;             // 0x100 Run Mode Clock Gating Control Register 0
    uint32_t RCGC1;             // 0x104 Run Mode Clock Gating Control Register 1
    uint32_t RCGC2;             // 0x108 Run Mode Clock Gating Control Register 2
    uint32_t reserved7;         // 0x10C-0x10F reserved
    uint32_t SCGC0;             // 0x110 Sleep Mode Clock Gating Control Register 0
    uint32_t SCGC1;             // 0x114 Sleep Mode Clock Gating Control Register 1
    uint32_t SCGC2;             // 0x118 Sleep Mode Clock Gating Control Register 2
    uint32_t reserved8;         // 0x11C-0x11F reserved
    uint32_t DCGC0;             // 0x120 Deep Sleep Mode Clock Gating Control Register 0
    uint32_t DCGC1;             // 0x124 Deep Sleep Mode Clock Gating Control Register 1
    uint32_t DCGC2;             // 0x128 Deep Sleep Mode Clock Gating Control Register 2
    uint32_t reserved9[6];      // 0x12C-0x140 reserved
    uint32_t DSLPCLKCFG;         // 0x144 Deep Sleep Clock Configuration
}sysctl_regs;

static volatile sysctl_regs *sysctl = (sysctl_regs*)SYS_CTL_BASE;

/* Possible values of the crystal frequency for the Main oscillator.
 * Refer: http://www.ti.com/lit/ds/symlink/lm3s6965.pdf 
 * Section 5.2.4.1 and Page 197.
 * Register RCC can be configured to aelect from among these crystal frequencies.
 */
static const uint32_t xtal_freq[] = 
{
    1000000,
    1843200,
    2000000,
    2457600,
    3579545,
    3686400,
    4000000,
    4096000,
    4915200,
    5000000,
    5120000,
    6000000,
    6144000,
    7372800,
    8000000,
    8192000    
};

/* The puropse of this function is to poll for the PLL lock interrupt
 * for a count of SYSCTL_PLLLOCK_DELAY. Depending on the interrupt or 
 * the count hitting zero (whatever happens earlier) we exit.
 */

static inline void sysctl_wait_pll_lock(void)
{
    uint32_t wait = SYSCTL_PLLLOCK_DELAY; 

    while(wait = SYSCTL_PLLLOCK_DELAY)
    {
        if(sysctl->RIS & SYSTCL_RIS_PLLRIS)
        {
            return;
        }

        wait--;
    }

    /* If we're here, it means the PLL lock interrupt wasn't
     * set within the timeout SYSCTL_PLLLOCK_DELAY.
     */
}

/* This is to introduce a delay of count. Note that we don't really have a 
 * timer in the system at this point - seeing as we are in the process building one - 
 * one  way to introduce delays is via counting up/down.
 */
static void __attribute__((naked)) sysctl_delay(uint32_t count)
{
    __asm__("subs r0, #1;"
            "bne sysctl_delay;"
            "bx lr;"
           );
}

/* Now we come to the business end of this module - set the system clock.
 * Refer: http://www.ti.com/lit/ds/symlink/lm3s6965.pdf 
 * Section 5.3 for how to go about.
 */
void sysctl_setclk(uint32_t cfg_rcc, uint32_t cfg_rcc2)
{
    uint32_t tmp_rcc, tmp_rcc2;
    uint32_t osc_delay = SYSCTL_FAST_OSCDELAY;

    // Get the current values of RCC and RCC2
    tmp_rcc = sysctl->RCC;
    tmp_rcc2 = sysctl->RCC2;
    

    // set BYPASS (bypass PLL) and clear USESYSDIV
    tmp_rcc |= SYSCTL_RCC_BYPASS;
    tmp_rcc &= ~(SYSCTL_RCC_USESYSDIV);
    tmp_rcc2 |= SYSCTL_RCC2_BYPASS2;

    sysctl->RCC = tmp_rcc;
    sysctl->RCC2 = tmp_rcc2;

    // check if MOSC or IOSC need turning on.
    if(((tmp_rcc & SYSCTL_RCC_IOSCDIS) != 0 && (cfg_rcc & SYSCTL_RCC_IOSCDIS) == 0) ||
       ((tmp_rcc & SYSCTL_RCC_MOSCDIS) != 0 && (cfg_rcc & SYSCTL_RCC_MOSCDIS) == 0))
    {
        // turn on the required oscillators. Don't turn any off for now
        tmp_rcc &= (~(SYSCTL_RCC_IOSCDIS | SYSCTL_RCC_MOSCDIS) |
                    (cfg_rcc & (SYSCTL_RCC_IOSCDIS | SYSCTL_RCC_MOSCDIS)));

        sysctl->RCC = tmp_rcc;

        /* Wait for the newly set oscillator(s) to settle.
         * This wait time would depend on 
         * the previous clock setting.
         */
        if((tmp_rcc2 & SYSCTL_RCC2_USERCC2))
        {
            if(((tmp_rcc2 & SYSCTL_RCC2_OSCSRC2_MASK) == SYSCTL_RCC2_OSCSRC2_30KHZ) ||
               ((tmp_rcc2 & SYSCTL_RCC2_OSCSRC2_MASK) == SYSCTL_RCC2_OSCSRC2_32KHZ))
            {
                osc_delay = SYSCTL_SLOW_OSCDELAY;
            }
        }
        else
        {
            if((tmp_rcc & SYSCTL_RCC_OSCSRC_MASK) == SYSCTL_RCC_OSCSRC_30KHZ) 
            {
                osc_delay = SYSCTL_SLOW_OSCDELAY;
            }
           
        }

        sysctl_delay(osc_delay);
    }

    // Select the crystal value and oscillator source. Clear PWRDWN to enable PLL
    tmp_rcc &= ~(SYSCTL_RCC_XTAL_MASK| SYSCTL_RCC_OSCSRC_MASK);
    tmp_rcc |= cfg_rcc & (SYSCTL_RCC_XTAL_MASK | SYSCTL_RCC_OSCSRC_MASK);

    tmp_rcc2 &= ~(SYSCTL_RCC2_USERCC2 | SYSCTL_RCC2_OSCSRC2_MASK);
    tmp_rcc2 |= cfg_rcc2 & (SYSCTL_RCC2_USERCC2 | SYSCTL_RCC2_OSCSRC2_MASK);

    tmp_rcc &= ~(SYSCTL_RCC_PWRDN);
    tmp_rcc |= cfg_rcc & SYSCTL_RCC_PWRDN;

    tmp_rcc2 &= ~(SYSCTL_RCC2_PWRDN2);
    tmp_rcc2 |= cfg_rcc2 & SYSCTL_RCC2_PWRDN2;

    //Clear the PLL lock interrupt
    sysctl->MISC |= SYSTCL_MISC_PLLIM;
    
    sysctl->RCC = tmp_rcc;
    sysctl->RCC2 = tmp_rcc2;

    //delay to ensure new crystal and oscillator source take effect.
    sysctl_delay(16);

    /* Set the USESYSDIV bit in RCC and appropriately set the system divider.
     * Disable the unused oscillator.
     */
    tmp_rcc &= ~(SYSCTL_RCC_USESYSDIV | SYSCTL_RCC_SYSDIV_MASK |
                  SYSCTL_RCC_IOSCDIS | SYSCTL_RCC_IOSCDIS);
    tmp_rcc |= cfg_rcc & (SYSCTL_RCC_USESYSDIV | SYSCTL_RCC_SYSDIV_MASK |
                  SYSCTL_RCC_IOSCDIS | SYSCTL_RCC_IOSCDIS);

    tmp_rcc2 &= ~(SYSCTL_RCC2_SYSDIV2_MASK);
    tmp_rcc2 |= cfg_rcc2 & (SYSCTL_RCC2_SYSDIV2_MASK);

    sysctl->RCC = tmp_rcc;
    sysctl->RCC2 = tmp_rcc2;

    // Enable the use of PLL by clearing the BYPASS
    if((cfg_rcc & SYSCTL_RCC_BYPASS) == 0)
    {
        // Waiting for the PLL to acquire a lock - poll for PLLRIS
        sysctl_wait_pll_lock();

        tmp_rcc  &= ~(SYSCTL_RCC_BYPASS); 
        tmp_rcc2 &= ~(SYSCTL_RCC2_BYPASS2);
    }

    sysctl->RCC = tmp_rcc;
    sysctl->RCC2 = tmp_rcc2;

    //delay to ensure system divider takes effect.
    sysctl_delay(16);
                    
}

/* This is function is for finding the system clock frequency.
 * Helps with setting time-periods/time-outs.
 * Refer: http://www.ti.com/lit/ds/symlink/lm3s6965.pdf 
 * Section 5-5 Pages 195-201
 */
uint32_t sysctl_getclk(void)
{
    uint32_t tmp_rcc, tmp_rcc2, tmp_pllcfg, clk_rt;

    // Read from the config registers RCC/RCC2
    tmp_rcc = sysctl->RCC;
    tmp_rcc2 = sysctl->RCC2;

    // determin the oscillator used (and if RCC2 overrides RCC)
    switch((tmp_rcc2 & SYSCTL_RCC2_USERCC2) ?
           (tmp_rcc2 & SYSCTL_RCC2_OSCSRC2_MASK) :
           (tmp_rcc & SYSCTL_RCC_OSCSRC_MASK))
    {
        case SYSCTL_RCC_OSCSRC_MOSC:
        {
            clk_rt = xtal_freq[(tmp_rcc & SYSCTL_RCC_XTAL_MASK) >> SYSCTL_RCC_XTAL_SHIFT];
            break;
        }

        case SYSCTL_RCC_OSCSRC_IOSC:
        {
            clk_rt = FREQ_12MHZ;
            break;
        }
        case SYSCTL_RCC_OSCSRC_IOSC_DIV4:
        {
            clk_rt = (FREQ_12MHZ)/4;
            break;
        }
        case SYSCTL_RCC_OSCSRC_30KHZ:
        {
            clk_rt = FREQ_30KHZ;
            break;
        }
        case SYSCTL_RCC2_OSCSRC2_32KHZ:
        {
            clk_rt = FREQ_32KHZ;
            break;
        }

    }

    // check if BYPASS is off, if it is, find the PLL frequency
    if(((tmp_rcc2 & SYSCTL_RCC2_USERCC2) != 0 && (tmp_rcc2 & SYSCTL_RCC2_BYPASS2) == 0) ||
       ((tmp_rcc2 & SYSCTL_RCC2_USERCC2) == 0 && (tmp_rcc & SYSCTL_RCC_BYPASS) == 0))
    {
        // Read PLLCFG to find the Fvalue and RValue
        tmp_pllcfg = sysctl->PLLCFG;
        
        // Compute PLLFreq = OSCFreq * F / (R + 1)
        clk_rt = clk_rt * 
                ((tmp_pllcfg & SYSCTL_PLLCFG_FVAL_MASK) >> SYSCTL_PLLCFG_FVAL_SHIFT) / 
                (((tmp_pllcfg & SYSCTL_PLLCFG_RVAL_MASK) >> SYSCTL_PLLCFG_RVAL_SHIFT) + 1);
    }

    /* apply the SYSDIV value to compute the frequency of the system clock.
     * if BYPASS is OFF, then PLL is ON, which means the PLL frequency computed earlier
     * must be divided by 2 (Refer:  http://www.ti.com/lit/ds/symlink/lm3s6965.pdf 
     * Section 5.2.4.2. Apply the appropriate SYSDIV factor - as in Table 5-5 and Table 5-6
     */
    if(tmp_rcc & SYSCTL_RCC_USESYSDIV)
    {
        if((tmp_rcc2 & SYSCTL_RCC2_USERCC2))
        {
            if((tmp_rcc2 & SYSCTL_RCC2_BYPASS2) == 0)
            {
                clk_rt = clk_rt/2;
            }

            clk_rt = clk_rt / 
                     (((tmp_rcc2 & SYSCTL_RCC2_SYSDIV2_MASK) >> 
                      SYSCTL_RCC2_SYSDIV2_SHIFT) + 1);
        }
        else 
        {
            if((tmp_rcc & SYSCTL_RCC_BYPASS) == 0)
            {
                clk_rt = clk_rt/2;
            }

            clk_rt = clk_rt / 
                     (((tmp_rcc & SYSCTL_RCC_SYSDIV_MASK) >> 
                      SYSCTL_RCC_SYSDIV_SHIFT) + 1);
        }
    }

    return (clk_rt);
}

/* This function helps us enable clocking for a peripheral whose 
 * base address is passed as a parameter. This is done by appropriatley configuring
 * RCGC1. Refer: http://www.ti.com/lit/ds/symlink/lm3s6965.pdf Section 5-5 Pages 220-222
 */
void sysctl_periph_clk_enable(uint32_t periph)
{
    switch(periph)
    {
        case UART0_BASE:
            sysctl->RCGC1 |= SYSCTL_RCGC1_UART0;
            break;
        case UART1_BASE:
            sysctl->RCGC1 |= SYSCTL_RCGC1_UART1;
            break;
        case UART2_BASE:
            sysctl->RCGC1 |= SYSCTL_RCGC1_UART2;
            break;
        default:
            break;
    }

}
