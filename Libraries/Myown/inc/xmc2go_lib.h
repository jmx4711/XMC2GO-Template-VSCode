/*
 * XMC2GO_lib.h
 *
 *  Created on: 03. Maerz 2017 / März24
 *      Author: Reinhold Birk
 *
 */

#ifndef XMC2GO_LIB_V2_H_
#define XMC2GO_LIB_V2_H_

//
// Bibliothek:       XMC11002GO_lib.h
// Controller:       XMC1100
// Version:          1.0
// erstellt am:      03.3.2024

#include "xmc1100.h"
//*******************************************************************************
// Globale Schalter und Symbole
// ... fuer Bit- und Byte-I/O
//#define NOPULLUPS
#define INP 0   // Input
#define OUTP 1
// Namen (Aliase) fuer Ports
enum ports {P0,P1,P2};  // PORT0 = 0, PORT1 = 1, ....
// ext.Interrupt
#define RE 1 // flanke=RE=1 Rising Edge ansteigende Flanke
#define FE 0 // flanke=FE=0 Falling Edge abfallende Flanke
// Interrupt UARTs
#define RE_INT 1	// ReceiveEnable Interrupt
#define NO_RE_INT 0	// No ReceiveEnable Interrupt
// ... fuer PWM (XMC1100)
//#define PWM_PIN 0           //      P0.6 ist PWM Ausgang1

//*******************************************************************************
// Funktionsprototypen fuer Bit- und Byte Ein-/Ausgabe
extern void     bit_init   ( uint8_t port, uint8_t bitnr, uint8_t direction);
extern void     bit_init_range  ( uint8_t port, uint8_t startBitNr, uint8_t endBitNr, uint8_t direction);
extern uint8_t  bit_read   ( uint8_t port, uint8_t bitnr);
extern void     bit_write  ( uint8_t port, uint8_t bitnr, uint8_t value);

extern void     port_init  ( uint8_t port, uint8_t direction);
extern uint16_t port_read  ( uint8_t port );
extern void     port_write ( uint8_t port, uint16_t value);

// Funktionsprototypen fuer PWM
extern void    pwm0_init   ( void );	// PWM an P0.6
extern void    pwm0_start_interrupt(void); //mit Interrupt bei jeder Periode
extern void    pwm0_start  ( void );
extern void    pwm0_stop   ( void );
extern void    pwm0_duty_cycle ( uint16_t value);
extern void    pwm0_duty_cycle_period (uint8_t vorteiler, uint16_t compare, uint16_t period);
extern void    pwm1_init   ( void );	// PWM an P0.7
extern void    pwm1_start_interrupt(void); //mit Interrupt bei jeder Periode
extern void    pwm1_start  ( void );
extern void    pwm1_stop   ( void );
extern void    pwm1_duty_cycle ( uint16_t value);
extern void    pwm1_duty_cycle_period (uint8_t vorteiler, uint16_t compare, uint16_t period);
extern void    pwm2_init   ( void );	//PWM an P0.8
extern void    pwm2_start_interrupt(void); //mit Interrupt bei jeder Periode
extern void    pwm2_start  ( void );
extern void    pwm2_stop   ( void );
extern void    pwm2_duty_cycle ( uint16_t value);
extern void    pwm2_duty_cycle_period (uint8_t vorteiler, uint16_t compare, uint16_t period);

// Funktionsprototypen fuer AD-Wandler
extern void    adc_init   ( void );
extern uint16_t adc_in	(uint8_t kanal);

// DELAY:
#define _T_CONST_ 109  // zuletzt 104, geändert 109 Bub Konstante fuer XMC1100 mit Default Frequ 32 MHz
					  // fuer 100 uSek
extern void delay_10us (uint8_t mikrosekunden10);
extern void delay_100us( uint8_t mikrosekunden100);
extern void delay_ms( uint16_t millisekunden);
//*************************************************************
// Timer Funktionen 1ms: mit CCU40_CC43
extern void timer1ms_init ( void );
extern void timer1ms_enable ( void );
extern void timer1ms_disable ( void );
//extern void timer1ms_isr(void);
//*************************
// Timer und Zeitmessfunktion mit CCU40_CC43:
extern void timer_init(uint8_t vorteiler, uint16_t periode);
extern void timer_start(void);
extern void timer_start_interrupt(void);
extern void timer_stop(void);
extern uint16_t timer_value(void); 	 // Rückgabe => Timer Wert
extern void timer_reset(void); 		// Timer rücksetzen, früher in timer_value enthalten
extern void timer_waitFlag(void); 	// wartet auf Flag; Reset Flag
//*************************************************************
//Globale Definitionen
// ... fuer I2C-Funktionen XMC2GO :
// Variable  Hier : P2.10 => SDA
//				    P2.11 => SCL


#define I2C_PORT_OUT PORT2->OUT
#define I2C_PORT_IN  PORT2->IN
#define I2C_DIR  PORT2->IOCR8

#define SDA  10       // Bit P2.10
#define SCL  11       // Bit P2.11
#define SDA_OUT 0x00c00000	// Out mit OPEN DRAIN an P2.10
#define SDA_IN  0xff00ffff
#define SCL_OUT 0x80000000	// Out push/pull, da kein Multi Master System an P2.11

#define ACK  0
#define NACK 1

//*************************************************************
// Funktionsprototypen fuer I2C-Busschnittstelle (Master-Mode)
extern void    i2c_init    ( void );
extern void    i2c_start   ( void );
extern void    i2c_stop    ( void );
extern uint8_t i2c_write   ( uint8_t value);     // R�ckgabewert ACK / NACK
extern uint8_t i2c_read    ( uint8_t ack );      // R�ckgabewert Value

//*************************************************************
// Funktionsprototypen fuer RS232-Schnittstelle

extern void    uart1_init  (uint32_t baudrate, uint8_t enable_interrupt); // Wahl Baudrate und ob Interrupt freigeben wird
extern void    uart0_init  (uint32_t baudrate, uint8_t enable_interrupt);
extern uint8_t uart1_get   (void);										  // 1 Zeichen empfangen
extern uint8_t uart0_get   (void);
extern void    uart1_put   (uint8_t value);								  // 1 Zeichen senden
extern void    uart0_put   (uint8_t value);
extern void    uart1_print (uint8_t *text);							 	  // String senden
extern void    uart0_print (uint8_t *text);
extern uint8_t uart1_char_received (void);								  // Abfrage, ob Zeichen vorhanden, wenn kein Interruptbetrieb
extern uint8_t uart0_char_received (void);

// Interrupt Einstellungen
extern void ext_interrupt_init ( void ); //bei fallender Flanke an P2.0, steigende Flanke an P2.7
extern void ext_interrupt0_init ( uint8_t flanke );
extern void ext_interrupt1_init ( uint8_t flanke );
extern void ext_interrupt0_enable ( void );
extern void ext_interrupt1_enable ( void );
//extern void ext_interrupt1_isr (void );
//extern void ext_interrupt2_isr (void );
extern void ext_interrupt0_disable( void );
extern void ext_interrupt1_disable( void );

// LCD - Routinen für OLED 0.91 inch Display
// bei font16 Pixel => 2 x 11 Zeichen möglich
extern void lcd_init      ( void );                            // Initialisierung des Displays
extern void lcd_clear     ( void );
extern void lcd_setcursor ( uint8_t row, uint8_t column );     // Setzen der Cursorposition
                                                               // row (Zeile) = 1,2, ...
                                                               // column (Spalte) = 1,2,3, ...
extern void lcd_print ( uint16_t zeile,uint16_t spalte ,char  text[] );// Textausgabe ab Cursorposition, bis '\0'
extern void lcd_char  ( uint16_t zeile,uint16_t spalte,char  value );// Ausgabe eines Zeichens an der aktuellen Cursorposition
extern void lcd_byte ( uint16_t zeile,uint16_t spalte,uint8_t  value  ); // 0 ... 255
extern void lcd_int( uint16_t zeile,uint16_t spalte,uint16_t val);           // 0 ... 65535

//**********************************************************************************

#define UBYTE   uint8_t
#define UWORD   uint16_t
#define UDOUBLE uint32_t

void OLED_0in91_Init(void);
void OLED_0in91_Clear(void);
void OLED_0in91_Display(const UBYTE *Image);



#define USE_IIC     1
#define SSD1306_SLAVE_ADDR_WR 0x78



#define OLED_0in91_WIDTH  128//OLED width
#define OLED_0in91_HEIGHT 32 //OLED height

#define IIC_CMD        0X00
#define IIC_RAM        0X40

#define _T_CONST_ 109  // zuletzt 104
/*------------------------------------------------------------------------------------------------------*/
void I2C_Write_Byte(uint8_t value, uint8_t Cmd);

void Driver_Delay_ms(uint16_t xms);
void Driver_Delay_us(int xus);
void delay( uint16_t millisekunden);
//void delay_100us (uint8_t n);
/*******************************************************
 * Image attributes
*********************************************/
typedef struct {
    UBYTE *Image;
    UWORD Width;
    UWORD Height;
    UWORD WidthMemory;
    UWORD HeightMemory;
    UWORD Color;
    UWORD Rotate;
    UWORD Mirror;
    UWORD WidthByte;
    UWORD HeightByte;
    UWORD Scale;
} PAINT;
extern PAINT Paint;

#define MAX_HEIGHT_FONT         41
#define MAX_WIDTH_FONT          32
#define OFFSET_BITMAP			0

/**
 * Display rotate
**/
#define ROTATE_0            0
#define ROTATE_90           90
#define ROTATE_180          180
#define ROTATE_270          270

/**
 * Display Flip
**/
typedef enum {
    MIRROR_NONE  = 0x00,
    MIRROR_HORIZONTAL = 0x01,
    MIRROR_VERTICAL = 0x02,
    MIRROR_ORIGIN = 0x03,
} MIRROR_IMAGE;
#define MIRROR_IMAGE_DFT MIRROR_NONE

/**
 * image color
**/

#define WHITE          0xFFFF
#define BLACK          0x0000
#define BLUE           0x001F
#define BRED           0XF81F
#define GRED           0XFFE0
#define GBLUE          0X07FF
#define RED            0xF800
#define MAGENTA        0xF81F
#define GREEN          0x07E0
#define CYAN           0x7FFF
#define YELLOW         0xFFE0
#define BROWN          0XBC40
#define BRRED          0XFC07
#define GRAY           0X8430

#define IMAGE_BACKGROUND    WHITE
#define FONT_FOREGROUND     BLACK
#define FONT_BACKGROUND     WHITE

//4 Gray level
#define  GRAY1 0x03 //Blackest
#define  GRAY2 0x02
#define  GRAY3 0x01 //gray
#define  GRAY4 0x00 //white
/**
 * The size of the point
**/
typedef enum {
    DOT_PIXEL_1X1  = 1, // 1 x 1
    DOT_PIXEL_2X2  ,    // 2 X 2
    DOT_PIXEL_3X3  ,    // 3 X 3
    DOT_PIXEL_4X4  ,    // 4 X 4
    DOT_PIXEL_5X5  ,    // 5 X 5
    DOT_PIXEL_6X6  ,    // 6 X 6
    DOT_PIXEL_7X7  ,    // 7 X 7
    DOT_PIXEL_8X8  ,    // 8 X 8
} DOT_PIXEL;
#define DOT_PIXEL_DFT  DOT_PIXEL_1X1  //Default dot pilex

/**
 * Point size fill style
**/
typedef enum {
    DOT_FILL_AROUND  = 1,   // dot pixel 1 x 1
    DOT_FILL_RIGHTUP  ,     // dot pixel 2 X 2
} DOT_STYLE;
#define DOT_STYLE_DFT  DOT_FILL_AROUND  //Default dot pilex

/**
 * Line style, solid or dashed
**/
typedef enum {
    LINE_STYLE_SOLID = 0,
    LINE_STYLE_DOTTED,
} LINE_STYLE;

/**
 * Whether the graphic is filled
**/
typedef enum {
    DRAW_FILL_EMPTY = 0,
    DRAW_FILL_FULL,
} DRAW_FILL;

/**
 * Custom structure of a time attribute
**/
typedef struct {
    UWORD Year;  //0000
    UBYTE Month; //1 - 12
    UBYTE Day;   //1 - 30
    UBYTE Hour;  //0 - 23
    UBYTE Min;   //0 - 59
    UBYTE Sec;   //0 - 59
} PAINT_TIME;
extern PAINT_TIME sPaint_time;

//ASCII
typedef struct _tFont
{
  const uint8_t *table;
  uint16_t Width;
  uint16_t Height;

} sFONT;
// Für Font16 gilt :
#define xpos16 11
#define ypos16 16

//GB2312
typedef struct                                          // 汉字字模数据结构
{
  unsigned char index[3];                               // 汉字内码索引
  const char matrix[MAX_HEIGHT_FONT*MAX_WIDTH_FONT/8];  // 点阵码数据
}CH_CN;


typedef struct
{
  const CH_CN *table;
  uint16_t size;
  uint16_t ASCII_Width;
  uint16_t Width;
  uint16_t Height;

}cFONT;

//extern sFONT Font24;
//extern sFONT Font20;
extern sFONT Font16;
//extern sFONT Font12;
//extern sFONT Font8;


//init and Clear
void Paint_NewImage(UBYTE *image, UWORD Width, UWORD Height, UWORD Rotate, UWORD Color);
void Paint_SelectImage(UBYTE *image);
void Paint_SetRotate(UWORD Rotate);
void Paint_SetMirroring(UBYTE mirror);
void Paint_SetPixel(UWORD Xpoint, UWORD Ypoint, UWORD Color);
void Paint_SetScale(UBYTE scale);

void Paint_Clear(UWORD Color);
void Paint_ClearWindows(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD Color);

//Drawing
void Paint_DrawPoint(UWORD Xpoint, UWORD Ypoint, UWORD Color, DOT_PIXEL Dot_Pixel, DOT_STYLE Dot_FillWay);
void Paint_DrawLine(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD Color, DOT_PIXEL Line_width, LINE_STYLE Line_Style);
void Paint_DrawRectangle(UWORD Xstart, UWORD Ystart, UWORD Xend, UWORD Yend, UWORD Color, DOT_PIXEL Line_width, DRAW_FILL Draw_Fill);
void Paint_DrawCircle(UWORD X_Center, UWORD Y_Center, UWORD Radius, UWORD Color, DOT_PIXEL Line_width, DRAW_FILL Draw_Fill);

//Display string
void Paint_DrawChar(UWORD Xstart, UWORD Ystart, const char Acsii_Char, sFONT* Font, UWORD Color_Foreground, UWORD Color_Background);
void Paint_DrawString_EN(UWORD Xstart, UWORD Ystart, const char * pString, sFONT* Font, UWORD Color_Foreground, UWORD Color_Background);
void Paint_DrawString_CN(UWORD Xstart, UWORD Ystart, const char * pString, cFONT* font, UWORD Color_Foreground, UWORD Color_Background);
void Paint_DrawNum(UWORD Xpoint, UWORD Ypoint, const char * Number,  sFONT* Font, UWORD Digit,UWORD Color_Foreground, UWORD Color_Background);
void Paint_DrawTime(UWORD Xstart, UWORD Ystart, PAINT_TIME *pTime, sFONT* Font, UWORD Color_Foreground, UWORD Color_Background);

//pic
void Paint_DrawBitMap(const unsigned char* image_buffer);
//void Paint_DrawBitMap_Half(const unsigned char* image_buffer, UBYTE Region);
//void Paint_DrawBitMap_OneQuarter(const unsigned char* image_buffer, UBYTE Region);
//void Paint_DrawBitMap_OneEighth(const unsigned char* image_buffer, UBYTE Region);
void Paint_DrawBitMap_Block(const unsigned char* image_buffer, UBYTE Region);


#endif /* XMC1100_LIB_H_ */
