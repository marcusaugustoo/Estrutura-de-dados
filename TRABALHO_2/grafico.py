import matplotlib.pyplot as plt

# Taxas de ocupação (%)
taxas = [10, 20, 30, 40, 50, 60, 70, 80, 90, 99]

# Tempos de execução em segundos (dados do gprof)
tempos_simples = [0.265, 0.240, 0.310, 0.750, 0.760, 0.840, 0.790, 0.700, 0.780, 0.800]
tempos_duplo   = [0.265, 0.240, 0.310, 0.510, 0.520, 0.520, 0.570, 0.600, 0.550, 0.640]

# Plotagem
plt.figure(figsize=(10, 6))
plt.plot(taxas, tempos_simples, marker='o', linestyle='-', label='Hash Simples')
plt.plot(taxas, tempos_duplo, marker='s', linestyle='--', label='Hash Duplo')

# Personalização
plt.title('Tempo de Busca vs Taxa de Ocupação')
plt.xlabel('Taxa de Ocupação (%)')
plt.ylabel('Tempo de Execução (segundos)')
plt.grid(True)
plt.legend()
plt.xticks(taxas)
plt.tight_layout()

# Exibir o gráfico
plt.show()
