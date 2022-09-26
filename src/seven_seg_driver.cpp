#include "Arduino.h"
#include "seven_seg_driver.h"

namespace mrc{

	seven_seg_driver::seven_seg_driver(uint8_t *seg_pins, uint8_t *digit_pins, uint8_t digit_amount, bool seg_low, uint8_t *return_pins, int return_length)
	{
		seven_seg_driver::return_pins = return_pins;
		seven_seg_driver::seg_pins = seg_pins;
		seven_seg_driver::digit_pins = digit_pins;
		seven_seg_driver::digit_amount = digit_amount;
		seven_seg_driver::return_length = return_length;
		seven_seg_driver::seg_low = seg_low;
		seven_seg_driver::is_shift_reg = false;

		seven_seg_driver::draw_buffer = new uint8_t[seven_seg_driver::digit_amount + 1];

		// return_buffer = new bool*[return_length];
		// for (int a = 0; a < return_length; a++) {
		// 	return_buffer[a] = new bool[display_length];
		// }
		
		for (int d = 0; d < seven_seg_driver::digit_amount; d++) {
			pinMode(seven_seg_driver::digit_pins[d], OUTPUT);
		}
		for (int s = 0; s < 8; s++) {
			pinMode(seven_seg_driver::seg_pins[s], OUTPUT);
		}
		// for (int r = 0; r < return_length; r++) {
		// 	pinMode(return_pins[r], INPUT);
		// }

	}
	seven_seg_driver::seven_seg_driver(uint8_t shift_pin, uint8_t latch_pin, uint8_t serial_pin, uint8_t *return_pins, uint8_t digit_amount, int return_length, bool segs_on_end, bool seg_low) 
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

		// return_buffer = new bool*[return_length];
		// for (int a = 0; a < return_length; a++) {
		// 	return_buffer[a] = new bool[display_length];
		// }

		pinMode(seven_seg_driver::serial_pin, OUTPUT);
		pinMode(seven_seg_driver::latch_pin, OUTPUT);
		pinMode(seven_seg_driver::shift_pin, OUTPUT);
		// for (int r = 0; r < return_length; r++) {
		// 	pinMode(return_pins[r], INPUT);
		// }
	}

	void seven_seg_driver::draw() {
		for (int d = 0; d < seven_seg_driver::digit_amount; d++) {
			int d_inv = (seven_seg_driver::digit_amount - 1) - d;
			if (seven_seg_driver::is_shift_reg) {
				long position = 1 << d;
				for (int i = seven_seg_driver::digit_amount - 1; i >= 0; i--) {
					digitalWrite(serial_pin, ((position >> i) % 2) == seven_seg_driver::seg_low);
					digitalWrite(shift_pin, HIGH);
					digitalWrite(shift_pin, LOW);
				}
			}
			else digitalWrite(seven_seg_driver::digit_pins[d_inv], !seven_seg_driver::seg_low);

			for (int s = 7; s >= 0; s--) {
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
				delayMicroseconds(true << dim_value);
				digitalWrite(seven_seg_driver::digit_pins[d_inv], !seven_seg_driver::seg_low);
			}
			
			// for (int r = 0; r < return_length; r++) {
			// 	return_buffer[r][d] = digitalRead(return_pins[r]) == seg_low;
			// }
		}
		// return return_buffer;
	}

	//drawables: .0123456789ABCDEFGHIJLNOPQRSUXY
	void seven_seg_driver::write(char* input) {
		for (int c = 0, d = 0; d <= (seven_seg_driver::digit_amount + 1); c++, d++) {
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
