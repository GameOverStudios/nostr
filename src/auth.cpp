#include "auth.hpp"
#include <iostream>
#include <map>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <string>
#include <random>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

std::map<std::string, std::string> user_db;
std::map<std::string, std::string> session_tokens;

std::string generate_salt(size_t length = 16) {
    std::string salt;
    salt.reserve(length);
    const char* const alphanumeric_chars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(0, strlen(alphanumeric_chars) - 1);

    for (size_t i = 0; i < length; ++i) {
        salt += alphanumeric_chars[distrib(gen)];
    }
    return salt;
}

bool register_user(const std::string& username, const std::string& password) {
    if (user_db.find(username) != user_db.end()) {
        std::cerr << "Usuário já registrado." << std::endl;
        return false;
    }

    // Gerar um sal aleatório
    std::string salt = generate_salt();

    // Hash da senha com sal (SHA-256)
    unsigned char hash[SHA256_DIGEST_LENGTH];
    std::string salted_password = password + salt;
    SHA256(reinterpret_cast<const unsigned char*>(salted_password.c_str()), salted_password.size(), hash);
    std::string hashed_password(reinterpret_cast<char*>(hash), SHA256_DIGEST_LENGTH);


    // Armazenar o usuário, a senha com sal e o sal no "banco de dados"
    user_db[username] = hashed_password + ":" + salt; // Armazenando o hash e o sal juntos
    return true;
}

std::string authenticate_user(const std::string& username, const std::string& password) {
    auto it = user_db.find(username);
    if (it == user_db.end()) {
        std::cerr << "Usuário não encontrado." << std::endl;
        return "";
    }

    // Extrair o hash e o sal do banco de dados
    size_t delimiter_pos = it->second.find(':');
    std::string stored_hash = it->second.substr(0, delimiter_pos);
    std::string salt = it->second.substr(delimiter_pos + 1);

    // Calcular o hash da senha fornecida com o sal armazenado
    unsigned char hash[SHA256_DIGEST_LENGTH];
    std::string salted_password = password + salt;
    SHA256(reinterpret_cast<const unsigned char*>(salted_password.c_str()), salted_password.size(), hash);
    std::string hashed_password(reinterpret_cast<char*>(hash), SHA256_DIGEST_LENGTH);

    if (stored_hash == hashed_password) {
        std::string token = generate_session_token(username);
        session_tokens[token] = username;
        return token;
    }

    std::cerr << "Senha incorreta." << std::endl;
    return "";
}

std::string generate_session_token(const std::string& username) {
    return generate_uuid();
}

bool validate_session_token(const std::string& token) {
    return session_tokens.find(token) != session_tokens.end();
}