#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include <boost/asio.hpp>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/objects.h>
#include <openssl/core_names.h>
#include <nlohmann/json.hpp>
#include <sqlite3.h>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <thread>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/err.h>
#include <openssl/objects.h>
#include <openssl/core_names.h>
#include <vector>
#include <string>
#include <iostream>
#include <openssl/params.h> // Inclua para OSSL_PARAM
#include <secp256k1.h>
#include <secp256k1_recovery.h> // Para assinaturas com recuperação
#include <iostream>
#include <vector>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
using json = nlohmann::json;

// Função auxiliar para converter string hexadecimal em array de bytes
std::vector<unsigned char> hex_string_to_bytes(const std::string& hex_str) {
    std::vector<unsigned char> bytes;
    for (unsigned int i = 0; i < hex_str.length(); i += 2) {
        std::string byteString = hex_str.substr(i, 2);
        char byte = (char)strtol(byteString.c_str(), nullptr, 16);
        bytes.push_back(byte);
    }
    return bytes;
}

// Classe para representar um evento Nostr
class NostrEvent {
public:
    int kind;
    std::string pubkey;
    std::string content;
    std::string id;
    std::vector<std::vector<std::string>> tags;
    std::string sig;
    long created_at;

    // Construtor padrão (sem argumentos)
    NostrEvent() : kind(0), pubkey(""), content(""), id(""), sig(""), created_at(0) {}

    NostrEvent(const json& event_json) {
        kind = event_json["kind"];
        pubkey = event_json["pubkey"];
        content = event_json["content"];
        id = event_json["id"];

        if (event_json.contains("tags") && event_json["tags"].is_array()) {
            for (const auto& tag_array : event_json["tags"]) {
                std::vector<std::string> tag_vec;
                for (const auto& tag_item : tag_array) {
                    tag_vec.push_back(tag_item.get<std::string>());
                }
                tags.push_back(tag_vec);
            }
        }

        sig = event_json["sig"];

        if (event_json.contains("created_at"))
            created_at = event_json["created_at"];
    }

    

    json to_json() const {
        json event_json;
        event_json["kind"] = kind;
        event_json["pubkey"] = pubkey;
        event_json["content"] = content;
        event_json["id"] = id;
        event_json["tags"] = tags;
        event_json["sig"] = sig;

        if (created_at != 0)
            event_json["created_at"] = created_at;

        return event_json;
    }
};

// Mapa para armazenar os eventos recebidos (chave: ID do evento)
std::map<std::string, NostrEvent> event_store;

// Função para lidar com cada conexão de cliente
// Função para verificar a chave pública recebida do cliente
bool verify_pubkey(const std::string& pubkey) {
    // 1. Inicializar o contexto secp256k1
    secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_VERIFY);
    if (!ctx) {
        std::cerr << "Erro ao criar o contexto secp256k1." << std::endl;
        return false;
    }

    // 2. Converter a chave pública para o formato secp256k1
    secp256k1_pubkey pubkey_secp256k1;
    if (!secp256k1_ec_pubkey_parse(ctx, &pubkey_secp256k1, hex_string_to_bytes(pubkey).data(), pubkey.length() / 2)) {
        std::cerr << "Chave pública inválida!" << std::endl;
        secp256k1_context_destroy(ctx);
        return false;
    }

    // 3. Liberar o contexto secp256k1
    secp256k1_context_destroy(ctx);

    return true; // Chave pública válida
}


// Função para lidar com cada conexão de cliente
void session(tcp::socket socket) {
    try {
        while (true) {
            boost::system::error_code error;
            char data[1024];

            // Receber a mensagem do socket
            size_t length = socket.read_some(boost::asio::buffer(data), error); 

            if (error == boost::asio::error::eof) {
                break; // Conexão fechada pelo cliente
            } else if (error) {
                throw boost::system::system_error(error); // Outro erro
            }

            std::string message(data, length); // Criar a string message aqui
            std::cout << "Mensagem recebida: " << message << std::endl;

            try {
                json message_json = json::parse(message);

                // ... (restante do código - sem alterações)
            } catch (json::parse_error& e) {
                std::cerr << "Erro ao analisar JSON: " << e.what() << std::endl;
            }
        }
    } catch (std::exception& e) {
        std::cerr << "Erro na sessão: " << e.what() << std::endl;
    }
}


// Função principal do servidor
int main() {
    try {
        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));

        std::cout << "Servidor Nostr aguardando conexões na porta 8080..." << std::endl;

        while (true) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);
            std::thread(session, std::move(socket)).detach(); // Inicia uma nova thread para cada cliente
        }
    } catch (std::exception& e) {
        std::cerr << "Erro do servidor: " << e.what() << std::endl;
    }

    return 0;
}
