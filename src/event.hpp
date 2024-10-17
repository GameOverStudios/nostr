#ifndef EVENT_HPP
#define EVENT_HPP

#include <string>
#include <asio.hpp>  // Inclua a biblioteca Asio para usar asio::io_context
#include <vector>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>

// Estrutura do evento
struct Event {
    std::string id;
    std::string pubkey;
    std::string content;
    std::string sig;
    long created_at;
    std::vector<std::vector<std::string>> tags;

    // Funções para serializar e desserializar eventos
    template<class Archive>
    void serialize(Archive& ar) {
        ar(id, pubkey, content, sig, created_at, tags);
    }

    std::string calculate_hash();  // Gerar o ID do evento (com UUID)
    std::string sign_event(const std::string& private_key);  // Assinar o evento
};

void send_message(asio::io_context& io_context, const std::string& message, const std::string& pubkey);
Event deserialize(const std::string& data);
bool verify_signature(const Event& event);

#endif // EVENT_HPP
