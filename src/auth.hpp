#ifndef AUTH_HPP
#define AUTH_HPP

#include <string>

// Declaração das funções de autenticação
bool register_user(const std::string& username, const std::string& password);
std::string authenticate_user(const std::string& username, const std::string& password);
bool validate_session_token(const std::string& token);

// Função para gerar tokens de sessão
std::string generate_session_token(const std::string& username);

#endif // AUTH_HPP
