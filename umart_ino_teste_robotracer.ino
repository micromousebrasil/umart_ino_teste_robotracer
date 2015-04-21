/**
 *****************************************************************
 * Arquivo  umart_ino_teste_robotracer.ino
 * Autor    Kleber Lima da Silva (kleber.ufu@hotmail.com)
 * Versão   V1.0.0
 * Data     19-Abril-2015
 * Resumo   Programa para teste do hardware do Robô uMaRT-INO no
 *         modo robotracer (seguidor de linha)
 *****************************************************************
 */
 
// Bibliotecas ---------------------------------------------------
#include <Encoder.h>

// Definição dos pinos no modo ROBOTRACER ------------------------
const int LINHA6 = A0;     // Sensor de linha 6 (esquerda)
const int LINHA5 = A1;     // Sensor de linha 5
const int LINHA4 = A2;     // Sensor de linha 4
const int LINHA3 = A3;     // Sensor de linha 3
const int LINHA2 = A4;     // Sensor de linha 2
const int LINHA1 = A5;     // Sensor de linha 1 (direita)
const int EMISSORES = 13;  // LEDs dos sensores de linha
const int LED1 = 0;        // LED1
const int LED2 = 1;        // LED2
const int B_ENC_E = 2;     // Encoder do motor esquerdo (sinal B)
const int A_ENC_D = 3;     // Encoder do motor direito (sinal A)
const int A_ENC_E = 4;     // Encoder do motor esquerdo (sinal A)
const int B_ENC_D = 5;     // Encoder do motor direito (sinal B)
const int PWM_E = 6;       // PWM do motor esquerdo
const int IN2_E = 7;       // IN2 da ponte H do motor esquerdo
const int IN1_E = 8;       // IN1 da ponte H do motor esquerdo
const int IN1_D = 9;       // IN1 da ponte H do motor direito
const int IN2_D = 10;      // IN2 da ponte H do motor direito
const int PWM_D = 11;      // PWM do motor direito
const int SW1 = 12;        // Botão SW1

// Configuração dos Encoders -------------------------------------
Encoder encoderEsquerda(B_ENC_E, A_ENC_E);
Encoder encoderDireita(A_ENC_D, B_ENC_D);

// Definição das constantes do programa --------------------------
#define LINHA LOW  // LOW: linha branca | HIGH: linha preta
#define INFINITO 255  // Para indicar que nenhum sensor leu linha


// Protótipos das funções ----------------------------------------
int lerSensoresLinha(void);
void setMotores(int pwm_esquerda, int pwm_direita);


// Inicialização dos pinos ---------------------------------------
void setup()
{ 
  pinMode(LINHA6, INPUT);
  pinMode(LINHA5, INPUT);
  pinMode(LINHA4, INPUT);
  pinMode(LINHA3, INPUT);
  pinMode(LINHA2, INPUT);
  pinMode(LINHA1, INPUT);
  pinMode(EMISSORES, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(PWM_E, OUTPUT);
  pinMode(IN1_E, OUTPUT);
  pinMode(IN2_E, OUTPUT);
  pinMode(PWM_D, OUTPUT);
  pinMode(IN1_D, OUTPUT);
  pinMode(IN2_D, OUTPUT);
  pinMode(SW1, INPUT);
  
  Serial.begin(9600);
}

// LOOP principal do programa ------------------------------------
void loop()
{
  int sensores;
  unsigned long t0;
  
  // Verifica a leitura dos sensores e mostra na "serial"
  sensores = lerSensoresLinha();
  if(sensores == INFINITO)
  {
    Serial.println("NENHUM sensor leu a linha");
  }
  else
  {
    Serial.print("Sensores de Linha: ");
    Serial.println(sensores);
  }
  
  // Atualiza a leitura dos encoders e mostra na "serial"
  Serial.println("Encoders: ");
  Serial.println(encoderEsquerda.read());
  Serial.println(encoderDireita.read());
  Serial.println();

  // Se o botão for pressionado: liga os motores por 1 segundo
  // .. ou aguarda 1 segundo sem fazer nada
  t0 = millis();
  while((millis() - t0) < 1000)
  {
    if(digitalRead(SW1) == HIGH)
    {
      setMotores(50, 50);
      delay(1000);
      setMotores(0, 0);
    }
  }
}


/* Função para leitura dos sensores de linha ---------------------
 * Retorna valores entre -30 e 30 (valores negativos indicam que
 * o robô precisa se deslocar para a esquerda)
 */
int lerSensoresLinha(void)
{
  int erro = 0, soma = 0, n = 0;
  unsigned long t0 = micros();
  
  // Habilita os emissores por 100 us
  digitalWrite(EMISSORES, HIGH);
  while((micros() - t0) < 100);
  
  // Realiza a leitura de todos os sensores de linha. Os sensores
  // das extremidades possuem peso maior. No final é realizada
  // a média ponderada
  if(digitalRead(LINHA1) == LINHA)
  {
    soma += -30;
    n++;
  }
  if(digitalRead(LINHA2) == LINHA)
  {
    soma += -20;
    n++;
  }
  if(digitalRead(LINHA3) == LINHA)
  {
    soma += -10;
    n++;
  }
  if(digitalRead(LINHA4) == LINHA)
  {
    soma += 10;
    n++;
  }
  if(digitalRead(LINHA5) == LINHA)
  {
    soma += 20;
    n++;
  }
  if(digitalRead(LINHA6) == LINHA)
  {
    soma += 30;
    n++;
  }
  
  // Desabilita os emissores
  digitalWrite(EMISSORES, LOW);
  
  // Retorna a média ou retorna a constante INFINITO indicando
  // que nenhum sensor leu linha
  if(n != 0) erro = soma / n;
  else erro = INFINITO;
  
  return erro;
}


/* Função para acionamento dos motores ---------------------------
 * pwm_esquerda e pwm_direita recebem valores entre -255 e 255
 * (valores negativos giram o respectivo motor para trás)
 */
void setMotores(int pwm_esquerda, int pwm_direita)
{
  if(pwm_esquerda < 0)
  {
    pwm_esquerda *= -1;
  
    digitalWrite(IN1_E, LOW);
    digitalWrite(IN2_E, HIGH);
  }
  else
  {
    digitalWrite(IN1_E, HIGH);
    digitalWrite(IN2_E, LOW);
  }
  
  if(pwm_direita < 0)
  {
    pwm_direita *= -1;
  
    digitalWrite(IN1_D, LOW);
    digitalWrite(IN2_D, HIGH);
  }
  else
  {
    digitalWrite(IN1_D, HIGH);
    digitalWrite(IN2_D, LOW);
  }
  
  if(pwm_esquerda > 255) pwm_esquerda = 255;
  if(pwm_direita > 255) pwm_direita = 255;
  
  analogWrite(PWM_E, pwm_esquerda);
  analogWrite(PWM_D, pwm_direita);
}

