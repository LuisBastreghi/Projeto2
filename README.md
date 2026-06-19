Introdução:
O objetivo deste trabalho foi projetar o hardware e o firmware de um dispositivo para aferição de temperatura interna de um forno industrial, em conjunto com um sistema de contagem regressiva de tempo pré-determinado. O projeto foi implementado utilizando o microcontrolador PIC18F4550 da Microchip e validado através do simulador SimulIDE.

Arquitetura de Hardware:
O circuito foi projetado para interagir com múltiplos periféricos simultâneos, exigindo a alocação correta dos pinos do microcontrolador para evitar conflitos:

Microcontrolador: PIC18F4550 operando com oscilador de 8 MHz.

Interface Visual (LCD): Um display LCD HD44780 configurado para comunicação no modo serial de 4 bits, com os pinos de dados e controle mapeados no PORTD.

Botões de Comando: Três botões (push buttons) operando com resistores de pull-up (10kΩ) conectados aos pinos RB0, RB1 e RB2, aproveitando o recurso de interrupções externas de hardware.

Sensor de Temperatura: A variação térmica de 0°C a 100°C do sensor LM35 foi simulada através de um potenciômetro conectado ao pino analógico RA0 (Canal AN0).

Referência de Tensão: Uma fonte externa de 1V foi aplicada ao pino RA3 (Vref+), com o pino RA2 (Vref-) aterrado, para garantir máxima resolução do conversor A/D.

Atuador (Resistência): Um LED conectado ao pino RC0 sinaliza o acionamento da resistência térmica do forno.

Lógica de Funcionamento e Firmware:
O código foi desenvolvido na IDE MikroC PRO for PIC, aplicando técnicas otimizadas para sistemas embarcados.

1. Sistema de Interrupções e Contagem de Tempo:
A seleção do tempo de aferição e o início do processo são feitos exclusivamente via hardware, sem travar o laço principal do código. Os botões geram interrupções externas na mudança de borda de subida.
Para a contagem regressiva, os temporizadores internos do PIC foram configurados:

Tempo Curto (10s): Utiliza o TMR1 para gerar uma base de tempo de 250 milissegundos.

Tempo Longo (60s): Utiliza o TMR0 para gerar uma base de tempo de 1 segundo.

2. Leitura Analógica e Otimização de Memória:
A conversão do sinal do potenciômetro é feita pelo módulo ADC de 10 bits. Para adequar a sensibilidade de leitura (o LM35 varia 10mV/°C), o registrador ADCON1 foi configurado para utilizar a tensão de referência externa de 1V, permitindo mapear o fundo de escala (1023) exatamente para os 100°C exigidos no projeto.
Toda a matemática de conversão e separação dos dígitos decimais para exibição na tela no formato XX.X °C foi estruturada utilizando apenas variáveis inteiras (int e long). O uso de variáveis de ponto flutuante (float) foi estritamente evitado para economizar memória de dados do microcontrolador.

3. Controle da Resistência:
Um laço condicional monitora a leitura do ADC continuamente. Caso a temperatura registrada ultrapasse a marca de 50°C, a porta RC0 recebe nível lógico alto, acendendo o LED que representa a resistência do forno.

Arquivos do Repositório:
Projeto2.c: Código-fonte integral em linguagem C.

Projeto2.hex: Firmware compilado, pronto para gravação no microcontrolador.

Projeto2.simu: Arquivo de simulação do circuito no software SimulIDE.
