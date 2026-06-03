#include "Medic.h"

#include <utility>

Medic::Medic(int id, std::string nume, std::string cnp, double salariuBaza,
             std::string specializare, double sporSpecializare)
    : Angajat(id, std::move(nume), std::move(cnp), salariuBaza),
      specializare_(std::move(specializare)),
      sporSpecializare_(sporSpecializare) {}

double Medic::calculeazaSalariu() const {
    return salariuBaza_ * (1.0 + sporSpecializare_);
}

std::string Medic::descriere() const {
    return "Dr. " + nume_ + " (" + specializare_ + ")";
}

json::Value Medic::toJson() const {
    json::Value v = json::Value::obiect();
    v["tip"]              = std::string("Medic");
    v["id"]               = id_;
    v["nume"]             = nume_;
    v["cnp"]              = cnp_;
    v["salariuBaza"]      = salariuBaza_;
    v["specializare"]     = specializare_;
    v["sporSpecializare"] = sporSpecializare_;
    return v;
}
