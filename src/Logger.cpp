#include "Logger.h"

#include <ctime>
#include <iostream>

Logger::Logger() : caleFisier_("spital.log") {
    fisier_.open(caleFisier_, std::ios::app);
}

Logger::~Logger() {
    if (fisier_.is_open()) {
        fisier_.close();
    }
}

Logger& Logger::instanta() {
    // Initializare lenesa, garantata thread-safe de standard incepand cu C++11.
    static Logger instanta;
    return instanta;
}

void Logger::setFisier(const std::string& cale) {
    if (fisier_.is_open()) {
        fisier_.close();
    }
    caleFisier_ = cale;
    fisier_.open(caleFisier_, std::ios::app);
}

std::string Logger::nivelToString(Nivel nivel) {
    switch (nivel) {
        case Nivel::INFO:    return "INFO";
        case Nivel::ATENTIE: return "ATENTIE";
        case Nivel::CRITIC:  return "CRITIC";
    }
    return "NECUNOSCUT";
}

std::string Logger::acum() {
    std::time_t t = std::time(nullptr);
    // std::localtime e portabil pe toate compilatoarele standard; pentru scopul
    // acestui proiect (un singur fir de executie) este suficient.
    std::tm* tm_local = std::localtime(&t);
    if (tm_local == nullptr) {
        return "0000-00-00 00:00:00";
    }
    char buf[32];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_local);
    return std::string(buf);
}

void Logger::log(Nivel nivel, const std::string& mesaj) {
    const std::string linie =
        "[" + acum() + "] [" + nivelToString(nivel) + "] " + mesaj;

    if (fisier_.is_open()) {
        fisier_ << linie << '\n';
        fisier_.flush();
    }

    // Operatiunile critice sunt afisate si in consola pentru vizibilitate.
    if (nivel == Nivel::CRITIC) {
        std::clog << linie << '\n';
    }
}

void Logger::info(const std::string& mesaj)    { log(Nivel::INFO, mesaj); }
void Logger::atentie(const std::string& mesaj) { log(Nivel::ATENTIE, mesaj); }
void Logger::critic(const std::string& mesaj)  { log(Nivel::CRITIC, mesaj); }
