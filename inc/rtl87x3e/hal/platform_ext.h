#ifndef _PLATFORM_EXT_H_
#define _PLATFORM_EXT_H_

/** @defgroup  HAL_87x3g_platform_ext    platform_ext
    * @brief hal debug api
    * @{
    */

/** @defgroup HAL_87x3g_platform_ext 2_Exported_Functions platform_ext  Exported Functions
    * @brief
    * @{
    */
#include "vector_table.h"
typedef enum
{

    WATCH_POINT_INDEX0 = 0, // Configure Watchpoint 0
    WATCH_POINT_INDEX1 = 1, // Configure Watchpoint 1
    WATCH_POINT_NUMBER = 2
} T_WATCH_POINT_INDEX;

typedef enum
{
    WATCH_POINT_BYTE  = 0, //Detect 1 byte access
    WATCH_POINT_HALFWORD,  //Detect 2 byte access
    WATCH_POINT_WORD,      //Detect 4 byte access
    WATCH_POINT_SIZE_MAX
} T_WATCH_POINT_ACCESS_SIZE;


typedef enum
{
    WATCH_POINT_FUNCTION_DISABLED = 0,    // Disabled. Never generates a match.
    WATCH_POINT_FUNCTION_INSTR_ADDR = 2,
    WATCH_POINT_FUNCTION_INSTR_ADDR_LIM,
    WATCH_POINT_FUNCTION_DADDR_RW,        // Detect Read or Write access
    WATCH_POINT_FUNCTION_DADDR_W,         // Detect Write access
    WATCH_POINT_FUNCTION_DADDR_R,         // Detect Read access
    WATCH_POINT_FUNCTION_DADDR_LIM,
    WATCH_POINT_FUNCTION_DVAL_RW,
    WATCH_POINT_FUNCTION_DVAL_W,
    WATCH_POINT_FUNCTION_DVAL_R,
    WATCH_POINT_FUNCTION_DVAL_LINK,
    WATCH_POINT_FUNCTION_MAX
} T_WATCH_POINT_WATCH_TYPE;

typedef enum
{
    WATCH_POINT_WATCH_SIZE_1BYTE  = 0,
    WATCH_POINT_WATCH_SIZE_2BYTE,
    WATCH_POINT_WATCH_SIZE_4BYTE,
    WATCH_POINT_WATCH_SIZE_8BYTE,
    WATCH_POINT_WATCH_SIZE_16BYTE,
    WATCH_POINT_WATCH_SIZE_32BYTE,
    WATCH_POINT_WATCH_SIZE_64BYTE,
    WATCH_POINT_WATCH_SIZE_128BYTE,
    WATCH_POINT_WATCH_SIZE_256BYTE,
    WATCH_POINT_WATCH_SIZE_512BYTE,
    WATCH_POINT_WATCH_SIZE_1024BYTE,
    WATCH_POINT_WATCH_SIZE_2048BYTE,
    WATCH_POINT_WATCH_SIZE_4096BYTE,
    WATCH_POINT_WATCH_SIZE_8192BYTE,
    WATCH_POINT_WATCH_SIZE_16384BYTE,
    WATCH_POINT_WATCH_SIZE_32768BYTE,
} T_WATCH_POINT_WATCH_SIZE;

/**
 * @brief     Update APP defined handlers
 * @note      none
 * @param     pAppVector: the vector table address of app
 * @param:    Default_Handler: APP defined hanlders
 * @return    none
 */
void vector_table_update(IRQ_Fun *pAppVector, void *Default_Handler);

/**
    * Debug monitor sample code:
      (1) To detect single address
      // detect start
            debug_monitor_enable();
      debug_monitor_point_set(WATCH_POINT_INDEX0, 0x200000, WATCH_POINT_BYTE, WATCH_POINT_FUNCTION_DADDR_W);
      // to stop debug monitor if desired
      debug_monitor_disable();

      (2) To detect a range of address
       // memory monitor start
            memory_monitor_start(0x200022,WATCH_POINT_WATCH_SIZE_256BYTE,WATCH_POINT_FUNCTION_DADDR_RW);
             // to memory monitor if desired
            memory_monitor_stop();
    *
    */

/**
 * @brief     enable debug monitor
 * @note      debug_monitor_enable must be called before debug_monitor_point_set
 * @param     none
 * @return    none
 */
void debug_monitor_enable(void);

/**
 * @brief     disable debug monitor
 * @note      none
 * @param     none
 * @return    none
 */
void debug_monitor_disable(void);

/**
 * @brief     check if debug monitor is enbale
 * @note      none
 * @param     none
 * @return    true: debug monitor has been enabled
 * @return    false: debug monitor is not enabled
 */
bool debug_monitor_is_enable(void);

/**
 * @brief     check if debug monitor is enbale
 * @note      debug_monitor_enable must be called before debug_monitor_point_set
 * @param     index: the index of WatchPoint x
 * @param     watch_address: the address of variable which is being watched
 * @param     access_size: the size of variable which is being watched.
 * @param     read_write_func: the detecting type of Watchpoint
 * @return    none
 */
void debug_monitor_point_set(T_WATCH_POINT_INDEX index, uint32_t watch_address,
                             T_WATCH_POINT_ACCESS_SIZE access_size, T_WATCH_POINT_WATCH_TYPE read_write_func);

/**
 * @brief     save debug monitor setting before enter dlpst
 * @param     none
 * @return    none
 */
void debug_monitor_save_site(void);

/**
 * @brief     restore debug monitor setting after exist dlpst
 * @param     none
 * @return    none
 */
void debug_monitor_restore_site(void);

/**
 * @brief   memory_monitor_start can specifies start address and end address with restriction
 *          for example:  watch_address = 0x123456 watch_size = WATCH_POINT_WATCH_SIZE_256BYTE
 *                                              address range of watch: 0x123400 ~ 0x1234ff  NOT 0x123456 ~ 0x123456 + 0xff
 * @param[in] watch_address: the start address of variable which is being watched
 * @param[in] watch_size: the size of address range which is being watched.
 * @param[in] read_write_func: the detecting type of Watchpoint.
 *              @arg  WATCH_POINT_WATCH_TYPE_RW: An access matches if the value accessed matches the comparator value
 *              @arg  WATCH_POINT_WATCH_TYPE_W: Data Value, writes. As FUNCTION_DVAL_RW, except that only write accesses generate a match.
 *              @arg  WATCH_POINT_WATCH_TYPE_R  Data Value, reads. As FUNCTION_DVAL_RW, except that only read accesses generate a match.
 * @return    none
 */
int memory_monitor_start(uint32_t watch_address, T_WATCH_POINT_WATCH_SIZE watch_size,
                         T_WATCH_POINT_WATCH_TYPE read_write_func);

/**
 * @brief     stop memory monitor if the range of address set by memory_monitor_start do not need to be detected
 * @param     none
 * @return    none
 */
void memory_monitor_stop(void);


/**
 * platform_ext.h
 *
 * \brief    get temperature in degrees Celsius.
 *
 * \xrefitem Experimental_Added_API_2_13_0_0 "Experimental Added Since 2.13.0.0" "Experimental Added API"
 * @return   temperature
 *
 * \ingroup  PLATFORM_EXT
 */
int16_t get_temperature(void);

/**
    * @brief  control LDO AUX2 power on or off
    * \xrefitem Added_API_2_13_0_0 "Added Since 2.13.0.0" "Added API"
    * @param  true:power on; false:power off
    * @return NULL
    */
void ldo_aux2_power_control(bool enable);

/** @} */ /* End of group HAL_87x3e_platform_ext_Exported_Functions */
/** @} */ /* End of group HAL_87x3e_platform_ext */

#endif
