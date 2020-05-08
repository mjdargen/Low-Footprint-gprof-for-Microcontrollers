#include <stdint.h>
#include <MKL25Z4.h>
#include <math.h>

#include "FRTOS1.h"
#include "mytasks.h"
#include "sound.h"
#include "misc.h"
#include "gpio_defs.h"
#include "timers.h"
#include "AudioAmp.h"
#include "WAIT1.h"
#include "DacLdd1.h"
#include "DMAMUX_PDD.h"

uint16_t * Reload_DMA_Source[2]={0,0};
uint32_t Reload_DMA_Byte_Count=0;
uint32_t DMA_Playback_Count=0;
uint8_t read_buffer_num=0;

void SineTable_Init(void) {
	unsigned n;

	for (n=0; n<NUM_STEPS; n++) {
		SineTable[n] = (MAX_DAC_CODE/2)*sinf(n*(2*3.1415927/NUM_STEPS));
	}
}

/* Fill waveform buffers with silence. */
void Init_Waveform(void) {
	uint32_t i;

	for (i=0; i<NUM_WAVEFORM_SAMPLES; i++) {
		Waveform[0][i] = (MAX_DAC_CODE/2);
		Waveform[1][i] = (MAX_DAC_CODE/2);
	}
}

void Init_Voices(void) {
	uint16_t i;

	for (i=0; i<NUM_VOICES; i++) {
		Voice[i].Volume = 0;
		Voice[i].Decay = 0;
		Voice[i].Duration = 0;
		Voice[i].Period = 0;
		Voice[i].Counter = 0;
		Voice[i].CounterIncrement = 0;
		Voice[i].Type = VW_UNINIT;
	}
}

/* Initialize sound hardware, sine table, and waveform buffer. */
void Sound_Init(void) {
	SineTable_Init();
	Init_Waveform();
	Init_Voices();
	write_buffer_num = 0; // Start writing to waveform buffer 0

	// amp should already be initialized, just turn on
	Sound_Enable_Amp();

}

void Sound_Enable_Amp(void) {
	AudioAmp_SetVal(NULL);
}

void Sound_Disable_Amp(void) {
	AudioAmp_ClrVal(NULL);
}

void ShortDelay (uint32_t dly) {
  volatile uint32_t t;

	for (t=dly; t>0; t--)
		;
}

/* Simple audio test function using busy-waiting. */
void Play_Tone(void) {
	int i, d=MAX_DAC_CODE>>7, n;
	int p[12] = {813, 1150, 813, 813, 813, 1150, 813, 813, 813, 813, 1150, 1150};
	int c[12] = {800, 283,  800, 800, 800, 283,  800, 800, 800, 800,  283, 283};

#if 0
	for (p=5; p>=1; p--) {
		for (n=0; n<20/p; n++) {
			Play_Sound_Sample((MAX_DAC_CODE>>1)+d);
			Delay(p);
			Play_Sound_Sample((MAX_DAC_CODE>>1)-d);
			Delay(p);
		}
	}
#else
	for (i=0; i<4; i++) {
		n = c[i];
		while (n--) {
			DacLdd1_SetValue(NULL,(MAX_DAC_CODE>>1)+d);
			ShortDelay(p[i]);
			DacLdd1_SetValue(NULL,(MAX_DAC_CODE>>1)-d);
			ShortDelay(p[i]);
		}
		WAIT1_Waitms(40);
	}
#endif
}

int16_t Sound_Generate_Next_Sample (VOICE_T *voice) {
	uint16_t lfsr;
	uint16_t bit;
	int16_t sample;

	switch (voice->Type) {
		case VW_NOISE:
			lfsr = voice->Counter;
			// source code from http://en.wikipedia.org/wiki/Linear_feedback_shift_register
			/* taps: 16 14 13 11; characteristic polynomial: x^16 + x^14 + x^13 + x^11 + 1 */
			bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;
			lfsr =  (lfsr >> 1) | (bit << 15);
			voice->Counter = lfsr;
			sample = (lfsr >> 4) - (MAX_DAC_CODE/2); // scale to get 12-bit value
			break;
		case VW_SQUARE:
			if (voice->Counter < voice->Period/2) {
				sample = MAX_DAC_CODE/2 - 1;
			} else {
				sample = -MAX_DAC_CODE/2;
			}
			voice->Counter++;
			if (voice->Counter == voice->Period) {
				voice->Counter = 0;
			}
			break;
		case VW_SINE:
			sample = SineTable[((voice->Counter)/256)]; // & (NUM_STEPS-1)];
			voice->Counter += voice->CounterIncrement;
			if (voice->Counter > voice->Period * voice->CounterIncrement){
				voice->Counter = 0;
			}
			break;
		default:
			sample = 0;
			break;
	}
	return sample;
}

void Play_Waveform_with_DMA(void) {
	Configure_DMA_For_Playback(Waveform[0], Waveform[1], NUM_WAVEFORM_SAMPLES, 1);
	Start_DMA_Playback();
}

void Configure_DMA_For_Playback(uint16_t * source1, uint16_t * source2, uint32_t count, uint32_t num_playbacks) {

	// Disable DMA channel in order to allow changes
	DMAMUX_PDD_EnableChannel(DMAMUX0_BASE_PTR, DMA_PDD_CHANNEL_0, PDD_DISABLE); /* disable DMA MUX0 */

	Reload_DMA_Source[0] = source1;
	Reload_DMA_Source[1] = source2;
	read_buffer_num = 0;

	Reload_DMA_Byte_Count = count*2;
	DMA_Playback_Count = num_playbacks;

	// clear control register
	DMA_PDD_WriteControlReg(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, 0);

	// enable transfer complete interrupt, src address increment, peripheral request, & cycle steal
	DMA_PDD_EnableTransferCompleteInterrupt(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, PDD_ENABLE);
	DMA_PDD_EnableSourceAddressIncrement(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, PDD_ENABLE);
	DMA_PDD_EnablePeripheralRequest(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, PDD_ENABLE);
	DMA_PDD_EnableCycleSteal(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, PDD_ENABLE);

	// set src and dst data transfer sizes
	DMA_PDD_SetSourceDataTransferSize(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, DMA_PDD_16_BIT);
	DMA_PDD_SetDestinationDataTransferSize(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, DMA_PDD_16_BIT);

	// Enable DMA MUX channel without periodic triggering
	// select TPM0 overflow as trigger
	DMAMUX_PDD_SetChannelSource(DMAMUX0_BASE_PTR, 0, 54);

}

void Start_DMA_Playback() {

	// initialize source and destination pointers
	DMA_PDD_SetSourceAddress(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, (uint32_t) Reload_DMA_Source[read_buffer_num]);
	DMA_PDD_SetDestinationAddress(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, (uint32_t) (&(DAC0->DAT[0])));

	// byte count
	DMA_PDD_SetByteCount(DMA_BASE_PTR, DMA_PDD_CHANNEL_0, Reload_DMA_Byte_Count);

	// Select TPM0 as trigger for DMA
	DMAMUX_PDD_SetChannelSource(DMAMUX0_BASE_PTR, DMA_PDD_CHANNEL_0, 54);

	// Enable DMA
	DMAMUX_PDD_EnableChannel(DMAMUX0_BASE_PTR, DMA_PDD_CHANNEL_0, PDD_ENABLE);

	// start the timer running
  TPM_PDD_ClearOverflowInterruptFlag(TPM0_BASE_PTR);

}
