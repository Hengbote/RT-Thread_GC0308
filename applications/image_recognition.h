#ifndef APPLICATIONS_IMAGE_RECOGNITION_H_
#define APPLICATIONS_IMAGE_RECOGNITION_H_

#include "stm32u5xx_hal.h"
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

#define SUBIMAGE_WIDTH          9   //历遍子图像列数(子图像宽)
#define SUBIMAGE_HEIGHT         18  //历遍子图像行数(子图像高)
#define STANDARD_IMAGE_WIDTH    12  //标准图像宽
#define STANDARD_IMAGE_HEIGHT   20  //标准图像高

#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

#define SATURATE_CAST(value, data) ((data) < 0 ? 0 : ((data) > (value) ? (value) : (data)))


static const rt_uint8_t standard_one[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH] = {};
static const rt_uint8_t standard_two[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH] = {};
static const rt_uint8_t standard_three[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH] = {};
static const rt_uint8_t standard_four[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH] = {};
static const rt_uint8_t standard_five[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH] = {};
static const rt_uint8_t standard_six[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH] = {};
//static const rt_uint8_t standard_seven[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH] = {   //7
//                        {0x84, 0x7D, 0x80, 0x6B, 0x6D, 0x71, 0x70, 0x6F, 0x69, 0x69, 0x64},
//                        {0x7B, 0x76, 0x95, 0x8E, 0x91, 0x98, 0x91, 0x89, 0x92, 0x7A, 0x5D},
//                        {0x62, 0x89, 0xAE, 0xA9, 0xA6, 0xA4, 0xA1, 0xA1, 0x9E, 0x85, 0x5C},
//                        {0x6B, 0x76, 0x91, 0x93, 0x7F, 0x88, 0x84, 0x99, 0xA1, 0x76, 0x55},
//                        {0x65, 0x66, 0x64, 0x64, 0x61, 0x5D, 0x5E, 0x94, 0x90, 0x5B, 0x54},
//                        {0x63, 0x64, 0x60, 0x60, 0x5B, 0x58, 0x71, 0x9A, 0x8B, 0x4E, 0x4E},
//                        {0x63, 0x65, 0x63, 0x60, 0x5D, 0x5E, 0x7D, 0x99, 0x79, 0x56, 0x51},
//                        {0x67, 0x6A, 0x6A, 0x65, 0x64, 0x61, 0x8B, 0x94, 0x72, 0x56, 0x49},
//                        {0x63, 0x6D, 0x6A, 0x68, 0x65, 0x74, 0x9C, 0x94, 0x5D, 0x4E, 0x48},
//                        {0x62, 0x6A, 0x68, 0x66, 0x63, 0x76, 0xA5, 0x94, 0x54, 0x49, 0x40},
//                        {0x62, 0x66, 0x63, 0x63, 0x5D, 0x92, 0xA5, 0x86, 0x4E, 0x43, 0x3E},
//                        {0x5E, 0x65, 0x65, 0x60, 0x6C, 0x9C, 0xA6, 0x79, 0x4B, 0x43, 0x3C},
//                        {0x5B, 0x58, 0x58, 0x56, 0x6C, 0xA3, 0x96, 0x59, 0x42, 0x40, 0x40},
//                        {0x5D, 0x58, 0x5A, 0x58, 0x81, 0xA9, 0x86, 0x48, 0x43, 0x42, 0x40},
//                        {0x5E, 0x62, 0x60, 0x65, 0x96, 0xA3, 0x69, 0x46, 0x45, 0x44, 0x40},
//                        {0x5E, 0x5B, 0x62, 0x72, 0x9A, 0x9B, 0x59, 0x48, 0x48, 0x46, 0x44},
//                        {0x62, 0x5B, 0x62, 0x6A, 0x9E, 0x7E, 0x48, 0x4E, 0x4E, 0x49, 0x48},
//                        {0x62, 0x63, 0x65, 0x6A, 0x61, 0x55, 0x58, 0x56, 0x50, 0x50, 0x4B},};
static const rt_uint8_t standard_eight[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH] = {};
static const rt_uint8_t standard_nine[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH] = {};
static const rt_uint8_t standard_ten[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH] = {};


void Image_Recognition_Reponse(void);  //图像识别任务调用函数

#endif /*USE_STM32IPL 是否使用IPL库*/
#endif /* APPLICATIONS_IMAGE_RECOGNITION_H_ */
