#include "Internare.h"
#include "Exceptii.h"

#include <sstream>
#include <iomanip>
#include <utility>

Internare::Internare(int id, std::shared_ptr<Pacient> pacient, std::string salon,
                     const DataOra& dataInternare, double costPeZi, int nrZile)
    : id_(id),
      pacient_(std::move(pacient)),
      salon_(std::move(salon)),
      dataInternare_(dataInternare),
      costPeZi_(costPeZi),
      nrZile_(nrZile) {
    if (!pacient_) {
        throw EntitateInexistentaException("internare fara pacient");
    }
    if (nrZile_ < 1) {
        throw DateInvalideException("numarul de zile de internare trebuie sa fie >= 1");
    }
    if (costPeZi_ < 0.0) {
        throw DateInvalideException("costul pe zi nu poate fi negativ");
    }
}

std::string Internare::descriere() const {
    std::ostringstream os;
    os << "Internare #" << id_ << " | " << pacient_->getNume()
       << " | salon " << salon_ << " | " << dataInternare_.toString()
       << " | " << nrZile_ << " zile | cost "
       << std::fixed << std::setprecision(2) << calculeazaCost() << " lei";
    return os.str();
}
