/*
  Hammer-hit capture, no timestamps.

  Threshold is only used to detect the hit start.
  Pre-trigger audio is always buffered.
*/

#include <PDM.h>

// Audio config
static const int frequency = 16000;
static const char channels = 1;

short sampleBuffer[512];
volatile int samplesRead = 0;

// Detection threshold (RMS of a chunk)
static const int THRESHOLD_RMS = 1500;  // tune for hammer loudness

// Window sizes
static const int PRE_MS  = 1;
static const int POST_MS = 1;

static const int PRE_SAMPLES   = (frequency * PRE_MS) / 1000;   // 800
static const int POST_SAMPLES  = (frequency * POST_MS) / 1000;  // 4000
static const int EVENT_SAMPLES = PRE_SAMPLES + POST_SAMPLES;    // 4800

// Ring buffer for pre-trigger
short preBuffer[PRE_SAMPLES];
int preIndex = 0;
bool preFilled = false;

// Event buffer
short eventBuffer[EVENT_SAMPLES];
int eventIndex = 0;
bool capturing = false;

bool headerPrinted = false;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  PDM.onReceive(onPDMdata);
  if (!PDM.begin(channels, frequency)) while (1);
}

void loop() {
  if (!samplesRead) return;

  int n = samplesRead;
  samplesRead = 0;

  // Compute RMS for this chunk
  int64_t sumSq = 0;
  for (int i = 0; i < n; i++) {
    int32_t s = sampleBuffer[i];
    sumSq += (int64_t)s * s;
  }
  int32_t meanSq = (n > 0) ? (int32_t)(sumSq / n) : 0;
  int rms = (int)sqrt((float)meanSq);

  // Update pre-trigger ring buffer continuously
  for (int i = 0; i < n; i++) {
    preBuffer[preIndex++] = sampleBuffer[i];
    if (preIndex >= PRE_SAMPLES) {
      preIndex = 0;
      preFilled = true;
    }
  }

  // Trigger start when hammer hit crosses threshold
  if (!capturing && preFilled && rms >= THRESHOLD_RMS) {
    capturing = true;
    eventIndex = 0;

    // Copy pre-trigger in correct order
    for (int i = 0; i < PRE_SAMPLES; i++) {
      int idx = (preIndex + i) % PRE_SAMPLES;
      eventBuffer[eventIndex++] = preBuffer[idx];
    }
  }

  // If capturing, append samples until window is full
  if (capturing) {
    for (int i = 0; i < n && eventIndex < EVENT_SAMPLES; i++) {
      eventBuffer[eventIndex++] = sampleBuffer[i];
    }

    // Window complete -> print raw samples only
    if (eventIndex >= EVENT_SAMPLES) {
      if (!headerPrinted) {
        Serial.println("sample");
        headerPrinted = true;
      }

      for (int i = 0; i < EVENT_SAMPLES; i++) {
        Serial.println(eventBuffer[i]);
      }

      Serial.println(); // blank line between hits
      capturing = false;
    }
  }
}

void onPDMdata() {
  int bytesAvailable = PDM.available();
  PDM.read(sampleBuffer, bytesAvailable);
  samplesRead = bytesAvailable / 2;
}
