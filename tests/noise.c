#include <math.h>
#include <stdlib.h>
#include "noise.h"

/*
 * Generate and add noise using Box-Muller method to transform two uniformly
 * distributed random values into normal distributed random values.
 */
int add_noise(unsigned char *in, signed char *out,
	      int len, float snr, float amp)
{
	int i, _amp;
	float x1, x2, w, y1, y2, z1, z2, scale;

	if (fabsf(amp) > 127.0f)
		_amp = (int) 127;
	else
		_amp = (int) amp;

	scale = sqrt(powf((float) _amp, 2.0f) / powf(10.0f, snr / 10.0f));

	for (i = 0; i < len / 2; i++) {
		do {
			x1 = 2.0 * (float) rand() / (float) RAND_MAX - 1.0f;
			x2 = 2.0 * (float) rand() / (float) RAND_MAX - 1.0f;
			w = x1 * x1 + x2 * x2;
		} while (w >= 1.0);

		w = sqrt((-2.0 * log(w)) / w);
		y1 = x1 * w;
		y2 = x2 * w;

		z1 = ((float) (-2 * in[2 * i + 0] + 1) * -amp) + y1 * scale;
		z2 = ((float) (-2 * in[2 * i + 1] + 1) * -amp) + y2 * scale;

		/* Saturate */
		if (z1 > 127.0f)
			z1 = 127.0f;
		else if (z1 < -127.0f)
			z1 = -127.0f;

		if (z2 > 127.0f)
			z2 = 127.0f;
		else if (z2 < -127.0f)
			z2 = -127.0f;

		out[2 * i + 0] = (signed char) z1;
		out[2 * i + 1] = (signed char) z2;
	}

	return 0;
}
