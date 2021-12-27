#include <ESP8266WebServer.h>

// Enter WLAN SSID here
const char* ssid = "WIFI Name";
//Enter WLAN Password here
const char* password = "WIFI Password";  

// HTTP Web server listen on port "80"
ESP8266WebServer server(80);

// LED on GPIO D6 (Digital Pin)
uint8_t LED1pin = D6;
bool LED1status = LOW;

// LED on GPIO D7 (Digital Pin)
uint8_t LED2pin = D7;
bool LED2status = LOW;

/// <summary>
/// Writes the HTML page to client.
/// </summary>
/// <param name="led1stat">The led1stat.</param>
/// <param name="led2stat">The led2stat.</param>
/// <returns>String.</returns>
String WriteHtmlPageToClient(uint8_t led1stat, uint8_t led2stat)
{
	String responseText = "<!DOCTYPE html> <html>\n";
	responseText += "<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
	responseText += "<title>GPIO Control</title>\n";
	responseText += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
	responseText += "body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
	responseText += ".button {display: block;width: 220px;background-color: #1abc9c;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
	responseText += ".button-on {background-color: #1664A0;}\n";
	responseText += ".button-on:active {background-color: #2FA91B;}\n";
	responseText += ".button-off {background-color: #34495e;}\n";
	responseText += ".button-off:active {background-color: #A91B1B;}\n";
	responseText += "p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
	responseText += "</style>\n";
	responseText += "</head>\n";
	responseText += "<body>\n";
	responseText += "<h1>NodeMCU 1E ESP8266 HTTP Web Server</h1>\n";

	// LED 1 an GPIO (Digital Pin 6)
	if (led1stat)
		responseText += "<p>LED 1 on pin D6 <br />Status: ON</p><a class=\"button button-off\" href=\"/led1off\">Switch OFF</a>\n";
	else
		responseText += "<p>LED 1 on pin D6<br />Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">Switch ON</a>\n";

	// LED 2 an GPIO (Digital Pin 7)
	if (led2stat)
		responseText += "<p>LED 2on pin D7<br />Status: ON</p><a class=\"button button-off\" href=\"/led2off\">Switch OFF</a>\n";
	else
		responseText += "<p>LED 2on pin D7<br />Status: OFF</p><a class=\"button button-on\" href=\"/led2on\">Switch ON</a>\n";

	responseText += "</body>\n";
	responseText += "</html>\n";
	return responseText;
}

/// <summary>
/// Handles the client connect event.
/// </summary>
void handle_OnConnect()
{
	LED1status = LOW;
	LED2status = LOW;
	Serial.println("GPIO6 Status: OFF | GPIO7 Status: OFF");
	server.send(200, "text/html", WriteHtmlPageToClient(LED1status, LED2status));
}

/// <summary>
/// Handles the LED 1 ON event.
/// </summary>
void handle_led1on()
{
	LED1status = HIGH;
	Serial.println("GPIO6 Status: ON");
	server.send(200, "text/html", WriteHtmlPageToClient(true, LED2status));
}

/// <summary>
/// Handles the LED 1 OFF event.
/// </summary>
void handle_led1off()
{
	LED1status = LOW;
	Serial.println("GPIO6 Status: OFF");
	server.send(200, "text/html", WriteHtmlPageToClient(false, LED2status));
}

/// <summary>
/// Handles the LED 2 ON event.
/// </summary>
void handle_led2on()
{
	LED2status = HIGH;
	Serial.println("GPIO7 Status: ON");
	server.send(200, "text/html", WriteHtmlPageToClient(LED1status, true));
}

/// <summary>
/// Handles the LED 2 OFF event.
/// </summary>
void handle_led2off()
{
	LED2status = LOW;
	Serial.println("GPIO7 Status: OFF");
	server.send(200, "text/html", WriteHtmlPageToClient(LED1status, false));
}

/// <summary>
/// Handles the 404 page not found.
/// </summary>
void handle_NotFound()
{
	server.send(404, "text/plain", "404, you've reached the end of the landfill.<br /><br />When in doubt - Panic!");
}

/// <summary>
/// Setup the controller.
/// </summary>
void setup()
{
	Serial.begin(115200);
	delay(100);
	pinMode(LED1pin, OUTPUT);
	pinMode(LED2pin, OUTPUT);

	digitalWrite(LED1pin, HIGH);
	digitalWrite(LED2pin, HIGH);

	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(ssid);

	// Connect to your local wi-fi network
	WiFi.begin(ssid, password);

	// Check if wi-fi is connected to wi-fi network
	while (WiFi.status() != WL_CONNECTED) {
		delay(1000);
		Serial.print("-");
	}
	Serial.println("");
	Serial.println("WiFi is connected successfully!");
	Serial.print("Server IP: ");  Serial.println(WiFi.localIP());

	// Add for all "recognized" uris and event handler
	// The event handler is called when on of these
	// "pages" are requested by client HTTP request
	// "/" is HTTP site root (like "typing www.lalaland.de" in a web browser)
	server.on("/", handle_OnConnect);
	server.on("/led1on", handle_led1on);
	server.on("/led1off", handle_led1off);
	server.on("/led2on", handle_led2on);
	server.on("/led2off", handle_led2off);

	// Redirect all other unknown uris over the border
	server.onNotFound(handle_NotFound);

	server.begin();
	Serial.println("HTTP server is now started and waiting for requests.");
	Serial.print("You can now connect via typing: ");
	Serial.print(WiFi.localIP());
	Serial.println(" in your web browser.");

	handle_led1off();
	handle_led2off();
}
/// <summary>
/// Do this again until end of days or power off.
/// </summary>
void loop()
{
	// Handle incoming client HTTP requests (e.g. from a web browser)
	// and updates 
	server.handleClient();

	// Apply then the states from client request to hardware

	// Update LED 1
	if (LED1status)
		digitalWrite(LED1pin, HIGH);
	else
		digitalWrite(LED1pin, LOW);

	// Update LED 2
	if (LED2status)
		digitalWrite(LED2pin, HIGH);
	else
		digitalWrite(LED2pin, LOW);
}



