#ifndef APPLICATIONS_GC0308_H_
#define APPLICATIONS_GC0308_H_

#include "stm32u5xx_hal.h"

#define CONFIG_GC_SENSOR_SUBSAMPLE_MODE 1   //子采样模式启用   否则就是窗口模式
/*
 * 假设大小320*240=76800单位是2字节(16bit) 所需内存为38400(rt_uint32_t)
 * 串口输出1字节(8bit)
 * */
#define pictureBufferLength 9600
#define Binary_pictureBufferLength 19200

/*
 * P0 (0xfe) 相机模块的复位与寄存器页面选择寄存器
 * Bit[7] (soft_reset): 软复位位 将此位设置为 1 以触发模块的软复位 复位完成后 此位将自动清零
 * Bit[6:5] (NA): 保留位 无功能
 * Bit[4] (CISCTL_restart_n): 当此位设置为 0 时 将重新启动 CISCTL 此位在低电平时有效
 * Bit[3:1] (NA): 保留位 无功能
 * Bit[0] (page_select): 寄存器页面选择位 通过设置此位 可以在两个寄存器页面之间切换
 *  * 1'b0: 选择 REGF0 页面 其中包含经常使用的寄存器
 *  * 1'b1: 选择 REGF1 页面 其中包含其他寄存器
 * */
#define RESET_RELATED 0xfe      //相机模块的复位与寄存器页面选择寄存器
#define GC0308_PID 0x9b         //GC0308 PID
#define GC0308_SCCB_ADDR 0x42   //GC0308设备总线地址

#define H8(v) ((v)>>8)
#define L8(v) ((v)&0xff)

typedef enum {
    FRAMESIZE_96X96,            // 96x96
    FRAMESIZE_160x120_QQVGA,    // 160x120
    FRAMESIZE_176x144_QCIF,     // 176x144
    FRAMESIZE_240x176_HQVGA,    // 240x176
    FRAMESIZE_240x240_240X240,  // 240x240
    FRAMESIZE_320x240_QVGA,     // 320x240
    FRAMESIZE_400x296_CIF,      // 400x296
    FRAMESIZE_480x320_HVGA,     // 480x320
    FRAMESIZE_640x480_VGA,      // 640x480
    FRAMESIZE_INVALID
} framesize_t;

typedef enum {
    ASPECT_RATIO_4X3,
    ASPECT_RATIO_3X2,
    ASPECT_RATIO_16X10,
    ASPECT_RATIO_5X3,
    ASPECT_RATIO_16X9,
    ASPECT_RATIO_21X9,
    ASPECT_RATIO_5X4,
    ASPECT_RATIO_1X1,
    ASPECT_RATIO_9X16
} aspect_ratio_t;

typedef enum {
    PIXFORMAT_RGB565,    // 2BPP/RGB565
    PIXFORMAT_YUV422,    // 2BPP/YUV422
} pixformat_t;

typedef struct {
        const rt_uint16_t width;            //宽
        const rt_uint16_t height;           //高
        const aspect_ratio_t aspect_ratio;  //长宽比
} resolution_info_t;

void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi);     //帧事件中断 回调函数
void GC0308_Reponse(void);  //任务调用

#endif /* APPLICATIONS_GC0308_H_ */
