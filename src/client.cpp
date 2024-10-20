#include <iostream>
#include <boost/asio.hpp>
#include <nlohmann/json.hpp>
#include <secp256k1.h>
#include <secp256k1_recovery.h>
#include <random>

using boost::asio::ip::tcp;
using json = nlohmann::json;

// Função auxiliar para converter bytes em string hexadecimal
std::string bytes_to_hex_string(const unsigned char *bytes, size_t len) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (size_t i = 0; i < len; ++i) {
        ss << std::setw(2) << static_cast<int>(bytes[i]);
    }
    return ss.str();
}


int main() {
    try {
        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", "8080"); // resolvendo os endpoints

        tcp::socket socket(io_context);  // Declarando o socket corretamente
        boost::asio::connect(socket, endpoints); // Conectando
        
        // 1. Inicializar o contexto secp256k1
        secp256k1_context *ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);

        // 2. Gerar chave privada
        unsigned char privkey[32];
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> distrib(0, 255);
        for (size_t i = 0; i < 32; ++i) {
            privkey[i] = distrib(gen);
        }

        // 3. Obter chave pública
        secp256k1_pubkey pubkey;
        if (!secp256k1_ec_pubkey_create(ctx, &pubkey, privkey)) {
            std::cerr << "Erro ao criar a chave pública." << std::endl;
            return 1;
        }

        // Converter a chave pública para bytes
        size_t pubkey_len = 33;  // Tamanho comprimido da chave pública
        unsigned char pubkey_bytes[33];


        secp256k1_ec_pubkey_serialize(ctx, pubkey_bytes, &pubkey_len, &pubkey, SECP256K1_EC_COMPRESSED );



        // ... (Conexão com o servidor - igual ao código anterior) ...

        // Criar um evento de exemplo (NIP-01)
        json event;
        event["kind"] = 1;
        event["pubkey"] = bytes_to_hex_string(pubkey_bytes, pubkey_len); // Chave pública correta
        event["content"] = "Teste de mensagem do cliente com libsecp256k1!";
        event["created_at"] = time(nullptr); // Timestamp atual
        event["tags"] = json::array();



        // 4. Assinar o evento
        secp256k1_ecdsa_signature signature;
        std::string serialized_event = event.dump(); // Serializar antes de assinar

        if (!secp256k1_ecdsa_sign(ctx, &signature, (const unsigned char*)serialized_event.data(), privkey, nullptr, nullptr)) {
        std::cerr << "Erro ao assinar o evento." << std::endl;
            return 1;
        }


        // Converter assinatura para formato compacto (bytes)
        unsigned char compact_sig[64];
        secp256k1_ecdsa_signature_serialize_compact(ctx, compact_sig, &signature);

        event["sig"] = bytes_to_hex_string(compact_sig, 64); // Assinatura correta



        // Serializar o evento em formato JSON e enviar para o servidor
        std::string event_str = event.dump();
        boost::asio::write(socket, boost::asio::buffer("[\"EVENT\", " + event_str + "]\n"));

        secp256k1_context_destroy(ctx); // Mover para DEPOIS do uso do contexto

    } catch (std::exception& e) {
        std::cerr << "Erro: " << e.what() << std::endl;
    }
    return 0;
}