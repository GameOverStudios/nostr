#include <boost/asio.hpp>
#include <iostream>
#include <thread>
#include <vector>
#include <nlohmann/json.hpp>  // Usaremos a biblioteca JSON para processar os eventos Nostr

using boost::asio::ip::tcp;
using json = nlohmann::json;

void handle_client(tcp::socket socket) {
    try {
        while (true) {
            char data[1024];
            boost::system::error_code error;

            size_t length = socket.read_some(boost::asio::buffer(data), error);
            if (error == boost::asio::error::eof)
                break; // Conexão encerrada pelo cliente
            else if (error)
                throw boost::system::system_error(error);

            // Interpretar a mensagem recebida como um evento Nostr
            std::string received_message(data, length);
            json event = json::parse(received_message);

            std::cout << "Evento Nostr recebido: " << event.dump() << std::endl;

            // Processar o evento conforme o NIP-01 (estrutura básica de eventos)
            if (event.contains("id") && event.contains("pubkey") && event.contains("content")) {
                // Enviar resposta de confirmação para o cliente
                boost::asio::write(socket, boost::asio::buffer("Evento recebido e validado!\n"));
            } else {
                boost::asio::write(socket, boost::asio::buffer("Evento inválido!\n"));
            }
        }
    } catch (std::exception& e) {
        std::cerr << "Erro: " << e.what() << std::endl;
    }
}

int main() {
    try {
        boost::asio::io_context io_context;

        // Configurar o servidor para escutar na porta 8080
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));
        std::cout << "Servidor Nostr aguardando conexões..." << std::endl;

        std::vector<std::thread> threads;

        // Aceitar múltiplos clientes
        while (true) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);

            // Criar uma nova thread para cada cliente
            threads.emplace_back(std::thread(handle_client, std::move(socket)));
        }

        // Garantir que todas as threads terminem corretamente
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }

    } catch (std::exception& e) {
        std::cerr << "Erro: " << e.what() << std::endl;
    }

    return 0;
}
