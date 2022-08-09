#pragma once
#ifndef Sensors_h
#define Sensors_h

#include "Arduino.h"
#include "debug.h"
#include "config.h"
#include <OneWire.h>

class Sensors {
private:
	Sensors() { };
	Sensors(Sensors const&) {};
	void operator=(Sensors const&) {};

	OneWire* oneWire = nullptr;
	
	float _temperature = 0.0f;
	byte _addrDS18B20[8];
	uint8_t _iterHCSR04 = 0;
	uint32_t _duration = 0, _distance = 0;
	uint8_t _trigPin = 0, _echoPin = 0;

public:
	static Sensors* getInstance() {
		static Sensors sensor;
		return &sensor;
	};

	bool initHCSR04(uint8_t trigPin, uint8_t echoPin);
	bool evalDistance(bool instantEval = true, bool display = false);
	int16_t getDistance(bool eval = false);

	bool initDS18B20(OneWire * a);
	bool connectDS18B20();
	bool evalTemperature(bool display = false);
	float getTemperature(bool eval = false);
};
#endif