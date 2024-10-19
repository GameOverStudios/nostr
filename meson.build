# meson.build

project('nostr', 'cpp',  # Mudando 'c' para 'cpp' já que você está usando C++
  version : '0.1',
  default_options : ['warning_level=3'])

# Dependência da biblioteca cJSON
cjson = dependency('libcjson')

# Definição do executável do servidor
exe_server = executable(
  'server',
  'src/server.cpp',
  install : true,
  dependencies: [cjson],
)

# Definição do executável do cliente
exe_client = executable(
  'client',
  'src/client.cpp',
  install : true,
  dependencies: [cjson],
)

# Testes básicos para os executáveis
test('basic', exe_server)
test('basic', exe_client)
