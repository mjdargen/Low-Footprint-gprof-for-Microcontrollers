#ifndef SOUND_H
#define SOUND_H

#include <stdint.h>

// Configuration
#define NUM_VOICES (8)
#define AUDIO_SAMPLE_FREQ (20000.0f)
#define NUM_STEPS (64)
#define NUM_WAVEFORM_SAMPLES (512)
#define USE_DOUBLE_BUFFER (0)

// On Port E
#define AMP_ENABLE_POS (29)
#define DAC_POS (30)
#define MAX_DAC_CODE (4095)

// Conversions
#define AUDIO_SAMPLE_PERIOD_US (1000000/AUDIO_SAMPLE_FREQ)
#define FREQ_TO_PERIOD(f) (AUDIO_SAMPLE_FREQ/(f))

// Voice type definitions
typedef enum {VW_UNINIT, VW_NOISE, VW_SQUARE, VW_SINE} VW_E;

typedef struct {
	uint16_t Volume; // scaled by 65536
	int16_t Decay; // scaled by 65536. 0 means no decay in volume.
	uint32_t Duration; // measured in samples
	uint32_t Counter; // internal, measured in samples (for sine, is scaled by 256)
	uint32_t CounterIncrement; // for sine only: internal, samples scaled by 256
	uint16_t Period; // measured in samples
	VW_E Type; // Sine, square, white noise
} VOICE_T;

typedef struct {
	uint32_t Volume, Frequency, Decay, Duration, Delay;
} NOTE_T;

void Play_Tone(void);
void Sound_Init(void);

void Sound_Enable_Amp(void);
void Sound_Disable_Amp(void);

void Sound_Refill_Buffer(uint32_t samples);
void Play_Waveform_with_DMA(void);
int16_t Sound_Generate_Next_Sample (VOICE_T *voice);
void Configure_DMA_For_Playback(uint16_t * source1, uint16_t * source2, uint32_t count, uint32_t num_playbacks);
void Start_DMA_Playback();

extern int16_t SineTable[NUM_STEPS];
extern uint16_t Waveform[2][NUM_WAVEFORM_SAMPLES];
extern VOICE_T Voice[NUM_VOICES];
extern uint8_t write_buffer_num;

extern uint16_t * Reload_DMA_Source[2];
extern uint8_t read_buffer_num;

extern uint32_t DMA_Playback_Count;

#endif // SOUND_H
