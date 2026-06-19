// Configurações dos pinos do LCD
sbit LCD_RS at LATD4_bit;
sbit LCD_EN at LATD5_bit;
sbit LCD_D4 at LATD0_bit;
sbit LCD_D5 at LATD1_bit;
sbit LCD_D6 at LATD2_bit;
sbit LCD_D7 at LATD3_bit;

sbit LCD_RS_Direction at TRISD4_bit;
sbit LCD_EN_Direction at TRISD5_bit;
sbit LCD_D4_Direction at TRISD0_bit;
sbit LCD_D5_Direction at TRISD1_bit;
sbit LCD_D6_Direction at TRISD2_bit;
sbit LCD_D7_Direction at TRISD3_bit;

// Variáveis Globais
int tempo_restante = 0;
short contando = 0;
short modo_selecionado = 0; // 1 = Curto, 2 = Longo
int contador_auxiliar = 0;

// Rotina de Interrupções (Botões e Timers)
void interrupt() {
    // Botão Tempo Curto (RB0)
    if (INTCON.INT0IF) {
        if (!contando) { modo_selecionado = 1; tempo_restante = 10; }
        INTCON.INT0IF = 0; // Limpa a flag
    }

    // Botão Tempo Longo (RB1)
    if (INTCON3.INT1IF) {
        if (!contando) { modo_selecionado = 2; tempo_restante = 60; }
        INTCON3.INT1IF = 0;
    }

    // Botão Iniciar (RB2)
    if (INTCON3.INT2IF) {
        if (modo_selecionado > 0 && !contando) {
            contando = 1;
            // Inicia os Timers
            if (modo_selecionado == 1) {
                T1CON = 0x31; // TMR1 para 250ms
                PIE1.TMR1IE = 1;
            } else {
                T0CON = 0x84; // TMR0 para 1s
                INTCON.TMR0IE = 1;
            }
        }
        INTCON3.INT2IF = 0;
    }

    // Timer0 - Estouro a cada 1 segundo
    if (INTCON.TMR0IF) {
        TMR0H = 0x0B; TMR0L = 0xDC; // Recarrega valor para 1s a 8MHz
        if (tempo_restante > 0) tempo_restante--;
        INTCON.TMR0IF = 0;
    }

    // Timer1 - Estouro a cada 250ms
    if (PIR1.TMR1IF) {
        TMR1H = 0x0B; TMR1L = 0xDC; // Recarrega valor para 250ms a 8MHz
        contador_auxiliar++;
        if (contador_auxiliar >= 4) { // 4x 250ms = 1 segundo
            contador_auxiliar = 0;
            if (tempo_restante > 0) tempo_restante--;
        }
        PIR1.TMR1IF = 0;
    }
}

void main() {
    unsigned int leitura_adc;
    unsigned long temp_calculada;
    char txt_temp[10];
    char txt_tempo[10];

    // Configuração de Portos
    TRISA = 0xFF;  // Entradas Analógicas
    TRISB = 0xFF;  // Botões
    TRISC.RC0 = 0; // LED
    PORTC.RC0 = 0;

    // Configuração de Interrupções Externas
    INTCON2.INTEDG0 = 1; // Borda de subida
    INTCON2.INTEDG1 = 1;
    INTCON2.INTEDG2 = 1;
    INTCON.INT0IE = 1;
    INTCON3.INT1IE = 1;
    INTCON3.INT2IE = 1;
    INTCON.GIE = 1; // Ativa interrupções globais
    INTCON.PEIE = 1;

    // Inicialização do LCD
    Lcd_Init();
    Lcd_Cmd(_LCD_CLEAR);
    Lcd_Cmd(_LCD_CURSOR_OFF);

    // Inicialização do ADC com Referência Externa a 1V (Pinos A2 e A3)
    ADC_Init();
    ADCON1 = 0x3B;

    while(1) {
        if (contando) {
            // Leitura da Temperatura
            leitura_adc = ADC_Get_Sample(0);
            temp_calculada = (leitura_adc * 100) / 1023; // Calcula direto para XX

            txt_temp[0] = (temp_calculada / 10) + '0';
            txt_temp[1] = (temp_calculada % 10) + '0';
            txt_temp[2] = 223; // Símbolo de grau
            txt_temp[3] = 'C';
            txt_temp[4] = '\0';

            Lcd_Out(1, 1, "Temp: ");
            Lcd_Out(1, 7, txt_temp);

            // Exibição do Tempo
            txt_tempo[0] = (tempo_restante / 10) + '0';
            txt_tempo[1] = (tempo_restante % 10) + '0';
            txt_tempo[2] = 's';
            txt_tempo[3] = '\0';
            Lcd_Out(2, 1, "Faltam: ");
            Lcd_Out(2, 9, txt_tempo);

            // Controle do LED (Resistência)
            if (leitura_adc > 511) PORTC.RC0 = 1; // Passou de 50 graus
            else PORTC.RC0 = 0;

            // Fim do tempo
            if (tempo_restante <= 0) {
                contando = 0;
                modo_selecionado = 0;
                PORTC.RC0 = 0;
                INTCON.TMR0IE = 0; // Desliga Timer0
                PIE1.TMR1IE = 0;   // Desliga Timer1
                Lcd_Cmd(_LCD_CLEAR);
            }
        } else {
            Lcd_Out(1, 1, "Selecione tempo:");
            if (modo_selecionado == 1) Lcd_Out(2, 1, "Curto: 10s      ");
            else if (modo_selecionado == 2) Lcd_Out(2, 1, "Longo: 60s      ");
            else Lcd_Out(2, 1, "                ");
        }
        Delay_ms(200);
    }
}