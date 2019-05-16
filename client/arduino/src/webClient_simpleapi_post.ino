/*
  Arduino Web client

*/

#include <SPI.h>
#include <WiFiNINA.h>
#include <ArduinoJson.h>

//#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;    // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

int keyIndex = 0;            // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
//IPAddress server(127,0,0,1);  // numeric IP for Google (no DNS)
char server[] = "http://b3a49641.ngrok.io";    // name address for Google (using DNS)

// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

#include <Bounce2.h>

#define BUTTON_PIN 2
#define LED_PIN 13

int ledState = LOW;
int lastButtonState = 0;

Bounce debouncer = Bounce(); // Instantiate a Bounce object

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0") {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");
  printWifiStatus();

  debouncer.attach(BUTTON_PIN,INPUT); // Attach the debouncer to a pin with INPUT mode
  debouncer.interval(25); // Use a debounce interval of 25 milliseconds
  
  pinMode(LED_PIN,OUTPUT); // Setup the LED
  digitalWrite(LED_PIN,ledState);
}

void loop() {
  // if there are incoming bytes available
  // from the server, read them and print them:
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // check if button is pressed
  debouncer.update(); // Update the Bounce instance

  if ( debouncer.rose() ) {  // Call code if button transitions from LOW to HIGH
     Serial.println("Button rose!");
     ledState = !ledState; // Toggle LED state
     digitalWrite(LED_PIN,ledState); // Apply new LED state

     postValues();
   }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  // print the mac address
  byte mac[6];  
  WiFi.macAddress(mac);
  Serial.print("MAC: ");
  Serial.print(mac[5],HEX);
  Serial.print(":");
  Serial.print(mac[4],HEX);
  Serial.print(":");
  Serial.print(mac[3],HEX);
  Serial.print(":");
  Serial.print(mac[2],HEX);
  Serial.print(":");
  Serial.print(mac[1],HEX);
  Serial.print(":");
  Serial.println(mac[0],HEX);
}

void getValues() {
  Serial.println("\n[GET] Starting connection to server...");
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("GET /api/values HTTP/1.1");
    client.println("Host: b3a49641.ngrok.io");
    client.println("Connection: close");
    client.println();
  }
}

void postValues() {
  const size_t capacity = JSON_OBJECT_SIZE(1);
  DynamicJsonDocument doc(capacity);

  doc["value"] = ledState;

  String body;
  serializeJson(doc, body);
  
  Serial.println("\n[{POST] Starting connection to server...");
  Serial.println(body);
  // if you get a connection, report back via serial:
  if (client.connect(server, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:
    client.println("POST /api/values HTTP/1.1");
    client.println("Host: b3a49641.ngrok.io");
    client.println("Content-Type: application/json");
    client.println("Connection: close");
    client.print("Content-Length: ");
    client.println(body.length());
    client.println();
    client.println(body);
    client.println();
  }
}
