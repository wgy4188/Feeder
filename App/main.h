#ifndef __MAIN_H
#define __MAIN_H


/* Private typedef -----------------------------------------------------------*/
#define Enable   0
#define Disable  1
#define Hight    1
#define Low      0
/* Private define ------------------------------------------------------------*/

//输出功能引脚
//motor1  振动
#define   M1_PG1_PWM_Q6_1(X)      (uint8_t)X? (GPIOG->ODR |= 0x02):(GPIOG->ODR &= ~0x02)
#define   M1_PE3_EN_J5_2(X)       (uint8_t)X? (GPIOE->ODR |= 0x08):(GPIOE->ODR &= ~0x08)
#define   M1_PE1_DIR_J5_5(X)      (uint8_t)X? (GPIOE->ODR |= 0x02):(GPIOE->ODR &= ~0x02)
#define   M1_PC4_PLS_J5_3(X)      (uint8_t)X? (GPIOC->ODR |= 0x10):(GPIOC->ODR &= ~0x10)

//motor2  转盘
#define   M2_PE5_PWM_Q9_1(X)      (uint8_t)X? (GPIOE->ODR |= 0x20):(GPIOE->ODR &= ~0x20)
#define   M2_PC2_EN_J7_2(X)       (uint8_t)X? (GPIOC->ODR |= 0x04):(GPIOC->ODR &= ~0x04)
#define   M2_PC3_DIR_J7_5(X)      (uint8_t)X? (GPIOC->ODR |= 0x08):(GPIOC->ODR &= ~0x08)
#define   M2_PC6_PLS_J7_3(X)      (uint8_t)X? (GPIOC->ODR |= 0x40):(GPIOC->ODR &= ~0x40)

//motor3  滚筒
#define   M3_PD0_PWM_Q7_1(X)      (uint8_t)X? (GPIOD->ODR |= 0x01):(GPIOD->ODR &= ~0x01)
#define   M3_PE0_EN_J6_2(X)       (uint8_t)X? (GPIOE->ODR |= 0x01):(GPIOE->ODR &= ~0x01)
#define   M3_PD3_DIR_J6_5(X)      (uint8_t)X? (GPIOD->ODR |= 0x08):(GPIOE->ODR &= ~0x08)
#define   M3_PD2_PLS_J6_3(X)      (uint8_t)X? (GPIOD->ODR |= 0x04):(GPIOD->ODR &= ~0x04)

//Digital tube
#define   MCU_PB2_DO_J11_1(X)     (uint8_t)X? (GPIOB->ODR |= 0x04):(GPIOB->ODR &= ~0x04)
#define   MCU_PB3_CK_J11_2(X)     (uint8_t)X? (GPIOB->ODR |= 0x08):(GPIOB->ODR &= ~0x08)
#define   LED_PD6_XY_J11_6(X)     (uint8_t)X? (GPIOD->ODR |= 0X40):(GPIOD->ODR &= ~0X40)//使能段选

#define   MCU_PB4_A0_J11_3(X)     (uint8_t)X? (GPIOB->ODR |= 0x10):(GPIOB->ODR &= ~0x10)
#define   MCU_PB5_A1_J11_4(X)     (uint8_t)X? (GPIOB->ODR |= 0x20):(GPIOB->ODR &= ~0x20)

//测试灯led
#define   LED_PB7_DO_D6_2(X)      (uint8_t)X? (GPIOB->ODR |= 0X80):(GPIOB->ODR &= ~0X80)//测试

//BEEP
#define   BEEP_PE6_DO_Q3_1(X)     (uint8_t)X? (GPIOE->ODR |= 0X40):(GPIOE->ODR &= ~0X40)

#define   LED_PD5_TST(X)          (uint8_t)X? (GPIOD->ODR |= 0X20):(GPIOD->ODR &= ~0X20)//未用

//输入功能引脚 (key event)
#define   KEY_J11_5_DI_PB6         (GPIOB->IDR&0x40)

//flash key
#define FLASH_RASS_KEY1 ((uint8_t)0x56) /*!< First RASS key */
#define FLASH_RASS_KEY2 ((uint8_t)0xAE) /*!< Second RASS key */

//输入sensor signal
#define  SENSOR_J9_5_DI_PD4        (GPIOD->IDR&0x10)
#define  SENSOR_J9_2_DI_PD7        (GPIOD->IDR&0x80)

//Setting键，菜单状态
typedef struct Menu{

     uint8_t   Mune_Setting_Step;   //设定步骤
     uint8_t   Setting_Flag;        //Setting键值标志
     uint8_t   OK_Flag;             //OK
     uint8_t   DN_Flag;             //DOWN
     uint8_t   UP_Flag;             //UP
     uint16_t  Setting_Key_Delay;   //按键延时
     uint8_t   Setting_Key_Lock;    //Setting功能，进入二级菜单标志位
     uint8_t   System_Reset;        //系统复位
     uint16_t  System_Reset_Delay;  //复位延时
	 uint16_t  System_Set_Delay;

     //菜单键内容
     uint16_t Counter;                  //总计数
     uint8_t  Setting_Counter_Value;      //设定计数值
     uint8_t  Counter_Mode;               //计数方式
     uint8_t  Beep_Mode;                  //蜂鸣器开关
     uint8_t  Vibra_Delay_Setting;        //振动延时设置
     uint8_t  Vibra_amplitude_Setting;    //振动幅度设置  0.5V一个分度（5-10V）
     uint8_t  Roller_Delay_Setting;       //滚筒延时设置
     uint8_t  Roller_amplitude_Setting;   //滚筒幅度设置 0.5V一个分度（5-10V）
     uint8_t  Turnplate_amplitude_Setting;//转盘速度设置
     uint8_t  Vibra_Direction__Setting;   //振动方向设置

}Menu_Setting_Typedef;

extern uint32_t Roll_Motor_Delay;
extern uint8_t  Roll_Motor_Speed;

extern uint32_t Vibra_Motor_Delay;
extern uint8_t  Vibra_Motor_Speed;

extern uint32_t Turnplate_Motor_Delay;
extern uint8_t  Turnplate_Motor_Speed;

extern uint32_t time_count1, time_count2, time_count3;
extern uint8_t  Roll_Puls, Vibra_Puls, Turnplate_Puls;

#endif