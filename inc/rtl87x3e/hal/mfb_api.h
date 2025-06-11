#ifndef         _MFB_API_
#define         _MFB_API_

#include <stdbool.h>


typedef void (*P_MFB_LEVEL_CHANGE_CBACK)(void);

/**
    * @brief  mfb_get_level
    * @param  none
    * @return mfb level 1 high 0 low
    */
bool mfb_get_level(void);

/**
    * @brief  Enable MFB interrupt
    * @param  none
    * @return none
    */
void mfb_irq_enable(void);

/**
    * @brief  Disable MFB interrupt
    * @param  none
    * @return none
    */
void mfb_irq_disable(void);

/**
    * @brief  Innatial the  MFB for power on key
    * @param  cback ,call back function which called in  mfb interrupt handler
    * @return none
    */
void mfb_init(P_MFB_LEVEL_CHANGE_CBACK cback);

#endif
