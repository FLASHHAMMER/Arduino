#ifndef DS3234_H
#define DS3234_H

#include <avr/pgmspace.h>
#if ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif
#include <SPI.h>

class DS3234 {
	byte cs_pin;

public:
	long time, cal;
	int tempt;

	//	static const uint8_t DS1307_CLOCKHALT = 0b10000000;

	static const uint8_t BITS_SEC = 0b01111111;
	static const uint8_t BITS_HR = 0b00111111;
	static const uint8_t BITS_MIN = 0b01111111;
	static const uint8_t BITS_DOW = 0b00000111;
	static const uint8_t BITS_DATE = 0b00111111;
	static const uint8_t BITS_MTH = 0b00111111;
	static const uint8_t BITS_YR = 0b11111111;

//	static const int DS1307_BASE_YR = 2000;

	static const byte ADDRESS_READ = 0x00;
	static const byte ADDRESS_WRITE = 0x80;
	static const byte REGISTER_SECONDS = 0x00;
	static const byte REGISTER_DAY = 0x03;
	static const byte REGISTER_DATE = 0x04;
	static const byte REGISTER_MONTH_CENTURY = 0x05;
	static const byte REGISTER_YEAR = 0x06;
	static const byte REGISTER_TEMPMSB = 0x11;
	static const byte REGISTER_TEMPLSB = 0x12;

	PROGMEM const static prog_char NameOfDay[36];
	PROGMEM const static prog_char NameOfMonth[60];

	DS3234(byte pin) {
		cs_pin = pin;
		deselect();
	}

	void init();
	void begin() {
		init();
	}
	void select() {
		SPI.setClockDivider(SPI_CLOCK_DIV4);
		SPI.setDataMode(SPI_MODE1);
		SPI.setBitOrder(MSBFIRST);
		digitalWrite(cs_pin, LOW);
	}
	void deselect() {
		digitalWrite(cs_pin, HIGH);
	}

	static long JD2000(byte y, byte m, byte d);
	static long JD2000(const long & yymmdd);

	static char * copyNameOfDay(char * buf, byte d) {
		strcpy_P(buf, NameOfDay + d * 4);
		return buf;
	}

	static char * copyNameOfMonth(char * buf, byte m) {
		strcpy_P(buf, NameOfMonth + ((m + 11) % 12) * 4);
		return buf;
	}

	byte * transfer(byte reg, byte * buf, int num);

	void setCalendar(const long & yymmdd);
	void setTime(const long & hhmmdd);
	void updateTime();
	void updateCalendar();
	void update() { updateTime(); updateCalendar(); }
	void updateTemperature(); // returns x100 of Celsius Centigrade.

	char * timeString(char * buf) {
		sprintf(buf, "%02lx:%02lx:%02lx", time >> 16 & BITS_HR,
				time >> 8 & BITS_MIN, time & BITS_SEC);
		return buf;
	}

	char * calendarString(char * buf) {
		sprintf(buf, "20%02lx-%02lx-%02lx", cal >> 16 & BITS_YR,
				cal >> 8 & BITS_MTH, cal & BITS_DATE);
		return buf;
	}

	byte dayOfWeek();

};

#endif //DS3234_H