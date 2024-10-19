#include "event.hpp"
#include <openssl/sha.h>
#include <random>
#include <asio.hpp>
#include <sstream>
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include "event.hpp"
#include <openssl/sha.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <iostream>


std::string generate_uuid() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::uniform_int_distribution<> dis2(8, 11);

    std::stringstream ss;
    ss << std::hex;
    for (int i = 0; i < 8; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 4; i++) ss << dis(gen);
    ss << "-4";
    for (int i = 0; i < 3; i++) ss << dis(gen);
    ss << "-";
    ss << dis2(gen);
    for (int i = 0; i < 3; i++) ss << dis(gen);
    ss << "-";
    for (int i = 0; i < 12; i++) ss << dis(gen);
    return ss.str();
}

std::string Event::calculate_hash() {
    return generate_uuid();
}

// Função de desserialização com Cereal
Event deserialize(const std::string& data) {
    Event event;
    std::istringstream iss(data);
    {
        cereal::JSONInputArchive archive(iss);
        archive(event);
    }
    return event;
}


std::string Event::sign_event(const std::string& private_key_pem) {
    BIO *bio = BIO_new_mem_buf(private_key_pem.c_str(), -1);
    if (bio == NULL) {
        std::cerr << "Erro ao criar BIO: " << ERR_error_string(ERR_get_error(), NULL) << std::endl;
        return "";
    }

    EVP_PKEY *pkey = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL);
    BIO_free(bio); // Libera o BIO após o uso

    if (!pkey) {
        std::cerr << "Erro ao carregar a chave privada: " << ERR_error_string(ERR_get_error(), NULL) << std::endl;
        return "";
    }

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        std::cerr << "Erro ao criar o contexto de assinatura: " << ERR_error_string(ERR_get_error(), NULL) << std::endl;
        EVP_PKEY_free(pkey);
        return "";
    }

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*)content.c_str(), content.length(), hash);

    size_t siglen;
    unsigned char* signature = (unsigned char*)malloc(EVP_PKEY_size(pkey));
    if (signature == NULL) {
        std::cerr << "Erro ao alocar memória para a assinatura." << std::endl;
        EVP_PKEY_free(pkey);
        EVP_MD_CTX_free(mdctx);
        return "";
    }

    if (!EVP_DigestSignInit(mdctx, NULL, EVP_sha256(), NULL, pkey) ||
        !EVP_DigestSignUpdate(mdctx, content.c_str(), content.length()) ||
        !EVP_DigestSignFinal(mdctx, signature, &siglen)) {

        std::cerr << "Erro ao assinar o evento: " << ERR_error_string(ERR_get_error(), NULL) << std::endl;
        free(signature);
        EVP_PKEY_free(pkey);
        EVP_MD_CTX_free(mdctx);
        return "";
    }

    std::string signed_event(reinterpret_cast<char*>(signature), siglen);
    free(signature);
    EVP_PKEY_free(pkey);
    EVP_MD_CTX_free(mdctx);

    return signed_event;
}

bool verify_signature(const Event& event, const std::string& public_key_pem) {
    BIO *bio = BIO_new_mem_buf(public_key_pem.c_str(), -1);
    EVP_PKEY *pkey = PEM_read_bio_PUBKEY(bio, NULL, NULL, NULL);
    BIO_free(bio);

    if (!pkey) {
        // ... tratamento de erro
        return false;
    }

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        // ... tratamento de erro
        EVP_PKEY_free(pkey);
        return false;
    }


    // ... (Calcular o hash SHA256 do event.content)


    int result = EVP_VerifyInit(mdctx, EVP_sha256()) &&
                 EVP_VerifyUpdate(mdctx, event.content.c_str(), event.content.length()) &&
                 EVP_VerifyFinal(mdctx, (const unsigned char*)event.sig.c_str(), event.sig.size(), pkey);

    EVP_PKEY_free(pkey);
    EVP_MD_CTX_free(mdctx);

    return result == 1;
}