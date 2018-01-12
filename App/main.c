
#include "stm8s.h"
#include "main.h"

//���������룺
uint8_t const table[]={
0x3f,0x06,0x5b,0x4f, \
0x66,0x6d,0x7d,0x07, \
0x7f,0x6f,0x77,0x7c, \
0x39,0x5e,0x79,0x71};

//���ʱ��
uint16_t Tick_clock=0;

//��˸��ʱ
uint16_t Display_delay=0;

//�˵�����
Menu_Setting_Typedef  Menu_Setting;

uint32_t Roll_Motor_Delay=0;
uint8_t  Roll_Motor_Speed=0;

uint32_t Vibra_Motor_Delay=0;
uint8_t  Vibra_Motor_Speed=0;

uint32_t Turnplate_Motor_Delay=0;
uint8_t  Turnplate_Motor_Speed=0;

uint32_t time_count1 = 0, time_count2 = 0, time_count3 = 0, time_count4 = 0;
uint8_t  Roll_Puls = 0,   Vibra_Puls = 0,  Turnplate_Puls = 0, Indicate_flag = 0;

void delay_loop(u16 wt)
{
    while(wt--);
}

//��ʱ�ӣ����ó�16M
void Master_Clk_Inital(void)
{
    //ʱ�ӵ�����16M

    /* Clear High speed internal clock prescaler */
    CLK->CKDIVR &= (uint8_t)(~CLK_CKDIVR_HSIDIV);

    /* Set High speed internal clock prescaler */
    CLK->CKDIVR |= (uint8_t)(0x00);

	return;

}

//timer2 ��ʼ����ʱ��2
void Timer2_setting(void)
{
    TIM2->PSCR = (uint8_t)(6);

    /* Set the Autoreload value */
    TIM2->ARRH = (uint8_t)(24>> 8); //100us �ж�һ��
    TIM2->ARRL = (uint8_t)(24);

    TIM2->IER |= (uint8_t)0x41;
    enableInterrupts();

    TIM2->CR1 |= (uint8_t)0x01;

	return;
}

//TIM3_CH2 ���ش���ģʽ
void TIM3_CH2_Edge_PWM(void)
{
    TIM3->PSCR = (uint8_t)(0x8);

    /* Set the Autoreload value */
    TIM3->ARRH = (uint8_t)(49>> 8); //800us �ж�һ��
    TIM3->ARRL = (uint8_t)(49);

	TIM3->IER |= (uint8_t)0x41;
	enableInterrupts();

    TIM3->CR1 &= ~(uint8_t)0x01;

	return;
}


//gpio ��ʼ��GPIO
void GPIO_Inital(void)
{
  GPIOA->ODR &=  ~0xFF; 	//
  GPIOA->DDR |=  0x42;      //  01000010   �������룬�������
  GPIOA->CR1 |=  0x7A;      //  01111010

  GPIOB->ODR &=  ~0xFF; 	//
  GPIOB->DDR |=  0xFF;      // 11111111
  GPIOB->CR1 |=  0xFF;      //

  GPIOD->ODR &=  ~0xFF; 	//
  GPIOD->DDR |=  0xB9;      // 10111001
  GPIOD->CR1 |=  0xB9;      //

  GPIOE->ODR &=  ~0xFF; 	//
  GPIOE->DDR |=  0x29;      // 00101001
  GPIOE->CR1 |=  0x29;      //
}


//��ȡ����
uint8_t FLASH_ReadByte(uint32_t Address)
{
  	return(*(PointerAttr uint8_t *) (MemoryAddressCast)Address);
}

//д����
void FLASH_ProgramByte(uint32_t Address, uint8_t Data)
{
  	*(PointerAttr uint8_t*) (MemoryAddressCast)Address = Data;
}

//д����
void Write_Parameters_to_flash(void)
{
    //����EEPROM
    FLASH->DUKR = FLASH_RASS_KEY2;
    FLASH->DUKR = FLASH_RASS_KEY1;

    while((FLASH->IAPSR&0x08) == 0);
    //�����ɹ�
    //EEPROM����0X004001��ʼ
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
	FLASH_ProgramByte(0x4012,Menu_Setting.Roller_Direction);
    while((FLASH->IAPSR&0x04) == 0);
	FLASH_ProgramByte(0x4013,Menu_Setting.Turnplate_Direction);
    while((FLASH->IAPSR&0x04) == 0);

    //����EEPROM ��
    FLASH->DUKR &=~0x08;

	return;
}

//�ָ�����
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
	Menu_Setting.Roller_Direction = FLASH_ReadByte(0x4012);
	Menu_Setting.Turnplate_Direction = FLASH_ReadByte(0x4013);

	return;

}

//Ӳ����ʼ��
void Bsp_Inital(void)
{
    //��ʱ�����ó�16M
    Master_Clk_Inital();

	//IO��ʼ��
    GPIO_Inital();

    BEEP_A(1);

	//��ȡEEPROM����
    Read_Parameters_from_flash();

	//���1
    VIBRA_POWER(0);
	VIBRA_DIR(0);
	VIBRA_PWM(1);

	//���2
    TURN_POWER(0);
	TURN_DIR(0);
	TURN_PWM(1);

    //���3
    ROLL_POWER(0);
	ROLL_DIR(0);
	ROLL_PWM(1);

    //���Ե�
    WORK_LED(0);
	INDICATE(1);

    //LED��Ӱ
    TUBE_CLR(0);


    //��ʱ��,��ʱ100us
    Timer2_setting();
    TIM3_CH2_Edge_PWM();

	return;
}


//������������ MSB
void Display_tube(uint8_t Char_DATA)
{
     int8_t numb_counter=0;
     TUBE_CLK(1);
     delay_loop(10);
     for(numb_counter=7;numb_counter>=0;numb_counter--)
     {
           TUBE_CLK(0);
           if((Char_DATA>>numb_counter)&0X01) {TUBE_DATA(1);}//�����ش�������λ
           else{TUBE_DATA(0);}
           delay_loop(10);
           TUBE_CLK(1);
           delay_loop(10);
     }
     TUBE_CLK(1);
}

void Scan_Key(uint8_t CHAR1,uint8_t CHAR2,uint8_t CHAR3,uint8_t CHAR4)
{
    uint8_t 	 Display_Sequence=1;
	uint8_t 	 Bit_Choice=1;
	static uint16_t  Jump_T=0;

    if( (1 == Menu_Setting.Setting_Key_Lock) || (Menu_Setting.System_Reset_Delay >0) )//����ģʽ
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

    for(Bit_Choice=1; Bit_Choice < 5; )//�����ʾ����ܣ���ɨ�谴��
	{
		  switch(Display_Sequence)
		  {
		      case 1://��һ�����������ʾ
			  {
			        if( 1 == Bit_Choice )
					{
				          if( Menu_Setting.System_Reset_Delay >0 ){if(Jump_T == 1){ CHAR1=0;}}

						  TUBE_A0(0);
				          TUBE_A1(0);

						  TUBE_CLR(1);
						  Display_tube(CHAR1);
						  delay_loop(1000);
						  TUBE_CLR(0);

			        }
					else if( 2 == Bit_Choice )
					{
				          if( Menu_Setting.System_Reset_Delay >0 ){if(Jump_T == 1){ CHAR2=0;}}

						  TUBE_A0(1);
				          TUBE_A1(0);

						  TUBE_CLR(1);
						  Display_tube(CHAR2);
						  delay_loop(1000);
						  TUBE_CLR(0);
			        }
					else if( 3 == Bit_Choice )
					{
				          if( (1 == Menu_Setting.Setting_Key_Lock) || (Menu_Setting.System_Reset_Delay >0) ){if(Jump_T== 1){CHAR3=0;}}

						  TUBE_A0(0);
				          TUBE_A1(1);

				          TUBE_CLR(1);
						  Display_tube(CHAR3);
						  delay_loop(1000);
						  TUBE_CLR(0);

			        }
					else if( 4 == Bit_Choice )
					{
				          if ( (1 == Menu_Setting.Setting_Key_Lock) || (Menu_Setting.System_Reset_Delay >0) ){if(Jump_T == 1){ CHAR4=0;}}

				          TUBE_A0(1);
				          TUBE_A1(1);

				          TUBE_CLR(1);
						  Display_tube(CHAR4);
						  delay_loop(1000);
						  TUBE_CLR(0);
					}

			        Display_Sequence++;

		  	  }
		  	 break;

		      case 2://����ɨ��
			  {
			        if(1 == Bit_Choice)
					{
			          if(!KEY){Menu_Setting.OK_Flag=1;}
			          else{Menu_Setting.OK_Flag= 0;}
			        }
					else if(2 == Bit_Choice)
					{
			          if(!KEY){Menu_Setting.DN_Flag=1;}
			          else{Menu_Setting.DN_Flag= 0;}
			        }
					else if(3 == Bit_Choice)
					{
			          if(!KEY){Menu_Setting.UP_Flag = 1;}
			          else{Menu_Setting.UP_Flag= 0;}
			        }
					else if(4 == Bit_Choice)
					{
			          if(!KEY){Menu_Setting.Setting_Flag = 1;}
			          else{Menu_Setting.Setting_Flag = 0;}
			        }

			        Display_Sequence++;

			  }
			  break;

		      case 3:
			  {
			        TUBE_CLR(1);    //��Ӱ
			        Display_tube(0x00);
					delay_loop(2000);
					TUBE_CLR(0);

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
	        BEEP_A(Enable);//�򿪷���
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
		  	BEEP_A(Disable);
		  }

	      Scan_Key(table[Menu_Setting.Counter/1000],table[Menu_Setting.Counter/100],table[Menu_Setting.Counter/10],table[Menu_Setting.Counter%10]);

    }

    BEEP_A(Disable);
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

/**  ϵͳ��λ ,ͬʱ����OK��Setting������ϵͳ��λ
     uint8_t   Setting_Flag;        //Setting��ֵ��־
     uint8_t   OK_Flag;             //OK
*/
void Scan_Reset_Signal(void)
{
    static uint16_t Reset_Key_Conter=0;
    if((Menu_Setting.Setting_Flag == 1)&&(Menu_Setting.OK_Flag == 1))
    {
        Reset_Key_Conter++;
        if(Reset_Key_Conter>200){   //����Setting���ܰ�����
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
            //��λ����
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
            //������λ

            //��λ��־
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
      if(M_Key_Couter>200)//��OK��3s��������ģʽ
	  {

	        M_Key_Couter=0;
	        Menu_Setting.Mune_Setting_Step=1;   //ģʽ�˵�
	        Menu_Setting.Setting_Key_Lock=1;	//����ģʽ��־
			Menu_Setting.System_Set_Delay = 50000;

      }

      if(1 == Menu_Setting.Setting_Key_Lock)
	  {

	        if(0 == Menu_Setting.Beep_Mode){BEEP_A(Enable);}//������ʾ��

	       	while( ((1 == Menu_Setting.Setting_Flag) && (0 == Menu_Setting.OK_Flag)) || (Menu_Setting.Setting_Key_Delay>10))
			{

		          if(Menu_Setting.Setting_Key_Delay < 10) {Menu_Setting.Setting_Key_Delay++;}
		          else{BEEP_A(Disable);} //������ʾ�ر�

		          Scan_Key(table[Menu_Setting.Mune_Setting_Step], 0 ,table[Menu_Setting.Setting_Counter_Value/10], table[Menu_Setting.Setting_Counter_Value%10]);

	        }

	        //��������˵�Ŀ¼
	        while(1)
	        {

	          switch(Menu_Setting.Mune_Setting_Step)
	          {
	          	case 1://���ü���ֵ  00~99
				{
	            	Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Setting_Counter_Value/10],table[Menu_Setting.Setting_Counter_Value%10]);
	            	if(1 == Menu_Setting.UP_Flag)
					{
	              		if(0 == Menu_Setting.Beep_Mode){BEEP_A(Enable);}
	              		if(Menu_Setting.Setting_Counter_Value < 99){Menu_Setting.Setting_Counter_Value++;}
						Menu_Setting.System_Set_Delay = 50000;
	            	}
					else if(1 == Menu_Setting.DN_Flag)
					{
	              		if(0 == Menu_Setting.Beep_Mode){BEEP_A(Enable);}
	              		if(Menu_Setting.Setting_Counter_Value > 0){Menu_Setting.Setting_Counter_Value--;}
						Menu_Setting.System_Set_Delay = 50000;
	            	}

	            	if(1 == Menu_Setting.Setting_Flag)
					{
						if(0 == Menu_Setting.Beep_Mode)
						{
							BEEP_A(Enable);
						}
						Menu_Setting.Mune_Setting_Step++;
						Menu_Setting.System_Set_Delay = 50000;
					}

	            	while( (1 == Menu_Setting.Setting_Flag) || (1 == Menu_Setting.DN_Flag) || (1 == Menu_Setting.UP_Flag) || (Menu_Setting.Setting_Key_Delay>10) )
					{
		              	if(Menu_Setting.Setting_Key_Delay < 10) {Menu_Setting.Setting_Key_Delay++;}
		              	else{BEEP_A(Disable);}
		              	Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Setting_Counter_Value/10],table[Menu_Setting.Setting_Counter_Value%10]);
	            	}
	            	Menu_Setting.Setting_Key_Delay=0;
	            	BEEP_A(Disable);
	          	}
			  	break;

	          	case 2: //���ü���ģʽ 0�� 1��
				{
	   	            Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Counter_Mode/10],table[Menu_Setting.Counter_Mode%10]);
	            	//����/�ݼ�ģʽ���  00 ����ģʽ , 01�ݼ�ģʽ
	            	if(1 == Menu_Setting.DN_Flag)
					{
	              		if(0 == Menu_Setting.Beep_Mode){BEEP_A(Enable);}
	              		Menu_Setting.Counter_Mode = 0;
						Menu_Setting.System_Set_Delay = 50000;
	            	}
					else if(1 == Menu_Setting.UP_Flag)
					{
	              		if(0 == Menu_Setting.Beep_Mode){BEEP_A(Enable);}
	              		Menu_Setting.Counter_Mode = 1;
						Menu_Setting.System_Set_Delay = 50000;
	            	}
	            	if(1 == Menu_Setting.Setting_Flag){ if(0 == Menu_Setting.Beep_Mode){BEEP_A(Enable);}Menu_Setting.Mune_Setting_Step++;Menu_Setting.System_Set_Delay = 50000;}

	            	while( (1 == Menu_Setting.Setting_Flag) || (1 == Menu_Setting.DN_Flag) || (1 == Menu_Setting.UP_Flag) || (Menu_Setting.Setting_Key_Delay>10)){
	              	if(Menu_Setting.Setting_Key_Delay < 10) {Menu_Setting.Setting_Key_Delay++;}
	              	else{BEEP_A(Disable);}
	              	Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Counter_Mode/10],table[Menu_Setting.Counter_Mode%10]);
	            	}
	            	Menu_Setting.Setting_Key_Delay=0;
	            	BEEP_A(Disable);
	          	}
				break;

	          case 3: //����������
				 {

	            //ɨ�谴������ʾ����
	            Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Beep_Mode/10],table[Menu_Setting.Beep_Mode%10]);
	            //��������������    00����������ģʽ��01�رշ�����ģʽ
	            if(1 == Menu_Setting.DN_Flag){
	              if(0 == Menu_Setting.Beep_Mode){BEEP_A(Enable);}
	              Menu_Setting.Beep_Mode = 0;
				  Menu_Setting.System_Set_Delay = 50000;

	            }else if(1 == Menu_Setting.UP_Flag){
	              if(0 == Menu_Setting.Beep_Mode){BEEP_A(Enable);}
	              Menu_Setting.Beep_Mode = 1;
				  Menu_Setting.System_Set_Delay = 50000;

	            }
	            if(1 == Menu_Setting.Setting_Flag){ if(0 == Menu_Setting.Beep_Mode){BEEP_A(Enable);} Menu_Setting.Mune_Setting_Step++; Menu_Setting.System_Set_Delay = 50000;}

	            //���� ���
	            while( (1 == Menu_Setting.Setting_Flag) || (1 == Menu_Setting.DN_Flag) || (1 == Menu_Setting.UP_Flag) || (Menu_Setting.Setting_Key_Delay>10) ){
	              if(Menu_Setting.Setting_Key_Delay < 10) {Menu_Setting.Setting_Key_Delay++;}
	              else{BEEP_A(Disable);}
	              Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Beep_Mode/10],table[Menu_Setting.Beep_Mode%10]);
	            }
	            Menu_Setting.Setting_Key_Delay=0;
	            BEEP_A(Disable);

	          }break;

	          case 4: //����ʱ
				 {

	            //ɨ�谴������ʾ����
	            Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Vibra_Delay_Setting/10],table[Menu_Setting.Vibra_Delay_Setting%10]);
	            //����ʱ����     01-12s
	            if(1 == Menu_Setting.UP_Flag){
	              if(0 == Menu_Setting.Beep_Mode){BEEP_A(Enable);}
	              if(Menu_Setting.Vibra_Delay_Setting < 12){Menu_Setting.Vibra_Delay_Setting++;Menu_Setting.System_Set_Delay = 50000;}
	            }else if(1 == Menu_Setting.DN_Flag){
	              if(0 == Menu_Setting.Beep_Mode){BEEP_A(Enable);}
	              if(Menu_Setting.Vibra_Delay_Setting > 1){Menu_Setting.Vibra_Delay_Setting--;}
				  Menu_Setting.System_Set_Delay = 50000;

	            }
	            if(1 == Menu_Setting.Setting_Flag){ if(0 == Menu_Setting.Beep_Mode){BEEP_A(Enable);}Menu_Setting.Mune_Setting_Step++;Menu_Setting.System_Set_Delay = 50000;}

	            //���� ���
	            while( (1 == Menu_Setting.Setting_Flag) || (1 == Menu_Setting.DN_Flag) || (1 == Menu_Setting.UP_Flag) || (Menu_Setting.Setting_Key_Delay>10) ){
	              if(Menu_Setting.Setting_Key_Delay < 10) {Menu_Setting.Setting_Key_Delay++;}
	              else{BEEP_A(Disable);}
	              Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Vibra_Delay_Setting/10],table[Menu_Setting.Vibra_Delay_Setting%10]);
	            }
	            Menu_Setting.Setting_Key_Delay=0;
	            BEEP_A(Disable);

	          }break;

	          case 5: //��ǿ��
				 {

	            //ɨ�谴������ʾ����
	            Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Vibra_amplitude_Setting/10],table[Menu_Setting.Vibra_amplitude_Setting%10]);
	            //��ǿ������     01-10 0.5Vһ���ֶ�
	            if(1 == Menu_Setting.UP_Flag){
	              if(0 == Menu_Setting.Beep_Mode){BEEP_A(Enable);}
	              if(Menu_Setting.Vibra_amplitude_Setting < 10){Menu_Setting.Vibra_amplitude_Setting++;}
				  Menu_Setting.System_Set_Delay = 50000;

	            }else if(1 == Menu_Setting.DN_Flag){
	              if(0 == Menu_Setting.Beep_Mode){BEEP_A(Enable);}
	              if(Menu_Setting.Vibra_amplitude_Setting > 1){Menu_Setting.Vibra_amplitude_Setting--;}
				  Menu_Setting.System_Set_Delay = 50000;

	            }
	            if(1 == Menu_Setting.Setting_Flag){ if(0 == Menu_Setting.Beep_Mode){BEEP_A(Enable);}Menu_Setting.Mune_Setting_Step++;Menu_Setting.System_Set_Delay = 50000;}

	            //���� ���
	            while( (1 == Menu_Setting.Setting_Flag) || (1 == Menu_Setting.DN_Flag) || (1 == Menu_Setting.UP_Flag) || (Menu_Setting.Setting_Key_Delay>10) ){
	              if(Menu_Setting.Setting_Key_Delay < 10) {Menu_Setting.Setting_Key_Delay++;}
	              else{BEEP_A(Disable);}
	              Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Vibra_amplitude_Setting/10],table[Menu_Setting.Vibra_amplitude_Setting%10]);
	            }
	            Menu_Setting.Setting_Key_Delay=0;
	            BEEP_A(Disable);

	          }break;

	          case 6:  //������ʱ
				 {

	            //ɨ�谴������ʾ����
	            Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Roller_Delay_Setting/10],table[Menu_Setting.Roller_Delay_Setting%10]);
	            //��Ͳ��ʱ����     01-12s
	            if(1 == Menu_Setting.UP_Flag){
	              if(0 == Menu_Setting.Beep_Mode){BEEP_A(Enable);}
	              if(Menu_Setting.Roller_Delay_Setting < 12){Menu_Setting.Roller_Delay_Setting++;}
				  Menu_Setting.System_Set_Delay = 50000;

	            }else if(1 == Menu_Setting.DN_Flag){
	              if(0 == Menu_Setting.Beep_Mode){BEEP_A(Enable);}
	              if(Menu_Setting.Roller_Delay_Setting > 1){Menu_Setting.Roller_Delay_Setting--;}
				  Menu_Setting.System_Set_Delay = 50000;

	            }
	            if(1 == Menu_Setting.Setting_Flag){ if(0 == Menu_Setting.Beep_Mode){BEEP_A(Enable);}Menu_Setting.Mune_Setting_Step++;Menu_Setting.System_Set_Delay = 50000;}

	            //���� ���
	            while( (1 == Menu_Setting.Setting_Flag) || (1 == Menu_Setting.DN_Flag) || (1 == Menu_Setting.UP_Flag)|| (Menu_Setting.Setting_Key_Delay>10) ){
	              if(Menu_Setting.Setting_Key_Delay < 10) {Menu_Setting.Setting_Key_Delay++;}
	              else{BEEP_A(Disable);}
	              Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Roller_Delay_Setting/10],table[Menu_Setting.Roller_Delay_Setting%10]);
	            }
	            Menu_Setting.Setting_Key_Delay=0;
	            BEEP_A(Disable);

	          }break;

	          case 7:   //����ǿ��
				 {

	            //ɨ�谴������ʾ����
	            Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Roller_amplitude_Setting/10],table[Menu_Setting.Roller_amplitude_Setting%10]);
	            //��Ͳǿ������     01-10 ÿ0.5vһ���ֶ�
	            if(1 == Menu_Setting.UP_Flag){
	              if(0 == Menu_Setting.Beep_Mode){BEEP_A(Enable);}
	              if(Menu_Setting.Roller_amplitude_Setting < 10){Menu_Setting.Roller_amplitude_Setting++;}
				  Menu_Setting.System_Set_Delay = 50000;

	            }else if(1 == Menu_Setting.DN_Flag){
	              if(0 == Menu_Setting.Beep_Mode){BEEP_A(Enable);}
	              if(Menu_Setting.Roller_amplitude_Setting > 1){Menu_Setting.Roller_amplitude_Setting--;}
				  Menu_Setting.System_Set_Delay = 50000;

	            }
	            if(1 == Menu_Setting.Setting_Flag){ if(0 == Menu_Setting.Beep_Mode){BEEP_A(Enable);}Menu_Setting.Mune_Setting_Step++;Menu_Setting.System_Set_Delay = 50000;}

	            //���� ���
	            while( (1 == Menu_Setting.Setting_Flag) || (1 == Menu_Setting.DN_Flag) || (1 == Menu_Setting.UP_Flag)|| (Menu_Setting.Setting_Key_Delay>10) ){
	              if(Menu_Setting.Setting_Key_Delay < 10) {Menu_Setting.Setting_Key_Delay++;}
	              else{BEEP_A(Disable);}
	              Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Roller_amplitude_Setting/10],table[Menu_Setting.Roller_amplitude_Setting%10]);
	            }
	            Menu_Setting.Setting_Key_Delay=0;
	            BEEP_A(Disable);

	          }break;

	          case 8: //ת��ǿ��
				 {

	            //ɨ�谴������ʾ����
	            Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Turnplate_amplitude_Setting/10],table[Menu_Setting.Turnplate_amplitude_Setting%10]);
	            //ת���������    01-10 ������˿�����Ե���
	            if(1 == Menu_Setting.UP_Flag){
	              if(0 == Menu_Setting.Beep_Mode){BEEP_A(Enable);}
	              if(Menu_Setting.Turnplate_amplitude_Setting< 10){Menu_Setting.Turnplate_amplitude_Setting++;}
				  Menu_Setting.System_Set_Delay = 50000;

	            }else if(1 == Menu_Setting.DN_Flag){
	              if(0 == Menu_Setting.Beep_Mode){BEEP_A(Enable);}
	              if(Menu_Setting.Turnplate_amplitude_Setting > 1){Menu_Setting.Turnplate_amplitude_Setting--;}
				  Menu_Setting.System_Set_Delay = 50000;

	            }
	            if(1 == Menu_Setting.Setting_Flag){ if(0 == Menu_Setting.Beep_Mode){BEEP_A(Enable);}Menu_Setting.Mune_Setting_Step++;Menu_Setting.System_Set_Delay = 50000;}

	            //���� ���
	            while( (1 == Menu_Setting.Setting_Flag) || (1 == Menu_Setting.DN_Flag) || (1 == Menu_Setting.UP_Flag)|| (Menu_Setting.Setting_Key_Delay>10)){
	              if(Menu_Setting.Setting_Key_Delay < 10) {Menu_Setting.Setting_Key_Delay++;}
	              else{BEEP_A(Disable);}
	              Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Turnplate_amplitude_Setting/10],table[Menu_Setting.Turnplate_amplitude_Setting%10]);
	            }
	            Menu_Setting.Setting_Key_Delay=0;
	            BEEP_A(Disable);

	          }break;

	          case 9: //�񶯵������
			  {

					//ɨ�谴������ʾ����
					Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Vibra_Direction__Setting/10],table[Menu_Setting.Vibra_Direction__Setting%10]);
					//�񶯷�������   00 ������ �� 01 ������
					if(1 == Menu_Setting.UP_Flag)
					{
						  if(0 == Menu_Setting.Beep_Mode)BEEP_A(Enable);
						  Menu_Setting.Vibra_Direction__Setting = 1;
						  Menu_Setting.System_Set_Delay = 50000;

					}
					if(1 == Menu_Setting.DN_Flag)
					{
						  if(0 == Menu_Setting.Beep_Mode)BEEP_A(Enable);
						  Menu_Setting.Vibra_Direction__Setting = 0;
						  Menu_Setting.System_Set_Delay = 50000;
					}
					if(1 == Menu_Setting.Setting_Flag)
					{
						  if(0 == Menu_Setting.Beep_Mode)BEEP_A(Enable);
						  Menu_Setting.Mune_Setting_Step++;
						  Menu_Setting.System_Set_Delay = 50000;
					}
					//���� ���
					while( (1 == Menu_Setting.Setting_Flag) || (1 == Menu_Setting.DN_Flag) || (1 == Menu_Setting.UP_Flag)|| (Menu_Setting.Setting_Key_Delay>10) )
					{
						  if(Menu_Setting.Setting_Key_Delay < 10)
						  {
							  Menu_Setting.Setting_Key_Delay++;
						  }
						  else
						  {
							  BEEP_A(Disable);
						  }
						  Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Vibra_Direction__Setting/10],table[Menu_Setting.Vibra_Direction__Setting%10]);
	            	}
	            	Menu_Setting.Setting_Key_Delay=0;
	            	BEEP_A(Disable);

	          }break;

			  case 10: //��Ͳ�������
			  {

					//ɨ�谴������ʾ����
					Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Roller_Direction/10],table[Menu_Setting.Roller_Direction%10]);
					//��Ͳ��������   00 ������ �� 01 ������
					if(1 == Menu_Setting.UP_Flag)
					{
						  if(0 == Menu_Setting.Beep_Mode)BEEP_A(Enable);
						  Menu_Setting.Roller_Direction = 1;
						  Menu_Setting.System_Set_Delay = 50000;

					}
					if(1 == Menu_Setting.DN_Flag)
					{
						  if(0 == Menu_Setting.Beep_Mode)BEEP_A(Enable);
						  Menu_Setting.Roller_Direction = 0;
						  Menu_Setting.System_Set_Delay = 50000;
					}
					if(1 == Menu_Setting.Setting_Flag)
					{
						  if(0 == Menu_Setting.Beep_Mode)BEEP_A(Enable);
						  Menu_Setting.Mune_Setting_Step++;
						  Menu_Setting.System_Set_Delay = 50000;
					}
					//���� ���
					while( (1 == Menu_Setting.Setting_Flag) || (1 == Menu_Setting.DN_Flag) || (1 == Menu_Setting.UP_Flag)|| (Menu_Setting.Setting_Key_Delay>10) )
					{
						  if(Menu_Setting.Setting_Key_Delay < 10)
						  {
							  Menu_Setting.Setting_Key_Delay++;
						  }
						  else
						  {
							  BEEP_A(Disable);
						  }
						  Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Roller_Direction/10],table[Menu_Setting.Roller_Direction%10]);
	            	}
	            	Menu_Setting.Setting_Key_Delay=0;
	            	BEEP_A(Disable);

	          }break;

			  case 11: //ת�̵������
			  {

					//ɨ�谴������ʾ����
					Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Turnplate_Direction/10],table[Menu_Setting.Turnplate_Direction%10]);
					//�񶯷�������   00 ������ �� 01 ������
					if(1 == Menu_Setting.UP_Flag)
					{
						  if(0 == Menu_Setting.Beep_Mode)BEEP_A(Enable);
						  Menu_Setting.Turnplate_Direction = 1;
						  Menu_Setting.System_Set_Delay = 50000;

					}
					if(1 == Menu_Setting.DN_Flag)
					{
						  if(0 == Menu_Setting.Beep_Mode)BEEP_A(Enable);
						  Menu_Setting.Turnplate_Direction = 0;
						  Menu_Setting.System_Set_Delay = 50000;
					}
					if(1 == Menu_Setting.Setting_Flag)
					{
						  if(0 == Menu_Setting.Beep_Mode)BEEP_A(Enable);
						  Menu_Setting.Mune_Setting_Step++;
						  Menu_Setting.System_Set_Delay = 50000;
					}
					//���� ���
					while( (1 == Menu_Setting.Setting_Flag) || (1 == Menu_Setting.DN_Flag) || (1 == Menu_Setting.UP_Flag)|| (Menu_Setting.Setting_Key_Delay>10) )
					{
						  if(Menu_Setting.Setting_Key_Delay < 10)
						  {
							  Menu_Setting.Setting_Key_Delay++;
						  }
						  else
						  {
							  BEEP_A(Disable);
						  }
						  Scan_Key(table[Menu_Setting.Mune_Setting_Step],0,table[Menu_Setting.Turnplate_Direction/10],table[Menu_Setting.Turnplate_Direction%10]);
	            	}
	            	Menu_Setting.Setting_Key_Delay=0;
	            	BEEP_A(Disable);

	          }break;

	          default:
				 break;

	          }

	          //ɨ��˵���
	          if((1 == Menu_Setting.OK_Flag )|| (Menu_Setting.Mune_Setting_Step>11)||Menu_Setting.System_Set_Delay <= 0)
			  {

	            	Menu_Setting.OK_Flag=0;
	            	Menu_Setting.Setting_Key_Lock=0;
	            	Menu_Setting.Setting_Flag=0;

	            	//�����������ݣ��˳��˵�
	            	if(Menu_Setting.Mune_Setting_Step<=11)
					{
	              		Write_Parameters_to_flash();
						delay_loop(1000);
	            	}

	            	if(0 == Menu_Setting.Beep_Mode)
					{
	              		BEEP_A(Enable);
	              		Menu_Setting.Setting_Key_Delay=0;
	            	}

					Write_Parameters_to_flash();

	            	break; //����forѭ��
	          }

	        }
      }
    }else{

      M_Key_Couter=0;
    }
}

void main(void)
{
  static uint8_t RollStopFlag = 1;
  static uint8_t VibraStopFlag = 1;
  static uint8_t TurnStopFlag = 1;

  Bsp_Inital();
  Read_Parameters_from_flash();

  while(1)
  {

       Ok_Key_Process();
       Setting_Key_Process();
       Scan_Reset_Signal();

	   if((GATE == 0)&&(RollStopFlag)&&(VibraStopFlag)&&(TurnStopFlag))
	   {
		    TIM3->CR1 |= (uint8_t)0x01;

			Roll_Motor_Delay =  (uint32_t)Menu_Setting.Roller_Delay_Setting * 1000 * 10;  //���ù�Ͳ��ʱ
			Roll_Motor_Speed =  Menu_Setting.Roller_amplitude_Setting;                    //���ù�Ͳ�ٶ�
			ROLL_DIR(Menu_Setting.Roller_Direction);                                      //���÷���
            ROLL_POWER(1); //��Ͳ���
			RollStopFlag=0;


			Vibra_Motor_Delay = (uint32_t)Menu_Setting.Vibra_Delay_Setting * 1000 * 10; //��������ʱ
			Vibra_Motor_Speed = Menu_Setting.Vibra_amplitude_Setting;                   //�������ٶ�
			VIBRA_DIR(Menu_Setting.Vibra_Direction__Setting);                           //���÷���
            VIBRA_POWER(1); //�񶯵��
			VibraStopFlag=0;

			Turnplate_Motor_Speed = Menu_Setting.Turnplate_amplitude_Setting;           //����ת���ٶ�
			TURN_DIR(Menu_Setting.Turnplate_Direction);                                //���÷���
			TURN_POWER(1); //ת�̵��
			TurnStopFlag=0;

			Menu_Setting.Counter++;
			if(Menu_Setting.Counter >= Menu_Setting.Setting_Counter_Value*100 )
			{
				Menu_Setting.Counter = 5;
			}
	   }

	   if(Roll_Motor_Delay <= 0)
	   {
	   		ROLL_POWER(0);
	   		RollStopFlag = 1;
	   }

	   if(Vibra_Motor_Delay <= 0)
	   {
	   		VIBRA_POWER(0);
	   		VibraStopFlag = 1;
	   }

   	   if(SENSOR == 0)
	   {
			TURN_POWER(0);
			TurnStopFlag = 1;
	   }

	   if(RollStopFlag && VibraStopFlag && TurnStopFlag)
	   {
	   		TIM3->CR1 &= ~((uint8_t)0x01);
	   }
  }

}
