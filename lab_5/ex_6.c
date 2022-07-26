/**
    @brief Faça um programa que transmita, bit a bit, o dado AAH pelo pino 0 da porta P1. O dado deve ser transmitido do bit menos significativo para omais significativo. O nível lógico de cada bit deve ser mantido no pino pelo intervalo de 60000 ciclos de instrução (clock do cristal/12). Utilize o timer1 no modo 1 para estabelecer tal período. Ao término do procedimento, volte a repetí-lo.

    Resolucao: Como estamos no modo 1 ambos TH1 e TL1 possuirão 8 bits, dessa forma o maior valor armazenado por ambos será 2^16 = 65536. Como queremos que o overflow em TF1 ocorra a cada 60000 ciclos de instrução devemos setar (TH1 << 4) | TL1 = 65536 - 60000 = 5536 = 0x15A0 .: TH1 = 0x15 e TL1 = 0xA0.

    TIMER 1 + MODO 1 + 60000 ciclos de inst.
             (16 bits) 
*/

#include <reg51.h>

#define TMR_CNTR_MODE_0       0x00 // 0000 0000
#define TMR_CNTR_MODE_1       0x01 // 0000 0001
#define TMR_CNTR_MODE_2       0x02 // 0000 0010
#define TMR_CNTR_MODE_3       0x03 // 0000 0011
#define TMR_CNTR_SET_TO_TMR   0x00 // 0000 0000
#define TMR_CNTR_SET_TO_CNTR  0x04 // 0000 0100
#define TMR_CNTR_GATE_LOW     0x00 // 0000 0000
#define TMR_CNTR_GATE_HIGH    0x08 // 0000 1000

#define TH0_VALUE 0x00 // 0000 0000
#define TL0_VALUE 0x00 // 0000 0000

#define TH1_VALUE 0x15 // 0001 0101
#define TL1_VALUE 0xA0 // 1010 0000

#define DATA_TO_SEND 0xAA // 1010 1010

struct TMOD_init_struct 
{ 
    unsigned char mode; 
    unsigned char cntr_tmr; 
    unsigned char gate; 
} timer_0, timer_1;

void timer_init(void) 
{
    unsigned char TMOD_aux;

    timer_1.mode = TMR_CNTR_MODE_1;
    timer_1.cntr_tmr = TMR_CNTR_SET_TO_TMR;
    timer_1.gate = TMR_CNTR_GATE_LOW;

    TMOD_aux = (timer_1.mode | timer_1.cntr_tmr | timer_1.gate) << 4;

    timer_0.mode = 0x00; 
    timer_0.cntr_tmr = 0x00;  
    timer_0.gate = 0x00; 

    TMOD = TMOD_aux | (timer_0.mode | timer_0.cntr_tmr | timer_0.gate);
 }

void c51_tmr1(void);

sbit DATA_PORT = P1^0; // the data will be sent via P1^0 port 

unsigned char state = 0;
unsigned char data_aux = DATA_TO_SEND;

bit data_bit_aux;

unsigned int counter_aux = 0;

void main(void) 
{
    timer_init();
    TH1 = TH1_VALUE; 
    TL1 = TL1_VALUE; 

    DATA_PORT = 0;

    ET1 = 1;
    EA = 1; 
    TR1 = 1;

    while(1) 
    {
        while (state == 0); state = 0;
        
        data_aux = DATA_TO_SEND << (7 - counter_aux); // shifts current analyzed bit to MSB position 
        data_aux = data_aux & 0x80; // isolates MSB (current analyzed bit)
        if(data_aux == 0x80) data_bit_aux = 1; // checks if current analyzed bit is equal to 1 or 0
        if(data_aux == 0) data_bit_aux = 0;
        DATA_PORT = data_bit_aux; // transmits the bit value checked through P1^0
        counter_aux++;

        // restarts the process after all bits from DATA_TO_SEND are analyzed
        if(counter_aux == 7) counter_aux = 0;
    } 
} 

void c51_tmr1(void) interrupt 3 
{
    TH1 = TH1_VALUE; 
    TL1 = TL1_VALUE; 
    state++;
} 
