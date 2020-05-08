#ifndef GPIO_DEFS_H
#define GPIO_DEFS_H

#define MASK(x) (1UL << (x))

// Freedom KL25Z LEDs
#define RED_LED_POS (18)		// on port B
#define GREEN_LED_POS (19)	// on port B
#define BLUE_LED_POS (1)		// on port D

/* PORT - Peripheral instance base addresses */
#define PORTA 	(PORTA_BASE_PTR)
#define PORTB		(PORTB_BASE_PTR)
#define PORTC		(PORTC_BASE_PTR)
#define PORTD		(PORTD_BASE_PTR)
#define PORTE		(PORTE_BASE_PTR)

#define PTA 		(GPIOA_BASE_PTR)
#define PTB			(GPIOB_BASE_PTR)
#define PTC			(GPIOC_BASE_PTR)
#define PTD			(GPIOD_BASE_PTR)
#define PTE			(GPIOE_BASE_PTR)

#define SIM  		(SIM_BASE_PTR)

#define FGPIOA  (FGPIOA_BASE_PTR)
#define FGPIOB  (FGPIOB_BASE_PTR)
#define FGPIOC  (FGPIOC_BASE_PTR)
#define FGPIOD  (FGPIOD_BASE_PTR)
#define FGPIOE  (FGPIOE_BASE_PTR)

#define FPTA		FGPIOA
#define FPTB		FGPIOB
#define FPTC		FGPIOC
#define FPTD		FGPIOD
#define FPTE		FGPIOE

#define DAC0  (DAC0_BASE_PTR)

#define DMA0  	(DMA_BASE_PTR)
#define DMAMUX0 (DMAMUX0_BASE_PTR)

#define TPM0  (TPM0_BASE_PTR)

#define UART2  (UART2_BASE_PTR)

#define BIT_0 ( 1 << 0 )

#endif
// *******************************ARM University Program Copyright � ARM Ltd 2013*************************************   
