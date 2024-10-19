#include "Arduino.h"
#include "seven_segment.h"

namespace mrc{

	seven_segment::seven_segment(uint8_t *seg_pins, uint8_t *digit_pins, uint8_t digit_amount, bool seg_low, int digit_sustain_time, uint8_t *return_pins, uint8_t return_length)
	{
		seven_segment::return_pins = return_pins;
		seven_segment::seg_pins = seg_pins;
		seven_segment::digit_pins = digit_pins;
		seven_segment::digit_amount = digit_amount;
		seven_segment::return_length = return_length;
		seven_segment::seg_low = seg_low;
		seven_segment::is_shift_reg = false;

		seven_segment::draw_buffer = new uint8_t[seven_segment::digit_amount + 1];

		seven_segment::return_buffer = new bool*[seven_segment::return_length];
		for (uint8_t a = 0; a < seven_segment::return_length; a++) {
			seven_segment::return_buffer[a] = new bool[seven_segment::digit_amount];
		}
		
		for (uint8_t d = 0; d < seven_segment::digit_amount; d++) {
			pinMode(seven_segment::digit_pins[d], OUTPUT);
		}
		for (uint8_t s = 0; s < 8; s++) {
			pinMode(seven_segment::seg_pins[s], OUTPUT);
		}
		for (uint8_t r = 0; r < return_length; r++) {
			pinMode(return_pins[r], INPUT);
		}

	}
	seven_segment::seven_segment(uint8_t shift_pin, uint8_t latch_pin, uint8_t serial_pin, uint8_t digit_amount, bool seg_low, bool segs_on_end, uint8_t *return_pins, uint8_t return_length) 
	{
		seven_segment::return_pins = return_pins;
		seven_segment::shift_pin = shift_pin;
		seven_segment::latch_pin = latch_pin;
		seven_segment::serial_pin = serial_pin;
		seven_segment::segs_on_end = segs_on_end;
		seven_segment::digit_amount = digit_amount;
		seven_segment::return_length = return_length;
		seven_segment::seg_low = seg_low;
		seven_segment::is_shift_reg = true;

		seven_segment::draw_buffer = new uint8_t[seven_segment::digit_amount + 1];

		seven_segment::return_buffer = new bool*[seven_segment::return_length];
		for (uint8_t a = 0; a < seven_segment::return_length; a++) {
			seven_segment::return_buffer[a] = new bool[seven_segment::digit_amount];
		}

		pinMode(seven_segment::serial_pin, OUTPUT);
		pinMode(seven_segment::latch_pin, OUTPUT);
		pinMode(seven_segment::shift_pin, OUTPUT);
		
		for (uint8_t r = 0; r < return_length; r++) {
			pinMode(return_pins[r], INPUT);
		}
	}

	void seven_segment::draw() {
		uint8_t last_d_inv = 0U;
		for (uint8_t d = 0; d < seven_segment::digit_amount; d++) {
			uint8_t d_inv = (seven_segment::digit_amount - 1) - d;
			if (seven_segment::is_shift_reg) {
				long position = 1 << d;
				for (uint8_t i = seven_segment::digit_amount - 1; i >= 0; i--) {
					digitalWrite(seven_segment::serial_pin, ((position >> i) % 2) == seven_segment::seg_low);
					digitalWrite(seven_segment::shift_pin, HIGH);
					digitalWrite(seven_segment::shift_pin, LOW);
				}
			}
			else {
				digitalWrite(seven_segment::digit_pins[last_d_inv], !seven_segment::seg_low);
			}

			for (int8_t s = 7; s >= 0; s--) {
				bool set_bit = (seven_segment::draw_buffer[d] >> s) % 2;
				if (seven_segment::is_shift_reg) {
					digitalWrite(seven_segment::serial_pin, set_bit == !seven_segment::seg_low);
					digitalWrite(seven_segment::shift_pin, HIGH);
					digitalWrite(seven_segment::shift_pin, LOW);
				}
				else digitalWrite(seven_segment::seg_pins[s], set_bit == !seven_segment::seg_low);
			}
			if (seven_segment::is_shift_reg) {
				digitalWrite(seven_segment::latch_pin, HIGH);
				digitalWrite(seven_segment::latch_pin, LOW);
			}
			else {
				digitalWrite(seven_segment::digit_pins[d_inv], seven_segment::seg_low);
				delayMicroseconds(100);
				last_d_inv = d_inv;
			}
			for (uint8_t r = 0; r < seven_segment::return_length; r++) {
				seven_segment::return_buffer[r][d] = digitalRead(seven_segment::return_pins[r]) == seven_segment::seg_low;
			}
		}
		if(!is_shift_reg) digitalWrite(seven_segment::digit_pins[last_d_inv], !seven_segment::seg_low);
	}

	//drawables: .0123456789ABCDEFGHIJLNOPQRSUXY
	void seven_segment::write(char* input) {
		for (uint16_t c = 0, d = 0; d <= (seven_segment::digit_amount + 1); c++, d++) {
			seven_segment::draw_buffer[d] = 0;
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
			if (input[c] >= 65 && input[c] <= 90) draw_buffer[d] = seven_segment::letters_codec[input[c] - 65];	//ascii A..Z
			if (input[c] >= 48 && input[c] <= 57) draw_buffer[d] = seven_segment::numbers_codec[input[c] - 48];	//ascii 0..9 to seven_seg
			if (input[c] == 46 || input[c] == 44) { 
				if (input[c-1] != 46 && input[c-1] != 44) d--;
				seven_segment::draw_buffer[d] = seven_segment::draw_buffer[d] + 128;
			}		//ascii Dot, Comma
		}
	}

	// void seven_seg_driver::write(String input) {
	// 	seven_seg_driver::write(sprintf())
	// }
}
