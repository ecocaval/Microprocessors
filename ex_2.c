/**
    @brief Refaça o programa anterior, utilizando o timer1 no modo 0, 
    para que cada caractere seja enviado em intervalos de 640 ciclos
    de instrução (clock do cristal/12).

    resolução: no modo 0 serão utilizados apenas 5 bits em TL1 e 8 bits
    em TH1, desta forma o valor máximo armazenado em TL1 será 2^5 = 32.
    Desta forma, são necessárias 20 contagens completas de TL1 para contabilizar 640 pulsos de instrução. TH1 deverá ser settado de forma a garantir overflow (TFx = 1) quando estes 20 ciclos tiverem sido completos. Desta forma TH1 = 0xFF - 20 = 256 - 20 = 236 = 0xEC ;

    TIMER 1 + MODO 0 + 640 pulsos de clock
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

#define TH1_VALUE 0xEC // 1110 1100
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

    timer_1.mode = TMR_CNTR_MODE_0;
    timer_1.cntr_tmr = TMR_CNTR_SET_TO_TMR;
    timer_1.gate = TMR_CNTR_GATE_LOW;

    TMOD_aux = (timer_1.mode | timer_1.cntr_tmr | timer_1.gate) << 4;

    timer_0.mode = 0x00; 
    timer_0.cntr_tmr = 0x00;  
    timer_0.gate = 0x00; 

    TMOD = TMOD_aux | (timer_0.mode | timer_0.cntr_tmr | timer_0.gate);
 }

void c51_tmr1 (void);

unsigned char state = 0;

void main (void) 
{
    unsigned char code message[]= MESSAGE_TO_SEND;
    unsigned char code *message_ptr;
    unsigned char aux = 0;

    message_ptr = message;

    timer_init();
    TH1 = TH1_VALUE; 
    TL1 = TL1_VALUE; 

    ET1 = 1;
    EA = 1; 
    TR1 = 1;

    while (1) {
        while (state == 0);
        state = 0;
        P1 = *(message_ptr+aux++);
        if (aux == 16) aux = 0;
    } 
} 

void c51_tmr1 (void) interrupt 3 
{
    TH1 = TH1_VALUE; 
    TL1 = TL1_VALUE; 
    state++;
} 
