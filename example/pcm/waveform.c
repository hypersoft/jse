
// gcc -shared -Wl,-soname,libwaveform.so -o libwaveform.so waveform.c -lc


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdarg.h>

double radians = (M_PI * 2);
double interval = 1.0;

typedef struct WaveForm {
	double frequency;
	double amplitude;
} WaveForm;

typedef struct WaveBuffer { // for customized WaveData
	short * data;
	size_t length;
	size_t loop;
	double amplitude;
} WaveBuffer;


WaveForm * new_wave_form(double freq, double amp) {
	WaveForm * result = malloc(sizeof(WaveForm));
	result->frequency = freq; result->amplitude = amp;
	return result;
}

void wave_form_free(WaveForm * wave) { free(wave); }

WaveBuffer * new_wave_buffer(size_t length, double amplitude) {
	WaveBuffer * result = malloc(sizeof(WaveBuffer));
	result->amplitude = amplitude; result->loop = 0;
	result->data = malloc(length << sizeof(short)); result->length = length;
	return result;
}

void wave_buffer_free(WaveBuffer * wave) { free(wave->data); free(wave); }

// phase == fractional time (seconds)
short wave_form_sample(WaveForm * wave, double phase) {
    return (short) wave->amplitude * sin(radians * wave->frequency * phase);
}

short wave_form_mix_samples(short a, short b) {
	int target = a + b;
	if (target > SHRT_MAX) return SHRT_MAX;
	if (target < SHRT_MIN) return SHRT_MIN;
	else return (short) target;
}

WaveBuffer * wave_buffer_mix(WaveBuffer * a, WaveBuffer * b, double amplitude) {
	size_t length = (a->length > b->length) ? a->length : b->length;
	WaveBuffer * result = new_wave_buffer(length, amplitude);
	size_t i; double sample;
	for (i = 0; i < length; i++) {
		sample = amplitude * wave_form_mix_samples(a->data[i], b->data[i]);
		result->data[i] = (short) sample;
	}
	return result;
}

void wave_form_sample_write(WaveForm * wave, double phase, FILE * file) {
    double sample = wave->amplitude * sin(radians * wave->frequency * phase);
    short s16 = (short)sample; fwrite(&s16, sizeof(short), 1, file);
}

// duration == fractional time (seconds)
void wave_form_stream_sample(WaveForm * wave, double duration, size_t rate, FILE * file) {

	double phase;
	for (phase = 0; phase < duration; phase += interval/rate) {
		double sample = wave->amplitude * sin(radians * wave->frequency * phase);
		short s16 = (short)sample;
		fwrite(&s16, sizeof(short), 1, file);
	}

}

WaveBuffer * wave_form_buffer(WaveForm * wave, double duration, size_t rate) {
	size_t length = (size_t)(duration * rate);
	WaveBuffer * buffer = new_wave_buffer(length, wave->amplitude);
	double phase;
	for (phase = 0; phase < duration; phase += interval/rate) {
		double sample = wave->amplitude * sin(radians * wave->frequency * phase);
		buffer->data[buffer->loop++] = (short)sample;
	}
	buffer->loop = 0;
	return buffer;
}

void wave_form_stream_mix(double duration, size_t rate, FILE * file, char * fmt, ...) {

	double phase, sample; va_list ap; WaveForm * wave; WaveBuffer * buffer;
	size_t index, sampleIndex = 0; char type; short s16;
	for (phase = 0; phase < duration; phase += interval/rate, sampleIndex++) {
		va_start(ap, fmt); index = 0;
		while ((type = fmt[index++]) != 0) {
			if (type == 'w') {
				wave = va_arg(ap, WaveForm *);
				sample = wave->amplitude * sin(radians * wave->frequency * phase);
				s16 = (short)sample;
				fwrite(&s16, sizeof(short), 1, file);
			} else if (type == 'b') {
				buffer = va_arg(ap, WaveBuffer *);
				if (sampleIndex < buffer->length) {
					s16 = buffer->data[sampleIndex];
				} else s16 = 0;
				fwrite(&s16, sizeof(short), 1, file);
			} else if (type == 'l') {
				buffer = va_arg(ap, WaveBuffer *);
				if (buffer->loop >= buffer->length) buffer->loop = 0;
				if (buffer->data) s16 = buffer->data[buffer->loop++];
				else s16 = 0; fwrite(&s16, sizeof(short), 1, file);
			} else {
				fprintf(stderr, "TypeError: '%c' is not a valid WaveForm argument\n", type);
				return; 
			}
		}
	}
}


