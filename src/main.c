// 1. Header-Dateien (Bibliotheken einbinden)
#include "xmc_common.h"

// 2. Makros und Konstanten (optional)
#define MAX_GROESSE 100
const unsigned int constant1 = 2; 

// 3. Funktionsprototypen (optional, falls Funktionen unter main stehen)
void begrüßung(void);

// 4. Globale Variablen deklarieren außerhalb main

// 5. Hauptfunktion (Einstiegspunkt des Programms)

int main() {
    // 6. Lokale Variablen deklarieren innerhalb main
    int status = 0;

    // 7. Programm-Logik
    begrüßung();

    // 8. Erfolgreiche Ausführung an das Betriebssystem melden
    return 0;
}

// 5. Funktionsdefinitionen (optional)
void begrüßung(void) {
    
}