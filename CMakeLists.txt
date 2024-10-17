cmake_minimum_required(VERSION 3.10)

# Nome do projeto
project(TCP_Client_Server LANGUAGES CXX)

# Definir o padrão da linguagem C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Diretórios de inclusão das bibliotecas
include_directories(externals/asio-1.30.2/include)
include_directories(externals/openssl-3.3.2/include)
include_directories(externals/cereal/include)

# Diretório da biblioteca SQLite
include_directories(externals/sqlite/libsqlite3)

# Adicionar os executáveis e seus respectivos arquivos fonte
add_executable(server src/nostr_server.cpp src/event.cpp src/database.cpp)
add_executable(client src/nostr_client.cpp src/event.cpp src/database.cpp)

find_package(SQLite3 REQUIRED)
target_link_libraries(server SQLite::SQLite3)

# Linkar bibliotecas
target_link_libraries(server 
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/openssl-3.3.2/libssl.a 
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/openssl-3.3.2/libcrypto.a
    sqlite3  # Linkando a biblioteca SQLite
)

target_link_libraries(client
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/openssl-3.3.2/libssl.a 
    ${CMAKE_CURRENT_SOURCE_DIR}/externals/openssl-3.3.2/libcrypto.a
    sqlite3  # Linkando a biblioteca SQLite
)