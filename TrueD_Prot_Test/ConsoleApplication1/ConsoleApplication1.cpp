// ConsoleApplication1.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
#include <stdint.h>
#include "crc16.h"
#pragma pack(8)

typedef struct trueDdata_t
{
	const uint8_t preamble1 = 0xFD;
	const uint8_t preamble2 = 0x07;
	uint8_t band;
	uint8_t channel;
	uint8_t rssi_max;
	uint8_t rssi1;
	uint8_t rssi2;
	const uint8_t  packet_size = 15;
}trueDdata_s;

typedef struct trueDdata_rx_t
{
	const uint8_t preamble1 = 0xFD;
	const uint8_t preamble2 = 0x07;
	uint8_t unknown1;
	uint8_t unknown2;
	uint8_t unknown3;
	uint8_t unknown4;
	uint8_t unknown5;
	uint8_t band_channel;
	uint8_t unknown6;
	uint8_t rssi_max;
	uint8_t rssi1;
	uint8_t rssi2;
	uint8_t unknown7;
	uint8_t crc_h;
	uint8_t crc_l;
}trueDdata_rx_s;

trueDdata_s trueDdata;

uint8_t testData1[] = //{ 0xFD, 0x07, 0x02, 0x05, 0x40, 0xEE, 0x00, 0x24, 0x00, 0xC8, 0xC8, 0xC8, 0x02, 0x71, 0x66 };
{0xFD, 0x00, 0xFD, 0x07, 0x02, 0x05, 0x40, 0xEE, 0x00, 0x0C, 0x00, 0x1E, 0x1B, 0x1E, 0x02, 0x45, 0xF3};


class Serial {
private:
	uint8_t cnt;
public:
	Serial() {
		cnt = 0;
	};
	uint8_t available(){
		return sizeof(testData1)-cnt;
	};
	uint8_t read(){
		return testData1[this->cnt++];
	};
	void readBytes(uint8_t *buf, uint8_t size) {
		for (int i = 0; i < size; i++) {
			buf[i] = testData1[cnt++];
		}
	};
};

Serial OSDsoft;


enum enum_trueD_packet_states {
	STATE_START = 0x00,
	STATE_GOT_PREAMBLE_1,
	STATE_GOT_PREAMBLE_2
};
uint8_t trueD_packet_state = STATE_START;


int main()
{

	while (true) {
		if (trueD_packet_state == STATE_START && OSDsoft.available()) {
			uint8_t rx = OSDsoft.read();
			trueD_packet_state = (rx == trueDdata.preamble1) ? STATE_GOT_PREAMBLE_1 : trueD_packet_state;

		}
		else if (trueD_packet_state == STATE_GOT_PREAMBLE_1 && OSDsoft.available()) {
			uint8_t rx = OSDsoft.read();
			trueD_packet_state = (rx == trueDdata.preamble2) ? STATE_GOT_PREAMBLE_2 : STATE_START;

		}
		else if (trueD_packet_state == STATE_GOT_PREAMBLE_2 && OSDsoft.available() >= trueDdata.packet_size - 2) {
			trueDdata_rx_s trueDdata_rx;
			OSDsoft.readBytes(&trueDdata_rx.unknown1, trueDdata.packet_size - 2);
			uint16_t crc_rx = ((uint16_t)trueDdata_rx.crc_h << 8) | trueDdata_rx.crc_l;
			uint16_t crc_calc = crc16_ccitt(&trueDdata_rx.preamble1, trueDdata.packet_size - 2);
			if (crc_rx == crc_calc) { //got complete data frame + crc match
				trueDdata.band = (trueDdata_rx.band_channel >> 3);
				trueDdata.channel = trueDdata_rx.band_channel & 0x07;
				trueDdata.rssi_max = trueDdata_rx.rssi_max;
				trueDdata.rssi1 = trueDdata_rx.rssi1;
				trueDdata.rssi2 = trueDdata_rx.rssi2;
			}
			trueD_packet_state = STATE_START;
		}
	}

    return 0;
}

