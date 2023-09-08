#include "lcd.h"
#include "stdlib.h"
#include "lcdfont.h"

#define DBG_TAG "lcd"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>


static rt_thread_t LCD_thread;                              //LCD的线程的句柄
extern rt_uint16_t JpegBuffer[INPUT_HEIGHT][INPUT_WIDTH];   //原始图片缓冲
extern rt_sem_t shot_sem;                                     //拍照完成信号量

//管理LCD重要参数
//默认为竖屏
_lcd_dev lcddev;

//写寄存器函数
//regval:寄存器值
void LCD_WR_REG(rt_uint16_t reg)
{
#if LCD_USE8BIT_MODEL==1//使用8位并行数据总线模式
    LCD_RS_CLR;
    LCD_CS_CLR;
    DATAOUT(reg);
    LCD_WR_CLR;
    LCD_WR_SET;
    LCD_CS_SET;
    LCD_RS_SET;
#else   //使用16位并行数据总线模式
    LCD_RS_CLR;
    LCD_CS_CLR;
    DATAOUT(reg);
    LCD_WR_CLR;
    LCD_WR_SET;
    LCD_RS_SET;
    LCD_CS_SET;
#endif
}
//写LCD数据
//data:要写入的值
void LCD_WR_data(rt_uint16_t data)
{
#if LCD_USE8BIT_MODEL==1//使用8位并行数据总线模式
    LCD_CS_CLR;
    DATAOUT(data);
    LCD_WR_CLR;
    LCD_WR_SET;
    LCD_CS_SET;
#else  //使用16位并行数据总线模式
    LCD_CS_CLR;
    DATAOUT(data);
    LCD_WR_CLR;
    LCD_WR_SET;
    LCD_CS_SET;
#endif
}

//写LCD数据
//data:要写入的值
void LCD_WR_DATA(rt_uint16_t data)
{
#if LCD_USE8BIT_MODEL==1//使用8位并行数据总线模式
    LCD_CS_CLR;
    DATAOUT(data>>8);
    LCD_WR_CLR;
    LCD_WR_SET;
    DATAOUT(data);
    LCD_WR_CLR;
    LCD_WR_SET;
    LCD_CS_SET;
#else   //使用16位并行数据总线模式
    LCD_CS_CLR;
    DATAOUT(data);
    LCD_WR_CLR;
    LCD_WR_SET;
    LCD_CS_SET;
#endif
}
//读LCD数据
//返回值:读到的值
rt_uint16_t LCD_RD_DATA(void)
{
    rt_uint16_t data=0;
    LCD_DATA_IN();
#if LCD_USE8BIT_MODEL==1//使用8位并行数据总线模式
    rt_uint16_t data0=0;
    LCD_CS_CLR;
    LCD_RD_CLR;
    data=DATAIN;
    LCD_RD_SET;
    LCD_RD_CLR;
    data0=DATAIN;
    LCD_RD_SET;
    LCD_CS_SET;
    data&=0xff;
    data=data0|data<<8;
#else //使用16位并行数据总线模式
    LCD_CS_CLR;
    LCD_RD_CLR;
    data=DATAIN;
    LCD_RD_SET;
    LCD_CS_SET;
#endif
    LCD_DATA_OUT();

//    LOG_I("RD_DATA:%x", data);

    return data;
}


/******************************************************************************
      函数说明：设置起始和结束地址
      入口数据：x1,x2 设置列的起始和结束地址
             y1,y2 设置行的起始和结束地址
      返回值：  无
******************************************************************************/
void LCD_Address_Set(rt_uint16_t x1,rt_uint16_t y1,rt_uint16_t x2,rt_uint16_t y2)
{
    LCD_WR_REG(0x2a);//列地址设置
    LCD_WR_data(x1>>8);
    LCD_WR_data(x1&0xff);
    LCD_WR_data(x2>>8);
    LCD_WR_data(x2&0xff);
    LCD_WR_REG(0x2b);//行地址设置
    LCD_WR_data(y1>>8);
    LCD_WR_data(y1&0xff);
    LCD_WR_data(y2>>8);
    LCD_WR_data(y2&0xff);
    LCD_WR_REG(0x2c);//储存器写
}

/******************************************************************************
      函数说明：设置光标位置
      入口数据：x,y 光标位置
      返回值：  无
******************************************************************************/
void LCD_SetCursor(rt_uint16_t x,rt_uint16_t y)
{
    LCD_WR_REG(0x2a);//列地址设置
    LCD_WR_data(x>>8);
    LCD_WR_data(x&0xff);
    LCD_WR_REG(0x2b);//行地址设置
    LCD_WR_data(y>>8);
    LCD_WR_data(y&0xff);
}

//LCD开启显示
void LCD_DisplayOn(void)
{
    LCD_WR_REG(0X29);   //开启显示
}
//LCD关闭显示
void LCD_DisplayOff(void)
{
    LCD_WR_REG(0X28);   //关闭显示
}


//读取个某点的颜色值
//x,y:坐标
//返回值:此点的颜色
rt_uint16_t LCD_ReadPoint(rt_uint16_t x,rt_uint16_t y)
{
    rt_uint16_t rgb;
    LCD_SetCursor(x,y);
    LCD_WR_REG(0X2E);
#if LCD_USE8BIT_MODEL==1//使用8位并行数据总线模式
    LCD_DATA_IN();
    LCD_RS_SET;
    LCD_CS_CLR;
    LCD_RD_CLR;
    rgb=DATAIN;
    LCD_RD_SET;
    LCD_RD_CLR;
#else //使用16位并行数据总线模式
    rgb=LCD_RD_DATA();//dummy read
#endif
    rgb=LCD_RD_DATA();//实际坐标颜色
    return rgb;
}


void LCD_Clear(rt_uint16_t color)
{
    rt_uint16_t i,j;
    LCD_Address_Set(0,0,lcddev.width-1,lcddev.height-1);//设置显示范围
    for(i=0;i<lcddev.width;i++)
    {
        for(j=0;j<lcddev.height;j++)
        {
            LCD_WR_DATA(color);
        }
    }
}

/******************************************************************************
      函数说明：在指定区域填充颜色
      入口数据：xsta,ysta   起始坐标
                xend,yend   终止坐标
                                color       要填充的颜色
      返回值：  无
******************************************************************************/
void LCD_Fill(rt_uint16_t xsta,rt_uint16_t ysta,rt_uint16_t xend,rt_uint16_t yend,rt_uint16_t color)
{
    rt_uint16_t i,j;
    LCD_Address_Set(xsta,ysta,xend-1,yend-1);//设置显示范围
    for(i=ysta;i<yend;i++)
    {
        for(j=xsta;j<xend;j++)
        {
            LCD_WR_DATA(color);
        }
    }
}

/******************************************************************************
      函数说明：在指定位置画点
      入口数据：x,y 画点坐标
                color 点的颜色
      返回值：  无
******************************************************************************/
void LCD_DrawPoint(rt_uint16_t x,rt_uint16_t y,rt_uint16_t color)
{
    LCD_Address_Set(x,y,x,y);//设置光标位置
    LCD_WR_DATA(color);
}


/******************************************************************************
      函数说明：画线
      入口数据：x1,y1   起始坐标
                x2,y2   终止坐标
                color   线的颜色
      返回值：  无
******************************************************************************/
void LCD_DrawLine(rt_uint16_t x1,rt_uint16_t y1,rt_uint16_t x2,rt_uint16_t y2,rt_uint16_t color)
{
    rt_uint16_t t;
    int xerr=0,yerr=0,delta_x,delta_y,distance;
    int incx,incy,uRow,uCol;
    delta_x=x2-x1; //计算坐标增量
    delta_y=y2-y1;
    uRow=x1;//画线起点坐标
    uCol=y1;
    if(delta_x>0)incx=1; //设置单步方向
    else if (delta_x==0)incx=0;//垂直线
    else {incx=-1;delta_x=-delta_x;}
    if(delta_y>0)incy=1;
    else if (delta_y==0)incy=0;//水平线
    else {incy=-1;delta_y=-delta_y;}
    if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴
    else distance=delta_y;
    for(t=0;t<distance+1;t++)
    {
        LCD_DrawPoint(uRow,uCol,color);//画点
        xerr+=delta_x;
        yerr+=delta_y;
        if(xerr>distance)
        {
            xerr-=distance;
            uRow+=incx;
        }
        if(yerr>distance)
        {
            yerr-=distance;
            uCol+=incy;
        }
    }
}


/******************************************************************************
      函数说明：画矩形
      入口数据：x1,y1   起始坐标
                x2,y2   终止坐标
                color   矩形的颜色
      返回值：  无
******************************************************************************/
void LCD_DrawRectangle(rt_uint16_t x1, rt_uint16_t y1, rt_uint16_t x2, rt_uint16_t y2,rt_uint16_t color)
{
    LCD_DrawLine(x1,y1,x2,y1,color);
    LCD_DrawLine(x1,y1,x1,y2,color);
    LCD_DrawLine(x1,y2,x2,y2,color);
    LCD_DrawLine(x2,y1,x2,y2,color);
}


/******************************************************************************
      函数说明：画圆
      入口数据：x0,y0   圆心坐标
                r       半径
                color   圆的颜色
      返回值：  无
******************************************************************************/
void Draw_Circle(rt_uint16_t x0,rt_uint16_t y0,rt_uint8_t r,rt_uint16_t color)
{
    int a,b;
    a=0;b=r;
    while(a<=b)
    {
        LCD_DrawPoint(x0-b,y0-a,color);             //3
        LCD_DrawPoint(x0+b,y0-a,color);             //0
        LCD_DrawPoint(x0-a,y0+b,color);             //1
        LCD_DrawPoint(x0-a,y0-b,color);             //2
        LCD_DrawPoint(x0+b,y0+a,color);             //4
        LCD_DrawPoint(x0+a,y0-b,color);             //5
        LCD_DrawPoint(x0+a,y0+b,color);             //6
        LCD_DrawPoint(x0-b,y0+a,color);             //7
        a++;
        if((a*a+b*b)>(r*r))//判断要画的点是否过远
        {
            b--;
        }
    }
}

/******************************************************************************
      函数说明：显示汉字串
      入口数据：x,y显示坐标
                *s 要显示的汉字串
                fc 字的颜色
                bc 字的背景色
                sizey 字号 可选 16 24 32
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void LCD_ShowChinese(rt_uint16_t x,rt_uint16_t y,rt_uint8_t *s,rt_uint16_t fc,rt_uint16_t bc,rt_uint8_t sizey,rt_uint8_t mode)
{
    while(*s!=0)
    {
        if(sizey==12) LCD_ShowChinese12x12(x,y,s,fc,bc,sizey,mode);
        else if(sizey==16) LCD_ShowChinese16x16(x,y,s,fc,bc,sizey,mode);
        else if(sizey==24) LCD_ShowChinese24x24(x,y,s,fc,bc,sizey,mode);
        else if(sizey==32) LCD_ShowChinese32x32(x,y,s,fc,bc,sizey,mode);
        else return;
        s+=2;
        x+=sizey;
    }
}

/******************************************************************************
      函数说明：显示单个12x12汉字
      入口数据：x,y显示坐标
                *s 要显示的汉字
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void LCD_ShowChinese12x12(rt_uint16_t x,rt_uint16_t y,rt_uint8_t *s,rt_uint16_t fc,rt_uint16_t bc,rt_uint8_t sizey,rt_uint8_t mode)
{
    rt_uint8_t i,j,m=0;
    rt_uint16_t k;
    rt_uint16_t HZnum;//汉字数目
    rt_uint16_t TypefaceNum;//一个字符所占字节大小
    rt_uint16_t x0=x;
    TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;

    HZnum=sizeof(tfont12)/sizeof(typFNT_GB12);  //统计汉字数目
    for(k=0;k<HZnum;k++)
    {
        if((tfont12[k].Index[0]==*(s))&&(tfont12[k].Index[1]==*(s+1)))
        {
            LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
            for(i=0;i<TypefaceNum;i++)
            {
                for(j=0;j<8;j++)
                {
                    if(!mode)//非叠加方式
                    {
                        if(tfont12[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
                        else LCD_WR_DATA(bc);
                        m++;
                        if(m%sizey==0)
                        {
                            m=0;
                            break;
                        }
                    }
                    else//叠加方式
                    {
                        if(tfont12[k].Msk[i]&(0x01<<j)) LCD_DrawPoint(x,y,fc);//画一个点
                        x++;
                        if((x-x0)==sizey)
                        {
                            x=x0;
                            y++;
                            break;
                        }
                    }
                }
            }
        }
        continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
    }
}

/******************************************************************************
      函数说明：显示单个16x16汉字
      入口数据：x,y显示坐标
                *s 要显示的汉字
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void LCD_ShowChinese16x16(rt_uint16_t x,rt_uint16_t y,rt_uint8_t *s,rt_uint16_t fc,rt_uint16_t bc,rt_uint8_t sizey,rt_uint8_t mode)
{
    rt_uint8_t i,j,m=0;
    rt_uint16_t k;
    rt_uint16_t HZnum;//汉字数目
    rt_uint16_t TypefaceNum;//一个字符所占字节大小
    rt_uint16_t x0=x;
  TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
    HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);  //统计汉字数目
    for(k=0;k<HZnum;k++)
    {
        if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
        {
            LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
            for(i=0;i<TypefaceNum;i++)
            {
                for(j=0;j<8;j++)
                {
                    if(!mode)//非叠加方式
                    {
                        if(tfont16[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
                        else LCD_WR_DATA(bc);
                        m++;
                        if(m%sizey==0)
                        {
                            m=0;
                            break;
                        }
                    }
                    else//叠加方式
                    {
                        if(tfont16[k].Msk[i]&(0x01<<j)) LCD_DrawPoint(x,y,fc);//画一个点
                        x++;
                        if((x-x0)==sizey)
                        {
                            x=x0;
                            y++;
                            break;
                        }
                    }
                }
            }
        }
        continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
    }
}


/******************************************************************************
      函数说明：显示单个24x24汉字
      入口数据：x,y显示坐标
                *s 要显示的汉字
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void LCD_ShowChinese24x24(rt_uint16_t x,rt_uint16_t y,rt_uint8_t *s,rt_uint16_t fc,rt_uint16_t bc,rt_uint8_t sizey,rt_uint8_t mode)
{
    rt_uint8_t i,j,m=0;
    rt_uint16_t k;
    rt_uint16_t HZnum;//汉字数目
    rt_uint16_t TypefaceNum;//一个字符所占字节大小
    rt_uint16_t x0=x;
    TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
    HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);  //统计汉字数目
    for(k=0;k<HZnum;k++)
    {
        if ((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1)))
        {
            LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
            for(i=0;i<TypefaceNum;i++)
            {
                for(j=0;j<8;j++)
                {
                    if(!mode)//非叠加方式
                    {
                        if(tfont24[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
                        else LCD_WR_DATA(bc);
                        m++;
                        if(m%sizey==0)
                        {
                            m=0;
                            break;
                        }
                    }
                    else//叠加方式
                    {
                        if(tfont24[k].Msk[i]&(0x01<<j)) LCD_DrawPoint(x,y,fc);//画一个点
                        x++;
                        if((x-x0)==sizey)
                        {
                            x=x0;
                            y++;
                            break;
                        }
                    }
                }
            }
        }
        continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
    }
}

/******************************************************************************
      函数说明：显示单个32x32汉字
      入口数据：x,y显示坐标
                *s 要显示的汉字
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void LCD_ShowChinese32x32(rt_uint16_t x,rt_uint16_t y,rt_uint8_t *s,rt_uint16_t fc,rt_uint16_t bc,rt_uint8_t sizey,rt_uint8_t mode)
{
    rt_uint8_t i,j,m=0;
    rt_uint16_t k;
    rt_uint16_t HZnum;//汉字数目
    rt_uint16_t TypefaceNum;//一个字符所占字节大小
    rt_uint16_t x0=x;
    TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
    HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);  //统计汉字数目
    for(k=0;k<HZnum;k++)
    {
        if ((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1)))
        {
            LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
            for(i=0;i<TypefaceNum;i++)
            {
                for(j=0;j<8;j++)
                {
                    if(!mode)//非叠加方式
                    {
                        if(tfont32[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
                        else LCD_WR_DATA(bc);
                        m++;
                        if(m%sizey==0)
                        {
                            m=0;
                            break;
                        }
                    }
                    else//叠加方式
                    {
                        if(tfont32[k].Msk[i]&(0x01<<j)) LCD_DrawPoint(x,y,fc);//画一个点
                        x++;
                        if((x-x0)==sizey)
                        {
                            x=x0;
                            y++;
                            break;
                        }
                    }
                }
            }
        }
        continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
    }
}

/******************************************************************************
      函数说明：显示单个字符
      入口数据：x,y显示坐标
                num 要显示的字符
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void LCD_ShowChar(rt_uint16_t x,rt_uint16_t y,rt_uint8_t num,rt_uint16_t fc,rt_uint16_t bc,rt_uint8_t sizey,rt_uint8_t mode)
{
    rt_uint8_t temp,sizex,t,m=0;
    rt_uint16_t i,TypefaceNum;//一个字符所占字节大小
    rt_uint16_t x0=x;
    sizex=sizey/2;
    TypefaceNum=(sizex/8+((sizex%8)?1:0))*sizey;
    num=num-' ';    //得到偏移后的值
    LCD_Address_Set(x,y,x+sizex-1,y+sizey-1);  //设置光标位置
    for(i=0;i<TypefaceNum;i++)
    {
        if(sizey==12)temp=ascii_1206[num][i];              //调用6x12字体
        else if(sizey==16)temp=ascii_1608[num][i];       //调用8x16字体
        else if(sizey==24)temp=ascii_2412[num][i];       //调用12x24字体
        else if(sizey==32)temp=ascii_3216[num][i];       //调用16x32字体
        else return;
        for(t=0;t<8;t++)
        {
            if(!mode)//非叠加模式
            {
                if(temp&(0x01<<t))LCD_WR_DATA(fc);
                else LCD_WR_DATA(bc);
                m++;
                if(m%sizex==0)
                {
                    m=0;
                    break;
                }
            }
            else//叠加模式
            {
                if(temp&(0x01<<t))LCD_DrawPoint(x,y,fc);//画一个点
                x++;
                if((x-x0)==sizex)
                {
                    x=x0;
                    y++;
                    break;
                }
            }
        }
    }
}


/******************************************************************************
      函数说明：显示字符串
      入口数据：x,y显示坐标
                *p 要显示的字符串
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
******************************************************************************/
void LCD_ShowString(rt_uint16_t x,rt_uint16_t y,const rt_uint8_t *p,rt_uint16_t fc,rt_uint16_t bc,rt_uint8_t sizey,rt_uint8_t mode)
{
    while(*p!='\0')
    {
        LCD_ShowChar(x,y,*p,fc,bc,sizey,mode);
        x+=sizey/2;
        p++;
    }
}


/******************************************************************************
      函数说明：显示数字
      入口数据：m底数，n指数
      返回值：  无
******************************************************************************/
rt_uint32_t mypow(rt_uint8_t m,rt_uint8_t n)
{
    rt_uint32_t result=1;
    while(n--)result*=m;
    return result;
}


/******************************************************************************
      函数说明：显示整数变量
      入口数据：x,y显示坐标
                num 要显示整数变量
                len 要显示的位数
                fc 字的颜色
                bc 字的背景色
                sizey 字号
      返回值：  无
******************************************************************************/
void LCD_ShowIntNum(rt_uint16_t x,rt_uint16_t y,rt_uint16_t num,rt_uint8_t len,rt_uint16_t fc,rt_uint16_t bc,rt_uint8_t sizey)
{
    rt_uint8_t t,temp;
    rt_uint8_t enshow=0;
    rt_uint8_t sizex=sizey/2;
    for(t=0;t<len;t++)
    {
        temp=(num/mypow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1))
        {
            if(temp==0)
            {
                LCD_ShowChar(x+t*sizex,y,' ',fc,bc,sizey,0);
                continue;
            }else enshow=1;

        }
        LCD_ShowChar(x+t*sizex,y,temp+48,fc,bc,sizey,0);
    }
}


/******************************************************************************
      函数说明：显示两位小数变量
      入口数据：x,y显示坐标
                num 要显示小数变量
                len 要显示的位数
                fc 字的颜色
                bc 字的背景色
                sizey 字号
      返回值：  无
******************************************************************************/
void LCD_ShowFloatNum1(rt_uint16_t x,rt_uint16_t y,float num,rt_uint8_t len,rt_uint16_t fc,rt_uint16_t bc,rt_uint8_t sizey)
{
    rt_uint8_t t,temp,sizex;
    rt_uint16_t num1;
    sizex=sizey/2;
    num1=num*100;
    for(t=0;t<len;t++)
    {
        temp=(num1/mypow(10,len-t-1))%10;
        if(t==(len-2))
        {
            LCD_ShowChar(x+(len-2)*sizex,y,'.',fc,bc,sizey,0);
            t++;
            len+=1;
        }
        LCD_ShowChar(x+t*sizex,y,temp+48,fc,bc,sizey,0);
    }
}


/******************************************************************************
      函数说明：显示图片
      入口数据：x,y起点坐标
                length 图片长度
                width  图片宽度
                pic[]  图片数组
      返回值：  无
******************************************************************************/
void LCD_ShowPicture(rt_uint16_t x,rt_uint16_t y,rt_uint16_t length,rt_uint16_t width,const rt_uint16_t pic[][INPUT_WIDTH])
{
    rt_uint16_t pic_val;
    int i,j;
    LCD_Address_Set(x,y,x+length-1,y+width-1);

    for(j=width;j>-1;j--)   //镜像翻转
    {
        for(i=0;i<length;i++)
        {
            pic_val = (pic[i][j] >> 8) | (pic[i][j] << 8);  //串口处理后的数据高位与低位位置会反过来
            LCD_WR_DATA(pic_val);
        }
    }
}

/******************************************************************************
      函数说明：显示目标子图片
      入口数据：x,y起点坐标
                length 图片长度
                width  图片宽度
                pic[]  图片数组
      返回值：  无
******************************************************************************/
void LCD_Show_Target_Subimages(rt_uint16_t x,rt_uint16_t y,rt_uint16_t length,rt_uint16_t width,const rt_uint16_t pic[SUBIMAGE_HEIGHT][SUBIMAGE_WIDTH])
{
    rt_uint16_t pic_val;
    int i,j;
    LCD_Address_Set(x,y,x+length-1,y+width-1);

//    for(j=0;j<width;j++)
    for(j=width;j>-1;j--)   //镜像翻转
    {
        for(i=0;i<length;i++)
        {
            pic_val = (pic[i][j] >> 8) | (pic[i][j] << 8);  //串口处理后的数据高位与低位位置会反过来
            LCD_WR_DATA(pic_val);
        }
    }
}

void Set_Dir(rt_uint8_t dir)
{
    if(USE_HORIZONTAL==0||USE_HORIZONTAL==1)
    {
        lcddev.width=320;
        lcddev.height=480;
        lcddev.dir=0;
    }else
    {
        lcddev.width=480;
        lcddev.height=320;
        lcddev.dir=1;
    }
}



//初始化lcd
void LCD_Init(void)
{
    rt_pin_mode(DB0 , PIN_MODE_OUTPUT);
    rt_pin_mode(DB1 , PIN_MODE_OUTPUT);
    rt_pin_mode(DB2 , PIN_MODE_OUTPUT);
    rt_pin_mode(DB3 , PIN_MODE_OUTPUT);
    rt_pin_mode(DB4 , PIN_MODE_OUTPUT);
    rt_pin_mode(DB5 , PIN_MODE_OUTPUT);
    rt_pin_mode(DB6 , PIN_MODE_OUTPUT);
    rt_pin_mode(DB7 , PIN_MODE_OUTPUT);
    rt_pin_mode(DB8 , PIN_MODE_OUTPUT);
    rt_pin_mode(DB9 , PIN_MODE_OUTPUT);
    rt_pin_mode(DB10, PIN_MODE_OUTPUT);
    rt_pin_mode(DB11, PIN_MODE_OUTPUT);
    rt_pin_mode(DB12, PIN_MODE_OUTPUT);
    rt_pin_mode(DB13, PIN_MODE_OUTPUT);
    rt_pin_mode(DB14, PIN_MODE_OUTPUT);
    rt_pin_mode(DB15, PIN_MODE_OUTPUT);

    rt_pin_mode(LCD_RD , PIN_MODE_OUTPUT);
    rt_pin_mode(LCD_WR , PIN_MODE_OUTPUT);
    rt_pin_mode(LCD_RS , PIN_MODE_OUTPUT);
    rt_pin_mode(LCD_CS , PIN_MODE_OUTPUT);
    rt_pin_mode(LCD_RST, PIN_MODE_OUTPUT);

    LCD_RST_CLR;
    rt_thread_mdelay(200);                  // delay 50 ms
    LCD_RST_SET;

    rt_thread_mdelay(50);                   // delay 50 ms
    Set_Dir(DFT_SCAN_DIR);

    LCD_WR_REG(0XD3);
#if LCD_USE8BIT_MODEL==1//使用8位并行数据总线模式
  lcddev.id=LCD_RD_DATA();  //dummy read
    lcddev.id=LCD_RD_DATA();    //读到0X9486
#else   //使用16位并行数据总线模式
    lcddev.id=LCD_RD_DATA();    //dummy read
    lcddev.id=LCD_RD_DATA();    //读到0X00
    lcddev.id=LCD_RD_DATA();    //读取94
    lcddev.id<<=8;
    lcddev.id|=LCD_RD_DATA();   //读取86
#endif

        LCD_WR_REG(0xE0);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x07);
        LCD_WR_DATA(0x0f);
        LCD_WR_DATA(0x0D);
        LCD_WR_DATA(0x1B);
        LCD_WR_DATA(0x0A);
        LCD_WR_DATA(0x3c);
        LCD_WR_DATA(0x78);
        LCD_WR_DATA(0x4A);
        LCD_WR_DATA(0x07);
        LCD_WR_DATA(0x0E);
        LCD_WR_DATA(0x09);
        LCD_WR_DATA(0x1B);
        LCD_WR_DATA(0x1e);
        LCD_WR_DATA(0x0f);

        LCD_WR_REG(0xE1);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x22);
        LCD_WR_DATA(0x24);
        LCD_WR_DATA(0x06);
        LCD_WR_DATA(0x12);
        LCD_WR_DATA(0x07);
        LCD_WR_DATA(0x36);
        LCD_WR_DATA(0x47);
        LCD_WR_DATA(0x47);
        LCD_WR_DATA(0x06);
        LCD_WR_DATA(0x0a);
        LCD_WR_DATA(0x07);
        LCD_WR_DATA(0x30);
        LCD_WR_DATA(0x37);
        LCD_WR_DATA(0x0f);

        LCD_WR_REG(0xC0);
        LCD_WR_DATA(0x10);
        LCD_WR_DATA(0x10);

        LCD_WR_REG(0xC1);
        LCD_WR_DATA(0x41);

        LCD_WR_REG(0xC5);
        LCD_WR_DATA(0x00);
        LCD_WR_DATA(0x22);
        LCD_WR_DATA(0x80);

        LCD_WR_REG(0x36);    // Memory Access Control
        if(USE_HORIZONTAL==0)LCD_WR_DATA(0x48);
        else if(USE_HORIZONTAL==1)LCD_WR_DATA(0x88);
        else if(USE_HORIZONTAL==2)LCD_WR_DATA(0x28);
        else LCD_WR_DATA(0xE8);


        LCD_WR_REG(0x3A); //Interface Mode Control，
        LCD_WR_DATA(0x55);

        LCD_WR_REG(0XB0);  //Interface Mode Control
        LCD_WR_DATA(0x00);
        LCD_WR_REG(0xB1);   //Frame rate 70HZ
        LCD_WR_DATA(0xB0);
        LCD_WR_DATA(0x11);
        LCD_WR_REG(0xB4);
        LCD_WR_DATA(0x02);
        LCD_WR_REG(0xB6); //RGB/MCU Interface Control
        LCD_WR_DATA(0x02);
        LCD_WR_DATA(0x02);

        LCD_WR_REG(0xB7);
        LCD_WR_DATA(0xC6);
        LCD_WR_REG(0xE9);
        LCD_WR_DATA(0x00);

        LCD_WR_REG(0XF7);
        LCD_WR_DATA(0xA9);
        LCD_WR_DATA(0x51);
        LCD_WR_DATA(0x2C);
        LCD_WR_DATA(0x82);

        LCD_WR_REG(0x11);
        rt_thread_mdelay(120);
        LCD_WR_REG(0x29);

        LOG_I("LCD初始化完成");
}

static void LCD_Reponse_Callback(void *parameter)      //LCD响应回调函数
{
//    MX_FMC_Init();
    LCD_Init();

//    LCD_Fill(0, 0, 320, 480, RED);
    while (1)
    {

        rt_sem_take(shot_sem, RT_WAITING_FOREVER);                      //接收拍照完成信号量 永久等待
        LCD_ShowPicture(0,0,INPUT_HEIGHT,INPUT_WIDTH,JpegBuffer);       //显示拍摄的照片
//        LCD_ShowPicture(0,160,INPUT_HEIGHT,INPUT_WIDTH,JpegBuffer);
        Split_Image_Into_Subimages(0, 0);

        rt_thread_mdelay(500);

    }
}

void LCD_Reponse(void)                      //LCD响应函数
{

    LCD_thread = rt_thread_create("LCD_thread", LCD_Reponse_Callback, RT_NULL, 1024, 10, 20);
    if (LCD_thread != RT_NULL){
        rt_thread_startup(LCD_thread);
//        LOG_I("初始化成功");
    }
}


