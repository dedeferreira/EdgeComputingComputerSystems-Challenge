# SmartWatch Simulator — Contador de Passos
**Edge Computing & Computer Systems | FIAP**  
**Sprint 2 — Prototipagem Digital e Arquitetura de Sistemas**
**Cainã Sandes Batista - RM:568571**
**André Rosa Ferreira Filho - RM:567149**
**Yuri Aroste Fukamachi - RM:567314**
**Michel Lima Benchimol - RM567345**

---

##  Descrição do Projeto

Simulação de um dispositivo Smartwatch baseado em **ESP32** desenvolvida no **Wokwi**. O dispositivo captura passos e metas e as envia via protocolo **MQTT** para a plataforma **FIWARE** através do broker **HiveMQ**.

---

## Link da Simulação
https://wokwi.com/projects/459510713203779585

---

## Arquitetura da Solução

```
┌─────────────────────────────────────────────────────────────┐
│                     EDGE LAYER (ESP32)                      │
│                                                             │
│  [Botão Verde]  →  Sensor de Passos (simulado)              │
│  [Buzzer]       →  Feedback Sonoro                          │
│  [LCD 16x2]     →  Display de Métricas                      │
└──────────────────────────┬──────────────────────────────────┘
                           │ Wi-Fi (Wokwi-GUEST)
                           ▼
┌─────────────────────────────────────────────────────────────┐
│                   MQTT LAYER                                │
│                                                             │
│  Broker: broker.hivemq.com : 1883                           │
│  Tópico: /TEF/smartwatch001/attrs                           │
│  Payload: s|{passos}|m|{metas}                              │
└──────────────────────────┬──────────────────────────────────┘
                           │ MQTT
                           ▼
┌─────────────────────────────────────────────────────────────┐
│                 BACK-END LAYER (FIWARE)                     │
│                                                             │
│  IoT Agent MQTT  →  Orion Context Broker                    │
│  Entidade: smartwatch001                                    │
│  Atributos: steps (s), metas (m)                            │
└──────────────────────────┬──────────────────────────────────┘
                           │ REST API
                           ▼
┌─────────────────────────────────────────────────────────────┐
│               APPLICATION LAYER                              │
│                                                             │
│  Dashboard de métricas de saúde                             │
│  Histórico de passos e metas                                │
└─────────────────────────────────────────────────────────────┘
```

---

## Componentes Utilizados

| Componente | Função | Pino ESP32 |
|---|---|---|
| Push Button (verde) | Simula sensor de passo | D4 |
| LED (amarelo) | Acende a cada 5 passos (meta) | D2 |
| Buzzer | Bipe sonoro a cada passo | D5 |
| LCD 16x2 I2C | Exibe passos e metas em tempo real | SDA(D21) / SCL(D22) |

---

## Configuração MQTT / FIWARE

| Parâmetro | Valor |
|---|---|
| Broker | `broker.hivemq.com` |
| Porta | `1883` |
| Tópico de publicação | `/TEF/smartwatch001/attrs` |
| Formato do payload | `s\|{passos}\|m\|{metas}` |
| Client ID | `smartwatch001-esp32` |

### Exemplo de payload enviado:
```
s|10|m|2
```
- `s` = steps (passos) = 10
- `m` = metas batidas = 2

---

## Funcionalidades

-  Contagem de passos a cada clique do botão
-  Bipe sonoro a cada passo registrado
-  LED acende por 800ms a cada 5 passos (meta batida)
-  LCD atualiza em tempo real com passos e metas
-  Histórico das últimas 20 sessões armazenado em memória
-  Envio de dados via MQTT para FIWARE a cada passo
-  Reconexão automática ao MQTT se a conexão cair

---

## Estrutura do Repositório

```
 smartwatch-esp32
 ┣ 📄 main.ino          # Firmware principal do ESP32
 ┣ 📄 diagram.json      # Diagrama de circuito do Wokwi
 ┣ 📄 libraries.txt     # Bibliotecas utilizadas
 ┗ 📄 README.md         # Documentação do projeto
```

---

## Bibliotecas Utilizadas

| Biblioteca | Versão | Uso |
|---|---|---|
| `LiquidCrystal I2C` | latest | Controle do LCD 16x2 via I2C |
| `PubSubClient` | latest | Comunicação MQTT |
| `WiFi` | built-in | Conexão Wi-Fi do ESP32 |
| `Wire` | built-in | Comunicação I2C |

