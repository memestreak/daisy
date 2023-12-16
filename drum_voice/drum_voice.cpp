#include "daisy_patch_sm.h"
#include "daisysp.h"

using namespace daisy;
using namespace patch_sm;
using namespace daisysp;

SyntheticBassDrum bd;
//Metro             tick;

DaisyPatchSM hw;

/**
 * Called back very frequently.
 */
void AudioCallback(AudioHandle::InputBuffer in,
                   AudioHandle::OutputBuffer out,
                   size_t size) {
  hw.ProcessAllControls();
  // float distortion = hw.GetAdcValue(CV_1);
  // float decay      = hw.GetAdcValue(CV_5);

  bool triggered = hw.gate_in_1.Trig();

  for(size_t i = 0; i < size; i++) {
    // float t = tick.Process();
    // bool triggered = hw.gate_in_1.State();
    // float distortion = hw.GetAdcValue(CV_1);
    // float decay = hw.GetAdcValue(CV_5);
	// if (triggered) {
	//   bd.SetAccent(0.9);
	//   bd.SetDirtiness(distortion);
	//   bd.SetDecay(decay);
	// }
    // if (triggered) {
    //   bd.SetAccent(0.9);
    //   bd.SetDirtiness(distortion);
    //   bd.SetDecay(decay);
    // }
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

int main(void)
{
	hw.Init();
	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

    bd.Init(48000);
    bd.SetFreq(50.f);
    bd.SetDirtiness(.5f);
    bd.SetDecay(.1f);
    bd.SetFmEnvelopeAmount(.6f);

    // tick.Init(2.f, 48000);

	hw.StartAudio(AudioCallback);

  	while(1) {}
}