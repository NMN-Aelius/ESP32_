#include <WiFi.h>

// config ssid and password
const char* ssid = "esp32_aelius";
const char* password = "nhutnguyen";

//init port use for server connection
WiFiServer server(80);

//String store http req
String header;

String stateiLED = "OFF";
const int iLED = 2;

//check time for timeout setting (optional)
unsigned long curTime = millis(); // this func is very difference with delay
unsigned long preTime = 0;

const long timeout = 2000;

void setup()
{
  Serial.begin(115200);
  pinMode(iLED, OUTPUT);

  WiFi.softAP(ssid, password);

  Serial.print("IP ADDR: ");
  Serial.print(WiFi.softAPIP());
  server.begin();
}

void loop()
{
 WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    curTime = millis();
    preTime = curTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client

    while (client.connected() && curTime - preTime <= timeout) {
      // loop while the client's connected
      curTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // turns the GPIOs on and off
            if (header.indexOf("GET /internalLED/on") >= 0) {
              stateiLED = "on";
              digitalWrite(iLED, HIGH);               // turns the LED on
            } else if (header.indexOf("GET /internalLED/off") >= 0) {
              stateiLED = "off";
              digitalWrite(iLED, LOW);                //turns the LED off
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            client.println("<style>html { font-family: monospace; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: yellowgreen; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 32px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: gray;}</style></head>");

            client.println("<body><h1>ESP32 Web Server</h1>");
            client.println("<p>Control LED State</p>");

            if (stateiLED == "off") {
              client.println("<p><a href=\"/internalLED/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/internalLED/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            
            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}