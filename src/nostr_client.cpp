#include <iostream>
#include <string>
#include <asio.hpp>
#include <cereal/archives/json.hpp>
#include <sstream>
#include "event.hpp"

using asio::ip::tcp;

void send_message(asio::io_context& io_context, const std::string& message, const std::string& private_key) {
    try {
        // Resolver endereço do servidor
        tcp::resolver resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", "8000");
        
        // Conectar ao servidor
        tcp::socket socket(io_context);
        asio::connect(socket, endpoints);

        // Criar o evento
        Event event;
        event.pubkey = "minha-chave-publica";  // Substituir pela chave pública real
        event.content = message;
        event.created_at = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        event.id = event.calculate_hash();
        event.sig = event.sign_event(private_key);

        // Serializar o evento usando Cereal
        std::ostringstream oss;
        {
            cereal::JSONOutputArchive archive(oss);  // Criar o arquivo JSON
            archive(event);  // Serializar o evento
        }
        std::string serialized_event = oss.str();  // Obter o evento serializado como string

        // Enviar a mensagem serializada para o servidor
        asio::write(socket, asio::buffer(serialized_event));
    } catch (std::exception& e) {
        std::cerr << "Erro no cliente: " << e.what() << std::endl;
    }
}

int main() {
    asio::io_context io_context;
    std::string message = "Olá, servidor!";
    std::string private_key = "minha-chave-privada";  // Substituir pela chave privada real

    send_message(io_context, message, private_key);

    return 0;
}