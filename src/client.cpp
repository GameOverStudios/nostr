#include <iostream>
#include <thread>
#include <vector>
#include <boost/asio.hpp>
#include <openssl/ec.h>
#include <openssl/pem.h>
#include <openssl/err.h>

using boost::asio::ip::tcp;

int main() {
    try {
        boost::asio::io_context io_context;
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("127.0.0.1", "8080");
        tcp::socket socket(io_context);
        boost::asio::connect(socket, endpoints);

        // Espera a resposta do servidor
        char reply[1024];
        boost::system::error_code error;
        size_t reply_length = socket.read_some(boost::asio::buffer(reply), error);

        if (error)
            throw boost::system::system_error(error);

        // Imprime a resposta bruta
        std::cout << "Resposta bruta do servidor: " << std::string(reply, reply_length) << std::endl;

        // Analisa a resposta como JSON
        std::string reply_str(reply, reply_length);
        std::cout << "Resposta JSON: " << reply_str << std::endl;

        // Aqui, você pode adicionar código para processar a resposta JSON, se necessário
    } catch (std::exception& e) {
        std::cerr << "Erro: " << e.what() << std::endl;
    }

    return 0;
}
