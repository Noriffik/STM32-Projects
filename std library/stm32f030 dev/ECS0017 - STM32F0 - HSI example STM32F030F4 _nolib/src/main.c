#include "stm32f0xx.h"
#include <stdio.h>

#define PROJECTNAME "ECS0017 _nolib"
#define VERSION "V0.0.1"
#define TESTED "2015/06/01"

/*
SWD:
PA14: SWCLK
PA15: SWDIO

Pinout:
PF0: LED ORANGE
PF1: LED GREEN
*/

// globals
	// systick timer
	volatile uint32_t timer_delayCounter;

// prototypes
	// systick timer
	void
	timer_delay(volatile uint32_t time);
	void
	timer_initMilliseconds(void);
	void
	timer_interruptHandler(void);

	// HSI for PLL
	void
	hsi_setPLLClock(uint32_t RCC_PLLMul);

	// interrupts
	void
	SysTick_Handler(void);

int
main(void)
{
	// set HSI clock
	hsi_setPLLClock(RCC_PLLMul_12);

	// set systick clock
	timer_initMilliseconds();

	while(1){

	}

}




// systick timer
volatile uint32_t timer_delayCounter;
void
timer_delay(volatile uint32_t time){
	timer_delayCounter = time;
	while(timer_delayCounter!=0x00);
}
void
timer_initMilliseconds(void){
	  SysTick->LOAD  = (SystemCoreClock / 1000) - 1;                                  /* set reload register */

	  /* set Priority for Systick Interrupt */
	  SCB->SHP[_SHP_IDX(SysTick_IRQn)] = (SCB->SHP[_SHP_IDX(SysTick_IRQn)] & ~(0xFF << _BIT_SHIFT(SysTick_IRQn))) |
	          (((((1<<__NVIC_PRIO_BITS) - 1) << (8 - __NVIC_PRIO_BITS)) & 0xFF) << _BIT_SHIFT(SysTick_IRQn));

	  SysTick->VAL   = 0;                                          /* Load the SysTick Counter Value */
	  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
	                   SysTick_CTRL_TICKINT_Msk   |
	                   SysTick_CTRL_ENABLE_Msk;                    /* Enable SysTick IRQ and SysTick Timer */
}
void
timer_interruptHandler(void){
	if(timer_delayCounter!=0x00){(timer_delayCounter--);}
}

// HSI for PLL
void
hsi_setPLLClock(uint32_t RCC_PLLMul){
	// rcc deinit
	  /* Set HSION bit */
	  RCC->CR |= (uint32_t)0x00000001;

	#if defined (STM32F0XX_MD)
	  /* Reset SW[1:0], HPRE[3:0], PPRE[2:0], ADCPRE and MCOSEL[2:0] bits */
	  RCC->CFGR &= (uint32_t)0xF8FFB80C;
	#else
	  /* Reset SW[1:0], HPRE[3:0], PPRE[2:0], ADCPRE, MCOSEL[2:0], MCOPRE[2:0] and PLLNODIV bits */
	  RCC->CFGR &= (uint32_t)0x08FFB80C;
	#endif /* STM32F0XX_MD */

	  /* Reset HSEON, CSSON and PLLON bits */
	  RCC->CR &= (uint32_t)0xFEF6FFFF;

	  /* Reset HSEBYP bit */
	  RCC->CR &= (uint32_t)0xFFFBFFFF;

	  /* Reset PLLSRC, PLLXTPRE and PLLMUL[3:0] bits */
	  RCC->CFGR &= (uint32_t)0xFFC0FFFF;

	  /* Reset PREDIV1[3:0] bits */
	  RCC->CFGR2 &= (uint32_t)0xFFFFFFF0;

	  /* Reset USARTSW[1:0], I2CSW, CECSW and ADCSW bits */
	  RCC->CFGR3 &= (uint32_t)0xFFFFFEAC;

	  /* Reset HSI14 bit */
	  RCC->CR2 &= (uint32_t)0xFFFFFFFE;

	  /* Disable all interrupts */
	  RCC->CIR = 0x00000000;

	// pll disable
	  RCC->CR &= ~RCC_CR_PLLON;


	// initialize PLL
	  /* Clear PLL Source [16] and Multiplier [21:18] bits */
	  RCC->CFGR &= ~(RCC_CFGR_PLLMULL | RCC_CFGR_PLLSRC);

	  /* Set the PLL Source and Multiplier */
	  RCC->CFGR |= (uint32_t)(RCC_PLLSource_HSI_Div2 | RCC_PLLMul);

	// Flash: 1 WaitState for 24MHz < SysCLK < 48 MHz
	  uint32_t tmpreg = 0;
	  /* Read the ACR register */
	  tmpreg = FLASH->ACR;

	  /* Sets the Latency value */
	  tmpreg &= (uint32_t) (~((uint32_t)FLASH_ACR_LATENCY));
	  tmpreg |= FLASH_Latency_1;

	  /* Write the ACR register */
	  FLASH->ACR = tmpreg;

	  /* Enable the prefetch buffer */
	  FLASH->ACR |= FLASH_ACR_PRFTBE;

	// Set ADC clock to sync PCLK/4->12MHz
	  /* Clear ADCPRE bit */
	  RCC->CFGR &= ~RCC_CFGR_ADCPRE;
	  /* Set ADCPRE bits according to RCC_PCLK value */
	  RCC->CFGR |= RCC_ADCCLK_HSI14 & 0xFFFF;

	  /* Clear ADCSW bit */
	  RCC->CFGR3 &= ~RCC_CFGR3_ADCSW;
	  /* Set ADCSW bits according to RCC_ADCCLK value */
	  RCC->CFGR3 |= RCC_ADCCLK_HSI14 >> 16;

	// and turn the PLL back on again
	  RCC->CR |= RCC_CR_PLLON;

	// set PLL as system clock source
	  tmpreg = RCC->CFGR;

	  /* Clear SW[1:0] bits */
	  tmpreg &= ~RCC_CFGR_SW;

	  /* Set SW[1:0] bits according to RCC_SYSCLKSource value */
	  tmpreg |= RCC_SYSCLKSource_PLLCLK;

	  /* Store the new value */
	  RCC->CFGR = tmpreg;
}

// interrupts
void
SysTick_Handler(void)
{
  timer_interruptHandler();
}

