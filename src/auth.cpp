#include "auth.hpp"
#include <iostream>
#include <map>
#include <openssl/sha.h>

// Mapa para armazenar as credenciais dos usuários (substituir por banco de dados real)
std::map<std::string, std::string> user_db;  // username -> hashed_password
std::map<std::string, std::string> session_tokens;  // token -> username

// Função para registrar um novo usuário
bool register_user(const std::string& username, const std::string& password) {
    if (user_db.find(username) != user_db.end()) {
        std::cerr << "Usuário já registrado." << std::endl;
        return false;
    }

    // Hash da senha (SHA-256)
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(password.c_str()), password.size(), hash);
    std::string hashed_password(reinterpret_cast<char*>(hash), SHA256_DIGEST_LENGTH);

    // Armazenar o usuário e a senha no "banco de dados"
    user_db[username] = hashed_password;
    return true;
}

// Função para autenticar um usuário
std::string authenticate_user(const std::string& username, const std::string& password) {
    auto it = user_db.find(username);
    if (it == user_db.end()) {
        std::cerr << "Usuário não encontrado." << std::endl;
        return "";
    }

    // Verificação de senha
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(password.c_str()), password.size(), hash);
    std::string hashed_password(reinterpret_cast<char*>(hash), SHA256_DIGEST_LENGTH);

    if (it->second == hashed_password) {
        std::string token = generate_session_token(username);
        session_tokens[token] = username;
        return token;
    }

    std::cerr << "Senha incorreta." << std::endl;
    return "";
}

// Função para gerar um token de sessão
std::string generate_session_token(const std::string& username) {
    // Gerar um token simples baseado em UUID
    return generate_uuid();  // Função reutilizada
}

// Função para validar o token de sessão
bool validate_session_token(const std::string& token) {
    return session_tokens.find(token) != session_tokens.end();
}
