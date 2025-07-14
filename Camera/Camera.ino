#include "esp_camera.h"       // ספריית מצלמה עבור ESP32, כולל חיבור למצלמה והפעלתה
#include <WiFi.h>              // ספריית Wi-Fi לחיבור לרשת Wi-Fi
#include "esp_timer.h"         // ספריית טיימר עבור ESP32, מאפשרת פעולות מבוססות זמן
#include "img_converters.h"    // ספריית המרת תמונות לפורמטים שונים, במיוחד JPEG
#include "Arduino.h"           // ספריית הבסיס של ארדואינו
#include "fb_gfx.h"            // ספריית גרפיקה למערכת תצוגה, מאפשרת ציור על מסך חיצוני
#include "soc/soc.h"           // פונקציות לפיקוח על בעיות מערכת (כגון brownout)
#include "soc/rtc_cntl_reg.h"  // פונקציות לפיקוח על בעיות במתח (brownout)
#include "esp_http_server.h"   // ספריית HTTP עבור יצירת שרת HTTP ב-ESP32

// const char* ssid = "HUAWEI-9E9A";       // שם הרשת
// const char* password = "036616258";     // סיסמת הרשת

const char* ssid = "Kita-2";
const char* password = "Xnhbrrfxho";

#define PART_BOUNDARY "123456789000000000000987654321" // גבול לשידור ה-MJPEG

// מודלים נתמכים למצלמה
#define CAMERA_MODEL_AI_THINKER  // הגדרת המודל של מצלמת AI Thinker
//#define CAMERA_MODEL_M5STACK_PSRAM // מיכוון למודל M5STACK עם PSRAM
//#define CAMERA_MODEL_M5STACK_WITHOUT_PSRAM // מיכוון למודל M5STACK ללא PSRAM
//#define CAMERA_MODEL_WROVER_KIT // מודל לא נבדק

// הגדרת פינים למודלים השונים של המצלמות:
#if defined(CAMERA_MODEL_WROVER_KIT) // הגדרת פינים למודל WROVER KIT
  #define PWDN_GPIO_NUM    -1
  #define RESET_GPIO_NUM   -1
  #define XCLK_GPIO_NUM    21
  #define SIOD_GPIO_NUM    26
  #define SIOC_GPIO_NUM    27
  #define Y9_GPIO_NUM      35
  #define Y8_GPIO_NUM      34
  #define Y7_GPIO_NUM      39
  #define Y6_GPIO_NUM      36
  #define Y5_GPIO_NUM      19
  #define Y4_GPIO_NUM      18
  #define Y3_GPIO_NUM       5
  #define Y2_GPIO_NUM       4
  #define VSYNC_GPIO_NUM   25
  #define HREF_GPIO_NUM    23
  #define PCLK_GPIO_NUM    22

#elif defined(CAMERA_MODEL_M5STACK_PSRAM)
  #define PWDN_GPIO_NUM     -1
  #define RESET_GPIO_NUM    15
  #define XCLK_GPIO_NUM     27
  #define SIOD_GPIO_NUM     25
  #define SIOC_GPIO_NUM     23
  #define Y9_GPIO_NUM       19
  #define Y8_GPIO_NUM       36
  #define Y7_GPIO_NUM       18
  #define Y6_GPIO_NUM       39
  #define Y5_GPIO_NUM        5
  #define Y4_GPIO_NUM       34
  #define Y3_GPIO_NUM       35
  #define Y2_GPIO_NUM       32
  #define VSYNC_GPIO_NUM    22
  #define HREF_GPIO_NUM     26
  #define PCLK_GPIO_NUM     21

#elif defined(CAMERA_MODEL_M5STACK_WITHOUT_PSRAM)
  #define PWDN_GPIO_NUM     -1
  #define RESET_GPIO_NUM    15
  #define XCLK_GPIO_NUM     27
  #define SIOD_GPIO_NUM     25
  #define SIOC_GPIO_NUM     23
  #define Y9_GPIO_NUM       19
  #define Y8_GPIO_NUM       36
  #define Y7_GPIO_NUM       18
  #define Y6_GPIO_NUM       39
  #define Y5_GPIO_NUM        5
  #define Y4_GPIO_NUM       34
  #define Y3_GPIO_NUM       35
  #define Y2_GPIO_NUM       17
  #define VSYNC_GPIO_NUM    22
  #define HREF_GPIO_NUM     26
  #define PCLK_GPIO_NUM     21

#elif defined(CAMERA_MODEL_AI_THINKER)
  #define PWDN_GPIO_NUM     32
  #define RESET_GPIO_NUM    -1
  #define XCLK_GPIO_NUM      0
  #define SIOD_GPIO_NUM     26
  #define SIOC_GPIO_NUM     27
  #define Y9_GPIO_NUM       35
  #define Y8_GPIO_NUM       34
  #define Y7_GPIO_NUM       39
  #define Y6_GPIO_NUM       36
  #define Y5_GPIO_NUM       21
  #define Y4_GPIO_NUM       19
  #define Y3_GPIO_NUM       18
  #define Y2_GPIO_NUM        5
  #define VSYNC_GPIO_NUM    25
  #define HREF_GPIO_NUM     23
  #define PCLK_GPIO_NUM     22
#else
  #error "Camera model not selected" // אם לא נבחר מודל מצלמה
#endif

// הגדרת סוג התוכן שהשרת ישדר
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";  // גבול לתמונות
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";  // מבנה כל חלק בתמונה

httpd_handle_t stream_httpd = NULL;  // משתנה לאחסון השרת HTTP

// פונקציה המובילה את הזרמת התמונות לממשק
static esp_err_t stream_handler(httpd_req_t *req){
  camera_fb_t * fb = NULL;        // משתנה לאחסון תמונה ממצלמה
  esp_err_t res = ESP_OK;         // משתנה לתוצאות הפונקציות
  size_t _jpg_buf_len = 0;        // אורך הבופר לתמונה JPEG
  uint8_t * _jpg_buf = NULL;      // בופר התמונה JPEG
  char * part_buf[64];            // בופר לשליחת החלקים

  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);  // הגדרת סוג התשובה
  if(res != ESP_OK){
    return res; // אם קרתה שגיאה, חזור
  }

  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*"); // מאפשר גישה מכל מקור

  // לולאת שידור שמביאה תמונה מהמצלמה וממירה אותה ל-JPEG
  while(true){
    fb = esp_camera_fb_get();  // קבלת תמונה מהמצלמה
    if (!fb) {
      Serial.println("Camera capture failed"); // במקרה של כשלון בתפיסת תמונה
      res = ESP_FAIL;
    } else {
      if(fb->width > 400){ // אם התמונה רחבה יותר מ-400 פיקסלים
        if(fb->format != PIXFORMAT_JPEG){
          bool jpeg_converted = frame2jpg(fb, 80, &_jpg_buf, &_jpg_buf_len);  // המרת התמונה ל-JPEG
          esp_camera_fb_return(fb);  // החזרת הבופר של התמונה
          fb = NULL;
          if(!jpeg_converted){
            Serial.println("JPEG compression failed"); // כשלון בהמרה ל-JPEG
            res = ESP_FAIL;
          }
        } else {
          _jpg_buf_len = fb->len;
          _jpg_buf = fb->buf;
        }
      }
    }

    if(res == ESP_OK){
      size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);  // יצירת כותרת ה-MJPEG
      res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);  // שליחת החלק הראשון
    }
    if(res == ESP_OK){
      res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);  // שליחת התמונה עצמה
    }
    if(res == ESP_OK){
      res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));  // שליחת גבול התמונה
    }

    if(fb){  // אם קיימת תמונה
      esp_camera_fb_return(fb);  // החזרת הבופר של התמונה
      fb = NULL;
      _jpg_buf = NULL;
    } else if(_jpg_buf){ // אם יש בופר JPEG
      free(_jpg_buf);  // שחרור זיכרון
      _jpg_buf = NULL;
    }

    if(res != ESP_OK){  // אם קרתה שגיאה בשידור
      break;
    }
  }
  return res;
}

// הפונקציה מתחילה את השרת HTTP
void startCameraServer(){
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();  // הגדרת ברירת מחדל לשרת
  config.server_port = 80;  // יציאה 80 עבור HTTP

  httpd_uri_t index_uri = {  // הגדרת URI
    .uri       = "/",
    .method    = HTTP_GET,
    .handler   = stream_handler,
    .user_ctx  = NULL
  };

  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &index_uri);  // רישום URI לשידור תמונה
  }
}

// הפונקציה setup מתחילה את המצלמה ומחברת את המכשיר לרשת Wi-Fi
void setup() {
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); // השבתת התרעת brownout

  Serial.begin(115200);  // אתחול קו סריאלי
  Serial.setDebugOutput(false); // השבתת פלט דיבוג

  camera_config_t config;  // הגדרת המצלמה
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if(psramFound()){  // אם נמצא PSRAM
    config.frame_size = FRAMESIZE_UXGA;  // הגדרת גודל תמונה
    config.jpeg_quality = 10;
    config.fb_count = 2;  // מספר בופרים לתמונה
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // אתחול המצלמה
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);  // אם קרתה שגיאה
    return;
  }

  // חיבור לרשת Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // הודעה על חיבור מצלמה והגדרת כתובת IP
  Serial.print("Camera Stream Ready! Go to: http://");
  Serial.print(WiFi.localIP());

  // התחלת שרת הזרמת תמונות
  startCameraServer();
}

void loop() {
  delay(1);  // השהיה
}
