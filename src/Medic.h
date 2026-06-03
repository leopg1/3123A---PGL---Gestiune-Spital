#ifndef SPITAL_MEDIC_H
#define SPITAL_MEDIC_H

#include "Angajat.h"
#include <string>

// ---------------------------------------------------------------------------
// Medic: angajat specializat care consulta pacienti si efectueaza interventii.
//
// Salariul = salariu de baza + spor de specializare (procent).
// ---------------------------------------------------------------------------

class Medic : public Angajat {
    std::string specializare_;
    double      sporSpecializare_; // ex: 0.25 = +25%

public:
    Medic(int id, std::string nume, std::string cnp, double salariuBaza,
          std::string specializare, double sporSpecializare = 0.25);

    const std::string& getSpecializare() const { return specializare_; }
    double getSporSpecializare() const          { return sporSpecializare_; }

    double calculeazaSalariu() const override;

    std::string tip() const override { return "Medic"; }
    std::string descriere() const override;

    json::Value toJson() const override;
};

#endif // SPITAL_MEDIC_H
