#include "Sensors.h"

bool Sensors::initHCSR04(uint8_t trigPin, uint8_t echoPin)
{
	pinMode(_trigPin = trigPin, OUTPUT);
	pinMode(_echoPin = echoPin, INPUT);
}

bool Sensors::evalDistance(bool instantEval, bool display)
{
	while (instantEval)
	{
		digitalWrite(_trigPin, LOW);
		delayMicroseconds(2);
		digitalWrite(_trigPin, HIGH);
		delayMicroseconds(10);
		digitalWrite(_trigPin, LOW);
		_duration += pulseIn(_echoPin, HIGH) / 54;
		delay(10);
		_iterHCSR04++;

		if (_iterHCSR04 > 9)
		{
			_distance = _duration / _iterHCSR04;
			_iterHCSR04 = _duration = 0;
			if (display) {
				debugf(PSTR("Distance: %d\n"), _distance);
			}
			return true;
		}
	}
	return false;
}

int16_t Sensors::getDistance(bool eval)
{
	if(eval)evalDistance();
	return _distance;
}

bool Sensors::initDS18B20(OneWire*oneWire)
{
	this->oneWire = oneWire;
	return connectDS18B20();
}

bool Sensors::connectDS18B20()
{
	if (!oneWire->search(_addrDS18B20)) {
		debuglnF("DS18x20 not found!");
		oneWire->reset_search();
		delay(100);
		return false;
	}
	return true;
}

bool Sensors::evalTemperature(bool display) {
	uint8_t type_s, data[12];

	if (OneWire::crc8(_addrDS18B20, 7) != _addrDS18B20[7])
		if (connectDS18B20()) {
			debuglnF("DS18x20 CRC is not valid!");
			return false;
		}

	switch (_addrDS18B20[0]) {
	case 0x10://DS18S20
		type_s = 1;
		break;
	case 0x28://DS18B20
		type_s = 0;
		break;
	case 0x22://DS1822
		type_s = 0;
		break;
	default:
		debuglnF("Wrong DS18x20 device!");
		return false;
	}

	oneWire->reset();
	oneWire->select(_addrDS18B20);
	oneWire->write(0x44, 1);
	delay(100);
	oneWire->reset();
	oneWire->select(_addrDS18B20);
	oneWire->write(0xBE);

	for (uint8_t i(0); i < 9; ++i)
		data[i] = oneWire->read();
	
	int16_t raw = (data[1] << 8) | data[0];
	if (type_s) {
		raw = raw << 3;
		if (data[7] == 0x10) 
			raw = (raw & 0xFFF0) + 12 - data[6];
	}
	else {
		byte cfg = (data[4] & 0x60);
		if (cfg == 0x00) raw = raw & ~7;
		else if (cfg == 0x20) raw = raw & ~3;
		else if (cfg == 0x40) raw = raw & ~1;
	}
	_temperature = (float)raw / 16.0f;
	if (display) {
		debugf(PSTR("Temperature: %f\n"), _temperature);
	}
	return true;
}

float Sensors::getTemperature(bool eval)
{
	if(eval)evalTemperature();
	return _temperature;
}
