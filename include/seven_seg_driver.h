#include "Arduino.h"

namespace mrc{

	class seven_seg_driver {
	public:
		//Native-Arduino-Pin-Mode
		//if used Pin-Mode is already set
		/**
		 * @param seg_pins Segement Pins as an Int Array
		 * @param digit_pins Pins of every Digit on the Entirety of the Display as an Int Array
		 * @param display_length Number of Digits on the Entirety of the Display as Int
		 * @param seg_low true = low, false = high
		 */
		seven_seg_driver(uint8_t *seg_pins, uint8_t *digit_pins, int display_length, bool seg_low, uint8_t *return_pins = {}, int return_length = 0);
		//Shift-Register-Mode
		//if used Pin-Mode is already set
		seven_seg_driver(uint8_t shift_pin, uint8_t latch_pin, uint8_t serial_pin, uint8_t *return_pins, int display_length, int return_length, bool segs_on_end, bool seg_low);
		
		uint8_t *seg_pins, *display_pins, *return_pins;
		int shift_pin, latch_pin, serial_pin, display_length, return_length;
		bool segs_on_end, seg_low, is_shift_reg = false;
		bool debug = false;
		int debug_time = 100;
		char *draw_buffer;
		// bool **return_buffer;
		//0123456789
		char numbers_codec[10] = { 63,6,91,79,102,109,125,7,127,111 };

		//ABCDEFGHIJLNOPQRSUXY
		char letters_codec[26] = { 119,124,57,94,121,113,111,116,6,30,112,56,84,84,92,115,103,80,109,49,42,42,42,118,114,91 };

		//��-()
		//char symbols_adaptor[7] = { '�', '�', '-', '(', ')', '=', '"' };
		//char symbols_codec[7] = { 99,114,64,57,15,82,34 };

		//draws the written draw-buffer once
		//returns 2D Array input-matrix [return-value][mux-position]
		void draw();
		void write(char* input);

	};
}