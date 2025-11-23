# Hammer Hit Data Collection README

This is a short overview of how the hammer-hit recording system works. For full, detailed explanations, refer directly to comments inside the Arduino code file.

---

## Overview

This system records hammer hits using a PDM microphone on an Arduino. The code detects a hit, then outputs a fixed-size block of **4800 raw audio samples** (pre + post trigger). Each output is stored line-by-line in a CSV file. For all detailed explanations, see comments directly inside the Arduino code.

---

## Consistent Sample Size

Each hammer hit always produces **4800 samples**, because both the pre-trigger and post-trigger windows are fixed in the Arduino code. This guarantees consistent dataset size suitable for ML training.

The only requirement is that your logging software does not lose data.

## Data Recording Using PuTTY (Recommended)

PuTTY reliably logs serial output into a file and prevents data loss.

### Steps

1. Install PuTTY: [https://www.putty.org/](https://www.putty.org/)
2. Open PuTTY → choose **Serial**

   * Serial line: your Arduino COM port
   * Speed: same as Arduino (e.g., 9600)
3. Before opening connection:

   * Go to **Session → Logging**
   * Select **All session output**
   * **Uncheck "Include header"** (important for clean CSV)
   * Choose a filename (e.g., wood.csv)
4. Click **Open** to start logging.
5. Perform hammer hits.
6. Close PuTTY to finish logging.

## Tips for Clean Dataset Collection

* Record **each material in its own file**.
* Keep the recording environment consistent.
* Perform strikes with different intensities.
* Keep microphone–target distance fixed.
* Always reset PuTTY logging before switching materials.

## Summary

* Arduino outputs fixed-size hit samples.
* All detailed explanations are in the Arduino code.
* PuTTY reliably records data to CSV.
* Ideal for building datasets for ML.

Sample Size Guarantee
Every hammer hit produces exactly **4800 samples** because:

* Pre-trigger window = fixed size
* Post-trigger window = fixed size
* Total window = fixed size

This ensures a clean and stable dataset ready for ML training.

The only requirement is that your logging tool **does not lose serial data**. Using PuTTY solves this.

---

## Summary

* The code outputs fixed-sized hit samples (4800 values).
* Thresholding only triggers on real hammer impacts.
* Logging through PuTTY guarantees no lost data.
* Each material is recorded into its own CSV file.
* Perfect for feeding into a TinyML model later.

If you need an automated Python script to split each detected hit into separate files (wood_001.csv, wood_002.csv, ...), it can be added as well.
