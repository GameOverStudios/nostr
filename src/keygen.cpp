#include "keygen.hpp"
#include <iostream>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

std::pair<std::string, std::string> generate_rsa_keypair(int key_size) {
    // Cria o contexto de chaves
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    if (!ctx) {
        std::cerr << "Erro ao criar contexto de chaves: " << ERR_error_string(ERR_get_error(), NULL) << std::endl;
        return {};
    }

    // Inicializa o contexto para geração de chaves
    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        std::cerr << "Erro ao inicializar contexto de geração de chaves: " << ERR_error_string(ERR_get_error(), NULL) << std::endl;
        EVP_PKEY_CTX_free(ctx);
        return {};
    }

    // Define o tamanho da chave
    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, key_size) <= 0) {
        std::cerr << "Erro ao definir tamanho da chave: " << ERR_error_string(ERR_get_error(), NULL) << std::endl;
        EVP_PKEY_CTX_free(ctx);
        return {};
    }

    // Gera a chave
    EVP_PKEY *pkey = NULL;
    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        std::cerr << "Erro ao gerar chave: " << ERR_error_string(ERR_get_error(), NULL) << std::endl;
        EVP_PKEY_CTX_free(ctx);
        return {};
    }

    // Obtém a chave RSA
    RSA *rsa = EVP_PKEY_get1_RSA(pkey);


    // Cria os BIOs para armazenar as chaves em memória
    BIO *pri = BIO_new(BIO_s_mem());
    BIO *pub = BIO_new(BIO_s_mem());
    // ... (Verificação de erros para BIO_new)

    // Escreve as chaves nos BIOs
     if (!PEM_write_bio_RSAPrivateKey(pri, rsa, NULL, NULL, 0, NULL, NULL)) {
        std::cerr << "Erro ao escrever chave privada PEM: " << ERR_error_string(ERR_get_error(), NULL) << std::endl;
    }
     if (!PEM_write_bio_RSAPublicKey(pub, rsa)) {
        std::cerr << "Erro ao escrever chave publica PEM: " << ERR_error_string(ERR_get_error(), NULL) << std::endl;
    }

    char *pri_key = NULL;
    char *pub_key = NULL;
    long pri_len = BIO_get_mem_data(pri, &pri_key);
    long pub_len = BIO_get_mem_data(pub, &pub_key);

    std::string private_key_pem(pri_key, pri_len);
    std::string public_key_pem(pub_key, pub_len);

    RSA_free(rsa);
    EVP_PKEY_free(pkey); // Libera a chave EVP_PKEY
    EVP_PKEY_CTX_free(ctx); // Libera o contexto
    BIO_free_all(pri);
    BIO_free_all(pub);

    return std::make_pair(private_key_pem, public_key_pem);
}