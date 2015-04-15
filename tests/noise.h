#ifndef _NOISE_H_
#define _NOISE_H_

int add_noise(unsigned char *in, signed char *out,
	      int len, float snr, float amp);

#endif /* _NOISE_H_ */
