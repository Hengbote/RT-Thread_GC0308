#include <rtthread.h>
#include <rtdevice.h>
#include "image_recognition.h"
#include "GC0308.h"
#include "i2c.h"

//#define pictureBufferLength 48000
#define SUBIMAGEWIDTH 14    //子图像宽
#define SUBIMAGEHEIGHT 20   //子图像高
#define INPUTWIDTH 160      //输入图像宽
#define INPUTHEIGHT 120     //输入图像高
#define SUB_IMAGES_COLS_NUMBER (INPUTWIDTH - SUBIMAGEWIDTH + 1)     //子图像列数
#define SUB_IMAGES_ROWS_NUMBER (INPUTHEIGHT - SUBIMAGEHEIGHT + 1)   //子图像行数

#define DBG_TAG "image_recognition"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

extern rt_uint32_t JpegBuffer[pictureBufferLength];
static rt_uint16_t Binary_JpegBuffer[Binary_pictureBufferLength];

//rt_uint16_t subImages_cols_number = INPUTWIDTH - SUBIMAGEWIDTH + 1;     //子图像列数
//rt_uint16_t subImages_rows_number = INPUTHEIGHT - SUBIMAGEHEIGHT + 1;   //子图像行数
rt_uint16_t subImages[SUB_IMAGES_COLS_NUMBER * SUB_IMAGES_ROWS_NUMBER][SUBIMAGEWIDTH * SUBIMAGEHEIGHT];    //子图像

extern Camera_Structure camera_device_t;    //摄像头设备
rt_thread_t rmage_recognition_response_t;   //图像识别任务结构体

void binary_threshold_rgb565(rt_uint16_t *imageBuffer, int width, int height) {
    int size = width * height;

    for (int i = 0; i < size; i++) {
        rt_uint16_t pixel = imageBuffer[i];

        // 提取RGB分量
        rt_uint8_t red = (pixel & 0xF800) >> 11;
        rt_uint8_t green = (pixel & 0x07E0) >> 5;
        rt_uint8_t blue = (pixel & 0x001F);

        // 二值化处理 96
        if (red + green/2 + blue >= 50)
            Binary_JpegBuffer[i] = 0xFFFF; // 设置为白色
        else
            Binary_JpegBuffer[i] = 0x0000; // 设置为黑色
    }
}

//void split_image_into_subimages(uint8_t *inputImage, uint8_t subImages[][subImageWidth * subImageHeight])
//{
//    for (int r = 0; r < rows; r++) {
//        for (int c = 0; c < cols; c++) {
//            for (int y = 0; y < subImageHeight; y++) {
//                for (int x = 0; x < subImageWidth; x++) {
//                    int inputIdx = (r + y) * inputWidth + (c + x);
//                    int subImageIdx = y * subImageWidth + x;
//                    subImages[r * cols + c][subImageIdx] = inputImage[inputIdx];
//                }
//            }
//        }
//    }
//}



void Image_Recognition_Reponse_Callback(void *parameter)
{

    while(1)
    {

        rt_thread_mdelay(10000);
    }
}

void Image_Recognition_Reponse(void)
{
    rmage_recognition_response_t = rt_thread_create("rmage_recognition_response_t", Image_Recognition_Reponse_Callback, RT_NULL, 2048, 10, 10);
    if(rmage_recognition_response_t!=RT_NULL)rt_thread_startup(rmage_recognition_response_t);
    LOG_I("Image_Recognition_Reponse Init Success\r\n");
}

void Binary_image(int argc, rt_uint8_t *argv[])
{
    binary_threshold_rgb565((rt_uint16_t*)JpegBuffer, 160, 120);
    rt_device_write(camera_device_t.uart, 0, (rt_uint8_t*)Binary_JpegBuffer, pictureBufferLength*4);
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(Binary_image, Binary image);
