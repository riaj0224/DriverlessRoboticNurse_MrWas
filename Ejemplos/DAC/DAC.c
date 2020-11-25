#include "derivative.h" /* include peripheral declarations */

void DAC0_init(void);
void delayMs(int n);

int main (void) {
    int i;
    
	DAC0_init();    /* Configure DAC0 */
    
    while (1) {
        for (i = 0; i < 0x1000; i += 0x0010) {
            /* write value of i to DAC0 */
            DAC0_DAT0L = i & 0xff;       /* write low byte */
            DAC0_DAT0H = (i >> 8) & 0x0f;/* write high byte */
            delayMs(1);         /* delay 1ms */
        }
    }
}

void DAC0_init(void) {
	SIM_SCGC6 |= 0x80000000;   /* clock to DAC module */
	DAC0_C1 = 0;               /* disable the use of buffer */
	DAC0_C0 = 0x80 | 0x20;     /* enable DAC and use software trigger */
}

/* Delay n milliseconds
 * The CPU core clock is set to MCGFLLCLK at 41.94 MHz in SystemInit().
 */
void delayMs(int n) {
    int i;
    int j;
    for(i = 0 ; i < n; i++)
        for (j = 0; j < 7000; j++) {}
}

