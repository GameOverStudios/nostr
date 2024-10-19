#!/bin/bash

# Verifica se foi passado um diretório como argumento
if [ -z "$1" ]; then
  echo "Uso: $0 <diretorio>"
  exit 1
fi

# Verifica se o diretório existe
if [ ! -d "$1" ]; then
  echo "Erro: Diretório '$1' não encontrado."
  exit 1
fi

# Diretório a ser processado
diretorio="$1"

# Nome do arquivo de saída
arquivo_saida="conteudo_combinado.txt"

# Limpa o arquivo de saída se ele já existir
> "$arquivo_saida"

# Itera por todos os arquivos no diretório (incluindo subdiretórios)
find "$diretorio" -type f -print0 | while IFS= read -r -d $'\0' arquivo; do
  # Obtém o caminho relativo do arquivo
  caminho_relativo=$(realpath --relative-to="$diretorio" "$arquivo")

  # Adiciona um separador com o nome do arquivo ao arquivo de saída
  echo "### Início do arquivo: $caminho_relativo ###" >> "$arquivo_saida"

  # Adiciona o conteúdo do arquivo ao arquivo de saída
  cat "$arquivo" >> "$arquivo_saida"

  # Adiciona um separador de fim de arquivo ao arquivo de saída
  echo "### Fim do arquivo: $caminho_relativo ###" >> "$arquivo_saida"
  echo "" >> "$arquivo_saida" # Adiciona uma linha em branco para melhor legibilidade
done

echo "Conteúdo dos arquivos combinado em '$arquivo_saida'"
