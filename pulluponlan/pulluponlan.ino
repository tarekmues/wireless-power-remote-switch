#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Ping.h>

const char* ssid = "#";
const char* password = "#";

WebServer server(80);

const int pin = 23;
const int statuspin = 25;

IPAddress usg (192, 168, 1, 1);

unsigned long lastmillis;

void setup() {
  Serial.begin(115200);
  Serial.println("ESP gestartet");
  delay(100);
  pinMode(statuspin, INPUT);
  pinMode(pin, OUTPUT);
  digitalWrite(pin, LOW);
  WiFi.begin(ssid, password);
  Serial.print("Verbindung wird hergestellt...");
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println();
  Serial.println("Verbindung wurde erfolgreich hergestellt");
  Serial.print("Meine IP: ");
  Serial.println(WiFi.localIP());
  lastmillis = millis();
  server.begin();
  
  server.on("/", []() {
      server.send(200, "text/html", "<html><head></head><body><p><a href=\"/on\">on</a></p><p><a href=\"/off\">off</a></p><p><a href=\"/status\">status</a></p><p><a href=\"/reset\">reset</a></p></body></html>");
  });

  server.on("/on", []() {
    if (digitalRead(statuspin) == LOW) {
        server.send(200, "text/html", "<html><head></head><body><p>Turning PC on!</p><p><a href=\"/\">back</a></p></body></html>");
        digitalWrite(pin, HIGH);
        delay(100);
        digitalWrite(pin, LOW);
    } else {
        server.send(200, "text/html", "<html><head></head><body><p>PC is already running!</p><p><a href=\"/\">back</a></p></body></html>");
    }
  });
  
  server.on("/off", []() {
      if (digitalRead(statuspin) == HIGH) {
        server.send(200, "text/html", "<html><head></head><body><p>Turning PC off!</p><p><a href=\"/\">back</a></p></body></html>");
        digitalWrite(pin, HIGH);
        delay(100);
        digitalWrite(pin, LOW);
    } else {
        server.send(200, "text/html", "<html><head></head><body><p>PC is already off!</p><p><a href=\"/\">back</a></p></body></html>");
    }
  });

  server.on("/reset", []() {
      server.send(200, "text/html", "<html><head></head><body><p>Your device gets now fully reset!</p><p><a href=\"/\">back</a></p></body></html>");
      digitalWrite(pin, HIGH);
      delay(10000);
      digitalWrite(pin, LOW);
  });

  server.on("/status", []() {
      if (digitalRead(statuspin) == HIGH) {
        server.send(200, "text/html", "<html><head></head><body><p>PC is turned on!</p><p><a href=\"/\">back</a></p></body></html>");
      } else {
        server.send(200, "text/html", "<html><head></head><body><p>PC is turned off!</p><p><a href=\"/\">back</a></p></body></html>");
      }
  });
}


void loop() {
  if (millis() - lastmillis >= 300000) {
    if(Ping.ping(usg) == true) {
      Serial.println("Echo received");
      lastmillis = millis();
    } else {
      ESP.restart();
    }
  }
  //Serial.println(millis());
  server.handleClient();
  delay(50);
}
