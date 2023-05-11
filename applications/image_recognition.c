#include <rtthread.h>
#include <rtdevice.h>
#include <stdlib.h>
#include "image_recognition.h"
#include "GC0308.h"
#include "i2c.h"


#define DBG_TAG "image_recognition"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

static rt_uint8_t  Binary_JpegBuffer[INPUT_HEIGHT][INPUT_WIDTH];        //图片二值化缓冲
static rt_uint8_t  Gray_Scale_JpegBuffer[INPUT_HEIGHT][INPUT_WIDTH];    //图片灰度化缓冲
extern rt_uint16_t JpegBuffer[INPUT_HEIGHT][INPUT_WIDTH];               //图片缓冲
extern struct rt_semaphore dcmi_sem;            //DCMI帧事件中断 回调函数信号量

extern Camera_Structure camera_device_t;    //摄像头设备结构体
static rt_thread_t rmage_recognition_response_t;   //图像识别任务结构体

static void Color_Channel_Rgb565(char *str)	    //色彩通道
{
    rt_uint8_t  red   = 0;  //红
    rt_uint8_t  green = 0;  //绿
    rt_uint8_t  blue  = 0;  //蓝
    rt_uint16_t pixel = 0;  //原图像素

    for (int col = 0; col < INPUT_HEIGHT; col++) {
        for (int row = 0; row < INPUT_WIDTH; row++) {

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

    for (int col = 0; col < INPUT_HEIGHT; col++) {
        for (int row = 0; row < INPUT_WIDTH; row++) {

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

    for (int col = 0; col < INPUT_HEIGHT; col++) {
        for (int row = 0; row < INPUT_WIDTH; row++) {

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

static void Split_An_Image_Into_Subimages(  rt_uint8_t (*subImages)[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH],
                                            rt_uint8_t row, rt_uint8_t column)  //分割子图像
{
    LOG_I("row = %d, column = %d", row, column);

    for (int y = 0; y < SUBIMAGE_HEIGHT; y++){
        for (int x = 0; x < SUBIMAGE_WIDTH; x++){

//            (*subImages)[y][x] = JpegBuffer[column + y][row + x];               //原图  rt_uint16_t
            (*subImages)[y][x] = Gray_Scale_JpegBuffer[column + y][row + x];    //灰度  rt_uint8_t
//            (*subImages)[y][x] = Binary_JpegBuffer[column + y][row + x];        //二值化 rt_uint8_t

//            LOG_I("y = %d, x = %d", y, x);
        }
    }
}


static void resize_image(   rt_uint8_t (*subImages)[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH],
                            rt_uint8_t (*resized_subImage)[STANDARD_IMAGE_HEIGHT][STANDARD_IMAGE_WIDTH],
                            int column, int row, int new_column, int new_row)       //缩放图片(使用双线性插值算法)
{
    float scale_x = (float)(column) / new_column;       //缩放宽比例
    float scale_y = (float)(row) / new_row;             //缩放长比例

    LOG_I("(scale_x = %f) = (column=%d)/(new_column=%d)", scale_x, column, new_column);
    LOG_I("(scale_y = %f) = (row=%d)/(new_row=%d)\n", scale_y, row, new_row);

    for (int y = 0; y < new_row; ++y){                          //遍历目标图像(输出)的所有行

        float PosY = (float)((y + 0.5) * scale_y - 0.5);        //计算源图像(输入)中对应的列坐标
        int NewY = (int)PosY;                   //向下取整
        int PartY;                              //对应表达式中的Y
        int InvY;                               //对应表达式中的1-Y
//        LOG_I("(PosY=%f)=(j+0.5=%.2f)*(scale_y=%.3f)-0.5", PosY, (float)j+0.5, scale_y);

        if (NewY < 0)                           //NewY不要超出行索引
            PartY = 0, NewY = 0;
        if (NewY >= row - 1)
            PartY = 0, NewY = row - 2;

        PartY = max(((PosY - NewY) * 2048), 0); //对应表达式中的Y
        InvY = 2048 - PartY;                    //对应表达式中的1-Y
//        LOG_I("(PartY=%d)=((PosY=%f)-(NewY=%d))*2048    (InvY=%d)=2048-PartY\n", PartY, PosY, NewY, InvY);

        for (int x = 0; x < new_column; ++x){                   //遍历目标图像(输出)的所有列

               float PosX = (float)((x + 0.5) * scale_x - 0.5); //计算源图像(输入)中对应的行坐标
               int NewX = (int)PosX;            //向下取整
               int PartX;                       //对应表达式中的X
               int InvX;                        //对应表达式中的1-X

//               LOG_I("(PosX=%f)=(i+0.5=%.2f)*(scale_x=%.3f)-0.5", PosX, (float)i+0.5, scale_x);

               if (NewX < 0)                    //NewX不要超出行索引
                   PartX = 0, NewX = 0;
               if (NewX >= column - 1)
                   PartX = 0, NewX = column - 2;

               PartX = max(((PosX - NewX) * 2048), 0);          //对应表达式中的X
               InvX = 2048 - PartX;                             //对应表达式中的1-X

//               LOG_I("(PartX=%d)=((PosX=%f)-(NewX=%d))*2048    (InvX=%d)=2048-PartX", PartX, PosX, NewX, InvX);

               /*((f(NewX,NewY)*(1-x)+f(NewX+1,NewY)*x)*(1-y)+(f(NewX,NewY+1)*(1-x)+f(NewX+1,NewY+1)*x)*y)>>22*/
               (*resized_subImage)[y][x] = (
                       ((*subImages)[NewY][NewX]     * InvX + (*subImages)[NewY][NewX+1]       * PartX) * InvY +
                       ((*subImages)[NewY + 1][NewX] * InvX + (*subImages)[NewY + 1][NewX + 1] * PartX) * PartY
                       )>>22;
//               LOG_I("[y=%d][x=%d]  [NewX=%d][NewY=%d]", y, x, NewX, NewY);
        }
//        LOG_I("\n");
    }

//    rt_device_write(camera_device_t.uart, 0, *resized_subImage, sizeof(*resized_subImage));        //输出缩放图
}

//rt_uint8_t Variance_ariance_Digital_Recognition(rt_uint8_t (*image)[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH])  //方差判断数字
//{
//    rt_int32_t standard_mun_one = 0;
//    rt_int32_t standard_mun_two = 0;
//    rt_int32_t standard_mun_three = 0;
//    rt_int32_t standard_mun_four = 0;
//    rt_int32_t standard_mun_five = 0;
//    rt_int32_t standard_mun_six = 0;
//    rt_int32_t standard_mun_seven = 0;
//    rt_int32_t standard_mun_eight = 0;
//    rt_int32_t standard_mun_nine = 0;
//    rt_int32_t standard_mun_ten = 0;
//
//    for(int i = 0; i < SUBIMAGE_HEIGHT; i++)
//    {
//        for(int j = 0; j < SUBIMAGE_WIDTH; j++)
//        {
//            standard_mun_one   += ((*image)[i][j] - standard_one[i][j]);
//            standard_mun_two   += ((*image)[i][j] - standard_two[i][j]);
//            standard_mun_three += ((*image)[i][j] - standard_three[i][j]);
//            standard_mun_four  += ((*image)[i][j] - standard_four[i][j]);
//            standard_mun_five  += ((*image)[i][j] - standard_five[i][j]);
//            standard_mun_six   += ((*image)[i][j] - standard_six[i][j]);
//            standard_mun_seven += ((*image)[i][j] - standard_seven[i][j]);
//            standard_mun_eight += ((*image)[i][j] - standard_eight[i][j]);
//            standard_mun_nine  += ((*image)[i][j] - standard_nine[i][j]);
//            standard_mun_ten   += ((*image)[i][j] - standard_ten[i][j]);
//        }
//    }
//
////    if(standard_mun_one < 1)
////        return 1;
////    else if(standard_mun_two   < 1)
////        return 2;
////    else if(standard_mun_three < 1)
////        return 3;
////    else if(standard_mun_four  < 1)
////        return 4;
////    else if(standard_mun_five  < 1)
////        return 5;
////    else if(standard_mun_six   < 1)
////        return 6;
////    else if(standard_mun_seven < 1000)
////        return 7;
////    else if(standard_mun_eight < 1)
////        return 8;
////    else if(standard_mun_nine  < 1)
////        return 9;
////    else if(standard_mun_ten   < 1)
////        return 0;
//    LOG_I("standard_mun_seven = %d", standard_mun_seven);
//    if(standard_mun_seven < 1000)
//        return 7;
//    else
//        return -1;
//}

void Split_Image_Into_Subimages(rt_uint8_t row, rt_uint8_t column)
{
//    rt_uint16_t inputImage_row_subimage    = INPUT_HEIGHT - SUBIMAGE_WIDTH + 1;    //子图像=裁剪后的图像
//    rt_uint16_t inputImage_column_subimage = INPUT_WIDTH  - SUBIMAGE_HEIGHT    + 1;
    rt_uint8_t (*subImages)[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH] = (rt_uint8_t (*)[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH])rt_malloc(SUBIMAGE_HEIGHT * SUBIMAGE_WIDTH * sizeof(rt_uint8_t)); //子图像动态内存
    rt_uint8_t (*resized_subImage)[STANDARD_IMAGE_HEIGHT][STANDARD_IMAGE_WIDTH] = (rt_uint8_t (*)[STANDARD_IMAGE_HEIGHT][STANDARD_IMAGE_WIDTH])rt_malloc(STANDARD_IMAGE_HEIGHT * STANDARD_IMAGE_WIDTH * sizeof(rt_uint8_t)); //缩放后的图像动态内存

//    int recognition_result = -1;    //识别结果标识位

    if ((subImages == NULL) || (resized_subImage == NULL)) {    //判断内存是否成功申请
        LOG_E("Failed to allocate memory for subImages or resized_subImage");return;}

//    for (int column = 0; column < inputImage_column_subimage; column++) {
//        for (int row = 0; row < inputImage_row_subimage; row++) {

            Split_An_Image_Into_Subimages(subImages, row, column);  //分割子图像

            resize_image(subImages, resized_subImage, SUBIMAGE_WIDTH, SUBIMAGE_HEIGHT, STANDARD_IMAGE_WIDTH, STANDARD_IMAGE_HEIGHT);  //缩放图片

//            rt_device_write(camera_device_t.uart, 0, *subImages, sizeof(*subImages));        //输出子图像
            rt_device_write(camera_device_t.uart, 0, *resized_subImage, sizeof(*resized_subImage));        //输出缩放图

//            recognition_result = Digital_Recognition(subImages);            //方差判断数字 子图像
//            recognition_result = Digital_Recognition(resized_subImage);     //方差判断数字 缩放后的图像
//            if(recognition_result == 7)
//                LOG_I("recognition_result = %d", recognition_result);

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
    LOG_I("Image_Recognition_Reponse Init Success");
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
    const char *take = "Take_Picture";
    const char *no = "No_output";
    rt_uint8_t *string[2] = {(rt_uint8_t *)take, (rt_uint8_t *)no};

    Take_Picture(2, string);
    Gray_Scale_Rgb565();

    if(argc == 1)
        rt_device_write(camera_device_t.uart, 0, (rt_uint8_t *)Gray_Scale_JpegBuffer, PICTURE_BUFFER_LENGTH*2);     //输出灰度调整后的图像
    else if(argc == 2)
        LOG_I("图像灰度化成功");
//        LOG_I("Grayscale image succeeded");
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
    const char *gray = "Gray_Scale";
    const char *no = "No_output";
    rt_uint8_t *string[2] = {(rt_uint8_t *)gray, (rt_uint8_t *)no};
//    rt_uint8_t (*subImages)[SUBIMAGE_WIDTH][SUBIMAGE_HEIGHT] = (rt_uint8_t (*)[SUBIMAGE_WIDTH][SUBIMAGE_HEIGHT])rt_malloc(SUBIMAGE_WIDTH * SUBIMAGE_HEIGHT * sizeof(rt_uint8_t)); //子图像动态内存
    rt_uint8_t (*subImages)[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH] = (rt_uint8_t (*)[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH])rt_malloc(SUBIMAGE_HEIGHT * SUBIMAGE_WIDTH * sizeof(rt_uint8_t)); //子图像动态内存

    if(argc != 3)
    {
        Gray_Scale(2, string);
//        Split_An_Image_Into_Subimages(subImages, 2, 18);
        Split_An_Image_Into_Subimages(subImages, 0, 0);
        LOG_I("子图像分割成功");
//        LOG_I("Subimage segmentation succeeded");
    }
    else if(argc == 3)
    {
        rt_uint8_t inputImage_row = (rt_uint8_t)atoi((const char *)argv[1]);        //相当于子图像x轴位置
        rt_uint8_t inputImage_column = (rt_uint8_t)atoi((const char *)argv[2]);     //相当于子图像y轴位置

        Gray_Scale(2, string);
        Split_An_Image_Into_Subimages(subImages, inputImage_row, inputImage_column);
    }

    rt_device_write(camera_device_t.uart, 0, *subImages, sizeof(*subImages));        //输出子图像
    rt_free(subImages);     //释放子图像内存
}

void Sub_Images(int argc, rt_uint8_t *argv[])
{
    const char *gray = "Gray_Scale";
    const char *no = "No_output";
    rt_uint8_t *string[2] = {(rt_uint8_t *)gray, (rt_uint8_t *)no};

    if(argc != 3)
    {
        Gray_Scale(2, string);
//        Split_Image_Into_Subimages(2, 18);
        Split_Image_Into_Subimages(0, 0);
    }
    else if(argc == 3)
    {
        rt_uint8_t inputImage_row = (rt_uint8_t)atoi((const char *)argv[1]);
        rt_uint8_t inputImage_column = (rt_uint8_t)atoi((const char *)argv[2]);

        Gray_Scale(2, string);
        Split_Image_Into_Subimages(inputImage_row, inputImage_column);
    }
}



/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(Color_Channel, Set the image color channel:<Color_Channel <str>>);
MSH_CMD_EXPORT(Gray_Scale, Gray scale the image);
MSH_CMD_EXPORT(Binary_Image, Binary image :<Binary_Image < >|<average>>);
MSH_CMD_EXPORT(SubImages_An, Output one subimage :<SubImages_An <x> <y>>);
MSH_CMD_EXPORT(Sub_Images, Output subimage);
