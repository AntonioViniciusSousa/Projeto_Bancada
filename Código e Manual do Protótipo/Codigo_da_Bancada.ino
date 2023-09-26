/*
  Projeto: Bancada Multiparamétrica 
  Curso: Tecnologia em Saneamento Ambiental
  Desenvolvedores: Anna Patrícia de Lima, Antônio Vinícius da Silva Sousa e Heraldo Antunes Silva Filho
  Orientador: Heraldo Antunes Silva Filho
  Data: xx/xx/2023
*/

// Inclusão de Bibliotecas.
#include <Wire.h>                          // Biblioteca para utilizar os displays LCD via comunicação I2C
#include <LiquidCrystal_I2C.h>             // Biblioteca para utilizar os displays LCD via comunicação I2C
#include <OneWire.h>                       // Biblioteca para utilizar o termômetro DS18B20 à prova d’água                 
#include <DallasTemperature.h>             // Biblioteca para utilizar o termômetro DS18B20 à prova d’água            
#include <Thermistor.h>                    // Biblioteca para utilizar o termômetro ambiente

// Definições e Criação de Objetos.
#define TensaoRef 1024                     // Definição da variável de referência da tensão elétrica do protótipo

// Definição das Variáveis da Condutividade Elétrica - CE.
float SensorCE;                            // Corresponde ao valor de CE, em mg/L

// Definição das Variáveis de Potencial Hidrogeniônico - pH.
float pH = A0;                             // Definição da variável pH na entrada analógica A0
float SinalAnapH;                          // Corresponde ao sinal analógico do sensor de pH
float TensaoElepH;                         // Corresponde a tensão elétrica do sensor de pH
float SensorpH;                            // Corresponde ao valor de pH
float MiliVoltpH;                          // Corresponde ao valor de pH, em milivolts

// Definição das Variáveis de Sólidos Dissolvidos Totais - SDT.
float SDT = A1;                            // Definição da variável Sólidos Dissolvidos Totais (SDT) na entrada analógica A1
float SinalAnaSDT;                         // Corresponde ao sinal analógico do sensor de SDT
float TensaoEleSDT;                        // Corresponde a tensão elétrica do sensor de SDT
float CoeficienteCompensacao;              // Corresponde ao coeficiente de compensação da temperatura para os SDT
float TensaoEletricaCompensacao;           // Corresponde ao coeficiente de compensação da tensão elétrica para os SDT
float SensorSDT;                           // Corresponde ao valor de SDT, em mg/L

// Definição das Variáveis de Temperatura Ambiente.
float SensorTA;                            // Corresponde ao valor de temperatura ambiente, em °C

// Definição das Variáveis de Temperatura Líquida.
#define TempL 7                            // Definição da variável Temperatura Líquida na entrada digital 7
float SensorTL;                            // Corresponde ao valor de temperatura líquida, em °C

// Definição das Variáveis de Turbidez.
float Turbidez = A3;                       // Definição da variável Turbidez na entrada analógica A3
float SinalAnaTurb;                        // Corresponde ao sinal analógico do sensor de turbidez
float TensaoEleTurb;                       // Corresponde a tensão elétrica do sensor de turbidez
float SensorTurb;                          // Corresponde ao valor de turbidez, em NTU

// Instanciando Objetos.
LiquidCrystal_I2C lcd1(0x27, 16, 2);       // °C | °C | NTU
LiquidCrystal_I2C lcd2(0x26, 16, 2);       // pH | CE | SDT
OneWire ourWire(TempL);                    // Configura uma instância ONEWIRE para se comunicar com o sensor de temperatura líquido
DallasTemperature SensorTempL(&ourWire);   // A biblioteca DallasTemperature utiliza a OneWire
Thermistor TempA(A2);                      // Variável do tipo THERMISTOR, indicando o pino analógico (A2) em que o TERMISTOR está conectado

void setup() {
  Serial.begin(9600);                                         // Inicialização do Monitor Serial
  SensorTempL.begin();                                        // Inicia o sensor de temperatura
  lcd1.init();                                                // Inicia a comunicação com o primeiro Display LCD
  lcd2.init();                                                // Inicia a comunicação com o segundo Display LCD
  lcd1.backlight();                                           // Liga a iluminação do primeiro Display LCD
  lcd2.backlight();                                           // Liga a iluminação do segundo Display LCD
  lcd1.clear();                                               // Limpa o primeiro Display LCD
  lcd2.clear();                                               // Limpa o segundo Display LCD
}

void loop(){

// ###################################################################################################################################

// ##### POTENCIAL HIDROGENIÔNICO ##### 
  SinalAnapH = analogRead(pH);                                // Leitura do sinal analógico
  TensaoElepH = (SinalAnapH / TensaoRef) * 5;                 // Conversão do sinal analógico para tensão elétrica
  SensorpH = (TensaoElepH - 4.9984) / - 0.1887;               // Cálculo do pH
  MiliVoltpH = (SensorpH * -59.16) + 414.12;                  // Cálculo do pH em Milivolts (mv)

// ###################################################################################################################################

// ##### TEMPERATURA AMBIENTE #####
  SensorTA = TempA.getTemp();                                 // Variável que recebe o valor de temperatura calculado pela biblioteca THERMISTOR
  
// ###################################################################################################################################

// ##### TEMPERATURA LÍQUIDA #####
  SensorTL = SensorTempL.getTempCByIndex(0);                  // Coleta a temperatura do líquido através do sensor DS18B20

// ###################################################################################################################################

// ##### SÓLIDOS DISSOLVIDOS TOTAIS ##### 
  SinalAnaSDT = analogRead(SDT);                              // Leitura do sinal analógico
  TensaoEleSDT = (SinalAnaSDT / TensaoRef) * 5;               // Conversão do sinal analógico para tensão elétrica
  CoeficienteCompensacao = 1.0 + 0.02 * (SensorTL - 25.0);
  TensaoEletricaCompensacao = TensaoEleSDT / CoeficienteCompensacao;
  SensorSDT = (TensaoEletricaCompensacao * 506.85) - 117.82;  // Cálculo dos Sólidos Dissolvidos Totais em Miligramas por Litro (mg/L)

// ###################################################################################################################################

// ##### TURBIDEZ ##### 
  SinalAnaTurb = analogRead(Turbidez);                        // Leitura do sinal analógico
  TensaoEleTurb = (SinalAnaTurb / TensaoRef) * 5;             // Conversão do sinal analógico para tensão elétrica
  SensorTurb = (TensaoEleTurb - 2.5768) / - 0.0026;           // Cálculo da Turbidez em Unidade Nefelométrica de Turbidez (NTU)
  
// ###################################################################################################################################

// ##### CONDUTIVIDADE ELÉTRICA #####
  SensorCE = (SensorSDT + 11.52) / 0.69;                      // Derterminando condutividade eletrica com base nos SDT dentro dos limites da condutividade elétrica (CE) que é entre 31,4 e 989 μS/cm

// ###################################################################################################################################

// Impressão no Display LCD.      
  SensorTempL.requestTemperatures();                          // Solicita que a função informe a temperatura do sensor
  lcd1.setCursor(0, 0);                                       // Posiciona o cursor na primeira coluna da linha 1
  lcd1.print("T.Liq");
  lcd1.setCursor(6, 0);                                       // Posiciona o cursor na sétima coluna da linha 1
  lcd1.print("T.Amb");              
  lcd1.setCursor(12, 0);                                      // Posiciona o cursor na décima terceira coluna da linha 1
  lcd1.print("Turb");
  
  lcd1.setCursor(0, 1);                                       // Posiciona o cursor na primeira coluna da linha 2
  lcd1.print(SensorTA, 2);
  lcd1.setCursor(6, 1);                                       // Posiciona o cursor na sétima coluna da linha 2
  lcd1.print(SensorTL, 2);
  lcd1.setCursor(12, 1);                                      // Posiciona o cursor na décima terceira coluna da linha 2
  lcd1.print(SensorTurb);

  lcd2.setCursor(0, 0);                                       // Posiciona o cursor na primeira coluna da linha 1
  lcd2.print("pH");
  lcd2.setCursor(6, 0);                                       // Posiciona o cursor na sétima coluna da linha 1
  lcd2.print("CE");
  lcd2.setCursor(12, 0);                                      // Posiciona o cursor na décima terceira coluna da linha 1
  lcd2.print("SDT");
  
  lcd2.setCursor(0, 1);                                       // Posiciona o cursor na primeira coluna da linha 2
  lcd2.print(SensorpH, 2);
  lcd2.setCursor(6, 1);                                       // Posiciona o cursor na sétima coluna da linha 2
  lcd2.print(SensorCE, 1);
  lcd2.setCursor(12, 1);                                      // Posiciona o cursor na décima terceira coluna da linha 2
  lcd2.print(SensorSDT);
}
