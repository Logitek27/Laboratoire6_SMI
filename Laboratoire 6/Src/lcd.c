/*
 * lcd.c
 *
 *  Created on: 27 sept. 2018
 *      Author: sebma
 */

#include "stm32f4xx_hal.h"
#include "main.h"
#include "lcd.h"
#include "tim.h"

char positionLine2 = 0;  //position of the next available dot on the 2nd line

void WriteCommand(int s32Command)
{		
	/*
	 * Set each data pin then enable data transfer.
	 */
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_9, (GPIO_PinState)((s32Command & 0x0200)>>9));  //RS
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_11,(GPIO_PinState)((s32Command & 0x0100)>>8));	//RW
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_15,(GPIO_PinState)((s32Command & 0x0080)>>7));  //DB7
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,(GPIO_PinState)((s32Command & 0x0040)>>6));	//DB6
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_11,(GPIO_PinState)((s32Command & 0x0020)>>5));	//DB5
	HAL_GPIO_WritePin(GPIOD,GPIO_PIN_9, (GPIO_PinState)((s32Command & 0x0010)>>4));	//DB4
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,(GPIO_PinState)((s32Command & 0x0008)>>3));	//DB3
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,(GPIO_PinState)((s32Command & 0x0004)>>2));	//DB2
	HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11,(GPIO_PinState)((s32Command & 0x0002)>>1));	//DB1
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_15,(GPIO_PinState)((s32Command & 0x0001)>>0));	//DB0
	

	//Enable
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_13,GPIO_PIN_SET);
	BoucleDAttente(100);  //wait 1000µs
	HAL_GPIO_WritePin(GPIOE,GPIO_PIN_13,GPIO_PIN_RESET);
	
}


void InitLCD()
{
	WriteCommand(0x001); //Clear display
	WriteCommand(0x002); //place cursor at the beginning
	WriteCommand(0x038); //function set 8-bits operation 2 lines display 5x8 character font
	WriteCommand(0x00C); // display on, cursor off, blink off
	WriteCommand(0x006); //Increment, display shift
	WriteCommand(0x24E); //write 'N'
	WriteCommand(0x24C); //write 'L'
	WriteCommand(0x25F); //write '_'
	WriteCommand(0x253); //write 'S'
	WriteCommand(0x24D); //write 'M'
	WriteCommand(0x220); //write ' ' (space)
	
}

void ClearLine2LCD()
{
	/* 
	*  Clear every character on the 2nd line
	*/
	char u8Cnt = 0;

	WriteCommand(0x00C0); // set DDRAM address to 40: moves cursor to the start of 2nd line.
	for(u8Cnt=0;u8Cnt<16;u8Cnt++)     //clear 2nd line
	{
		WriteCommand(0x0220);  //write ' ' (space)
	}
	WriteCommand(0x00C0); // set DDRAM address to 40: moves cursor to the start of 2nd line.
	
	positionLine2 = 0;
}

void WriteNumberLCD(char u8Number)
{
	if(u8Number==((char)0xFF)){  //Clear line command
		ClearLine2LCD();
	}
	else if (u8Number < 10){     //button 0 to 9
		WriteCommand(0x0230 + u8Number);
	}
}

void WriteAsciiCharacterLCD(char asciiChar){
	if(positionLine2 < 16){
		WriteCommand(0x00C0 + positionLine2); // set DDRAM address to the second line, on the next available dot
		WriteCommand(0x0200 + asciiChar);   // Write character
		positionLine2++;
	}
}

void WriteTimeLCD(unsigned int time)
/* displays the number of seconds from the start of the CPU on the first line */
{
	char u8Cnt = 0;
	unsigned int timeCopy = time%100000;
	char time4, time3, time2, time1, time0;
	
	WriteCommand(0x0080+6); //set cursor to the 6th dot of the 1st line
	for(u8Cnt=0;u8Cnt<10;u8Cnt++)     //clear line after the 5th dot
	{
		WriteCommand(0x0220);
	}
	WriteCommand(0x0080+6); //set back cursor to the 6th dot of the 1st line
	
	/*extract each digits of the number stored in time*/
	time4 = (char)(timeCopy/10000);
	timeCopy -= (unsigned int)(time4)*10000;
	time3 = (char)(timeCopy/1000);
	timeCopy -= (unsigned int)(time3)*1000;
	time2 = (char)(timeCopy/100);
	timeCopy -= (unsigned int)(time2)*100;
	time1 = (char)(timeCopy/10);
	timeCopy -= (unsigned int)(time1)*10;
	time0 = (char)(timeCopy);
	
	/*display the value time*/
	WriteNumberLCD(time4);
	WriteNumberLCD(time3);
	WriteNumberLCD(time2);
	WriteNumberLCD(time1);
	WriteNumberLCD(time0);

}

void Affichedouble(double flottant){  //affiche double
	flottant = flottant-0.1;						//suppress bias
	char IntPart = (char)flottant;      //get Integer part
	WriteNumberLCD(IntPart);						//write Integer part
	double tenthf = (double)((flottant-IntPart)*10);  //get tenth part
	int tenth = (int)(tenthf);
	WriteCommand(0x22E); 														//write "."
	WriteNumberLCD(tenth);													//write tenth part
}

void ClearNbCharacterLine2LCD(char Nb)
{
	/* 
	*  Clear every character on the 2nd line
	*/
	char u8Cnt = 0;

	WriteCommand(0x00C0); // set DDRAM address to 40: moves cursor to the start of 2nd line.
	for(u8Cnt=0;u8Cnt<Nb;u8Cnt++)     //clear 2nd line
	{
		WriteCommand(0x0220);  //write ' ' (space)
	}
	WriteCommand(0x00C0); // set DDRAM address to 40: moves cursor to the start of 2nd line.
	
	positionLine2 = 0;
}

void AfficheRMS(double flottant){
	ClearNbCharacterLine2LCD(5);  //clear 5 character at the end of the 2nd line
	Affichedouble(flottant);			//write flottant on the screen, format : A.AA
	WriteCommand(0x256); 					//write V (Volts)
}
