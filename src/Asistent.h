#ifndef SPITAL_ASISTENT_H
#define SPITAL_ASISTENT_H

#include "Angajat.h"
#include <string>

// ---------------------------------------------------------------------------
// Asistent: angajat care sprijina activitatea medicala intr-o sectie.
//
// Salariul = salariu de baza + spor fix de tura (bonus in lei).
// ---------------------------------------------------------------------------

class Asistent : public Angajat {
    std::string sectie_;
    double      sporTura_; // bonus fix in lei

public:
    Asistent(int id, std::string nume, std::string cnp, double salariuBaza,
             std::string sectie, double sporTura = 500.0);

    const std::string& getSectie() const { return sectie_; }
    double getSporTura() const            { return sporTura_; }

    double calculeazaSalariu() const override;

    std::string tip() const override { return "Asistent"; }
    std::string descriere() const override;

    json::Value toJson() const override;
};

#endif // SPITAL_ASISTENT_H
