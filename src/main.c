// 1. Header-Dateien (Bibliotheken einbinden)
#include "xmc_common.h"
#include "xmc2go_lib.h"

// 2. Makros und Konstanten (optional)

// 3. Funktionsprototypen (optional, falls Funktionen unter main stehen)
void InitFunction(void); // Initialisierung

// 4. Globale Variablen deklarieren außerhalb main
uint8_t ui8zaehler = 0;
uint16_t ui16zaehler, wert; // mit uint8_t können wir Pin 8/9 nicht ansteuern
unsigned long ulWait;
char text1[] = "byte: ", text2[] = "uint:";

// 5. Hauptfunktion (Einstiegspunkt des Programms)

int main(void) {
    // 6. Lokale Variablen deklarieren innerhalb main

    // 7. Programm-Logik
    InitFunction();
    
    while (1U)
    {
        wert = adc_in(0); // Poti einlesen
        PORT0->OUT = (wert>>7) << 5; // 5 Bit-Wert ab P0.5 anzeigen
        // Zeile 1
        lcd_print(0,0,text1);
        lcd_byte(0,6,ui8zaehler); // 8 Bit !
        // Zeile 2
        lcd_print(1,0,text2);
        lcd_int(1,6,wert); // 16 Bit !

        delay_ms(150);
        ui8zaehler--;
        
        // LEDs
        //  Port0, Pin1-4 auf 0 setzen, Pin 0/5-9 bleiben wie gehabt
        PORT0->OUT = ui16zaehler & 0b1111111111100001;

        // Einschalten Port1 Pin 0 und 1, andere Pins unverändert
        // PORT1->OUT |= 0b11;

        // Toggeln der LEDs an Port1 Pin 0 und 1
        // PORT1->OUT ^= 0b11;

        // Ausschalten Port1 Pin 1
        PORT1->OUT &= 0b1111111111111101;
    }
    
    // 8. Erfolgreiche Ausführung an das Betriebssystem melden
    return 0;
}

// 5. Funktionsdefinitionen (optional)
void InitFunction(void) { // Implementierung
    // Script S. 40

    // Port 1 Pin 0/1 als Ausgang konfigurieren
    PORT1->IOCR0 = 0x00008080;
    // Port 0 Pin0 als Ausgang konfigurieren
    PORT0->IOCR0 = 0x80;
    // Port 0 Pin 5-7 als Ausgang konfigurieren
    PORT0->IOCR4 = 0x80808000;
    // Port0 Pin 8/9 als Ausgang konfigurieren
    PORT0->IOCR8 = 0x00008080;
    // Port 0 Pin 14/15 als Ausgang konfigurieren
    PORT0->IOCR12 = 0x8080000;
    // Port2 Pin 4-7 als Eingang konfigurieren
    PORT2->IOCR4 = 0x00000000;

    // LEDs von Port1 ausschalten
    PORT1->OUT = 0;
    // LEDs von Port0 ausschalten
    PORT0->OUT = 0;
    // LED an Port 1 Pin 0 einschalten
    PORT1->OUT = 0b01; // =1;
    


    // OLED initialisieren
    lcd_init();
    lcd_clear();

    // ADC initialisieren
    adc_init();
    port_init(P0,OUTP); // Port0 initialisieren über Funktion aus Bibliothek
}