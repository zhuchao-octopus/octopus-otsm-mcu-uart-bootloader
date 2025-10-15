/*******************************************************************************
 * @copyright: Shenzhen Hangshun Chip Technology R&D Co., Ltd
 * @filename:  main.c
 * @brief:     Main program body
 * @author:    AE Team
 * @version:   V1.0.0/2024-01-04
 *             1.Initial version
 * @log:
 *******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "../OTSM/octopus_bsp_hk32l08x.h"
#include "../OTSM/octopus.h"
#include "../OTSM/octopus_flash.h"
#include "../OTSM/octopus_vehicle.h"
#include "../OTSM/octopus_tickcounter.h"
#include "../OTSM/octopus_update_mcu.h"
#include "../OTSM/octopus_sif.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Private function prototypes -----------------------------------------------*/

//#define WATCH_DOG_ENABLE

static uint32_t l_t_msg_wait_60000_timer = 0; // Timer for 1 minu message wait
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Initialize SysTick to generate interrupt every 1ms
#if 1
#else
static void SysTick_Config2(void)
{
	// Configure SysTick to trigger interrupt every 1ms
	// This sets up the SysTick timer to generate an interrupt every 1 millisecond
	// SystemCoreClock is the system clock frequency, dividing it by 1000 gives the value for 1ms interval
	// SysTick_Config automatically sets the LOAD, VAL, and CTRL registers and enables the SysTick interrupt
	// If configuration fails, SysTick_Config returns a non-zero value
	if (SysTick_Config(SystemCoreClock / 1000))
	{
		// Error handler if configuration fails
		// If SysTick configuration fails, the program will enter this infinite loop
		// You can replace this with specific error handling code or debugging actions
		while (1)
		{
		}
	}

	// Set SysTick interrupt priority
	// This function sets the priority of the SysTick interrupt
	// NVIC_SetPriority(SysTick_IRQn, 3) sets the priority for the SysTick interrupt
	// SysTick_IRQn is the interrupt number for SysTick timer
	// 3 indicates the priority level, with lower values meaning higher priority (e.g., 0 is the highest priority)
	// Here, we are setting the priority to 3, which indicates medium priority
	NVIC_SetPriority(SysTick_IRQn, 3);
}
#endif

void Delay_MS_(uint32_t ms)
{
	uint32_t i, count;
	count = (SystemCoreClock / 1000) * ms;
	for (i = 0; i < count; i++)
	{
		__NOP(); // __NOP()
	}
}
#if 0
 const uint8_t start_msg[] = {
    0x4D, 0x43, 0x55, 0x20,            
    0x48, 0x4B, 0x33, 0x32, 0x4C, 0x30, 0x78, 0x20, 
    0xE8, 0xAE, 0xBE,                
    0xE5, 0xA4, 0x87,                  
    0xE5, 0x88, 0x9D,                 
    0xE5, 0xA7, 0x8B,                  
    0xE5, 0xAE, 0x89,                  
    0xE8, 0xA3, 0x85,                
    0xE5, 0xAE, 0x8C,                  
    0xE6, 0x88, 0x90,                  
    0xEF, 0xBC, 0x8C,                
    0xE7, 0xB3, 0xBB,                
    0xE7, 0xBB, 0x9F,                  
    0xE5, 0x90, 0xAF,                
    0xE5, 0x8A, 0xA8,                
    0xE4, 0xB8, 0xAD,                  
    0x2E, 0x2E, 0x2E,                   
    0x0D, 0x0A  };
#endif

// const uint8_t test_help[] = {0xaa,0xf0,0x00,0x02,0x64,0x00,0x00,0x9c};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief  Main program.
 * @param  None
 * @retval None
 */
int main(void)
{
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	flash_vector_table_config(FLASH_BANK_CONFIG_MODE_BANK,FLASH_BANK_CONFIG_MODE_SLOT,FLASH_ROM_BASE_ADDRESS, FLASH_MAPPING_VECT_TABLE_TO_SRAM);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SYS_Config();
	RCC_Config();
	GPIO_Config();
	
#ifdef HARDWARE_BSP_ADC_CHANNEL_8	
	ADC_Channel_8_Config();
#endif	
	
#ifdef HARDWARE_BSP_TIMER_2	
	TIM2_Config();
#endif
	
#ifdef FLASH_USE_EEROM_FOR_DATA_SAVING
	EEPROM_Init();
#endif
	
#ifdef HARDWARE_BSP_UART_1
	UART1_Config_IRQ();
#endif	
#ifdef HARDWARE_BSP_UART_2
	UART2_Config_IRQ();
#endif
#ifdef HARDWARE_BSP_UART_3
	UART3_Config_IRQ();
#endif
#ifdef HARDWARE_BSP_UART_4
	UART4_Config_IRQ();
#endif

#ifdef HARDWARE_BSP_LUART_1
	LPUART_WakeStop_Config();
#endif

#ifdef TASK_MANAGER_STATE_MACHINE_CAN
	CAN_Config();
#endif
	
#ifdef WATCH_DOG_ENABLE
	IWDG_Init(IWDG_Prescaler_256, 2342); // 15 seconds timeout
#endif
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// UART2_Send_Buffer(test_help,sizeof(test_help));
	// UART3_Send_Buffer_DMA(test_help,sizeof(test_help));
	// LPUART_Send_Buffer(start_msg,sizeof(start_msg));
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	dbg_log_set_channel(TASK_MANAGER_STATE_MACHINE_LOG_CHANNEL);
	flash_loader_active_user_app(FLASH_BANK_CONFIG_MODE_SLOT,__DATE__, __TIME__);
		
	TaskManagerStateMachineInit();
	StartTickCounter(&l_t_msg_wait_60000_timer);
  
	#ifdef TASK_MANAGER_STATE_MACHINE_SIF
	//LOG_LEVEL("start test sif_delay_50_us %u\r\n",system_timer_tick_50us);
	sif_delay_50_us(1000*1000);
	//LOG_LEVEL("ended test sif_delay_50_us %u\r\n",system_timer_tick_50us);
	#else
	Delay_MS_(100);
	#endif
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	while (1)
	{
		uint32_t counter = GetTickCounter(&l_t_msg_wait_60000_timer);
		if (counter >= 60000 * 3)
		{
#ifdef TASK_MANAGER_STATE_MACHINE_CARINFOR
			LOG_LEVEL("tms voltage=%d,current=%d,trip_odo=%d,power=%d,soc=%d,range=%d,range_max=%d\r\n",
			lt_carinfo_battery.voltage, lt_carinfo_battery.current, lt_carinfo_meter.trip_odo,
			lt_carinfo_battery.power, lt_carinfo_battery.soc,
			lt_carinfo_battery.range, lt_carinfo_battery.range_max);
#else
			LOG_LEVEL("task manager state machine loop %d/%d\r\n", counter, l_t_msg_wait_60000_timer); // Log unhandled events
#endif
			RestartTickCounter(&l_t_msg_wait_60000_timer);
		}

		TaskManagerStateEventLoop((void(*))0);

#ifdef WATCH_DOG_ENABLE
		IWDG_Feed(); // Feed the dog periodically to avoid reset
#endif
	}
}
