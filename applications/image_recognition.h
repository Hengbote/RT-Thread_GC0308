#ifndef APPLICATIONS_IMAGE_RECOGNITION_H_
#define APPLICATIONS_IMAGE_RECOGNITION_H_

#include "stm32u5xx_hal.h"
#include "pin_config.h"
#include "main.h"

#ifndef USE_STM32IPL

//#define SUBIMAGE_WIDTH          48  //历遍子图像列数(子图像宽)
//#define SUBIMAGE_HEIGHT         48  //历遍子图像行数(子图像高)
//#define STANDARD_IMAGE_WIDTH    96  //标准图像宽
//#define STANDARD_IMAGE_HEIGHT   96  //标准图像高

//#define SUBIMAGE_WIDTH          96  //历遍子图像列数(子图像宽)
//#define SUBIMAGE_HEIGHT         96  //历遍子图像行数(子图像高)
//#define STANDARD_IMAGE_WIDTH    48  //标准图像宽
//#define STANDARD_IMAGE_HEIGHT   48  //标准图像高

#define SUBIMAGE_WIDTH          20  //历遍子图像列数(子图像宽)
#define SUBIMAGE_HEIGHT         32  //历遍子图像行数(子图像高)
#define STANDARD_IMAGE_WIDTH    20  //标准图像宽
#define STANDARD_IMAGE_HEIGHT   32  //标准图像高
#define STANDARD_IMAGE_COLOUR   3   //标准图颜色通道

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

#define SATURATE_CAST(value, data) ((data) < 0 ? 0 : ((data) > (value) ? (value) : (data)))



void Image_Recognition_Reponse(void);  //图像识别任务调用函数
void Split_Image_Into_Subimages(rt_uint8_t row, rt_uint8_t column);

#endif /*USE_STM32IPL 是否使用IPL库*/
#endif /* APPLICATIONS_IMAGE_RECOGNITION_H_ */
