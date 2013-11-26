
// gcc -shared -Wl,-soname,libwaveform.so -o libwaveform.so waveform.c -lc


#include <math.h>
#include <stdio.h>
#include <stdlib.h>

double radians = (M_PI * 2);

typedef struct WaveForm {
	double frequency;
	double amplitude;
} WaveForm;

WaveForm * new_wave_form(double freq, double amp) {
	WaveForm * result = malloc(sizeof(WaveForm));
	result->frequency = freq; result->amplitude = amp;
	return result;
}

void wave_form_free(WaveForm * wave) { free(wave); }

// period == fractional time (seconds)
short wave_form_sample(WaveForm * wave, double period) {
    return (short) wave->amplitude * sin(radians * wave->frequency * period);
}

void wave_form_sample_write(WaveForm * wave, double period, FILE * file) {
    double sample = wave->amplitude * sin(radians * wave->frequency * period);
    short s16 = (short)sample; fwrite(&s16, sizeof(short), 1, file);
}

// duration == fractional time (seconds)
void wave_form_stream_sample(WaveForm * wave, double duration, size_t rate, FILE * file) {

	double t;
	for (t = 0; t < duration; t += 1.0/rate) {
		double sample = wave->amplitude * sin(radians * wave->frequency * t);
		short s16 = (short)sample;
		fwrite(&s16, sizeof(short), 1, file);
	}

}


