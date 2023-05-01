#include <rtthread.h>
#include <rtdevice.h>
#include <stdlib.h>
#include "image_recognition.h"
#include "GC0308.h"
#include "i2c.h"


#define SUBIMAGE_ROW    20   //历遍子图像行数(子图像高)
#define SUBIMAGE_COLUMN 12   //历遍子图像列数(子图像宽)

#define DBG_TAG "image_recognition"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

static rt_uint8_t  Binary_JpegBuffer[INPUT_WIDTH][INPUT_HEIGHT];        //图片二值化缓冲
static rt_uint8_t  Gray_Scale_JpegBuffer[INPUT_WIDTH][INPUT_HEIGHT];    //图片灰度化缓冲
extern rt_uint16_t JpegBuffer[INPUT_WIDTH][INPUT_HEIGHT];               //图片缓冲

extern Camera_Structure camera_device_t;    //摄像头设备
rt_thread_t rmage_recognition_response_t;   //图像识别任务结构体

static void Color_Channel_Rgb565(char *str)	    //色彩通道
{
    rt_uint8_t  red   = 0;  //红
    rt_uint8_t  green = 0;  //绿
    rt_uint8_t  blue  = 0;  //蓝
    rt_uint16_t pixel = 0;  //原图像素

    for (int col = 0; col < INPUT_WIDTH; col++) {           //有多少列
        for (int row = 0; row < INPUT_HEIGHT; row++) {      //有多少行

            pixel = ((JpegBuffer[col][row] & 0xFF00) >> 8) | ((JpegBuffer[col][row] & 0x00FF) << 8); //获得输入图像像素并调整字节顺序

            red     = (pixel & 0xF800) >> 8;    //248
            green   = (pixel & 0x07E0) >> 3;    //252
            blue    = (pixel & 0x001F) << 3;    //248

//            if((row < 20) && col == 0)
//            {
//                LOG_I("pixel=%#x", pixel);
//                LOG_I("(pixel & 0xF800)=%#x r=%#x", (pixel & 0xF800), red);
//                LOG_I("(pixel & 0x07E0)=%#x g=%#x", (pixel & 0x07E0), green);
//                LOG_I("(pixel & 0x001F)=%#x b=%#x", (pixel & 0x001F), blue);
//                LOG_I("\n");
//            }

            if(*str == 'r')
                Gray_Scale_JpegBuffer[col][row] = (rt_uint8_t)red;
            else if(*str == 'g')
                Gray_Scale_JpegBuffer[col][row] = (rt_uint8_t)green;
            else if(*str == 'b')
                Gray_Scale_JpegBuffer[col][row] = (rt_uint8_t)blue;
        }
    }
}

static void Gray_Scale_Rgb565(void)    	        //灰度调整
{
    rt_uint8_t  red   = 0;  //红
    rt_uint8_t  green = 0;  //绿
    rt_uint8_t  blue  = 0;  //蓝
    rt_uint16_t pixel = 0;  //原图像素
    float pixel_color = 0;  //像素灰度

    for (int col = 0; col < INPUT_WIDTH; col++) {           //有多少列
        for (int row = 0; row < INPUT_HEIGHT; row++) {      //有多少行

            pixel = ((JpegBuffer[col][row] & 0xFF00) >> 8) | ((JpegBuffer[col][row] & 0x00FF) << 8); //获得输入图像像素并调整字节顺序

            // 提取RGB565分量至RGB888
            red     = (pixel & 0xF800) >> 8;    //248
            green   = (pixel & 0x07E0) >> 3;    //252
            blue    = (pixel & 0x001F) << 3;    //248
            pixel_color = red*0.21 + green*0.72 + blue*0.07;    //R+G+B~255

            Gray_Scale_JpegBuffer[col][row] = (rt_uint8_t)pixel_color;   //灰度化
        }
    }
}

static void Binary_Threshold_Rgb565(rt_uint8_t average)    //二值化
{
    rt_uint8_t  red   = 0;      //红
    rt_uint8_t  green = 0;      //绿
    rt_uint8_t  blue  = 0;      //蓝
    rt_uint16_t pixel = 0;      //原图像素
    float pixel_color = 0.0;    //像素颜色叠加值

    for (int col = 0; col < INPUT_WIDTH; col++) {           //有多少列
        for (int row = 0; row < INPUT_HEIGHT; row++) {      //有多少行

            pixel = ((JpegBuffer[col][row] & 0xFF00) >> 8) | ((JpegBuffer[col][row] & 0x00FF) << 8); //获得输入图像像素并调整字节顺序

//            // 提取RGB565分量
//            red     = (pixel & 0xF800) >> 11;
//            green   = (pixel & 0x07E0) >> 5;
//            blue    = (pixel & 0x001F);
////            pixel_color = (red*2 + green + blue*2);
//            pixel_color = (red + green + blue/3);             //3:6:1 R+G+B~100
            // 提取RGB565分量至RGB888
            red     = (pixel & 0xF800) >> 8;    //248
            green   = (pixel & 0x07E0) >> 3;    //252
            blue    = (pixel & 0x001F) << 3;    //248
            pixel_color = red*0.21 + green*0.72 + blue*0.07;    //R+G+B~255

            // 二值化处理
            if (pixel_color >= average)
                Binary_JpegBuffer[col][row] = 0xFF; // 设置为白色
            else
                Binary_JpegBuffer[col][row] = 0x00; // 设置为黑色

        }
    }
}

void Split_Image_Into_Subimages(rt_uint8_t column, rt_uint8_t row)
{
//    rt_uint16_t inputImage_row_subimage    = INPUT_WIDTH  - SUBIMAGE_WIDTH  + 1;    //输入图像中每行中子图像数
//    rt_uint16_t inputImage_column_subimage = INPUT_HEIGHT - SUBIMAGE_HEIGHT + 1;    //输入图像中每列中子图像数
    rt_uint8_t (*subImages)[SUBIMAGE_ROW][SUBIMAGE_COLUMN] = rt_malloc(SUBIMAGE_ROW * SUBIMAGE_COLUMN * sizeof(rt_uint8_t)); //动态内存
//    rt_uint16_t (*subImages)[SUBIMAGE_ROW][SUBIMAGE_COLUMN] = rt_malloc(SUBIMAGE_ROW * SUBIMAGE_COLUMN * sizeof(rt_uint16_t)); //动态内存(原图)

    if (subImages == NULL) {
        LOG_E("Failed to allocate memory for subImages\n");
        return;
    }

//    for (int column = 0; column < inputImage_column_subimage; column++) {       //历遍每列中子图像
//        for (int row = 0; row < inputImage_row_subimage; row++) {               //历遍每行中子图像
            /*合成子图像*/
            for (int y = 0; y < SUBIMAGE_COLUMN; y++){                          //历遍子图像列数
                for (int x = 0; x < SUBIMAGE_ROW; x++){                         //历遍子图像行数

//                    (*subImages)[x][y] = JpegBuffer[column + x][row + y];               //原图
//                    (*subImages)[x][y] = Gray_Scale_JpegBuffer[column + x][row + y];    //灰度
                    (*subImages)[x][y] = Binary_JpegBuffer[column + x][row + y];        //二值化
                }
            }
            rt_device_write(camera_device_t.uart, 0, *subImages, sizeof(*subImages));        //输出子图像
            rt_free(subImages);
//        }
//    }
}

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

void Color_Channel(int argc, rt_uint8_t *argv[])    //提取色彩通道命令
{
    if(argc != 2)
        LOG_I(" Set the image color channel:<Color_Channel <str>>");
    if(argc == 2)
    {
        Color_Channel_Rgb565((char *)argv[1]);
        rt_device_write(camera_device_t.uart, 0, (rt_uint8_t *)Gray_Scale_JpegBuffer, PICTURE_BUFFER_LENGTH*2);     //输出提取到的色彩通道
    }
}

void Gray_Scale(int argc, rt_uint8_t *argv[])	    //灰度调整命令
{
    Gray_Scale_Rgb565();
    rt_device_write(camera_device_t.uart, 0, (rt_uint8_t *)Gray_Scale_JpegBuffer, PICTURE_BUFFER_LENGTH*2);     //输出灰度调整后的图像
}

void Binary_Image(int argc, rt_uint8_t *argv[])     //二值化命令
{
    if(argc < 2)
        Binary_Threshold_Rgb565(100);
    else if(argc == 2)
    {
        rt_uint8_t average = (rt_uint8_t)atoi((const char *)argv[1]);
        Binary_Threshold_Rgb565(average);
    }

    rt_device_write(camera_device_t.uart, 0, (rt_uint8_t *)Binary_JpegBuffer, PICTURE_BUFFER_LENGTH*2);        //输出二值化后的图像
}

void Sub_Images(int argc, rt_uint8_t *argv[])
{
    if(argc < 3)
        Split_Image_Into_Subimages(0, 0);
    else if(argc == 3)
    {
        rt_uint8_t inputImage_x = (rt_uint8_t)atoi((const char *)argv[1]);
        rt_uint8_t inputImage_y = (rt_uint8_t)atoi((const char *)argv[2]);

        Split_Image_Into_Subimages(inputImage_y, inputImage_x);
    }
    else
    {
        Split_Image_Into_Subimages(0, 0);
    }
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(Color_Channel, Set the image color channel:<Color_Channel <str>>);
MSH_CMD_EXPORT(Gray_Scale, Gray scale the image);
MSH_CMD_EXPORT(Binary_Image, Binary image :<Binary_Image < >|<average>>);
MSH_CMD_EXPORT(Sub_Images, Output subimage :<Sub_Images <x> <y>>);
