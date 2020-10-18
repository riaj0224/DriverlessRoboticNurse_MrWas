// FIRC = 4 MHz. BusClk = 4 MHz
// UART0: FIRC. UART1: BusClk. UART2: BusClk. TPM: FIRC. IIC: BusClk
MCG_C1 |= (1 << 6) + (1 << 1); //MCGOUTCLK : IRCLK. CG: Clock gate, MCGIRCLK enable pag 116
MCG_C2 |= 1;                   //Mux IRCLK : FIRC (4 MHz) pag 116
MCG_SC = 0;                    //Preescaler FIRC 1:1 pag 116

SIM_CLKDIV1 = 0;       //OUTDIV4=OUTDIV1= 1:1 pag 116. Busclk 4 MHz
SIM_SOPT2 |= 15 << 24; //Seleccion MCGIRCLK tanto para UART0 como para TPM
}