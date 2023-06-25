/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AI_UTILITIES_H
#define __AI_UTILITIES_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "fp_vision_app.h"
#include "ai_interface.h"

void Run_Preprocessing(AppContext_TypeDef *);
void Init_DataMemoryLayout(AppContext_TypeDef *);
bool Image_CheckResizeMemoryLayout(image_t *src_img, image_t *dst_img);
bool Image_CheckPfcMemoryLayout(image_t *src_img, image_t *dst_img);

#ifdef __cplusplus
}
#endif

#endif /*__AI_UTILITIES_H */
