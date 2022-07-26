/**
    @brief Modifique o programa do roteiro do laboratório para que o mesmo
    apenas receba dados e os armazene em 5 posições da memória RAM
    interna. Armazenar os dados de forma cíclica; ou seja, ao término do
    preenchimento do quinta posição da memória, voltar a preencher a partir
    da primeira. Para enviar dado para a recepção serial pela Debug
    session, tem-se que digitar (por exemplo) SIN=0x33 na janela de
    comando do Keil (no prompt (>) da janela de comando).

    Resolucao: para que o 8051 apenas receba dados basta mudar o teste que está
    dentro do tratamento de interrupção, ao invés de olharmos para a variável TI
    (que indica dado trasmitido) podemos continuamente verificar o flip flop RI,
    analisando se algum dado foi recebido.
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

unsigned char data received_data_1;
unsigned char data received_data_2;
unsigned char data received_data_3;
unsigned char data received_data_4;
unsigned char data received_data_5;

unsigned char state = 0;

void main(void) 
{
    unsigned char aux = 1;

    scon_init();
    timer_init();

    ES = 1; 
    EA = 1; 

    TH1 = 0xFD; // recharge value for 9600 baud rate, considering clk = 11,059 MHz;
    TR1 = 1; 

    while (1) 
    {
        while(state != 1); 
        state = 0; 
        switch(aux)
        {
            case 1:
            received_data_1 = SBUF; break;
            case 2:
            received_data_2 = SBUF; break;
            case 3:
            received_data_3 = SBUF; break;
            case 4:
            received_data_4 = SBUF; break;
            case 5:
            received_data_5 = SBUF; break;
        }
        aux++;
        if(aux == 6) aux = 1;
    } 
} 

void serial(void) interrupt 4 
{ 
    if(RI) 
    { 
        RI = 0; 
        state = 1;
    }
}