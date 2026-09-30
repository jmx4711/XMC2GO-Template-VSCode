// 1. Header-Dateien (Bibliotheken einbinden)
#include <stdio.h>
#include <stdlib.h>

// 2. Makros und Konstanten (optional)
#define MAX_GROESSE 100

// 3. Funktionsprototypen (optional, falls Funktionen unter main stehen)
void begrüßung(void);

// 4. Hauptfunktion (Einstiegspunkt des Programms)
// int argc, char *argv[] can be omitted for "void" if not needed for shell inputs
int main(int argc, char *argv[]) {
    // Variablen deklarieren
    int status = 0;

    // Programm-Logik
    begrüßung();

    // Erfolgreiche Ausführung an das Betriebssystem melden
    return 0;
}

// 5. Funktionsdefinitionen (optional)
void begrüßung(void) {
    printf("Hallo Welt!\n");
}