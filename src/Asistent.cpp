#include "Asistent.h"

#include <utility>

Asistent::Asistent(int id, std::string nume, std::string cnp, double salariuBaza,
                   std::string sectie, double sporTura)
    : Angajat(id, std::move(nume), std::move(cnp), salariuBaza),
      sectie_(std::move(sectie)), sporTura_(sporTura) {}

double Asistent::calculeazaSalariu() const {
    return salariuBaza_ + sporTura_;
}

std::string Asistent::descriere() const {
    return "As. med. " + nume_ + " (sectia " + sectie_ + ")";
}

json::Value Asistent::toJson() const {
    json::Value v = json::Value::obiect();
    v["tip"]         = std::string("Asistent");
    v["id"]          = id_;
    v["nume"]        = nume_;
    v["cnp"]         = cnp_;
    v["salariuBaza"] = salariuBaza_;
    v["sectie"]      = sectie_;
    v["sporTura"]    = sporTura_;
    return v;
}
