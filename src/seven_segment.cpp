#include "Arduino.h"
#include "seven_segment.h"

namespace mrc{

	seven_segment::seven_segment(const int8_t *seg_pins, const int8_t *digit_pins, const uint8_t digit_amount, const bool seg_low, const int digit_sustain_time, const int8_t *return_pins, const uint8_t return_length)
	: 	seg_pins(seg_pins), digit_pins(digit_pins), digit_amount(digit_amount), 
		seg_low(seg_low), digit_sustain_time(digit_sustain_time), return_pins(return_pins), 
		return_length(return_length), is_shift_reg(false),
		// not needed variable initialization
		shift_pin(-1), latch_pin(-1), serial_pin(-1),
		segs_on_end(false)
	{

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
	
	seven_segment::seven_segment(const int8_t shift_pin, const int8_t latch_pin, const int8_t serial_pin, const uint8_t digit_amount, const bool seg_low, const bool segs_on_end, const int8_t *return_pins, const uint8_t return_length) 
	: 	shift_pin(shift_pin), latch_pin(latch_pin), serial_pin(serial_pin), 
		digit_amount(digit_amount), return_pins(return_pins), seg_low(seg_low), 
		segs_on_end(segs_on_end), return_length(return_length), is_shift_reg(true),
		// not needed variable initialization
		seg_pins((int8_t*)-1), digit_pins((int8_t*)-1), 
		digit_sustain_time(0)
	{

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
		
		// current drawing position
		if (seven_segment::current_digit_draw_position >= seven_segment::digit_amount - 1)
			seven_segment::current_digit_draw_position = 0; 
		else seven_segment::current_digit_draw_position++;

		// digit position declare
		uint8_t d = seven_segment::current_digit_draw_position;
		uint8_t d_inv = (seven_segment::digit_amount - 1) - d;

		// set digit for draw
		if (seven_segment::is_shift_reg) {
			// Shift Register Mode
			long position = 1 << d;
			for (int8_t i = seven_segment::digit_amount - 1; i >= 0; i--) {
				digitalWrite(seven_segment::serial_pin, ((position >> i) % 2) == seven_segment::seg_low);
				digitalWrite(seven_segment::shift_pin, HIGH);
				digitalWrite(seven_segment::shift_pin, LOW);
			}
		}
		else {
			// Normal Wired Mode
			digitalWrite(seven_segment::digit_pins[last_d_inv], !seven_segment::seg_low);
		}

		// set segments for draw
		for (int8_t s = 7; s >= 0; s--) {
			// set seg Date bit
			bool set_bit = (seven_segment::draw_buffer[d] >> s) % 2;
			if (seven_segment::is_shift_reg) {
				// Shift Register Mode
				digitalWrite(seven_segment::serial_pin, set_bit == !seven_segment::seg_low);
				digitalWrite(seven_segment::shift_pin, HIGH);
				digitalWrite(seven_segment::shift_pin, LOW);
			}
			else {
				// Normal Wired Mode
				digitalWrite(seven_segment::seg_pins[s], set_bit == !seven_segment::seg_low);
			}
		}

		// Latch/Sustain Output
		if (seven_segment::is_shift_reg) {
			// Shift Register Mode
			digitalWrite(seven_segment::latch_pin, HIGH);
			digitalWrite(seven_segment::latch_pin, LOW);
		}
		else {
			// Normal Wired Mode
			digitalWrite(seven_segment::digit_pins[d_inv], seven_segment::seg_low);
			delayMicroseconds(seven_segment::digit_sustain_time);
			last_d_inv = d_inv;
		}

		// read pins for aditional Key Matrix
		for (uint8_t r = 0; r < seven_segment::return_length; r++) {
			seven_segment::return_buffer[r][d] = digitalRead(seven_segment::return_pins[r]) == seven_segment::seg_low;
		}
		
		// Normal Wired Mode - Cut Output for current Digit
		if(!is_shift_reg) digitalWrite(seven_segment::digit_pins[last_d_inv], !seven_segment::seg_low);
		
	}

	//drawables: .0123456789ABCDEFGHIJLNOPQRSUXY
	void seven_segment::write(const char* input) {
		for (uint16_t c = 0, d = 0; d <= (seven_segment::digit_amount + 1); c++, d++) {

			seven_segment::draw_buffer[d] = 0;

			for (size_t s = 0; s < seven_segment::symbolic_length; s++)
			{
				if(seven_segment::symbolic_adaptor[s] == input[c]){
					draw_buffer[d] = seven_segment::symbolic_codec[s];
					break;
				}
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
