#ifndef SPITAL_SERVICIUMEDICAL_H
#define SPITAL_SERVICIUMEDICAL_H

#include "Json.h"
#include <string>
#include <memory>

// ---------------------------------------------------------------------------
// ServiciuMedical: clasa de baza ABSTRACTA pentru serviciile facturabile.
//
// Aici se concentreaza cerinta de POLIMORFISM: fiecare tip de serviciu isi
// calculeaza costul dupa propria regula prin metoda virtuala calculeazaCost().
// O factura aadua servicii eterogene (Consultatie, Analiza, Interventie) prin
// pointeri la clasa de baza si insumeaza costurile fara sa stie tipul concret.
//
//      ServiciuMedical (abstracta)
//        |-- Consultatie
//        |-- Analiza
//        |-- Interventie
// ---------------------------------------------------------------------------

class ServiciuMedical {
protected:
    std::string nume_;

public:
    explicit ServiciuMedical(std::string nume);
    virtual ~ServiciuMedical() = default;

    const std::string& getNume() const { return nume_; }

    // Cele doua metode polimorfice cheie.
    virtual double      calculeazaCost() const = 0;
    virtual std::string tipServiciu() const = 0;

    // Reprezentare text pentru afisare pe factura.
    virtual std::string descriere() const;

    // Serializare polimorfica (folosita de factura ca raport JSON).
    virtual json::Value toJson() const;
};

// --- Consultatie: tarif fix, eventual majorat pentru urgente ---
class Consultatie : public ServiciuMedical {
    double tarif_;
    bool   urgenta_;

public:
    Consultatie(std::string nume, double tarif, bool urgenta = false);
    double      calculeazaCost() const override;
    std::string tipServiciu() const override { return "Consultatie"; }
    json::Value toJson() const override;
};

// --- Analiza: cost pe proba, inmultit cu numarul de probe ---
class Analiza : public ServiciuMedical {
    double tarifPerProba_;
    int    nrProbe_;

public:
    Analiza(std::string nume, double tarifPerProba, int nrProbe);
    double      calculeazaCost() const override;
    std::string tipServiciu() const override { return "Analiza"; }
    json::Value toJson() const override;
};

// --- Interventie: tarif de baza + cost pe minut, scalat de complexitate ---
class Interventie : public ServiciuMedical {
    double tarifBaza_;
    int    durataMinute_;
    double factorComplexitate_; // ex: 1.0 simpla, 2.5 complexa

public:
    Interventie(std::string nume, double tarifBaza, int durataMinute,
                double factorComplexitate = 1.0);
    double      calculeazaCost() const override;
    std::string tipServiciu() const override { return "Interventie"; }
    json::Value toJson() const override;
};

#endif // SPITAL_SERVICIUMEDICAL_H
