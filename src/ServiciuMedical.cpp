#include "ServiciuMedical.h"

#include <sstream>
#include <iomanip>
#include <utility>

// ---------------------------------------------------------------------------
// ServiciuMedical (baza)
// ---------------------------------------------------------------------------
ServiciuMedical::ServiciuMedical(std::string nume) : nume_(std::move(nume)) {}

std::string ServiciuMedical::descriere() const {
    std::ostringstream os;
    os << tipServiciu() << " - " << nume_ << ": "
       << std::fixed << std::setprecision(2) << calculeazaCost() << " lei";
    return os.str();
}

json::Value ServiciuMedical::toJson() const {
    json::Value v = json::Value::obiect();
    v["tip"]  = tipServiciu();
    v["nume"] = nume_;
    v["cost"] = calculeazaCost();
    return v;
}

// ---------------------------------------------------------------------------
// Consultatie
// ---------------------------------------------------------------------------
Consultatie::Consultatie(std::string nume, double tarif, bool urgenta)
    : ServiciuMedical(std::move(nume)), tarif_(tarif), urgenta_(urgenta) {}

double Consultatie::calculeazaCost() const {
    // Urgentele se taxeaza cu un spor de 50%.
    return urgenta_ ? tarif_ * 1.5 : tarif_;
}

json::Value Consultatie::toJson() const {
    json::Value v = ServiciuMedical::toJson();
    v["urgenta"] = urgenta_;
    return v;
}

// ---------------------------------------------------------------------------
// Analiza
// ---------------------------------------------------------------------------
Analiza::Analiza(std::string nume, double tarifPerProba, int nrProbe)
    : ServiciuMedical(std::move(nume)),
      tarifPerProba_(tarifPerProba), nrProbe_(nrProbe) {}

double Analiza::calculeazaCost() const {
    return tarifPerProba_ * nrProbe_;
}

json::Value Analiza::toJson() const {
    json::Value v = ServiciuMedical::toJson();
    v["nrProbe"] = nrProbe_;
    return v;
}

// ---------------------------------------------------------------------------
// Interventie
// ---------------------------------------------------------------------------
Interventie::Interventie(std::string nume, double tarifBaza, int durataMinute,
                         double factorComplexitate)
    : ServiciuMedical(std::move(nume)),
      tarifBaza_(tarifBaza), durataMinute_(durataMinute),
      factorComplexitate_(factorComplexitate) {}

double Interventie::calculeazaCost() const {
    // Tarif de baza + 5 lei pe minut de operatie, totul scalat de complexitate.
    const double costPeMinut = 5.0;
    return (tarifBaza_ + durataMinute_ * costPeMinut) * factorComplexitate_;
}

json::Value Interventie::toJson() const {
    json::Value v = ServiciuMedical::toJson();
    v["durataMinute"]       = durataMinute_;
    v["factorComplexitate"] = factorComplexitate_;
    return v;
}
