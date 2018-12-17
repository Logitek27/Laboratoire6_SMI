/*
 * lcd.h
 *
 *  Created on: 27 sept. 2018
 *      Author: sebma
 */

#ifndef LCD_H_
#define LCD_H_

void WriteCommand(int s32Command);
void InitLCD(void);
void WriteNumberLCD(char number);
void WriteAsciiCharacterLCD(char asciiChar);
void ClearLine2LCD(void);
void WriteTimeLCD(unsigned int);
void AfficheFloat(float flottant);

#endif /* LCD_H_ */
