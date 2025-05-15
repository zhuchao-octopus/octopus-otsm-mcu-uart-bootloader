/*******************************************************************************
 * @file     octopus_ipc_socket.c
 * @brief    Implements the system control logic for managing states,
 *           message handling, and UART communication in an embedded application.
 *
 * This source file is responsible for initializing and controlling the system's
 * state machine. It handles incoming and outgoing messages through a UART
 * interface, processes system-level events, and manages the power state
 * and handshake procedures with both the MCU and external applications.
 *
 * The code uses a modular design to interface with other system components,
 * ensuring flexibility and scalability.
 *
 * @version  1.0.0
 * @date     2024-12-11
 * @author   Octopus Team
 ******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "octopus_platform.h"     // Include platform-specific header for hardware platform details
#include "octopus_log.h"          // Include logging functions for debugging
#include "octopus_task_manager.h" // Include task manager for scheduling tasks
#include "octopus_gpio.h"
#include "octopus_system.h"
#include "octopus_tickcounter.h"
#include "octopus_msgqueue.h"
#include "octopus_ipc.h"
#include "octopus_message.h"
#include "octopus_uart_hal.h"
#include "octopus_flash.h"
/*******************************************************************************
 * Debug Switch Macros
 * Define debug levels or other switches as required.
 ******************************************************************************/

/*******************************************************************************
 * MACROS
 * The following macros define key IDs and their respective actions.
 */
#define SYSTEM_POWER_ON_VALUE 1
#define SYSTEM_POWER_OFF_VALUE 0
/*******************************************************************************
 * Local Function Declarations
 * Declare static functions used only within this file.
 ******************************************************************************/
static bool ipc_socket_send_handler(ptl_frame_type_t frame_type, uint16_t param1, uint16_t param2, ptl_proc_buff_t *buff);
static bool ipc_socket_receive_handler(ptl_frame_payload_t *payload, ptl_proc_buff_t *ackbuffer);

/*******************************************************************************
 * Global Variables
 * Define variables accessible across multiple files if needed.
 ******************************************************************************/

/*******************************************************************************
 * Local Variables
 * Define static variables used only within this file.
 ******************************************************************************/

//static uint8_t l_u8_idle_swich = 0;
static uint32_t l_t_msg_wait_10_timer; // Timer for 10 ms message waiting period
static uint32_t l_t_msg_wait_500_timer;
//static uint16_t l_t_callback_delay = 0;//1000;
/*******************************************************************************
 * Global Function Implementations
 ******************************************************************************/

/**
 * @brief Initializes the system for running.
 *
 * This function registers the system module with the communication layer
 * and transitions the system task to an invalid state.
 */
void app_ipc_socket_init_running(void)
{
    LOG_LEVEL("app_ipc_socket_init_running\r\n");
    OTMS(TASK_ID_IPC_SOCKET, OTMS_S_INVALID);
    ptl_register_module(MCU_TO_SOC_MOD_IPC, ipc_socket_send_handler, ipc_socket_receive_handler);
}

/**
 * @brief Starts the system.
 *
 * This function transitions the system task to the "assert run" state.
 */
void app_ipc_socket_start_running(void)
{
    LOG_LEVEL("app_ipc_socket_start_running\r\n");
    OTMS(TASK_ID_IPC_SOCKET, OTMS_S_ASSERT_RUN);
}

/**
 * @brief Asserts the system is running.
 *
 * This function starts timers, requests the system to start running,
 * and transitions the system task to the running state.
 */
void app_ipc_socket_assert_running(void)
{
    StartTickCounter(&l_t_msg_wait_10_timer);
    StartTickCounter(&l_t_msg_wait_500_timer);

    #ifdef TASK_MANAGER_STATE_MACHINE_MCU
    ptl_reqest_running(MCU_TO_SOC_MOD_IPC);
    #elif defined(TASK_MANAGER_STATE_MACHINE_SOC)
    ptl_reqest_running(SOC_TO_MCU_MOD_IPC);
    #endif
    OTMS(TASK_ID_IPC_SOCKET, OTMS_S_RUNNING);
}

/**
 * @brief Main system running state handler.
 *
 * Processes system messages and handles events like power on/off and device events.
 */
void app_ipc_socket_running(void)
{
    if (GetTickCounter(&l_t_msg_wait_10_timer) < 10)
        return;
    StartTickCounter(&l_t_msg_wait_10_timer);

    Msg_t *msg = get_message(TASK_ID_IPC_SOCKET);
   
    if (msg->id == NO_MSG)
    {
        return;
    }
   
    switch (msg->id)
    {
        case MSG_DEVICE_CAR_INFOR_EVENT:
        case MSG_DEVICE_CAN_EVENT:
            break;
        case MSG_CAR_SETTING_SAVE:
        case MSG_CAR_SET_LIGHT:
            break;
				case MSG_CAR_SET_GEAR_LEVEL:
            break;
    }
    StartTickCounter(&l_t_msg_wait_500_timer);
}

void app_ipc_socket_post_running(void)
{
}

void app_ipc_socket_stop_running(void)
{
    OTMS(TASK_ID_IPC_SOCKET, OTMS_S_INVALID);
}

void update_push_interval_ms(uint16_t delay_ms)
{
    //l_t_callback_delay = delay_ms;
}
/*******************************************************************************
 * FUNCTION: ipc_socket_send_handler
 *
 * DESCRIPTION:
 * Handles the sending of system-related commands based on the frame type and parameters.
 * This function processes commands for system, setup, and app modules and sends appropriate responses.
 *
 * PARAMETERS:
 * - frame_type: Type of the frame (MCU_TO_SOC_MOD_SYSTEM, SOC_TO_MCU_MOD_SYSTEM, MCU_TO_SOC_MOD_SETUP).
 * - param1: Command identifier.
 * - param2: Additional parameter for the command.
 * - buff: Pointer to the buffer where the frame should be built.
 *
 * RETURNS:
 * - true if the command was processed successfully, false otherwise.
 ******************************************************************************/
bool ipc_socket_send_handler(ptl_frame_type_t frame_type, uint16_t param1, uint16_t param2, ptl_proc_buff_t *buff)
{
    //assert(buff);         // Ensure the buffer is valid
    //uint8_t tmp[2] = {0};   // Temporary buffer for command parameters

    // Handle commands for SOC_TO_MCU_MOD_SYSTEM frame type
    LOG_LEVEL("frame_type= %d param1=%d,param2=%d\r\n",frame_type,param1,param2);
    if (SOC_TO_MCU_MOD_IPC == frame_type)
    {
        switch (param1)
        {
        case CMD_MODSYSTEM_SAVE_DATA:
        case CMD_MODCAR_SET_LIGHT:
        default:
            break;
        }
    }

    if (MCU_TO_SOC_MOD_IPC == frame_type)
    {
        switch (param1)
        {
        default:
            break;
        }
    }
    return false; // Command not processed
}

/*******************************************************************************
 * FUNCTION: ipc_socket_receive_handler
 *
 * DESCRIPTION:
 * Handles the reception of system-related commands based on the payload and sends appropriate responses.
 *
 * PARAMETERS:
 * - payload: Pointer to the received payload data.
 * - ackbuff: Pointer to the buffer where the acknowledgment frame will be built.
 *
 * RETURNS:
 * - true if the command was processed successfully, false otherwise.
 ******************************************************************************/
//���ָʾ��
//static const uint8_t protocol_cmd_lamp_on[3] = { 0x16, 0x1A, 0xF1 };   //����
//static const uint8_t protocol_cmd_lamp_off[3] = { 0x16, 0x1A, 0xF0 };  //�ص�
extern void bafang_lamp_on_off(bool on_off);
extern void bafang_set_gear(uint8_t level);
bool ipc_socket_receive_handler(ptl_frame_payload_t *payload, ptl_proc_buff_t *ackbuffer)
{
    //assert(payload); 		// Ensure payload is valid
    //assert(ackbuffer); 	// Ensure acknowledgment buffer is valid
    //uint8_t tmp[1];     // Temporary variable for holding command data
	  LOG_LEVEL("payload.frame_type=%02x cmd=%02x,length=%d\r\n",payload->frame_type,payload->cmd,payload->data_len);
    if (SOC_TO_MCU_MOD_IPC == payload->frame_type)
    {
        switch (payload->cmd)
        {
        case CMD_MODSYSTEM_SAVE_DATA:
					  lt_carinfo_meter.unit_type = payload->data[0];
				    #ifdef USE_EEROM_FOR_DATA_SAVING
					  carinfor_save_to_flash();
				    #endif
						return true;
        case CMD_MODCAR_SET_LIGHT:
				    if(payload->data[0] ==1)
							bafang_lamp_on_off(true);
						else
							bafang_lamp_on_off(false);
				  
            return true;
				case CMD_MODCAR_SET_GEAR:				
            bafang_set_gear(payload->data[0]);	
				    return true;
        default:
            break;
        }
    }
    // Handle received commands for MCU_TO_SOC_MOD_SYSTEM frame type
    return false; // Command not processed
}

//__attribute__((visibility("default")))
void otsm_do_ipc_Command(uint8_t *data, uint8_t length)
{
    LOG_LEVEL("ipc_doCommand called with length: %d\n", length);
    LOG_BUFF_LEVEL(data,length);
}


