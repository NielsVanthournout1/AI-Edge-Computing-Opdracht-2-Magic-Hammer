# AI-Edge-Computing-Opdracht-2-Magic-Hammer

## 🎯 Projectbeschrijving  
In deze opdracht, opgezet in het kader van **AI Edge Computing**, onderzoeken we het gebruik van edge-gebaseerde AI-modellen voor realtime perceptie. Het project draagt de naam **“Magic Hammer”** omdat de toepassing de metafoor van een magische hamer gebruikt: op het juiste moment en op de juiste plaats ‘hamert’ de AI toe met een beslissing.

Het experiment is beschikbaar als een interactieve notebook:  
[Bekijk de Google Colab notebook](https://colab.research.google.com/drive/1odDQebzn6BkJUVjjhlwe06yY2V6S1FD2?usp=sharing)

## 📋 Inhoud van de repository  
- `MagicHammer.ipynb` (of `AI-Edge-Computing-Opdracht-2-Magic-Hammer.ipynb`): de hoofd-Colab notebook waarin het experiment is uitgewerkt  
- `data/` (optioneel): dataset(s) die lokaal worden gebruikt of voor edge-testen  
- `models/`: opgeslagen modellen of conversies voor edge-gebruik (bv. TensorRT, ONNX, tflite)  
- `README.md`: dit bestand  
- `requirements.txt` (optioneel): Python-packages die gebruikt worden in de notebook  
- `edge_scripts/` (optioneel): scripts om het model op een edge-device te deployen  
- `results/`: experimentresultaten, grafieken en evaluaties  

## 🧠 Doelstellingen  
* Onderzoeken hoe AI-modellen kunnen worden geoptimaliseerd voor edge-deployments.  
* Evalueren van realtime prestaties (latentie, throughput, resourcegebruik) op een edge-device (bijv. Raspberry Pi, NVIDIA Jetson, …).  
* Visualiseren of demonstreren van de werking van het edge-gebaseerde model (bijv. video, live feed, detectie, classificatie).  
* Vergelijken met een “klassieke” cloud-gebaseerde aanpak (optioneel) qua prestaties of energieverbruik.

## 🚀 Installatie & gebruik  
1. Open de notebook via de hierboven gelinkte Colab-URL.  
2. Zorg ervoor dat de runtime is ingesteld op `GPU` of `TPU` indien nodig (Colab > Runtime > Change runtime type).  
3. Installeer eventueel vereiste pakketten via:  
   ```bash
   pip install -r requirements.txt
