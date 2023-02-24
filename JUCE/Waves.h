#ifndef M_PI
#define M_PI		3.14159265358
#endif

#ifndef M_TWO_PI
#define M_TWO_PI    6.28318530718
#endif

#ifndef M_HALF_PI
#define M_HALF_PI    1.570796327
#endif

namespace waves {

	enum {
		WAVE_SINE,
		WAVE_TRIANGLE,
		WAVE_SQUARE,
		WAVE_RAMP
	};

	float square_wave(float angle, float pulse_width = 0.5f);

	float ramp_wave(float angle, float squareness = 0.5f);

	float triangle_wave(float angle, float shape = 0.5f);

}