/**
    @brief Enviar caracteres para a porta P1 em intervalos de 100 
    ciclos de instrução (isto é, clock do cristal/12),
    utilizando para TMOD_auxorização o timer 0 no modo 1.

    TIMER 0 + MODO 1 + 100 pulsos de clock
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

#define TH0_VALUE 0xFF // 1111 1111
#define TL0_VALUE 0x9C // 1001 1100

#define TH1_VALUE 0x00 // 0000 0000
#define TL1_VALUE 0x00 // 0000 0000

#define MESSAGE_TO_SEND "Microcontrolador"

struct TMOD_init_struct 
{ 
    unsigned char mode; 
    unsigned char cntr_tmr; 
    unsigned char gate; 
} timer_0, timer_1;

void timer_init(void) 
{
    unsigned char TMOD_aux;

    timer_1.mode = 0x00; // set timer 1 mode
    timer_1.cntr_tmr = 0x00; // set timer 1 to counter or timer
    timer_1.gate = 0x00; // set gate = '1' or gate = '0'

    TMOD_aux = (timer_1.mode | timer_1.cntr_tmr | timer_1.gate) << 4;
    // shifts 4 LSB to 4 MSB 

    timer_0.mode = TMR_CNTR_MODE_1; // set timer 0
    timer_0.cntr_tmr = TMR_CNTR_SET_TO_TMR; // set timer 0 to counter or timer 
    timer_0.gate = TMR_CNTR_GATE_LOW; // set gate = '1' or gate ='0'

    TMOD = TMOD_aux | (timer_0.mode | timer_0.cntr_tmr | timer_0.gate);
    // no need to shift since timer 0 corresponds to the 4 LSB
 }

void c51_tmr0 (void);

unsigned char state = 0;

void main (void) 
{
    unsigned char code message[]= MESSAGE_TO_SEND;
    unsigned char code *message_ptr;
    unsigned char aux = 0;

    message_ptr = message;

    timer_init();
    TH0 = TH0_VALUE; 
    TL0 = TL0_VALUE; /* .: TH0 << TL0 => 65436 so after 
                     100 clock pulses TFX is set to 1*/

    ET0 = 1;
    EA = 1; 
    TR0 = 1;

    while (1) {
        while (state == 0);
        state = 0;
        P1 = *(message_ptr+aux++);
        if (aux == 16) aux = 0;
    } 
} 

/* 
   remember interrupt 1 => timer 0
            interrupt 3 => timer 1
*/
void c51_tmr0 (void) interrupt 1 
{
    TH0 = TH0_VALUE;
    TL0 = TL0_VALUE;
    state++;
} 
