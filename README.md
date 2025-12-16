# Magic Hammer — EDA

Een compacte, moderne README voor het `Magic_Hammer` EDA‑project.

**Wat is dit**
- Kleine toolkit (Jupyter Notebook) voor exploratory data analysis van hamerslagen die als CSV worden geëxporteerd door het capture‑systeem.
- Bevat helpers om CSV-rijen (elke rij = één hit, 4800 samples @16kHz) te laden, basisstatistieken te berekenen, en plots te genereren.

**Belangrijk**
- De capture firmware schrijft één hit per CSV‑rij met PRE + POST windows (PRE_MS=50, POST_MS=250) → 4800 samples per hit.
- Bewaar altijd de ruwe data; preprocessing is optioneel maar aanbevolen voor ML.

**Bestanden**
- `EDA.ipynb` — het interactieve notebook met loader, EDA plots (waveforms, amplitude KDE, FFT, RMS) en cell‑commentaar.
- `Data/` — raw CSV bestanden (bijv. `blik.csv`, `steen.csv`).
- `requirements.txt` — basis Python dependencies (numpy, pandas, matplotlib, seaborn).
- `plots/` — door het notebook gegenereerde PNG's (worden aangemaakt bij uitvoeren).

**Snelle start (Windows / PowerShell)**

1) Maak en activeer een virtuele omgeving

```powershell
python -m venv .venv
.\.venv\Scripts\Activate.ps1
```

2) Installeer dependencies

```powershell
python -m pip install -r requirements.txt
```

3) Open en draai het notebook (gebruik Jupyter Lab/Notebook of VS Code)

```powershell
jupyter lab
# of
jupyter notebook
```

4) In `EDA.ipynb`:
- Voer cell 3 (imports) en cell 5 (laden & samenvatting) uit om data in te laden.
- Draai visualisatiecellen om plots te inspecteren. Er is ook een cel die `plots/` afbeeldingsbestanden opslaat.

**Aanbevolen minimale preprocessing**
- Controleer dat elke rij exact `4800` samples heeft; log afwijkingen.
- Verwijder DC offset: `sig = sig - sig.mean()`.
- Bewaar raw RMS/peak waarden en maak een genormaliseerde kopie (`peak` of `RMS`) als je plots wilt vergelijken.
- Optioneel: high-pass <50Hz en align op piek (centreer en crop rond de maximale impact sample).


