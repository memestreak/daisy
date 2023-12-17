/**
 *  A simple trigger-based drum voice.
 *
 * Gate 1: Trigger
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
DaisyPatchSM hw;

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
  bool triggered = hw.gate_in_1.Trig();

  for(size_t i = 0; i < size; i++) {
    if (triggered) {
      bd.SetAccent(0.9);
      bd.SetDirtiness(hw.GetAdcValue(CV_1));
      bd.SetDecay(hw.GetAdcValue(CV_5));
      bd.SetFmEnvelopeAmount(hw.GetAdcValue(CV_2));
      bd.SetFmEnvelopeDecay(hw.GetAdcValue(CV_6));
      bd.Trig();
    }
    out[0][i] = out[1][i] = bd.Process();
    triggered = false;
  }
  triggered = false;
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
  hw.StartAudio(AudioCallback);
  while(1) {}
}
