#include "Delay.h"

/*********************************************************************************************************//**
* @brief  Used to delay x ms.
* @retval None
***********************************************************************************************************/
void DelayMS(u32 ms)
{
	GPTM_SetCounter(GPTM0, 0);

	while(ms--)
	{
		 GPTM_ClearFlag(GPTM0, GPTM_FLAG_UEV);
		 while(!GPTM_GetFlagStatus(GPTM0, GPTM_FLAG_UEV));
	}
}
