#ifndef EVENT_HPP
#define EVENT_HPP

#include <string>
#include <asio.hpp>
#include <vector>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>


struct Event {
    std::string id;
    std::string pubkey;
    std::string content;
    std::string sig;
    long created_at;
    std::vector<std::vector<std::string>> tags;

    template<class Archive>
    void serialize(Archive& ar) {
        ar(id, pubkey, content, sig, created_at, tags);
    }

    std::string calculate_hash();
    std::string sign_event(const std::string& private_key);
};

struct EventMessage {
    Event event;
    std::string pubkey; // Chave pública do usuário
    // Outros campos conforme necessário

    // Usar Cereal para serialização
    template<class Archive>
    void serialize(Archive & archive) {
        archive(event, pubkey);
    }
};


void send_message(asio::io_context& io_context, const std::string& message, const std::string& pubkey);
Event deserialize(const std::string& data);
bool verify_signature(const Event& event, const std::string& public_key_pem); // Corrigido: inclui a chave pública


std::string generate_uuid();


#endif // EVENT_HPP