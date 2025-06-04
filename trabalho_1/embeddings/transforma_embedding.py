# -*- coding: utf-8 -*-
"""
Script para extrair embeddings faciais com DeepFace (modelo Facenet)
e salvar em um arquivo JSON.
"""

# Instale a biblioteca DeepFace se necessário:
# !pip install deepface

from deepface import DeepFace
from PIL import Image
import os
import json

# Caminho para a pasta com as imagens
IMAGES_DIR = r"F:\1000 faces"

# Arquivo de saída
OUTPUT_JSON = "embeddings.json"

# Lista para armazenar os resultados
embeddings = []

# Verifica se o diretório existe
if not os.path.exists(IMAGES_DIR):
    raise FileNotFoundError(f"Pasta não encontrada: {IMAGES_DIR}")

# Percorre os arquivos da pasta
for filename in os.listdir(IMAGES_DIR):
    if filename.lower().endswith(('.jpg', '.jpeg', '.png')):
        img_path = os.path.join(IMAGES_DIR, filename)
        print(f"🔍 Processando: {filename}")

        try:
            # Nome da pessoa (antes da extensão)
            nome = os.path.splitext(filename)[0]

            # Extrair embedding
            result = DeepFace.represent(img_path=img_path, model_name='Facenet', enforce_detection=False)

            if result:
                embeddings.append({
                    "nome": nome,
                    "embedding": result[0]["embedding"]
                })

        except Exception as e:
            print(f"❌ Erro ao processar {filename}: {e}")

# Salva os embeddings em JSON
with open(OUTPUT_JSON, "w") as f:
    json.dump(embeddings, f, indent=2)

print(f"\n✅ Total de embeddings salvos: {len(embeddings)}")
print(f"📁 Arquivo criado: {OUTPUT_JSON}")