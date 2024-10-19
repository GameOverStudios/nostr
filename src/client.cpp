#include <boost/asio.hpp>
#include <iostream>
#include <nlohmann/json.hpp>  // Usaremos a biblioteca JSON para criar eventos Nostr

using boost::asio::ip::tcp;
using json = nlohmann::json;

int main() {
    try {
        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("127.0.0.1", "8080");

        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        // Criar um evento Nostr de exemplo (NIP-01)
        json event = {
            {"id", "abc123"},
            {"pubkey", "public_key_value"},
            {"created_at", 1692894000},
            {"kind", 1},  // tipo do evento
            {"tags", json::array()},
            {"content", "Olá, este é um evento Nostr!"}
        };

        std::string message = event.dump();

        // Enviar o evento ao servidor
        boost::asio::write(socket, boost::asio::buffer(message));

        // Receber a resposta do servidor
        char reply[1024];
        boost::system::error_code error;
        size_t reply_length = socket.read_some(boost::asio::buffer(reply), error);

        if (error)
            throw boost::system::system_error(error);

        std::cout << "Resposta do servidor: " << std::string(reply, reply_length) << std::endl;

    } catch (std::exception& e) {
        std::cerr << "Erro: " << e.what() << std::endl;
    }

    return 0;
}
