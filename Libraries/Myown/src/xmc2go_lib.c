


// Bibliothek:       XMC2GO_LIB
// Controller:       XMC1100
// Version:          1.0
// letzte Aenderung: 22.4.24 => neue Lib zusammengefasst
//
//
#include <xmc1100.h>
#include <stdlib.h>
#include <xmc_usic.h>
#include <xmc2go_lib.h>
//Definition der lokalen Funktionen:
uint8_t *BlackImage;
uint16_t bit_set_clr(uint16_t wert, uint16_t set ,uint16_t bitnr);
void clock_init(void);
void XMC_USIC_CH_SetzeBaudrate(XMC_USIC_CH_t *const channel, uint32_t rate);

PAINT Paint;


//***************************************************************
// ab hier Funktionen f�r Port-Ein-/Ausgabe
//***************************************************************

// --------------------------------------------------------------
// Einzelnes Bit f�r Ein- oder Ausgabe initialisieren
// port: P0,P1,P2 bitnr: 0..15 direction: INP 0, OUTP 1
//---------------------------------------------------------------
void bit_init(uint8_t port, uint8_t bitnr, uint8_t direction)
{
	switch (port)
	{
		case 0 :
			{if (bitnr < 4 )
				{ if (direction == 1) PORT0->IOCR0 |=1UL << ((bitnr * 8)+7);
				  else PORT0->IOCR0 &= ~(1UL << ((bitnr * 8 ) + 7 ));
				}
			 if ( (bitnr <8) && (bitnr > 3))
					 { if (direction == 1) PORT0->IOCR4 |= 1UL<< (((bitnr-4) * 8)+7);
					   else PORT0->IOCR4 &= ~(1UL << (((bitnr-4) * 8)+7) );
					 }
			 if ( (bitnr <12) && (bitnr > 7))
			 		 { if (direction == 1) PORT0->IOCR8 |= 1UL << (((bitnr-8) * 8)+7);
			 		   else PORT0->IOCR8 &= ~(1UL << (((bitnr-8) * 8)+7) );
			 		 }
			 if ( (bitnr <16) && (bitnr > 11))
			 		{ if (direction == 1) PORT0->IOCR12 |= 1UL << (((bitnr-12) * 8)+7);
			 		  else PORT0->IOCR12 &= ~(1UL << (((bitnr-12) * 8)+7) );
			 		}
			}
			break;
	  case 1 :
	  {
			{
				if (bitnr < 4 )
				{ if (direction == 1) PORT1->IOCR0 |=1UL << ((bitnr * 8)+7);
						else PORT1->IOCR0 &= ~(1UL << ((bitnr * 8 ) + 7 ));
				}
				if ( (bitnr <8) && (bitnr > 3))
				{ if (direction == 1) PORT1->IOCR4 |= 1UL<< (((bitnr-4) * 8)+7);
					else PORT1->IOCR4 &= ~(1UL << (((bitnr-4) * 8)+7) );
				}
			}
		}
		break;

	  case 2 :
	  	  	 {
	                 if ((bitnr == 9) && (direction == 0))
	                     {
	                     // P2.9 als dig. Input
	                     CLR_BIT(PORT2->PDISC,9); // P2.9 => dig. Input
	                     PORT2->IOCR8 &= 0xffff00ff;  // P2.9 => Input
	                     }
	                 if ((bitnr == 10) && (direction == 0))
	                     {
	                     // P2.10 als dig. Input
	                     CLR_BIT(PORT2->PDISC,10);    // P2.10 => dig. Input
	                     PORT2->IOCR8 &= 0xff00ffff;  // P2.10 => Input
	                     }
	                 if ((bitnr == 11) && (direction == 0))
	                     {
	                     // P2.11 als dig. Input
	                     CLR_BIT(PORT2->PDISC,11);    // P2.11 => dig. Input
	                     PORT2->IOCR8 &= 0x00ffffff;  // P2.11 => Input
	                     }
	                 if ((bitnr == 2) && (direction == 0))
	                     {
	                     // P2.2 als dig. Input
	                     CLR_BIT(PORT2->PDISC,2); // P2.2 => dig. Input
	                     PORT2->IOCR0 &= 0xff00ffff;  // P2.2 => Input
	                     }
	             }
	  			break;
	  	}
}
// --------------------------------------------------------------
// Bereich von Bits f�r Ein- oder Ausgabe initialisieren
// port: P0,P1,P2 startBitNr: 0..15 endBitNr: 0..15 direction: INP 0, OUTP 1
//---------------------------------------------------------------
void bit_init_range (uint8_t port, uint8_t startBitNr, uint8_t endBitNr, uint8_t direction)
{
	  uint16_t i;

	  if(endBitNr<startBitNr)
		  endBitNr = startBitNr;

	  for(i=startBitNr; i<=endBitNr; i++)
		  bit_init(port,i,direction);
}

//---------------------------------------------------------------
//Gesamtes Port f�r Ein- oder Ausgabe initialisieren
// port: P0,P1 direction: INP 0, OUTP 1
//---------------------------------------------------------------
 void port_init(uint8_t port, uint8_t direction)
{
	switch(port)
	{
		case 0:	if (direction == OUTP) {
					PORT0->IOCR0 = 0x80808080;  // auf Ausgang setzen
					PORT0->IOCR4 = 0x80808080;
					PORT0->IOCR8 = 0x80808080;
					PORT0->IOCR12 = 0x80808080;
					}
				else {
					PORT0->IOCR0 = 0x00;
					PORT0->IOCR4 = 0x00;
					PORT0->IOCR8 = 0x00;
					PORT0->IOCR12 = 0x00;
					}
				break;
		case 1:	 if (direction == OUTP) {
					PORT1->IOCR0 = 0x80808080;  // auf Ausgang setzen
					PORT1->IOCR4 = 0x80808080;
								}
				else {
					PORT1->IOCR0 = 0x00;
					PORT1->IOCR4 = 0x00;
			}
			break;
	}
}

//---------------------------------------------------------------
// Einzelnes Portbit einlesen
// port: P0,P1,P2 bitnr: 0..15
//---------------------------------------------------------------
uint8_t bit_read(uint8_t port, uint8_t bitnr)
{
  uint16_t temp;

  temp = port_read(port);
  return ((temp>>bitnr) & 0x01);
}
//---------------------------------------------------------------
// Einzelnes Portbit ausgeben
// port: P0,P1 bitnr: 0..15 P2=>bitnr 9..11
//---------------------------------------------------------------
void bit_write(uint8_t port, uint8_t bitnr, uint8_t value)
{
	value &= 0x01;       // value darf nur 0 oder 1 sein!
	switch (port)
	{
		case 0 :
			{if (value == 1) PORT0->OUT |= 1UL << bitnr ;
			else PORT0->OUT &= ~( 1UL << bitnr);
			}
			break;
	    case 1 :
	  	  	  {if (value == 1) PORT1->OUT |= 1UL << bitnr ;
	  		   else PORT1->OUT &= ~( 1UL << bitnr);
	  		  }
			break;

		}
 }
//---------------------------------------------------------------
// Gesamtes Port einlesen
// port: P0,P1,P2
//---------------------------------------------------------------
uint16_t port_read (uint8_t port)
{
  uint16_t temp;
  switch (port)
  {
    case 0: temp = PORT0->IN; return temp; break;
    case 1: temp = PORT1->IN; return temp; break;
    case 2: temp = PORT2->IN; return temp; break;
    default: return 0; break;
  }
  //return 0;
}

//---------------------------------------------------------------
// Gesamtes Port ausgeben
// port: P0,P1
//---------------------------------------------------------------
void port_write(uint8_t port, uint16_t value)
{
  switch (port)
  {
    case 0: PORT0->OUT  = value; break;
    case 1: PORT1->OUT = value; break;

  }
}
// ***************************************************************
// ab hier PWM-Funktionen
// ***************************************************************

#define periode_pwm 50000 // fuer ca. 640 Hz

void clock_init(void)
{
	SCU_GENERAL->PASSWD = 0x000000C0UL;
	SCU_CLK->CLKCR = 0x3FF10100;			// Config SCU Clock = 32MHz , PCLK = 64 MHz
	while((SCU_CLK->CLKCR)&0x40000000UL);	// wait for VDDC to stabilize
	SCU_GENERAL->PASSWD = 0x000000C3UL;
	SCU_GENERAL->PASSWD = 0x000000C0UL;
	SCU_CLK->CGATCLR0 |= 0x04;				// Disable CCU4 Gating
	while((SCU_CLK->CLKCR)&0x40000000UL);	// wait for VDDC to stabilize
	SCU_GENERAL->PASSWD = 0x000000C3UL;
	SCU_CLK->CGATSET0 |= 0xfff;				// Modul  Clock enable , all Modul's
}
//---------------------------------------------------------------
// PWM0-Ausgang P0.6 initialisieren,
// Hinweis: anschlie�end starten mit pwm1_start oder pwm1_start_interrupt
void pwm0_init(void)
{
	clock_init();
	SCU_GENERAL->CCUCON = 1;				// Enable CCU40
	// CCU40 Init:
	CCU40->GIDLC = 0x10F; 					// Vorteiler enable,CCU4x enable
	CCU40_CC40->TC = 4;  					// Shadow transfer enable on clear
	CCU40_CC40->PSC = 0x1;					// Vorteiler , hier 64Mhz/2
	CCU40_CC40->PRS = periode_pwm; 			// Period Register set, hier ca. 640 Hz

	// CCU40 Compare:  (Ausganspin P0.6)
	CCU40_CC40->CRS = 25000;				// Compare Wert,hier 1:1 Imp
	CCU40->GCSS = 1; 						// Shadow transfer request slice 0
	//PORT0->IOCR0 = 0xA0;					// Alt4=>OUT bei P0.0
	PORT0->IOCR4 |= 0xA0<<16;				// Alt4=>OUT bei P0.6
	//CCU40_CC40->TCSET = 0x1; //starts counting
}
//---------------------------------------------------------------
// PWM2-Ausgang P0.7 initialisieren
//---------------------------------------------------------------
void pwm1_init(void)
{
	clock_init();
	SCU_GENERAL->CCUCON = 1;				// Enable CCU40
	// CCU41 Init:
	CCU40->GIDLC = 0x10F; 					// Vorteiler enable,CCU4x enable
	CCU40_CC41->TC = 4;  					// Shadow transfer enable on clear
	CCU40_CC41->PSC = 0x1;					// Vorteiler , hier 64Mhz/2
	CCU40_CC41->PRS = periode_pwm; 			// Period Register set, hier ca. 640 Hz

	// CCU40 Compare:  (Ausganspin P0.0)
	CCU40_CC41->CRS = 25000;			// Compare Wert,hier 1:1 Imp
	CCU40->GCSS = 1<<4; 				// Shadow transfer request slice 1
	//PORT0->IOCR0 |= 0xA0 <<8;			// Alt4=>OUT bei P0.1
	PORT0->IOCR4 |= 0xA0<<24;			// Alt4=>OUT bei P0.7
}
//---------------------------------------------------------------
// PWM3-Ausgang P0.8 initialisieren
//---------------------------------------------------------------
void pwm2_init(void)
{
	clock_init();
	SCU_GENERAL->CCUCON = 1;			// Enable CCU40
		// CCU42 Init:
	CCU40->GIDLC = 0x10F; 				// Vorteiler enable,CCU4x enable
	CCU40_CC42->TC = 4;  				// Shadow transfer enable on clear
	CCU40_CC42->PSC = 0x1;				// Vorteiler , hier 64Mhz/2
	CCU40_CC42->PRS = periode_pwm; 		// Period Register set , hier ca. 640 Hz

	// CCU40 Compare:  (Ausganspin P0.8)
	CCU40_CC42->CRS = 25000;			// Compare Wert,hier 1:1 Imp
	CCU40->GCSS = 1<<8; 				// Shadow transfer request slice 2
	//PORT0->IOCR0 |= 0xA0 << 16;		// Alt4=>OUT bei P0.2
	PORT0->IOCR8 |= 0xA0;				// Alt4=>OUT bei P0.8
}
//---------------------------------------------------------------
// Comparewert einstellen, der den Tastgrad des PWM-Singals ver�ndert
// compare = 0...50000, Tastgrad 50% bei compare = 25000
//---------------------------------------------------------------
void pwm0_duty_cycle (uint16_t compare)
{
	CCU40_CC40->CRS = compare; 	// Compare Wert, hier 1:1 Imp
	CCU40->GCSS |= 1; 					// Shadow transfer request slice 0
}
//---------------------------------------------------------------
// Comparewert (f�r Tastgrad) und Periodendauer der PWM0 P0.6 einstellen
// compare = 0...period, period = 0...50000
// Tastgrad 50% bei compare = period/2
//---------------------------------------------------------------
void pwm0_duty_cycle_period (uint8_t vorteiler, uint16_t compare, uint16_t periode)
{
	vorteiler %= 0x10;				// max. 15
	CCU40_CC40->PSC = vorteiler;	
	CCU40_CC40->PRS = periode; 		// Period Register set, ca. 640 Hz bei periode = 50000
	CCU40_CC40->CRS = compare;		// Compare Wert
	CCU40->GCSS |= 1; 				// Shadow transfer request slice 0
}
//---------------------------------------------------------------
// Comparewert einstellen, der den Tastgrad des PWM-Singals ver�ndert
// compare = 0...50000, Tastgrad 50% bei compare = 25000
//---------------------------------------------------------------
void pwm1_duty_cycle (uint16_t compare)
{
	CCU40_CC41->CRS = compare;	// Compare Wert,hier 1:1 Imp
	CCU40->GCSS |= 1<<4; 				// Shadow transfer request slice 1
}
//---------------------------------------------------------------
// Comparewert (f�r Tastgrad) und Periodendauer der PWM1 P0.7 einstellen
// compare = 0...period, period = 0...50000
// Tastgrad 50% bei compare = periode/2
//---------------------------------------------------------------
void pwm1_duty_cycle_period (uint8_t vorteiler, uint16_t compare, uint16_t periode)
{
	vorteiler %= 0x10;					// max. 15
	CCU40_CC41->PSC = vorteiler;	
	CCU40_CC41->PRS = periode; 			// Period Register set, ca. 640 Hz bei periode = 50000
	CCU40_CC41->CRS = compare;			// Compare Wert
	CCU40->GCSS |= 1<<4; 				// Shadow transfer request slice 1
}
//---------------------------------------------------------------
// Comparewert einstellen, der den Tastgrad des PWM-Singals ver�ndert
// compare = 0...50000, Tastgrad 50% bei compare = 25000
//---------------------------------------------------------------
void pwm2_duty_cycle (uint16_t compare)
{
	CCU40_CC42->CRS = compare;	// Compare Wert,hier 1:1 Imp
	CCU40->GCSS |= 1<<8; 				// Shadow transfer request slice 2
}
//---------------------------------------------------------------
// Comparewert (f�r Tastgrad) und Periodendauer der PWM2 P0.8 einstellen
// compare = 0...period, period = 0...50000
// Tastgrad 50% bei compare = periode/2
//---------------------------------------------------------------
void pwm2_duty_cycle_period (uint8_t vorteiler, uint16_t compare, uint16_t periode)
{
	vorteiler %= 0x10;					// max. 15
	CCU40_CC42->PSC = vorteiler;	
	CCU40_CC42->PRS = periode; 			// Period Register set, ca. 640 Hz bei periode = 50000
	CCU40_CC42->CRS = compare;			// Compare Wert
	CCU40->GCSS |= 1<<8; 				// Shadow transfer request slice 2
}
//----------------------------------------------------------------
// PWM0-Ausgang P0.6 Ausgabe starten ohne Interrupt starten
//----------------------------------------------------------------
void pwm0_start(void)
{
	 CCU40_CC40->TCSET = 0x1; //starts counting
}
/*------------------------------------------------------------------
* PWM0-Ausgang P0.6 Ausgabe starten mit Interrupt nach jeder Periode
* Diese Timer-ISR ins eigene Programm kopieren
void CCU40_0_IRQHandler (void)
{
	pwm0_duty_cycle (uint16_t compare); // neuen Comparewert laden
}
*/
void pwm0_start_interrupt(void)
{
	CCU40_CC40->TCSET = 0x1; 		// starts counting
	// Int bei Periode CCU40_CC40 :
	CCU40_CC40->INTE |= 1;			// Int bei Period Match_
	CCU40_CC40->SRS |= 0;			// Node Pointer auf Ausg. 0 schalten
	NVIC_EnableIRQ(CCU40_0_IRQn); 	// Int Freigabe f�r CCU40 => CC40
}
//----------------------------------------------------------------
// PWM1-Ausgang P0.7 Ausgabe starten ohne Interrupt starten
//----------------------------------------------------------------
void pwm1_start(void)
{
	 CCU40_CC41->TCSET = 0x1; //starts counting
}
/*------------------------------------------------------------------
* PWM1-Ausgang P0.7 Ausgabe starten mit Interrupt nach jeder Periode
* Diese Timer-ISR ins eigene Programm kopieren
void CCU40_1_IRQHandler (void)
{
	pwm1_duty_cycle (uint16_t compare); // neuen Comparewert laden
}
*/
void pwm1_start_interrupt(void)
{
	CCU40_CC41->TCSET = 0x1; 		// starts counting
	// Int bei Periode CCU40_CC41:
	CCU40_CC41->INTE |= 1;			// Int bei Period Match_
	CCU40_CC41->SRS |= 1;			// Node Pointer auf Ausg. 1 schalten
	NVIC_EnableIRQ(CCU40_1_IRQn); 	// Int Freigabe f�r CCU40 => CC41
}
//----------------------------------------------------------------
// PWM2-Ausgang P0.8 Ausgabe starten ohne Interrupt starten
//----------------------------------------------------------------
void pwm2_start(void)
{
	 CCU40_CC42->TCSET = 0x1; //starts counting
}
/*------------------------------------------------------------------
* PWM2-Ausgang P0.8 Ausgabe starten mit Interrupt nach jeder Periode
* Diese Timer-ISR ins eigene Programm kopieren
void CCU40_2_IRQHandler (void)
{
	pwm3_duty_cycle (uint16_t compare); // neuen Comparewert laden
}
*/
void pwm2_start_interrupt(void)
{
	CCU40_CC42->TCSET = 0x1; 		// starts counting
	// Int bei Periode CCU40_CC42:
	CCU40_CC42->INTE |= 1;			// Int bei Period Match_
	CCU40_CC42->SRS |= 2;			// Node Pointer auf Ausg. 2 schalten
	NVIC_EnableIRQ(CCU40_2_IRQn); 	// Int Freigabe f�r CCU40 => CC42
}
// PWM0 anhalten -----------------------------------------------------
void pwm0_stop(void)
{
	CCU40_CC40->TCCLR = 0x01;
	NVIC_DisableIRQ(CCU40_0_IRQn); 	// Int sperren f�r CCU40 => CC40
}
// PWM1 anhalten -----------------------------------------------------
void pwm1_stop(void)
{
	CCU40_CC41->TCCLR = 0x01;
	NVIC_DisableIRQ(CCU40_1_IRQn); 	// Int sperren f�r CCU40 => CC41
}
// PWM2 anhalten -----------------------------------------------------
void pwm2_stop(void)
{
	CCU40_CC42->TCCLR = 0x01;
	NVIC_DisableIRQ(CCU40_2_IRQn); 	// Int sperren f�r CCU40 => CC42
}

// ab hier ADC-Funktionen ****************************************

//---------------------------------------------------------------
// Analog-Digital-Converter initialisieren
//---------------------------------------------------------------
void adc_init(void)
{
	PORT2->IOCR4 = PORT2->IOCR4 & 0x00000000;	// P2 => INPUT
	// hier ggf. PORT2->PDISC einstellen => Port2 auf analog input
	SCU_GENERAL->PASSWD = 0x000000C0UL;			// Modul write enable
	SCU_CLK->CGATCLR0 |= 0x00000001; 			// disable  VADC gating
	while (( SCU_CLK->CLKCR)& 0x40000000);		// wait for stabilize
	SCU_GENERAL->PASSWD = 0x000000C3UL;
	CLR_BIT(VADC->CLC,0); 						// Clock Enable

	SHS0->SHSCFG = 0x00008000 ; 				// SCWC Bit = 1
	//SET_BIT (SHS0->SHSCFG,15); => geht nicht !!
	CLR_BIT (SHS0->SHSCFG,12);  				// ANOFF = 0
	SET_BIT (SHS0->SHSCFG,11); 					// int. Uref
	*((int*)0x40010500)= 0x01;					// Toggle on => workarond
	//VADC_Start = 0x00000001;					// Zusatz Bit f�r Start
	while ( (SHS0->SHSCFG & 0x00004000) == 0);	// Warte auf ready
    //--------------Kalibrierung
	 // VADC->GLOBCFG &= ~VADC_GLOBCFG_DPCAL0_Msk;    // Enable post-calibration.
	  // workaround for calibration
	  VADC->GLOBCFG |= VADC_GLOBCFG_DPCAL0_Msk;
	  VADC->GLOBCFG |= VADC_GLOBCFG_DPCAL1_Msk;
	  VADC->GLOBCFG &= ~VADC_GLOBCFG_SUCAL_Msk;      // Raising edge on SUCAL to initiate the calibration.
	  VADC->GLOBCFG |= VADC_GLOBCFG_SUCAL_Msk;
	  // workaround for calibration
	  VADC->GLOBCFG |= VADC_GLOBCFG_DPCAL0_Msk;
	  VADC->GLOBCFG |= VADC_GLOBCFG_DPCAL1_Msk;
	  while ((SHS0->SHSCFG & SHS_SHSCFG_STATE_Msk) != 0);  // Wait until calibration is finished.
	  *((int*)0x480340E0) = 0x80008000;
	  *((int*)0x480340E4) = 0x80008000;
	//--------------Ende Kalibrierung
	SET_BIT (VADC->BRSMR,0);					// Conv Request => pending Bit
	VADC->BRSCTRL = 0x00000000;					// kein ext. Trigger
	SET_BIT(VADC->BRSSEL[0],0)  ;				// Kanal 0
}
//---------------------------------------------------------------
// 12-Bit-Wert vom Analogeingang einlesen
// Kanal 0: P2.6 =>Poti , Kanal 2: P2.9
//---------------------------------------------------------------
uint16_t adc_in(uint8_t kanal)

{
	kanal %=5;									// max Kanal 5
	VADC->BRSPND[0]  = 1 << kanal;				// Pending Kanal 0.. 8
	while ((VADC->GLOBRES & 0x80000000) == 0 );	// Abfrage VF Bit
	CLR_BIT(VADC->GLOBRES,31);					// Clear VF Bit
	return (VADC->GLOBRES & 0x00000fff);		// 12 Bit Ergebnis
}
// Delays ********************************************************
//---------------------------------------------------------------
void delay_10us (uint8_t n)
{
	volatile uint16_t z;
	for (;n != 0;--n)
			for (z = 10; 	z!= 0; 	--z); // 10 zu lang, 9 zu kurz
}
//---------------------------------------------------------------
// Zeitverz�gerung n mal 100�s
//---------------------------------------------------------------
void delay_100us (uint8_t n)
{
	volatile uint16_t z;
	for (;n != 0;--n)
		for (z = _T_CONST_; 	z!= 0; 	--z);
}

//---------------------------------------------------------------
// Zeitverz�gerung Millisekunden
//---------------------------------------------------------------
void delay_ms( uint16_t millisekunden)
{
	volatile uint16_t n = 0;

	while(n < millisekunden)
	{
		delay_100us(10);
		n++;
	}
}

//void delay_s(uint16_t sekunden)
//{
//	volatile uint16_t n = 0;
//
//	while(n < sekunden)
//	{
//		delay_ms(1000);
//		n++;
//	}
//}

// ****************************************************************************
// I2C- Routinen zur Ansteuerung eins I2C-Slaves
// I2C-Bus-Funktionen i2c_init, i2c_start, i2c_stop, i2c_write, i2c_read
// ****************************************************************************

//---------------------------------------------------------------
// **** Zeitverzoegerung zur Verlangsamung der Datenuebertragungsrate ***********
// **** i=2 bis i=100 waehlen je nach I2C-IC und Pull-Up-Widerstand
//---------------------------------------------------------------
void i2c_delay(void)
//---------------------------------------------------------------
{
  volatile uint8_t i;	         // i nicht rausoptimieren (volatile)!!
  for (i=3;i!=0;i--);
}

//---------------------------------------------------------------
// Initialiserung I2C-Bus
// SDA P2.10, SCL P2.11 , f�r XMC2GO , wird auch f�r Display benutzt
//---------------------------------------------------------------
void i2c_init (void)
{ CLR_BIT(PORT2->PDISC,10);  // Port2 digital Fkt.
  CLR_BIT(PORT2->PDISC,11);

  I2C_DIR |= SDA_OUT; // SDA => Pn.0
  I2C_DIR |= SCL_OUT;	//SCL => OUT
  I2C_PORT_OUT |= (1<<SDA);   // Leitungen in den Grundzustand High
  i2c_delay();
  I2C_PORT_OUT |= (1<<SCL);
  i2c_delay();
}

//---------------------------------------------------------------
// Startbedingung I2C-Bus
//---------------------------------------------------------------
void i2c_start (void)
{
  I2C_DIR |= SDA_OUT;	// Richtung => OUT
  I2C_DIR |= SCL_OUT;	// Richtung => OUT
  I2C_PORT_OUT |= (1<<SDA);                  // Grundzustand
  I2C_PORT_OUT |= (1<<SCL);
  i2c_delay();
  I2C_PORT_OUT &= ~(1<<SDA);                 // Wechsel SDA von 1 -> 0 w hrend SCL = 1
  i2c_delay();
  I2C_PORT_OUT &= ~(1<<SCL);
  i2c_delay();
}

//---------------------------------------------------------------
// Stoppbedingung I2C-Bus
//---------------------------------------------------------------
void i2c_stop (void)
{
	I2C_DIR |= SDA_OUT;	// Richtung => OUT
	I2C_DIR |= SCL_OUT;	// Richtung => OUT
	I2C_PORT_OUT &= ~(1<<SDA);                 // SDA = 0;
	I2C_PORT_OUT |= (1<<SCL);                  // SCL = 1;
	i2c_delay();
  // Wechsel von SDA 0 -> 1 w hrend SCL = 1
	I2C_PORT_OUT |= (1<<SDA);                  // SDA = 1;
	i2c_delay();
}

//---------------------------------------------------------------
// Byte ausgeben an I2C-Bus , Rueckgabewert = ack = ACK/NACK
//---------------------------------------------------------------
uint8_t i2c_write (uint8_t value)
{
	uint8_t z;                             // Zaehler
	  uint8_t ack;                           // Acknowledge-Bit

	    I2C_DIR |= SDA_OUT;	// Richtung => OUT
	    I2C_DIR |= SCL_OUT;	// Richtung => OUT

	  for (z = 8; z != 0; z --)              // Zaehler: serielle Ausgabe von 8 Bit
	  	  {
		  I2C_PORT_OUT &= ~(1<<SCL);               // SCL = 0  Clockausgabe
		  // Daten:
	    if ((value & 0x80) == 0x80)          // Ausgabe: MSB first
	      I2C_PORT_OUT |= (1<<SDA);              // SDA = 1;
	    else
	      I2C_PORT_OUT &= ~(1<<SDA);             // SDA = 0;
			value <<=1;                          // naechstes Bit!

		i2c_delay();
		I2C_PORT_OUT |= (1<<SCL);                // SCL = 1
	    i2c_delay();
	  	  }

	  I2C_PORT_OUT &= ~(1<<SCL);               // SCL = 0 Datenausabe

	  i2c_delay();
	  I2C_PORT_OUT |= (1<<SCL);                  // SCL = 1 Slave kann best tigen
	  i2c_delay();                           // warten
	  I2C_DIR &= SDA_IN;			// Data auf Input
	  ack = (I2C_PORT_IN & (1<<SDA))>>SDA;

	  i2c_delay();
	  I2C_DIR |= SDA_OUT;	// Richtung => OUT
	  I2C_PORT_OUT &= ~(1<<SCL);                 // SCL = 0 Slave soll Ackn-Ausgabe beenden
	  i2c_delay();

	  return ack;                            // Acknoledge-Bit ist R ckgabewert der Funktion
	                                         // ack = 0 bedeutet Slave hat "verstanden" !!!!!!
}
//---------------------------------------------------------------
// Byte einlesen vom I2C-Bus.
//---------------------------------------------------------------
uint8_t i2c_read (uint8_t ack)
{
  uint8_t z, value = 0;
  I2C_DIR &= SDA_IN;			// Data auf Input


  for (z = 8; z != 0; z--)      // Z hler: serielles Einlesen von 8 Bit
  {
    I2C_PORT_OUT |= (1<<SCL);       // SCL = 1  Daten sind g ltig
    i2c_delay();

    value <<= 1;
    value |= (uint8_t) ((I2C_PORT_IN & (1<<SDA))>>SDA);    //SDA   Datenbit in Puffer

    i2c_delay();
    I2C_PORT_OUT &= ~(1<<SCL);                // SCL = 0  Daten lesen beendet
    i2c_delay();
  }
  I2C_DIR |= SDA_OUT;	// Richtung => OUT
  if (ack == ACK)  I2C_PORT_OUT &= ~(1<<SDA); // SDA = 0  (= Acknowledge)
  else             I2C_PORT_OUT |= (1<<SDA);  // SDA = 1  (= Not Acknowledge)

  i2c_delay();
  I2C_PORT_OUT |= (1<<SCL);                   // SCL = 1  Ackn. g ltig
  i2c_delay();
  I2C_PORT_OUT &= ~(1<<SCL);                  // SCL = 0  Ackn. beendet
  i2c_delay();
  I2C_PORT_OUT &= ~(1<<SDA);                  // SDA = 0  Leitung SDA vorbereiten f r Stoppbed.
  i2c_delay();

  return ( value );                       // eingelesenes Byte = R ckgabewert
}

// ****************************************************************************
// RS232-Routinen zur Kommunikation mit PC-Terminal
// ****************************************************************************
// RxD auf P2.2 , XMC1100 TxD auf P2.1 f�r VCOM
// Baudrate = 32 MHz * Step/1024 * 1/(PDIV +1)  * 1/ (PCTQ +1) * 1/ (DCTQ +1)
/* Registerwerte f�r alte Routinen mit fester Baudrate
#define FDR_STEP	25	// fuer 9.6 kBaud
#define BRG_PDIV	4
#define BRG_DCTQ	15	// Time quanta per Bit => immer 15
#define BRG_PCTQ	0
Ende alte Routinen */
// USIC : FIFO DPTR & SIZE MASK and POS Values
#define   USIC_CH_TBCTR_DPTRSIZE_Pos  	(0U)
#define   USIC_CH_TBCTR_DPTRSIZE_Msk  	(0x0700003FU << USIC_CH_TBCTR_DPTRSIZE_Pos)

#define   USIC_CH_RBCTR_DPTRSIZE_Pos  	(0U)
#define   USIC_CH_RBCTR_DPTRSIZE_Msk  	(0x0700003FU << USIC_CH_RBCTR_DPTRSIZE_Pos)



//---------------------------------------------------------------
/* Serielle Schnittstelle UART0 initialisieren, Baudrate 300,1200,9600...115000, Standard 9600, 8N1
 XMC11002GO => RxD auf P2.2 ,  TxD auf P2.1
  �ber USB als virtuelle Schnittstelle COM x am PC verwendbar
 enable_interrupt = RE_INT = 1 Interrupt bei Receive, Empfang
 enable_interrupt = NO_RE_INT = 0 kein Interrupt bei Receive, Empfang
 Folgende Routine zum interruptgesteuertem Empfang eines Bytes ins eigene Programm kopieren
 void USIC0_0_IRQHandler(void)   // Interrupt-Service-Routine 1 Byte empfangen
   {
   empfangsvariabe2 = uart0_get();  // 1 Zeichen aus Empfangspuffer abholen
   }
 */
void uart0_init (uint32_t baudrate, uint8_t enable_interrupt)
{
	SCU_GENERAL->PASSWD = 0x000000C0UL;
	SCU_CLK->CGATCLR0 |= 0x00000008;		// stop gating USIC
		while((SCU_CLK->CLKCR)&0x40000000UL);	// wait for VDDC to stabilize
	SCU_GENERAL->PASSWD = 0x000000C3UL;

	USIC0_CH0->KSCFG |= 3UL ;	//  Mode enable + Freigabe write
	USIC0_CH0->FDR |= 1UL << 15 ;	//Fractional divider mode selected
	// Baudrate setzen :
	XMC_USIC_CH_SetzeBaudrate( XMC_USIC0_CH0, baudrate);
	/*USIC0_CH0->FDR|= FDR_STEP ;								 // feste Baudrate 9600
				//Vorteiler:
				// Baudrate = 32 MHz * Step/1024 * 1/(PDIV +1)  * 1/ (PCTQ +1) * 1/ (DCTQ +1)
			USIC0_CH0->BRG |= (BRG_PDIV << 16) ;
			USIC0_CH0->BRG |= BRG_DCTQ << 10 ;
			USIC0_CH0->BRG |= BRG_PCTQ << 8 ;*/
	// Kanal 0 soll auf P1.0 ( RxD ) / P1.1 ( TxD) als ASC Schnittstelle zur Verf�gung stehen
			//Configuration of USIC Shift Control Channel 0 :
			//Transmit/Receive LSB first is selected, Transmission Mode (TRM) = 1, Passive Data Level (PDL) = 1
		WR_REG(USIC0_CH0->SCTR, USIC_CH_SCTR_PDL_Msk, USIC_CH_SCTR_PDL_Pos, 1);
		WR_REG(USIC0_CH0->SCTR, USIC_CH_SCTR_TRM_Msk, USIC_CH_SCTR_TRM_Pos, 1);
			//Set Word Length (WLE) & Frame Length (FLE)
		WR_REG(USIC0_CH0->SCTR, USIC_CH_SCTR_FLE_Msk, USIC_CH_SCTR_FLE_Pos, 7);
		WR_REG(USIC0_CH0->SCTR, USIC_CH_SCTR_WLE_Msk, USIC_CH_SCTR_WLE_Pos, 7);

			//Configuration of USIC Transmit Control/Status Register
			//TBUF Data Enable (TDEN) = 1, TBUF Data Single Shot Mode (TDSSM) = 1
		WR_REG(USIC0_CH0->TCSR, USIC_CH_TCSR_TDEN_Msk, USIC_CH_TCSR_TDEN_Pos, 1);
		WR_REG(USIC0_CH0->TCSR, USIC_CH_TCSR_TDSSM_Msk, USIC_CH_TCSR_TDSSM_Pos, 1);

			//Configuration of Protocol Control Register
			//Sample Mode (SMD) = 1, 1 Stop bit is selected, Sample Point (SP) = 2, Pulse Length (PL) = 0
		WR_REG(USIC0_CH0->PCR_ASCMode, USIC_CH_PCR_ASCMode_SMD_Msk,
					USIC_CH_PCR_ASCMode_SMD_Pos, 1);
		WR_REG(USIC0_CH0->PCR_ASCMode, USIC_CH_PCR_ASCMode_STPB_Msk, USIC_CH_PCR_ASCMode_STPB_Pos, 0);
		WR_REG(USIC0_CH0->PCR_ASCMode, USIC_CH_PCR_ASCMode_SP_Msk, USIC_CH_PCR_ASCMode_SP_Pos, 9);
			//TBIF is set to simplify polling
		WR_REG(USIC0_CH0->PSR_ASCMode, USIC_CH_PSR_TBIF_Msk, USIC_CH_PSR_TBIF_Pos, 1);
			//Configuration of Transmitter Buffer Control Register
		WR_REG(USIC0_CH0->TBCTR, USIC_CH_TBCTR_LIMIT_Msk, USIC_CH_TBCTR_LIMIT_Pos, 0);

			//Configuration of Channel Control Register
			//parity generation is disabled
		USIC0_CH0->CCR |=  0x02;	// ASC Mode
		// Int Freigabe USIC0
		if (enable_interrupt == RE_INT)
			{
			USIC0_CH0->CCR |= 1UL <<14;	// Receive Int enable
			NVIC_EnableIRQ(USIC0_0_IRQn);	// SR0 Int enable
			}
			//Data Pointer & Buffer Size for Transmitter Buffer Control - DPTR = 64,  SIZE = 6
		WR_REG(USIC0_CH0->TBCTR, USIC_CH_TBCTR_DPTRSIZE_Msk, USIC_CH_TBCTR_DPTRSIZE_Pos, 0x06000040);
		//**************************************************************************************************
		// Channel 0 => Tx / Rx Pin
		// USIC0_CH0.DX3A oder DX4A =>  P2.2 Input => RxD
		//USIC0_CH0.DOUT0 => P2.1 Data OUT => TxD
			// P2.2 => Dx3CR
		USIC0_CH0->DX0CR = 6;
		USIC0_CH0->DX3CR = 0;
		USIC0_CH0->DX5CR = 0;
		 /*Set input source path
			  XMC_USIC_CH_SetInputSource(XMC_UART0_CH0, XMC_USIC_CH_INPUT_DX0, 6U);
			  XMC_USIC_CH_SetInputSource(XMC_UART0_CH0, XMC_USIC_CH_INPUT_DX3, 0U);
			  XMC_USIC_CH_SetInputSource(XMC_UART0_CH0, XMC_USIC_CH_INPUT_DX5, 0U); */
		PORT2->PDISC &= 0xff9;	// P2.1 / P2.2 Digital Fkt
		PORT2->IOCR0  |= 0b10110 << 11 ; // ALT6 fuer Pin P2.1 USIC0-Chanel 0 TxD
		PORT2->IOCR0 &= 0xff00ffff;	// P2.2 => INPUT =>  RxD
}

//-----------------------------------------------------------------
// liefert 1 wenn Zeichen empfangen und 0 wenn kein Zeichen empfangen (P)
//----------------------------------------------------------------
uint8_t uart0_char_received (void)
{
	if ((USIC0_CH0->PSR &(1UL <<14)) != 0) return 1; // wenn Zeichen vorhanden
		else return 0;
}

//---------------------------------------------------------------
// liest ein empfangenes Byte (0..255).
// 0 kann Byte=0 bedeuten oder kein Zeichen empfangen!
//---------------------------------------------------------------
uint8_t uart0_get ( void )
{
  uint8_t buffer;
		buffer = USIC0_CH0->RBUF & 0xff ;	// RX Buffer auslesen
		USIC0_CH0->PSCR |= 1UL <<14 ;		// Receive Flag l�schen
	return buffer;
}

// ***********************************************************************
void uart0_put( uint8_t value)
{	// Transmitt auf P1.1
	USIC0_CH0->IN[0] = value;
}

//---------------------------------------------------------------
// schreibt eine nullterminierte Zeichenfolge auf uart0
//---------------------------------------------------------------
void uart0_print ( uint8_t *text )
{
  while (*text != '\0')
    uart0_put(*text++);
}

//*************************************************************************
// Baudrate einstellen:
// cannel0 => XMC_USIC0_CH0	==> P2.1 / P2.2
// cannel1=>  XMC_USIC0_CH1
// rate => Baudrate zwischen 300 .... 115000 Baud
void XMC_USIC_CH_SetzeBaudrate(XMC_USIC_CH_t *const channel,uint32_t rate)
{
  uint32_t peripheral_clock;

  uint32_t clock_divider;
  uint32_t clock_divider_min;

  uint32_t pdiv;
  uint32_t pdiv_int;
  uint32_t pdiv_int_min;

  uint32_t pdiv_frac;
  uint32_t pdiv_frac_min;
  uint32_t oversampling = 16;

  /* The rate and peripheral clock are divided by 100 to be able to use only 32bit arithmetic */
  if (rate >= 100U)
  {
    peripheral_clock = 32000000 / 100U;
    rate = rate / 100U;

    clock_divider_min = 1U;
    pdiv_int_min = 1U;
    pdiv_frac_min = 0x3ffU;

    for(clock_divider = 1023U; clock_divider > 0U; --clock_divider)
    {
      pdiv = ((peripheral_clock * clock_divider) / (rate * oversampling));
      pdiv_int = pdiv >> 10U;
      pdiv_frac = pdiv & 0x3ffU;

      if ((pdiv_int < 1024U) && (pdiv_frac < pdiv_frac_min))
      {
        pdiv_frac_min = pdiv_frac;
        pdiv_int_min = pdiv_int;
        clock_divider_min = clock_divider;
      }
    }

   channel->FDR |= (clock_divider_min << USIC_CH_FDR_STEP_Pos);

   channel->BRG = (channel->BRG & ~(USIC_CH_BRG_DCTQ_Msk |
                                     USIC_CH_BRG_PDIV_Msk |
                                     USIC_CH_BRG_PCTQ_Msk |
                                     USIC_CH_BRG_PPPEN_Msk)) |
                   ((oversampling - 1U) << USIC_CH_BRG_DCTQ_Pos) |
                   ((pdiv_int_min - 1U) << USIC_CH_BRG_PDIV_Pos);
  }
  return;
}
//***************************************************************************

//---------------------------------------------------------------
/* Serielle Schnittstelle UART1 initialisieren, Baudrate 300,1200,9600...115000, Standard 9600, 8N1
  UART1: RxD auf P0.6 , TxD auf P0.7
 enable_interrupt = RE_INT = 1 Interrupt bei Receive, Empfang
 enable_interrupt = NO_RE_INT = 0 kein Interrupt bei Receive, Empfang
 Folgende Routine zum interruptgesteuertem Empfang eines Bytes ins eigene Programm kopieren
 void USIC0_1_IRQHandler(void)   // Interrupt-Service-Routine 1 Byte empfangen von UART2
   {
   empfangsvariabe2 = uart1_get();  // 1 Zeichen aus Empfangspuffer abholen
   }
 */
void uart1_init (uint32_t baudrate, uint8_t enable_interrupt)
{
	SCU_GENERAL->PASSWD = 0x000000C0UL;
	SCU_CLK->CGATCLR0 |= 0x00000008;		// stop gating USIC
	while((SCU_CLK->CLKCR)&0x40000000UL);	// wait for VDDC to stabilize
	SCU_GENERAL->PASSWD = 0x000000C3UL;

	USIC0_CH1->KSCFG |= 3UL ;				//  Mode enable + Freigabe write
	//WR_REG(USIC0_CH1->FDR, USIC_CH_FDR_DM_Msk, USIC_CH_FDR_DM_Pos, 2);
	USIC0_CH1->FDR |= 1UL << 15 ;			//Fractional divider mode selected
	XMC_USIC_CH_SetzeBaudrate(XMC_USIC0_CH1, baudrate);  // Baudrate einstellen Kanal 1 CH1  ????????? geht noch nicht
		/*USIC0_CH1->FDR|= FDR_STEP ;                            // feste Baudrate 9600
				//Vorteiler:
				// Baudrate = 32 MHz * Step/1024 * 1/(PDIV +1)  * 1/ (PCTQ +1) * 1/ (DCTQ +1)
			USIC0_CH1->BRG |= (BRG_PDIV << 16) ;
			USIC0_CH1->BRG |= BRG_DCTQ << 10 ;
			USIC0_CH1->BRG |= BRG_PCTQ << 8 ;
			*/

		//Configuration of USIC Shift Control Channel 1 :
		//Transmit/Receive LSB first is selected, Transmission Mode (TRM) = 1, Passive Data Level (PDL) = 1
	WR_REG(USIC0_CH1->SCTR, USIC_CH_SCTR_PDL_Msk, USIC_CH_SCTR_PDL_Pos, 1);
	WR_REG(USIC0_CH1->SCTR, USIC_CH_SCTR_TRM_Msk, USIC_CH_SCTR_TRM_Pos, 1);
			//Set Word Length (WLE) & Frame Length (FLE)
	WR_REG(USIC0_CH1->SCTR, USIC_CH_SCTR_FLE_Msk, USIC_CH_SCTR_FLE_Pos, 7);
	WR_REG(USIC0_CH1->SCTR, USIC_CH_SCTR_WLE_Msk, USIC_CH_SCTR_WLE_Pos, 7);

			//Configuration of USIC Transmit Control/Status Register
			//TBUF Data Enable (TDEN) = 1, TBUF Data Single Shot Mode (TDSSM) = 1
	WR_REG(USIC0_CH1->TCSR, USIC_CH_TCSR_TDEN_Msk, USIC_CH_TCSR_TDEN_Pos, 1);
	WR_REG(USIC0_CH1->TCSR, USIC_CH_TCSR_TDSSM_Msk, USIC_CH_TCSR_TDSSM_Pos, 1);

			//Configuration of Protocol Control Register
			//Sample Mode (SMD) = 1, 1 Stop bit is selected, Sample Point (SP) = 2, Pulse Length (PL) = 0
	WR_REG(USIC0_CH1->PCR_ASCMode, USIC_CH_PCR_ASCMode_SMD_Msk,
					USIC_CH_PCR_ASCMode_SMD_Pos, 1);
	WR_REG(USIC0_CH1->PCR_ASCMode, USIC_CH_PCR_ASCMode_STPB_Msk, USIC_CH_PCR_ASCMode_STPB_Pos, 0);
	WR_REG(USIC0_CH1->PCR_ASCMode, USIC_CH_PCR_ASCMode_SP_Msk, USIC_CH_PCR_ASCMode_SP_Pos, 9);
			//TBIF is set to simplify polling
	WR_REG(USIC0_CH1->PSR_ASCMode, USIC_CH_PSR_TBIF_Msk, USIC_CH_PSR_TBIF_Pos, 1);
			//Configuration of Transmitter Buffer Control Register
	WR_REG(USIC0_CH1->TBCTR, USIC_CH_TBCTR_LIMIT_Msk, USIC_CH_TBCTR_LIMIT_Pos, 0);
	SET_BIT(USIC0_CH1->PCR_ASCMode,17);// TSTSEN bit SET
			//Configuration of Channel Control Register
			//parity generation is disabled
	USIC0_CH1->CCR |=  0x02;	// ASC Mode
		// Int Freigabe USIC0 (  )
	if (enable_interrupt == RE_INT)
			{
			USIC0_CH1->INPR |= 1 <<8; 		//SR1 ausw hlen
			USIC0_CH1->CCR |= 1UL <<14;		// Receive Int enable
			NVIC_EnableIRQ(USIC0_1_IRQn);	// SR Int enable
			}
			//Data Pointer & Buffer Size for Transmitter Buffer Control - DPTR = 64,  SIZE = 6
	WR_REG(USIC0_CH1->TBCTR, USIC_CH_TBCTR_DPTRSIZE_Msk, USIC_CH_TBCTR_DPTRSIZE_Pos, 0x06000040);
	 /*Set input source path
	  XMC_USIC_CH_SetInputSource(XMC_UART0_CH1, XMC_USIC_CH_INPUT_DX0, 2U);
	  XMC_USIC_CH_SetInputSource(XMC_UART0_CH1, XMC_USIC_CH_INPUT_DX3, 0U);
	  XMC_USIC_CH_SetInputSource(XMC_UART0_CH1, XMC_USIC_CH_INPUT_DX5, 0U);
	  */
		// Channel 1 => Tx / Rx Pin:
		//USIC0_CH1.DX0A  P0.6 Input => RxD
		//USIC0_CH1.DOUT0 P0.7 Output => TxD
	USIC0_CH1->DX0CR = 0x02;	// Kanal: USIC0_CH1 => DXnC auf P0.6 ( siehe Pin Mapping)
	USIC0_CH1->DX3CR = 0x0;
	USIC0_CH1->DX5CR = 0x0;

	PORT0->IOCR4 &= 0x0000ffff;	// P0.6 => INPUT =>  RxD
	PORT0->IOCR4  |= 0b10111 << 27 ; // ALT 7 fuer P0.7 => TxD d.h. XMC1100 sendet auf P0.7

}
//-----------------------------------------------------------------
// liefert 1 wenn Zeichen empfangen und 0 wenn kein Zeichen empfangen (P1.3) bzw. vom PC
//----------------------------------------------------------------
uint8_t uart1_char_received (void)
{
	if ((USIC0_CH1->PSR &(1UL <<14)) != 0) return 1; // wenn Zeichen vorhanden
		else return 0;
}
//---------------------------------------------------------------
// liest ein empfangenes Byte (0..255).
// 0 kann Byte=0 bedeuten oder kein Zeichen empfangen!
//---------------------------------------------------------------
uint8_t uart1_get ( void )
{
  uint8_t buffer;
		buffer = USIC0_CH1->RBUF & 0xff ;	// RX Buffer auslesen
		USIC0_CH1->PSCR |= 1UL <<14 ;		// Receive Flag l schen
	return buffer;
}
//---------------------------------------------------------------
// schreibt ein Byte auf uart1 ( P1.2 )
//---------------------------------------------------------------
void uart1_put ( uint8_t value )
{
	USIC0_CH1->IN[0] = value;
	while((USIC0_CH1->PSR_ASCMode & (1UL<< 9))  != 0);  // Warten , bis Transmit Frame finish BUSY Flag
}
//---------------------------------------------------------------
// schreibt eine nullterminierte Zeichenfolge auf uart1
//---------------------------------------------------------------
void uart1_print ( uint8_t *text )
{
  while (*text != '\0')
    uart1_put(*text++);
}
//***************************************************************************
//**************************************************************************
// Ab hier Int - Funktionen :
// lokale Fkt :
void ext_interrupt0_init(uint8_t flanke);
void ext_interrupt1_init(uint8_t flanke);


/* --------------------------------------------------------------------------
   Externen Interrupt0 initialisieren, ERU0 Kanal 2B0 -> Eingang P2.0,
   flanke=RE=1 Rising Edge ansteigende Flanke
   flanke=FE=0 Falling Edge abfallende Flanke
   Interrupt-Handler ins eigene Programm kopieren:
void ERU0_0_IRQHandler(void)	// ext. Int an P2.0
	{
								// Interrupt-Servie-Routine
	}
//--------------------------------------------------------------------------*/
void ext_interrupt0_init(uint8_t flanke)
{
ERU0->EXISEL = 0x00000000;			//Eingang 0B0 ausw�hlen (P2.0)
if (flanke == FE)
	 ERU0->EXICON[0]=0x00000109;	// PE=1 (Trigger Impuls erzeugen) , FE (auf fallende Flanke reagieren)
else ERU0->EXICON[0]=0x00000105;	// PE=1 (Trigger Impuls erzeugen) , RE (auf ansteigende Flanke reagieren)
ERU0->EXOCON[0]= 0x00000010;		// GP=1 , OGU1 wird mit NVIC Request Line verbunden
CLR_BIT (PORT2->PDISC,0);			// Port 2.0 auf dig. Input
NVIC_EnableIRQ(ERU0_0_IRQn); // Interrupt wird freigegeben Kanal 0 (CMSIS Bib)
}


/* --------------------------------------------------------------------------
   Externen Interrupt1 initialisieren,  ERU0 Kanal 3A1 -> Eingang P2.7,
   flanke=RE=1 Rising Edge ansteigende Flanke
   flanke=FE=0 Falling Edge abfallende Flanke
   Interrupt-Service-Routinen ins eigene Programm kopieren:
void ERU0_3_IRQHandler(void)	// ext. Int an P2.7
	{
								// Interrupt-Servie-Routine
	}
//--------------------------------------------------------------------------*/
void ext_interrupt1_init(uint8_t flanke)
{
 SET_BIT(ERU0->EXISEL,12); 		//EXS3A = 01 =>Eingang 3A1 auswaehlen (P2.7)
if (flanke == FE)
	 ERU0->EXICON[3]=0x00000039; 	// PE=1 , FE (auf fallende Flanke reagieren) , Trigger an OGU3 ,SS=01b fuer B
else ERU0->EXICON[3]=0x00000035; 	// PE=1 , RE (steigende Flanke Kanal 2) , Trigger an OGU3 ,SS=01b fuer B
ERU0->EXOCON[3]=0x00000010; 		// GP=1 , OGU3 wird mit NVIC Request Line verbunden
CLR_BIT (PORT2->PDISC,7);			// Port 2.7 auf dig. Input
NVIC_EnableIRQ(ERU0_3_IRQn); 		// Freigabe Interrupt Kanal 3 (CMSIS Bib)
}

/* Externen Interrupt1 initialisieren --------------------------------------
   ERU0 Kanal 2B0 -> Eingang P2.0, abfallende Flanke
   Interrupt-Handler ins eigene Programm kopieren:
void ERU0_0_IRQHandler(void)	// ext. Int bei fallender Flanke an P2.0
	{							// Interrupt-Servie-Routine
	}
   Externen Interrupt2 initialisieren -------------------------------------
   ERU0 Kanal 3B0 -> Eingang P2.7, steigende Flanke
   Interrupt-Service-Routinen ins eigene Programm kopieren:
void ERU0_3_IRQHandler(void)	// ext. Int bei steigender Flanke an P2.7
	{							// Interrupt-Servie-Routine
	}*/
void ext_interrupt_init ( void )	// beide Int Eing�nge Init
{
	ext_interrupt0_init(FE);
	ext_interrupt1_init(RE);
}

/* --------------------------------------------------------------------------
   Externen Interrupt2 freigeben
   ERU0 Kanal 3B0 -> Eingang P2.7, steigende Flanke
   Interrupt-Service-Routinen ins eigene Programm kopieren:
void ERU0_3_IRQHandler(void)	// ext. Int an P2.7
	{							// Interrupt-Servie-Routine
	}*/

void ext_interrupt0_disable( void )
{
	NVIC_DisableIRQ(ERU0_0_IRQn);
}
void ext_interrupt1_disable( void )
{
	NVIC_DisableIRQ(ERU0_3_IRQn);
}
//***********************************************
/*Interrupt-Service-Routinen*/
// void ERU0_0_IRQHandler(void)// P2.0
//{
			// Interrupt-Servie-Routine
//}
//**********************************************
//void ERU0_3_IRQHandler(void) // P2.7
//{
			// Interrupt-Servie-Routine
//}

/* --------------------------------------------------------
   Timer-Interrupt 1ms initialisieren
   Timer-Interrupt-Handler ins eigene Programm kopieren:
void CCU40_3_IRQHandler (void)  // Timer-Interrupt jede 1 ms
	{
								// Interrupt-Servie-Routine
    }
 */
void timer1ms_init ( void )
{
	clock_init();
	SCU_GENERAL->CCUCON |= 1;	// Enable CCU40 Modul
	// CCU43 Init:
	CCU40->GIDLC 	= 0x10F; 	//Vorteiler enable f�r alle Timer , IDLE clear
	CCU40_CC43->TC	= 0x08;		//Shadow transfer enable
	CCU40_CC43->PSC = 0x1;		// Vorteiler , hier 64Mhz/2 => 32 MHzclock
	CCU40_CC43->PRS	= 32000;	//Period Register f�r 1ms
	CCU40->GCSS = 0b001 << 12; 	//Shadow transfer request  Timer CCU40->CC43
	CCU40_CC43->TCSET = 1; 		//starts counting
	// Interrupt Freigaben:
	CCU40_CC43->SRS |= 0b11;	// Service Req. Line 3
	CCU40_CC43->INTE |= 1;		// PME => Period match  Int. enable
	NVIC_EnableIRQ(CCU40_3_IRQn);  // Freigabe CCU40SR3
}
/* wie timer_start_interrupt();
   Timer-Interrupt-Handler ins eigene Programm kopieren:
	void CCU40_3_IRQHandler (void)  // Timer-Interrupt nach eingestellter Periodendauer
		{
								// Interrupt-Servie-Routine
    	}*/
/* Timer-Interrupt freigeben */
void timer1ms_enable ( void )
{
	CCU40_CC43->TCSET = 1; 			//starts counting
		// Interrupt Freigaben:
	CCU40_CC43->INTE |= 1;			// PME => Period match  Int. enable
	NVIC_EnableIRQ(CCU40_3_IRQn);  // Freigabe CCU40SR0
}

void timer1ms_disable ( void )
{
	CCU40_CC43->TCCLR = 1; 			//stop counting
}
//***********  Timer ISR : *******************************
//void CCU40_3_IRQHandler (void)
//{
									// Interrupt-Servie-Routine
// }

/* ---------------------------------------alte Funktion
Zeitmessfunktionen mit Timer CCU40_CC43
kann anstelle der 1ms Sek Funktion benutzt werden
Hinweis: zus�tzlich verwenden: timer_start();timer_stop();timer_value();
void timer_init(uint8_t teiler)
{	teiler %= 0x10;				// max. 15
	clock_init();
	SCU_GENERAL->CCUCON |= 1;	// Enable CCU40 Modul
				// CCU43 Init:
	CCU40->GIDLC 	= 0x10F; 	// Vorteiler enable f�r alle Timer , IDLE clear
	CCU40_CC43->TC	= 0x08;		// Shadow transfer enable
	CCU40_CC43->PSC = teiler;	// Vorteiler , hier 64Mhz/2 => 32 MHzclock
	CCU40_CC43->PRS	= 0xffff;	// Period Register f�r 1ms
	CCU40->GCSS = 0b001 <<12 ; 	// Shadow transfer request  Timer
}*/
/* ---------------------------------------------------------------------
Initialisierung Timer CCU40_CC43, vorteiler 0...15 ergibt Teilung mit 2^vorteiler
                                  periode 1...65536, 64MHz-Takt
Hinweise: F�r Zeitmessungen zus�tzlich verwenden:
timer_start();timer_stop();timer_value(); timer_reset();
F�r timergesteuerte Interrupts zus�tzlich verwenden:
timer_start_interrupt();
Timer-Interrupt-Handler ins eigene Programm kopieren:
	void CCU40_3_IRQHandler (void)  // Timer-Interrupt nach eingestellter Periodendauer
		{							// Interrupt-Servie-Routine
    	} */
void timer_init(uint8_t vorteiler, uint16_t periode)
{	vorteiler %= 0x10;				// max. 15
	clock_init();
	SCU_GENERAL->CCUCON |= 1;	// Enable CCU40 Modul
				// CCU43 Init:
	CCU40->GIDLC 	= 0x10F; 	// Vorteiler enable f�r alle Timer , IDLE clear
	CCU40_CC43->TC	= 0x08;		// Shadow transfer enable
	CCU40_CC43->PSC = vorteiler;// Vorteiler , hier 64Mhz/2 => 32 MHzclock
	CCU40_CC43->PRS	= periode;	// Period Register
	CCU40->GCSS = 0b001 <<12 ; 	// Shadow transfer request  Timer
}
// Timer f�r Zeitmessung starten
void timer_start(void)
{
	CCU40_CC43->TCSET = 1; 		//starts counting
}
/* Timer starten mit Interruptfreigabe
   Timer-Interrupt-Handler ins eigene Programm kopieren:
	void CCU40_3_IRQHandler (void)  // Timer-Interrupt nach eingestellter Periodendauer
		{
								// Interrupt-Servie-Routine
    	} */
void timer_start_interrupt(void)
{
	CCU40_CC43->TCSET = 1; 		//starts counting
	// Interrupt Freigaben:
	CCU40_CC43->SRS |= 0b11;	// Service Req. Line 3
	CCU40_CC43->INTE |= 1;		// PME => Period match  Int. enable
	NVIC_EnableIRQ(CCU40_3_IRQn);  // Freigabe CCU40SR3
}

// Timer stoppen
void timer_stop(void)
{
	CCU40_CC43->TCCLR = 0x1;	// Timer stop
}
// Timerwert lesen
uint16_t timer_value(void)
{
	uint16_t wert;
	wert = CCU40_CC43->TIMER;
	return (wert);
}
// Timer zur�cksetzen
void timer_reset(void) // Timer r�cksetzen, fr�her in timer_value enthalten
{
	CCU40_CC43->TCCLR = 0x3;	// Timer stop und clear
}
// Warten bis Timer abgelaufen ist Polling)
void timer_waitFlag(void)
{
		while((CCU40_CC43->INTS & 1) == 0); 	// Countflag = 1 ??
		CCU40_CC43->SWR = 1;					// Flag reset
}
//**********************************************************************************************
//**********************************************************************************************
/* LCD Routinen f�r OLED 0.91 Zoll Display =>128 x 32 Pixel
 * Ansteuerung �ber I2C Bus , IO-Adr = 0x78
 *
 ***********************************************************************************************
 ***********************************************************************************************
 */
void lcd_init(void) // Initialisierung des Displays
{
	OLED_0in91_Init();
	// 0.Create a new image cache

	UWORD Imagesize = ((OLED_0in91_WIDTH%8==0)? (OLED_0in91_WIDTH/8): (OLED_0in91_WIDTH/8+1)) * OLED_0in91_HEIGHT;
	BlackImage = (UBYTE *) malloc(Imagesize);
	Paint_NewImage(BlackImage, OLED_0in91_HEIGHT, OLED_0in91_WIDTH, 90, BLACK);

	//1.Select Image
	Paint_SelectImage(BlackImage);
	Driver_Delay_ms(500);
}
//*********************************************

void lcd_clear     ( void ) // anzeige l�schen
{
	OLED_0in91_Clear();
}
//***********************************************
void lcd_setcursor ( uint8_t row, uint8_t column ) // row (Zeile) = 1,2
															// column (Spalte) = 1,2,3, ..11
{     // Setzen der Cursorposition

}
 //********************************************************
void lcd_print ( uint16_t zeile,  uint16_t spalte ,char  text[] ) // Textausgabe ab Cursorposition, bis '\0'
{	uint16_t xpos,ypos,i;
	char blanktext[22];
	xpos = spalte *  xpos16;	// xpos16 => mit define  f�r Font16 definiert
	ypos = zeile *   ypos16;
	spalte %=12; zeile %=2;
	//Paint_Clear(BLACK);
	for (i=0;i<strlen(text);i++) { blanktext[i]= ' ';}
	Paint_DrawString_EN( xpos,ypos, blanktext, &Font16, BLACK, BLACK);
	Paint_DrawString_EN( xpos,ypos, text, &Font16, WHITE, WHITE);
	OLED_0in91_Display(BlackImage);
}
//**************************************************************
void lcd_char ( uint16_t zeile,uint16_t spalte, char  value  ) // Ausgabe eines Zeichens an der aktuellen Cursorposition
															// Cursor Xpos = 0..11 , Ypos = 0,1
{
	uint16_t xpos,ypos;
	spalte %=12; zeile %=2;	// Max . Positionswerte
	xpos = spalte *  xpos16;	// xpos16 => mit #define  f�r Font16 definiert
	ypos = zeile *   ypos16;
	Paint_DrawChar(xpos,ypos, ' ', &Font16, BLACK, BLACK);
	Paint_DrawChar(xpos,ypos, value, &Font16, WHITE, WHITE);
	OLED_0in91_Display(BlackImage);
}
//****************************************************************
void lcd_byte ( uint16_t zeile,uint16_t spalte,uint8_t  value  ) // 0 ... 255
{
	uint8_t buffer[3];
	uint8_t n = 0;
	uint16_t xpos,ypos;

	ypos= zeile;
	xpos = spalte;
	xpos %=12; ypos %=2;	// Max . Positionswerte
	///xpos = spalte *  xpos16;	// xpos16 => mit #define  f�r Font16 definiert
	//ypos = zeile *   ypos16;
	//n=3;
	//buffer[]

	  do
	  {
	    buffer[n++] = value%10 + '0';
	  } while ((value /= 10) > 0);
	  while (n<3)   buffer[n++] = ' ';  // Rest von buffer mit blank fuellen
	  while (n > 0)                     // Ausgabe auf das Display (umgekehrt)
	  {
	    n--;
	   lcd_char(ypos,xpos, buffer[n]);
	   xpos++;
	  }
}
//*******************************************************************
//---------------------------------------------------------------
// Gibt den Integer n als 5 stelligen Dez-Wert aufs Display.
// F hrende Nullen werden zu blank.
//---------------------------------------------------------------
	void lcd_int( uint16_t zeile,uint16_t spalte, uint16_t val)
	{
	  uint8_t buffer[5];
	  uint8_t n = 0;
	  uint16_t xpos,ypos;

	  ypos= zeile;
	  xpos = spalte;
	  xpos %=12; ypos %=2;	// Max . Positionswerte

	  do
	  {
	    buffer[n++] = val%10 + '0';
	  } while ((val /= 10) > 0);
	  while (n<5)   buffer[n++] = ' ';  // Rest von buffer mit blank fuellen

	  while (n > 0)                            // Ausgabe auf das Display (umgekehrt)
	  {
	    n--;
	    lcd_char(ypos,xpos, buffer[n]);
	    xpos++;
	  }
}

//********************************************************************

void I2C_Write_Byte(uint8_t value, uint8_t Cmd)
{
  //uint8_t buf[5];
  //buf[0] = Cmd;
  //buf[1] = value;
  i2c_start();
  i2c_write(SSD1306_SLAVE_ADDR_WR);
  i2c_write(Cmd);
  i2c_write(value);
  i2c_stop();

  //I2C_MASTER_Transmit(&I2C_MASTER_0,true,SSD1306_SLAVE_ADDR_WR,buf,2,true);
 		      //while(I2C_MASTER_IsTxBusy(&I2C_MASTER_0));

}

/********************************************************************************
  function: Delay function
  note:
  Driver_Delay_ms(xms) : Delay x ms
  Driver_Delay_us(xus) : Delay x us
********************************************************************************/
void Driver_Delay_ms(uint16_t xms)
{
  delay(xms);
}

void Driver_Delay_us(int xus)
{
  for (int j = xus; j > 0; j--);
}

// Zeitverz�gerung Millisekunden
//---------------------------------------------------------------
void delay( uint16_t millisekunden)
{
	volatile uint16_t n = 0;

	while(n < millisekunden)
	{
		delay_100us(10);
		n++;
	}
}

/*******************************************************************************
function:
			Write register address and data
*******************************************************************************/

void OLED_WriteData(uint8_t Data)
{

    I2C_Write_Byte(Data,IIC_RAM);

}
 void OLED_WriteReg(uint8_t Reg)
{

    I2C_Write_Byte(Reg,IIC_CMD);

}
/*******************************************************************************
function:
			Common register initialization
*******************************************************************************/
void OLED_InitReg(void)
{
    OLED_WriteReg(0xAE);

    OLED_WriteReg(0x40);//---set low column address
    OLED_WriteReg(0xB0);//---set high column address

    OLED_WriteReg(0xC8);//-not offset

    OLED_WriteReg(0x81);
    OLED_WriteReg(0xff);

    OLED_WriteReg(0xa1);

    OLED_WriteReg(0xa6);

    OLED_WriteReg(0xa8);
    OLED_WriteReg(0x1f);

    OLED_WriteReg(0xd3);
    OLED_WriteReg(0x00);

    OLED_WriteReg(0xd5);
    OLED_WriteReg(0xf0);

    OLED_WriteReg(0xd9);
    OLED_WriteReg(0x22);

    OLED_WriteReg(0xda);
    OLED_WriteReg(0x02);

    OLED_WriteReg(0xdb);
    OLED_WriteReg(0x49);

    OLED_WriteReg(0x8d);
    OLED_WriteReg(0x14);
}

/********************************************************************************
function:
			initialization OLED
********************************************************************************/
void OLED_0in91_Init(void)
{

	i2c_init();
    //Set the initialization register
    OLED_InitReg();
    Driver_Delay_ms(200);

    //Turn on the OLED display
    OLED_WriteReg(0xaf);
}


/********************************************************************************
function:
			Clear screen
********************************************************************************/
void OLED_0in91_Clear(void)
{
    UBYTE Column,Page;
    for(Page = 0; Page < OLED_0in91_HEIGHT/8; Page++) {
        OLED_WriteReg(0xb0 + Page);    //Set page address
        OLED_WriteReg(0x00);           //Set display position - column low address
        OLED_WriteReg(0x10);           //Set display position - column high address
        for(Column = 0; Column < OLED_0in91_WIDTH; Column++)
            OLED_WriteData(0x00);
    }
    Paint_Clear(BLACK);
}

/******************************************************************************
function: Draw Pixels
parameter:
    Xpoint : At point X
    Ypoint : At point Y
    Color  : Painted colors
******************************************************************************/
void Paint_SetPixel(UWORD Xpoint, UWORD Ypoint, UWORD Color)
{
	 UWORD X, Y;

    if(Xpoint > Paint.Width || Ypoint > Paint.Height){  return; }

    X = Xpoint;
    Y = Ypoint;

    switch(Paint.Rotate) {
    case 0:
        X = Xpoint;
        Y = Ypoint;
        break;
    case 90:
        X = Paint.WidthMemory - Ypoint - 1;
        Y = Xpoint;
        break;
    case 180:
        X = Paint.WidthMemory - Xpoint - 1;
        Y = Paint.HeightMemory - Ypoint - 1;
        break;
    case 270:
        X = Ypoint;
        Y = Paint.HeightMemory - Xpoint - 1;
        break;
    default:
        return;
    }

    switch(Paint.Mirror) {
    case MIRROR_NONE:
        break;
    case MIRROR_HORIZONTAL:
        X = Paint.WidthMemory - X - 1;
        break;
    case MIRROR_VERTICAL:
        Y = Paint.HeightMemory - Y - 1;
        break;
    case MIRROR_ORIGIN:
        X = Paint.WidthMemory - X - 1;
        Y = Paint.HeightMemory - Y - 1;
        break;
    default:
        return;
    }


    if(X > Paint.WidthMemory || Y > Paint.HeightMemory){

        return;
    }

    if(Paint.Scale == 2){
        UDOUBLE Addr = X / 8 + Y * Paint.WidthByte;
        UBYTE Rdata = Paint.Image[Addr];
        if(Color == BLACK)
            Paint.Image[Addr] = Rdata & ~(0x80 >> (X % 8));
        else
            Paint.Image[Addr] = Rdata | (0x80 >> (X % 8));
    }else if(Paint.Scale == 4){
        UDOUBLE Addr = X / 4 + Y * Paint.WidthByte;
        Color = Color % 4;//Guaranteed color scale is 4  --- 0~3
        UBYTE Rdata = Paint.Image[Addr];

        Rdata = Rdata & (~(0xC0 >> ((X % 4)*2)));
        Paint.Image[Addr] = Rdata | ((Color << 6) >> ((X % 4)*2));
    }else if(Paint.Scale == 16) {
        UDOUBLE Addr = X / 2 + Y * Paint.WidthByte;
        UBYTE Rdata = Paint.Image[Addr];
        Color = Color % 16;
        Rdata = Rdata & (~(0xf0 >> ((X % 2)*4)));
        Paint.Image[Addr] = Rdata | ((Color << 4) >> ((X % 2)*4));
    }else if(Paint.Scale == 65) {
        UDOUBLE Addr = X*2 + Y*Paint.WidthByte;
        Paint.Image[Addr] = 0xff & (Color>>8);
        Paint.Image[Addr+1] = 0xff & Color;
    }
}

/******************************************************************************
function: Clear the color of the picture
parameter:
    Color : Painted colors
******************************************************************************/
void Paint_Clear(UWORD Color)
{
    if(Paint.Scale == 2 || Paint.Scale == 4) {
        for (UWORD Y = 0; Y < Paint.HeightByte; Y++) {
            for (UWORD X = 0; X < Paint.WidthByte; X++ ) {//8 pixel =  1 byte
                UDOUBLE Addr = X + Y*Paint.WidthByte;
                Paint.Image[Addr] = Color;
            }
        }
    }else if(Paint.Scale == 16) {
        for (UWORD Y = 0; Y < Paint.HeightByte; Y++) {
            for (UWORD X = 0; X < Paint.WidthByte; X++ ) {//8 pixel =  1 byte
                UDOUBLE Addr = X + Y*Paint.WidthByte;
                Color = Color & 0x0f;
                Paint.Image[Addr] = (Color<<4) | Color;
            }
        }
    }else if(Paint.Scale == 65) {
        for (UWORD Y = 0; Y < Paint.HeightByte; Y++) {
            for (UWORD X = 0; X < Paint.WidthByte; X++ ) {//8 pixel =  1 byte
                UDOUBLE Addr = X*2 + Y*Paint.WidthByte;
                Paint.Image[Addr] = 0x0f & (Color>>8);
                Paint.Image[Addr+1] = 0x0f & Color;
            }
        }
    }
}


/******************************************************************************
function: Display the string
parameter:
    Xstart           ?X coordinate
    Ystart           ?Y coordinate
    pString          ?The first address of the English string to be displayed
    Font             ?A structure pointer that displays a character size
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
******************************************************************************/
void Paint_DrawString_EN(UWORD Xstart, UWORD Ystart, const char * pString,
                         sFONT * Font, UWORD Color_Foreground, UWORD Color_Background)
{
    UWORD Xpoint = Xstart;
    UWORD Ypoint = Ystart;

    if (Xstart > Paint.Width || Ystart > Paint.Height) {
        //Debug("Paint_DrawString_EN Input exceeds the normal display range\r\n");
        return;
    }

    while (* pString != '\0') {
        //if X direction filled , reposition to(Xstart,Ypoint),Ypoint is Y direction plus the Height of the character
        if ((Xpoint + Font->Width ) > Paint.Width ) {
            Xpoint = Xstart;
            Ypoint += Font->Height;
        }

        // If the Y direction is full, reposition to(Xstart, Ystart)
        if ((Ypoint  + Font->Height ) > Paint.Height ) {
            Xpoint = Xstart;
            Ypoint = Ystart;
        }
        Paint_DrawChar(Xpoint, Ypoint, * pString, Font, Color_Background, Color_Foreground);

        //The next character of the address
        pString ++;

        //The next word of the abscissa increases the font of the broadband
        Xpoint += Font->Width;
    }
}


/******************************************************************************
function: Show English characters
parameter:
    Xpoint           ?X coordinate
    Ypoint           ?Y coordinate
    Acsii_Char       ?To display the English characters
    Font             ?A structure pointer that displays a character size
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
******************************************************************************/
void Paint_DrawChar(UWORD Xpoint, UWORD Ypoint, const char Acsii_Char,
                    sFONT* Font, UWORD Color_Foreground, UWORD Color_Background)
{

  UWORD Page, Column;

  if (Xpoint > Paint.Width || Ypoint > Paint.Height) {
    //Debug("Paint_DrawChar Input exceeds the normal display range\r\n");
    return;
  }
  uint32_t Char_Offset = (Acsii_Char - ' ') * Font->Height * (Font->Width / 8 + (Font->Width % 8 ? 1 : 0));
  const unsigned char *ptr = &Font->table[Char_Offset];

  for ( Page = 0; Page < Font->Height; Page ++ ) {
    for ( Column = 0; Column < Font->Width; Column ++ ) {

      //To determine whether the font background color and screen background color is consistent
      if (FONT_BACKGROUND == Color_Background) { //this process is to speed up the scan
        if (*ptr & (0x80 >> (Column % 8)))
          Paint_SetPixel (Xpoint + Column, Ypoint + Page, Color_Foreground );
      } else {
        if (*ptr & (0x80 >> (Column % 8))) {
          Paint_SetPixel (Xpoint + Column, Ypoint + Page, Color_Foreground );
        } else {
          Paint_SetPixel (Xpoint + Column, Ypoint + Page, Color_Background );
        }
      }
      //One pixel is 8 bits
      if (Column % 8 == 7) {
        ptr++;
      }
    }/* Write a line */
    if (Font->Width % 8 != 0) {
      ptr++;
    }
  }/* Write all */
}


/********************************************************************************
function:
			Update all memory to OLED
********************************************************************************/
void OLED_0in91_Display(const UBYTE *Image)
{
    UBYTE Column,Page;
    UWORD temp;
    for(Page = 0; Page < OLED_0in91_HEIGHT/8; Page++) {
        OLED_WriteReg(0xb0 + Page);
        OLED_WriteReg(0x00);
        OLED_WriteReg(0x10);
        for(Column = 0; Column < OLED_0in91_WIDTH; Column++) {
            temp = Image[(3-Page) + Column*4];
            OLED_WriteData(temp);
        }
    }
}

/********************************************************************************
function:
      Update all memory to OLED
********************************************************************************/
void OLED_0in91_Display_Array(const UBYTE *Image)
{
    UBYTE Column,Page;
    UWORD temp;
    for(Page = 0; Page < OLED_0in91_HEIGHT/8; Page++) {
        OLED_WriteReg(0xb0 + Page);
        OLED_WriteReg(0x00);
        OLED_WriteReg(0x10);
        for(Column = 0; Column < OLED_0in91_WIDTH; Column++) {
            temp = Image[(3-Page) + Column*4];
            OLED_WriteData(temp);
        }
    }
}


/******************************************************************************
function: Create Image
parameter:
    image   :   Pointer to the image cache
    width   :   The width of the picture
    Height  :   The height of the picture
    Color   :   Whether the picture is inverted
******************************************************************************/
void Paint_NewImage(UBYTE *image, UWORD Width, UWORD Height, UWORD Rotate, UWORD Color)
{
    Paint.Image = NULL;
    Paint.Image = image;

    Paint.WidthMemory = Width;
    Paint.HeightMemory = Height;
    Paint.Color = Color;
    Paint.Scale = 2;

    Paint.WidthByte = (Width % 8 == 0)? (Width / 8 ): (Width / 8 + 1);
    Paint.HeightByte = Height;
//    printf("WidthByte = %d, HeightByte = %d\r\n", Paint.WidthByte, Paint.HeightByte);
//    printf(" EPD_WIDTH / 8 = %d\r\n",  122 / 8);

    Paint.Rotate = Rotate;
    Paint.Mirror = MIRROR_NONE;

    if(Rotate == ROTATE_0 || Rotate == ROTATE_180) {
        Paint.Width = Width;
        Paint.Height = Height;
    } else {
        Paint.Width = Height;
        Paint.Height = Width;
    }
}

/******************************************************************************
function: Select Image
parameter:
    image : Pointer to the image cache
******************************************************************************/
void Paint_SelectImage(UBYTE *image)
{
    Paint.Image = image;
}


//
//  Font data for Courier New 16pt
//

const uint8_t Font16_Table[]  =
{
	// @0 ' ' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @32 '!' (11 pixels wide)
	0x00, 0x00, //
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x00, 0x00, //
	0x0C, 0x00, //     ##
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @64 '"' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x1D, 0xC0, //    ### ###
	0x1D, 0xC0, //    ### ###
	0x08, 0x80, //     #   #
	0x08, 0x80, //     #   #
	0x08, 0x80, //     #   #
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @96 '#' (11 pixels wide)
	0x00, 0x00, //
	0x0D, 0x80, //     ## ##
	0x0D, 0x80, //     ## ##
	0x0D, 0x80, //     ## ##
	0x0D, 0x80, //     ## ##
	0x3F, 0xC0, //   ########
	0x1B, 0x00, //    ## ##
	0x3F, 0xC0, //   ########
	0x1B, 0x00, //    ## ##
	0x1B, 0x00, //    ## ##
	0x1B, 0x00, //    ## ##
	0x1B, 0x00, //    ## ##
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @128 '$' (11 pixels wide)
	0x04, 0x00, //      #
	0x1F, 0x80, //    ######
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x38, 0x00, //   ###
	0x1E, 0x00, //    ####
	0x0F, 0x00, //     ####
	0x03, 0x80, //       ###
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x3F, 0x00, //   ######
	0x04, 0x00, //      #
	0x04, 0x00, //      #
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @160 '%' (11 pixels wide)
	0x00, 0x00, //
	0x18, 0x00, //    ##
	0x24, 0x00, //   #  #
	0x24, 0x00, //   #  #
	0x18, 0xC0, //    ##   ##
	0x07, 0x80, //      ####
	0x1E, 0x00, //    ####
	0x31, 0x80, //   ##   ##
	0x02, 0x40, //       #  #
	0x02, 0x40, //       #  #
	0x01, 0x80, //        ##
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @192 '&' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x0F, 0x00, //     ####
	0x18, 0x00, //    ##
	0x18, 0x00, //    ##
	0x18, 0x00, //    ##
	0x0C, 0x00, //     ##
	0x1D, 0x80, //    ### ##
	0x37, 0x00, //   ## ###
	0x33, 0x00, //   ##  ##
	0x1D, 0x80, //    ### ##
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @224 ''' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x07, 0x00, //      ###
	0x07, 0x00, //      ###
	0x02, 0x00, //       #
	0x02, 0x00, //       #
	0x02, 0x00, //       #
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @256 '(' (11 pixels wide)
	0x00, 0x00, //
	0x03, 0x00, //       ##
	0x03, 0x00, //       ##
	0x06, 0x00, //      ##
	0x0E, 0x00, //     ###
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x0E, 0x00, //     ###
	0x06, 0x00, //      ##
	0x03, 0x00, //       ##
	0x03, 0x00, //       ##
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @288 ')' (11 pixels wide)
	0x00, 0x00, //
	0x18, 0x00, //    ##
	0x18, 0x00, //    ##
	0x0C, 0x00, //     ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x0C, 0x00, //     ##
	0x1C, 0x00, //    ###
	0x18, 0x00, //    ##
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @320 '*' (11 pixels wide)
	0x00, 0x00, //
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x3F, 0xC0, //   ########
	0x3F, 0xC0, //   ########
	0x0F, 0x00, //     ####
	0x1F, 0x80, //    ######
	0x19, 0x80, //    ##  ##
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @352 '+' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x04, 0x00, //      #
	0x04, 0x00, //      #
	0x04, 0x00, //      #
	0x3F, 0x80, //   #######
	0x04, 0x00, //      #
	0x04, 0x00, //      #
	0x04, 0x00, //      #
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @384 ',' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x06, 0x00, //      ##
	0x04, 0x00, //      #
	0x0C, 0x00, //     ##
	0x08, 0x00, //     #
	0x08, 0x00, //     #
	0x00, 0x00, //
	0x00, 0x00, //

	// @416 '-' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x3F, 0x80, //   #######
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @448 '.' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @480 '/' (11 pixels wide)
	0x00, 0xC0, //         ##
	0x00, 0xC0, //         ##
	0x01, 0x80, //        ##
	0x01, 0x80, //        ##
	0x03, 0x00, //       ##
	0x03, 0x00, //       ##
	0x06, 0x00, //      ##
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x18, 0x00, //    ##
	0x18, 0x00, //    ##
	0x30, 0x00, //   ##
	0x30, 0x00, //   ##
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @512 '0' (11 pixels wide)
	0x00, 0x00, //
	0x0E, 0x00, //     ###
	0x1B, 0x00, //    ## ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x1B, 0x00, //    ## ##
	0x0E, 0x00, //     ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @544 '1' (11 pixels wide)
	0x00, 0x00, //
	0x06, 0x00, //      ##
	0x3E, 0x00, //   #####
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x3F, 0xC0, //   ########
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @576 '2' (11 pixels wide)
	0x00, 0x00, //
	0x0F, 0x00, //     ####
	0x19, 0x80, //    ##  ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x03, 0x00, //       ##
	0x06, 0x00, //      ##
	0x0C, 0x00, //     ##
	0x18, 0x00, //    ##
	0x30, 0x00, //   ##
	0x3F, 0x80, //   #######
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @608 '3' (11 pixels wide)
	0x00, 0x00, //
	0x3F, 0x00, //   ######
	0x61, 0x80, //  ##    ##
	0x01, 0x80, //        ##
	0x03, 0x00, //       ##
	0x1F, 0x00, //    #####
	0x03, 0x80, //       ###
	0x01, 0x80, //        ##
	0x01, 0x80, //        ##
	0x61, 0x80, //  ##    ##
	0x3F, 0x00, //   ######
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @640 '4' (11 pixels wide)
	0x00, 0x00, //
	0x07, 0x00, //      ###
	0x07, 0x00, //      ###
	0x0F, 0x00, //     ####
	0x0B, 0x00, //     # ##
	0x1B, 0x00, //    ## ##
	0x13, 0x00, //    #  ##
	0x33, 0x00, //   ##  ##
	0x3F, 0x80, //   #######
	0x03, 0x00, //       ##
	0x0F, 0x80, //     #####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @672 '5' (11 pixels wide)
	0x00, 0x00, //
	0x1F, 0x80, //    ######
	0x18, 0x00, //    ##
	0x18, 0x00, //    ##
	0x18, 0x00, //    ##
	0x1F, 0x00, //    #####
	0x11, 0x80, //    #   ##
	0x01, 0x80, //        ##
	0x01, 0x80, //        ##
	0x21, 0x80, //   #    ##
	0x1F, 0x00, //    #####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @704 '6' (11 pixels wide)
	0x00, 0x00, //
	0x07, 0x80, //      ####
	0x1C, 0x00, //    ###
	0x18, 0x00, //    ##
	0x30, 0x00, //   ##
	0x37, 0x00, //   ## ###
	0x39, 0x80, //   ###  ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x19, 0x80, //    ##  ##
	0x0F, 0x00, //     ####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @736 '7' (11 pixels wide)
	0x00, 0x00, //
	0x7F, 0x00, //  #######
	0x43, 0x00, //  #    ##
	0x03, 0x00, //       ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @768 '8' (11 pixels wide)
	0x00, 0x00, //
	0x1F, 0x00, //    #####
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x1F, 0x00, //    #####
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x1F, 0x00, //    #####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @800 '9' (11 pixels wide)
	0x00, 0x00, //
	0x1E, 0x00, //    ####
	0x33, 0x00, //   ##  ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x33, 0x80, //   ##  ###
	0x1D, 0x80, //    ### ##
	0x01, 0x80, //        ##
	0x03, 0x00, //       ##
	0x07, 0x00, //      ###
	0x3C, 0x00, //   ####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @832 ':' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @864 ';' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x03, 0x00, //       ##
	0x03, 0x00, //       ##
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x06, 0x00, //      ##
	0x04, 0x00, //      #
	0x08, 0x00, //     #
	0x08, 0x00, //     #
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @896 '<' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0xC0, //         ##
	0x03, 0x00, //       ##
	0x04, 0x00, //      #
	0x18, 0x00, //    ##
	0x60, 0x00, //  ##
	0x18, 0x00, //    ##
	0x04, 0x00, //      #
	0x03, 0x00, //       ##
	0x00, 0xC0, //         ##
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @928 '=' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x7F, 0xC0, //  #########
	0x00, 0x00, //
	0x7F, 0xC0, //  #########
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @960 '>' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x60, 0x00, //  ##
	0x18, 0x00, //    ##
	0x04, 0x00, //      #
	0x03, 0x00, //       ##
	0x00, 0xC0, //         ##
	0x03, 0x00, //       ##
	0x04, 0x00, //      #
	0x18, 0x00, //    ##
	0x60, 0x00, //  ##
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @992 '?' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x1F, 0x00, //    #####
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x01, 0x80, //        ##
	0x07, 0x00, //      ###
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x00, 0x00, //
	0x0C, 0x00, //     ##
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1024 '@' (11 pixels wide)
	0x00, 0x00, //
	0x0E, 0x00, //     ###
	0x11, 0x00, //    #   #
	0x21, 0x00, //   #    #
	0x21, 0x00, //   #    #
	0x27, 0x00, //   #  ###
	0x29, 0x00, //   # #  #
	0x29, 0x00, //   # #  #
	0x27, 0x00, //   #  ###
	0x20, 0x00, //   #
	0x11, 0x00, //    #   #
	0x0E, 0x00, //     ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1056 'A' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x3F, 0x00, //   ######
	0x0F, 0x00, //     ####
	0x09, 0x00, //     #  #
	0x19, 0x80, //    ##  ##
	0x19, 0x80, //    ##  ##
	0x1F, 0x80, //    ######
	0x30, 0xC0, //   ##    ##
	0x30, 0xC0, //   ##    ##
	0x79, 0xE0, //  ####  ####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1088 'B' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x7F, 0x00, //  #######
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x3F, 0x00, //   ######
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x7F, 0x00, //  #######
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1120 'C' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x1F, 0x40, //    ##### #
	0x30, 0xC0, //   ##    ##
	0x60, 0x40, //  ##      #
	0x60, 0x00, //  ##
	0x60, 0x00, //  ##
	0x60, 0x00, //  ##
	0x60, 0x40, //  ##      #
	0x30, 0x80, //   ##    #
	0x1F, 0x00, //    #####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1152 'D' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x7F, 0x00, //  #######
	0x31, 0x80, //   ##   ##
	0x30, 0xC0, //   ##    ##
	0x30, 0xC0, //   ##    ##
	0x30, 0xC0, //   ##    ##
	0x30, 0xC0, //   ##    ##
	0x30, 0xC0, //   ##    ##
	0x31, 0x80, //   ##   ##
	0x7F, 0x00, //  #######
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1184 'E' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x7F, 0x80, //  ########
	0x30, 0x80, //   ##    #
	0x30, 0x80, //   ##    #
	0x32, 0x00, //   ##  #
	0x3E, 0x00, //   #####
	0x32, 0x00, //   ##  #
	0x30, 0x80, //   ##    #
	0x30, 0x80, //   ##    #
	0x7F, 0x80, //  ########
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1216 'F' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x7F, 0xC0, //  #########
	0x30, 0x40, //   ##     #
	0x30, 0x40, //   ##     #
	0x32, 0x00, //   ##  #
	0x3E, 0x00, //   #####
	0x32, 0x00, //   ##  #
	0x30, 0x00, //   ##
	0x30, 0x00, //   ##
	0x7C, 0x00, //  #####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1248 'G' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x1E, 0x80, //    #### #
	0x31, 0x80, //   ##   ##
	0x60, 0x80, //  ##     #
	0x60, 0x00, //  ##
	0x60, 0x00, //  ##
	0x67, 0xC0, //  ##  #####
	0x61, 0x80, //  ##    ##
	0x31, 0x80, //   ##   ##
	0x1F, 0x00, //    #####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1280 'H' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x7B, 0xC0, //  #### ####
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x3F, 0x80, //   #######
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x7B, 0xC0, //  #### ####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1312 'I' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x3F, 0xC0, //   ########
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x3F, 0xC0, //   ########
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1344 'J' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x1F, 0xC0, //    #######
	0x03, 0x00, //       ##
	0x03, 0x00, //       ##
	0x03, 0x00, //       ##
	0x03, 0x00, //       ##
	0x63, 0x00, //  ##   ##
	0x63, 0x00, //  ##   ##
	0x63, 0x00, //  ##   ##
	0x3E, 0x00, //   #####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1376 'K' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x7B, 0xC0, //  #### ####
	0x31, 0x80, //   ##   ##
	0x33, 0x00, //   ##  ##
	0x36, 0x00, //   ## ##
	0x3C, 0x00, //   ####
	0x3E, 0x00, //   #####
	0x33, 0x00, //   ##  ##
	0x31, 0x80, //   ##   ##
	0x79, 0xC0, //  ####  ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1408 'L' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x7E, 0x00, //  ######
	0x18, 0x00, //    ##
	0x18, 0x00, //    ##
	0x18, 0x00, //    ##
	0x18, 0x00, //    ##
	0x18, 0x40, //    ##    #
	0x18, 0x40, //    ##    #
	0x18, 0x40, //    ##    #
	0x7F, 0xC0, //  #########
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1440 'M' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0xE0, 0xE0, // ###     ###
	0x60, 0xC0, //  ##     ##
	0x71, 0xC0, //  ###   ###
	0x7B, 0xC0, //  #### ####
	0x6A, 0xC0, //  ## # # ##
	0x6E, 0xC0, //  ## ### ##
	0x64, 0xC0, //  ##  #  ##
	0x60, 0xC0, //  ##     ##
	0xFB, 0xE0, // ##### #####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1472 'N' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x73, 0xC0, //  ###  ####
	0x31, 0x80, //   ##   ##
	0x39, 0x80, //   ###  ##
	0x3D, 0x80, //   #### ##
	0x35, 0x80, //   ## # ##
	0x37, 0x80, //   ## ####
	0x33, 0x80, //   ##  ###
	0x31, 0x80, //   ##   ##
	0x79, 0x80, //  ####  ##
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1504 'O' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x1F, 0x00, //    #####
	0x31, 0x80, //   ##   ##
	0x60, 0xC0, //  ##     ##
	0x60, 0xC0, //  ##     ##
	0x60, 0xC0, //  ##     ##
	0x60, 0xC0, //  ##     ##
	0x60, 0xC0, //  ##     ##
	0x31, 0x80, //   ##   ##
	0x1F, 0x00, //    #####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1536 'P' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x7F, 0x00, //  #######
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x3F, 0x00, //   ######
	0x30, 0x00, //   ##
	0x30, 0x00, //   ##
	0x7E, 0x00, //  ######
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1568 'Q' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x1F, 0x00, //    #####
	0x31, 0x80, //   ##   ##
	0x60, 0xC0, //  ##     ##
	0x60, 0xC0, //  ##     ##
	0x60, 0xC0, //  ##     ##
	0x60, 0xC0, //  ##     ##
	0x60, 0xC0, //  ##     ##
	0x31, 0x80, //   ##   ##
	0x1F, 0x00, //    #####
	0x0C, 0xC0, //     ##  ##
	0x1F, 0x80, //    ######
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1600 'R' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x7F, 0x00, //  #######
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x3E, 0x00, //   #####
	0x33, 0x00, //   ##  ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x7C, 0xE0, //  #####  ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1632 'S' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x1F, 0x80, //    ######
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x38, 0x00, //   ###
	0x1F, 0x00, //    #####
	0x03, 0x80, //       ###
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x3F, 0x00, //   ######
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1664 'T' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x7F, 0x80, //  ########
	0x4C, 0x80, //  #  ##  #
	0x4C, 0x80, //  #  ##  #
	0x4C, 0x80, //  #  ##  #
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x3F, 0x00, //   ######
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1696 'U' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x7B, 0xC0, //  #### ####
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x1F, 0x00, //    #####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1728 'V' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x7B, 0xC0, //  #### ####
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x1B, 0x00, //    ## ##
	0x1B, 0x00, //    ## ##
	0x1B, 0x00, //    ## ##
	0x0A, 0x00, //     # #
	0x0E, 0x00, //     ###
	0x0E, 0x00, //     ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1760 'W' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0xFB, 0xE0, // ##### #####
	0x60, 0xC0, //  ##     ##
	0x64, 0xC0, //  ##  #  ##
	0x6E, 0xC0, //  ## ### ##
	0x6E, 0xC0, //  ## ### ##
	0x2A, 0x80, //   # # # #
	0x3B, 0x80, //   ### ###
	0x3B, 0x80, //   ### ###
	0x31, 0x80, //   ##   ##
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1792 'X' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x7B, 0xC0, //  #### ####
	0x31, 0x80, //   ##   ##
	0x1B, 0x00, //    ## ##
	0x0E, 0x00, //     ###
	0x0E, 0x00, //     ###
	0x0E, 0x00, //     ###
	0x1B, 0x00, //    ## ##
	0x31, 0x80, //   ##   ##
	0x7B, 0xC0, //  #### ####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1824 'Y' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x79, 0xE0, //  ####  ####
	0x30, 0xC0, //   ##    ##
	0x19, 0x80, //    ##  ##
	0x0F, 0x00, //     ####
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x1F, 0x80, //    ######
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1856 'Z' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x3F, 0x80, //   #######
	0x21, 0x80, //   #    ##
	0x23, 0x00, //   #   ##
	0x06, 0x00, //      ##
	0x04, 0x00, //      #
	0x0C, 0x00, //     ##
	0x18, 0x80, //    ##   #
	0x30, 0x80, //   ##    #
	0x3F, 0x80, //   #######
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1888 '[' (11 pixels wide)
	0x00, 0x00, //
	0x07, 0x80, //      ####
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x07, 0x80, //      ####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1920 '\' (11 pixels wide)
	0x30, 0x00, //   ##
	0x30, 0x00, //   ##
	0x18, 0x00, //    ##
	0x18, 0x00, //    ##
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x06, 0x00, //      ##
	0x03, 0x00, //       ##
	0x03, 0x00, //       ##
	0x01, 0x80, //        ##
	0x01, 0x80, //        ##
	0x00, 0xC0, //         ##
	0x00, 0xC0, //         ##
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1952 ']' (11 pixels wide)
	0x00, 0x00, //
	0x1E, 0x00, //    ####
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x1E, 0x00, //    ####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @1984 '^' (11 pixels wide)
	0x04, 0x00, //      #
	0x0A, 0x00, //     # #
	0x0A, 0x00, //     # #
	0x11, 0x00, //    #   #
	0x20, 0x80, //   #     #
	0x20, 0x80, //   #     #
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2016 '_' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0xFF, 0xE0, // ###########

	// @2048 '`' (11 pixels wide)
	0x08, 0x00, //     #
	0x04, 0x00, //      #
	0x02, 0x00, //       #
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2080 'a' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x1F, 0x00, //    #####
	0x01, 0x80, //        ##
	0x01, 0x80, //        ##
	0x1F, 0x80, //    ######
	0x31, 0x80, //   ##   ##
	0x33, 0x80, //   ##  ###
	0x1D, 0xC0, //    ### ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2112 'b' (11 pixels wide)
	0x00, 0x00, //
	0x70, 0x00, //  ###
	0x30, 0x00, //   ##
	0x30, 0x00, //   ##
	0x37, 0x00, //   ## ###
	0x39, 0x80, //   ###  ##
	0x30, 0xC0, //   ##    ##
	0x30, 0xC0, //   ##    ##
	0x30, 0xC0, //   ##    ##
	0x39, 0x80, //   ###  ##
	0x77, 0x00, //  ### ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2144 'c' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x1E, 0x80, //    #### #
	0x31, 0x80, //   ##   ##
	0x60, 0x80, //  ##     #
	0x60, 0x00, //  ##
	0x60, 0x80, //  ##     #
	0x31, 0x80, //   ##   ##
	0x1F, 0x00, //    #####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2176 'd' (11 pixels wide)
	0x00, 0x00, //
	0x03, 0x80, //       ###
	0x01, 0x80, //        ##
	0x01, 0x80, //        ##
	0x1D, 0x80, //    ### ##
	0x33, 0x80, //   ##  ###
	0x61, 0x80, //  ##    ##
	0x61, 0x80, //  ##    ##
	0x61, 0x80, //  ##    ##
	0x33, 0x80, //   ##  ###
	0x1D, 0xC0, //    ### ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2208 'e' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x1F, 0x00, //    #####
	0x31, 0x80, //   ##   ##
	0x60, 0xC0, //  ##     ##
	0x7F, 0xC0, //  #########
	0x60, 0x00, //  ##
	0x30, 0xC0, //   ##    ##
	0x1F, 0x80, //    ######
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2240 'f' (11 pixels wide)
	0x00, 0x00, //
	0x07, 0xE0, //      ######
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x3F, 0x80, //   #######
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x3F, 0x80, //   #######
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2272 'g' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x1D, 0xC0, //    ### ###
	0x33, 0x80, //   ##  ###
	0x61, 0x80, //  ##    ##
	0x61, 0x80, //  ##    ##
	0x61, 0x80, //  ##    ##
	0x33, 0x80, //   ##  ###
	0x1D, 0x80, //    ### ##
	0x01, 0x80, //        ##
	0x01, 0x80, //        ##
	0x1F, 0x00, //    #####
	0x00, 0x00, //
	0x00, 0x00, //

	// @2304 'h' (11 pixels wide)
	0x00, 0x00, //
	0x70, 0x00, //  ###
	0x30, 0x00, //   ##
	0x30, 0x00, //   ##
	0x37, 0x00, //   ## ###
	0x39, 0x80, //   ###  ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x7B, 0xC0, //  #### ####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2336 'i' (11 pixels wide)
	0x00, 0x00, //
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x00, 0x00, //
	0x1E, 0x00, //    ####
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x3F, 0xC0, //   ########
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2368 'j' (11 pixels wide)
	0x00, 0x00, //
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x00, 0x00, //
	0x3F, 0x00, //   ######
	0x03, 0x00, //       ##
	0x03, 0x00, //       ##
	0x03, 0x00, //       ##
	0x03, 0x00, //       ##
	0x03, 0x00, //       ##
	0x03, 0x00, //       ##
	0x03, 0x00, //       ##
	0x03, 0x00, //       ##
	0x3E, 0x00, //   #####
	0x00, 0x00, //
	0x00, 0x00, //

	// @2400 'k' (11 pixels wide)
	0x00, 0x00, //
	0x70, 0x00, //  ###
	0x30, 0x00, //   ##
	0x30, 0x00, //   ##
	0x37, 0x80, //   ## ####
	0x36, 0x00, //   ## ##
	0x3C, 0x00, //   ####
	0x3C, 0x00, //   ####
	0x36, 0x00, //   ## ##
	0x33, 0x00, //   ##  ##
	0x77, 0xC0, //  ### #####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2432 'l' (11 pixels wide)
	0x00, 0x00, //
	0x1E, 0x00, //    ####
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x3F, 0xC0, //   ########
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2464 'm' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x7F, 0x80, //  ########
	0x36, 0xC0, //   ## ## ##
	0x36, 0xC0, //   ## ## ##
	0x36, 0xC0, //   ## ## ##
	0x36, 0xC0, //   ## ## ##
	0x36, 0xC0, //   ## ## ##
	0x76, 0xE0, //  ### ## ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2496 'n' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x77, 0x00, //  ### ###
	0x39, 0x80, //   ###  ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x7B, 0xC0, //  #### ####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2528 'o' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x1F, 0x00, //    #####
	0x31, 0x80, //   ##   ##
	0x60, 0xC0, //  ##     ##
	0x60, 0xC0, //  ##     ##
	0x60, 0xC0, //  ##     ##
	0x31, 0x80, //   ##   ##
	0x1F, 0x00, //    #####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2560 'p' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x77, 0x00, //  ### ###
	0x39, 0x80, //   ###  ##
	0x30, 0xC0, //   ##    ##
	0x30, 0xC0, //   ##    ##
	0x30, 0xC0, //   ##    ##
	0x39, 0x80, //   ###  ##
	0x37, 0x00, //   ## ###
	0x30, 0x00, //   ##
	0x30, 0x00, //   ##
	0x7C, 0x00, //  #####
	0x00, 0x00, //
	0x00, 0x00, //

	// @2592 'q' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x1D, 0xC0, //    ### ###
	0x33, 0x80, //   ##  ###
	0x61, 0x80, //  ##    ##
	0x61, 0x80, //  ##    ##
	0x61, 0x80, //  ##    ##
	0x33, 0x80, //   ##  ###
	0x1D, 0x80, //    ### ##
	0x01, 0x80, //        ##
	0x01, 0x80, //        ##
	0x07, 0xC0, //      #####
	0x00, 0x00, //
	0x00, 0x00, //

	// @2624 'r' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x7B, 0x80, //  #### ###
	0x1C, 0xC0, //    ###  ##
	0x18, 0x00, //    ##
	0x18, 0x00, //    ##
	0x18, 0x00, //    ##
	0x18, 0x00, //    ##
	0x7F, 0x00, //  #######
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2656 's' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x1F, 0x80, //    ######
	0x31, 0x80, //   ##   ##
	0x3C, 0x00, //   ####
	0x1F, 0x00, //    #####
	0x03, 0x80, //       ###
	0x31, 0x80, //   ##   ##
	0x3F, 0x00, //   ######
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2688 't' (11 pixels wide)
	0x00, 0x00, //
	0x18, 0x00, //    ##
	0x18, 0x00, //    ##
	0x18, 0x00, //    ##
	0x7F, 0x00, //  #######
	0x18, 0x00, //    ##
	0x18, 0x00, //    ##
	0x18, 0x00, //    ##
	0x18, 0x00, //    ##
	0x18, 0x80, //    ##   #
	0x0F, 0x00, //     ####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2720 'u' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x73, 0x80, //  ###  ###
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x33, 0x80, //   ##  ###
	0x1D, 0xC0, //    ### ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2752 'v' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x7B, 0xC0, //  #### ####
	0x31, 0x80, //   ##   ##
	0x31, 0x80, //   ##   ##
	0x1B, 0x00, //    ## ##
	0x1B, 0x00, //    ## ##
	0x0E, 0x00, //     ###
	0x0E, 0x00, //     ###
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2784 'w' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0xF1, 0xE0, // ####   ####
	0x60, 0xC0, //  ##     ##
	0x64, 0xC0, //  ##  #  ##
	0x6E, 0xC0, //  ## ### ##
	0x3B, 0x80, //   ### ###
	0x3B, 0x80, //   ### ###
	0x31, 0x80, //   ##   ##
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //



	// @2816 'x' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x7B, 0xC0, //  #### ####
	0x1B, 0x00, //    ## ##
	0x0E, 0x00, //     ###
	0x0E, 0x00, //     ###
	0x0E, 0x00, //     ###
	0x1B, 0x00, //    ## ##
	0x7B, 0xC0, //  #### ####
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2848 'y' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x79, 0xE0, //  ####  ####
	0x30, 0xC0, //   ##    ##
	0x19, 0x80, //    ##  ##
	0x19, 0x80, //    ##  ##
	0x0B, 0x00, //     # ##
	0x0F, 0x00, //     ####
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x0C, 0x00, //     ##
	0x3E, 0x00, //   #####
	0x00, 0x00, //
	0x00, 0x00, //

	// @2880 'z' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x3F, 0x80, //   #######
	0x21, 0x80, //   #    ##

	0x03, 0x00, //       ##
	0x0E, 0x00, //     ###
	0x18, 0x00, //    ##
	0x30, 0x80, //   ##    #
	0x3F, 0x80, //   #######
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2912 '{' (11 pixels wide)
	0x00, 0x00, //
	0x06, 0x00, //      ##
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x18, 0x00, //    ##
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x0C, 0x00, //     ##
	0x06, 0x00, //      ##
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2944 '|' (11 pixels wide)
	0x00, 0x00, //
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @2976 '}' (11 pixels wide)
	0x00, 0x00, //
	0x0C, 0x00, //     ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x03, 0x00, //       ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x06, 0x00, //      ##
	0x0C, 0x00, //     ##
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //

	// @3008 '~' (11 pixels wide)
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x18, 0x00, //    ##
	0x24, 0x80, //   #  #  #
	0x03, 0x00, //       ##
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
	0x00, 0x00, //
};

sFONT Font16 = {
  Font16_Table,
  11, /* Width */
  16, /* Height */
};


