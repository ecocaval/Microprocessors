/**
    @brief Utilize o timer 0 no modo 2, para enviar os caracteres da mensagem "Microcontrolador" em intervalos de 160 ciclos de instrução para a porta P1. Caso ocorra interrupção externa 1 (solicitada por borda de descida), e sempre que a mesma ocorrer, ler dado da porta P2 (a ser especificado pelo usuário); este valor deve ser utilizado pelo timer 0 no modo 2 para determinar o valor de início de contagem em TH0.

    resolução: Para que sejam contados 160 ciclos de instrução em cada ciclo o valor de recarga em TH0 deverá ser de: 256 - 160 = 96 = 0x60;

    TIMER 0 + MODO 2 + 160 ciclos de inst.
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

#define P2_SET_BY_USER 0xAA // just an example value 

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
void c51_ext1(void);

unsigned char state = 0;

void main(void) 
{
    unsigned char code message[]= MESSAGE_TO_SEND;
    unsigned char code *message_ptr;
    unsigned char aux = 0;

    message_ptr = message;

    P2 = P2_SET_BY_USER;

    timer_init();
    TH0 = TH0_VALUE; 
    TL0 = TL0_VALUE; 

    ET0 = 1;
    EX1 = 1;
    EA = 1; 

    IT1 = 1; /* interrupção externa 1 será 
                sensível a borda de descida */

    TR0 = 1;

    while(1) {
        while(state == 0); 
        if(state == 1)
        {
            if(state != 2) state = 0; /*
                                        Tests if external interruption was not set while "if(state = 1)" code is running.
                                      */
            P1 = *(message_ptr+aux++);
            if (aux == 16) aux = 0;
        }
        if (state == 2)
        {
            state = 0;
            TH0 = P2;
        }
    } 
} 

void c51_tmr0(void) interrupt 1 
{
    state = 1;
} 

void c51_ext1(void) interrupt 2 
{
    state = 2;
} 
