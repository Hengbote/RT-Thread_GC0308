#include <rtthread.h>
#include <rtdevice.h>
#include <stdlib.h>
#include "image_recognition.h"
#include "GC0308.h"
#include "i2c.h"
#include "lcd.h"

#define DBG_TAG "image_recognition"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

rt_uint8_t Target_Subimage_0[STANDARD_IMAGE_HEIGHT * STANDARD_IMAGE_WIDTH * STANDARD_IMAGE_COLOUR];   //目标子图片缓冲
rt_uint8_t Target_Subimage_1[STANDARD_IMAGE_HEIGHT * STANDARD_IMAGE_WIDTH * STANDARD_IMAGE_COLOUR];   //目标子图片缓冲
rt_uint8_t Target_Subimage_2[STANDARD_IMAGE_HEIGHT * STANDARD_IMAGE_WIDTH * STANDARD_IMAGE_COLOUR];   //目标子图片缓冲
rt_uint8_t Target_Subimage_3[STANDARD_IMAGE_HEIGHT * STANDARD_IMAGE_WIDTH * STANDARD_IMAGE_COLOUR];   //目标子图片缓冲
rt_uint8_t Target_Subimage_4[STANDARD_IMAGE_HEIGHT * STANDARD_IMAGE_WIDTH * STANDARD_IMAGE_COLOUR];   //目标子图片缓冲
rt_uint8_t Target_Subimage_5[STANDARD_IMAGE_HEIGHT * STANDARD_IMAGE_WIDTH * STANDARD_IMAGE_COLOUR];   //目标子图片缓冲
static rt_uint8_t  Binary_JpegBuffer[INPUT_HEIGHT][INPUT_WIDTH];                                    //图片二值化缓冲
static rt_uint8_t  Gray_Scale_JpegBuffer[INPUT_HEIGHT][INPUT_WIDTH];                                //图片灰度化缓冲
extern rt_uint16_t JpegBuffer[INPUT_HEIGHT][INPUT_WIDTH];                                           //原始图片缓冲

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

static void Split_An_Image_Into_Target_Subimages(   rt_uint16_t (*subImages)[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH],
                                                    rt_uint8_t row, rt_uint8_t column)  //分割子图像
{
//    LOG_I("row = %d, column = %d", row, column);

    for (int y = 0; y < SUBIMAGE_HEIGHT; y++){
        for (int x = 0; x < SUBIMAGE_WIDTH; x++){

            (*subImages)[y][x] = JpegBuffer[column + y][row + x];               //原图  rt_uint16_t

//            LOG_I("y = %d, x = %d", y, x);
        }
    }
}

void Split_Image_Conversion_Format( rt_uint8_t Target_Subimage[STANDARD_IMAGE_HEIGHT * STANDARD_IMAGE_WIDTH * STANDARD_IMAGE_COLOUR],
                                    rt_uint8_t row, rt_uint8_t column)  //分割子图像转换格式放入数组
{
    rt_uint8_t  red   = 0;      //红
    rt_uint8_t  green = 0;      //绿
    rt_uint8_t  blue  = 0;      //蓝
    rt_uint16_t pixel = 0;      //原图像素

    for (int y = 0; y < STANDARD_IMAGE_HEIGHT; y++){
        for (int x = 0; x < STANDARD_IMAGE_WIDTH; x++){

            pixel = ((JpegBuffer[column + y][row + x] & 0xFF00) >> 8) | ((JpegBuffer[column + y][row + x] & 0x00FF) << 8); //获得输入图像像素并调整字节顺序
//            rt_kprintf("%x, ", pixel);

            // 提取RGB565分量至RGB888
            red     = (pixel & 0xF800) >> 8;    //248
            green   = (pixel & 0x07E0) >> 3;    //252
            blue    = (pixel & 0x001F) << 3;    //248
//            LOG_I("red = %x, green = %x, blue = %x", red, green, blue);

            Target_Subimage[x*3 + (y*20*3) + 0] = red;
            Target_Subimage[x*3 + (y*20*3) + 1] = green;
            Target_Subimage[x*3 + (y*20*3) + 2] = blue;

//            LOG_I("a = %d, b = %d, c = %d", x*3 + (y*20*3) + 0, x*3 + (y*20*3) + 1, x*3 + (y*20*3) + 2);
        }
    }
}

static void resize_image(   rt_uint8_t (*subImages)[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH],
                            rt_uint8_t (*resized_subImage)[STANDARD_IMAGE_HEIGHT][STANDARD_IMAGE_WIDTH],
                            int column, int row, int new_column, int new_row)       //缩放图片(使用双线性插值算法)
{
    float scale_x = (float)(column) / new_column;       //缩放宽比例
    float scale_y = (float)(row) / new_row;             //缩放长比例

    LOG_I("(scale_x = %d.%d) = (column=%d)/(new_column=%d)", (int)scale_x, (int)((scale_x-(int)scale_x)*1000), column, new_column);
    LOG_I("(scale_y = %d.%d) = (row=%d)/(new_row=%d)\n", (int)scale_y, (int)((scale_y-(int)scale_y)*1000), row, new_row);

    for (int y = 0; y < new_row; ++y){                          //遍历目标图像(输出)的所有行

        float PosY = (float)((y + 0.5) * scale_y - 0.5);        //计算源图像(输入)中对应的列坐标
        int NewY = (int)PosY;                   //向下取整
        int PartY;                              //对应表达式中的Y
        int InvY;                               //对应表达式中的1-Y
//        LOG_I("(PosY=%d.%d)=(y+0.5=%d.5)*(scale_y=%d.%d)-0.5", (int)PosY, (int)((PosY-(int)PosY)*1000), y, (int)scale_y, (int)((scale_y-(int)scale_y)*1000));

        if (NewY < 0)                           //NewY不要超出行索引
            PartY = 0, NewY = 0;
        if (NewY >= row - 1)
            PartY = 0, NewY = row - 2;

        PartY = max(((PosY - NewY) * 2048), 0); //对应表达式中的Y
        InvY = 2048 - PartY;                    //对应表达式中的1-Y
//        LOG_I("(PartY=%d)=((PosY=%d.%d)-(NewY=%d))*2048    (InvY=%d)=2048-PartY\n", PartY, (int)PosY, (int)((PosY-(int)PosY)*1000), NewY, InvY);

        for (int x = 0; x < new_column; ++x){                   //遍历目标图像(输出)的所有列

               float PosX = (float)((x + 0.5) * scale_x - 0.5); //计算源图像(输入)中对应的行坐标
               int NewX = (int)PosX;            //向下取整
               int PartX;                       //对应表达式中的X
               int InvX;                        //对应表达式中的1-X

//               LOG_I("(PosX=%d.%d)=(x+0.5=%d.5)*(scale_x=%d.%d)-0.5", (int)PosX, (int)((PosX-(int)PosX)*1000), x, (int)scale_x, (int)((scale_x-(int)scale_x)*1000));

               if (NewX < 0)                    //NewX不要超出行索引
                   PartX = 0, NewX = 0;
               if (NewX >= column - 1)
                   PartX = 0, NewX = column - 2;

               PartX = max(((PosX - NewX) * 2048), 0);          //对应表达式中的X
               InvX = 2048 - PartX;                             //对应表达式中的1-X

//               LOG_I("(PartX=%d)=((PosX=%d.%d)-(NewX=%d))*2048    (InvX=%d)=2048-PartX", PartX, (int)PosX, (int)((PosX-(int)PosX)*1000), NewX, InvX);

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


void Split_Image_Into_Subimages(rt_uint8_t row, rt_uint8_t column)
{

    rt_uint16_t (*subImages)[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH] =
            (rt_uint16_t (*)[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH])
            rt_malloc(SUBIMAGE_HEIGHT * SUBIMAGE_WIDTH * sizeof(rt_uint16_t));               //子图像动态内存
    if (subImages == NULL){    //判断子图像内存是否成功申请
        LOG_E("Failed to allocate memory for subImages");return;}
//    rt_uint8_t (*subImages)[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH] =
//            (rt_uint8_t (*)[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH])
//            rt_malloc(SUBIMAGE_HEIGHT * SUBIMAGE_WIDTH * sizeof(rt_uint8_t));               //子图像动态内存
//    if (subImages == NULL){    //判断子图像内存是否成功申请
//        LOG_E("Failed to allocate memory for subImages");return;}

//    rt_uint8_t (*resized_subImage)[STANDARD_IMAGE_HEIGHT][STANDARD_IMAGE_WIDTH] =
//            (rt_uint8_t (*)[STANDARD_IMAGE_HEIGHT][STANDARD_IMAGE_WIDTH])
//            rt_malloc(STANDARD_IMAGE_HEIGHT * STANDARD_IMAGE_WIDTH * sizeof(rt_uint8_t));   //缩放后的图像动态内存
//    if (resized_subImage == NULL){    //判断缩放图内存是否成功申请
//        LOG_E("Failed to allocate memory for resized_subImage");return;}

//    int recognition_result = -1;    //识别结果标识位


    int y_skew = 65;    //y轴高度调整

    Split_Image_Conversion_Format(Target_Subimage_0, 136, y_skew+0);                //分割子图像转换格式放入数组
    Split_Image_Conversion_Format(Target_Subimage_1, 110, y_skew+1);                //分割子图像转换格式放入数组
    Split_Image_Conversion_Format(Target_Subimage_2, 84, y_skew+2);                 //分割子图像转换格式放入数组
    Split_Image_Conversion_Format(Target_Subimage_3, 58, y_skew+2);                 //分割子图像转换格式放入数组
    Split_Image_Conversion_Format(Target_Subimage_4, 33, y_skew+3);                 //分割子图像转换格式放入数组
    Split_Image_Conversion_Format(Target_Subimage_5, 6, y_skew+6);                  //分割子图像转换格式放入数组


//    for(int i = 0; i < STANDARD_IMAGE_HEIGHT * STANDARD_IMAGE_WIDTH * STANDARD_IMAGE_COLOUR; i++){
//        rt_kprintf("%3d", Target_Subimage_1[i]);}
//    rt_kprintf("\n");

    Split_An_Image_Into_Target_Subimages(subImages, 136, y_skew+0);                 //分割目标子图像
    LCD_Show_Target_Subimages(0,200,SUBIMAGE_HEIGHT,SUBIMAGE_WIDTH, *subImages);    //显示子图像
    Split_An_Image_Into_Target_Subimages(subImages, 110, y_skew+1);                 //分割目标子图像
    LCD_Show_Target_Subimages(0,230,SUBIMAGE_HEIGHT,SUBIMAGE_WIDTH, *subImages);    //显示子图像
    Split_An_Image_Into_Target_Subimages(subImages, 84, y_skew+2);                  //分割目标子图像
    LCD_Show_Target_Subimages(0,260,SUBIMAGE_HEIGHT,SUBIMAGE_WIDTH, *subImages);    //显示子图像
    Split_An_Image_Into_Target_Subimages(subImages, 58, y_skew+2);                  //分割目标子图像
    LCD_Show_Target_Subimages(0,290,SUBIMAGE_HEIGHT,SUBIMAGE_WIDTH, *subImages);    //显示子图像
    Split_An_Image_Into_Target_Subimages(subImages, 33, y_skew+3);                  //分割目标子图像
    LCD_Show_Target_Subimages(0,320,SUBIMAGE_HEIGHT,SUBIMAGE_WIDTH, *subImages);    //显示子图像
    Split_An_Image_Into_Target_Subimages(subImages, 6, y_skew+6);                   //分割目标子图像
    LCD_Show_Target_Subimages(0,350,SUBIMAGE_HEIGHT,SUBIMAGE_WIDTH, *subImages);    //显示子图像


//    镜像
//    int y_skew = 64;
//    Split_Image_Conversion_Format(Target_Subimage_0, 17, y_skew+0);                 //分割子图像转换格式放入数组
//    Split_Image_Conversion_Format(Target_Subimage_1, 46, y_skew+1);                 //分割子图像转换格式放入数组
//    Split_Image_Conversion_Format(Target_Subimage_2, 72, y_skew+2);                 //分割子图像转换格式放入数组
//    Split_Image_Conversion_Format(Target_Subimage_3, 98, y_skew+4);                 //分割子图像转换格式放入数组
//    Split_Image_Conversion_Format(Target_Subimage_4, 125, y_skew+5);                //分割子图像转换格式放入数组
//    Split_Image_Conversion_Format(Target_Subimage_5, 152, y_skew+6);                //分割子图像转换格式放入数组
//
//    for(int i = 0; i < STANDARD_IMAGE_HEIGHT * STANDARD_IMAGE_WIDTH * STANDARD_IMAGE_COLOUR; i++)
//        {rt_kprintf("%3d", Target_Subimage_1[i]);}
//
//    rt_kprintf("\n");
//
//    Split_An_Image_Into_Target_Subimages(subImages, 17, y_skew+0);                  //分割目标子图像
//    LCD_Show_Target_Subimages(0,200,SUBIMAGE_HEIGHT,SUBIMAGE_WIDTH, *subImages);    //显示子图像
//    Split_An_Image_Into_Target_Subimages(subImages, 46, y_skew+1);                  //分割目标子图像
//    LCD_Show_Target_Subimages(0,230,SUBIMAGE_HEIGHT,SUBIMAGE_WIDTH, *subImages);    //显示子图像
//    Split_An_Image_Into_Target_Subimages(subImages, 72, y_skew+2);                  //分割目标子图像
//    LCD_Show_Target_Subimages(0,260,SUBIMAGE_HEIGHT,SUBIMAGE_WIDTH, *subImages);    //显示子图像
//    Split_An_Image_Into_Target_Subimages(subImages, 98, y_skew+4);                  //分割目标子图像
//    LCD_Show_Target_Subimages(0,290,SUBIMAGE_HEIGHT,SUBIMAGE_WIDTH, *subImages);    //显示子图像
//    Split_An_Image_Into_Target_Subimages(subImages, 125, y_skew+5);                 //分割目标子图像
//    LCD_Show_Target_Subimages(0,320,SUBIMAGE_HEIGHT,SUBIMAGE_WIDTH, *subImages);    //显示子图像
//    Split_An_Image_Into_Target_Subimages(subImages, 152, y_skew+6);                 //分割目标子图像
//    LCD_Show_Target_Subimages(0,350,SUBIMAGE_HEIGHT,SUBIMAGE_WIDTH, *subImages);    //显示子图像




//            resize_image(subImages, resized_subImage, SUBIMAGE_WIDTH, SUBIMAGE_HEIGHT, STANDARD_IMAGE_WIDTH, STANDARD_IMAGE_HEIGHT);    //缩放图片

//            recognition_result = Digital_Recognition(subImages);            //方差判断数字 子图像
//            recognition_result = Digital_Recognition(resized_subImage);     //方差判断数字 缩放后的图像
//            if(recognition_result == 7)
//                LOG_I("recognition_result = %d", recognition_result);


//    rt_free(resized_subImage);  //释放缩放图像内存
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

    rt_uint8_t (*subImages)[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH] =
            (rt_uint8_t (*)[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH])
            rt_malloc(SUBIMAGE_HEIGHT * SUBIMAGE_WIDTH * sizeof(rt_uint8_t)); //子图像动态内存
    if (subImages == NULL){    //判断子图像内存是否成功申请
        LOG_E("Failed to allocate memory for subImages");return;}

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
        rt_uint8_t inputImage_row    = (rt_uint8_t)atoi((const char *)argv[1]);        //相当于子图像x轴位置
        rt_uint8_t inputImage_column = (rt_uint8_t)atoi((const char *)argv[2]);     //相当于子图像y轴位置

        Gray_Scale(2, string);
        Split_An_Image_Into_Subimages(subImages, inputImage_row, inputImage_column);
    }

    rt_device_write(camera_device_t.uart, 0, *subImages, sizeof(*subImages));        //输出子图像
    rt_free(subImages);     //释放子图像内存
}

void Resizing_Image(int argc, rt_uint8_t *argv[])	//缩放子图像命令
{
    const char *gray = "Gray_Scale";
    const char *no = "No_output";
    rt_uint8_t *string[2] = {(rt_uint8_t *)gray, (rt_uint8_t *)no};

    rt_uint8_t (*subImages)[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH] =
            (rt_uint8_t (*)[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH])
            rt_malloc(SUBIMAGE_HEIGHT * SUBIMAGE_WIDTH * sizeof(rt_uint8_t)); //子图像动态内存
    if (subImages == NULL){    //判断子图像内存是否成功申请
        LOG_E("Failed to allocate memory for subImages");return;}

    rt_uint8_t (*resized_subImage)[STANDARD_IMAGE_HEIGHT][STANDARD_IMAGE_WIDTH] =
            (rt_uint8_t (*)[STANDARD_IMAGE_HEIGHT][STANDARD_IMAGE_WIDTH])
            rt_malloc(STANDARD_IMAGE_HEIGHT * STANDARD_IMAGE_WIDTH * sizeof(rt_uint8_t)); //缩放后的图像动态内存
    if (resized_subImage == NULL){    //判断缩放图内存是否成功申请
        LOG_E("Failed to allocate memory for resized_subImage");return;}

    if(argc != 3)
    {
        Gray_Scale(2, string);                              //灰度调整命令
//        Split_An_Image_Into_Subimages(subImages, 2, 18);    //分割子图像
        Split_An_Image_Into_Subimages(subImages, 0, 0);     //分割子图像
        resize_image(subImages, resized_subImage, SUBIMAGE_WIDTH, SUBIMAGE_HEIGHT, STANDARD_IMAGE_WIDTH, STANDARD_IMAGE_HEIGHT);    //缩放图片
        rt_device_write(camera_device_t.uart, 0, *resized_subImage, sizeof(*resized_subImage));                                     //输出缩放图
    }
    else if(argc == 3)
    {
        rt_uint8_t inputImage_row    = (rt_uint8_t)atoi((const char *)argv[1]);         //x轴位置
        rt_uint8_t inputImage_column = (rt_uint8_t)atoi((const char *)argv[2]);         //y轴位置

        Gray_Scale(2, string);                                                          //灰度调整命令
        Split_An_Image_Into_Subimages(subImages, inputImage_row, inputImage_column);    //分割子图像
        resize_image(subImages, resized_subImage, SUBIMAGE_WIDTH, SUBIMAGE_HEIGHT, STANDARD_IMAGE_WIDTH, STANDARD_IMAGE_HEIGHT);    //缩放图片
        rt_device_write(camera_device_t.uart, 0, *resized_subImage, sizeof(*resized_subImage));                                     //输出缩放图
    }

    rt_free(resized_subImage);  //释放缩放图像内存
    rt_free(subImages);         //释放子图像内存
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
MSH_CMD_EXPORT(Resizing_Image, Resizing the image :<Resizing_Image <x> <y>>);
MSH_CMD_EXPORT(Sub_Images, Output subimage);
