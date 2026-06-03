#ifndef SPITAL_ANGAJAT_H
#define SPITAL_ANGAJAT_H

#include "Persoana.h"
#include "Json.h"
#include <string>
#include <memory>

// ---------------------------------------------------------------------------
// Angajat: clasa de baza ABSTRACTA pentru personalul spitalului.
//
// Cerinta obligatorie de mostenire:  Angajat -> Medic, Asistent.
//
// Defineste salariul de baza si o metoda virtuala pura calculeazaSalariu(),
// suprascrisa diferit de fiecare tip de angajat (al doilea exemplu de
// polimorfism, pe langa calculul costului serviciilor medicale).
// ---------------------------------------------------------------------------

class Angajat : public Persoana {
protected:
    int    id_;
    double salariuBaza_;

public:
    Angajat(int id, std::string nume, std::string cnp, double salariuBaza);

    int    getId() const          { return id_; }
    double getSalariuBaza() const { return salariuBaza_; }

    // Polimorfism: salariul final depinde de tipul concret de angajat.
    virtual double calculeazaSalariu() const = 0;

    // tip() ramane pur virtuala (mostenita de la Persoana) -> Angajat e abstracta.

    // --- Persistenta JSON ---
    // Serializeaza un angajat (cu un camp "tip" discriminator) si reconstruieste
    // tipul concret corect (Medic / Asistent) din JSON.
    virtual json::Value toJson() const = 0;
    static std::shared_ptr<Angajat> fromJson(const json::Value& v);
};

#endif // SPITAL_ANGAJAT_H
