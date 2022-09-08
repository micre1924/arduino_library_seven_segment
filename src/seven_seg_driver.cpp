/*
 Name:		seven_seg_driver.cpp
 Created:	4/3/2022 5:54:36 PM
 Author:	GPD
 Editor:	http://www.visualmicro.com
*/

#include "seven_seg_driver.h"
#include "avr/io.h"

seven_seg_driver::seven_seg_driver(const int *seg_pins, const int *display_pins, int display_length, bool seg_low, const int *return_pins, int return_length)
{
	draw_buffer = new char[display_length + 1];

	// return_buffer = new bool*[return_length];
	// for (int a = 0; a < return_length; a++) {
	// 	return_buffer[a] = new bool[display_length];
	// }
	
	for (int d = 0; d < display_length; d++) {
		pinMode(display_pins[d], OUTPUT);
	}
	for (int s = 0; s < 8; s++) {
		pinMode(seg_pins[s], OUTPUT);
	}
	// for (int r = 0; r < return_length; r++) {
	// 	pinMode(return_pins[r], INPUT);
	// }

	seven_seg_driver::return_pins = return_pins;
	seven_seg_driver::seg_pins = seg_pins;
	seven_seg_driver::display_pins = display_pins;
	seven_seg_driver::display_length = display_length;
	seven_seg_driver::return_length = return_length;
	seven_seg_driver::seg_low = seg_low;
}
seven_seg_driver::seven_seg_driver(int shift_pin, int latch_pin, int serial_pin, const int *return_pins,int display_length,int return_length, bool segs_on_end, bool seg_low) 
{
	draw_buffer = new char[display_length];

	// return_buffer = new bool*[return_length];
	// for (int a = 0; a < return_length; a++) {
	// 	return_buffer[a] = new bool[display_length];
	// }

	pinMode(serial_pin, OUTPUT);
	pinMode(latch_pin, OUTPUT);
	pinMode(shift_pin, OUTPUT);
	// for (int r = 0; r < return_length; r++) {
	// 	pinMode(return_pins[r], INPUT);
	// }

	seven_seg_driver::return_pins = return_pins;
	seven_seg_driver::shift_pin = shift_pin;
	seven_seg_driver::latch_pin = latch_pin;
	seven_seg_driver::serial_pin = serial_pin;
	seven_seg_driver::segs_on_end = segs_on_end;
	seven_seg_driver::display_length = display_length;
	seven_seg_driver::return_length = return_length;
	seven_seg_driver::seg_low = seg_low;
	seven_seg_driver::is_shift_reg = true;
}

void seven_seg_driver::draw() {
	for (int d = 0; d < display_length; d++) {
		int d_inv = (display_length - 1) - d;
		if (is_shift_reg) {
			long position = 1 << d;
			for (int i = display_length - 1; i >= 0; i--) {
				digitalWrite(serial_pin, ((position >> i) % 2) == seg_low);
				digitalWrite(shift_pin, HIGH);
				digitalWrite(shift_pin, LOW);
			}
		}
		else digitalWrite(display_pins[d_inv], !seg_low);

		for (int s = 7; s >= 0; s--) {
			bool set_bit = (draw_buffer[d] >> s) % 2;
			if (is_shift_reg) {
				digitalWrite(serial_pin, set_bit == !seg_low);
				digitalWrite(shift_pin, HIGH);
				digitalWrite(shift_pin, LOW);
			}
			else digitalWrite(seg_pins[s], set_bit == !seg_low);
		}
		if (is_shift_reg) {
			digitalWrite(latch_pin, HIGH);
			digitalWrite(latch_pin, LOW);
		}
		else {
			digitalWrite(display_pins[d_inv], seg_low);
			delayMicroseconds(100);
			digitalWrite(display_pins[d_inv], !seg_low);
		}
		
		// for (int r = 0; r < return_length; r++) {
		// 	return_buffer[r][d] = digitalRead(return_pins[r]) == seg_low;
		// }
	}
	// return return_buffer;
}

//drawables: .0123456789ABCDEFGHIJLNOPQRSUXY
void seven_seg_driver::write(char *input) {
	for (int c = 0, d = 0; d <= (display_length + 1); c++, d++) {
		draw_buffer[d] = 0;
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
		if (input[c] >= 65 && input[c] <= 90) draw_buffer[d] = letters_codec[input[c] - 65];	//ascii A..Z
		if (input[c] >= 48 && input[c] <= 57) draw_buffer[d] = numbers_codec[input[c] - 48];	//ascii 0..9 to seven_seg
		if (input[c] == 46 || input[c] == 44) { 
			if (input[c-1] != 46 && input[c-1] != 44) d--;
			draw_buffer[d] = draw_buffer[d] + 128;
		}		//ascii Dot, Comma
	}
}
