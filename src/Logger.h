#ifndef SPITAL_LOGGER_H
#define SPITAL_LOGGER_H

#include <string>
#include <fstream>

// ---------------------------------------------------------------------------
// Logger simplu, implementat ca Singleton.
//
// Inregistreaza operatiunile importante ale aplicatiei intr-un fisier de log
// (cu marca de timp si nivel de severitate). Operatiunile CRITICE (internare,
// emitere factura) sunt afisate suplimentar in consola pentru vizibilitate.
// ---------------------------------------------------------------------------

enum class Nivel { INFO, ATENTIE, CRITIC };

class Logger {
public:
    // Acces la unica instanta a logger-ului.
    static Logger& instanta();

    // Schimba fisierul in care se scriu log-urile (implicit "spital.log").
    void setFisier(const std::string& cale);

    // Scrie un mesaj cu nivelul dat.
    void log(Nivel nivel, const std::string& mesaj);

    // Comenzi rapide pentru nivelurile uzuale.
    void info(const std::string& mesaj);
    void atentie(const std::string& mesaj);
    void critic(const std::string& mesaj);

    // Interzicem copierea (este un Singleton).
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger();
    ~Logger();

    std::ofstream fisier_;
    std::string   caleFisier_;

    static std::string nivelToString(Nivel nivel);
    static std::string acum(); // marca de timp curenta "AAAA-LL-ZZ HH:MM:SS"
};

#endif // SPITAL_LOGGER_H
