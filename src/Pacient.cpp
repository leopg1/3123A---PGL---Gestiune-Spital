#include "Pacient.h"

#include <utility>

Pacient::Pacient(int id, std::string nume, std::string cnp, int varsta,
                 std::string diagnostic)
    : Persoana(std::move(nume), std::move(cnp)),
      id_(id), varsta_(varsta), diagnostic_(std::move(diagnostic)) {}

std::string Pacient::descriere() const {
    std::string d = "Pacient #" + std::to_string(id_) + " " + nume_ +
                    ", " + std::to_string(varsta_) + " ani";
    if (!diagnostic_.empty()) {
        d += ", diagnostic: " + diagnostic_;
    }
    return d;
}

json::Value Pacient::toJson() const {
    json::Value v = json::Value::obiect();
    v["id"]         = id_;
    v["nume"]       = nume_;
    v["cnp"]        = cnp_;
    v["varsta"]     = varsta_;
    v["diagnostic"] = diagnostic_;
    return v;
}

Pacient Pacient::fromJson(const json::Value& v) {
    return Pacient(
        v.get("id").asInt(),
        v.get("nume").asSir(),
        v.get("cnp").asSir(),
        v.get("varsta").asInt(),
        v.are("diagnostic") ? v.get("diagnostic").asSir() : "");
}
