#include <WiFi.h>
#include <WebServer.h>
#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#define KHZ 38
#define PIN_SEND 32
IRsend irsend(PIN_SEND);
//URL http://192.168.11.6/ これは人によって違う。
uint16_t rawData1[83] = { 8400, 3978, 816, 1450, 840, 1450, 838, 492, 794, 438, 846, 440, 848, 466, 812, 1458, 846, 1448, 842, 436, 844, 1450, 812, 450, 864, 1428, 784, 498, 836, 474, 844, 442, 842, 468, 810, 442, 850, 450, 836, 1458, 844, 1448, 842, 438, 846, 440, 786, 494, 846, 548, 770, 442, 844, 440, 846, 444, 846, 468, 758, 504, 838, 520, 794, 444, 844, 468, 810, 438, 848, 1462, 810, 472, 842, 1450, 846, 438, 842, 1454, 842, 438, 844, 1450, 842 };  // UNKNOWN A4FF0604
//電源オン
uint16_t rawData2[83] = { 8316, 4020, 810, 462, 816, 498, 816, 1480, 762, 1556, 762, 470, 814, 494, 764, 488, 852, 1482, 784, 462, 822, 1482, 784, 498, 788, 1558, 760, 470, 816, 1480, 814, 464, 818, 494, 766, 486, 822, 464, 824, 1488, 806, 1480, 812, 472, 784, 498, 788, 488, 820, 548, 766, 474, 814, 472, 814, 470, 818, 468, 790, 490, 824, 546, 766, 472, 816, 498, 762, 490, 820, 1490, 810, 474, 784, 1562, 738, 490, 818, 1482, 836, 442, 842, 1462, 806 };  // UNKNOWN D153ABDF
//自動ボタン
uint16_t rawData3[83] = { 8342, 3988, 812, 470, 812, 468, 816, 1480, 816, 1478, 760, 488, 820, 550, 762, 472, 816, 1482, 814, 1480, 762, 1508, 782, 498, 810, 1482, 812, 468, 814, 1484, 812, 520, 736, 524, 762, 498, 790, 496, 790, 1558, 736, 1510, 812, 472, 782, 496, 816, 496, 764, 574, 764, 470, 816, 470, 818, 470, 816, 472, 816, 472, 786, 490, 850, 470, 818, 492, 768, 494, 792, 1556, 736, 500, 810, 1486, 810, 470, 786, 1510, 782, 546, 764, 1484, 812 };  // UNKNOWN 8080BCC5
//温度上昇ボタン
uint16_t rawData4[83] = { 8344, 4010, 816, 466, 812, 468, 816, 1480, 786, 1482, 812, 494, 762, 492, 846, 470, 816, 1482, 814, 468, 812, 1484, 762, 494, 788, 1556, 738, 498, 788, 1556, 734, 524, 762, 488, 822, 498, 812, 474, 814, 1482, 762, 1554, 738, 496, 786, 500, 784, 574, 740, 498, 816, 472, 814, 472, 792, 496, 788, 490, 820, 574, 740, 472, 814, 496, 766, 550, 790, 470, 816, 1458, 786, 494, 790, 1526, 766, 498, 786, 1558, 736, 500, 786, 1508, 812 };  // UNKNOWN D153ABDF
//電源下降ボタン
uint16_t rawData[83] = { 8370, 3970, 834, 1450, 838, 1454, 840, 434, 848, 434, 852, 434, 790, 496, 842, 1456, 786, 498, 842, 432, 882, 1420, 850, 432, 796, 1506, 814, 464, 850, 460, 816, 436, 884, 434, 854, 432, 854, 434, 852, 1448, 848, 1446, 808, 436, 882, 484, 776, 460, 852, 462, 790, 462, 856, 440, 850, 460, 852, 434, 850, 436, 854, 460, 770, 482, 860, 458, 824, 464, 824, 1476, 846, 434, 850, 1444, 764, 484, 858, 1446, 790, 496, 844, 1474, 784 };
// 電源オフ

const char *ssid = "";  // 自分のssid
const char *pass = "";   // 自分のパスワード

WebServer server(80);

void setup(void) {
  Serial.begin(115200);
  irsend.begin();
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  server.on("/", handleRoot);
  server.on("/sendIR1", handleSendIR1);
  server.on("/sendIR2", handleSendIR2);
  server.on("/sendIR3", handleSendIR3);
  server.on("/sendIR4", handleSendIR4);  // "Power Off" ボタンの新しいルートを追加
  server.onNotFound(handleNotFound);
  server.begin();
}

void loop() {
  server.handleClient();
}

void handleRoot() {
  String html = "<!DOCTYPE html><html><head><meta charset=\"utf-8\"><title>IR制御</title>";
  html += "<style>body{font-size:32px;background-color:#f0f0f0;font-family:'Hiragino Kaku Gothic ProN',Meiryo,sans-serif;}";
  html += ".button{font-size:36px;background-color:#4CAF50;color:white;padding:20px 40px;text-align:center;text-decoration:none;display:inline-block;";
  html += "margin:4px 2px;cursor:pointer;border-radius:12px;}</style></head><body><h1>IR制御</h1><p>ボタンをクリックしてIR信号を送信</p>";
  html += "<button class=\"button\" onclick=\"location.href='/sendIR1'\">電源ON</button><br/>";
  html += "<button class=\"button\" onclick=\"location.href='/sendIR2'\">温度上昇</button><br/>";
  html += "<button class=\"button\" onclick=\"location.href='/sendIR3'\">温度下降</button><br/>";
  html += "<button class=\"button\" onclick=\"location.href='/sendIR4'\">電源OFF</button><br/></body></html>";
  server.send(200, "text/html", html);
}
void handleSendIR1() {
  irsend.sendRaw(rawData1, sizeof(rawData1) / sizeof(rawData1[0]), KHZ);
  delay(1000);
  String msg = "IR信号を送信しました.";
  server.send(200, "text/plain; charset=utf-8", msg);
}

void handleSendIR2() {
  irsend.sendRaw(rawData2, sizeof(rawData2) / sizeof(rawData2[0]), KHZ);
  String msg = "IR信号を送信しました.";
  server.send(200, "text/plain; charset=utf-8", msg);
}

void handleSendIR3() {
  irsend.sendRaw(rawData3, sizeof(rawData3) / sizeof(rawData3[0]), KHZ);
  String msg = "IR信号を送信しました.";
  server.send(200, "text/plain; charset=utf-8", msg);
}

void handleSendIR4() {
  irsend.sendRaw(rawData4, sizeof(rawData4) / sizeof(rawData4[0]), KHZ);
  String msg = "IR信号を送信しました.";
  server.send(200, "text/plain; charset=utf-8", msg);
}

void handleNotFound(void) {
  server.send(404, "text/plain", "ページが見つかりません");
}
