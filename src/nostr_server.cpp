#include <iostream>
#include <string>
#include <asio.hpp>
#include <cereal/archives/json.hpp>
#include <sstream>
#include <sqlite3.h>
#include "event.hpp"
#include "database.hpp"

using asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, sqlite3* db)
        : socket_(std::move(socket)), db_(db) {}

    void start() {
        do_read();
    }

private:
    void do_read() {
        auto self(shared_from_this());
        socket_.async_read_some(asio::buffer(data_, max_length),
            [this, self](std::error_code ec, std::size_t length) {
                if (!ec) {
                    std::string received_message(data_, length);

                    try {
                        std::istringstream iss(received_message);
                        EventMessage event_message;
                        {
                            cereal::JSONInputArchive archive(iss);
                            archive(event_message);
                        }

                        // Agora você tem o objeto event_message desserializado, use-o diretamente
                        if (verify_signature(event_message.event, event_message.pubkey)) {
                            store_event(db_, event_message.event);
                            std::cout << "Evento armazenado com sucesso." << std::endl;

                            // Envia uma resposta de volta para o cliente
                            std::string response = "{\"status\":\"ok\"}";
                            asio::write(socket_, asio::buffer(response));

                        } else {
                            std::cerr << "Assinatura inválida." << std::endl;
                            std::string response = "{\"status\":\"erro\", \"mensagem\":\"Assinatura inválida.\"}";
                            asio::write(socket_, asio::buffer(response));
                        }
                    } catch (const std::exception& e) {
                        std::cerr << "Erro ao desserializar ou processar a mensagem: " << e.what() << std::endl;
                        std::string response = "{\"status\":\"erro\", \"mensagem\":\"Erro ao processar a mensagem.\"}";

                        try {
                            asio::write(socket_, asio::buffer(response));
                        } catch (const std::exception& write_error) {
                            std::cerr << "Erro ao escrever resposta de volta para o cliente: " << write_error.what() << std::endl;
                        }
                    }

                    do_read();
                } else if (ec == asio::error::eof) {
                    std::cout << "Conexão encerrada pelo cliente." << std::endl;
                } else {
                    std::cerr << "Erro ao ler: " << ec.message() << std::endl;
                }
            });
    }

    tcp::socket socket_;
    sqlite3* db_;
    enum { max_length = 1024 };
    char data_[max_length];
};

class Server {
public:
    Server(asio::io_context& io_context, short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port)), db_(open_database("events.db")) {
        create_events_table(db_);
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept([this](std::error_code ec, tcp::socket socket) {
            if (!ec) {
                std::make_shared<Session>(std::move(socket), db_)->start();
            } else {
                std::cerr << "Erro ao aceitar conexão: " << ec.message() << std::endl;
            }

            do_accept();
        });
    }

    tcp::acceptor acceptor_;
    sqlite3* db_;
};

int main() {
    try {
        asio::io_context io_context;
        int port = 8888;  
        Server server(io_context, port);
        std::cout << "Servidor aguardando conexões no porto " << port << "...\n";
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Erro: " << e.what() << std::endl;
    }
}
