#ifndef SPITAL_EXCEPTII_H
#define SPITAL_EXCEPTII_H

#include <stdexcept>
#include <string>

// ---------------------------------------------------------------------------
// Ierarhie de exceptii personalizate pentru aplicatia de gestiune a spitalului.
//
// Toate exceptiile deriva dintr-o radacina comuna (SpitalException) astfel
// incat un singur catch(const SpitalException&) poate prinde orice eroare de
// domeniu, dar pastram si tipuri specifice pentru tratare granulara.
// ---------------------------------------------------------------------------

// Radacina tuturor erorilor de domeniu din aplicatie.
class SpitalException : public std::runtime_error {
public:
    explicit SpitalException(const std::string& mesaj)
        : std::runtime_error(mesaj) {}
};

// Date de intrare invalide (CNP gresit, data calendaristica imposibila etc.).
class DateInvalideException : public SpitalException {
public:
    explicit DateInvalideException(const std::string& mesaj)
        : SpitalException("Date invalide: " + mesaj) {}
};

// Programare care incalca regulile de business (conflict de orar, medic ocupat).
class ProgramareInvalidaException : public SpitalException {
public:
    explicit ProgramareInvalidaException(const std::string& mesaj)
        : SpitalException("Programare invalida: " + mesaj) {}
};

// Cautarea unei entitati (pacient, medic, factura) care nu exista.
class EntitateInexistentaException : public SpitalException {
public:
    explicit EntitateInexistentaException(const std::string& mesaj)
        : SpitalException("Entitate inexistenta: " + mesaj) {}
};

// Erori aparute la salvarea / incarcarea datelor (persistenta JSON).
class PersistentaException : public SpitalException {
public:
    explicit PersistentaException(const std::string& mesaj)
        : SpitalException("Eroare de persistenta: " + mesaj) {}
};

#endif // SPITAL_EXCEPTII_H
