#include <iostream>
#include <thread>
#include <vector>
#include <boost/asio.hpp>
#include <openssl/evp.h>
#include <openssl/ec.h>
#include <openssl/pem.h>
#include <openssl/err.h>

using boost::asio::ip::tcp;

// Função para gerar chaves pública e privada
std::pair<std::string, std::string> generate_keypair() {
    // Criação do contexto para chaves EC
    EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_EC, nullptr);
    EVP_PKEY* pkey = nullptr;

    // Geração da chave
    if (EVP_PKEY_keygen_init(pctx) <= 0 || 
        EVP_PKEY_CTX_set_ec_paramgen_curve_nid(pctx, NID_secp256k1) <= 0 || 
        EVP_PKEY_keygen(pctx, &pkey) <= 0) {
        EVP_PKEY_CTX_free(pctx);
        throw std::runtime_error("Erro ao gerar a chave EC");
    }
    EVP_PKEY_CTX_free(pctx);

    // Geração da chave privada
    BIO* private_bio = BIO_new(BIO_s_mem());
    PEM_write_bio_PrivateKey(private_bio, pkey, nullptr, nullptr, 0, nullptr, nullptr);

    char* private_key = nullptr;
    long private_key_len = BIO_get_mem_data(private_bio, &private_key);
    std::string private_key_str(private_key, private_key_len);
    BIO_free(private_bio);

    // Geração da chave pública
    BIO* public_bio = BIO_new(BIO_s_mem());
    PEM_write_bio_PUBKEY(public_bio, pkey);

    char* public_key = nullptr;
    long public_key_len = BIO_get_mem_data(public_bio, &public_key);
    std::string public_key_str(public_key, public_key_len);
    BIO_free(public_bio);

    EVP_PKEY_free(pkey); // Libera a chave
    return { public_key_str, private_key_str };
}

// Função para lidar com cada conexão de cliente
void session(tcp::socket socket) {
    try {
        auto [public_key, private_key] = generate_keypair();
        
        // Envia as chaves para o cliente
        std::string response = "Chave Pública:\n" + public_key + "\nChave Privada:\n" + private_key + "\n";
        boost::asio::write(socket, boost::asio::buffer(response));
    } catch (std::exception& e) {
        std::cerr << "Erro: " << e.what() << std::endl;
    }
}

// Função principal do servidor
int main() {
    try {
        boost::asio::io_context io_context;
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 8080));

        std::cout << "Servidor Nostr aguardando conexões na porta 8080..." << std::endl;

        while (true) {
            tcp::socket socket(io_context);
            acceptor.accept(socket);
            std::thread(session, std::move(socket)).detach(); // Inicia uma nova thread para cada cliente
        }
    } catch (std::exception& e) {
        std::cerr << "Erro do servidor: " << e.what() << std::endl;
    }

    return 0;
}
