#!/bin/bash

# Função para iniciar o servidor
start_server() {
  echo "Iniciando o servidor..."
  build/server &
  SERVER_PID=$!
  echo "Servidor iniciado com PID: $SERVER_PID"
}

# Função para iniciar os clientes
start_clients() {
  local num_clients=$1
  echo "Iniciando $num_clients clientes..."

  for ((i=1; i<=num_clients; i++)); do
    echo "Iniciando cliente $i..."
    build/client &
    CLIENT_PID[$i]=$!
    echo "Cliente $i iniciado com PID: ${CLIENT_PID[$i]}"
  done
}

# Função para parar o servidor e os clientes
stop_server_and_clients() {
  echo "Parando todos os clientes e o servidor..."
  
  # Parar clientes
  for pid in "${CLIENT_PID[@]}"; do
    echo "Parando cliente com PID: $pid"
    kill $pid 2>/dev/null
  done

  # Parar servidor
  echo "Parando o servidor com PID: $SERVER_PID"
  kill $SERVER_PID 2>/dev/null
}

# Função principal
main() {
  if [ -z "$1" ]; then
    echo "Uso: $0 <número_de_clientes>"
    exit 1
  fi

  local num_clients=$1

  # Iniciar o servidor
  start_server

  # Iniciar os clientes
  start_clients $num_clients

  # Aguardar até o servidor ser interrompido
  wait $SERVER_PID
}

# Capturar Ctrl+C e encerrar o servidor e os clientes corretamente
trap stop_server_and_clients SIGINT

# Executar a função principal
main "$@"
