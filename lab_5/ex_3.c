/**
    @brief Refaça o programa anterior, utilizando o timer0 no modo 2, 
    para que cada caractere seja enviado para a porta P1 em intervalos
    de 640 ciclos de instrução (clock do cristal/12).

    Resolucao: No modo 2 são utilizados apenas 8 bits, apenas os bits 
    de TL0 irão variar, o valor settado em TH0 servirá como referencia 
    de recarga para TL0 quando houve overflow neste byte. No modo 2 o 
    máximo valor que podemos contar com a utilização de TL0 é 2^8 
    ou seja 256. Como queremos fazer a contagem de 640 pulsos a estratégia
    escolhida foi separar esses 640 pulsos em contagens intermediárias de 
    160 pulsos. Para que sejam contados 160 ciclos de inst. em cada ciclo o 
    valor de recarga em TH0 deverá ser de: 256 - 160 = 96 = 0x60;

    TIMER 0 + MODO 2 + 640 ciclos de inst.
             (8 bits) -> auto-recarga
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

#define TH0_VALUE 0x60 // 0110 0000
#define TL0_VALUE 0x60 // 0110 0000

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

    timer_1.mode = 0x00;
    timer_1.cntr_tmr = 0x00;
    timer_1.gate = 0x00;

    TMOD_aux = (timer_1.mode | timer_1.cntr_tmr | timer_1.gate) << 4;

    timer_0.mode = TMR_CNTR_MODE_2; 
    timer_0.cntr_tmr = TMR_CNTR_SET_TO_TMR;  
    timer_0.gate = TMR_CNTR_GATE_LOW; 

    TMOD = TMOD_aux | (timer_0.mode | timer_0.cntr_tmr | timer_0.gate);
 }

void c51_tmr0(void);

unsigned char cycle_aux = 0;

void main(void) 
{
    unsigned char code message[]= MESSAGE_TO_SEND;
    unsigned char code *message_ptr;
    unsigned char aux = 0;

    message_ptr = message;

    timer_init();
    TH0 = TH0_VALUE; 
    TL0 = TL0_VALUE; 

    ET0 = 1;
    EA = 1; 
    TR0 = 1;

    while(1) 
    {
        while (cycle_aux%4 != 0); 
        cycle_aux = 0;
        P1 = *(message_ptr+aux++);
        if (aux == 16) aux = 0;
    } 
} 

void c51_tmr0(void) interrupt 1 
{
    cycle_aux++;
} 
