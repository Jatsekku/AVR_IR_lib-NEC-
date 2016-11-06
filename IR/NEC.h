//
// NEC.h
//
//  Created on: 30.08.2016
//      Author: Jatsekku
//
//      Biblioteka do dekodowania standardu NEC powsta³a na bazie pomyslu Krausera z forum.atnel.pl
//
//      Cechy i funkcje biblioteki:
//      - Mo¿liwosc wybiorczej kompilacji wybranych funkcji/opcji
//      - Obsluga oparta na zdarzeniach
//      - Licznik odebranych ramek
//      - Redundacja odebranego adresu i komendy
//      - Mo¿liwosc rozszerzenia pola danych do lacznie 32 bitow


//------------------------------------------------------------------------------------------------------------------
//                                            POSTAC RAMKI DANYCH
//	   _________________________________________________________________________________________________________
//	  |   			  							_____                                             ____          |
//    | AGC |          ADRES         |          ADRES         |          DANE          |          DANE          |
//    |_________________________________________________________________________________________________________|
//
//    ADRES, DANE - zmienne 8 bitowe
//    AGC - naglowek ramki
//------------------------------------------------------------------------------------------------------------------

#ifndef IR_NEC_H_
#define IR_NEC_H_
#if IR_STD == NEC
//------------------------------------------------------------------------------------------------------------------
//                                      *****KONFIGURACJA BIBLIOTEKI*****
//                                  1 - Modul/wariant wlaczony do kompilacji
//                                  0 - Modul/wariant wylaczony z kompilacji
//------------------------------------------------------------------------------------------------------------------
#define USE_NEC_FRAME_COUNTER 		1	//Licznik odebranych ramek
#define USE_NEC_EXTENDED_PROTOCOL	0	//Rozszerzenie przestrzeni komend i adrsu z 1 bajtu do 2 bajtów
#define USE_NEC_REDUNDANCY_CHECK 	1	//Redundacja danych - dodatkowe sprawdzenie poprawnosci odebranych danych

//------------------------------------------------------------------------------------------------------------------
//                               *****WARTOSCI SPECYFICZNE DLA STANDARDU NEC*****
//                                            !!!!!NIE ZMIENIAC!!!!!
//------------------------------------------------------------------------------------------------------------------
#define NEC_HEADER 	  ms_pulse(13.5)	//Naglowek (AGC)
#define NEC_HIGH 	  ms_pulse(2.25)	//Bit 1
#define NEC_LOW 	  ms_pulse(1.12)	//Bit 0
#define NEC_TOLERANCE ms_pulse(0.4)		//Odchylki +/-
#define NEC_REHEADER  ms_pulse(11.0)	//Naglowek retransmisji (AGC')

//------------------------------------------------------------------------------------------------------------------
//                                     *****ZABEZPIECZENIA BIBLIOTEKI*****
//                                            !!!!!NIE ZMIENIAC!!!!!
//------------------------------------------------------------------------------------------------------------------

//Zabezpiecznie przed jednoczesnym uzywaniem rozszerzonego protokolu oraz redundancji
#if (USE_NEC_EXTENDED_PROTOCOL && USE_NEC_REDUNDANCY_CHECK)
#error U¿yto jednoczeœnie rozszerzonego protoko³u NEC oraz redundancji danych (pl)
#endif

//------------------------------------------------------------------------------------------------------------------
//                           *****DEKLARACJE, PLIKI NAGLWOKOWE, ZMIENNE GLOBALNE*****
//                                            !!!!!NIE ZMIENIAC!!!!!
//------------------------------------------------------------------------------------------------------------------
#define FRAME_HEADER  0b00000001	//Flaga bitowa inf. o wystapieniu naglowka
#define FRAME_FULL	  0b00000010	//Flaga bitowa inf. o odebraniu pelnej ramki
#define FRAME_RESTART 0b00000000	//Flaga bitowa - pusta

//--------------------------------Rejestrowanie CALLBACKu dla wersji PODSTAWOWEJ------------------------------------
#if USE_NEC_FRAME_COUNTER == 0 && USE_NEC_EXTENDED_PROTOCOL == 0
void IR_Callback_Register(void (*Callback)(uint8_t NEC_address0, uint8_t NEC_command0));
#endif
//------------------------------------------------------------------------------------------------------------------

//------------------------Rejestrowanie CALLBACKu dla wersji z ROZSZERZONYM POLEM DANYCH----------------------------
#if USE_NEC_FRAME_COUNTER == 0 && USE_NEC_EXTENDED_PROTOCOL == 1
void IR_Callback_Register(void (*Callback)(uint8_t NEC_address0, uint8_t NEC_command0, uint8_t address1, uint8_t command1));
#endif
//------------------------------------------------------------------------------------------------------------------

//-----------------------Rejestrowanie CALLBACKu dla wersji z LICZNIKIEM odebranych RAMEK---------------------------
#if USE_NEC_FRAME_COUNTER == 1 && USE_NEC_EXTENDED_PROTOCOL == 0
void IR_Callback_Register(void (*Callback)(uint8_t NEC_address0, uint8_t NEC_command0, uint8_t NEC_counter));
#endif
//------------------------------------------------------------------------------------------------------------------

//---------Rejestrowanie CALLBACKu dla wersji z ROZSZERZONYM POLEM DANYCH i LICZNIKIEM odebranych RAMEK-------------
#if USE_NEC_FRAME_COUNTER == 1 && USE_NEC_EXTENDED_PROTOCOL == 1
void IR_Callback_Register(void (*Callback)(uint8_t NEC_address0, uint8_t NEC_command0,
										   uint8_t address1, uint8_t command1, uint8_t NEC_counter));
#endif
//------------------------------------------------------------------------------------------------------------------

#endif
#endif /* IR_NEC_H_ */
