from deepface import DeepFace
from PIL import Image
import os
import json

IMAGES_DIR = r"F:\1000 faces"
OUTPUT_JSON = "embeddings.json"

embeddings = []

if not os.path.exists(IMAGES_DIR):
    raise FileNotFoundError(f"Pasta não encontrada: {IMAGES_DIR}")

for filename in os.listdir(IMAGES_DIR):
    if filename.lower().endswith(('.jpg', '.jpeg', '.png')):
        img_path = os.path.join(IMAGES_DIR, filename)
        print(f"Processando: {filename}")

        try:

            nome = os.path.splitext(filename)[0]
           
            result = DeepFace.represent(img_path=img_path, model_name='Facenet', enforce_detection=False)

            if result:
                embeddings.append({
                    "nome": nome,
                    "embedding": result[0]["embedding"]
                })

        except Exception as e:
            print(f"Erro ao processar {filename}: {e}")

with open(OUTPUT_JSON, "w") as f:
    json.dump(embeddings, f, indent=2)

print(f"\nTotal de embeddings salvos: {len(embeddings)}")
print(f"Arquivo criado: {OUTPUT_JSON}")