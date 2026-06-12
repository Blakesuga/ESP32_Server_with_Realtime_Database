# include <WiFi.h>
# include <Firebase_ESP_Client.h>
# include <ESP32Servo.h>
# include "addons/TokenHelper.h"
# include "addons/RTDBHelper.h"

# define WIFI_SSID "[YOUR_WIFI_SSID]"
# define WIFI_PASSWORD "[YOUR_WIFI_PASSWORD]"
# define API_KEY "[YOUR_API_KEY]" 
# define DATABASE_URL "[YOUR_DATABASE_URL]"

# define LED1_PIN 12
# define LED2_PIN 14
# define LEDR_PIN 36
# define BTN_PIN 22
# define SERVO_PIN 25

const int freq = 5000;
const int resolution = 8;

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConf+ig config;
Servo myServo;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;
int ldrData = 0;
float voltage = 0.0;
int pwmValue = 0;
bool ledStatus = false;
bool btn_state = false;
int servoAngle = 0;


void setup() {
 pinMode(LED2_PIN, OUTPUT);
 pinMode(BTN_PIN, INPUT_PULLUP);
 ledcAttach(LED1_PIN, freq, resolution);

 Serial.begin(115200);
 WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 Serial.print("Connecting to Wi-Fi");
 
 while (WiFi.status() != WL_CONNECTED) {
  Serial.print(" . "); delay(300);
 }

 Serial.println();
 Serial.print("Connected with IP: ");
 Serial.println(WiFi.localIP());
 Serial.println();

 config.api_key = API_KEY;
 config.database_url = DATABASE_URL;
 if (Firebase.signUp (&config, &auth, "", "")) {
  Serial.println("signUp OK");
  signupOK = true;
 }

 else {
  Serial.printf("@s\n", config.signer.signupError.message.c_str());
 }

 config.token_status_callback = tokenStatusCallback;
 Firebase.begin(&config, &auth);
 Firebase.reconnectWiFi(true);
}

void loop() {
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 5000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

    // ----------------------------store sensor data to Firebase
    ldrData = analogRead(LEDR_PIN);
    voltage = (float)analogReadMilliVolts(LEDR_PIN)/1000;
  
    if (Firebase.RTDB.setInt(&fbdo, "Sensor/ldr_data", ldrData)) {
      Serial.println(); Serial.print(ldrData);
      Serial.print(" - successfully saved to: " + fbdo.dataPath());
      Serial.println(" (" + fbdo.dataType() + ")");
    }
    else {
      Serial.println("FAILED: " + fbdo.errorReason());
    }

    if (Firebase.RTDB.setFloat(&fbdo, "Sensor/voltage", voltage)) {
      Serial.print(voltage);
      Serial.print(" - successfully saved to: " + fbdo.dataPath());
      Serial.println(" (" + fbdo.dataType() + ")");
    }
    else {
      Serial.println("FAILED: " + fbdo.errorReason());
    }
  }

    // ----------------------------read data from button and saves to Firebase
    if (Firebase.ready() && signupOK) { 
      btn_state = !digitalRead(BTN_PIN);
      
      if (Firebase.RTDB.setBool(&fbdo, "Sensor/switch", btn_state)) {
        Serial.print(btn_state);
        Serial.print(" - successfully saved to: " + fbdo.dataPath());
        Serial.println(" (" + fbdo.dataType() + ")");
      }
      else {
        Serial.println("FAILED: " + fbdo.errorReason());
      }
    }

  // ----------------------------read data from Firebase to control devices attached to ESP32
  if (Firebase.RTDB.getInt(&fbdo, "LED/analog")) {
    if (fbdo.dataType() == "int") {
      pwmValue = fbdo.intData();
      Serial.println("Successful read from " + fbdo.dataPath() + ": " + pwmValue + " (" + fbdo.dataType() + ")");
      ledcWrite(LED1_PIN, pwmValue);
    }
  }
    else {
      Serial.println("FAILED: " + fbdo.errorReason());
    }

  if (Firebase.RTDB.getBool(&fbdo, "LED/digital")) {
    if (fbdo.dataType() == "boolean") {
      ledStatus = fbdo.boolData();
      Serial.println("Successful read from " + fbdo.dataPath() + ": " + ledStatus + " (" + fbdo.dataType() + ")");
      digitalWrite(LED2_PIN, ledStatus);
    }
  }
    else {
      Serial.println("FAILED: " + fbdo.errorReason());
    }

    // ----------------------------read data from servo angle (0-100)
  if (Firebase.RTDB.getInt(&fbdo, "Servo/angle")) {
    if (fbdo.dataType() == "int" || fbdo.dataType() == "float") {
      servoAngle = fbdo.intData();
      servoAngle = constrain(servoAngle, 0, 180);
      Serial.println("Successful read from " + fbdo.dataPath() + ": " + servoAngle + " (" + fbdo.dataType() + ")");
      myServo.write(servoAngle);
    }
  }
    else {
      Serial.println("FAILED: " + fbdo.errorReason());
    }

}
