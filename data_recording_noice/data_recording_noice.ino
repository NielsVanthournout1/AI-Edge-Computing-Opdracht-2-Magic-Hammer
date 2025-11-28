/*
  ============================================================================
  Hammer-Hit Capture System – Parameter Guide
  ============================================================================

  DIT IS BELANGRIJK:  
  Onderstaande instellingen bepalen hoe gevoelig het systeem is,  
  hoe het omgaat met ruis, en hoe hits worden gedetecteerd.  
  Pas deze parameters aan afhankelijk van:
    - jouw microfoon
    - omgeving (stil / lawaai)
    - afstand van hamer tot microfoon
    - materiaal dat je test
    - hoe hard je slaat

  ---------------------------------------------------------------------------
  1. PRE_MS  (standaard: 50 ms)
     Hoeveel audio vóór de slag je wil opslaan.
     - Te klein: je mist het begin van de slag.
     - Te groot: onnodig veel data.
     50 ms is perfect voor snelle impacts (hamer).

  2. POST_MS (standaard: 250 ms)
     Hoeveel audio NA de slag je wil bewaren.
     Verschillende materialen hebben andere ‘decay’ / resonantie:
       - metaal: lange ring (~200–400 ms)
       - hout: kortere decay
       - plastic: iets langer
     Voor materiaalherkenning is 250 ms meestal voldoende.
     Meer detail nodig? Verhoog tot 300–400 ms.

  ---------------------------------------------------------------------------
  3. WARMUP_MS (standaard: 400 ms)
     De PDM-microfoon maakt een *startup pop* en instabiele waarden bij opstart.
     Tijdens warmup worden triggers genegeerd.
     Verhoog naar 600–800 ms als jouw mic lang nodig heeft om te stabiliseren.

  ---------------------------------------------------------------------------
  4. NOISE_CAL_MS (standaard: 600 ms)
     Tijdens deze periode wordt het achtergrondgeluid gemeten.
     Het systeem berekent een gemiddelde RMS waarde van ruis:

        noiseRms = gemiddelde kamer-ruis

     Hierdoor werkt het systeem in stille én lawaaierige omgevingen.

     - IN EEN STILLE KAMER:
         noiseRms is laag → threshold óók laag
     - IN EEN DRUKKE WERKPLAATS:
         noiseRms is hoger → threshold automatisch hoger

     Pas aan:
       - Korter (300 ms) → als je snel wil starten
       - Langer (800–1200 ms) → als ruis erg wisselt

  ---------------------------------------------------------------------------
  5. THRESHOLD_MULT (standaard: 4.0)
     De echte detectie-threshold wordt zo berekend:

         dynamicThreshold = noiseRms * THRESHOLD_MULT

     Richtlijnen:
       - TE VEEL FALSE TRIGGERS?
           Verhoog naar 5.0, 6.0 of zelfs 8.0
       - SLAAT HIJ NIET AAN ALS JE SLAG GEEFT?
           Verlaag naar 3.0 of 2.5

     AANRECHTSMETHODE (snelle manier):
       1. Zet THRESHOLD_MULT op 5.0.
       2. Test een paar slagen.
       3. Als hij niets detecteert → omlaag naar 4.0 of 3.0.
       4. Als hij nog steeds te snel false-triggert → omhoog naar 6.0.

  ---------------------------------------------------------------------------
  6. MIN_THRESHOLD (standaard: 1200)
     De minimumwaarde waartoe de threshold mag zakken.  
     Dit voorkomt dat het systeem TE gevoelig wordt in een extreem stille kamer.

     Als je systeem zelfs in stilte te snel triggert:
       → verhoog naar 1500–2000

     Als jouw microfoon zwakker is of je hamer minder luid:
       → verlaag naar 800–1000

  ---------------------------------------------------------------------------
  7. REQUIRED_CONSECUTIVE (standaard: 2)
     Het aantal opeenvolgende luide chunks dat nodig is om een hit te bevestigen.
     Dit is een belangrijke bescherming tegen ruispieken.

       - Bij 1: snelle maar gevoelig voor random pieken
       - Bij 2: veilige keuze (aanbevolen)
       - Bij 3: nóg stabieler, maar kans dat je zachte slagen mist

  ---------------------------------------------------------------------------
  8. LOCKOUT_MS (standaard: 300 ms)
     Na een detectie wordt het systeem een tijdje ongevoelig.
     Dit voorkomt:
       - re-triggers tijdens de ‘ring’ van materiaal
       - dubbele detecties bij één slag

     Bij metalen objecten met lange resonantie:
       → verhoog naar 400–600 ms

     Bij korte, droge slagen (hout, rubber):
       → 200–300 ms is genoeg

  ---------------------------------------------------------------------------
  AANPASSINGS-RICHTLIJNEN SAMENVATTING:
  -------------------------------------
  - Slaat hij TE VAAK vals aan?
        → THRESHOLD_MULT omhoog
        → MIN_THRESHOLD omhoog
        → REQUIRED_CONSECUTIVE verhogen
        → LOCKOUT_MS verhogen

  - Slaat hij juist NIET aan bij echte slagen?
        → THRESHOLD_MULT omlaag
        → MIN_THRESHOLD omlaag
        → REQUIRED_CONSECUTIVE verlagen (naar 1)
        → microfoon dichterbij

  - Wil je meer detail per slag?
        → POST_MS verhogen (tot 300–400 ms max)

  - Wil je kleinere datasets?
        → POST_MS verlagen (maar minimaal 150 ms behouden voor AI-materiaalherkenning)

  ============================================================================
*/

#include <PDM.h>

// Audio config
static const int frequency = 16000;
static const char channels = 1;

short sampleBuffer[512];
volatile int samplesRead = 0;

// Window sizes (ms)
static const int PRE_MS  = 50;   // audio before hit
static const int POST_MS = 250;  // audio after hit

static const int PRE_SAMPLES   = (frequency * PRE_MS) / 1000;   // 800
static const int POST_SAMPLES  = (frequency * POST_MS) / 1000;  // 4000
static const int EVENT_SAMPLES = PRE_SAMPLES + POST_SAMPLES;    // 4800

// Startup and noise calibration
static const int WARMUP_MS    = 400;   // ignore triggers at start
static const int NOISE_CAL_MS = 600;   // measure background noise

static const float THRESHOLD_MULT = 4.0f; // threshold = noiseRms * mult
static const int MIN_THRESHOLD    = 1200; // minimum RMS threshold

// Trigger robustness
static const int REQUIRED_CONSECUTIVE = 2;  // number of loud chunks needed
static const int LOCKOUT_MS           = 300; // ignore new hits after a trigger

// Pre-trigger ring buffer
short preBuffer[PRE_SAMPLES];
int preIndex = 0;
bool preFilled = false;

// Event buffer (PRE + POST)
short eventBuffer[EVENT_SAMPLES];
int eventIndex = 0;
bool capturing = false;

// State for noise calibration and triggering
unsigned long startMs;
bool noiseCalDone = false;
int64_t noiseSumRms = 0;
int noiseChunks = 0;
int noiseRms = 0;
int loudStreak = 0;
unsigned long lastTriggerMs = 0;

// CSV header flag
bool headerPrinted = false;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  PDM.onReceive(onPDMdata);
  if (!PDM.begin(channels, frequency)) {
    while (1);
  }

  startMs = millis();
}

void loop() {
  if (!samplesRead) return;

  int n = samplesRead;
  samplesRead = 0;

  // Compute RMS of this chunk
  int64_t sumSq = 0;
  for (int i = 0; i < n; i++) {
    int32_t s = sampleBuffer[i];
    sumSq += (int64_t)s * s;
  }
  int32_t meanSq = (n > 0) ? (int32_t)(sumSq / n) : 0;
  int rms = (int)sqrt((float)meanSq);

  unsigned long now = millis();

  // Always update pre-trigger ring buffer
  for (int i = 0; i < n; i++) {
    preBuffer[preIndex++] = sampleBuffer[i];
    if (preIndex >= PRE_SAMPLES) {
      preIndex = 0;
      preFilled = true;
    }
  }

  // Warmup phase: ignore triggers
  if (now - startMs < WARMUP_MS) {
    return;
  }

  // Noise calibration phase
  if (!noiseCalDone) {
    noiseSumRms += rms;
    noiseChunks++;

    if (now - startMs >= (WARMUP_MS + NOISE_CAL_MS)) {
      noiseRms = (noiseChunks > 0) ? (int)(noiseSumRms / noiseChunks) : 1;
      if (noiseRms < 1) noiseRms = 1;
      noiseCalDone = true;
    }
    return; // do not trigger during calibration
  }

  // Dynamic threshold
  int dynamicThreshold = (int)(noiseRms * THRESHOLD_MULT);
  if (dynamicThreshold < MIN_THRESHOLD) dynamicThreshold = MIN_THRESHOLD;

  // Lockout: ignore triggers right after a hit
  if (!capturing && (now - lastTriggerMs < LOCKOUT_MS)) {
    return;
  }

  // Trigger logic
  if (!capturing && preFilled) {
    if (rms >= dynamicThreshold) {
      loudStreak++;
    } else {
      loudStreak = 0;
    }

    if (loudStreak >= REQUIRED_CONSECUTIVE) {
      capturing = true;
      eventIndex = 0;
      loudStreak = 0;
      lastTriggerMs = now;

      // Copy PRE window into eventBuffer in correct order
      for (int i = 0; i < PRE_SAMPLES; i++) {
        int idx = (preIndex + i) % PRE_SAMPLES;
        eventBuffer[eventIndex++] = preBuffer[idx];
      }
    }
  }

  // Capture POST samples
  if (capturing) {
    for (int i = 0; i < n && eventIndex < EVENT_SAMPLES; i++) {
      eventBuffer[eventIndex++] = sampleBuffer[i];
    }

    // Window complete -> print ONE CSV row
    if (eventIndex >= EVENT_SAMPLES) {
      // Print header once: s0,s1,...,s4799
      if (!headerPrinted) {
        Serial.print("s0");
        for (int i = 1; i < EVENT_SAMPLES; i++) {
          Serial.print(",s");
          Serial.print(i);
        }
        Serial.println();
        headerPrinted = true;
      }

      // Print one hit as a single line
      Serial.print(eventBuffer[0]);
      for (int i = 1; i < EVENT_SAMPLES; i++) {
        Serial.print(',');
        Serial.print(eventBuffer[i]);
      }
      Serial.println();   // end of this sample (window)

      capturing = false;
    }
  }
}

// PDM callback (ISR): only reading into buffer
void onPDMdata() {
  int bytesAvailable = PDM.available();
  PDM.read(sampleBuffer, bytesAvailable);
  samplesRead = bytesAvailable / 2; // 2 bytes per sample
}
