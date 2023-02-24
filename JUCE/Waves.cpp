#pragma once
#include "Waves.h"


/*
	angle is assumed to be in the range [0, 2PI).
*/

float waves::square_wave(float angle, float pulse_width) {
	return angle > (M_TWO_PI * pulse_width) ? 1.0f : -1.0f;
}

// TODO: squareness
float waves::ramp_wave(float angle, float squareness) {
	return 1 - (angle / M_PI);
}

// TODO: shape
float waves::triangle_wave(float angle, float shape) {
	if (angle < M_PI) {
		return 1 - (angle / M_HALF_PI);
	}
	else {
		return ((angle - M_PI) / M_HALF_PI) - 1;
	}
}
