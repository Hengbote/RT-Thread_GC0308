#include <rtthread.h>
#include <rtdevice.h>
#include <stdlib.h>
#include "image_recognition.h"
#include "GC0308.h"
#include "i2c.h"

#define BIBYTE_PICTURE_BUFFER_LENGTH PICTURE_SIZE(FRAME_SIZE)  //2字节图像缓冲长度
#define SUBIMAGEWIDTH 14    //子图像宽
#define SUBIMAGEHEIGHT 20   //子图像高
//#define INPUTWIDTH 160      //输入图像宽
//#define INPUTHEIGHT 120     //输入图像高

#define DBG_TAG "image_recognition"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

extern rt_uint32_t JpegBuffer[PICTURE_BUFFER_LENGTH];               //图片缓冲
static rt_uint16_t Binary_JpegBuffer[BIBYTE_PICTURE_BUFFER_LENGTH]; //图片二值化缓冲

/*动态分配内存_子图像*/
//rt_uint16_t inputImage_in_row_subimage_number    = resolution[FRAME_SIZE].width  - SUBIMAGEWIDTH  + 1;  //输入图像每列中子图像数
//rt_uint16_t inputImage_in_column_subimage_number = resolution[FRAME_SIZE].height - SUBIMAGEHEIGHT + 1;  //输入图像每行中子图像数
//rt_uint16_t (**subImages)[SUBIMAGEWIDTH * SUBIMAGEHEIGHT];              //子图像首地址
//subImages = (rt_uint16_t (*)[SUBIMAGEWIDTH * SUBIMAGEHEIGHT])malloc(sizeof(rt_uint16_t) * SUBIMAGEWIDTH * SUBIMAGEHEIGHT * subImages_cols_number * subImages_rows_number);子图像数组
/*动态分配内存_子图像*/

extern Camera_Structure camera_device_t;    //摄像头设备
rt_thread_t rmage_recognition_response_t;   //图像识别任务结构体

static void Binary_Threshold_Rgb565(rt_uint16_t *imageBuffer, framesize_t framesize)    //二值化
{
    rt_uint16_t  pixel_color = 0;
    rt_uint16_t average_color = 0;
    rt_uint32_t size = resolution[framesize].width * resolution[framesize].height;

    for (int i = 0; i < size; i++) {
        rt_uint16_t pixel = imageBuffer[i];

        // 提取RGB分量
        rt_uint8_t red = (pixel & 0xF800) >> 11;
        rt_uint8_t green = (pixel & 0x07E0) >> 5;
        rt_uint8_t blue = (pixel & 0x001F);

        pixel_color = (red + green/2 + blue);

        if(i < 20)
        {
            average_color += pixel_color;
            LOG_I("pixel_color = %d r = %d g = %d b = %d", pixel_color, red, (green/2), blue);
        }
        else if(i == 20)
        {
            average_color /= 20;
            LOG_I("average_color = %d", average_color);
        }

        // 二值化处理 96
        if (pixel_color >= (average_color - 10))
            Binary_JpegBuffer[i] = 0xFFFF; // 设置为白色
        else
            Binary_JpegBuffer[i] = 0x0000; // 设置为黑色
    }
}

static void Set_Binary_Threshold_Rgb565(rt_uint16_t *imageBuffer, framesize_t framesize, rt_uint8_t set_value)    //二值化
{
    rt_uint16_t  pixel_color = 0;
    rt_uint32_t size = resolution[framesize].width * resolution[framesize].height;

    LOG_I("set_value = %d", set_value);

    for (int i = 0; i < size; i++) {
        rt_uint16_t pixel = imageBuffer[i];

        //提取RGB分量
        rt_uint8_t red = (pixel & 0xF800) >> 11;
        rt_uint8_t green = (pixel & 0x07E0) >> 5;
        rt_uint8_t blue = (pixel & 0x001F);

        pixel_color = (red + green/2 + blue);

        // 二值化处理 96
        if (pixel_color >= set_value)
            Binary_JpegBuffer[i] = 0xFFFF; // 设置为白色
        else
            Binary_JpegBuffer[i] = 0x0000; // 设置为黑色
    }
}

//void Split_Image_Into_Subimages(uint8_t *inputImage, uint8_t subImages[][subImageWidth * subImageHeight])
//{
//    for (int r = 0; r < rows; r++) {    //遍历输入图像的行
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

void Image_Recognition_Reponse_Callback(void *parameter)    //图像识别任务回调函数
{
    while(1)
    {
        rt_thread_mdelay(10000);
    }
}

void Image_Recognition_Reponse(void)    //图像识别任务调用函数
{
    rmage_recognition_response_t = rt_thread_create("rmage_recognition_response_t", Image_Recognition_Reponse_Callback, RT_NULL, 2048, 10, 10);
    if(rmage_recognition_response_t!=RT_NULL)rt_thread_startup(rmage_recognition_response_t);
    LOG_I("Image_Recognition_Reponse Init Success\r\n");
}

void Binary_Image(int argc, rt_uint8_t *argv[])     //二值化命令
{
    Binary_Threshold_Rgb565((rt_uint16_t *)JpegBuffer, FRAME_SIZE);
    rt_device_write(camera_device_t.uart, 0, (rt_uint8_t*)Binary_JpegBuffer, PICTURE_BUFFER_LENGTH*4);
}
void Set_Binary_Image(int argc, rt_uint8_t *argv[])     //二值化命令
{
    rt_uint8_t set_value = (rt_uint8_t)atoi((const char *)argv[1]);
    Set_Binary_Threshold_Rgb565((rt_uint16_t *)JpegBuffer, FRAME_SIZE, set_value);
    rt_device_write(camera_device_t.uart, 0, (rt_uint8_t *)Binary_JpegBuffer, PICTURE_BUFFER_LENGTH*4);
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(Binary_Image, Binary image);
MSH_CMD_EXPORT(Set_Binary_Image, Set Binary image :<Set_Binary_Image <set_value>>);
