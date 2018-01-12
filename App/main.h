#ifndef __MAIN_H
#define __MAIN_H


/* Private typedef -----------------------------------------------------------*/
#define Enable   0
#define Disable  1
#define Hight    1
#define Low      0
/* Private define ------------------------------------------------------------*/

//输出功能引脚

//motor3  滚筒
#define   ROLL_POWER(X)      (uint8_t)X? (GPIOD->ODR |= 0x01):(GPIOD->ODR &= ~0x01)//-->PD0
#define   ROLL_FG(X)       	 (uint8_t)X? (GPIOD->ODR |= 0x04):(GPIOD->ODR &= ~0x04)//-->PD2
#define   ROLL_PWM(X)      	 (uint8_t)X? (GPIOD->ODR |= 0x08):(GPIOD->ODR &= ~0x08)//-->PD3
#define   ROLL_DIR(X)      	 (uint8_t)X? (GPIOD->ODR |= 0x10):(GPIOD->ODR &= ~0x10)//-->PD4
//motor1  振动
#define   VIBRA_POWER(X)     (uint8_t)X? (GPIOD->ODR |= 0x20):(GPIOD->ODR &= ~0x20)//PD5
#define   VIBRA_FG(X)        (uint8_t)X? (GPIOD->ODR |= 0x40):(GPIOD->ODR &= ~0x40)//PD6
#define   VIBRA_PWM(X)       (uint8_t)X? (GPIOD->ODR |= 0x80):(GPIOD->ODR &= ~0x80)//PD7
#define   VIBRA_DIR(X)       (uint8_t)X? (GPIOE->ODR |= 0x01):(GPIOE->ODR &= ~0x01)//PE0

//motor2  转盘
#define   TURN_POWER(X)      (uint8_t)X? (GPIOA->ODR |= 0x02):(GPIOA->ODR &= ~0x02)//PE1
#define   TURN_FG(X)         (uint8_t)X? (GPIOE->ODR |= 0x04):(GPIOE->ODR &= ~0x04)//PE2
#define   TURN_PWM(X)        (uint8_t)X? (GPIOE->ODR |= 0x08):(GPIOE->ODR &= ~0x08)//PE3
#define   TURN_DIR(X)        (uint8_t)X? (GPIOE->ODR |= 0x20):(GPIOE->ODR &= ~0x20)//PE5

//Digital tube
#define   TUBE_DATA(X)       (uint8_t)X? (GPIOB->ODR |= 0x01):(GPIOB->ODR &= ~0x01)//PB0
#define   TUBE_CLK(X)        (uint8_t)X? (GPIOB->ODR |= 0x02):(GPIOB->ODR &= ~0x02)//PB1
#define   TUBE_CLR(X)        (uint8_t)X? (GPIOB->ODR |= 0X04):(GPIOB->ODR &= ~0X04)//PB2
#define   TUBE_A0(X)         (uint8_t)X? (GPIOB->ODR |= 0x08):(GPIOB->ODR &= ~0x08)//PB3
#define   TUBE_A1(X)         (uint8_t)X? (GPIOB->ODR |= 0x10):(GPIOB->ODR &= ~0x10)//PB4

//RELAY
#define   RELAY(X)           (uint8_t)X? (GPIOB->ODR |= 0X20):(GPIOB->ODR &= ~0X20)//PB5

//BEEP
#define   BEEP_A(X)            (uint8_t)X? (GPIOB->ODR |= 0X40):(GPIOB->ODR &= ~0X40)//PB6

//led
#define   WORK_LED(X)        (uint8_t)X? (GPIOB->ODR |= 0X80):(GPIOB->ODR &= ~0X80)//PB7

//INDICATED
#define   INDICATE(X)        (uint8_t)X? (GPIOA->ODR |= 0X40):(GPIOA->ODR &= ~0X40)//PA6

//输入功能引脚 (key event)
#define   KEY         		 (GPIOA->IDR&0x20)//PA5

//flash key
#define FLASH_RASS_KEY1 ((uint8_t)0x56) /*!< First RASS key */
#define FLASH_RASS_KEY2 ((uint8_t)0xAE) /*!< Second RASS key */

//输入sensor signal
#define  GATE          		(GPIOA->IDR&0x08)   //PA3
#define  SENSOR        		(GPIOA->IDR&0x10)   //PA4

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
     uint16_t Counter;                    //总计数      0
     uint8_t  Setting_Counter_Value;      //设定计数值  1
     uint8_t  Counter_Mode;               //计数方式    2
     uint8_t  Beep_Mode;                  //蜂鸣器开关  3
     uint8_t  Vibra_Delay_Setting;        //振动延时设置4
     uint8_t  Vibra_amplitude_Setting;    //振动幅度设置5
     uint8_t  Roller_Delay_Setting;       //滚筒延时设置6
     uint8_t  Roller_amplitude_Setting;   //滚筒幅度设置7
     uint8_t  Turnplate_amplitude_Setting;//转盘幅度设置8
     uint8_t  Vibra_Direction__Setting;   //振动方向设置9
	 uint8_t  Roller_Direction;           //滚筒方向设置10
	 uint8_t  Turnplate_Direction;        //转盘方向设置11

}Menu_Setting_Typedef;

extern uint32_t Roll_Motor_Delay;
extern uint8_t  Roll_Motor_Speed;

extern uint32_t Vibra_Motor_Delay;
extern uint8_t  Vibra_Motor_Speed;

extern uint32_t Turnplate_Motor_Delay;
extern uint8_t  Turnplate_Motor_Speed;

extern uint32_t time_count1, time_count2, time_count3, time_count4;
extern uint8_t  Roll_Puls, Vibra_Puls, Turnplate_Puls, Indicate_flag;

#endif
