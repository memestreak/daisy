#include <stdio.h>
#include <string.h>
#include <string>
#include "daisy_seed.h"
#include "dev/oled_ssd130x.h"

using namespace daisy;

/**
 * Typedef the OledDisplay to make syntax cleaner below.
 * This is a 4Wire SPI Transport controlling an 128x32 sized SSDD1306.
*/
using MyOledDisplay = OledDisplay<SSD130x4WireSpi128x32Driver>;

DaisySeed hw;
Encoder encoder;
MyOledDisplay display;
volatile int encoder_value = 0;
volatile int click_sum = 0;

// Pin ordering for my specific encoder is: Gnd, Vcc, SW, DT, CLK.
constexpr Pin ENC_CLICK_PIN = seed::D27;  // SW
constexpr Pin ENC_A_PIN     = seed::D28;  // DT
constexpr Pin ENC_B_PIN     = seed::D14;  // CLK

// Pin ordering for my SPI OLED device along with th Daisy Seed connection:
//
//    | Device | Daisy Seed
//    |--------+------------
//    | GND    | DGND
//    | VCC    | 3V3 Digital
//    | CLK    | D8 SPI SCK
//    | MOSI   | D10 SPI MOSI
//    | RES    | D11
//    | DC     | D9 SPI MISO ?
//    | CS     | D7 SPI1 CS

constexpr Pin OLED_DC_PIN = seed::D9;     // Data/Command
constexpr Pin OLED_RESET_PIN = seed::D11;

void AudioCallback(AudioHandle::InterleavingInputBuffer in,
                   AudioHandle::InterleavingOutputBuffer out,
                   size_t size) {
  encoder.Debounce();
  click_sum += encoder.RisingEdge();
  encoder_value += encoder.Increment();
}

int main(void) {
  hw.Configure();
  hw.Init();
  hw.SetAudioBlockSize(4);
  encoder.Init(ENC_A_PIN, ENC_B_PIN, ENC_CLICK_PIN);

  // Configure the Display
  MyOledDisplay::Config disp_cfg;
  disp_cfg.driver_config.transport_config.pin_config.dc = OLED_DC_PIN;
  disp_cfg.driver_config.transport_config.pin_config.reset = OLED_RESET_PIN;

  display.Init(disp_cfg);
  char strbuff[128];

  hw.StartAudio(AudioCallback);

  while(true) {
    System::Delay(100);
    sprintf(strbuff, "e:%d c:%d", encoder_value, click_sum);
    display.Fill(true);
    display.SetCursor(0, 0);
    display.WriteString(strbuff, Font_11x18, /* on= */ false);
    display.Update();
  }
}
