#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <stdio.h>
#include <string.h>
#include "onenet.h"
#include "bc28.h"

#define DBG_TAG "BC28"
#define DBG_LVL DBG_INFO
#include <rtdbg.h>

extern int result[6];

//static rt_event_t recvdata_event;                   /* 事件集 */
static rt_sem_t mqttinit_sem;                       /* 信号量 */
//static rt_mailbox_t tmp_msg_mb;                     /* 邮箱 */
//static rt_mp_t tmp_msg_mp;                          /* 内存池 */

static rt_thread_t onenet_upload_data_thread;       /* onenet上传数据的线程的句柄 */
static rt_thread_t onenet_mqtt_init_thread;         /* mqtt初始化的线程的句柄 */

static void onenet_mqtt_init_entry(void *parameter)
{
    uint8_t onenet_mqtt_init_failed_times;

//    rt_thread_mdelay(30000);

    /* mqtt初始化 */
    while (1)
    {
        if (!onenet_mqtt_init())
        {
            /* mqtt初始化成功之后，释放信号量告知onenet_upload_data_thread线程可以上传数据了 */
            rt_sem_release(mqttinit_sem);
            return;
        }
        rt_thread_mdelay(100);
        LOG_E("onenet mqtt init failed %d times", onenet_mqtt_init_failed_times++);
    }
}

static void onenet_upload_data_entry(void *parameter)
{
    int data = 0;

    /* 永久等待方式接收信号量，若收不到，该线程会一直挂起 */
    rt_sem_take(mqttinit_sem, RT_WAITING_FOREVER);
    /* 后面用不到这个信号量了，把它删除了，回收资源 */
    rt_sem_delete(mqttinit_sem);

    while (1)
    {

        data = result[0]*100000+result[1]*10000+result[2]*1000+result[3]*100+result[4]*10;
        /* 60s上传一次数据 */
//        rt_thread_mdelay(600);
        rt_thread_mdelay(30000);

        /* 上传发送节点1的数据到OneNet服务器，数据流名字是temperature_p0 */
        if (onenet_mqtt_upload_digit("Data_stream", data) != RT_EOK)
        {
            rt_kprintf("upload Data_stream has an error, try again\n");
        }

    }
}

void BC28_Reponse(void)
{

    mqttinit_sem = rt_sem_create("mqtt_sem", RT_NULL, RT_IPC_FLAG_FIFO);
    if(mqttinit_sem     == RT_NULL)
        LOG_E("Description Failed to create the mqtt semaphore");

    onenet_mqtt_init_thread = rt_thread_create("mqttinit", onenet_mqtt_init_entry, RT_NULL, 1024, 10, 10);
    if (onenet_mqtt_init_thread != RT_NULL){
        rt_thread_startup(onenet_mqtt_init_thread);
        LOG_I("BC28初始化函数开始调用");
    }

    onenet_upload_data_thread = rt_thread_create("uploaddata", onenet_upload_data_entry, RT_NULL, 1024, 10, 20);
    if (onenet_upload_data_thread != RT_NULL){
        rt_thread_startup(onenet_upload_data_thread);
        LOG_I("BC28初始化成功");
    }
}
