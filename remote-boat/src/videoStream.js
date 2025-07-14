// VideoStream.js 
import React, { useRef, useImperativeHandle, forwardRef } from 'react';

const VideoStream = forwardRef((props, ref) => {
    const streamUrl = "http://192.168.22.39";
 
    const imgRef = useRef(null); // Ref עבור תג ה-<img>

    // חושפים פונקציות מסוימות דרך ה-ref שהאב מעביר
    useImperativeHandle(ref, () => ({
        // הפונקציה הזו תהיה זמינה לרכיב האב דרך ה-ref
        captureFrame: () => {
            const imgElement = imgRef.current;

            // ודא שהאלמנט קיים ושהתמונה נטענה (naturalWidth > 0)
            if (!imgElement || !imgElement.naturalWidth || imgElement.naturalWidth === 0) {
                console.error("שגיאה: זרם הוידאו אינו זמין או לא נטען כראוי.");
                return Promise.reject(new Error("Image stream not ready for capture."));
            }

            const canvas = document.createElement("canvas");
            canvas.width = imgElement.naturalWidth;
            canvas.height = imgElement.naturalHeight;
            const ctx = canvas.getContext("2d");
            ctx.drawImage(imgElement, 0, 0, canvas.width, canvas.height);

            return new Promise((resolve, reject) => {
                canvas.toBlob((blob) => {
                    if (blob) {
                        resolve(blob); // מחזיר את ה-Blob של התמונה
                    } else {
                        reject(new Error("נכשלה לכידת תמונה מהזרם (Blob is null)."));
                    }
                }, "image/jpeg"); // פורמט התמונה
            });
        }
    }));

    return (
        <div className="flex justify-center items-center p-4 bg-gray-800 rounded-lg shadow-lg">
            <img
                ref={imgRef} // קושרים את ה-ref לאלמנט ה-<img>
                src={streamUrl}
                alt="Live Stream from ESP32-CAM"
                crossOrigin="anonymous"
                className="rounded-md object-cover w-full max-w-md h-auto border-2 border-indigo-500 shadow-xl"
                height={500}
                width={700}
                onError={(e) => {
                    e.target.onerror = null;
                    e.target.src = "https://placehold.co/640x360/FF0000/FFFFFF?text=Stream+Error";
                    console.error("Failed to load video stream from ESP32-CAM. Please check the IP address and ESP32 server status.");
                }}
            />
        </div>
    );
});

export default VideoStream;