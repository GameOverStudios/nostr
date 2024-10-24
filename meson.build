# sudo apt install libboost-all-dev libsecp256k1-dev libcurl4-openssl-dev
# meson wrap install sqlite3 asio openssl
# meson setup build
# cd build && meson compile

project('nostr', 'cpp', version : '0.1', default_options : ['warning_level=3'])

json = dependency('nlohmann_json')
sqlite = dependency('sqlite3')
boost = dependency('boost', modules : ['thread', 'system'])
openssl = dependency('openssl', required : true)
secp256k1 = dependency('libsecp256k1', required: true)


exe_server = executable('server', 'src/server.cpp',
  install : true,
  dependencies: [json, sqlite, boost, openssl, secp256k1],
)

# Definição do executável do cliente
exe_client = executable('client', 'src/client.cpp',
  install : true,
  dependencies: [json, sqlite, boost, openssl, secp256k1],
)

# Testes básicos para os executáveis
test('basic', exe_server)
test('basic', exe_client)
