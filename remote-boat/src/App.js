// src/App.js
import React from 'react';
import 'bootstrap/dist/css/bootstrap.min.css';

function App() {
  // === חובה לעדכן את כתובת ה-IP של ה-ESP32 כאן! ===
  // כתובת ה-IP הזו היא הכתובת שה-ESP32 מקבל כשהוא מתחבר לרשת ה-Wi-Fi שלך.
  // תוכלי למצוא אותה ב-Serial Monitor של ה-Arduino IDE לאחר שה-ESP32 מתחבר לרשת.
const esp32IpAddress = "http://192.168.8.150"; // הוסף את "http://"
  const sendCommand = (command) => {
    // בונה את ה-URL לשליחת הפקודה ל-ESP32
    // לדוגמה: http://192.168.1.100/control?cmd=forward
    fetch(`${esp32IpAddress}/control?cmd=${command}`)
      .then(res => {
        if (!res.ok) {
          throw new Error(`HTTP error! status: ${res.status}`);
        }
        console.log("Command sent:", command);
        return res;
      })
      .catch(err => console.error("Error sending command:", err));
  };

  return (
    <div className="container text-center mt-5">
      <h1 className="mb-4">שליטה מרחוק על הסירה 🚤</h1>

      {/* שידור חי */}
      <div className="mb-4">
        <h4>שידור מהמצלמה 🎥</h4>
        {/*
          שימי לב: אם את משתמשת ב-ESP32-CAM, הנתיב '/stream' הוא הנתיב הסטנדרטי לשידור וידאו.
          אם הזרם לא עובד, ודאי שה-ESP32-CAM שלך אכן משדר וידאו לכתובת הזו.
          ייתכן שתצטרכי לציין את כתובת ה-IP המלאה של ה-ESP32-CAM גם כאן,
          לדוגמה: src={`${esp32IpAddress}/stream`}
        */}
        <img
          src="/stream"
          alt="Live Stream"
          style={{ width: '100%', maxWidth: '600px', borderRadius: '12px', border: '2px solid #007bff' }}
        />
      </div>

      {/* כפתורי שליטה */}
      <div className="mb-5">
        <h4>כיוון הסירה 🎮</h4>
        <div className="d-flex flex-column align-items-center">
          <button className="btn btn-primary m-2" onClick={() => sendCommand('forward')}>⬆ קדימה</button>
          <div>
            <button className="btn btn-primary m-2" onClick={() => sendCommand('left')}>⬅ שמאלה</button>
            <button className="btn btn-danger m-2" onClick={() => sendCommand('stop')}>⏹ עצור</button>
            <button className="btn btn-primary m-2" onClick={() => sendCommand('right')}>➡ ימינה</button>
          </div>
          <button className="btn btn-primary m-2" onClick={() => sendCommand('backward')}>⬇ אחורה</button>
        </div>
      </div>
    </div>
  );
}

export default App;