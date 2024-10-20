#include "Arduino.h"

namespace mrc{

	class seven_segment {
	public:
		/**
		 * @brief Native-Arduino-Pin-Mode ( if used, Pin-Mode is already set )
		 * @param seg_pins Segement Pins as an uint8_t Array
		 * @param digit_pins Pins of every Digit on the Entirety of the Display as an uint8_t Array
		 * @param digit_amount Number of Digits on the Entirety of the Display as Int
		 * @param seg_low true = low, false = high
		 * @param digit_sustain_time time that a digit is sustained in microseconds
		 * @param return_pins Return Pins for additional Key Matrix
		 * @param return_length Return Length of the additional Key Matrix
		 */
		seven_segment(const int8_t *seg_pins, const int8_t *digit_pins, const uint8_t digit_amount, const bool seg_low, const int digit_sustain_time = 100, const int8_t *return_pins = {}, const uint8_t return_length = 0);
		
		/**
		 * @brief Shift-Register-Mode ( if used, Pin-Mode is already set )
		 * @param shift_pin Shift Pin of Shift Register
		 * @param latch_pin Latch Pin of Shift Register
		 * @param serial_pin Serial Pin of Shift Register
		 * @param digit_amount Number of Digits on the Entirety of the Display as Int
		 * @param seg_low true = low, false = high
		 * @param seg_on_end Segment Pins are on the End of the Shift Register
		 * @param return_pins Pins for receiving parrallel data from additional Key Matrix
		 * @param return_length Return Length of the additional Key Matrix
		 */
		seven_segment(const int8_t shift_pin, const int8_t latch_pin, const int8_t serial_pin, const uint8_t digit_amount, const bool seg_low, const bool segs_on_end, const int8_t *return_pins = {}, const uint8_t return_length = 0);
		
		// ----- Normal Wired exclusive variables -----

			// Normal Wired Segment-Pins
			const int8_t 		*seg_pins;
			// Normal Wired Segment-Pins
			const int8_t		*digit_pins;

		// ---- Shift Register exclusive variables ----

			// Shift Register Shift Pin
			const uint8_t		shift_pin; 
			// Shift Register Latch Pin
			const uint8_t		latch_pin;
			// Shift Register Serial Pin
			const uint8_t		serial_pin;

			// Shift Register Segment pins location
			const bool 			segs_on_end;

		// ----- Variables needed for both Modes ------

			// Return Mode (Additional Key Matrix)
			
				// Pins for receiving parrallel data from additional Key Matrix
				const int8_t 	*return_pins;
				// Buffer for returned data from additional Key Matrix
				// structure: [matrixReturn][matrixInputMux]
				bool 			**return_buffer;
				// Length of parrallel retured Keymatrix Data in Bits
				uint8_t			return_length;
			
			// Segments are Low-Active
			const bool			seg_low;

			// initialized Mode (Wired Normal or Shift Register)
			const bool			is_shift_reg;

			// 7-Segment Raw Draw buffer					
			uint8_t 			*draw_buffer;

			// Current Segment Drawing Position
			uint8_t				current_digit_draw_position;
			
			// Amount of 7-Segment Digits
			const uint8_t		digit_amount;

			//normal
			uint8_t 			dim_value = 10;
			const int 			digit_sustain_time;

			//0123456789
			const uint8_t		numbers_codec[10] = { 63,6,91,79,102,109,125,7,127,111 };

			//ABCDEFGHIJLNOPQRSTUXY
			const uint8_t 		letters_codec[26] = { 119,124,57,94,121,113,111,118,6,30,112,56,84,84,92,115,103,80,109,120,42,42,42,118,114,91 };

			//special Characters
			const uint8_t 		symbolic_length = 6;
			const char 			symbolic_adaptor[7] = { '-', '(', ')', '=', '"', '*' };
			const uint8_t 		symbolic_codec[7] = { 64, 57, 15, 82, 34, 99 };

			//draws one digit of the written draw-buffer
			void draw();

			// write cstring to Buffer
			void write(const char* input);

	};
}