#ifndef APPLICATIONS_I2C_BARE_H_
#define APPLICATIONS_I2C_BARE_H_

#include "main.h"

#ifdef USE_BARE

#include "pin_config.h"

/*摄像头寄存器地址*/
#define OV2640_DSP_RA_DLMT 0xFF         // DSP 地址
#define OV2640_SENSOR_MIDH 0x1C         //传感器 ID 高位
#define OV2640_SENSOR_MIDL 0x1D         //传感器 ID 低位
#define OV2640_SENSOR_PIDH 0x0A         //芯片的版本信息高位
#define OV2640_SENSOR_PIDL 0x0B         //芯片的版本信息低位

#define OV2640_BUS_ADDR 0x42            //总线读地址

#define CAMERA_I2C_BUS_NAME      "i2c1" /* 传感器连接的I2C总线设备名称 */

//存储摄像头ID的结构体
typedef struct
{
    uint8_t Manufacturer_ID1;           //制造商id1
    uint8_t Manufacturer_ID2;           //制造商id2
    uint8_t PIDH;                       //
    uint8_t PIDL;                       //
} OV2640_IDTypeDef;

typedef struct
{
    rt_mutex_t lock;                    //互斥量
    struct rt_i2c_bus_device *i2c;      //挂载的总线
}Camera_Structure;


void I2C_Reponse(void);

#endif  /*USE_BARE*/

#endif /* APPLICATIONS_I2C_BARE_H_ */
