#include <WiFi.h>
#include <ESPAsyncWebSrv.h>

// Configurações de rede Wi-Fi
const char* ssid = "WiFi_PradoLux";
const char* password = "pr0701lux";

// Configuração da GPIO
const int porta34 = 34;

// Inicialização do servidor e WebSocket
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

unsigned long lastSendTime = 0;
const long interval = 1000; // Intervalo de 1 segundo

// Função para enviar dados via WebSocket
void sendData() {
  int valor = analogRead(porta34); 
  String mensagem = String(valor);
  ws.textAll(mensagem); // Envia a mensagem para todos os clientes conectados
}

void setup() {
  // Inicializa a comunicação serial
  Serial.begin(9600);

  // Configura a GPIO 34 como entrada
  pinMode(porta34, INPUT);

  // Conecta ao Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao Wi-Fi...");
  }
  Serial.println("Conectado ao Wi-Fi!");

  // Configura o WebSocket
  ws.onEvent([](AsyncWebSocket *server, AsyncWebSocketClient *client, 
                AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
      Serial.println("Cliente conectado!");
    } else if (type == WS_EVT_DISCONNECT) {
      Serial.println("Cliente desconectado!");
    }
  });
  server.addHandler(&ws);

  // Inicia o servidor
  server.begin();
}

void loop() {
  // Processa WebSocket
  ws.cleanupClients();  // Limpa clientes desconectados

  // Verifica o tempo e envia dados a cada 1 segundo
  unsigned long currentMillis = millis();
  if (currentMillis - lastSendTime >= interval) {
    lastSendTime = currentMillis;  // Atualiza o tempo da última mensagem
    sendData();  // Envia os dados
    Serial.println("Dados enviados!");
  }
}
