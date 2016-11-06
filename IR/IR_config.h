//
// IR_config.h
//
//  Created on: 13.09.2016
//      Author: Jatsekku
//
//      Biblioteka do szeroko pojetej obslugi podczerwieni
//
//      Cechy i funkcje biblioteki:
//      - Mo¿liwosc obslugi wielu standardow
//      - Mo¿liwosc dodadawania standardow
//      - Opis mozliwosci poszczegolnych standardow
//        znajduje sie w ich plikach naglowkowych


#ifndef IR_IR_CONFIG_H_
#define IR_IR_CONFIG_H_

//------------------------------------------------------------------------------------------------------------------
//                                     *****KONFIGURACJA SPRZETOWA*****
//             Nalezy osobno zdefiniowac nazwy portów (a,b,c,d) oraz numery pinow (PC7,PB1,PF5...)
//                  Nalezy osobno zdefiniowac numer pinu oraz port do ktorego przynalezy pin!
//                        Port i pin s¹ scisle zwiazane z Timerem1 i jego trybem Capture
//------------------------------------------------------------------------------------------------------------------
#define PORT_IR_IN	D		
#define PIN_IR_IN	PD6

//------------------------------------------------------------------------------------------------------------------
//                                         *****ZBIOR STANDARDOW*****
//------------------------------------------------------------------------------------------------------------------
#define NEC 1
//#define NOWY_STANDARD 2


//------------------------------------------------------------------------------------------------------------------
//                                      *****KONFIGURACJA BIBLIOTEKI*****
//                             Nalezy wybrac jaki standard bedzie wykorzysytywany.
//------------------------------------------------------------------------------------------------------------------
#define IR_STD 1

//------------------------------------------------------------------------------------------------------------------
//                           *****DEKLARACJE, PLIKI NAGLWOKOWE, ZMIENNE GLOBALNE*****
//                                            !!!!!NIE ZMIENIAC!!!!!
//------------------------------------------------------------------------------------------------------------------


//Makro przeliczaj¹ca czas w milisekundach na liczbe taktow zegara
#define ms_pulse(ms)	F_CPU/8000 * ms
//Makra upraszczaj¹ce dostep do rejestrów portów
#include "../common.h"
//Biblioteka podstawowa AVR
#include <avr/io.h>

void IR_Init(void);		//Funkcja inicjalizujaca hardware do dekodowania podczerwieni
void IR_Event(void);	//Funkcja - zdarzenie od odebrania ramki

//------------------------------------------------------------------------------------------------------------------
//                                   *****ZBIOR STANDARDÓW - INCLUDE*****
//------------------------------------------------------------------------------------------------------------------
#include "NEC.h"
//#include "NOWY_STANDARD.h"

#endif /* IR_IR_CONFIG_H_ */
