# include <WiFi.h>
# include <WebServer.h>


# define ssid "[YOUR_WIFI_SSID]"
# define password "[YOUR_WIFI_PASSWORD]"

# define redLedPin 27
# define greenLedPin 26
# define blueLedPin 25  // used for PWM brightness

IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);
WebServer server (80);

// tracks which mode is active
String curr_mode = "off";
int brightnessValue = 0;

unsigned long prev_millis = 0;
bool BlinkState = false;


void setup() {
  Serial.begin(115200);
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);

  server.on("/", handle_OnConnect);
  server.on("/Blink", handle_Blink);
  server.on("/Alternate", handle_Alternate);
  server.on("/ledoff", handle_ledOFF);
  server.on("/slider", handle_slider);
  server.onNotFound(handle_NotFound);
  server.begin();
  Serial.println("HTTP Server Started");
}

void loop() {
  server.handleClient();

  unsigned long curr_millis = millis();

  if (curr_mode == "Blink") {
    // turns on blue light every 500 ms to show blinking connection
    if (curr_millis - prev_millis >= 500) {
      prev_millis = curr_millis;
      BlinkState = !BlinkState;
      digitalWrite(redLedPin, BlinkState ? HIGH : LOW); // blinks red LED
      digitalWrite(greenLedPin, BlinkState ? HIGH : LOW); // blinks green LED
    }
  }
  else if (curr_mode == "Alternate") {
    // alternate between red and green every 500ms
    if (curr_millis - prev_millis >= 500) {
      prev_millis = curr_millis;
      BlinkState = !BlinkState;
      digitalWrite(blueLedPin, LOW);
      digitalWrite(greenLedPin, BlinkState ? HIGH : LOW);
      digitalWrite(redLedPin, BlinkState ? LOW : HIGH);
    }
  }

  else if (curr_mode == "off") {
    // turns off all the LED buttons
    digitalWrite(redLedPin, LOW);
    digitalWrite(greenLedPin, LOW);
    digitalWrite(blueLedPin, LOW);
    analogWrite(blueLedPin, 0);
  }
}

// HTML page
String getHTML() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>ESP32 Web Server</title>";
  
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; text-align: center; background: #f4f4f4; padding-top: 50px; }";
  html += "h1 { color: #1a1a2e; font-size: 26px; }";
  html += "p.subtitle { color: #555; font-size: 13px; margin-bottom: 25px; }";
  html += "p.status  { color: #888; font-size: 14px; margin-bottom: 15px; }";
  
  // button styling
  html += ".btn { display: block; width: 200px; margin: 10px auto; padding: 14px;";
  html += "       font-size: 15px; border: none; border-radius: 6px; color: white;";
  html += "       text-decoration: none; cursor: pointer; }";
  html += ".btn-grey  { background-color: #5a6268; }";
  html += ".btn-blue  { background-color: #1e81b0; }";
  html += ".btn:active { opacity: 0.75; }";
  html += "input[type=range] { width: 200px; margin-top: 10px; }";
  html += "label { font-size: 14px; color: #555; }";
  html += "</style></head><body>";

  html += "<h1>ESP32 Web Server</h1>";
  html += "<p class='subtitle'>Control LEDs using Access Point (AP) Mode</p>";

  // show which mode is active
  html += "<p class='status'>LED Status: " + curr_mode + "</p>";

  // highlight active button in blue, gray out the others
  html += "<a href='/Blink'     class='btn " + String(curr_mode == "Blink"     ? "btn-blue" : "btn-grey") + "'>Blink LED</a>";
  html += "<a href='/Alternate' class='btn " + String(curr_mode == "Alternate" ? "btn-blue" : "btn-grey") + "'>Alternate LED</a>";
  html += "<a href='/ledoff'       class='btn " + String(curr_mode == "off"       ? "btn-blue" : "btn-grey") + "'>Turn OFF</a>";

  // brightness slider (manipulates brghtness of blue LED light)
  html += "<br><label id='brightnessLabel'> Brightness (Blue LED): " + String(brightnessValue) + "</label><br>";
  html += "<input type='range' min='0' max='255' value='" + String(brightnessValue) + "'";
  html += " oninput='document.getElementById(\"brightnessLabel\").innerText = \"Brightness (Blue LED): \" + this.value; fetch(\"/slider?val=\" + this.value)'>";

  html += "</body></html>";
  return html;
}

void handle_OnConnect() {
  server.send(200, "text/html", getHTML());
}

void handle_Blink() {
  curr_mode = "Blink";
  prev_millis = millis();
  BlinkState = false;
  server.send(200, "text/html", getHTML());
}

void handle_Alternate() {
  curr_mode = "Alternate";
  prev_millis = millis();
  BlinkState = false;
  server.send(200, "text/html", getHTML());
}

void handle_ledOFF() {
  curr_mode = "off";
  brightnessValue = 0;
  analogWrite(blueLedPin, 0);
  server.send(200, "text/html", getHTML());
}

void handle_slider() {
  if (server.hasArg("val")) {
    brightnessValue = server.arg("val").toInt();
    analogWrite(blueLedPin, brightnessValue);
    Serial.print("Brightness set to: ");
    Serial.print(brightnessValue);
  }
  server.send(200, "text/plain", "OK");
}

void handle_NotFound() {
  server.send(404, "text/plain", "Not Found");
}