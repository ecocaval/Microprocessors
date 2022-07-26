#include <reg51.h>

void serial(void); // protótipo de função

unsigned char state = 0;

void main(void) 
{
    unsigned char code mensagem[]= "Microcontrolador";
    unsigned char code *ponteiro;
    unsigned char aux = 0;
    ponteiro=mensagem;

    SCON = 0x40; // SCON: modo 1, 8-bit
    TMOD |= 0x20; // TMOD: timer 1, modo 2
    TH1 = 0xFD; // TH1: valor de recarga para 9600 baud; clk = 11,059 MHz
    TR1 = 1; // TR1: dispara timer
    ES = 1; EA = 1; // habilita interrupcao serial
    SBUF = *ponteiro; // envia µM'
    aux++;

    while (1) 
    {
        while(state != 1); 
        state = 0; 
        SBUF = *(ponteiro+aux++);
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