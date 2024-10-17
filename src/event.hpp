#ifndef EVENT_H
#define EVENT_H

#include <string>
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

    std::string calculate_hash(); // Função que agora gera UUIDs únicos
    std::string sign_event(const std::string& private_key); // Para assinatura
};

// Declaração das funções de serialização e verificação
Event deserialize(const std::string& data);
bool verify_signature(const Event& event);

#endif // EVENT_H
