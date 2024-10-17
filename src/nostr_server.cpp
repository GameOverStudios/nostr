#include <iostream>
#include <string>
#include <asio.hpp>
#include <cereal/archives/json.hpp>
#include <sstream>
#include <sqlite3.h>
#include <openssl/pem.h>
#include "event.hpp"
#include "database.hpp" // Certifique-se de que este cabeçalho está correto

using asio::ip::tcp;

// Classe para gerenciar a sessão de cada cliente
class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, sqlite3* db)
        : socket_(std::move(socket)), db_(db) {}

    void start() {
        do_read();
    }

private:
    tcp::socket socket_;
    sqlite3* db_;
    char data_[1024]; // Buffer para armazenar dados recebidos

    void do_read() {
        auto self(shared_from_this());
        socket_.async_read_some(asio::buffer(data_),
            [this, self](std::error_code ec, std::size_t length) {
                if (!ec) {
                    std::string received_message(data_, length);
                    Event event = deserialize(received_message);

                    if (verify_signature(event)) {
                        store_event(db_, event);
                        std::cout << "Evento armazenado com sucesso." << std::endl;
                    } else {
                        std::cerr << "Assinatura inválida." << std::endl;
                    }

                    do_read();  // Continue lendo a próxima mensagem
                } else if (ec == asio::error::eof) {
                    std::cerr << "Conexão encerrada pelo cliente." << std::endl;
                } else {
                    std::cerr << "Erro ao ler: " << ec.message() << std::endl;
                }
            });
    }
};

// Classe para gerenciar o servidor
class Server {
public:
    Server(asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), db_(open_database("events.db")) {
        create_events_table(db_); // Criar a tabela no início
        do_accept();
    }

private:
    tcp::acceptor acceptor_;
    sqlite3* db_;

    void do_accept() {
        acceptor_.async_accept([this](std::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::make_shared<Session>(std::move(socket), db_)->start();
            }
            do_accept();  // Aceitar a próxima conexão
        });
    }
};

// Função principal
int main() {
    try {
        asio::io_context io_context;
        int port = 8000;  // Defina a porta desejada
        
        Server server(io_context, port);
        std::cout << "Servidor aguardando conexões no porto " << port << "...\n";
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Erro: " << e.what() << std::endl;
    }
}