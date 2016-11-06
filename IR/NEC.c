//
// NEC.c
//
//  Created on: 30.08.2016
//      Author: Jatsekku

#include "IR_config.h"			//Plik nag��wkowy kofiguracyjny obs�ug� podczerwieni

//--------------------------------------------Wykorzystanie standardu NEC-------------------------------------------
#if IR_STD == NEC
#include <avr/io.h>				//Biblioteka podstawowa AVR
#include <avr/interrupt.h>		//Biblioteka przerwa�

#include "NEC.h"				//Plik nag��wkowy dla standardu NEC

volatile uint8_t NEC_ready;		//Flaga o zdekodowaniu poprawnej ramki
volatile uint8_t NEC_address0;	//Adres pilota
volatile uint8_t NEC_command0;	//Komenda pilota

//-------------------------Zmienne wykorzystywane w wersji z ROZSZERZONYM POLEM DANYCH------------------------------
#if USE_NEC_EXTENDED_PROTOCOL == 1
volatile uint8_t NEC_address1;		//Dodatkowa zmienna "adresowa"
volatile uint8_t NEC_command1;		//Dodatkowa zmienna "komend"
#endif
//------------------------------------------------------------------------------------------------------------------

//------------------------Zmienne wykorzystywane w wersji z LICZNIKIEM odebranych RAMEK-----------------------------
#if USE_NEC_FRAME_COUNTER == 1
volatile uint8_t NEC_counter;		//Zmienna zawieraj�ca inofrmacje o ilo�ci powt�rzonych ramek
volatile uint8_t NEC_status;		//Flagi bitowe dot. stanu ramki
volatile uint8_t IR_timer;			//Timer do obs�ugi podczerwieni
#endif
//------------------------------------------------------------------------------------------------------------------



//--------------------------------Rejestrowanie CALLBACKu dla wersji PODSTAWOWEJ------------------------------------
#if USE_NEC_FRAME_COUNTER == 0 && USE_NEC_EXTENDED_PROTOCOL == 0
//Deklaracja funkcji zwrotnej (okre�lenie postaci Callbacku)
static void (*Ir_Event_Callback)(uint8_t NEC_address0, uint8_t NEC_command0);
//Funkcja rejestruj�ca funkcj� zwrotn� (Callback) do zdarzenia NEC_Event
void IR_Callback_Register(void (*Callback)(uint8_t NEC_address0, uint8_t NEC_command0))
{
	Ir_Event_Callback = Callback;
}
#endif
//------------------------------------------------------------------------------------------------------------------

//------------------------Rejestrowanie CALLBACKu dla wersji z ROZSZERZONYM POLEM DANYCH----------------------------
#if USE_NEC_FRAME_COUNTER == 0 && USE_NEC_EXTENDED_PROTOCOL == 1
//Deklaracja funkcji zwrotnej (okre�lenie postaci Callbacku)
static void (*Ir_Event_Callback)(uint8_t NEC_address0, uint8_t NEC_command0,
								 uint8_t NEC_address1, uint8_t NEC_command1);
//Funkcja rejestruj�ca funkcj� zwrotn� (Callback) do zdarzenia NEC_Event
void IR_Callback_Register(void (*Callback)(uint8_t NEC_address0, uint8_t NEC_command0,
												 uint8_t NEC_address1, uint8_t NEC_command1))
{
	Ir_Event_Callback = Callback;
}
#endif
//------------------------------------------------------------------------------------------------------------------

//-----------------------Rejestrowanie CALLBACKu dla wersji z LICZNIKIEM odebranych RAMEK---------------------------
#if USE_NEC_FRAME_COUNTER == 1 && USE_NEC_EXTENDED_PROTOCOL == 0
//Deklaracja funkcji zwrotnej (okre�lenie postaci Callbacku)
static void (*Ir_Event_Callback)(uint8_t NEC_address0, uint8_t NEC_command0, uint8_t NEC_counter);
//Funkcja rejestruj�ca funkcj� zwrotn� (Callback) do zdarzenia NEC_Event
void IR_Callback_Register(void (*Callback)(uint8_t NEC_address0, uint8_t NEC_command0, uint8_t NEC_counter))
{
	Ir_Event_Callback = Callback;
}
#endif
//------------------------------------------------------------------------------------------------------------------

//---------Rejestrowanie CALLBACKu dla wersji z ROZSZERZONYM POLEM DANYCH i LICZNIKIEM odebranych RAMEK-------------
#if USE_NEC_FRAME_COUNTER == 1 && USE_NEC_EXTENDED_PROTOCOL == 1
//Deklaracja funkcji zwrotnej (okre�lenie postaci Callbacku)
static void (*Ir_Event_Callback)(uint8_t NEC_address0, uint8_t NEC_command0,
								 uint8_t NEC_address1, uint8_t NEC_command1, uint8_t NEC_counter);
//Funkcja rejestruj�ca funkcj� zwrotn� (Callback) do zdarzenia NEC_Event
void IR_Callback_Register(void (*Callback)(uint8_t NEC_address0, uint8_t NEC_command0,
										   uint8_t NEC_address1, uint8_t NEC_command1, uint8_t NEC_counter))
{
	Ir_Event_Callback = Callback;
}
#endif
//------------------------------------------------------------------------------------------------------------------



//....................Funkcja NEC_Event (Zdarzenie - programowa implementacja "przerwania").........................
void IR_Event(void)
{
	if(NEC_ready)		//Je�eli odebrano poprawn� ramk� danych
	{
		//Wywo�anie funkcji u�ytkownika je�li takowa zosta�a utworzona i zarejestrowana
		//Procedura wywo�uj�ca wybierana jest automatycznie na podstawie ustawie� biblioteki

//----------------------------------Wywo�anie CALLBACKu dla wersji PODSTAWOWEJ--------------------------------------
#if USE_NEC_FRAME_COUNTER == 0 && USE_NEC_EXTENDED_PROTOCOL == 0
		if(Ir_Event_Callback)	(*Ir_Event_Callback)(NEC_address0, NEC_command0);

		NEC_address0 = 0;
		NEC_command0 = 0;
#endif
//------------------------------------------------------------------------------------------------------------------

//--------------------------Wywo�anie CALLBACKu dla wersji z ROZSZERZONYM POLEM DANYCH------------------------------
#if USE_NEC_FRAME_COUNTER == 0 && USE_NEC_EXTENDED_PROTOCOL == 1
		if(Ir_Event_Callback)	(*Ir_Event_Callback)(NEC_address0, NEC_command0, NEC_address1, NEC_command1);

		NEC_address0 = 0;
		NEC_command0 = 0;
		NEC_address1 = 0;
		NEC_command1 = 0;
#endif
//------------------------------------------------------------------------------------------------------------------

//-------------------------Wywo�anie CALLBACKu dla wersji z LICZNIKIEM odebranych RAMEK-----------------------------
#if USE_NEC_FRAME_COUNTER == 1 && USE_NEC_EXTENDED_PROTOCOL == 0
		if(Ir_Event_Callback)	(*Ir_Event_Callback)(NEC_address0, NEC_command0, NEC_counter);

		if(NEC_status == FRAME_RESTART)	//Je�li nie ma znacznika nag��wka ani pe�nej ramki
		{
			NEC_address0 = 0;
			NEC_command0 = 0;
		}
#endif
//------------------------------------------------------------------------------------------------------------------

//-----------Wywo�anie CALLBACKu dla wersji z ROZSZERZONYM POLEM DANYCH i LICZNIKIEM odebranych RAMEK---------------
#if USE_NEC_FRAME_COUNTER == 1 && USE_NEC_EXTENDED_PROTOCOL == 1
		if(Ir_Event_Callback)	(*Ir_Event_Callback)(NEC_address0, NEC_command0, NEC_address1,
													 NEC_command1, NEC_counter);
		if(NEC_status == FRAME_RESTART)	//Je�li nie ma znacznika nag��wka ani pe�nej ramki
		{
			NEC_address0 = 0;
			NEC_command0 = 0;
			NEC_address1 = 0;
			NEC_command1 = 0;
		}
#endif
//------------------------------------------------------------------------------------------------------------------
		NEC_ready = 0;	//Usuni�cie zezwolenia na wykonanie zdarzenia
	}
}
//..................................................................................................................



//-------------------------------------------INICJALIZACJA SPRZ�TOWA------------------------------------------------
//Inicjalizacja hardware'u do obs�ugi podczerwieni (Timer1 oraz jeden PinX[ICP])
//Biblioteka dzia�a na mikrokontrolerach posiadaj�cych 16bitowy timer, wyposa�ony w tryb Capture.
//------------------------------------------------------------------------------------------------------------------

//........................Funkcja IR_Init - odpowiedzialana za inicjalizacj� hardware'u.............................
void IR_Init(void)
{
	DDR(PORT_IR_IN) &= ~(PIN_IR_IN);	//Ustawienie pinu powi�zanego z odbiornikiem IR jako wej�cie
	PORT(PORT_IR_IN) |= PIN_IR_IN;		//Podci�gni�cie pinu powi�zanego z odbiornikiem IR do VCC (wewn�trzny pull-up)
    TCCR1B |= (1<<CS11);   			    //Preskaler Timera1 ustawiony na 8
    TCCR1B &= ~(1<<ICES1);  			//Reakcja na zbocze opadaj�ce
    TIMSK |= (1<<TICIE1);   			//Odblokowanie przerwania ICP (reakcja na sygana�y zewn�trzne)

//-----------------------DODATKOWA INICJALIZACJA dla wersji z LICZNIKIEM odebranych RAMEK---------------------------
#if USE_NEC_FRAME_COUNTER == 1
    TIMSK |= (1<<TOIE1);				//Odblokowanie przerwania Overflow (przepe�nienie)
#endif
//------------------------------------------------------------------------------------------------------------------
}
//..................................................................................................................



//----------------------------------------------------PRZERWANIA----------------------------------------------------

//................................PRZERWANIE INPUT CAPTURE (reakcja na zbocze)......................................
ISR(TIMER1_CAPT_vect)
{
	/*Zmienna przechowuj�ca informacje o aktualnie obrabianej ramce danych.
	  Swoisty bufor zwi�znay z poni�szym indexem */
	static uint32_t NEC_frame;
	/*Zmienna zawieraj�ca informacje o aktualnie obrabianym BICIE danych
	  Ramka sk�ada si� z 16 bit�w dot. adresu oraz 16 bit�w dot. komendy. W sumie 32 bity */
	static uint8_t NEC_frame_idx = 33;
	//Zmienna informuj�ca czy aktualnie obrabiany bit to 0 czy 1
	uint8_t NEC_bit = 2;
	//Zmienna zawiraj�ca inf. na temat szeroko�ci odebranego impulsu
	uint16_t NEC_pulse_width;
	NEC_pulse_width = ICR1;

//-------------------------DODATKOWE PROCEDURY dla wersji z LICZNIKIEM odebranych RAMEK-----------------------------
#if USE_NEC_FRAME_COUNTER == 1
	//Reakcja na header lub re-header (nag��wek) (z dopuszczaln� tolerancj� b��du +/-)
	if(((NEC_pulse_width > NEC_REHEADER - NEC_TOLERANCE) && (NEC_pulse_width < NEC_REHEADER + NEC_TOLERANCE))
		|| ((NEC_pulse_width > NEC_HEADER - NEC_TOLERANCE) && (NEC_pulse_width < NEC_HEADER + NEC_TOLERANCE)))
	{
		NEC_counter++;				//Inkrementacja licznika odebranych ramek
		PORTA ^= (1<<PA1);
		NEC_status |= FRAME_HEADER;	//Ustawienie flagi o wykryciu nag��wka
	}
	IR_timer = 0;					//Wyzerowanie timera programowego odpowiedzialnego za timeout
#endif
//------------------------------------------------------------------------------------------------------------------

	//Je�eli odczytywana jest nowa ramka oraz nast�pi� nag��wek startu (z dopuszczaln� tolerancj� b��du +/-)
	if((NEC_frame_idx == 33) && (NEC_pulse_width > NEC_HEADER - NEC_TOLERANCE)
		&& (NEC_pulse_width < NEC_HEADER + NEC_TOLERANCE))
	{
		NEC_frame_idx --; 	//Zmniejszenie indeksu o jeden i tym samym zezwalenie na anliz� ramki danych
	}


	if(NEC_frame_idx < 32)	//Sprawdzanie czy mo�liwa jest analiza ramki danych
	{
		//Rozr�nienie czy wyst�pi�a 1 czy 0 w kolejno nadlatuj�cych bitach ramki
		if((NEC_pulse_width > NEC_HIGH - NEC_TOLERANCE)
			&& (NEC_pulse_width < NEC_HIGH + NEC_TOLERANCE) ) NEC_bit = 1;
		if((NEC_pulse_width > NEC_LOW - NEC_TOLERANCE)
			&& (NEC_pulse_width < NEC_LOW + NEC_TOLERANCE) ) NEC_bit = 0;

		if(NEC_bit !=2)		//W wypadku gdy NEC_bit jest 0 lub 1
		{
			/*Sk�adanie ramki w ca�o�c przy u�yciu przesuni�cia w lewo o warto�c
			  aktualnego indeksu oraz z uwzgl�dnieniem rzutowania jawnego*/
			NEC_frame |= (uint32_t)NEC_bit << NEC_frame_idx;
			if(NEC_frame_idx == 0)	//Gdy zako�czono analiz� ramki
			{
				//Podzia� adresu i komendy do zmiennych
				NEC_address0 = (uint8_t)(NEC_frame >> 24);
				NEC_command0 = (uint8_t)(NEC_frame >> 8);

//-------------------DODATKOWY PODZIA� DANYCH dla wersji z ROZSZERZONYM POLEM DANYCH--------------------------------
#if USE_NEC_EXTENDED_PROTOCOL == 1
				NEC_address1 = (uint8_t)(NEC_frame >> 16);
				NEC_command1 = (uint8_t)(NEC_frame);
#endif
//------------------------------------------------------------------------------------------------------------------

//---------------------------------DODATKOWE PROCEDURY dla wersji PODSTAWOWEJ---------------------------------------
#if USE_NEC_FRAME_COUNTER == 0 && USE_NEC_REDUNDANCY_CHECK == 0
				NEC_ready = 1;		//Zg�oszenie zdekodowania pe�nej ramki
#endif
//------------------------------------------------------------------------------------------------------------------

//-----------------------------DODATKOWE PROCEDURY dla wersji z REDUNDACJ� DANYCH-----------------------------------
#if USE_NEC_FRAME_COUNTER == 0 && USE_NEC_REDUNDANCY_CHECK == 1
				if(!(0x00FF00FF & NEC_frame & (NEC_frame>>8))) NEC_ready = 1;
#endif
//------------------------------------------------------------------------------------------------------------------

//-------------------------DODATKOWE PROCEDURY dla wersji z LICZNIKIEM odebranych RAMEK-----------------------------
#if USE_NEC_FRAME_COUNTER == 1 && USE_NEC_REDUNDANCY_CHECK == 0
				NEC_status |= FRAME_FULL;	//Ustawienie bitu(flagi) inf. o odebraniu pe�nej ramki
#endif
//------------------------------------------------------------------------------------------------------------------

//---------------DODATKOWE PROCEDURY dla wersji z LICZNIKIEM odebranych RAMEK i REDUNDACJ� DANYCH-------------------
#if USE_NEC_FRAME_COUNTER == 1 && USE_NEC_REDUNDANCY_CHECK == 1
				if(!(0x00FF00FF & NEC_frame & (NEC_frame>>8))) NEC_status |= FRAME_FULL;
#endif
//------------------------------------------------------------------------------------------------------------------

				NEC_frame = 0;					//Wykasowanie zawarto�ci zmiennej dot. obrabianej ramki
				NEC_frame_idx = 33;				//Nadanie indeksu pocz�tkowego ramce
			}
			else NEC_frame_idx --;				//Dekrementacja indeksu ramki
		}
		else NEC_frame_idx = 33;				//Nadanie indeksu pocz�tkowego ramce
	}
	if(NEC_frame_idx == 32) NEC_frame_idx --;	//Dekrementacja indeksu w wypadku gdy indeks jest r�wny 32
	TCNT1 = 0;									//Wyzerowanie rejestru timera sprz�towego
//-------------------------DODATKOWE PROCEDURY dla wersji z LICZNIKIEM odebranych RAMEK-----------------------------
#if USE_NEC_FRAME_COUNTER == 1
	//Ustawienie flagi gotowo�ci odbioru je�eli wyst�pi� nag��wek i przynajmniej jedna pe�na ramka
	if(NEC_status == (FRAME_FULL | FRAME_HEADER)) NEC_ready = 1;
#endif
//------------------------------------------------------------------------------------------------------------------
}
//..................................................................................................................

//.........................PRZERWANIE OVERFLOW dla wersji z LICZNIKIEM odebranych RAMEK.............................
#if USE_NEC_FRAME_COUNTER == 1
ISR(TIMER1_OVF_vect)
{
	static uint8_t NEC_lastcounter;	//Zmienna statyczna dot. ost warto�ci licznika ramek
	IR_timer++;						//Timer programowy dla potrzeb licznika ramek
	if(IR_timer%2)					//Je�li up�yn�� timeout (ok.130ms)
	{
		/*W przypadku gdy w czasie mniejszym ni� timeout nie odebrano �adnej ramki
		  nast�puje zerowanie licznika ramek oraz flag dot. stanu ramki.*/
		if(NEC_counter == NEC_lastcounter)
		{
				NEC_counter = 0;
				NEC_status = FRAME_RESTART;
		}
		NEC_lastcounter = NEC_counter; //Aktualizacja ost. warto�ci licznika ramek
	}
}
#endif
//..................................................................................................................
#endif
//------------------------------------------------------------------------------------------------------------------


