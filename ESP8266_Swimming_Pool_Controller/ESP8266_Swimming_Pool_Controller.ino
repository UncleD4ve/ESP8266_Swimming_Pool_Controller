#include <DallasTemperature.h>
#include "WiFiController.h"
#include "WebServerController.h"
#include "StorageController.h"
#include "debug.h"
#include "Sensors.h"

void preinit() {
	ESP8266WiFiClass::preinitWiFiOff();
}

WebServerController WebServerContr;
OneWire DS18B20(DS18B20_PIN);

DallasTemperature sensors(&DS18B20);   
DeviceAddress inThermometer  = { 0x28, 0xFB, 0x6E, 0xB6, 0x94, 0x21, 0x06, 0xD1 };
DeviceAddress midThermometer = { 0x28, 0xA5, 0x60, 0xB9, 0x94, 0x21, 0x06, 0xD4 };
DeviceAddress outThermometer = { 0x28, 0x39, 0x7B, 0xF8, 0x94, 0x21, 0x06, 0x7B };

uint32_t secondTimer(0), servoWebSocket(0), DS18B20Timer(0);
uint16_t seconds(0);
uint8_t position(0), destPosition(0), dieCounter(0);
uint8_t saveA;

bool changeButton(false);

enum wsEvents {
	WS_NULL = 0,
	WS_BUTTON_CLICK = 1,
	WS_COLOR_PICKER = 2
};
uint8_t wsEvent = WS_NULL;

// End of example variables

void setup()
{
	storage::serialBegin();
	storage::initialPrint();

	storage::addVar("saveA", saveA);
	storage::setOnResetFunction([]() {
		saveA = 0;
		storage::saveVar("saveA", saveA);
	});
	storage::getVar("saveA", saveA);

	storage::resetByButton();
	storage::load();
	storage::print();

	sensors.begin();
	DS18B20.reset_search();
	if (!DS18B20.search(inThermometer))  Serial.println("Unable to find address for insideThermometer");
	if (!DS18B20.search(midThermometer)) Serial.println("Unable to find address for MIDsideThermometer");
	if (!DS18B20.search(outThermometer)) Serial.println("Unable to find address for outsideThermometer");

	sensors.setResolution(inThermometer , 12);
	sensors.setResolution(midThermometer, 12);
	sensors.setResolution(outThermometer, 12);



	WebServerContr.addWsInitial(PSTR("initText"), []() {return "initialText"; });
	WebServerContr.addWsInitial(PSTR("initseconds"), []() {return String(seconds); });
	WebServerContr.addWsInitial(PSTR("initPosition"), []() {return String(position); });
	WebServerContr.addWsInitial(PSTR("initSaveA"), []() {return String(saveA); });
	WebServerContr.addWsInitial(PSTR("initChangeButton"), []() {return String(changeButton); });

	WebServerContr.addWsEvent("slider", [](void * arg, uint8_t *data, size_t len) {
		destPosition = atoi((char*)data);
	});

	WebServerContr.addWsEvent("buttonClick", [](void * arg, uint8_t *data, size_t len) {
		wsEvent = WS_BUTTON_CLICK;
	});

	WebServerContr.addWsEvent("buttonChange", [](void * arg, uint8_t *data, size_t len) {
		changeButton = !changeButton;
		debugf(PSTR("Change Button: %d\n"), changeButton);
		WebServerContr.ws.printfAll_P(PSTR("{\"changeButton\":%d}"), changeButton);
	});

	WebServerContr.addWsEvent("buttonSetValue", [](void * arg, uint8_t *data, size_t len) {
		WebServerContr.ws.printfAll_P(PSTR("{\"saveA\":%d}"), position);
	});

	WebServerContr.addWsEvent("buttonRestoreValue", [](void * arg, uint8_t *data, size_t len) {
		storage::getVar("saveA", destPosition);
		debugf(PSTR("Restore Value: %d\n"), destPosition);
	});

	//WebServerContr.WiFiContr.forceWifiRegister();
	if (WebServerContr.WiFiContr.begin())
		WebServerContr.beginSPIFFS().beginServer(true).beginWsServer().beginOTA(10);



	pinMode(BUILTIN_LED, OUTPUT);
	digitalWrite(BUILTIN_LED, HIGH);
}

void loop()
{
	WebServerContr.WebServerLoop();

	if (millis() - servoWebSocket >= 20)
	{
		servoWebSocket = millis();

		if (position != destPosition)
		{
			position = destPosition;
			WebServerContr.ws.printfAll_P(PSTR("{\"position\":%d}"), position);
			yield();
		}

		switch (wsEvent)
		{
		case WS_BUTTON_CLICK:
			wsEvent = WS_NULL;
			blink();
			break;

		default:
			break;
		}

		sensors.requestTemperatures();
	}

	sensors.requestTemperatures();

	if ((millis() - DS18B20Timer) > DS18B20Time-700) {
		//sensor->evalTemperature(true);
		printTemperature(inThermometer);
		printTemperature(midThermometer);
		printTemperature(outThermometer);
		dbln();
		//if (sensor.getTemperature() != measurementData.temperature) {
		//	measurementData.temperature = sensor.getTemperature();
		//}
		DS18B20Timer = millis();
	}

	//if (millis() - secondTimer >= 1e3)
	//{
	//	secondTimer = millis(); yield();
	//	seconds++;
	//}

}

void blink() {
	digitalWrite(BUILTIN_LED, LOW);
	delay(500);
	digitalWrite(BUILTIN_LED, HIGH);
}


void printTemperature(DeviceAddress deviceAddress)
{
	float tempC = sensors.getTempC(deviceAddress);
	if (tempC == DEVICE_DISCONNECTED_C)
	{
		Serial.println("Error: Could not read temperature data");
		return;
	}
	Serial.print("Temp C: ");
	Serial.println(tempC);

}



// Var examples

//int integer = 123456789;
//storage::addVar("intiger", integer);

//char character = 'A';
//storage::addVar("char", character);

//char str[] = "abc";
//storage::addVar("str", str);
//str[0] = '1'; str[1] = '2'; str[2] = '3';

////storage::saveVar("str", str);

//debugf(PSTR("Storage get Int: %d\n"),storage::getVar("intiger", integer));
//debugf(PSTR("Storage get char: %d\n"), storage::getVar("char", character));
//storage::getVar("str", str);
//debugf(PSTR("Storage get str: %s\n"), str);
//debugf(PSTR("Storage get null: %d\n"), storage::getVar("null", integer));