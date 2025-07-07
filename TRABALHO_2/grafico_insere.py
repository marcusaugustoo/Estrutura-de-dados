import matplotlib.pyplot as plt

# Dados de tempo de inserção (obtidos do gprof)
tempos_insercao = {
    'insere6100()': 1.45,  # Tempo para iniciar com 6100 buckets
    'insere1000()': 1.90   # Tempo para iniciar com 1000 buckets (com redimensionamento)
}

funcoes = list(tempos_insercao.keys())
tempos = list(tempos_insercao.values())

# Criação do gráfico de barras
plt.figure(figsize=(8, 6))
bars = plt.bar(funcoes, tempos, color=['skyblue', 'lightcoral'])

# Adicionar os valores exatos nas barras
for bar in bars:
    yval = bar.get_height()
    plt.text(bar.get_x() + bar.get_width()/2, yval + 0.05, round(yval, 2), ha='center', va='bottom')

# Personalização do gráfico
plt.title('Tempo de Inserção: Tabela Hash com Diferentes Capacidades Iniciais')
plt.xlabel('Função de Inserção')
plt.ylabel('Tempo de Execução (segundos)')
plt.ylim(0, max(tempos) * 1.2) # Ajusta o limite Y para ter espaço para os rótulos
plt.grid(axis='y', linestyle='--', alpha=0.7) # Grade apenas no eixo Y

# Mostrar o gráfico
plt.tight_layout()
plt.show()