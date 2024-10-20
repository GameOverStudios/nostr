#include <iostream>
#include <thread>
#include <vector>
#include <map>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include <sqlite3.h> // Ou outro mecanismo de persistência, se necessário
#include <iomanip>
#include <sstream>
#include <secp256k1.h>
#include <secp256k1_recovery.h>

using boost::asio::ip::tcp;
using json = nlohmann::json;

std::vector<unsigned char> hex_string_to_bytes(const std::string& hex_str) {
    std::vector<unsigned char> bytes;
    for (unsigned int i = 0; i < hex_str.length(); i += 2) {
        std::string byteString = hex_str.substr(i, 2);
        char byte = (char)strtol(byteString.c_str(), nullptr, 16);
        bytes.push_back(byte);
    }
    return bytes;
}

std::string bytes_to_hex_string(const unsigned char *bytes, size_t len) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; ++i) {
        ss << std::setw(2) << static_cast<int>(bytes[i]);
    }
    return ss.str();
}

std::string serialize_event_data(const json& event_json) {
    std::stringstream ss;
    ss << "[" << event_json["kind"] << ",\"" << event_json["pubkey"] << "\",\"" << event_json["content"] << "\",";

    // Serializar as tags
    ss << "[";
    if (event_json.contains("tags") && event_json["tags"].is_array()) {
        for (size_t i = 0; i < event_json["tags"].size(); ++i) {
            ss << "[";
            for (size_t j = 0; j < event_json["tags"][i].size(); ++j) {
                ss << "\"" << event_json["tags"][i][j].get<std::string>() << "\"";
                if (j < event_json["tags"][i].size() - 1) {
                    ss << ",";
                }
            }
            ss << "]";
            if (i < event_json["tags"].size() - 1) {
                ss << ",";
            }
        }
    }
    ss << "],";
    ss << event_json["created_at"] << "]";

    return ss.str();
}

bool verify_event_signature(const json& event_json) {
    // 1. Inicializar o contexto secp256k1
    secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_VERIFY);


    // 2. Converter a chave pública (pubkey) para o formato secp256k1
    secp256k1_pubkey pubkey_secp256k1;
     if (!secp256k1_ec_pubkey_parse(ctx, &pubkey_secp256k1, hex_string_to_bytes(event_json["pubkey"]).data(), event_json["pubkey"].get<std::string>().length()/2)) {
        std::cerr << "Erro ao analisar a chave pública." << std::endl;
        secp256k1_context_destroy(ctx);
        return false;
    }


    // 3. Converter a assinatura (sig) para o formato secp256k1
    secp256k1_ecdsa_signature signature_secp256k1;

     if (!secp256k1_ecdsa_signature_parse_compact(ctx, &signature_secp256k1, hex_string_to_bytes(event_json["sig"]).data())) {
       std::cerr << "Erro ao analisar a assinatura." << std::endl;
        secp256k1_context_destroy(ctx);
       return false;
    }

    // 4. Serializar os dados do evento (sem a assinatura)
    std::string serialized_data = serialize_event_data(event_json);


    // 5. Verificar a assinatura
    if (secp256k1_ecdsa_verify(ctx, &signature_secp256k1, (const unsigned char*)serialized_data.data(), &pubkey_secp256k1) != 1) {
         std::cerr << "Assinatura do evento inválida!" << std::endl;
        secp256k1_context_destroy(ctx);
        return false;
    }

    secp256k1_context_destroy(ctx); // Destruir o contexto após o uso
    return true;
}

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

int main() {
    try {
        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));

        std::cout << "Servidor Nostr aguardando conexões na porta 8080..." << std::endl;

        while (true) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);
            std::thread(session, std::move(socket)).detach();
        }
    } catch (std::exception& e) {
        std::cerr << "Erro do servidor: " << e.what() << std::endl;
    }

    return 0;
}