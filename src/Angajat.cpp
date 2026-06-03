#include "Angajat.h"
#include "Medic.h"
#include "Asistent.h"
#include "Exceptii.h"

#include <utility>

Angajat::Angajat(int id, std::string nume, std::string cnp, double salariuBaza)
    : Persoana(std::move(nume), std::move(cnp)),
      id_(id), salariuBaza_(salariuBaza) {}

std::shared_ptr<Angajat> Angajat::fromJson(const json::Value& v) {
    const std::string tip = v.get("tip").asSir();

    if (tip == "Medic") {
        return std::make_shared<Medic>(
            v.get("id").asInt(),
            v.get("nume").asSir(),
            v.get("cnp").asSir(),
            v.get("salariuBaza").asNumar(),
            v.get("specializare").asSir(),
            v.are("sporSpecializare") ? v.get("sporSpecializare").asNumar() : 0.25);
    }
    if (tip == "Asistent") {
        return std::make_shared<Asistent>(
            v.get("id").asInt(),
            v.get("nume").asSir(),
            v.get("cnp").asSir(),
            v.get("salariuBaza").asNumar(),
            v.get("sectie").asSir(),
            v.are("sporTura") ? v.get("sporTura").asNumar() : 500.0);
    }
    throw PersistentaException("tip de angajat necunoscut: '" + tip + "'");
}
