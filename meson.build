# meson wrap install cjson sqlite3 asio
# meson setup build
# cd build && meson compile

project('nostr', 'cpp',
  version : '0.1',
  default_options : ['warning_level=3'])

json_dep = dependency('nlohmann_json')
sqlite = dependency('sqlite3')

# Boost para rede e multithreading
boost_dep = dependency('boost', modules : ['thread', 'system'])

# Definição do executável do servidor
exe_server = executable(
  'server',
  'src/server.cpp',
  install : true,
  dependencies: [json_dep, sqlite, boost_dep],
)

# Definição do executável do cliente
exe_client = executable(
  'client',
  'src/client.cpp',
  install : true,
  dependencies: [json_dep, sqlite, boost_dep],
)

# Testes básicos para os executáveis
test('basic', exe_server)
test('basic', exe_client)
