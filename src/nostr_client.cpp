#include <iostream>
#include <string>
#include <asio.hpp>
#include <cereal/archives/json.hpp>
#include <sstream>
#include "event.hpp"

using asio::ip::tcp;

// Função para enviar uma mensagem/evento
void send_message(asio::io_context& io_context, const std::string& message, const std::string& pubkey) {
    Event event;
    event.pubkey = pubkey;
    event.content = message;
    event.created_at = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    event.id = event.calculate_hash();
    event.sig = event.sign_event("minha-chave-privada");

    std::ostringstream oss;
    {
        cereal::JSONOutputArchive archive(oss);
        archive(event);
    }
    std::string serialized_event = oss.str();

    asio::ip::tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve("127.0.0.1", "8000");
    asio::ip::tcp::socket socket(io_context);
    asio::connect(socket, endpoints);
    asio::write(socket, asio::buffer(serialized_event));
}


int main() {
    asio::io_context io_context;
    std::string message = "Olá, servidor!";
    std::string private_key = "minha-chave-privada";  // Substituir pela chave privada real

    send_message(io_context, message, private_key);

    return 0;
}