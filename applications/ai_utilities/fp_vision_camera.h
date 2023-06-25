#ifndef __FP_VISION_CAMERA_H
#define __FP_VISION_CAMERA_H

#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include "GC0308.h"

/* Camera resolutions */
#define CAMERA_R160x120                 0U     /* QQVGA Resolution            */
#define CAMERA_R320x240                 1U     /* QVGA Resolution             */
#define CAMERA_R480x272                 2U     /* 480x272 Resolution          */
#define CAMERA_R640x480                 3U     /* VGA Resolution              */
#define CAMERA_R800x480                 4U     /* WVGA Resolution             */

#define VGA_640_480_RES 1
#define QVGA_320_240_RES 2

#ifndef CAMERA_CAPTURE_RES                      //相机捕捉分辨率
 #define CAMERA_CAPTURE_RES QVGA_320_240_RES    //使用了但是不知道在那里添加的,目前只能加在这里

 #if CAMERA_CAPTURE_RES == VGA_640_480_RES && FRAME_SIZE == FRAMESIZE_640x480_VGA
  #define CAMERA_RESOLUTION CAMERA_R640x480 //相机分辨率
  #define CAM_RES_WIDTH INPUT_WIDTH         //相机分辨率宽
  #define CAM_RES_HEIGHT INPUT_HEIGHT       //相机分辨率高
 #elif CAMERA_CAPTURE_RES == QVGA_320_240_RES && FRAME_SIZE == FRAMESIZE_320x240_QVGA
  #define CAMERA_RESOLUTION CAMERA_R320x240
  #define CAM_RES_WIDTH INPUT_WIDTH
  #define CAM_RES_HEIGHT INPUT_HEIGHT
 #else
  #error Please check definition of CAMERA_CAPTURE_RES define
 #endif
#endif

#endif /* __FP_VISION_CAMERA_H */

