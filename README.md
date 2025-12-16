
# Magic Hammer — EDA & Preprocessing

Compacte README voor het `Magic_Hammer` project: interactieve EDA én een preprocessing pipeline die Mel-Frequency Energy (MFE) features berekent, normaliseert en visualiseert.

**Wat doet dit repo**
- Bevat notebooks om raw CSV hits te verkennen (`EDA.ipynb`) en een preprocessing pipeline (`prepro.ipynb`) die:
	- raw CSV's uit `Data/` leest (elke rij = 1 hit),
	- MFE (Mel-Frequency Energy) features extraheert met `librosa`,
	- verwerkte bestanden opslaat in `processed/` (bestandsnaam krijgt suffix `_processed.csv`),
	- genormaliseerde versies opslaat in `normalized/` en een `scaler.pkl` bewaart.

**Belangrijke directories & bestandsconventies**
- `Data/` — raw CSV bestanden (bijv. `blik.csv`, `steen.csv`).
- `processed/` — MFE features per hit worden hier opgeslagen met de naam `<origineel>_processed.csv`.
- `normalized/` — genormaliseerde features (StandardScaler) en `scaler.pkl`.
- `plots/` — optioneel: gegenereerde figuren.

**Hoe de preprocessing werkt (kort)**
1. `prepro.ipynb` leest alle CSV's in `Data/` en berekent per rij (hit) een MFE-matrix (shape = `n_mels x time_frames`).
2. Elke MFE-matrix wordt geflattened en per hit als rij in een CSV geschreven naar `processed/<name>_processed.csv`.
3. Alle `processed/*_processed.csv` bestanden worden geladen en samengevoegd om een `StandardScaler` te fitten.
4. Geschaalde resultaten worden weggeschreven naar `normalized/<name>_processed.csv` en de scaler wordt opgeslagen als `normalized/scaler.pkl`.

**Visualisatie**
- `prepro.ipynb` bevat compacte, overzichtelijke visualisaties:
	- overlappende raw waveforms per materiaal (zodat je direct verschil tussen materialen ziet),
	- overlappende MFE-gemiddelde per mel-band,
	- totale MFE-energie per materiaal (bar chart),
	- per-materiaal pipeline-plots (raw → preprocessed → MFE → normalized).

**Snelle start (Windows / PowerShell)**

1) Maak en activeer een virtuele omgeving

```powershell
python -m venv .venv
.\\.venv\\Scripts\\Activate.ps1
```

2) Installeer dependencies (voeg `librosa`, `scikit-learn`, `joblib` toe aan `requirements.txt` als nodig)

```powershell
python -m pip install -r requirements.txt
```

3) Start Jupyter en open `prepro.ipynb`

```powershell
jupyter lab
# of
jupyter notebook
```

4) In `prepro.ipynb` voer de cells van boven naar beneden uit:
- Sectie **Setup**: controleer `DATA_DIR`, `PROCESSED_DIR`, `NORMALIZED_DIR` en MFE-parameters.
- Sectie **Verwerking**: rekent MFE per hit en schrijft `processed/<name>_processed.csv`.
- Sectie **Normalisatie**: fit en apply `StandardScaler`, schrijft `normalized/<name>_processed.csv` en `normalized/scaler.pkl`.
- Sectie **Visualisatie**: draait overzichtsplots met overlapping en per-materiaal pipeline views.

**Tips & opmerkingen**
- Zorg dat alle `processed/*_processed.csv` dezelfde feature-dimensie hebben; anders faalt het samenvoegen.
- Bij grote datasets: overweeg dtype `float32` en batch-wise processing om geheugen te sparen.
- Bewaar `normalized/scaler.pkl` als referentie en versieer het wanneer je de dataset uitbreidt.

---

Als je wilt kan ik `requirements.txt` bijwerken met de exacte pakketten die `prepro.ipynb` gebruikt en een korte run-instructie toevoegen. Laat het weten!


