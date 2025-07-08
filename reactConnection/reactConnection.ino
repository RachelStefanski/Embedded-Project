#include <WiFi.h>
#include <WebServer.h>

// ===== הגדרות Wi-Fi =====
const char* ssid = "HUAWEI-9E9A";     // שם רשת ה-Wi-Fi שלך
const char* password = "036616258";    // סיסמת רשת ה-Wi-Fi שלך

// ===== אובייקט שרת ווב =====
WebServer server(80); // השרת יאזין בפורט 80 (HTTP סטנדרטי)

// ===== פונקציית טיפול בבקשות HTTP =====
void handleControl() {
  String cmd = server.arg("cmd"); // קבל את הפרמטר 'cmd' מה-URL
  Serial.print("Received command: ");
  Serial.println(cmd); // הדפס את הפקודה שהתקבלה בסריאל

  server.send(200, "text/plain", "OK"); // שלח תגובה לאפליקציה
}

// ===== Setup =====
void setup() {
  Serial.begin(115200);

  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); // התחבר לרשת ה-Wi-Fi

  // המתן עד לחיבור לרשת
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP()); // הדפס את כתובת ה-IP של ה-ESP32

  // הגדר את הטיפול בבקשות HTTP
  server.on("/control", handleControl); // כאשר מגיעה בקשה לנתיב /control, קרא לפונקציה handleControl

  // התחל את השרת
  server.begin();
  Serial.println("Web server started.");
}

// ===== Loop =====
void loop() {
  server.handleClient(); // טפל בבקשות HTTP נכנסות
}
