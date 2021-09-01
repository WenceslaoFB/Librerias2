/*
 * i2c.c
 *
 * Created: 30/8/2021 13:46:23
 *  Author: Wenceslao
 */ 

#define DISPLAY_OFF 0xAE
#define	SETDISPLAYCLOCKDIV 0xD5
#define SETMULTIPLEX 0xA8
#define SETDISPLAYOFFSET 0xD3
#define SETSTARTLINE 0x40
#define CHARGEPUMP 0x8D
#define MEMORYMODE 0x20
#define SEGREMAP 0xA0
#define COMSCANDEC 0xC8
#define SETCOMPINS 0xDA
#define SETCONTRAST 0x81
#define SETVCOMDETECT 0xDB
#define DISPLAYALLON_RESUME 0xA4
#define NORMALDISPLAY 0xA6
#define REVERSEDISPLAY 0xA7
#define DISPLAYON 0xAF
#define SETPRECHARGE 0xD9

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <i2c.h>


//Prototipos
void InitTimers();
void InitCom();
void InitDisplay();
void DecodificarHeader();
void CMDVerif();
void SendTx();

//Prototipos TWI
void Start_TWI();
void Send_Address();
void Send_Data(uint8_t);
void Stop_TWI();
void Send_Command(uint8_t,uint8_t);

typedef union{
	struct{
		unsigned char validHeader: 1;
		unsigned char validCmd: 1;
		unsigned char confirmBoton : 1;
		unsigned char botonpresionado : 1;
		unsigned char esperandoTwi : 1;
	}bit;
}_sFlag;

volatile _sFlag flags1;


//variables globales
volatile uint8_t timeoutBlink;
volatile uint8_t maxtimeBlink;
uint8_t timeBoton;
uint8_t buff[256];
uint8_t indL;
uint8_t indE;
uint8_t cksTx;
uint8_t cks;
uint8_t command;
uint8_t indcmd;
uint8_t status_TWI;
uint8_t estado_com_display;
uint16_t nbytes;
short indexHeader;



//interrupcion cada 10ms
ISR(TIMER1_COMPA_vect){
	if(timeoutBlink)
	timeoutBlink--;
	if(timeBoton & flags1.bit.confirmBoton)
	timeBoton--;
}

//Recepción UART
ISR(USART0_RX_vect){
	buff[indE++] = UDR0;
}


int main(void)
{
	cli();
	InitTimers();
	InitCom();
	InitDisplay();

	estado_com_display = 0;
	timeoutBlink = 25;
	maxtimeBlink = timeoutBlink;
	timeBoton = 10;
	flags1.bit.botonpresionado = 0;
	flags1.bit.confirmBoton = 0;
	
	//Configuro el pin de led del arduino como salida (PB5)
	DDRB = (1 << DDB5); //se configura el pin como salida, al hacerlo con igual solo el bit 5 de ese byte queda en 1
	PORTB |= (1 << PB7); //se pone el pin en alto, al ser or = el resto de bits queda iguales
	
	//Configuro digital pin 2 con el pull up interno para leer un boton, registro PIND, bit PIND2 para leer el estado
	DDRD &=~ (1<<DDD2);
	PORTD |= (1<<PB7);
	
	sei(); //habilita todas las interrupciones
	PORTB &= ~(1<<PB7);
	
	
	while (1)
	{
		//if(!(PIND & (1<<PIND2)) & !flags1.bit.botonpresionado){
		//flags1.bit.confirmBoton = 1;
		//if(!timeBoton){
		//flags1.bit.botonpresionado = 1;
		//flags1.bit.confirmBoton = 0;
		//PORTB |= (1<<PORTB5);
		//}
		//}
		
		//if(flags1.bit.botonpresionado){
		if(!(PIND & (1<<PIND2))){
			
			
			Send_Command(0x00,0xB3);
			Send_Command(0x00,0x0F);
			Send_Command(0x00,0x10);
			//Send_Command(0x00,0xE0);
			
			Send_Command(0x40,0xFF);
			Send_Command(0x40,0x88);
			Send_Command(0x40,0x88);
			Send_Command(0x40,0x80);
			Send_Command(0x40,0x80);
			Send_Command(0x40,0x00);
			Send_Command(0x40,0x00);
			Send_Command(0x40,0x00);
			
			Send_Command(0x40,0x7f);
			Send_Command(0x40,0x81);
			Send_Command(0x40,0x81);
			Send_Command(0x40,0x81);
			Send_Command(0x40,0x81);
			Send_Command(0x40,0x00);
			Send_Command(0x40,0x00);
			Send_Command(0x40,0x00);
			
			Send_Command(0x40,0x7f);
			Send_Command(0x40,0x88);
			Send_Command(0x40,0x88);
			Send_Command(0x40,0x88);
			Send_Command(0x40,0x7F);
			Send_Command(0x40,0x00);
			Send_Command(0x40,0x00);
			Send_Command(0x40,0x00);

			Send_Command(0x40,0xFf);
			Send_Command(0x40,0x01);
			Send_Command(0x40,0x01);
			Send_Command(0x40,0x01);
			Send_Command(0x40,0x01);
			Send_Command(0x40,0x00);
			Send_Command(0x40,0x00);
			Send_Command(0x40,0x00);

			
			PORTB |= (1<<PB7);
			//Send_Command(0x00,0xEE);
			
			//Send_Command(0x40,0x05);
			//Send_Command(0x40,0x05);
			//Send_Command(0x40,0x05);
			//Send_Command(0x40,0x05);
		}
		//if(!(PIND & (1<<PIND2))){
		//switch(estado_com_display){
		//case 0:
		//Start_TWI();
		//break;
		//case 1:
		//Send_Address();
		//break;
		//case 2:
		////Send_Data();
		//Send_Data(0x00);
		//Send_Data(DISPLAY_OFF);
		//
		//Send_Data(0x00);
		//Send_Data(SETDISPLAYCLOCKDIV);
		//Send_Data(0x80);
		//
		//Send_Data(0x00);
		//Send_Data(SETMULTIPLEX);
		//Send_Data(0x0F);
		//
		//Send_Data(0x00);
		//Send_Data(SETDISPLAYOFFSET);
		//Send_Data(0x00);
		//
		//Send_Data(0x00);
		//Send_Data(SETSTARTLINE | 0x0);
		//
		//Send_Data(0x0);
		//Send_Data(CHARGEPUMP);
		//Send_Data(0x10);
		//
		//Send_Data(0x00);
		//Send_Data(MEMORYMODE);
		//Send_Data(0x00);
		//
		//Send_Data(0x00);
		//Send_Data(SEGREMAP | 0x1);
		//
		//Send_Data(0x40);
		//Send_Data(COMSCANDEC);
		//
		//Send_Data(0x00);
		//Send_Data(SETCOMPINS);
		//Send_Data(0x2);
		//
		//Send_Data(0x00);
		//Send_Data(SETCONTRAST);
		//Send_Data(0x10);
		//
		//Send_Data(0x00);
		//Send_Data(SETPRECHARGE);
		//Send_Data(0x22);
		//
		//Send_Data(0x00);
		//Send_Data(SETVCOMDETECT);
		//Send_Data(0x40);
		//
		//Send_Data(0x00);
		//Send_Data(DISPLAYALLON_RESUME);
		//
		//Send_Data(0x00);
		//Send_Data(NORMALDISPLAY);
		//
		//Send_Data(0x00);
		//Send_Data(DISPLAYON);
		//
		//Send_Data(0x00);
		//Send_Data(0xB7);
		//
		//Send_Data(0x40);
		//Send_Data(0x05);
		//
		//break;
		//case 3:
		//Stop_TWI();
		//break;
		//}
		//}
		
		if(indL!=indE){
			DecodificarHeader();
			if (flags1.bit.validHeader)
			CMDVerif();
			if(flags1.bit.validCmd)
			SendTx();
		}
		
		//if (!timeoutBlink)
		//{
		//if(PORTB & (1<< PORTB5))
		//PORTB &= ~(1<<PORTB5);
		////else if(PIND & (1<<PIND2))
		//else
		//PORTB |= (1<<PORTB5);
		//
		//timeoutBlink = maxtimeBlink;
		//}
		
	}
}

void Send_Command(uint8_t c,uint8_t cmd){
	Start_TWI();
	Send_Address();
	Send_Data(c);
	Send_Data(cmd);
	Stop_TWI();
}


void Start_TWI(){
	TWCR =	(1<<TWINT)|(1<<TWSTA)| (1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	status_TWI = TWSR & 0xF8;
	if(status_TWI == 0x08 || status_TWI == 0x10){
		estado_com_display = 1;
	}
	//if((TWCR & (1<<TWINT)) & flags1.bit.esperandoTwi){
	//status_TWI = TWSR & 0xF8;
	//if(status_TWI == 0x08 || status_TWI == 0x10){
	//estado_com_display = 1;
	//flags1.bit.esperandoTwi = 0;
	//}
	//}
	//else
	//flags1.bit.esperandoTwi = 1;
}
void Send_Address(){
	TWDR = (ADDRESS << 1);
	TWCR =(1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	status_TWI = TWSR & 0xF8;
	if(status_TWI==0x18 || status_TWI==0x20){
		estado_com_display = 2;
	}
	//TWDR = ADDRESS << 1;//address + w por el 0 en la última posición, con 1 sería read
	//TWCR |= (1<<TWINT);
	//if((TWCR & (1<<TWINT)) & flags1.bit.esperandoTwi){
	//status_TWI = TWSR & 0xF8;
	//if(status_TWI == 0x18 || status_TWI == 0x28){
	//estado_com_display = 2;
	//flags1.bit.esperandoTwi = 0;
	//}
	//}
	//else
	//flags1.bit.esperandoTwi = 1;
}
void Send_Data(uint8_t cmd){
	//TWDR = 0xAF;
	TWDR = cmd;
	TWCR =	(1<<TWINT)|(1<<TWEN);
	while (!(TWCR & (1<<TWINT)));
	status_TWI = TWSR & 0xF8;
	if(status_TWI == 28 || status_TWI == 30){
		estado_com_display = 2;
	}
	else{
		estado_com_display = 4;
	}
	//if((TWCR & (1<<TWINT)) & flags1.bit.esperandoTwi){
	//status_TWI = TWSR & 0xF8;
	//if(status_TWI == 0x28){
	//estado_com_display = 3;
	//flags1.bit.esperandoTwi = 0;
	//}
	//}
	//else
	//flags1.bit.esperandoTwi = 1;
}
void Stop_TWI(){
	TWCR =	(1<<TWINT)|(1<<TWEN)|(1<<TWSTO);
	while ((TWCR & (1<<TWSTO)));
	//TWCR |= (1<<TWINT) | (1<<TWSTO);
	//if((TWCR & (1<<TWINT)) & flags1.bit.esperandoTwi){
	//TWCR |= (1<<TWINT);
	//estado_com_display = 4;
	//flags1.bit.esperandoTwi = 0;
	//}
	//else
	//flags1.bit.esperandoTwi = 1;
}

void InitTimers(){
	TCCR1B = 0x08; // Modo ctc
	//OCR1A = 0x4E20; // Cargamos 20000 en el registro de comparación A para obtener un timer de 10ms con este preescalador
	TIFR1 = TIFR1; // Se limpian las banderas
	//TCCR1B = 0x02; //Selección del preescalador a 8
	OCR1AH = 0x02;
	OCR1AL = 0x71;
	TCCR1B = 0x0C;
	TIMSK1 = (1<<OCIE1A); //Habilitación de la interrupción en match con A
}

void InitCom(){
	//Configuración de la comunicación
	
	//Configuración del USART como UART
	UCSR0C &=~ (1<<UMSEL00);
	UCSR0C &=~ (1<<UMSEL01);
	
	//Paridad desactivada
	UCSR0C &=~ (1<<UPM00);
	UCSR0C &=~ (1<<UPM01);
	
	//Tamaño de la comunicación 8 bits
	UCSR0C = (1<<UCSZ00);
	UCSR0C |= (1<<UCSZ01);
	UCSR0B &=~ (1<<UCSZ02);
	
	//Configurando baudrate a 9600
	UCSR0A &=~ (1<<U2X0); //en 1 dobla la velocidad de transmision en 0 quedá igual
	UBRR0 = (F_CPU/16/BAUDRATE) - 1;
	
	//Configurando el pin de transmisión como salida y el recepción como entrada poniendo 1 en los siguientes bits
	UCSR0B |= (1<<RXEN0);
	UCSR0B |= (1<<TXEN0);
	
	//Activando interrupción de recepción
	UCSR0B |= (1<<RXCIE0);
}

void InitDisplay(){
	TWBR = 0x08; //bit rate register
	TWSR &=~ (1<<TWPS1) ;
	TWSR |= (1<<TWPS0) ;//bits del preescalador
	//Estos 3 datos son para el cálculo de la frec de transmisión, da 200KHz en esta config
	//TWCR |= (1<<TWEA);//habilita bit de acknowledge
	TWCR |= (1<<TWEN); //habilita el twi
	
	Send_Command(0x00,DISPLAY_OFF);
	Send_Command(0x00,SETDISPLAYCLOCKDIV);
	Send_Command(0x00,0x80);
	Send_Command(0x00,SETMULTIPLEX);
	Send_Command(0x00,0x3F);
	Send_Command(0x00,SETDISPLAYOFFSET);
	Send_Command(0x00,0x00);
	Send_Command(0x00,SETSTARTLINE|0x0);
	//Send_Command(0x00,CHARGEPUMP);
	//Send_Command(0x00,0x10);
	//Send_Command(0x00,MEMORYMODE);
	//Send_Command(0x00,0x00);
	Send_Command(0x00,SEGREMAP|0x1);
	Send_Command(0x00,COMSCANDEC);
	Send_Command(0x00,SETCOMPINS);
	Send_Command(0x00,0x12);
	Send_Command(0x00,SETCONTRAST);
	Send_Command(0x00,0x9F);
	Send_Command(0x00,SETPRECHARGE);
	Send_Command(0x00,0x22);
	Send_Command(0x00,SETVCOMDETECT);
	Send_Command(0x00,0x40);
	Send_Command(0x00,DISPLAYALLON_RESUME);
	Send_Command(0x00,NORMALDISPLAY);
	
	for(uint8_t j=0; j<8;j++){
		switch(j){
			case 0:
			Send_Command(0x00,0xB0);
			break;
			case 1:
			Send_Command(0x00,0xB1);
			break;
			case 2:
			Send_Command(0x00,0xB2);
			break;
			case 3:
			Send_Command(0x00,0xB3);
			break;
			case 4:
			Send_Command(0x00,0xB4);
			break;
			case 5:
			Send_Command(0x00,0xB5);
			break;
			case 6:
			Send_Command(0x00,0xB6);
			break;
			case 7:
			Send_Command(0x00,0xB7);
			break;
		}
		for(int i=0;i<128; i++){
			Send_Command(0x40,0x00);
		}
	}
	
	Send_Command(0x00,DISPLAYON);
	_delay_ms(100);
	Send_Command(0x00,0xA4);
	_delay_ms(1000);
	Send_Command(0x00,0xA5);
	_delay_ms(1000);
	Send_Command(0x00,0xA4);
	_delay_ms(1000);
	//Send_Command(0x00,0xB0);
	//Send_Command(0x00,0x00);
	//Send_Command(0x00,0x10);
	//
	
	Send_Command(0x00,0xB3);
	Send_Command(0x00,0x0F);
	Send_Command(0x00,0x10);
	//Send_Command(0x00,0xE0);
	
	Send_Command(0x40,0xFF);
	Send_Command(0x40,0x88);
	Send_Command(0x40,0x88);
	Send_Command(0x40,0x80);
	Send_Command(0x40,0x80);
	Send_Command(0x40,0x00);
	Send_Command(0x40,0x00);
	Send_Command(0x40,0x00);
	
	Send_Command(0x40,0x7f);
	Send_Command(0x40,0x81);
	Send_Command(0x40,0x81);
	Send_Command(0x40,0x81);
	Send_Command(0x40,0x81);
	Send_Command(0x40,0x00);
	Send_Command(0x40,0x00);
	Send_Command(0x40,0x00);
	
	Send_Command(0x40,0x7f);
	Send_Command(0x40,0x88);
	Send_Command(0x40,0x88);
	Send_Command(0x40,0x88);
	Send_Command(0x40,0x7F);
	Send_Command(0x40,0x00);
	Send_Command(0x40,0x00);
	Send_Command(0x40,0x00);

	Send_Command(0x40,0xFf);
	Send_Command(0x40,0x01);
	Send_Command(0x40,0x01);
	Send_Command(0x40,0x01);
	Send_Command(0x40,0x01);
	Send_Command(0x40,0x00);
	Send_Command(0x40,0x00);
	Send_Command(0x40,0x00);
}

void DecodificarHeader(){
	static int indexHeader=0;
	uint8_t index;

	index = indE;

	while(index != indL){
		switch(indexHeader){
			case 0:
			if(buff[indL] == 'U'){
				indexHeader = 1;
			}
			break;
			case 1:
			if(buff[indL] == 'N')
			indexHeader = 2;
			else{
				indexHeader = 0;
				indL--;
			}
			break;
			case 2:
			if(buff[indL] == 'E')
			indexHeader = 3;
			else{
				indexHeader = 0;
				indL--;
			}
			break;
			case 3:
			if(buff[indL] == 'R')
			indexHeader = 4;
			else{
				indexHeader = 0;
				indL--;
			}
			break;
			case 4:
			nbytes = buff[indL];
			cks = 'U' ^ 'N' ^ 'E' ^ 'R' ^ buff[indL];
			indexHeader = 5;
			break;
			case 5:
			nbytes += (256*buff[indL]);
			cks ^= buff[indL];
			indexHeader = 6;
			break;
			case 6:
			if(buff[indL] == ':'){
				indexHeader = 7;
				indcmd = indL+1;
				cks ^= buff[indL];
			}
			else{
				indexHeader = 0;
				indL--;
			}
			break;
			case 7:
			if(nbytes != 1)
			cks ^= buff[indL];
			nbytes--;
			if(nbytes == 0){
				if(cks == buff[indL]){
					flags1.bit.validHeader = 1;
				}
				indexHeader = 0;
			}
			break;
			default:
			indexHeader = 0;
		}
		indL++;
	}
}

void CMDVerif(){
	switch((uint8_t)buff[indcmd]){
		case 0xF0://alive
		command = 1;
		flags1.bit.validCmd = 1;
		break;
		case 0x2B://+ aumentar frec de blink
		if(maxtimeBlink>5)
		maxtimeBlink -=5;
		command = 2;
		flags1.bit.validCmd = 1;
		break;
		case 0x2D://- reducir frec de blink
		if(maxtimeBlink<100)
		maxtimeBlink +=5;
		command = 3;
		flags1.bit.validCmd = 1;
		break;
	}
	//if((uint8_t)buff[indcmd]==0xF0){ //alive
	//command = 1;
	//flags1.bit.validCmd = 1;
	//}
	//if((uint8_t)buff[indcmd]==0x2B){ //+ aumentar frec de blink
	//command = 2;
	//flags1.bit.validCmd = 1;
	//}
	//if((uint8_t)buff[indcmd]==0x2D){ //- reducir frec de blink
	//command = 3;
	//flags1.bit.validCmd = 1;
	//}
}

void SendTx(){
	uint8_t dato[20];
	uint8_t indice=0;

	dato[indice++]='U';
	dato[indice++]='N';
	dato[indice++]='E';
	dato[indice++]='R';
	dato[indice++]=0x00;
	dato[indice++]=0x00;
	dato[indice++]=':';
	
	switch(command){
		case 1:
		dato[indice++]=0xF0; //alive
		dato[4]=0x02;
		break;
		case 2:
		dato[indice++]=0x2B; //frec aumentada -> valor actualizado
		dato[indice++]=maxtimeBlink;
		dato[4]=0x03;
		break;
		case 3:
		dato[indice++]=0x2D; //frec reducida -> valor actualizado
		dato[indice++]=maxtimeBlink;
		dato[4]=0x03;
		break;
	}
	for(int a=0 ;a<indice;a++)
	cksTx^=dato[a];
	dato[indice++]=cksTx;
	
	for(int i=0;i<indice;){
		if(UCSR0A & (1<<UDRE0))
		UDR0 = dato[i++];
	}
	flags1.bit.validHeader = 0;
	flags1.bit.validCmd = 0;
	command=0;
	cksTx=0;
}