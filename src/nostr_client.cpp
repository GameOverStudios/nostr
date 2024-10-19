#include <iostream>
#include <string>
#include <asio.hpp>
#include <cereal/archives/json.hpp>
#include <sstream>
#include "event.hpp"
#include "keygen.hpp"

using asio::ip::tcp;

// Função para enviar uma mensagem/evento
void send_message(asio::io_context& io_context, const std::string& message, const std::string& private_key_pem, const std::string& pubkey) {
    Event event;
    event.pubkey = pubkey;
    event.content = message;
    event.created_at = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    event.id = event.calculate_hash();  // O ID deve ser calculado corretamente
    event.sig = event.sign_event(private_key_pem);

    EventMessage event_message;  // Aqui, EventMessage deve ser um objeto que encapsula o evento.
    event_message.pubkey = pubkey;  // A chave pública do usuário
    event_message.event = event;      // O evento que você criou

    // Serialização da mensagem
    std::ostringstream oss;
    {
        cereal::JSONOutputArchive archive(oss);
        archive(event_message);  // Serializa o objeto event_message
    }
    
    std::string serialized_event = oss.str();
    
    // Verifica a mensagem serializada
    std::cout << "Mensagem serializada: " << serialized_event << std::endl;

    try {
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("127.0.0.1", "8888");
        tcp::socket socket(io_context);
        asio::connect(socket, endpoints);
        asio::write(socket, asio::buffer(serialized_event));
        std::cout << "Mensagem enviada: " << message << std::endl;  // Adiciona feedback
    } catch (std::exception& e) {
        std::cerr << "Erro ao enviar mensagem: " << e.what() << std::endl;
    }
}

int main() {
    asio::io_context io_context;
    std::string message = "Olá, servidor!";

    auto keypair = generate_rsa_keypair(2048);

    if (!keypair.first.empty() && !keypair.second.empty()) {
        std::cout << "Chave privada (PEM):\n" << keypair.first << std::endl;
        std::cout << "\nChave pública (PEM):\n" << keypair.second << std::endl;

        send_message(io_context, message, keypair.first, keypair.second);

        // Mantém o cliente rodando para que a resposta do servidor possa ser recebida
        io_context.run();

    } else {
        std::cerr << "Erro ao gerar o par de chaves." << std::endl;
        return 1;
    }

    return 0;
}
