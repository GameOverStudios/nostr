#include "event.hpp"
#include <openssl/sha.h>  // Para SHA256
#include <random>
#include <asio.hpp>
#include <sstream>
#include <cereal/archives/json.hpp>
#include <sstream>
#include "database.hpp" 

// Função para gerar UUIDs únicos
std::string generate_uuid() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::uniform_int_distribution<> dis2(8, 11);

    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < 8; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 4; i++) ss << dis(gen);
    ss << "-4";  // UUID version 4
    for (int i = 0; i < 3; i++) ss << dis(gen);
    ss << "-";
    ss << dis2(gen);  // UUID variant 1
    for (int i = 0; i < 3; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 12; i++) ss << dis(gen);
    return ss.str();
}

// Função que agora gera um UUID único para cada evento
std::string Event::calculate_hash() {
    return generate_uuid();
}

// Implementação fictícia de assinatura
std::string Event::sign_event(const std::string& private_key) {
    return "assinatura";  // Substitua pela lógica real
}

// Função de desserialização com Cereal
Event deserialize(const std::string& data) {
    Event event;
    std::istringstream iss(data);
    {
        cereal::JSONInputArchive archive(iss);
        archive(event);
    }
    return event;
}

// Verificação de assinatura fictícia
bool verify_signature(const Event& event) {
    // Lógica fictícia de verificação
    return true;  // Substitua pela lógica real
}