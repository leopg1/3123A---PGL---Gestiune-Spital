#ifndef SPITAL_INTERNARE_H
#define SPITAL_INTERNARE_H

#include "Pacient.h"
#include "DataOra.h"

#include <memory>
#include <string>

// ---------------------------------------------------------------------------
// Internare: spitalizarea unui pacient intr-un salon, pe un numar de zile.
//
// Este o operatiune CRITICA: crearea ei este inregistrata in Logger de catre
// managerul Spital. Costul total = cost pe zi * numarul de zile.
// ---------------------------------------------------------------------------

class Internare {
    int                      id_;
    std::shared_ptr<Pacient> pacient_;
    std::string              salon_;
    DataOra                  dataInternare_;
    double                   costPeZi_;
    int                      nrZile_;

public:
    Internare(int id, std::shared_ptr<Pacient> pacient, std::string salon,
              const DataOra& dataInternare, double costPeZi, int nrZile);

    int getId() const                              { return id_; }
    const std::shared_ptr<Pacient>& getPacient() const { return pacient_; }
    const std::string& getSalon() const            { return salon_; }
    const DataOra& getDataInternare() const        { return dataInternare_; }
    int getNrZile() const                          { return nrZile_; }
    double getCostPeZi() const                     { return costPeZi_; }

    double calculeazaCost() const { return costPeZi_ * nrZile_; }

    std::string descriere() const;
};

#endif // SPITAL_INTERNARE_H
