#ifndef SAMC21E17A_HAL_H_
#define SAMC21E17A_HAL_H_

#include "../Modules/Modules.h"

#define	CLRWDT()	BSP_WWDG_Clear()
#define	SLEEP()		HAL_PWREx_EnterSHUTDOWNMode()

void hal_halt(void);
void hal_close(void);
void hal_flush(void);

#endif //SAMC21E17A_HAL_H_
