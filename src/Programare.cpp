#include "Programare.h"
#include "Exceptii.h"

#include <utility>

Programare::Programare(int id,
                       std::shared_ptr<Pacient> pacient,
                       std::shared_ptr<Medic> medic,
                       const DataOra& dataOra,
                       std::shared_ptr<ServiciuMedical> serviciu)
    : id_(id),
      pacient_(std::move(pacient)),
      medic_(std::move(medic)),
      dataOra_(dataOra),
      serviciu_(std::move(serviciu)) {
    if (!pacient_) {
        throw ProgramareInvalidaException("pacientul lipseste");
    }
    if (!medic_) {
        throw ProgramareInvalidaException("medicul lipseste");
    }
    if (!serviciu_) {
        throw ProgramareInvalidaException("serviciul medical lipseste");
    }
}

bool Programare::inConflictCu(const Programare& alta) const {
    return medic_->getId() == alta.medic_->getId() &&
           dataOra_ == alta.dataOra_;
}

std::string Programare::descriere() const {
    return "Programare #" + std::to_string(id_) + " | " + dataOra_.toString() +
           " | " + pacient_->getNume() + " la " + medic_->descriere() +
           " | " + serviciu_->descriere();
}
