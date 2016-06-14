/**
  ******************************************************************************
  * @file    stm32f0xx_hal_rcc.c
  * @author  MCD Application Team
  * @version V1.3.0
  * @date    26-June-2015
  * @brief   RCC HAL module driver.
  *          This file provides firmware functions to manage the following 
  *          functionalities of the Reset and Clock Control (RCC) peripheral:
  *           + Initialization and de-initialization functions
  *           + Peripheral Control functions
  *       
  @verbatim                
  ==============================================================================
                      ##### RCC specific features #####
  ==============================================================================
    [..]  
      After reset the device is running from Internal High Speed oscillator
      (HSI 8MHz) with Flash 0 wait state, Flash prefetch buffer is enabled, 
      and all peripherals are off except internal SRAM, Flash and JTAG.
      (+) There is no prescaler on High speed (AHB) and Low speed (APB) busses;
          all peripherals mapped on these busses are running at HSI speed.
      (+) The clock for all peripherals is switched off, except the SRAM and FLASH.
      (+) All GPIOs are in input floating state, except the JTAG pins which
          are assigned to be used for debug purpose.
    [..] Once the device started from reset, the user application has to:
      (+) Configure the clock source to be used to drive the System clock
          (if the application needs higher frequency/performance)
      (+) Configure the System clock frequency and Flash settings  
      (+) Configure the AHB and APB busses prescalers
      (+) Enable the clock for the peripheral(s) to be used
      (+) Configure the clock source(s) for peripherals whose clocks are not
          derived from the System clock (RTC, ADC, I2C, USART, TIM, USB FS, etc..)

                      ##### RCC Limitations #####
  ==============================================================================
    [..]  
      A delay between an RCC peripheral clock enable and the effective peripheral 
      enabling should be taken into account in order to manage the peripheral read/write 
      from/to registers.
      (+) This delay depends on the peripheral mapping.
        (++) AHB & APB peripherals, 1 dummy read is necessary

    [..]  
      Workarounds:
      (#) For AHB & APB peripherals, a dummy read to the peripheral register has been
          inserted in each __HAL_RCC_PPP_CLK_ENABLE() macro.

  @endverbatim
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2015 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************  
*/ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/** @addtogroup STM32F0xx_HAL_Driver
  * @{
  */

/** @defgroup RCC RCC
* @brief RCC HAL module driver
  * @{
  */

#ifdef HAL_RCC_MODULE_ENABLED

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/** @defgroup RCC_Private_Constants RCC Private Constants
 * @{
 */
/**
  * @}
  */
/* Private macro -------------------------------------------------------------*/
/** @defgroup RCC_Private_Macros RCC Private Macros
  * @{
  */

#define MCO1_CLK_ENABLE()     __HAL_RCC_GPIOA_CLK_ENABLE()
#define MCO1_GPIO_PORT        GPIOA
#define MCO1_PIN              GPIO_PIN_8

/**
  * @}
  */

/* Private variables ---------------------------------------------------------*/
/** @defgroup RCC_Private_Variables RCC Private Variables
  * @{
  */
const uint8_t aAPBAHBPrescTable[16]       = {0, 0, 0, 0, 1, 2, 3, 4, 1, 2, 3, 4, 6, 7, 8, 9};
/**
  * @}
  */

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup RCC_Exported_Functions RCC Exported Functions
  * @{
  */

/** @defgroup RCC_Exported_Functions_Group1 Initialization and de-initialization functions 
  *  @brief    Initialization and Configuration functions 
  *
  @verbatim    
  ===============================================================================
           ##### Initialization and de-initialization function #####
  ===============================================================================
    [..]
      This section provides functions allowing to configure the internal/external oscillators
      (HSE, HSI, HSI14, HSI48, LSE, LSI, PLL, CSS and MCO) and the System busses clocks (SYSCLK, 
      AHB and APB1).

    [..] Internal/external clock and PLL configuration
      (#) HSI (high-speed internal), 8 MHz factory-trimmed RC used directly or through
          the PLL as System clock source.
             The HSI clock can be used also to clock the USART and I2C peripherals.

      (#) HSI14 (high-speed internal), 14 MHz factory-trimmed RC used directly to clock 
             the ADC peripheral.

      (#) LSI (low-speed internal), ~40 KHz low consumption RC used as IWDG and/or RTC
          clock source.

      (#) HSE (high-speed external), 4 to 32 MHz crystal oscillator used directly or
          through the PLL as System clock source. Can be used also as RTC clock source.

      (#) LSE (low-speed external), 32 KHz oscillator used as RTC clock source.   

      (#) PLL (clocked by HSI, HSI48 or HSE), featuring different output clocks:
       (++) The first output is used to generate the high speed system clock (up to 48 MHz)
       (++) The second output is used to generate the clock for the USB FS (48 MHz)
       (++) The third output may be used to generate the clock for the TIM, I2C and USART 
            peripherals (up to 48 MHz)

      (#) CSS (Clock security system), once enable using the macro __HAL_RCC_CSS_ENABLE()
          and if a HSE clock failure occurs(HSE used directly or through PLL as System 
          clock source), the System clockis automatically switched to HSI and an interrupt
          is generated if enabled. The interrupt is linked to the Cortex-M0 NMI 
          (Non-Maskable Interrupt) exception vector.   

      (#) MCO (microcontroller clock output), used to output SYSCLK, HSI, HSE, LSI, LSE or PLL
          clock (divided by 2) output on pin (such as PA8 pin).

    [..] System, AHB and APB busses clocks configuration  
      (#) Several clock sources can be used to drive the System clock (SYSCLK): HSI,
          HSE and PLL.
          The AHB clock (HCLK) is derived from System clock through configurable 
          prescaler and used to clock the CPU, memory and peripherals mapped 
          on AHB bus (DMA, GPIO...). APB1 (PCLK1) clock is derived
          from AHB clock through configurable prescalers and used to clock 
          the peripherals mapped on these busses. You can use 
          "HAL_RCC_GetSysClockFreq()" function to retrieve the frequencies of these clocks.  

      (#) All the peripheral clocks are derived from the System clock (SYSCLK) except:
        (++) The FLASH program/erase clock  which is always HSI 8MHz clock.
        (++) The USB 48 MHz clock which is derived from the PLL VCO clock.
        (++) The USART clock which can be derived as well from HSI 8MHz, LSI or LSE.
        (++) The I2C clock which can be derived as well from HSI 8MHz clock.
        (++) The ADC clock which is derived from PLL output.
        (++) The RTC clock which is derived from the LSE, LSI or 1 MHz HSE_RTC
             (HSE divided by a programmable prescaler). The System clock (SYSCLK)
             frequency must be higher or equal to the RTC clock frequency.
        (++) IWDG clock which is always the LSI clock.

      (#) For the STM32F0xx devices, the maximum frequency of the SYSCLK, HCLK and PCLK1 is 48 MHz,
          Depending on the SYSCLK frequency, the flash latency should be adapted accordingly:
          +-----------------------------------------------+
          | Latency       | SYSCLK clock frequency (MHz)  |
          |---------------|-------------------------------|
          |0WS(1CPU cycle)|       0 < SYSCLK <= 24        |
          |---------------|-------------------------------|
          |1WS(2CPU cycle)|      24 < SYSCLK <= 48        |
          +-----------------------------------------------+

      (#) After reset, the System clock source is the HSI (8 MHz) with 0 WS and
          prefetch is disabled.
  @endverbatim
  * @{
  */

/**
  * @brief  Resets the RCC clock configuration to the default reset state.
  * @note   The default reset state of the clock configuration is given below:
  *            - HSI ON and used as system clock source
  *            - HSE and PLL OFF
  *            - AHB, APB1 prescaler set to 1.
  *            - CSS and MCO1 OFF
  *            - All interrupts disabled
  * @note   This function doesn't modify the configuration of the
  *            - Peripheral clocks  
  *            - LSI, LSE and RTC clocks 
  * @retval None
  */
void HAL_RCC_DeInit(void)
{
  /* Set HSION bit, HSITRIM[4:0] bits to the reset value*/
  SET_BIT(RCC->CR, RCC_CR_HSION | RCC_CR_HSITRIM_4); 

  /* Reset SW[1:0], HPRE[3:0], PPRE[2:0] and MCOSEL[2:0] bits */
  CLEAR_BIT(RCC->CFGR, RCC_CFGR_SW | RCC_CFGR_HPRE | RCC_CFGR_PPRE | RCC_CFGR_MCO);
  
  /* Reset HSEON, CSSON, PLLON bits */
  CLEAR_BIT(RCC->CR, RCC_CR_PLLON | RCC_CR_CSSON | RCC_CR_HSEON);
  
  /* Reset HSEBYP bit */
  CLEAR_BIT(RCC->CR, RCC_CR_HSEBYP);
  
  /* Reset CFGR register */
  CLEAR_REG(RCC->CFGR);
  
  /* Reset CFGR2 register */
  CLEAR_REG(RCC->CFGR2);
  
  /* Reset CFGR3 register */
  CLEAR_REG(RCC->CFGR3);
  
  /* Disable all interrupts */
  CLEAR_REG(RCC->CIR);
}

/**
  * @brief  Initializes the RCC Oscillators according to the specified parameters in the
  *         RCC_OscInitTypeDef.
  * @param  RCC_OscInitStruct: pointer to an RCC_OscInitTypeDef structure that
  *         contains the configuration information for the RCC Oscillators.
  * @note   The PLL is not disabled when used as system clock.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RCC_OscConfig(RCC_OscInitTypeDef  *RCC_OscInitStruct)
{
   uint32_t tickstart = 0;
  
  /* Check the parameters */
  assert_param(RCC_OscInitStruct != NULL);
  assert_param(IS_RCC_OSCILLATORTYPE(RCC_OscInitStruct->OscillatorType));
  
  /*------------------------------- HSE Configuration ------------------------*/ 
  if(((RCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_HSE) == RCC_OSCILLATORTYPE_HSE)
  {
    /* Check the parameters */
    assert_param(IS_RCC_HSE(RCC_OscInitStruct->HSEState));
        
    /* When the HSE is used as system clock or clock source for PLL in these cases it is not allowed to be disabled */
    if((__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_HSE) 
       || ((__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_PLLCLK) && (__HAL_RCC_GET_PLL_OSCSOURCE() == RCC_PLLSOURCE_HSE)))
    {
      if((__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) != RESET) && (RCC_OscInitStruct->HSEState == RCC_HSE_OFF))
      {
        return HAL_ERROR;
      }
    }
    else
    {
      /* Reset HSEON and HSEBYP bits before configuring the HSE --------------*/
      __HAL_RCC_HSE_CONFIG(RCC_HSE_OFF);
      
      /* Get Start Tick*/
      tickstart = HAL_GetTick();
      
      /* Wait till HSE is disabled */  
      while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) != RESET)
      {
        if((HAL_GetTick() - tickstart ) > HSE_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
      
      /* Set the new HSE configuration ---------------------------------------*/
      __HAL_RCC_HSE_CONFIG(RCC_OscInitStruct->HSEState);
      
      /* Check the HSE State */
      if(RCC_OscInitStruct->HSEState != RCC_HSE_OFF)
      {
        /* Get Start Tick*/
        tickstart = HAL_GetTick();
        
        /* Wait till HSE is ready */  
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) == RESET)
        {
          if((HAL_GetTick() - tickstart ) > HSE_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
      else
      {
        /* Get Start Tick*/
        tickstart = HAL_GetTick();
        
        /* Wait till HSE is bypassed or disabled */
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) != RESET)
        {
           if((HAL_GetTick() - tickstart ) > HSE_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
    }
  }
  /*----------------------------- HSI Configuration --------------------------*/ 
  if(((RCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_HSI) == RCC_OSCILLATORTYPE_HSI)
  {
    /* Check the parameters */
    assert_param(IS_RCC_HSI(RCC_OscInitStruct->HSIState));
    assert_param(IS_RCC_CALIBRATION_VALUE(RCC_OscInitStruct->HSICalibrationValue));
    
    /* Check if HSI is used as system clock or as PLL source when PLL is selected as system clock */ 
    if((__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_HSI) 
       || ((__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_PLLCLK) && (__HAL_RCC_GET_PLL_OSCSOURCE() == RCC_PLLSOURCE_HSI)))
    {
      /* When HSI is used as system clock it will not disabled */
      if((__HAL_RCC_GET_FLAG(RCC_FLAG_HSIRDY) != RESET) && (RCC_OscInitStruct->HSIState != RCC_HSI_ON))
      {
        return HAL_ERROR;
      }
      /* Otherwise, just the calibration is allowed */
      else
      {
        /* Adjusts the Internal High Speed oscillator (HSI) calibration value.*/
        __HAL_RCC_HSI_CALIBRATIONVALUE_ADJUST(RCC_OscInitStruct->HSICalibrationValue);
      }
    }
    else
    {
      /* Check the HSI State */
      if(RCC_OscInitStruct->HSIState != RCC_HSI_OFF)
      {
       /* Enable the Internal High Speed oscillator (HSI). */
        __HAL_RCC_HSI_ENABLE();
        
        /* Get Start Tick*/
        tickstart = HAL_GetTick();
        
        /* Wait till HSI is ready */  
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSIRDY) == RESET)
        {
          if((HAL_GetTick() - tickstart ) > HSI_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
                
        /* Adjusts the Internal High Speed oscillator (HSI) calibration value.*/
        __HAL_RCC_HSI_CALIBRATIONVALUE_ADJUST(RCC_OscInitStruct->HSICalibrationValue);
      }
      else
      {
        /* Disable the Internal High Speed oscillator (HSI). */
        __HAL_RCC_HSI_DISABLE();
        
        /* Get Start Tick*/
        tickstart = HAL_GetTick();
        
        /* Wait till HSI is disabled */  
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSIRDY) != RESET)
        {
          if((HAL_GetTick() - tickstart ) > HSI_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
    }
  }
  /*------------------------------ LSI Configuration -------------------------*/ 
  if(((RCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_LSI) == RCC_OSCILLATORTYPE_LSI)
  {
    /* Check the parameters */
    assert_param(IS_RCC_LSI(RCC_OscInitStruct->LSIState));
    
    /* Check the LSI State */
    if(RCC_OscInitStruct->LSIState != RCC_LSI_OFF)
    {
      /* Enable the Internal Low Speed oscillator (LSI). */
      __HAL_RCC_LSI_ENABLE();
      
      /* Get Start Tick*/
      tickstart = HAL_GetTick();
      
      /* Wait till LSI is ready */  
      while(__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) == RESET)
      {
        if((HAL_GetTick() - tickstart ) > LSI_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
    }
    else
    {
      /* Disable the Internal Low Speed oscillator (LSI). */
      __HAL_RCC_LSI_DISABLE();
      
      /* Get Start Tick*/
      tickstart = HAL_GetTick();
      
      /* Wait till LSI is disabled */  
      while(__HAL_RCC_GET_FLAG(RCC_FLAG_LSIRDY) != RESET)
      {
        if((HAL_GetTick() - tickstart ) > LSI_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
    }
  }
  /*------------------------------ LSE Configuration -------------------------*/ 
  if(((RCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_LSE) == RCC_OSCILLATORTYPE_LSE)
  {
    /* Check the parameters */
    assert_param(IS_RCC_LSE(RCC_OscInitStruct->LSEState));
    
    /* Enable Power Clock*/
    __HAL_RCC_PWR_CLK_ENABLE();
    
    /* Enable write access to Backup domain */
    SET_BIT(PWR->CR, PWR_CR_DBP);

    /* Wait for Backup domain Write protection disable */
    tickstart = HAL_GetTick();
    
    while((PWR->CR & PWR_CR_DBP) == RESET)
    {
      if((HAL_GetTick() - tickstart ) > RCC_DBP_TIMEOUT_VALUE)
      {
        return HAL_TIMEOUT;
      }      
    }
    
    /* Reset LSEON and LSEBYP bits before configuring the LSE ----------------*/
    __HAL_RCC_LSE_CONFIG(RCC_LSE_OFF);
    
    /* Get Start Tick*/
    tickstart = HAL_GetTick();
    
    /* Wait till LSE is disabled */  
    while(__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) != RESET)
    {
      if((HAL_GetTick() - tickstart ) > RCC_LSE_TIMEOUT_VALUE)
      {
        return HAL_TIMEOUT;
      }
    }
    
    /* Set the new LSE configuration -----------------------------------------*/
    __HAL_RCC_LSE_CONFIG(RCC_OscInitStruct->LSEState);
    /* Check the LSE State */
    if(RCC_OscInitStruct->LSEState != RCC_LSE_OFF)
    {
      /* Get Start Tick*/
      tickstart = HAL_GetTick();
      
      /* Wait till LSE is ready */  
      while(__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) == RESET)
      {
        if((HAL_GetTick() - tickstart ) > RCC_LSE_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
    }
    else
    {
      /* Get Start Tick*/
      tickstart = HAL_GetTick();
      
      /* Wait till LSE is disabled */  
      while(__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY) != RESET)
      {
        if((HAL_GetTick() - tickstart ) > RCC_LSE_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
    }
  }

  /*----------------------------- HSI14 Configuration --------------------------*/
  if(((RCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_HSI14) == RCC_OSCILLATORTYPE_HSI14)
  {
    /* Check the parameters */
    assert_param(IS_RCC_HSI14(RCC_OscInitStruct->HSI14State));
    assert_param(IS_RCC_CALIBRATION_VALUE(RCC_OscInitStruct->HSI14CalibrationValue));

    /* Check the HSI14 State */
    if(RCC_OscInitStruct->HSI14State == RCC_HSI14_ON)
    {
      /* Disable ADC control of the Internal High Speed oscillator HSI14 */
      __HAL_RCC_HSI14ADC_DISABLE();

      /* Enable the Internal High Speed oscillator (HSI). */
      __HAL_RCC_HSI14_ENABLE();

      /* Get timeout */
      tickstart = HAL_GetTick();
      
      /* Wait till HSI is ready */  
      while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSI14RDY) == RESET)
      {
        if((HAL_GetTick() - tickstart) > HSI14_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }      
      } 

      /* Adjusts the Internal High Speed oscillator 14Mhz (HSI14) calibration value. */
      __HAL_RCC_HSI14_CALIBRATIONVALUE_ADJUST(RCC_OscInitStruct->HSI14CalibrationValue);
    }
    else if(RCC_OscInitStruct->HSI14State == RCC_HSI14_ADC_CONTROL)
    {
      /* Enable ADC control of the Internal High Speed oscillator HSI14 */
      __HAL_RCC_HSI14ADC_ENABLE();

      /* Adjusts the Internal High Speed oscillator 14Mhz (HSI14) calibration value. */
      __HAL_RCC_HSI14_CALIBRATIONVALUE_ADJUST(RCC_OscInitStruct->HSI14CalibrationValue);
    }
    else
    {
      /* Disable ADC control of the Internal High Speed oscillator HSI14 */
      __HAL_RCC_HSI14ADC_DISABLE();

      /* Disable the Internal High Speed oscillator (HSI). */
      __HAL_RCC_HSI14_DISABLE();

      /* Get timeout */
      tickstart = HAL_GetTick();
      
      /* Wait till HSI is ready */  
      while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSI14RDY) != RESET)
      {
        if((HAL_GetTick() - tickstart) > HSI14_TIMEOUT_VALUE)
        {
          return HAL_TIMEOUT;
        }
      }
    }
  }

#if defined(RCC_CR2_HSI48ON)
  /*----------------------------- HSI48 Configuration --------------------------*/
  if(((RCC_OscInitStruct->OscillatorType) & RCC_OSCILLATORTYPE_HSI48) == RCC_OSCILLATORTYPE_HSI48)
  {
    /* Check the parameters */
    assert_param(IS_RCC_HSI48(RCC_OscInitStruct->HSI48State));

    /* When the HSI48 is used as system clock it is not allowed to be disabled */
    if((__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_HSI48) ||
       ((__HAL_RCC_GET_SYSCLK_SOURCE() == RCC_SYSCLKSOURCE_STATUS_PLLCLK) && (__HAL_RCC_GET_PLL_OSCSOURCE() == RCC_PLLSOURCE_HSI48)))
    {
      if((__HAL_RCC_GET_FLAG(RCC_FLAG_HSI48RDY) != RESET) && (RCC_OscInitStruct->HSI48State != RCC_HSI48_ON))
      {
        return HAL_ERROR;
      }
    }
    else
    {
      /* Check the HSI State */
      if(RCC_OscInitStruct->HSI48State != RCC_HSI48_OFF)
      {
        /* Enable the Internal High Speed oscillator (HSI48). */
        __HAL_RCC_HSI48_ENABLE();

        /* Get timeout */
        tickstart = HAL_GetTick();
      
        /* Wait till HSI is ready */  
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSI48RDY) == RESET)
        {
          if((HAL_GetTick() - tickstart) > HSI48_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        } 
      }
      else
      {
        /* Disable the Internal High Speed oscillator (HSI48). */
        __HAL_RCC_HSI48_DISABLE();

        /* Get timeout */
        tickstart = HAL_GetTick();
      
        /* Wait till HSI is ready */  
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSI48RDY) != RESET)
        {
          if((HAL_GetTick() - tickstart) > HSI48_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
    }
  }
#endif /* RCC_CR2_HSI48ON */
       
  /*-------------------------------- PLL Configuration -----------------------*/
  /* Check the parameters */
  assert_param(IS_RCC_PLL(RCC_OscInitStruct->PLL.PLLState));
  if ((RCC_OscInitStruct->PLL.PLLState) != RCC_PLL_NONE)
  {
    /* Check if the PLL is used as system clock or not */
    if(__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_PLLCLK)
    { 
      if((RCC_OscInitStruct->PLL.PLLState) == RCC_PLL_ON)
      {
        /* Check the parameters */
        assert_param(IS_RCC_PLLSOURCE(RCC_OscInitStruct->PLL.PLLSource));
        assert_param(IS_RCC_PLL_MUL(RCC_OscInitStruct->PLL.PLLMUL));
        assert_param(IS_RCC_PREDIV(RCC_OscInitStruct->PLL.PREDIV));
  
        /* Disable the main PLL. */
        __HAL_RCC_PLL_DISABLE();
        
        /* Get Start Tick*/
        tickstart = HAL_GetTick();
        
        /* Wait till PLL is disabled */
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY)  != RESET)
        {
          if((HAL_GetTick() - tickstart ) > PLL_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
        
        /* Configure the main PLL clock source, predivider and multiplication factor. */
        __HAL_RCC_PLL_CONFIG(RCC_OscInitStruct->PLL.PLLSource,
                             RCC_OscInitStruct->PLL.PREDIV,
                             RCC_OscInitStruct->PLL.PLLMUL);
        /* Enable the main PLL. */
        __HAL_RCC_PLL_ENABLE();
        
        /* Get Start Tick*/
        tickstart = HAL_GetTick();
        
        /* Wait till PLL is ready */
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY)  == RESET)
        {
          if((HAL_GetTick() - tickstart ) > PLL_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
      else
      {
        /* Disable the main PLL. */
        __HAL_RCC_PLL_DISABLE();
 
        /* Get Start Tick*/
        tickstart = HAL_GetTick();
        
        /* Wait till PLL is disabled */  
        while(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY)  != RESET)
        {
          if((HAL_GetTick() - tickstart ) > PLL_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
    }
    else
    {
      return HAL_ERROR;
    }
  }
  
  return HAL_OK;
}

/**
  * @brief  Initializes the CPU, AHB and APB busses clocks according to the specified 
  *         parameters in the RCC_ClkInitStruct.
  * @param  RCC_ClkInitStruct: pointer to an RCC_OscInitTypeDef structure that
  *         contains the configuration information for the RCC peripheral.
  * @param  FLatency: FLASH Latency                   
  *          This parameter can be one of the following values:
  *            @arg FLASH_LATENCY_0:  FLASH 0 Latency cycle
  *            @arg FLASH_LATENCY_1:  FLASH 1 Latency cycle
  * @note   The SystemCoreClock CMSIS variable is used to store System Clock Frequency 
  *         and updated by HAL_RCC_GetHCLKFreq() function called within this function
  *
  * @note   The HSI is used (enabled by hardware) as system clock source after
  *         startup from Reset, wake-up from STOP and STANDBY mode, or in case
  *         of failure of the HSE used directly or indirectly as system clock
  *         (if the Clock Security System CSS is enabled).
  *           
  * @note   A switch from one clock source to another occurs only if the target
  *         clock source is ready (clock stable after startup delay or PLL locked). 
  *         If a clock source which is not yet ready is selected, the switch will
  *         occur when the clock source will be ready. 
  *         You can use HAL_RCC_GetClockConfig() function to know which clock is
  *         currently used as system clock source.
  * @retval HAL status
  */
HAL_StatusTypeDef HAL_RCC_ClockConfig(RCC_ClkInitTypeDef  *RCC_ClkInitStruct, uint32_t FLatency)
{
  uint32_t tickstart = 0;
  
  /* Check the parameters */
  assert_param(RCC_ClkInitStruct != NULL);
  assert_param(IS_RCC_CLOCKTYPE(RCC_ClkInitStruct->ClockType));
  assert_param(IS_FLASH_LATENCY(FLatency));
  
  /* To correctly read data from FLASH memory, the number of wait states (LATENCY) 
  must be correctly programmed according to the frequency of the CPU clock 
    (HCLK) of the device. */
  
  /* Increasing the CPU frequency */
  if(FLatency > (FLASH->ACR & FLASH_ACR_LATENCY))
  {    
    /* Program the new number of wait states to the LATENCY bits in the FLASH_ACR register */
    __HAL_FLASH_SET_LATENCY(FLatency);
    
    /* Check that the new number of wait states is taken into account to access the Flash
    memory by reading the FLASH_ACR register */
    if((FLASH->ACR & FLASH_ACR_LATENCY) != FLatency)
    {
      return HAL_ERROR;
    }
    
    /*-------------------------- HCLK Configuration --------------------------*/
    if(((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_HCLK) == RCC_CLOCKTYPE_HCLK)
    {
      assert_param(IS_RCC_HCLK(RCC_ClkInitStruct->AHBCLKDivider));
      MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_ClkInitStruct->AHBCLKDivider);
    }

    /*------------------------- SYSCLK Configuration ---------------------------*/ 
    if(((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_SYSCLK) == RCC_CLOCKTYPE_SYSCLK)
    {    
      assert_param(IS_RCC_SYSCLKSOURCE(RCC_ClkInitStruct->SYSCLKSource));
      
      /* HSE is selected as System Clock Source */
      if(RCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_HSE)
      {
        /* Check the HSE ready flag */  
        if(__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) == RESET)
        {
          return HAL_ERROR;
        }
      }
      /* PLL is selected as System Clock Source */
      else if(RCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_PLLCLK)
      {
        /* Check the PLL ready flag */  
        if(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET)
        {
          return HAL_ERROR;
        }
      }
#if defined(RCC_CR2_HSI48ON)
      /* HSI48 is selected as System Clock Source */
      else if(RCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_HSI48)
      {
        /* Check the HSI48 ready flag */
        if(__HAL_RCC_GET_FLAG(RCC_FLAG_HSI48RDY) == RESET)
        {
          return HAL_ERROR;
        }
      }
#endif /* RCC_CR2_HSI48ON */
      /* HSI is selected as System Clock Source */
      else
      {
        /* Check the HSI ready flag */  
        if(__HAL_RCC_GET_FLAG(RCC_FLAG_HSIRDY) == RESET)
        {
          return HAL_ERROR;
        }
      }
      MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_ClkInitStruct->SYSCLKSource);
      
      /* Get Start Tick*/
      tickstart = HAL_GetTick();
      
      if(RCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_HSE)
      {
        while (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_HSE)
        {
          if((HAL_GetTick() - tickstart ) > CLOCKSWITCH_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
      else if(RCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_PLLCLK)
      {
        while (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_PLLCLK)
        {
          if((HAL_GetTick() - tickstart ) > CLOCKSWITCH_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
#if defined(RCC_CR2_HSI48ON)
      else if(RCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_HSI48)
      {
        while (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_HSI48)
        {
          if((HAL_GetTick() - tickstart) > CLOCKSWITCH_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
#endif /* RCC_CR2_HSI48ON */
      else
      {
        while (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_HSI)
        {
          if((HAL_GetTick() - tickstart ) > CLOCKSWITCH_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }      
    }    
  }
  /* Decreasing the CPU frequency */
  else
  {
    /*-------------------------- HCLK Configuration --------------------------*/
    if(((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_HCLK) == RCC_CLOCKTYPE_HCLK)
    {
      assert_param(IS_RCC_HCLK(RCC_ClkInitStruct->AHBCLKDivider));
      MODIFY_REG(RCC->CFGR, RCC_CFGR_HPRE, RCC_ClkInitStruct->AHBCLKDivider);
    }
    
    /*------------------------- SYSCLK Configuration -------------------------*/
    if(((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_SYSCLK) == RCC_CLOCKTYPE_SYSCLK)
    {    
      assert_param(IS_RCC_SYSCLKSOURCE(RCC_ClkInitStruct->SYSCLKSource));
      
      /* HSE is selected as System Clock Source */
      if(RCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_HSE)
      {
        /* Check the HSE ready flag */  
        if(__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) == RESET)
        {
          return HAL_ERROR;
        }
      }
      /* PLL is selected as System Clock Source */
      else if(RCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_PLLCLK)
      {
        /* Check the PLL ready flag */  
        if(__HAL_RCC_GET_FLAG(RCC_FLAG_PLLRDY) == RESET)
        {
          return HAL_ERROR;
        }
      }
#if defined(RCC_CR2_HSI48ON)
      /* HSI48 is selected as System Clock Source */
      else if(RCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_HSI48)
      {
        /* Check the HSI48 ready flag */
        if(__HAL_RCC_GET_FLAG(RCC_FLAG_HSI48RDY) == RESET)
        {
          return HAL_ERROR;
        }
      }
#endif /* RCC_CR2_HSI48ON */
      /* HSI is selected as System Clock Source */
      else
      {
        /* Check the HSI ready flag */  
        if(__HAL_RCC_GET_FLAG(RCC_FLAG_HSIRDY) == RESET)
        {
          return HAL_ERROR;
        }
      }
      MODIFY_REG(RCC->CFGR, RCC_CFGR_SW, RCC_ClkInitStruct->SYSCLKSource);
      
      /* Get Start Tick*/
      tickstart = HAL_GetTick();
      
      if(RCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_HSE)
      {
        while (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_HSE)
        {
          if((HAL_GetTick() - tickstart ) > CLOCKSWITCH_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
      else if(RCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_PLLCLK)
      {
        while (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_PLLCLK)
        {
          if((HAL_GetTick() - tickstart ) > CLOCKSWITCH_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
#if defined(RCC_CR2_HSI48ON)
      else if(RCC_ClkInitStruct->SYSCLKSource == RCC_SYSCLKSOURCE_HSI48)
      {
        while (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_HSI48)
        {
          if((HAL_GetTick() - tickstart) > CLOCKSWITCH_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }
#endif /* RCC_CR2_HSI48ON */
      else
      {
        while (__HAL_RCC_GET_SYSCLK_SOURCE() != RCC_SYSCLKSOURCE_STATUS_HSI)
        {
          if((HAL_GetTick() - tickstart ) > CLOCKSWITCH_TIMEOUT_VALUE)
          {
            return HAL_TIMEOUT;
          }
        }
      }      
    } 
    
    /* Program the new number of wait states to the LATENCY bits in the FLASH_ACR register */
    __HAL_FLASH_SET_LATENCY(FLatency);
    
    /* Check that the new number of wait states is taken into account to access the Flash
    memory by reading the FLASH_ACR register */
    if((FLASH->ACR & FLASH_ACR_LATENCY) != FLatency)
    {
      return HAL_ERROR;
    }
  }
  
  /*-------------------------- PCLK1 Configuration ---------------------------*/ 
  if(((RCC_ClkInitStruct->ClockType) & RCC_CLOCKTYPE_PCLK1) == RCC_CLOCKTYPE_PCLK1)
  {
    assert_param(IS_RCC_PCLK(RCC_ClkInitStruct->APB1CLKDivider));
    MODIFY_REG(RCC->CFGR, RCC_CFGR_PPRE, RCC_ClkInitStruct->APB1CLKDivider);
  }
  
  /* Configure the source of time base considering new system clocks settings*/
  HAL_InitTick (TICK_INT_PRIORITY);
  
  return HAL_OK;
}

/**
  * @}
  */

/** @defgroup RCC_Exported_Functions_Group2 Peripheral Control functions 
  *  @brief   RCC clocks control functions 
  *
  @verbatim   
  ===============================================================================
                  ##### Peripheral Control functions #####
  ===============================================================================  
    [..]
    This subsection provides a set of functions allowing to control the RCC Clocks 
    frequencies.

  @endverbatim
  * @{
  */

/**
  * @brief  Selects the clock source to output on MCO pin.
  * @note   MCO pin should be configured in alternate function mode.
  * @param  RCC_MCOx: specifies the output direction for the clock source.
  *          This parameter can be one of the following values:
  *            @arg RCC_MCO: Clock source to output on MCO1 pin(PA8).
  * @param  RCC_MCOSource: specifies the clock source to output.
  *          This parameter can be one of the following values:
  *            @arg RCC_MCOSOURCE_HSI: HSI selected as MCO clock
  *            @arg RCC_MCOSOURCE_HSE: HSE selected as MCO clock
  *            @arg RCC_MCOSOURCE_LSI: LSI selected as MCO clock
  *            @arg RCC_MCOSOURCE_LSE: LSE selected as MCO clock
  *            @arg RCC_MCOSOURCE_PLLCLK_NODIV: PLLCLK selected as MCO clock (not applicable to STM32F05x devices)
  *            @arg RCC_MCOSOURCE_PLLCLK_DIV2: PLLCLK Divided by 2 selected as MCO clock
  *            @arg RCC_MCOSOURCE_SYSCLK: System Clock selected as MCO clock
  *            @arg RCC_MCOSOURCE_HSI14: HSI14 selected as MCO clock
  *            @arg RCC_MCOSOURCE_HSI48: HSI48 selected as MCO clock
  * @param  RCC_MCODiv: specifies the MCO DIV.
  *          This parameter can be one of the following values:
  *            @arg RCC_MCODIV_1: no division applied to MCO clock
  * @retval None
  */
void HAL_RCC_MCOConfig(uint32_t RCC_MCOx, uint32_t RCC_MCOSource, uint32_t RCC_MCODiv)
{
  GPIO_InitTypeDef gpio;
  
  /* Check the parameters */
  assert_param(IS_RCC_MCO(RCC_MCOx));
  assert_param(IS_RCC_MCODIV(RCC_MCODiv));
  assert_param(IS_RCC_MCO1SOURCE(RCC_MCOSource));
  
  /* MCO Clock Enable */
  MCO1_CLK_ENABLE();
  
  /* Configure the MCO1 pin in alternate function mode */    
  gpio.Pin = MCO1_PIN;
  gpio.Mode = GPIO_MODE_AF_PP;
  gpio.Speed = GPIO_SPEED_HIGH;
  gpio.Pull = GPIO_NOPULL;
  gpio.Alternate = GPIO_AF0_MCO;
  HAL_GPIO_Init(MCO1_GPIO_PORT, &gpio);
  
  /* Configure the MCO clock source */
  __HAL_RCC_MCO_CONFIG(RCC_MCOSource, RCC_MCODiv);
}

/**
  * @brief  Enables the Clock Security System.
  * @note   If a failure is detected on the HSE oscillator clock, this oscillator
  *         is automatically disabled and an interrupt is generated to inform the
  *         software about the failure (Clock Security System Interrupt, CSSI),
  *         allowing the MCU to perform rescue operations. The CSSI is linked to 
  *         the Cortex-M0 NMI (Non-Maskable Interrupt) exception vector.  
  * @retval None
  */
void HAL_RCC_EnableCSS(void)
{
  SET_BIT(RCC->CR, RCC_CR_CSSON) ;
}

/**
  * @brief  Disables the Clock Security System.
  * @retval None
  */
void HAL_RCC_DisableCSS(void)
{
  CLEAR_BIT(RCC->CR, RCC_CR_CSSON) ;
}

/**
  * @brief  Returns the SYSCLK frequency     
  *        
  * @note   The system frequency computed by this function is not the real 
  *         frequency in the chip. It is calculated based on the predefined 
  *         constant and the selected clock source:
  * @note     If SYSCLK source is HSI, function returns values based on HSI_VALUE(*)
  * @note     If SYSCLK source is HSE, function returns a value based on HSE_VALUE
  *           divided by PREDIV factor(**)
  * @note     If SYSCLK source is PLL, function returns a value based on HSE_VALUE
  *           divided by PREDIV factor(**) or depending on STM32F0xx devices either a value based 
  *           on HSI_VALUE divided by 2 or HSI_VALUE divided by PREDIV factor(*) multiplied by the 
  *           PLL factor .
  * @note     (*) HSI_VALUE is a constant defined in stm32f0xx_hal_conf.h file (default value
  *               8 MHz) but the real value may vary depending on the variations
  *               in voltage and temperature.
  * @note     (**) HSE_VALUE is a constant defined in stm32f0xx_hal_conf.h file (default value
  *                8 MHz), user has to ensure that HSE_VALUE is same as the real
  *                frequency of the crystal used. Otherwise, this function may
  *                have wrong result.
  *                  
  * @note   The result of this function could be not correct when using fractional
  *         value for HSE crystal.
  *           
  * @note   This function can be used by the user application to compute the 
  *         baudrate for the communication peripherals or configure other parameters.
  *           
  * @note   Each time SYSCLK changes, this function must be called to update the
  *         right SYSCLK value. Otherwise, any configuration based on this function will be incorrect.
  *         
  *               
  * @retval SYSCLK frequency
  */
uint32_t HAL_RCC_GetSysClockFreq(void)
{
  const uint8_t aPLLMULFactorTable[16] = { 2,  3,  4,  5,  6,  7,  8,  9,
                                         10, 11, 12, 13, 14, 15, 16, 16};
  const uint8_t aPredivFactorTable[16] = { 1, 2,  3,  4,  5,  6,  7,  8,
                                           9,10, 11, 12, 13, 14, 15, 16};

  uint32_t tmpreg = 0, prediv = 0, pllclk = 0, pllmul = 0;
  uint32_t sysclockfreq = 0;
  
  tmpreg = RCC->CFGR;
  
  /* Get SYSCLK source -------------------------------------------------------*/
  switch (tmpreg & RCC_CFGR_SWS)
  {
    case RCC_SYSCLKSOURCE_STATUS_HSE:  /* HSE used as system clock */
    {
      sysclockfreq = HSE_VALUE;
      break;
    }
    case RCC_SYSCLKSOURCE_STATUS_PLLCLK:  /* PLL used as system clock */
    {
      pllmul = aPLLMULFactorTable[(uint32_t)(tmpreg & RCC_CFGR_PLLMUL) >> RCC_CFGR_PLLMUL_BITNUMBER];
      prediv = aPredivFactorTable[(uint32_t)(RCC->CFGR2 & RCC_CFGR2_PREDIV) >> RCC_CFGR2_PREDIV_BITNUMBER];
      if ((tmpreg & RCC_CFGR_PLLSRC) == RCC_PLLSOURCE_HSE)
      {
        /* HSE used as PLL clock source : PLLCLK = HSE/PREDIV * PLLMUL */
        pllclk = (HSE_VALUE/prediv) * pllmul;
      }
#if defined(RCC_CR2_HSI48ON)
      else if ((tmpreg & RCC_CFGR_PLLSRC) == RCC_PLLSOURCE_HSI48)
      {
        /* HSI48 used as PLL clock source : PLLCLK = HSI48/PREDIV * PLLMUL */
        pllclk = (HSI48_VALUE/prediv) * pllmul;
      }
#endif /* RCC_CR2_HSI48ON */
      else
      {
#if  (defined(STM32F042x6) || defined(STM32F048xx) || defined(STM32F070x6) || defined(STM32F071xB) || defined(STM32F072xB) || defined(STM32F078xx) || defined(STM32F070xB) || defined(STM32F091xC) || defined(STM32F098xx) || defined(STM32F030xC))
        /* HSI used as PLL clock source : PLLCLK = HSI/PREDIV * PLLMUL */
        pllclk = (HSI_VALUE/prediv) * pllmul;
#else
        /* HSI used as PLL clock source : PLLCLK = HSI/2 * PLLMUL */
        pllclk = (uint32_t)((HSI_VALUE >> 1) * pllmul);
#endif
      }
      sysclockfreq = pllclk;
      break;
    }
#if defined(RCC_CR2_HSI48ON)
    case RCC_SYSCLKSOURCE_STATUS_HSI48:    /* HSI48 used as system clock source */
    {
      sysclockfreq = HSI48_VALUE;
      break;
    }
#endif /* RCC_CR2_HSI48ON */
    case RCC_SYSCLKSOURCE_STATUS_HSI:  /* HSI used as system clock source */
    default: /* HSI used as system clock */
    {
      sysclockfreq = HSI_VALUE;
      break;
    }
  }
  return sysclockfreq;
}

/**
  * @brief  Returns the HCLK frequency     
  * @note   Each time HCLK changes, this function must be called to update the
  *         right HCLK value. Otherwise, any configuration based on this function will be incorrect.
  * 
  * @note   The SystemCoreClock CMSIS variable is used to store System Clock Frequency 
  *         and updated within this function
  * @retval HCLK frequency
  */
uint32_t HAL_RCC_GetHCLKFreq(void)
{
  SystemCoreClock = HAL_RCC_GetSysClockFreq() >> aAPBAHBPrescTable[(RCC->CFGR & RCC_CFGR_HPRE)>> RCC_CFGR_HPRE_BITNUMBER];
  return SystemCoreClock;
}

/**
  * @brief  Returns the PCLK1 frequency     
  * @note   Each time PCLK1 changes, this function must be called to update the
  *         right PCLK1 value. Otherwise, any configuration based on this function will be incorrect.
  * @retval PCLK1 frequency
  */
uint32_t HAL_RCC_GetPCLK1Freq(void)
{
  /* Get HCLK source and Compute PCLK1 frequency ---------------------------*/
  return (HAL_RCC_GetHCLKFreq() >> aAPBAHBPrescTable[(RCC->CFGR & RCC_CFGR_PPRE)>> RCC_CFGR_PPRE_BITNUMBER]);
}    

/**
  * @brief  Configures the RCC_OscInitStruct according to the internal 
  * RCC configuration registers.
  * @param  RCC_OscInitStruct: pointer to an RCC_OscInitTypeDef structure that 
  * will be configured.
  * @retval None
  */
void HAL_RCC_GetOscConfig(RCC_OscInitTypeDef  *RCC_OscInitStruct)
{
  /* Check the parameters */
  assert_param(RCC_OscInitStruct != NULL);

  /* Set all possible values for the Oscillator type parameter ---------------*/
  RCC_OscInitStruct->OscillatorType = RCC_OSCILLATORTYPE_HSE | RCC_OSCILLATORTYPE_HSI  \
                  | RCC_OSCILLATORTYPE_LSE | RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSI14;
#if defined(RCC_CR2_HSI48ON)
  RCC_OscInitStruct->OscillatorType |= RCC_OSCILLATORTYPE_HSI48;
#endif /* RCC_CR2_HSI48ON */
  
  /* Get the HSE configuration -----------------------------------------------*/
  if((RCC->CR &RCC_CR_HSEBYP) == RCC_CR_HSEBYP)
  {
    RCC_OscInitStruct->HSEState = RCC_HSE_BYPASS;
  }
  else if((RCC->CR &RCC_CR_HSEON) == RCC_CR_HSEON)
  {
    RCC_OscInitStruct->HSEState = RCC_HSE_ON;
  }
  else
  {
    RCC_OscInitStruct->HSEState = RCC_HSE_OFF;
  }
  
  /* Get the HSI configuration -----------------------------------------------*/
  if((RCC->CR &RCC_CR_HSION) == RCC_CR_HSION)
  {
    RCC_OscInitStruct->HSIState = RCC_HSI_ON;
  }
  else
  {
    RCC_OscInitStruct->HSIState = RCC_HSI_OFF;
  }
  
  RCC_OscInitStruct->HSICalibrationValue = (uint32_t)((RCC->CR &RCC_CR_HSITRIM) >> RCC_CR_HSITRIM_BitNumber);
  
  /* Get the LSE configuration -----------------------------------------------*/
  if((RCC->BDCR &RCC_BDCR_LSEBYP) == RCC_BDCR_LSEBYP)
  {
    RCC_OscInitStruct->LSEState = RCC_LSE_BYPASS;
  }
  else if((RCC->BDCR &RCC_BDCR_LSEON) == RCC_BDCR_LSEON)
  {
    RCC_OscInitStruct->LSEState = RCC_LSE_ON;
  }
  else
  {
    RCC_OscInitStruct->LSEState = RCC_LSE_OFF;
  }
  
  /* Get the LSI configuration -----------------------------------------------*/
  if((RCC->CSR &RCC_CSR_LSION) == RCC_CSR_LSION)
  {
    RCC_OscInitStruct->LSIState = RCC_LSI_ON;
  }
  else
  {
    RCC_OscInitStruct->LSIState = RCC_LSI_OFF;
  }
  
  /* Get the PLL configuration -----------------------------------------------*/
  if((RCC->CR &RCC_CR_PLLON) == RCC_CR_PLLON)
  {
    RCC_OscInitStruct->PLL.PLLState = RCC_PLL_ON;
  }
  else
  {
    RCC_OscInitStruct->PLL.PLLState = RCC_PLL_OFF;
  }
  RCC_OscInitStruct->PLL.PLLSource = (uint32_t)(RCC->CFGR & RCC_CFGR_PLLSRC);
  RCC_OscInitStruct->PLL.PLLMUL = (uint32_t)(RCC->CFGR & RCC_CFGR_PLLMUL);
  RCC_OscInitStruct->PLL.PREDIV = (uint32_t)(RCC->CFGR2 & RCC_CFGR2_PREDIV);
  
  /* Get the HSI14 configuration -----------------------------------------------*/
  if((RCC->CR2 & RCC_CR2_HSI14ON) == RCC_CR2_HSI14ON)
  {
    RCC_OscInitStruct->HSI14State = RCC_HSI_ON;
  }
  else
  {
    RCC_OscInitStruct->HSI14State = RCC_HSI_OFF;
  }

  RCC_OscInitStruct->HSI14CalibrationValue = (uint32_t)((RCC->CR2 & RCC_CR2_HSI14TRIM) >> RCC_CR2_HSI14TRIM_BitNumber);
  
#if defined(RCC_CR2_HSI48ON)
  /* Get the HSI48 configuration if any-----------------------------------------*/
  RCC_OscInitStruct->HSI48State = __HAL_RCC_GET_HSI48_STATE();
#endif /* RCC_CR2_HSI48ON */
}

/**
  * @brief  Get the RCC_ClkInitStruct according to the internal 
  * RCC configuration registers.
  * @param  RCC_ClkInitStruct: pointer to an RCC_ClkInitTypeDef structure that 
  * contains the current clock configuration.
  * @param  pFLatency: Pointer on the Flash Latency.
  * @retval None
  */
void HAL_RCC_GetClockConfig(RCC_ClkInitTypeDef  *RCC_ClkInitStruct, uint32_t *pFLatency)
{
  /* Check the parameters */
  assert_param(RCC_ClkInitStruct != NULL);
  assert_param(pFLatency != NULL);

  /* Set all possible values for the Clock type parameter --------------------*/
  RCC_ClkInitStruct->ClockType = RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1;
  
  /* Get the SYSCLK configuration --------------------------------------------*/ 
  RCC_ClkInitStruct->SYSCLKSource = (uint32_t)(RCC->CFGR & RCC_CFGR_SW);
  
  /* Get the HCLK configuration ----------------------------------------------*/ 
  RCC_ClkInitStruct->AHBCLKDivider = (uint32_t)(RCC->CFGR & RCC_CFGR_HPRE); 
  
  /* Get the APB1 configuration ----------------------------------------------*/ 
  RCC_ClkInitStruct->APB1CLKDivider = (uint32_t)(RCC->CFGR & RCC_CFGR_PPRE);   
  /* Get the Flash Wait State (Latency) configuration ------------------------*/   
  *pFLatency = (uint32_t)(FLASH->ACR & FLASH_ACR_LATENCY); 
}

/**
  * @brief This function handles the RCC CSS interrupt request.
  * @note This API should be called under the NMI_Handler().
  * @retval None
  */
void HAL_RCC_NMI_IRQHandler(void)
{
  /* Check RCC CSSF flag  */
  if(__HAL_RCC_GET_IT(RCC_IT_CSS))
  {
    /* RCC Clock Security System interrupt user callback */
    HAL_RCC_CSSCallback();
    
    /* Clear RCC CSS pending bit */
    __HAL_RCC_CLEAR_IT(RCC_IT_CSS);
  }
}

/**
  * @brief  RCC Clock Security System interrupt callback
  * @retval none
  */
__weak void HAL_RCC_CSSCallback(void)
{
  /* NOTE : This function Should not be modified, when the callback is needed,
    the HAL_RCC_CSSCallback could be implemented in the user file
    */ 
}

/**
  * @}
  */

/**
  * @}
  */

#endif /* HAL_RCC_MODULE_ENABLED */
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
