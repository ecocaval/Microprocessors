/**
    @brief Compile e execute o programa abaixo que transmite, serialmente, a cadeia de
    16 caracteres: "Microcontrolador". A transmissão é assíncrona sem envio de bit
    de paridade à taxa de 9600 bauds (bit/s). Para visualizar dados transmitidos,
    selecionar no menu: View, Serial Windows, UART #1. obs: Informar cristal a
    ser utilizado no hardware (11,059 MHz) para o Keil: Project - Options for Target
    (ou Alt + F7); modificar o campo Xtal.

    SM0|SM1 MODO 1 + 9600 baud
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

#define SM0_SM1_MODE_0        0x00 // 0000 0000
#define SM0_SM1_MODE_1        0x40 // 0100 0000
#define SM0_SM1_MODE_2        0x80 // 1000 0000
#define SM0_SM1_MODE_3        0xC0 // 1100 0000
#define SET_SM2_TO_0          0x00 // 0000 0000
#define SET_REN_TO_0          0x00 // 0000 0000
#define SET_TB8_TO_0          0x00 // 0000 0000
#define SET_SM2_TO_1          0x20 // 0010 0000
#define SET_REN_TO_1          0x10 // 0001 0000
#define SET_TB8_TO_1          0x00 // 0000 1000

#define TH0_VALUE 0x00 // 0000 0000
#define TL0_VALUE 0x00 // 0000 0000

#define TH1_VALUE 0x15 // 0001 0101
#define TL1_VALUE 0xA0 // 1010 0000

#define MESSAGE_TO_SEND "Microcontrolador"

struct TMOD_init_struct 
{ 
    unsigned char mode; 
    unsigned char cntr_tmr; 
    unsigned char gate; 
} timer_0, timer_1;

struct SCON_init_struct 
{ 
    unsigned char mode; 
    unsigned char SM2; 
    unsigned char REN; 
    unsigned char TB8; 
} scon_config;

void timer_init(void) 
{
    unsigned char TMOD_aux;

    timer_1.mode = TMR_CNTR_MODE_2;
    timer_1.cntr_tmr = TMR_CNTR_SET_TO_TMR;
    timer_1.gate = TMR_CNTR_GATE_LOW;

    TMOD_aux = (timer_1.mode | timer_1.cntr_tmr | timer_1.gate) << 4;

    timer_0.mode = 0x00; 
    timer_0.cntr_tmr = 0x00;  
    timer_0.gate = 0x00; 

    TMOD = TMOD_aux | (timer_0.mode | timer_0.cntr_tmr | timer_0.gate);
 }

void scon_init(void) 
{
    unsigned char SCON_aux;

    scon_config.mode = SM0_SM1_MODE_1;
    scon_config.SM2 = SET_SM2_TO_0;
    scon_config.REN = SET_REN_TO_0;
    scon_config.TB8 = SET_TB8_TO_0;

    SCON_aux = (scon_config.mode | scon_config.SM2 | 
                scon_config.REN | scon_config.TB8);
    SCON = SCON_aux;
 }

void serial(void); 

unsigned char state = 0;

void main(void) 
{
    unsigned char code message[]= MESSAGE_TO_SEND;
    unsigned char code *message_ptr;
    unsigned char aux = 0;
    
    message_ptr = message;

    scon_init();
    timer_init();

    ES = 1; 
    EA = 1; 

    TH1 = 0xFD; // recharge value for 9600 baud rate, considering clk = 11,059 MHz;
    TR1 = 1; 

    SBUF = *message_ptr; // to send and receive data we use SBUF 
    aux++;

    while (1) 
    {
        while(state != 1); 
        state = 0; 
        SBUF = *(message_ptr+aux++);
        if(aux == 16) aux = 0;
    } 
} 

void serial(void) interrupt 4 
{ 
    if(TI) 
    { 
        TI = 0; 
        state = 1;
    }
}