import argparse
import pandas as pd
import matplotlib.pyplot as plt

# cria um objeto ArgumentParser
parser = argparse.ArgumentParser()

# define o argumento para o nome do arquivo
parser.add_argument("filename", help="nome do arquivo CSV")

# faz o parsing dos argumentos da linha de comando
args = parser.parse_args()

# lê o arquivo CSV e armazena os dados em um DataFrame
df = pd.read_csv(args.filename)

# cria os gráficos dos valores e dos erros lado a lado
fig, axs = plt.subplots(1, 2, figsize=(12, 6))

axs[0].plot(df['Value'], df['Result'], color='blue')
axs[0].set_xlabel('Value')
axs[0].set_ylabel('Result')

axs[1].plot(df['Value'], df['Error'], color='red')
axs[1].set_xlabel('Value')
axs[1].set_ylabel('Error')

plt.show()
