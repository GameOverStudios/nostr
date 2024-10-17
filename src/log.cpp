#include "log.hpp"
#include <fstream>
#include <chrono>
#include <ctime>

// Função para registrar uma ação no log
void log_action(const std::string& action) {
    std::ofstream logfile("log.txt", std::ios_base::app);
    if (!logfile.is_open()) {
        std::cerr << "Erro ao abrir o arquivo de log." << std::endl;
        return;
    }

    // Pegar o timestamp atual
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    // Escrever no log
    logfile << "[" << std::ctime(&now_time) << "] " << action << std::endl;
    logfile.close();
}
