/*
 Name:		LED_Driver.ino
 Created:	6/5/2022 3:16:35 PM
 Author:	Administrator
*/
#include <stdlib.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <string.h>
#include <ESP8266WiFiMulti.h> 


#include <ESP8266WebServer.h>   // Include the WebServer library

ESP8266WiFiMulti wifiMulti;     // Create an instance of the ESP8266WiFiMulti class, called 'wifiMulti'
unsigned long milliscopy = 0;
const int MinutesToFullBrightness = 15;
float AdjustmentStep[3]{ 0 };
int iterations[3]{ 0 };
int redbrightness = 0; 
int greenbrightness = 0;
int bluebrightness = 0;
int brightness = 100;
int ledtiming = 4096;
bool staticled = true;
void (*ExectuteFunction)() = 0;
ESP8266WebServer server(81);    // Create a webserver object that listens for HTTP request on port 80'

// the setup function runs once when you press reset or power the board
void setup() {
	Serial.begin(115200);
	//certain pins have default states at boot. Other pins must down be pulled down or pulled high during boot or 
	//it will cause a failure to boot. D3 and D4 are problem pins.
	pinMode(5, OUTPUT); //green
	pinMode(14, OUTPUT); //D4 was causing a failure to boot. Use D5 to correct. //blue
	pinMode(4, OUTPUT); //red
	wifiMulti.addAP("2gmaster", "31415926535");
	wifiMulti.run();
	server.on("/", handleRoot);
	server.on("/alarmwakeup", alarm_wakeup_page);
	server.on("/testresult", HTTP_POST, testresult);
	server.begin();
}

void handleRoot() {                         // When URI / is requested, send a web page with a button to toggle the LED
	//All of this is used to convert the integers into strings
	//microsoft documentation for the itoa function at: https://docs.microsoft.com/en-us/cpp/c-runtime-library/reference/itoa-itow?view=msvc-170
	char redbrightnessraw[30]{ 0 };
	char greenbrightnessraw[30]{ 0 };
	char bluebrightnessraw[30]{ 0 };
	char brightnessraw[30]{ 0 };
	itoa(redbrightness, redbrightnessraw, 10);
	itoa(greenbrightness, greenbrightnessraw, 10);
	itoa(bluebrightness, bluebrightnessraw, 10);
	itoa(brightness, brightnessraw, 10);
	String redbrightnessconverted;
	String greenbrightnessconverted;
	String bluebrightnessconverted;
	String brightnessconverted;

		for (size_t i = 0; i < 30; i++)
		{
			
			if ((byte)redbrightnessraw[i] >= 48 && (byte)redbrightnessraw[i] <= 57) {
				redbrightnessconverted += redbrightnessraw[i];
				Serial.println((byte)redbrightnessraw[i]);
			}
		}
		for (size_t i = 0; i < 30; i++)
		{
			if ((byte)greenbrightnessraw[i] >= 48 && (byte)greenbrightnessraw[i] <= 57) {
				greenbrightnessconverted += greenbrightnessraw[i];
			}
		}
		for (size_t i = 0; i < 30; i++)
		{
			if ((byte)bluebrightnessraw[i] >= 48 && (byte)bluebrightnessraw[i] <= 57) {
				bluebrightnessconverted += bluebrightnessraw[i];
			}
		}
		for (size_t i = 0; i < 30; i++)
		{
			if ((byte)brightnessraw[i] >= 48 && (byte)brightnessraw[i] <= 57) {
				brightnessconverted += brightnessraw[i];
			}
		}

	server.send(200, "text/html", "<html>\
<head>\
<title>Adjust RGB Color</title>\
\
</head>\
<body>\
<form action=\"/testresult\" enctype=\"application/x-www-form-urlencoded\" method=\"post\">\
<br>\
<div style=\"width:100%\">\
<div style=\"width:25%; float:left\">\
<div style=\"clear:right; clear:left; margin:0px\">\
<div style=\"float:left\"> <p style=\"margin-top:0px\" class=\"selectorlabel\">Red:</p> </div> <div style=\"float:right\"> <input onchange=\"update()\" style=\"margin-top:0px\" class=\"selector\" type=\"range\" min=\"0\" max=\"255\" name=\"Red\" value=\"" + redbrightnessconverted +  "\">\
</div></div>\
<div style=\"clear:right; clear:left; margin:0px\">\
<div style=\"float:left\"> <p style=\"margin-top:0px\" class=\"selectorlabel\">Green:</p> </div> <div style=\"float:right\"> <input onchange=\"update()\" style=\"margin-top:0px\" class=\"selector\" type=\"range\" min=\"0\" max=\"255\" name=\"Green\" value=\"" + greenbrightnessconverted + "\">\
</div></div>\
<div style=\"clear:right; clear:left; margin:0px\">\
<div style=\"float:left\"> <p style=\"margin-top:0px\" class=\"selectorlabel\">Blue:</p> </div> <div style=\"float:right\"> <input onchange=\"update()\" style=\"margin-top:0px\" class=\"selector\" type=\"range\" min=\"0\" max=\"255\" name=\"Blue\" value=\"" + bluebrightnessconverted + "\">\
</div></div>\
<div style=\"clear:right; clear:left; margin:0px\">\
<div style=\"float:left\"> <p style=\"margin-top:0px\" class=\"selectorlabel\">Brightness:</p> </div> <div style=\"float:right\"> <input onchange=\"update()\" style=\"margin-top:0px\" class=\"selector\" type=\"range\" min=\"0\" max=\"100\" step=\"1\" name=\"Brightness\" value=\"" + brightnessconverted + "\">\
</div></div>\
</div>\
\
<div style=\"height: 100px; width:10%; margin-left:5%; float:left; background-color: rgba(\"" + redbrightnessconverted + ', ' + greenbrightnessconverted + ', ' + bluebrightnessconverted + ', ' + brightnessconverted  + "\"); id=\"ColorPanel\">\
\
</div>\
</div>\
<br style=\"clear:right; clear:left;\">\
<br style=\"clear:right; clear:left;\">\
<input style=\"clear:right; clear:left;\"  type=\"submit\" value=\"submit\">\
</form>\
<script>\
document.getElementsByClassName(\"selector\")[0].value;\
function update(){\
document.getElementById(\"ColorPanel\").style.backgroundColor = \"rgba(\" + document.getElementsByClassName(\"selector\")[0].value + \", \" + document.getElementsByClassName(\"Selector\")[1].value + \", \" + document.getElementsByClassName(\"Selector\")[2].value + \", \" + document.getElementsByClassName(\"Selector\")[3].value / 100 + \")\";\
}\
</script>\
</body>\
</html>  \
");
}

float stringTointeger(String str)
{
	float temp = 0;

		for (int i = 0; i < str.length(); i++) {

			// Since ASCII value of character from '0'
			// to '9' are contiguous. So if we subtract
			// '0' from ASCII value of a digit, we get 
			// the integer value of the digit.
			temp = temp * 10 + (str[i] - '0');
		}

	return temp;
}

void testresult() {
	Serial.println("Printing POST Data");
	String message = "";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += (server.method() == HTTP_GET) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";
	for (uint8_t i = 0; i < server.args(); i++) {
		message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
		if (server.argName(i) == "Red") {
			redbrightness = stringTointeger(server.arg(i));
		}
		Serial.println(stringTointeger(server.arg(i)));
		if (server.argName(i) == "Green") {  
			greenbrightness = stringTointeger(server.arg(i));
		}
		if (server.argName(i) == "Blue") {
			bluebrightness = stringTointeger(server.arg(i));
		}
		if (server.argName(i) == "Delay") {
			ledtiming = stringTointeger(server.arg(i));
		}
		if (server.argName(i) == "Brightness") {
			brightness = stringTointeger(server.arg(i));
		}
		if (server.argName(i) == "staticled") {
			if (server.arg(i) == "true") {
				Serial.println("staticled");
				ExectuteFunction = &staticpower;
			}
			else {
				staticled = false;
			}
		}
	}

	ExectuteFunction = &staticpower;
	Serial.println(message);
	server.sendHeader("Location", "/");        // Add a header to respond with a new location for the browser to go to the home page again
	server.send(303);
}

void CalculateAdjustmentStep() { //Formula used: AdjustmentStep = (255 - InitialBrightness) / (MinutesToFullBrightness * 60)
	AdjustmentStep[0] = (float(255 - redbrightness)) / (float(MinutesToFullBrightness * 60)); //red
	AdjustmentStep[1] = (float(0 + greenbrightness)) / (float(MinutesToFullBrightness * 60)); //green
	AdjustmentStep[2] = (float(0 + bluebrightness)) / (float(MinutesToFullBrightness * 60)); //blue

}

void alarm_wakeup_page() {
	ExectuteFunction = &alarm_wakeup;
	CalculateAdjustmentStep();
	brightness = 100;
	milliscopy = 0; //dont know why but for some reason I have to reset the value here otherwise it just overwrites it with some huge number
	iterations[0] = 0;
	iterations[1] = 0;
	iterations[2] = 0;

	server.sendHeader("Location", "/");        // Add a header to respond with a new location for the browser to go to the home page again
	server.send(303);
}

void alarm_wakeup() { 
	if ((redbrightness < 255 || greenbrightness > 0 || bluebrightness > 0) && (millis() > milliscopy + 1000)) {
		milliscopy = millis();
		iterations[0] += 1;
		iterations[1] += 1;
		iterations[2] += 1;

		if ((float)redbrightness + AdjustmentStep[0] * (float)iterations[0] < (float)255 && AdjustmentStep[0] * (float)iterations[0] >= 1) {
			redbrightness += AdjustmentStep[0] * (float)iterations[0];
			iterations[0] = 0;
		}
		if ((float)greenbrightness - AdjustmentStep[1] * (float)iterations[1] > (float)0 && AdjustmentStep[0] * (float)iterations[1] >= 1) {
			greenbrightness -= AdjustmentStep[1] * (float)iterations[1];
			iterations[1] = 0;
		}
		if ((float)bluebrightness - AdjustmentStep[2] * (float)iterations[2] > (float)0 && AdjustmentStep[0] * (float)iterations[2] >= 1) {
			bluebrightness -= AdjustmentStep[2] * (float)iterations[2];
			iterations[2] = 0;
		}

		analogWrite(5, greenbrightness * ((float)brightness / (float)100));
		analogWrite(14, bluebrightness * ((float)brightness / (float)100));
		analogWrite(4, redbrightness * ((float)brightness / (float)100));
	} else if (redbrightness + AdjustmentStep[0] * iterations[0] > 255 && greenbrightness - AdjustmentStep[1] * iterations[1] < 0 && bluebrightness - AdjustmentStep[2] * iterations[2] < 0) {
		ExectuteFunction = 0;
	}
}

// the loop function runs over and over again until power down or reset
void loop() {
	server.handleClient();
	
	if (ExectuteFunction != 0) {
		ExectuteFunction();
	}
	
	/*if (staticled) {
		staticpower(redbrightness * (float)brightness / (float)100, greenbrightness * (float)brightness / (float)100, bluebrightness * (float)brightness / (float)100);
	}
	else
	{
		powerswitch(redbrightness, greenbrightness, bluebrightness, ledtiming);
	}*/
	
}

void staticpower() {
	ExectuteFunction = 0;

	analogWrite(5, greenbrightness * ((float)brightness / (float)100));
	analogWrite(14, bluebrightness * ((float)brightness / (float)100));
	analogWrite(4, redbrightness * ((float)brightness / (float)100));
}

void powerswitch(int red, int green, int blue, int timedelay) {
	for (size_t i = 0; i < green; i++)
	{
		analogWrite(5, i);
		delayMicroseconds(timedelay);
	}
	for (size_t i = green; i > 0; --i)
	{
		analogWrite(5, i);
		delayMicroseconds(timedelay);
	}
	for (size_t i = 0; i < red; i++)
	{
		analogWrite(4, i);
		delayMicroseconds(timedelay);
	}
	for (size_t i = red; i > 0; --i)
	{
		analogWrite(4, i);
		delayMicroseconds(timedelay);
	}
	for (size_t i = 0; i < blue; i++)
	{
		analogWrite(2, i);
		delayMicroseconds(timedelay);
	}
	for (size_t i = blue; i > 0; --i)
	{
		analogWrite(2, i);
		delayMicroseconds(timedelay);
	}
}