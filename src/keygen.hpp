#ifndef KEYGEN_HPP
#define KEYGEN_HPP

#include <utility>
#include <string>

std::pair<std::string, std::string> generate_rsa_keypair(int key_size = 2048);

#endif // KEYGEN_HPP