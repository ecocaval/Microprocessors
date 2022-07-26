/**
    @brief Acrescente ao programa do exercício 3, a geração de uma onda
    quadrada no pino 3 da porta P2 com período de 960 ciclos de instrução
    (ou seja, alterar o nível lógico do pino 3 da porta P2) utilizando o Timer1 no modo 0.

    Resolucao: Como utilizaremos o modo 0, TL1 terá apenas 5 bits e TH1 será incrementado a cada 32 contagens de TL1, pois 2^5 = 32. Como queremos que haja overflow em 960 ciclos de instrução para termos 960/32 = 30 contagens "completas" de TL1. Desta forma TH1 deve ser recarregado com o valor: 2^8 - 30 = 256 - 30 = 226 = 0xE2;

    TIMER 1 + MODO 0 + 960 ciclos de inst.
             (13 bits)
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

#define TH1_VALUE 0xE2 // 1110 0010
#define TL1_VALUE 0x00 // 0000 0000

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

void c51_tmr1(void);

sbit WAVE_PORT = P2^3; // the wave will be transmitted via P2^3 port 

unsigned char state = 0;

void main(void) 
{
    timer_init();
    TH1 = TH1_VALUE; 
    TL1 = TL1_VALUE; 

    WAVE_PORT = 0;

    ET1 = 1;
    EA = 1; 
    TR1 = 1;

    while(1) 
    {
        while (state == 0); 
        state = 0;
        WAVE_PORT ^= 1; // XOR inverter
    } 
} 

void c51_tmr1(void) interrupt 3 
{
    state++;
} 
