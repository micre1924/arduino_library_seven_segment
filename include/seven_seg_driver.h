#include "Arduino.h"

namespace mrc{

	class seven_seg_driver {
	public:
		//Native-Arduino-Pin-Mode
		//if used Pin-Mode is already set
		/**
		 * @param seg_pins Segement Pins as an Int Array
		 * @param digit_pins Pins of every Digit on the Entirety of the Display as an Int Array
		 * @param digit_amount Number of Digits on the Entirety of the Display as Int
		 * @param seg_low true = low, false = high
		 */
		seven_seg_driver(uint8_t *seg_pins, uint8_t *digit_pins, uint8_t digit_amount, bool seg_low, uint8_t *return_pins = {}, int return_length = 0);
		//Shift-Register-Mode
		//if used Pin-Mode is already set
		seven_seg_driver(uint8_t shift_pin, uint8_t latch_pin, uint8_t serial_pin, uint8_t *return_pins, uint8_t display_length, int return_length, bool segs_on_end, bool seg_low);
		//pointers
		// bool 	**return_buffer;
		uint8_t *seg_pins, 
				*digit_pins, 
				*return_pins,
			 	*draw_buffer;
		//normal
		bool 	segs_on_end, 
				seg_low, 
				is_shift_reg = false;
		uint8_t digit_amount,
				dim_value = 10;
		int 	shift_pin, 
				latch_pin, 
				serial_pin, 
				return_length;
		//0123456789
		uint8_t	numbers_codec[10] = { 63,6,91,79,102,109,125,7,127,111 };

		//ABCDEFGHIJLNOPQRSTUXY
		uint8_t letters_codec[26] = { 119,124,57,94,121,113,111,118,6,30,112,56,84,84,92,115,103,80,109,120,42,42,42,118,114,91 };

		//��-()
		//char symbolic_adaptor[7] = { '�', '�', '-', '(', ')', '=', '"' };
		//char symbolic_codec[7] = { 99,114,64,57,15,82,34 };

		//draws the written draw-buffer once
		//returns 2D Array input-matrix [return-value][mux-position]
		void draw();
		void write(char* input);

	};
}