#include <rtthread.h>
#include <rtdevice.h>
#include <stdlib.h>
#include "image_recognition.h"
#include "GC0308.h"
#include "i2c.h"


#define DBG_TAG "image_recognition"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

static rt_uint8_t  Binary_JpegBuffer[INPUT_WIDTH][INPUT_HEIGHT];        //图片二值化缓冲
static rt_uint8_t  Gray_Scale_JpegBuffer[INPUT_WIDTH][INPUT_HEIGHT];    //图片灰度化缓冲
extern rt_uint16_t JpegBuffer[INPUT_WIDTH][INPUT_HEIGHT];               //图片缓冲
extern struct rt_semaphore dcmi_sem;            //DCMI帧事件中断 回调函数信号量

extern Camera_Structure camera_device_t;    //摄像头设备结构体
static rt_thread_t rmage_recognition_response_t;   //图像识别任务结构体

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

static void Split_An_Image_Into_Subimages(rt_uint8_t (*subImages)[SUBIMAGE_ROW][SUBIMAGE_COLUMN], rt_uint8_t column, rt_uint8_t row)  //分割子图像
{
    for (int y = 0; y < SUBIMAGE_COLUMN; y++){                          //历遍子图像列数
        for (int x = 0; x < SUBIMAGE_ROW; x++){                         //历遍子图像行数

//            (*subImages)[x][y] = JpegBuffer[column + x][row + y];               //原图  rt_uint16_t
            (*subImages)[x][y] = Gray_Scale_JpegBuffer[column + x][row + y];    //灰度  rt_uint8_t
//            (*subImages)[x][y] = Binary_JpegBuffer[column + x][row + y];        //二值化 rt_uint8_t
        }
    }
}

static rt_uint8_t bilinear_interpolation(rt_uint8_t (*image)[SUBIMAGE_ROW][SUBIMAGE_COLUMN], int old_width, int old_height, float x_ratio, float y_ratio, int x, int y)    // 双线性插值函数
{
    int x1 = (int)(((x + 0.5)/ x_ratio) - 0.5);
    int y1 = (int)(((y + 0.5)/ y_ratio) - 0.5);
    int x2 = x1 + 1;
    int y2 = y1 + 1;

    float x_diff = (x_ratio * x) - x1;
    float y_diff = (y_ratio * y) - y1;

    rt_uint8_t intensity = (rt_uint8_t)(
        (*image)[y1][x1] * (1 - x_diff) * (1 - y_diff) +
        (*image)[y1][x2] * x_diff * (1 - y_diff) +
        (*image)[y2][x1] * (1 - x_diff) * y_diff +
        (*image)[y2][x2] * x_diff * y_diff);

    return intensity;
}

static void resize_image(rt_uint8_t (*image)[SUBIMAGE_ROW][SUBIMAGE_COLUMN], rt_uint8_t (*resized_image)[STANDARD_IMAGE_WIDTH][STANDARD_IMAGE_HEIGHT], int old_width, int old_height, int new_width, int new_height)       //缩放图片
{
    float x_ratio = (float)(old_width) / new_width;
    float y_ratio = (float)(old_height) / new_height;

    for (int y = 0; y < new_height; y++)
    {
        for (int x = 0; x < new_width; x++)
        {
            (*resized_image)[y][x] = bilinear_interpolation(image, old_width, old_height, x_ratio, y_ratio, x, y);
        }
    }
}

rt_uint8_t Variance_ariance_Digital_Recognition(rt_uint8_t (*image)[SUBIMAGE_ROW][SUBIMAGE_COLUMN])  //方差判断数字
{
    rt_int32_t standard_mun_one = 0;
    rt_int32_t standard_mun_two = 0;
    rt_int32_t standard_mun_three = 0;
    rt_int32_t standard_mun_four = 0;
    rt_int32_t standard_mun_five = 0;
    rt_int32_t standard_mun_six = 0;
    rt_int32_t standard_mun_seven = 0;
    rt_int32_t standard_mun_eight = 0;
    rt_int32_t standard_mun_nine = 0;
    rt_int32_t standard_mun_ten = 0;

    for(int i = 0; i < SUBIMAGE_ROW; i++)
    {
        for(int j = 0; j < SUBIMAGE_COLUMN; j++)
        {
            standard_mun_one   += ((*image)[i][j] - standard_one[i][j]);
            standard_mun_two   += ((*image)[i][j] - standard_two[i][j]);
            standard_mun_three += ((*image)[i][j] - standard_three[i][j]);
            standard_mun_four  += ((*image)[i][j] - standard_four[i][j]);
            standard_mun_five  += ((*image)[i][j] - standard_five[i][j]);
            standard_mun_six   += ((*image)[i][j] - standard_six[i][j]);
            standard_mun_seven += ((*image)[i][j] - standard_seven[i][j]);
            standard_mun_eight += ((*image)[i][j] - standard_eight[i][j]);
            standard_mun_nine  += ((*image)[i][j] - standard_nine[i][j]);
            standard_mun_ten   += ((*image)[i][j] - standard_ten[i][j]);
        }
    }

//    if(standard_mun_one < 1)
//        return 1;
//    else if(standard_mun_two   < 1)
//        return 2;
//    else if(standard_mun_three < 1)
//        return 3;
//    else if(standard_mun_four  < 1)
//        return 4;
//    else if(standard_mun_five  < 1)
//        return 5;
//    else if(standard_mun_six   < 1)
//        return 6;
//    else if(standard_mun_seven < 1000)
//        return 7;
//    else if(standard_mun_eight < 1)
//        return 8;
//    else if(standard_mun_nine  < 1)
//        return 9;
//    else if(standard_mun_ten   < 1)
//        return 0;
    LOG_I("standard_mun_seven = %d", standard_mun_seven);
    if(standard_mun_seven < 1000)
        return 7;
    else
        return -1;
}

void Split_Image_Into_Subimages(rt_uint8_t column, rt_uint8_t row)  //分割图像
{
//    rt_uint16_t inputImage_row_subimage    = INPUT_WIDTH  - SUBIMAGE_COLUMN + 1;    //输入图像中每行中子图像数
//    rt_uint16_t inputImage_column_subimage = INPUT_HEIGHT - SUBIMAGE_ROW    + 1;    //输入图像中每列中子图像数
    rt_uint8_t (*subImages)[SUBIMAGE_ROW][SUBIMAGE_COLUMN] = (rt_uint8_t (*)[SUBIMAGE_ROW][SUBIMAGE_COLUMN])rt_malloc(SUBIMAGE_ROW * SUBIMAGE_COLUMN * sizeof(rt_uint8_t)); //子图像动态内存
    rt_uint8_t (*resized_subImage)[STANDARD_IMAGE_WIDTH][STANDARD_IMAGE_HEIGHT] = (rt_uint8_t (*)[STANDARD_IMAGE_WIDTH][STANDARD_IMAGE_HEIGHT])rt_malloc(STANDARD_IMAGE_WIDTH * STANDARD_IMAGE_HEIGHT * sizeof(rt_uint8_t)); //缩放后的图像动态内存

//    int recognition_result = -1;    //识别结果标识位

    if ((subImages == NULL) || (resized_subImage == NULL)) {
        LOG_E("Failed to allocate memory for subImages or resized_subImage\n");return;}

//    for (int column = 0; column < inputImage_column_subimage; column++) {       //历遍每列中子图像
//        for (int row = 0; row < inputImage_row_subimage; row++) {               //历遍每行中子图像

            Split_An_Image_Into_Subimages(subImages, column, row);  //分割子图像

            LOG_I("column = %d, row = %d", column, row);

//            resize_image((*subImages), (rt_uint8_t (*)[STANDARD_IMAGE_WIDTH][STANDARD_IMAGE_HEIGHT])resized_subImage, SUBIMAGE_COLUMN, SUBIMAGE_ROW, STANDARD_IMAGE_WIDTH, STANDARD_IMAGE_HEIGHT);
            resize_image(subImages, resized_subImage, SUBIMAGE_COLUMN, SUBIMAGE_ROW, STANDARD_IMAGE_WIDTH, STANDARD_IMAGE_HEIGHT);  //缩放图片
            rt_device_write(camera_device_t.uart, 0, *resized_subImage, sizeof(*resized_subImage));        //输出子图像

//            recognition_result = Digital_Recognition(subImages);            //方差判断数字 子图像
//            recognition_result = Digital_Recognition(resized_subImage);     //方差判断数字 缩放后的图像
//            if(recognition_result == 7)
//                LOG_I("recognition_result = %d\r\n", recognition_result);

//        }/*历遍每行中子图像*/
//    }/*历遍每列中子图像*/

    rt_free(resized_subImage);  //释放缩放图像内存
    rt_free(subImages);         //释放子图像内存
//    if(recognition_result == -1)
//        LOG_E("识别失败");
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
    __HAL_DCMI_ENABLE_IT(camera_device_t.dcmi, DCMI_IT_FRAME);  //使用帧中断
    rt_memset((void *)JpegBuffer,0,PICTURE_BUFFER_LENGTH * sizeof(rt_uint32_t));  //把接收BUF清空
    //启动拍照    DCMI结构体指针 DCMI捕获模式 目标内存缓冲区地址 要传输的捕获长度
    HAL_DCMI_Start_DMA(camera_device_t.dcmi, DCMI_MODE_SNAPSHOT,(rt_uint32_t)JpegBuffer, PICTURE_BUFFER_LENGTH);    //启动拍照

    if(rt_sem_take(&dcmi_sem, RT_WAITING_FOREVER) == RT_EOK)
    {
        HAL_DCMI_Suspend(camera_device_t.dcmi);   //拍照完成，挂起DCMI
        HAL_DCMI_Stop(camera_device_t.dcmi);      //拍照完成，停止DMA传输
    }

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

void SubImages_An(int argc, rt_uint8_t *argv[])     //分割子图像
{
    rt_uint8_t (*subImages)[SUBIMAGE_ROW][SUBIMAGE_COLUMN] = (rt_uint8_t (*)[SUBIMAGE_ROW][SUBIMAGE_COLUMN])rt_malloc(SUBIMAGE_ROW * SUBIMAGE_COLUMN * sizeof(rt_uint8_t)); //子图像动态内存

    if(argc != 3)
        Split_An_Image_Into_Subimages(subImages, 0, 0);
    else if(argc == 3)
    {
        rt_uint8_t inputImage_row = (rt_uint8_t)atoi((const char *)argv[1]);        //相当于子图像x轴位置
        rt_uint8_t inputImage_column = (rt_uint8_t)atoi((const char *)argv[2]);     //相当于子图像y轴位置

        Split_An_Image_Into_Subimages(subImages, inputImage_column, inputImage_row);
    }

    rt_device_write(camera_device_t.uart, 0, *subImages, sizeof(*subImages));        //输出子图像
    rt_free(subImages);     //释放子图像内存
}

void Sub_Images(int argc, rt_uint8_t *argv[])
{
    if(argc != 3)
        Split_Image_Into_Subimages(0, 0);
    else if(argc == 3)
    {
        rt_uint8_t inputImage_row = (rt_uint8_t)atoi((const char *)argv[1]);
        rt_uint8_t inputImage_column = (rt_uint8_t)atoi((const char *)argv[2]);

        Split_Image_Into_Subimages(inputImage_column, inputImage_row);
    }
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(Color_Channel, Set the image color channel:<Color_Channel <str>>);
MSH_CMD_EXPORT(Gray_Scale, Gray scale the image);
MSH_CMD_EXPORT(Binary_Image, Binary image :<Binary_Image < >|<average>>);
MSH_CMD_EXPORT(SubImages_An, Output one subimage :<SubImages_An <x> <y>>);
MSH_CMD_EXPORT(Sub_Images, Output subimage);
