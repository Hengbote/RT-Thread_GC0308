#include <rtthread.h>
#include <rtdevice.h>
#include <stdlib.h>
#include "image_recognition.h"
#include "GC0308.h"
#include "i2c.h"

#define SUBIMAGEWIDTH 12    //子图像宽
#define SUBIMAGEHEIGHT 20   //子图像高

#define DBG_TAG "image_recognition"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

static rt_uint8_t  Gray_Scale_JpegBuffer[INPUT_WIDTH][INPUT_HEIGHT];    //图片灰度化缓冲
extern rt_uint16_t JpegBuffer[INPUT_WIDTH][INPUT_HEIGHT];               //图片缓冲
static rt_uint16_t Binary_JpegBuffer[INPUT_WIDTH][INPUT_HEIGHT];        //图片二值化缓冲

/*动态分配内存_子图像*/
//rt_uint16_t (**subImages)[SUBIMAGEWIDTH * SUBIMAGEHEIGHT];              //子图像首地址
//subImages = (rt_uint16_t (*)[SUBIMAGEWIDTH * SUBIMAGEHEIGHT])malloc(sizeof(rt_uint16_t) * SUBIMAGEWIDTH * SUBIMAGEHEIGHT * subImages_cols_number * subImages_rows_number);子图像数组
/*动态分配内存_子图像*/

extern Camera_Structure camera_device_t;    //摄像头设备
rt_thread_t rmage_recognition_response_t;   //图像识别任务结构体

static void Binary_Threshold_Rgb565(void)    //二值化
{
    rt_uint8_t red = 0;
    rt_uint8_t green = 0;
    rt_uint8_t blue = 0;
    rt_uint16_t pixel = 0;          //原图像素
    float average_color = 0;  //二值化颜色分界值
    float pixel_color = 0.0;    //像素颜色叠加值

    for (int col = 0; col < INPUT_WIDTH; col++) {           //有多少列
        for (int row = 0; row < INPUT_HEIGHT; row++) {      //有多少行

//            pixel = JpegBuffer[col][row];               //获得输入图像像素
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

            if((row < 20) && col == 0)
            {
                average_color += pixel_color;
                LOG_I("pixel=%#x", pixel);
                LOG_I("(pixel & 0xF800)=%#x r=%#x", (pixel & 0xF800), red);
                LOG_I("(pixel & 0x07E0)=%#x g=%#x", (pixel & 0x07E0), green);
                LOG_I("(pixel & 0x001F)=%#x b=%#x", (pixel & 0x001F), blue);
                LOG_I("pixel_color=%.3f pixel_color=%d r=%.2f g=%.2f b=%.2f ", pixel_color, (rt_uint8_t)pixel_color, red*0.21, green*0.72, blue*0.07);
                LOG_I("\n");
            }
            else if(row == 20 && col == 0)
            {
                average_color /= 20;
                LOG_I("average_color = %f.3", average_color);
            }

            // 二值化处理
            if (pixel_color >= average_color)
                Binary_JpegBuffer[col][row] = 0xFFFF; // 设置为白色
            else
                Binary_JpegBuffer[col][row] = 0x0000; // 设置为黑色

//            Gray_Scale_JpegBuffer[col][row] = (rt_uint8_t)pixel_color;   //灰度化
        }
    }
}


static void Set_Binary_Threshold_Rgb565(rt_uint8_t set_value)    //二值化
{
    rt_uint8_t red = 0;
    rt_uint8_t green = 0;
    rt_uint8_t blue = 0;
    rt_uint16_t pixel = 0;
    float pixel_color = 0;

    for (int col = 0; col < INPUT_WIDTH; col++) {           //有多少列
        for (int row = 0; row < INPUT_HEIGHT; row++) {      //有多少行

//            pixel = JpegBuffer[col][row];               //获得输入图像像素
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

//            // 二值化处理
//            if (pixel_color >= set_value)
//                Binary_JpegBuffer[col][row] = 0xFFFF;   //设置为白色
//            else
//                Binary_JpegBuffer[col][row] = 0x0000;   //设置为黑色

            Gray_Scale_JpegBuffer[col][row] = (rt_uint8_t)pixel_color;   //灰度化
        }
    }
}

static void Color_Channel_Rgb565(char *str)    //色彩通道
{
    rt_uint8_t red = 0;
    rt_uint8_t green = 0;
    rt_uint8_t blue = 0;
    rt_uint16_t pixel = 0;

    for (int col = 0; col < INPUT_WIDTH; col++) {           //有多少列
        for (int row = 0; row < INPUT_HEIGHT; row++) {      //有多少行

//            pixel = JpegBuffer[col][row];               //获得输入图像像素
            pixel = ((JpegBuffer[col][row] & 0xFF00) >> 8) | ((JpegBuffer[col][row] & 0x00FF) << 8); //获得输入图像像素并调整字节顺序

            red     = (pixel & 0xF800) >> 8;    //248
            green   = (pixel & 0x07E0) >> 3;    //252
            blue    = (pixel & 0x001F) << 3;    //248

            if((row < 20) && col == 0)
            {
                LOG_I("pixel=%#x", pixel);
                LOG_I("(pixel & 0xF800)=%#x r=%#x", (pixel & 0xF800), red);
                LOG_I("(pixel & 0x07E0)=%#x g=%#x", (pixel & 0x07E0), green);
                LOG_I("(pixel & 0x001F)=%#x b=%#x", (pixel & 0x001F), blue);
                LOG_I("\n");
            }

            if(*str == 'r')
                Gray_Scale_JpegBuffer[col][row] = (rt_uint8_t)red;
            else if(*str == 'g')
                Gray_Scale_JpegBuffer[col][row] = (rt_uint8_t)green;
            else if(*str == 'b')
                Gray_Scale_JpegBuffer[col][row] = (rt_uint8_t)blue;
        }
    }
}

//void Split_Image_Into_Subimages(uint8_t *inputImage, uint8_t subImages[][SUBIMAGEWIDTH * SUBIMAGEHEIGHT])
//{
//    rt_uint16_t inputImage_in_row_subimage_number    = resolution[FRAME_SIZE].width  - SUBIMAGEWIDTH  + 1;  //输入图像中每行中子图像数
//    rt_uint16_t inputImage_in_column_subimage_number = resolution[FRAME_SIZE].height - SUBIMAGEHEIGHT + 1;  //输入图像中每列中子图像数
//
//    for (int row = 0; row < inputImage_in_row_subimage_number; row++) {                     //每行中子图像数
//        for (int column = 0; column < inputImage_in_column_subimage_number; column++) {     //每列中子图像数
//            for (int y = 0; y < SUBIMAGEHEIGHT; y++) {                                      //子图像高
//                for (int x = 0; x < SUBIMAGEWIDTH; x++) {                                   //子图像宽
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
    Binary_Threshold_Rgb565();
    rt_device_write(camera_device_t.uart, 0, (rt_uint16_t *)Binary_JpegBuffer, PICTURE_BUFFER_LENGTH*4);
}
void Set_Binary_Image(int argc, rt_uint8_t *argv[])     //二值化命令
{
    if(argc < 2)
        Set_Binary_Threshold_Rgb565(50);
    else if(argc == 2)
    {
        rt_uint8_t set_value = (rt_uint8_t)atoi((const char *)argv[1]);
        Set_Binary_Threshold_Rgb565(set_value);
    }

//    rt_device_write(camera_device_t.uart, 0, (rt_uint16_t *)Binary_JpegBuffer, PICTURE_BUFFER_LENGTH*4);    //二值化
    rt_device_write(camera_device_t.uart, 0, (rt_uint8_t *)Gray_Scale_JpegBuffer, PICTURE_BUFFER_LENGTH*2);    //灰度化
}
void Color_Channel(int argc, rt_uint8_t *argv[])     //二值化命令
{
    Color_Channel_Rgb565((char *)argv[1]);
    rt_device_write(camera_device_t.uart, 0, (rt_uint8_t *)Gray_Scale_JpegBuffer, PICTURE_BUFFER_LENGTH*2);    //灰度化
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(Color_Channel, Color channel set:<Color_Channel <str>>);
MSH_CMD_EXPORT(Binary_Image, Binary image);
MSH_CMD_EXPORT(Set_Binary_Image, Set Binary image :<Set_Binary_Image <set_value>>);
