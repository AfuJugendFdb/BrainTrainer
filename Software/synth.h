#ifndef _SYNTH_H
#define _SYNTH_H

void synth_init(void);
void playTone(uint8_t octave, uint8_t tone);
void playSound(uint16_t value);
void toneOff(void);

#endif
