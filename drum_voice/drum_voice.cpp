/**
 *  A simple trigger-based drum voice.
 *
 * Gate 1: Trigger
 * Gate 2: Trigger
 *
 * CV 1: Grit
 * CV 2: FM envelope
 * CV 5: Decay
 * CV 6: FM envelope decay
 */

#include "daisy_patch_sm.h"
#include "daisysp.h"

using namespace daisy;
using namespace patch_sm;
using namespace daisysp;

SyntheticBassDrum bd;
AnalogSnareDrum sd;

DaisyPatchSM hw;

// float cv1 = 0.f;
const int SELECTOR_CV = CV_4;

// Bass drum stuff
const int BASS_DRUM_DIRT_CV = CV_1;
const int BASS_DRUM_DECAY_CV = CV_5;
const int BASS_DRUM_FM_ENV_AMOUNT_CV = CV_2;
const int BASS_DRUM_FM_ENV_DECAY_CV = CV_6;

// Snare drum stuff
const int SNARE_DRUM_FREQUENCY_CV = CV_1;
const int SNARE_DRUM_TONE_CV = CV_5;
const int SNARE_DRUM_DECAY_CV = CV_2;
const int SNARE_DRUM_SNAPPY_CV = CV_6;

bool ShouldUpdateBassDrumParams() {
  return hw.GetAdcValue(SELECTOR_CV) <= 0.5;
}

bool ShouldUpdateSnareDrumParams() {
  return hw.GetAdcValue(SELECTOR_CV) > 0.5;
}

void SetBassDrumParams() {
  bd.SetDirtiness(hw.GetAdcValue(BASS_DRUM_DIRT_CV));
  bd.SetDecay(hw.GetAdcValue(BASS_DRUM_DECAY_CV));
  bd.SetFmEnvelopeAmount(hw.GetAdcValue(BASS_DRUM_FM_ENV_AMOUNT_CV));
  bd.SetFmEnvelopeDecay(hw.GetAdcValue(BASS_DRUM_FM_ENV_DECAY_CV));
  bd.SetAccent(0.9);
}

void SetSnareDrumParams() {
  //sd.SetFreq(hw.GetAdcValue(SNARE_DRUM_FREQUENCY_CV));
  sd.SetTone(hw.GetAdcValue(SNARE_DRUM_TONE_CV));
  sd.SetDecay(hw.GetAdcValue(SNARE_DRUM_DECAY_CV));
  sd.SetSnappy(hw.GetAdcValue(SNARE_DRUM_SNAPPY_CV));
}

/**
 * Processes audio blocks.
 *
 * At 48Khz and a block size of 4, this is called at 12Khz, meaning that
 * we have 83 ms to perform our computations.
 */
void AudioCallback(AudioHandle::InputBuffer in,
                   AudioHandle::OutputBuffer out,
                   size_t size) {
  hw.ProcessAllControls();
  bool bd_triggered = hw.gate_in_1.Trig();
  bool sd_triggered = hw.gate_in_2.Trig();

  for(size_t i = 0; i < size; i++) {
    if (bd_triggered) {
      bd_triggered = false;

      if (ShouldUpdateBassDrumParams()) {
        SetBassDrumParams();
      }

      bd.Trig();

    }
    if (sd_triggered) {
      sd_triggered = false;

      if (ShouldUpdateSnareDrumParams()) {
        SetSnareDrumParams();
      }

      sd.Trig();
    }

    out[0][i] = out[1][i] = bd.Process() + sd.Process();

  }
}

int main(void) {
  hw.Init();
  hw.SetAudioBlockSize(4);  // Number of samples handled per callback
  hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

  bd.Init(48000);
  bd.SetFreq(50.f);
  bd.SetDirtiness(.5f);
  bd.SetDecay(.1f);
  bd.SetFmEnvelopeAmount(.6f);

  sd.Init(48000);

  hw.StartLog();
  hw.StartAudio(AudioCallback);

  while (1)
  {
    // hw.PrintLine("cv1: " FLT_FMT3, FLT_VAR3(cv1));
    // System::Delay(1000);
  }
}
