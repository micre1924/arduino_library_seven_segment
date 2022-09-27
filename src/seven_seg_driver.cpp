#include "Arduino.h"
#include "seven_seg_driver.h"

namespace mrc{

	seven_seg_driver::seven_seg_driver(uint8_t *seg_pins, uint8_t *digit_pins, uint8_t digit_amount, bool seg_low, int digit_sustain_time, uint8_t *return_pins, uint8_t return_length)
	{
		seven_seg_driver::return_pins = return_pins;
		seven_seg_driver::seg_pins = seg_pins;
		seven_seg_driver::digit_pins = digit_pins;
		seven_seg_driver::digit_amount = digit_amount;
		seven_seg_driver::return_length = return_length;
		seven_seg_driver::seg_low = seg_low;
		seven_seg_driver::is_shift_reg = false;

		seven_seg_driver::draw_buffer = new uint8_t[seven_seg_driver::digit_amount + 1];

		seven_seg_driver::return_buffer = new bool*[seven_seg_driver::return_length];
		for (uint8_t a = 0; a < seven_seg_driver::return_length; a++) {
			seven_seg_driver::return_buffer[a] = new bool[seven_seg_driver::digit_amount];
		}
		
		for (uint8_t d = 0; d < seven_seg_driver::digit_amount; d++) {
			pinMode(seven_seg_driver::digit_pins[d], OUTPUT);
		}
		for (uint8_t s = 0; s < 8; s++) {
			pinMode(seven_seg_driver::seg_pins[s], OUTPUT);
		}
		for (uint8_t r = 0; r < return_length; r++) {
			pinMode(return_pins[r], INPUT);
		}

	}
	seven_seg_driver::seven_seg_driver(uint8_t shift_pin, uint8_t latch_pin, uint8_t serial_pin, uint8_t digit_amount, bool seg_low, bool segs_on_end, uint8_t *return_pins, uint8_t return_length) 
	{
		seven_seg_driver::return_pins = return_pins;
		seven_seg_driver::shift_pin = shift_pin;
		seven_seg_driver::latch_pin = latch_pin;
		seven_seg_driver::serial_pin = serial_pin;
		seven_seg_driver::segs_on_end = segs_on_end;
		seven_seg_driver::digit_amount = digit_amount;
		seven_seg_driver::return_length = return_length;
		seven_seg_driver::seg_low = seg_low;
		seven_seg_driver::is_shift_reg = true;

		seven_seg_driver::draw_buffer = new uint8_t[seven_seg_driver::digit_amount + 1];

		seven_seg_driver::return_buffer = new bool*[seven_seg_driver::return_length];
		for (uint8_t a = 0; a < seven_seg_driver::return_length; a++) {
			seven_seg_driver::return_buffer[a] = new bool[seven_seg_driver::digit_amount];
		}

		pinMode(seven_seg_driver::serial_pin, OUTPUT);
		pinMode(seven_seg_driver::latch_pin, OUTPUT);
		pinMode(seven_seg_driver::shift_pin, OUTPUT);
		
		for (uint8_t r = 0; r < return_length; r++) {
			pinMode(return_pins[r], INPUT);
		}
	}

	void seven_seg_driver::draw() {
		uint8_t last_d_inv = 0U;
		for (uint8_t d = 0; d < seven_seg_driver::digit_amount; d++) {
			uint8_t d_inv = (seven_seg_driver::digit_amount - 1) - d;
			if (seven_seg_driver::is_shift_reg) {
				long position = 1 << d;
				for (uint8_t i = seven_seg_driver::digit_amount - 1; i >= 0; i--) {
					digitalWrite(seven_seg_driver::serial_pin, ((position >> i) % 2) == seven_seg_driver::seg_low);
					digitalWrite(seven_seg_driver::shift_pin, HIGH);
					digitalWrite(seven_seg_driver::shift_pin, LOW);
				}
			}
			else {
				digitalWrite(seven_seg_driver::digit_pins[last_d_inv], !seven_seg_driver::seg_low);
			}

			for (int8_t s = 7; s >= 0; s--) {
				bool set_bit = (seven_seg_driver::draw_buffer[d] >> s) % 2;
				if (seven_seg_driver::is_shift_reg) {
					digitalWrite(seven_seg_driver::serial_pin, set_bit == !seven_seg_driver::seg_low);
					digitalWrite(seven_seg_driver::shift_pin, HIGH);
					digitalWrite(seven_seg_driver::shift_pin, LOW);
				}
				else digitalWrite(seven_seg_driver::seg_pins[s], set_bit == !seven_seg_driver::seg_low);
			}
			if (seven_seg_driver::is_shift_reg) {
				digitalWrite(seven_seg_driver::latch_pin, HIGH);
				digitalWrite(seven_seg_driver::latch_pin, LOW);
			}
			else {
				digitalWrite(seven_seg_driver::digit_pins[d_inv], seven_seg_driver::seg_low);
				delayMicroseconds(100);
				last_d_inv = d_inv;
			}
			for (uint8_t r = 0; r < seven_seg_driver::return_length; r++) {
				seven_seg_driver::return_buffer[r][d] = digitalRead(seven_seg_driver::return_pins[r]) == seven_seg_driver::seg_low;
			}
		}
		if(!is_shift_reg) digitalWrite(seven_seg_driver::digit_pins[last_d_inv], !seven_seg_driver::seg_low);
	}

	//drawables: .0123456789ABCDEFGHIJLNOPQRSUXY
	void seven_seg_driver::write(char* input) {
		for (uint16_t c = 0, d = 0; d <= (seven_seg_driver::digit_amount + 1); c++, d++) {
			seven_seg_driver::draw_buffer[d] = 0;
			switch (input[c])
			{
				//case 0xDF: draw_buffer[d] = 99; break;	//�
				//case 0xE4: draw_buffer[d] = 114; break;	//�
				case '-': draw_buffer[d] = 64; break;
				case '(': draw_buffer[d] = 57; break;
				case ')': draw_buffer[d] = 15; break;
				case '=': draw_buffer[d] = 72; break;
				case 0x22: draw_buffer[d] = 34; break;	//"
				default: break;
			}
			if (input[c] >= 65 && input[c] <= 90) draw_buffer[d] = seven_seg_driver::letters_codec[input[c] - 65];	//ascii A..Z
			if (input[c] >= 48 && input[c] <= 57) draw_buffer[d] = seven_seg_driver::numbers_codec[input[c] - 48];	//ascii 0..9 to seven_seg
			if (input[c] == 46 || input[c] == 44) { 
				if (input[c-1] != 46 && input[c-1] != 44) d--;
				seven_seg_driver::draw_buffer[d] = seven_seg_driver::draw_buffer[d] + 128;
			}		//ascii Dot, Comma
		}
	}

	// void seven_seg_driver::write(String input) {
	// 	seven_seg_driver::write(sprintf())
	// }
}
