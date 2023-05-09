#ifndef APPLICATIONS_GC0308_H_
#define APPLICATIONS_GC0308_H_

#include "stm32u5xx_hal.h"
#include "GC0308_config.h"

#define CONFIG_GC_SENSOR_SUBSAMPLE_MODE 1   //子采样模式启用   否则就是窗口模式

extern const resolution_info_t resolution[FRAMESIZE_INVALID];

/*要改图片大小改这里*/
#define FRAME_SIZE FRAMESIZE_160x120_QQVGA  //图片大小

/*图像宽*/
#define INPUTWIDTH(framesize) (\
    (framesize) == FRAMESIZE_96X96              ? 96  : \
    (framesize) == FRAMESIZE_160x120_QQVGA      ? 160 : \
    (framesize) == FRAMESIZE_176x144_QCIF       ? 176 : \
    (framesize) == FRAMESIZE_240x176_HQVGA      ? 240 : \
    (framesize) == FRAMESIZE_240x240            ? 240 : \
    (framesize) == FRAMESIZE_320x240_QVGA       ? 320 : \
    (framesize) == FRAMESIZE_400x296_CIF        ? 400 : \
    (framesize) == FRAMESIZE_480x320_HVGA       ? 480 : \
    (framesize) == FRAMESIZE_640x480_VGA        ? 640 : 0\
)

/*图像高*/
#define INPUTHEIGHT(framesize) (\
    (framesize) == FRAMESIZE_96X96              ? 96  : \
    (framesize) == FRAMESIZE_160x120_QQVGA      ? 120 : \
    (framesize) == FRAMESIZE_176x144_QCIF       ? 144 : \
    (framesize) == FRAMESIZE_240x176_HQVGA      ? 176 : \
    (framesize) == FRAMESIZE_240x240            ? 240 : \
    (framesize) == FRAMESIZE_320x240_QVGA       ? 240 : \
    (framesize) == FRAMESIZE_400x296_CIF        ? 296 : \
    (framesize) == FRAMESIZE_480x320_HVGA       ? 320 : \
    (framesize) == FRAMESIZE_640x480_VGA        ? 480 : 0\
)

#define INPUT_WIDTH  INPUTWIDTH(FRAME_SIZE)      //图像宽
#define INPUT_HEIGHT INPUTHEIGHT(FRAME_SIZE)     //图像高
/*图片缓冲长度
 * 假设大小320*240=76800单位是2字节(16bit)
 * 所需内存为38400(rt_uint32_t)
 * 串口输出1字节(8bit)*/
#define PICTURE_BUFFER_LENGTH ((INPUT_WIDTH/2) * INPUT_HEIGHT)  //图片缓冲长度

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

void Take_Picture(int argc, rt_uint8_t *argv[]);    //拍照命令 调用自定义MSH命令
void HAL_DCMI_FrameEventCallback(DCMI_HandleTypeDef *hdcmi);     //帧事件中断 回调函数
void GC0308_Reponse(void);  //任务调用

#endif /* APPLICATIONS_GC0308_H_ */
