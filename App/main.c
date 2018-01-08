
#include "stm8s.h"
#include "main.h"

//共阴极段码：
uint8_t const table[]={
0x3f,0x06,0x5b,0x4f, \
0x66,0x6d,0x7d,0x07, \
0x7f,0x6f,0x77,0x7c, \
0x39,0x5e,0x79,0x71};

//嘀嗒时钟
uint16_t Tick_clock=0;

//闪烁延时
uint16_t Display_delay=0;

//菜单设置
Menu_Setting_Typedef  Menu_Setting;

uint32_t Roll_Motor_Delay=0;
uint8_t  Roll_Motor_Speed=0;

uint32_t Vibra_Motor_Delay=0;
uint8_t  Vibra_Motor_Speed=0;

uint32_t Turnplate_Motor_Delay=0;
uint8_t  Turnplate_Motor_Speed=0;

uint32_t time_count1 = 0, time_count2 = 0, time_count3 = 0;
uint8_t  Roll_Puls = 0,   Vibra_Puls = 0,  Turnplate_Puls = 0;

//dummy loop
void delay_loop(u16 wt)
{
    while(wt--);
}

//主时钟，设置成16M
void Master_Clk_Inital(void)
{
    //时钟调整到16M

    /* Clear High speed internal clock prescaler */
    CLK->CKDIVR &= (uint8_t)(~CLK_CKDIVR_HSIDIV);

    /* Set High speed internal clock prescaler */
    CLK->CKDIVR |= (uint8_t)(0x00);

	return;

}

//timer2 初始化定时器2
void Timer2_setting(void)
{
    TIM2->PSCR = (uint8_t)(6);

    /* Set the Autoreload value */
    TIM2->ARRH = (uint8_t)(24>> 8); //100us 中断一次
    TIM2->ARRL = (uint8_t)(24);

    TIM2->IER |= (uint8_t)0x41;
    enableInterrupts();

    TIM2->CR1 |= (uint8_t)0x01;

	return;
}

//TIM3_CH2 边沿触发模式
void TIM3_CH2_Edge_PWM(void)
{
    TIM3->PSCR = (uint8_t)(0x8);

    /* Set the Autoreload value */
    TIM3->ARRH = (uint8_t)(49>> 8); //800us 中断一次
    TIM3->ARRL = (uint8_t)(49);

	TIM3->IER |= (uint8_t)0x41;
	enableInterrupts();

    TIM3->CR1 &= ~(uint8_t)0x01;
    //TIM3->CR1 |= (uint8_t)0x01;

	return;
}

/*
//掉电保护计数
void PVD_Interrupt_EXTI_Lin16(void)
{
   //

}
*/

//gpio 初始化GPIO
void GPIO_Inital(void)
{
  //输入，输出功能配置
  GPIOD->ODR &=  ~0xFF; 	//输出0
  GPIOD->DDR |=  0x6d;      //01101101  PD 0 2 3 5 6
  GPIOD->CR1 |=  0xFd;      //11111101  推挽输出

  GPIOE->ODR &=  ~0xFF; 	//
  GPIOE->DDR |=  0x6B;      //01100011  PE 0 1 3 5 6
  GPIOE->CR1 |=  0x6B;      //01101011

  GPIOG->ODR &=  ~0xFF;     //
  GPIOG->DDR |=  0x02;      //00000010  PG 1
  GPIOG->CR1 |=  0x02;      //

  GPIOB->ODR &=  ~0xFF; 	//
  GPIOB->DDR |=  0xBC;      //10111100  PB 2 3 4 5 7
  GPIOB->CR1 |=  0xBC;      //

  GPIOC->ODR &=  ~0xFF;     //
  GPIOC->DDR |=  0x5C;      //01011100  PC 2 3 4 6
  GPIOC->CR1 |=  0x5C;      //01011100
}

/*   EEPROM的问题
     uint16_t Counter;                    //总计数
     uint8_t  Setting_Counter_Value;      //设定计数值
     uint8_t  Counter_Mode;               //计数方式
     uint8_t  Beep_Mode;                  //蜂鸣器开关
     uint8_t  Vibra_Delay_Setting;        //振动延时设置
     uint8_t  Vibra_amplitude_Setting;    //振动幅度设置  0.5V一个分度（5-10V）
     uint8_t  Roller_Delay_Setting;       //滚筒延时设置
     uint8_t  Roller_amplitude_Setting;   //滚筒幅度设置 0.5V一个分度（5-10V）
     uint8_t  Turnplate_amplitude_Setting;//转盘速度设置
     uint8_t  Vibra_Direction__Setting;   //振动方向设置

*/

//读取数据
uint8_t FLASH_ReadByte(uint32_t Address)
{
  	return(*(PointerAttr uint8_t *) (MemoryAddressCast)Address);
}

//写数据
void FLASH_ProgramByte(uint32_t Address, uint8_t Data)
{
  	*(PointerAttr uint8_t*) (MemoryAddressCast)Address = Data;
}

//写参数
void Write_Parameters_to_flash(void)
{
    //解锁EEPROM
    FLASH->DUKR = FLASH_RASS_KEY2;
    FLASH->DUKR = FLASH_RASS_KEY1;

    while((FLASH->IAPSR&0x08) == 0);
    //解锁成功
    //EEPROM区从0X004001开始
    FLASH_ProgramByte(0x4001,Menu_Setting.Counter>>8);
    while((FLASH->IAPSR&0x04) == 0);
    FLASH_ProgramByte(0x4002,Menu_Setting.Counter);
    while((FLASH->IAPSR&0x04) == 0);
    FLASH_ProgramByte(0x4003,Menu_Setting.Setting_Counter_Value);
    while((FLASH->IAPSR&0x04) == 0);
    FLASH_ProgramByte(0x4004,Menu_Setting.Counter_Mode);
    while((FLASH->IAPSR&0x04) == 0);
    FLASH_ProgramByte(0x4005,Menu_Setting.Beep_Mode);
    while((FLASH->IAPSR&0x04) == 0);
    FLASH_ProgramByte(0x4006,Menu_Setting.Vibra_Delay_Setting);
    while((FLASH->IAPSR&0x04) == 0);
    FLASH_ProgramByte(0x4007,Menu_Setting.Vibra_amplitude_Setting);
    while((FLASH->IAPSR&0x04) == 0);
    FLASH_ProgramByte(0x4008,Menu_Setting.Roller_Delay_Setting);
    while((FLASH->IAPSR&0x04) == 0);
    FLASH_ProgramByte(0x4009,Menu_Setting.Roller_amplitude_Setting);
    while((FLASH->IAPSR&0x04) == 0);
    FLASH_ProgramByte(0x4010,Menu_Setting.Turnplate_amplitude_Setting);
    while((FLASH->IAPSR&0x04) == 0);
    FLASH_ProgramByte(0x4011,Menu_Setting.Vibra_Direction__Setting);
    while((FLASH->IAPSR&0x04) == 0);
    //锁定EEPROM 区
    FLASH->DUKR &=~0x08;
}

//恢复参数
void Read_Parameters_from_flash(void)
{
    Menu_Setting.Counter  = (uint16_t)FLASH_ReadByte(0x4001)<<8;
    Menu_Setting.Counter |= (uint16_t)FLASH_ReadByte(0x4002);
    Menu_Setting.Setting_Counter_Value = FLASH_ReadByte(0x4003);
    Menu_Setting.Counter_Mode = FLASH_ReadByte(0x4004);
    Menu_Setting.Beep_Mode = FLASH_ReadByte(0x4005);
    Menu_Setting.Vibra_Delay_Setting = FLASH_ReadByte(0x4006);
    Menu_Setting.Vibra_amplitude_Setting = FLASH_ReadByte(0x4007);
    Menu_Setting.Roller_Delay_Setting = FLASH_ReadByte(0x4008);
    Menu_Setting.Roller_amplitude_Setting = FLASH_ReadByte(0x4009);
    Menu_Setting.Turnplate_amplitude_Setting = FLASH_ReadByte(0x4010);
    Menu_Setting.Vibra_Direction__Setting = FLASH_ReadByte(0x4011);

}

//硬件初始化
void Bsp_Inital(void)
{
    //主时钟设置成16M
    Master_Clk_Inital();

	//IO初始化
    GPIO_Inital();

    BEEP_PE6_DO_Q3_1(1);

	//读取EEPROM数据
    Read_Parameters_from_flash();

	//电机1
    M1_PG1_PWM_Q6_1(0);
    //M1_PE3_EN_J5_2(0);
	M1_PE1_DIR_J5_5(0);
	M1_PC4_PLS_J5_3(0);

	//电机2
    M2_PE5_PWM_Q9_1(0);
    M2_PC2_EN_J7_2(0);
	M2_PC3_DIR_J7_5(0);
	M2_PC6_PLS_J7_3(0);

    //电机3
    M3_PD0_PWM_Q7_1(0);
    M3_PE0_EN_J6_2(0);
	M3_PD3_DIR_J6_5(0);
	M3_PD2_PLS_J6_3(0);

    //测试等
    LED_PB7_DO_D6_2(0);

    //LED消影
    LED_PD5_TST(0);

    //定时器,定时100us
    Timer2_setting();
    TIM3_CH2_Edge_PWM();

	return;
}

//Main fuction
/*
系统功能：

1,总计数状态，按 OK 键切换
2，恢复出厂设置，同时按下 SET及OK 键3S,LED屏幕闪烁3s,蜂鸣器提示恢复出厂设置
3，SET键描述
   3.1  待机状态下或者工作状态，第一次按SET键，按住3S,进入计数数量设定，
        LED显示屏闪烁显示，可按 UP 和 DOWN 键调整计数数量
        3秒内按OK键或者3秒后不按键，系统完成设定并且蜂鸣提示

   3.2  待机状态下或者工作状态，第二次按SET键，系统进入递增，递减模式设定
        LED显示屏闪烁显示， 00 递增 ，01递减，默认为00，可按 UP 和 DOWN 键切换模式
        3秒内按OK键或者3秒后不按键，系统完成设定并且蜂鸣提示



*/

//串行数据输送 MSB
void Display_tube(uint8_t Char_DATA)
{
     int8_t numb_counter=0;
     MCU_PB3_CK_J11_2(1);
     delay_loop(10);
     for(numb_counter=7;numb_counter>=0;numb_counter--)
     {
           MCU_PB3_CK_J11_2(0);
           if((Char_DATA>>numb_counter)&0X01) {MCU_PB2_DO_J11_1(1);}//上升沿传入数据位
           else{MCU_PB2_DO_J11_1(0);}
           delay_loop(10);
           MCU_PB3_CK_J11_2(1);
           delay_loop(10);
     }
     MCU_PB3_CK_J11_2(1);
}

void Scan_Key(uint8_t CHAR1,uint8_t CHAR2,uint8_t CHAR3,uint8_t CHAR4)
{
    uint8_t 	 Display_Sequence=1;
	uint8_t 	 Bit_Choice=1;
	static uint16_t  Jump_T=0;

    if( (1 == Menu_Setting.Setting_Key_Lock) || (Menu_Setting.System_Reset_Delay >0) )//设置模式
	{
	      if((Display_delay == 0) && (Jump_T == 0))
		  {
	        Display_delay=5000;
	        Jump_T =1;
	      }
		  else if((Display_delay == 0) && (Jump_T == 1))
	      {
	        Display_delay=5000;
	        Jump_T =0;
	      }
    }

    for(Bit_Choice=1; Bit_Choice < 5; )//逐个显示数码管，并扫描按键
	{
		  switch(Display_Sequence)
		  {
		      case 1://第一步，数码管显示
			  {
			        if( 1 == Bit_Choice )
					{
				          if( Menu_Setting.System_Reset_Delay >0 ){if(Jump_T == 1){ CHAR1=0;}}

						  MCU_PB4_A0_J11_3(0);
				          MCU_PB5_A1_J11_4(0);

						  LED_PD6_XY_J11_6(1);
						  Display_tube(CHAR1);
						  delay_loop(1000);
						  LED_PD6_XY_J11_6(0);

			        }
					else if( 2 == Bit_Choice )
					{
				          if( Menu_Setting.System_Reset_Delay >0 ){if(Jump_T == 1){ CHAR2=0;}}

						  MCU_PB4_A0_J11_3(1);
				          MCU_PB5_A1_J11_4(0);

						  LED_PD6_XY_J11_6(1);
						  Display_tube(CHAR2);
						  delay_loop(1000);
						  LED_PD6_XY_J11_6(0);
			        }
					else if( 3 == Bit_Choice )
					{
				          if( (1 == Menu_Setting.Setting_Key_Lock) || (Menu_Setting.System_Reset_Delay >0) ){if(Jump_T== 1){CHAR3=0;}}

						  MCU_PB4_A0_J11_3(0);
				          MCU_PB5_A1_J11_4(1);

				          LED_PD6_XY_J11_6(1);
						  Display_tube(CHAR3);
						  delay_loop(1000);
						  LED_PD6_XY_J11_6(0);

			        }
					else if( 4 == Bit_Choice )
					{
				          if ( (1 == Menu_Setting.Setting_Key_Lock) || (Menu_Setting.System_Reset_Delay >0) ){if(Jump_T == 1){ CHAR4=0;}}

				          MCU_PB4_A0_J11_3(1);
				          MCU_PB5_A1_J11_4(1);

				          LED_PD6_XY_J11_6(1);
						  Display_tube(CHAR4);
						  delay_loop(1000);
						  LED_PD6_XY_J11_6(0);
					}

			        Display_Sequence++;

		  	  }
		  	 break;

		      case 2://按键扫描
			  {
			        if(1 == Bit_Choice)
					{
			          if(!KEY_J11_5_DI_PB6){Menu_Setting.OK_Flag=1;}
			          else{Menu_Setting.OK_Flag= 0;}
			        }
					else if(2 == Bit_Choice)
					{
			          if(!KEY_J11_5_DI_PB6){Menu_Setting.DN_Flag=1;}
			          else{Menu_Setting.DN_Flag= 0;}
			        }
					else if(3 == Bit_Choice)
					{
			          if(!KEY_J11_5_DI_PB6){Menu_Setting.UP_Flag = 1;}
			          else{Menu_Setting.UP_Flag= 0;}
			        }
					else if(4 == Bit_Choice)
					{
			          if(!KEY_J11_5_DI_PB6){Menu_Setting.Setting_Flag = 1;}
			          else{Menu_Setting.Setting_Flag = 0;}
			        }

			        Display_Sequence++;

			  }
			  break;

		      case 3:
			  {
			        LED_PD6_XY_J11_6(1);    //消影
			        Display_tube(0x00);
					delay_loop(1000);
					LED_PD6_XY_J11_6(0);

					Display_Sequence=1;
					Bit_Choice++;
		      }
			  break;

		      default:
			  	break;

	      }
    }
}


void Ok_Key_Process(void)
{
	static uint8_t OK_Switch_Mode=0;

    if( (1 == Menu_Setting.OK_Flag) && (0 == Menu_Setting.Setting_Flag) )
	{
	      if(0 == Menu_Setting.Beep_Mode )
		  {
	        BEEP_PE6_DO_Q3_1(Enable);//打开蜂鸣
	        Menu_Setting.Setting_Key_Delay=0;
	      }

		  if(OK_Switch_Mode == 0)
		  {
			OK_Switch_Mode = 1;
		  }
		  else if(OK_Switch_Mode == 1)
		  {
			OK_Switch_Mode = 0;
		  }

    }

    while( (1 == Menu_Setting.OK_Flag) && (0 == Menu_Setting.Setting_Flag) )
	{

	      if(Menu_Setting.Setting_Key_Delay < 10)
		  {
		  	Menu_Setting.Setting_Key_Delay++;
		  }
	      else
		  {
		  	BEEP_PE6_DO_Q3_1(Disable);
		  }

	      Scan_Key(table[Menu_Setting.Counter/1000],table[Menu_Setting.Counter/100],table[Menu_Setting.Counter/10],table[Menu_Setting.Counter%10]);

    }

    BEEP_PE6_DO_Q3_1(Disable);
    Menu_Setting.Setting_Key_Delay=0;

	if(OK_Switch_Mode==0)
	{
		Scan_Key(table[Menu_Setting.Counter/1000],table[Menu_Setting.Counter/100],table[Menu_Setting.Counter/10],table[Menu_Setting.Counter%10]);
	}
	else if(OK_Switch_Mode == 1)
	{
		Scan_Key(table[Menu_Setting.Setting_Counter_Value /1000],table[Menu_Setting.Setting_Counter_Value/100],table[Menu_Setting.Setting_Counter_Value/10],table[Menu_Setting.Setting_Counter_Value%10]);
	}

	return;
}

/**  系统复位 ,同时按下OK和Setting按键，系统复位
     uint8_t   Setting_Flag;        //Setting键值标志
     uint8_t   OK_Flag;             //OK
*/
void Scan_Reset_Signal(void)
{
    static uint16_t Reset_Key_Conter=0;
    if((Menu_Setting.Setting_Flag == 1)&&(Menu_Setting.OK_Flag == 1))
    {
        Reset_Key_Conter++;
        if(Reset_Key_Conter>200){   //进入Setting功能按键区
            Reset_Key_Conter=0;
            Menu_Setting.System_Reset=1;
            Menu_Setting.System_Reset_Delay=30000;
        }
        if(Menu_Setting.System_Reset == 1)
		{
            while((Menu_Setting.Setting_Flag == 1)&&(Menu_Setting.OK_Flag == 1))
			{
                Scan_Key(table[Menu_Setting.Counter/1000],table[Menu_Setting.Counter/100],table[Menu_Setting.Counter/10],table[Menu_Setting.Counter%10]);
        	}
            //复位参数
            Menu_Setting.Counter = 0;
            Menu_Setting.Counter = 0;
            Menu_Setting.Setting_Counter_Value = 0;
            Menu_Setting.Counter_Mode = 0;
            Menu_Setting.Beep_Mode = 0;
            Menu_Setting.Vibra_Delay_Setting = 0;
            Menu_Setting.Vibra_amplitude_Setting = 0;
            Menu_Setting.Roller_Delay_Setting = 0;
            Menu_Setting.Roller_amplitude_Setting = 0;
            Menu_Setting.Turnplate_amplitude_Setting = 0;
            Menu_Setting.Vibra_Direction__Setting = 0;
            Write_Parameters_to_flash();
            //动作复位

            //复位标志
            Menu_Setting.System_Reset=0;
        }
    }

}

void Setting_Key_Process(void)
{
    static uint16_t M_Key_Couter=0;

    if((1 == Menu_Setting.Setting_Flag) && (0 == Menu_Setting.OK_Flag) )
    {

      M_Key_Couter++;
      if(M_Key_Couter>200)//按OK键3s进入设置模式
	  {

	        M_Key_Couter=0;
	        Menu_Setting.Mune_Setting_Step=1;   //模式菜单
	        Menu_Setting.Setting_Key_Lock=1;	//设置模式标志
			Menu_Setting.System_Set_Delay = 30000;

      }

      if(1 == Menu_Setting.Setting_Key_Lock)
	  {

	        if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);}//蜂鸣提示打开

	       	while( ((1 == Menu_Setting.Setting_Flag) && (0 == Menu_Setting.OK_Flag)) || (Menu_Setting.Setting_Key_Delay>10))
			{

		          if(Menu_Setting.Setting_Key_Delay < 10) {Menu_Setting.Setting_Key_Delay++;}
		          else{BEEP_PE6_DO_Q3_1(Disable);} //蜂鸣提示关闭

		          Scan_Key(table[Menu_Setting.Mune_Setting_Step], 0 ,table[Menu_Setting.Setting_Counter_Value/10], table[Menu_Setting.Setting_Counter_Value%10]);

	        }

	        //进入二级菜单目录
	        while(1)
	        {

	          switch(Menu_Setting.Mune_Setting_Step)
	          {
	          	case 1:
				{
	            	Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Setting_Counter_Value/10],table[Menu_Setting.Setting_Counter_Value%10]);
	            	if(1 == Menu_Setting.UP_Flag)
					{
	              		if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);}
	              		if(Menu_Setting.Setting_Counter_Value < 99){Menu_Setting.Setting_Counter_Value++;}
						Menu_Setting.System_Set_Delay = 30000;
	            	}
					else if(1 == Menu_Setting.DN_Flag)
					{
	              		if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);}
	              		if(Menu_Setting.Setting_Counter_Value > 0){Menu_Setting.Setting_Counter_Value--;}
						Menu_Setting.System_Set_Delay = 30000;
	            	}

	            	if(1 == Menu_Setting.Setting_Flag)
					{
						if(0 == Menu_Setting.Beep_Mode)
						{
							BEEP_PE6_DO_Q3_1(Enable);
						}
						Menu_Setting.Mune_Setting_Step++;
						Menu_Setting.System_Set_Delay = 30000;
					}

	            	while( (1 == Menu_Setting.Setting_Flag) || (1 == Menu_Setting.DN_Flag) || (1 == Menu_Setting.UP_Flag) || (Menu_Setting.Setting_Key_Delay>10) )
					{
		              	if(Menu_Setting.Setting_Key_Delay < 10) {Menu_Setting.Setting_Key_Delay++;}
		              	else{BEEP_PE6_DO_Q3_1(Disable);}
		              	Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Setting_Counter_Value/10],table[Menu_Setting.Setting_Counter_Value%10]);
	            	}
	            	Menu_Setting.Setting_Key_Delay=0;
	            	BEEP_PE6_DO_Q3_1(Disable);
	          	}
			  	break;

	          	case 2:
				{
	   	            Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Counter_Mode/10],table[Menu_Setting.Counter_Mode%10]);
	            	//递增/递减模式设计  00 递增模式 , 01递减模式
	            	if(1 == Menu_Setting.DN_Flag)
					{
	              		if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);}
	              		Menu_Setting.Counter_Mode = 0;
						Menu_Setting.System_Set_Delay = 30000;
	            	}
					else if(1 == Menu_Setting.UP_Flag)
					{
	              		if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);}
	              		Menu_Setting.Counter_Mode = 1;
						Menu_Setting.System_Set_Delay = 30000;
	            	}
	            	if(1 == Menu_Setting.Setting_Flag){ if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);}Menu_Setting.Mune_Setting_Step++;Menu_Setting.System_Set_Delay = 30000;}

	            	while( (1 == Menu_Setting.Setting_Flag) || (1 == Menu_Setting.DN_Flag) || (1 == Menu_Setting.UP_Flag) || (Menu_Setting.Setting_Key_Delay>10)){
	              	if(Menu_Setting.Setting_Key_Delay < 10) {Menu_Setting.Setting_Key_Delay++;}
	              	else{BEEP_PE6_DO_Q3_1(Disable);}
	              	Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Counter_Mode/10],table[Menu_Setting.Counter_Mode%10]);
	            	}
	            	Menu_Setting.Setting_Key_Delay=0;
	            	BEEP_PE6_DO_Q3_1(Disable);
	          	}
				break;

	          case 3:{

	            //扫描按键，显示内容
	            Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Beep_Mode/10],table[Menu_Setting.Beep_Mode%10]);
	            //蜂鸣器开关设置    00开启蜂鸣器模式，01关闭蜂鸣器模式
	            if(1 == Menu_Setting.DN_Flag){
	              if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);}
	              Menu_Setting.Beep_Mode = 0;
				  Menu_Setting.System_Set_Delay = 30000;

	            }else if(1 == Menu_Setting.UP_Flag){
	              if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);}
	              Menu_Setting.Beep_Mode = 1;
				  Menu_Setting.System_Set_Delay = 30000;

	            }
	            if(1 == Menu_Setting.Setting_Flag){ if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);} Menu_Setting.Mune_Setting_Step++; Menu_Setting.System_Set_Delay = 30000;}

	            //松手 检测
	            while( (1 == Menu_Setting.Setting_Flag) || (1 == Menu_Setting.DN_Flag) || (1 == Menu_Setting.UP_Flag) || (Menu_Setting.Setting_Key_Delay>10) ){
	              if(Menu_Setting.Setting_Key_Delay < 10) {Menu_Setting.Setting_Key_Delay++;}
	              else{BEEP_PE6_DO_Q3_1(Disable);}
	              Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Beep_Mode/10],table[Menu_Setting.Beep_Mode%10]);
	            }
	            Menu_Setting.Setting_Key_Delay=0;
	            BEEP_PE6_DO_Q3_1(Disable);

	          }break;

	          case 4:{

	            //扫描按键，显示内容
	            Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Vibra_Delay_Setting/10],table[Menu_Setting.Vibra_Delay_Setting%10]);
	            //振动延时设置     01-12s
	            if(1 == Menu_Setting.UP_Flag){
	              if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);}
	              if(Menu_Setting.Vibra_Delay_Setting < 12){Menu_Setting.Vibra_Delay_Setting++;Menu_Setting.System_Set_Delay = 30000;}
	            }else if(1 == Menu_Setting.DN_Flag){
	              if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);}
	              if(Menu_Setting.Vibra_Delay_Setting > 1){Menu_Setting.Vibra_Delay_Setting--;}
				  Menu_Setting.System_Set_Delay = 30000;

	            }
	            if(1 == Menu_Setting.Setting_Flag){ if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);}Menu_Setting.Mune_Setting_Step++;Menu_Setting.System_Set_Delay = 30000;}

	            //松手 检测
	            while( (1 == Menu_Setting.Setting_Flag) || (1 == Menu_Setting.DN_Flag) || (1 == Menu_Setting.UP_Flag) || (Menu_Setting.Setting_Key_Delay>10) ){
	              if(Menu_Setting.Setting_Key_Delay < 10) {Menu_Setting.Setting_Key_Delay++;}
	              else{BEEP_PE6_DO_Q3_1(Disable);}
	              Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Vibra_Delay_Setting/10],table[Menu_Setting.Vibra_Delay_Setting%10]);
	            }
	            Menu_Setting.Setting_Key_Delay=0;
	            BEEP_PE6_DO_Q3_1(Disable);

	          }break;

	          case 5:{

	            //扫描按键，显示内容
	            Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Vibra_amplitude_Setting/10],table[Menu_Setting.Vibra_amplitude_Setting%10]);
	            //振动强度设置     01-10 0.5V一个分度
	            if(1 == Menu_Setting.UP_Flag){
	              if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);}
	              if(Menu_Setting.Vibra_amplitude_Setting < 10){Menu_Setting.Vibra_amplitude_Setting++;}
				  Menu_Setting.System_Set_Delay = 30000;

	            }else if(1 == Menu_Setting.DN_Flag){
	              if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);}
	              if(Menu_Setting.Vibra_amplitude_Setting > 1){Menu_Setting.Vibra_amplitude_Setting--;}
				  Menu_Setting.System_Set_Delay = 30000;

	            }
	            if(1 == Menu_Setting.Setting_Flag){ if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);}Menu_Setting.Mune_Setting_Step++;Menu_Setting.System_Set_Delay = 30000;}

	            //松手 检测
	            while( (1 == Menu_Setting.Setting_Flag) || (1 == Menu_Setting.DN_Flag) || (1 == Menu_Setting.UP_Flag) || (Menu_Setting.Setting_Key_Delay>10) ){
	              if(Menu_Setting.Setting_Key_Delay < 10) {Menu_Setting.Setting_Key_Delay++;}
	              else{BEEP_PE6_DO_Q3_1(Disable);}
	              Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Vibra_amplitude_Setting/10],table[Menu_Setting.Vibra_amplitude_Setting%10]);
	            }
	            Menu_Setting.Setting_Key_Delay=0;
	            BEEP_PE6_DO_Q3_1(Disable);

	          }break;
	          case 6:{

	            //扫描按键，显示内容
	            Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Roller_Delay_Setting/10],table[Menu_Setting.Roller_Delay_Setting%10]);
	            //滚筒延时设置     01-12s
	            if(1 == Menu_Setting.UP_Flag){
	              if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);}
	              if(Menu_Setting.Roller_Delay_Setting < 12){Menu_Setting.Roller_Delay_Setting++;}
				  Menu_Setting.System_Set_Delay = 30000;

	            }else if(1 == Menu_Setting.DN_Flag){
	              if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);}
	              if(Menu_Setting.Roller_Delay_Setting > 1){Menu_Setting.Roller_Delay_Setting--;}
				  Menu_Setting.System_Set_Delay = 30000;

	            }
	            if(1 == Menu_Setting.Setting_Flag){ if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);}Menu_Setting.Mune_Setting_Step++;Menu_Setting.System_Set_Delay = 30000;}

	            //松手 检测
	            while( (1 == Menu_Setting.Setting_Flag) || (1 == Menu_Setting.DN_Flag) || (1 == Menu_Setting.UP_Flag)|| (Menu_Setting.Setting_Key_Delay>10) ){
	              if(Menu_Setting.Setting_Key_Delay < 10) {Menu_Setting.Setting_Key_Delay++;}
	              else{BEEP_PE6_DO_Q3_1(Disable);}
	              Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Roller_Delay_Setting/10],table[Menu_Setting.Roller_Delay_Setting%10]);
	            }
	            Menu_Setting.Setting_Key_Delay=0;
	            BEEP_PE6_DO_Q3_1(Disable);

	          }break;
	          case 7:{

	            //扫描按键，显示内容
	            Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Roller_amplitude_Setting/10],table[Menu_Setting.Roller_amplitude_Setting%10]);
	            //滚筒强度设置     01-10 每0.5v一个分度
	            if(1 == Menu_Setting.UP_Flag){
	              if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);}
	              if(Menu_Setting.Roller_amplitude_Setting < 10){Menu_Setting.Roller_amplitude_Setting++;}
				  Menu_Setting.System_Set_Delay = 30000;

	            }else if(1 == Menu_Setting.DN_Flag){
	              if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);}
	              if(Menu_Setting.Roller_amplitude_Setting > 1){Menu_Setting.Roller_amplitude_Setting--;}
				  Menu_Setting.System_Set_Delay = 30000;

	            }
	            if(1 == Menu_Setting.Setting_Flag){ if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);}Menu_Setting.Mune_Setting_Step++;Menu_Setting.System_Set_Delay = 30000;}

	            //松手 检测
	            while( (1 == Menu_Setting.Setting_Flag) || (1 == Menu_Setting.DN_Flag) || (1 == Menu_Setting.UP_Flag)|| (Menu_Setting.Setting_Key_Delay>10) ){
	              if(Menu_Setting.Setting_Key_Delay < 10) {Menu_Setting.Setting_Key_Delay++;}
	              else{BEEP_PE6_DO_Q3_1(Disable);}
	              Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Roller_amplitude_Setting/10],table[Menu_Setting.Roller_amplitude_Setting%10]);
	            }
	            Menu_Setting.Setting_Key_Delay=0;
	            BEEP_PE6_DO_Q3_1(Disable);

	          }break;
	          case 8:{

	            //扫描按键，显示内容
	            Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Turnplate_amplitude_Setting/10],table[Menu_Setting.Turnplate_amplitude_Setting%10]);
	            //转盘马达设置    01-10 根据螺丝的特性调整
	            if(1 == Menu_Setting.UP_Flag){
	              if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);}
	              if(Menu_Setting.Turnplate_amplitude_Setting< 10){Menu_Setting.Turnplate_amplitude_Setting++;}
				  Menu_Setting.System_Set_Delay = 30000;

	            }else if(1 == Menu_Setting.DN_Flag){
	              if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);}
	              if(Menu_Setting.Turnplate_amplitude_Setting > 1){Menu_Setting.Turnplate_amplitude_Setting--;}
				  Menu_Setting.System_Set_Delay = 30000;

	            }
	            if(1 == Menu_Setting.Setting_Flag){ if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);}Menu_Setting.Mune_Setting_Step++;Menu_Setting.System_Set_Delay = 30000;}

	            //松手 检测
	            while( (1 == Menu_Setting.Setting_Flag) || (1 == Menu_Setting.DN_Flag) || (1 == Menu_Setting.UP_Flag)|| (Menu_Setting.Setting_Key_Delay>10)){
	              if(Menu_Setting.Setting_Key_Delay < 10) {Menu_Setting.Setting_Key_Delay++;}
	              else{BEEP_PE6_DO_Q3_1(Disable);}
	              Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Turnplate_amplitude_Setting/10],table[Menu_Setting.Turnplate_amplitude_Setting%10]);
	            }
	            Menu_Setting.Setting_Key_Delay=0;
	            BEEP_PE6_DO_Q3_1(Disable);

	          }break;

	          case 9:{

	            //扫描按键，显示内容
	            Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Vibra_Direction__Setting/10],table[Menu_Setting.Vibra_Direction__Setting%10]);
	            //振动方向设置   00 正方向 ， 01 反方向
	            if(1 == Menu_Setting.UP_Flag){
	              if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);}
	              Menu_Setting.Vibra_Direction__Setting = 1;
				  Menu_Setting.System_Set_Delay = 30000;

	            }else if(1 == Menu_Setting.DN_Flag){
	              if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);}
	              Menu_Setting.Vibra_Direction__Setting = 0;
				  Menu_Setting.System_Set_Delay = 30000;

	            }
	            if(1 == Menu_Setting.Setting_Flag){ if(0 == Menu_Setting.Beep_Mode){BEEP_PE6_DO_Q3_1(Enable);}Menu_Setting.Mune_Setting_Step++;Menu_Setting.System_Set_Delay = 30000;}

	            //松手 检测
	            while( (1 == Menu_Setting.Setting_Flag) || (1 == Menu_Setting.DN_Flag) || (1 == Menu_Setting.UP_Flag)|| (Menu_Setting.Setting_Key_Delay>10) ){
	              if(Menu_Setting.Setting_Key_Delay < 10) {Menu_Setting.Setting_Key_Delay++;}
	              else{BEEP_PE6_DO_Q3_1(Disable);}
	              Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Vibra_Direction__Setting/10],table[Menu_Setting.Vibra_Direction__Setting%10]);
	            }
	            Menu_Setting.Setting_Key_Delay=0;
	            BEEP_PE6_DO_Q3_1(Disable);

	          }break;

	          default:{}

	          }

	          //扫描菜单键
	          if((1 == Menu_Setting.OK_Flag )|| (Menu_Setting.Mune_Setting_Step>9)||Menu_Setting.System_Set_Delay <= 0){

	            Menu_Setting.OK_Flag=0;
	            Menu_Setting.Setting_Key_Lock=0;
	            Menu_Setting.Setting_Flag=0;

	            //保存设置数据，退出菜单
	            if(Menu_Setting.Mune_Setting_Step<=9)
				{
	              	Write_Parameters_to_flash();
	            }

	            if(0 == Menu_Setting.Beep_Mode)
				{
	              	BEEP_PE6_DO_Q3_1(Enable);
	              	Menu_Setting.Setting_Key_Delay=0;
	            }
	            break; //跳出for循环
	          }

	        }
      }
    }else{

      M_Key_Couter=0;
    }
}

void main(void)
{
  Menu_Setting.Beep_Mode =1;
  Menu_Setting.Vibra_Delay_Setting = 9;
  Menu_Setting.Vibra_amplitude_Setting=8;
  Menu_Setting.Roller_Delay_Setting=10;
  Menu_Setting.Roller_amplitude_Setting=10;
  Menu_Setting.Turnplate_amplitude_Setting=9;
  Menu_Setting.Vibra_Direction__Setting=0;
  Write_Parameters_to_flash();

  Bsp_Inital();

  while(1)
  {

       Ok_Key_Process();
       Setting_Key_Process();
       Scan_Reset_Signal();

	   if(SENSOR_J9_5_DI_PD4 == 0)
	   {
		    TIM3->CR1 |= (uint8_t)0x01;

			Roll_Motor_Delay =  (uint32_t)Menu_Setting.Roller_Delay_Setting * 1000 * 10;  //设置滚筒延时
			Roll_Motor_Speed =  0;                                                        //设置滚筒速度
			M3_PD3_DIR_J6_5(1);                                                           //设置方向
            M3_PD0_PWM_Q7_1(1); //滚筒电机
			M3_PE0_EN_J6_2(0);


			Vibra_Motor_Delay = (uint32_t)Menu_Setting.Vibra_Delay_Setting * 1000 * 10;  //设置振动延时
			Vibra_Motor_Speed =  0;                                                      //设置振动速度
			M1_PE1_DIR_J5_5(1);                                                          //设置方向
            M1_PG1_PWM_Q6_1(1); //振动电机
			//M1_PE3_EN_J5_2(0);

			Turnplate_Motor_Speed = 0;                                                   //设置转盘速度
			M2_PC3_DIR_J7_5(1);
			M2_PE5_PWM_Q9_1(1); //转盘电机
			M2_PC2_EN_J7_2(0);

			Menu_Setting.Counter++;
			if(Menu_Setting.Counter >= Menu_Setting.Setting_Counter_Value*100 )
			{
				Menu_Setting.Counter = 0;
			}

	   }

	   if(Roll_Motor_Delay <= 0){ M3_PD0_PWM_Q7_1(0);M3_PD3_DIR_J6_5(0);M2_PC6_PLS_J7_3(0);M3_PE0_EN_J6_2(1); }
	   if(Vibra_Motor_Delay <= 0){M1_PG1_PWM_Q6_1(0);M1_PE1_DIR_J5_5(0);M2_PC6_PLS_J7_3(0);M1_PE3_EN_J5_2(1); TIM3->CR1 &= ~(uint8_t)0x01;}
	   if(SENSOR_J9_2_DI_PD7 == 0){M2_PE5_PWM_Q9_1(0);M2_PC3_DIR_J7_5(0);M2_PC6_PLS_J7_3(0);M2_PC2_EN_J7_2(1);}

  }

}
