#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <PubSubClient.h>

//   LCD  
LiquidCrystal_I2C lcd(0x27, 16, 2);

//   Pinos  
#define BOTAO_PASSO  4
#define LED_META     2
#define BUZZER_PIN   5

//   Meta 
#define PASSOS_META  5

//   Wi-Fi  
const char* ssid     = "Wokwi-GUEST";
const char* password = "";

//   MQTT / FIWARE  
const char* mqttBroker = "broker.hivemq.com";
const int   mqttPort   = 1883;
const char* mqttTopic  = "/TEF/smartwatch001/attrs";

WiFiClient   espClient;
PubSubClient mqttClient(espClient);

//   Histórico  
struct Registro {
  int passos;
  int metas;
};

const int MAX_REGISTROS = 20;
Registro  historico[MAX_REGISTROS];
int       totalRegistros = 0;

//   Estado global  
int           passosHoje      = 0;
int           metasBatidas    = 0;
bool          ultimoBotao     = HIGH;
unsigned long tempoAcendeuLed = 0;
bool          ledAceso        = false;


void conectarWiFi() {
  Serial.print("Conectando ao WiFi");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

void conectarMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Conectando ao MQTT...");
    if (mqttClient.connect("smartwatch001-esp32")) {
      Serial.println(" conectado!");
    } else {
      Serial.print(" falhou. Codigo: ");
      Serial.println(mqttClient.state());
      delay(2000);
    }
  }
}

void publicarFIWARE() {
  if (!mqttClient.connected()) {
    conectarMQTT();
  }
  char payload[100];
  snprintf(payload, sizeof(payload), "s|%d|m|%d", passosHoje, metasBatidas);
  mqttClient.publish(mqttTopic, payload);
  Serial.print("MQTT publicado: ");
  Serial.println(payload);
}

void salvarHistorico() {
  if (totalRegistros < MAX_REGISTROS) {
    historico[totalRegistros].passos = passosHoje;
    historico[totalRegistros].metas  = metasBatidas;
    totalRegistros++;
  }
}

void bipePasso() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(60);
  digitalWrite(BUZZER_PIN, LOW);
}

void atualizarLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Passos: ");
  lcd.print(passosHoje);
  lcd.setCursor(0, 1);
  lcd.print("Metas:  ");
  lcd.print(metasBatidas);
}


void setup() {
  Serial.begin(115200);

  Wire.begin(21, 22);
  lcd.init();
  lcd.backlight();

  pinMode(BOTAO_PASSO, INPUT_PULLUP);
  pinMode(LED_META,    OUTPUT);
  pinMode(BUZZER_PIN,  OUTPUT);

  digitalWrite(LED_META,   LOW);
  digitalWrite(BUZZER_PIN, LOW);

  lcd.setCursor(0, 0);
  lcd.print("  SmartWatch!  ");
  lcd.setCursor(0, 1);
  lcd.print(" Iniciando...  ");

  conectarWiFi();
  mqttClient.setServer(mqttBroker, mqttPort);
  conectarMQTT();

  delay(1500);
  atualizarLCD();
}


void loop() {
  unsigned long agora = millis();

  if (!mqttClient.connected()) {
    conectarMQTT();
  }
  mqttClient.loop();

  bool btnAtual = digitalRead(BOTAO_PASSO);
  if (ultimoBotao == HIGH && btnAtual == LOW) {

    passosHoje++;
    bipePasso();
    salvarHistorico();

    Serial.print("Passo: ");
    Serial.println(passosHoje);

    if (passosHoje % PASSOS_META == 0) {
      metasBatidas++;
      digitalWrite(LED_META, HIGH);
      tempoAcendeuLed = agora;
      ledAceso = true;
      salvarHistorico();

      Serial.print("Meta batida! Total: ");
      Serial.println(metasBatidas);
    }

    atualizarLCD();
    publicarFIWARE();

    delay(50);
  }
  ultimoBotao = btnAtual;

  if (ledAceso && (agora - tempoAcendeuLed >= 800)) {
    digitalWrite(LED_META, LOW);
    ledAceso = false;
  }
}
