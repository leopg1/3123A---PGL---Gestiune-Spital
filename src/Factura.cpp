#include "Factura.h"
#include "Exceptii.h"

#include <sstream>
#include <iomanip>
#include <utility>

Factura::Factura(int id, std::shared_ptr<Pacient> pacient,
                 const DataOra& dataEmitere, double tvaProcent)
    : id_(id),
      pacient_(std::move(pacient)),
      dataEmitere_(dataEmitere),
      costInternare_(0.0),
      tvaProcent_(tvaProcent),
      emisa_(false) {
    if (!pacient_) {
        throw EntitateInexistentaException("factura fara pacient");
    }
    if (tvaProcent_ < 0.0) {
        throw DateInvalideException("procentul de TVA nu poate fi negativ");
    }
}

void Factura::adaugaServiciu(std::shared_ptr<ServiciuMedical> serviciu) {
    if (!serviciu) {
        throw EntitateInexistentaException("serviciu inexistent adaugat pe factura");
    }
    servicii_.push_back(std::move(serviciu));
}

void Factura::setCostInternare(double cost) {
    if (cost < 0.0) {
        throw DateInvalideException("costul internarii nu poate fi negativ");
    }
    costInternare_ = cost;
}

void Factura::setTva(double procent) {
    if (procent < 0.0) {
        throw DateInvalideException("procentul de TVA nu poate fi negativ");
    }
    tvaProcent_ = procent;
}

double Factura::subtotal() const {
    double s = costInternare_;
    for (const auto& serviciu : servicii_) {
        s += serviciu->calculeazaCost(); // apel POLIMORFIC
    }
    return s;
}

double Factura::valoareTva() const {
    return subtotal() * tvaProcent_;
}

double Factura::total() const {
    return subtotal() + valoareTva();
}

void Factura::emite() {
    emisa_ = true;
}

json::Value Factura::toJson() const {
    json::Value v = json::Value::obiect();
    v["id"]            = id_;
    v["pacient"]       = pacient_->getNume();
    v["dataEmitere"]   = dataEmitere_.toString();
    v["costInternare"] = costInternare_;
    v["tvaProcent"]    = tvaProcent_;
    v["subtotal"]      = subtotal();
    v["tva"]           = valoareTva();
    v["total"]         = total();
    v["emisa"]         = emisa_;

    json::Value lista = json::Value::array();
    for (const auto& serviciu : servicii_) {
        lista.push(serviciu->toJson());
    }
    v["servicii"] = lista;
    return v;
}

std::string Factura::descriere() const {
    std::ostringstream os;
    os << std::fixed << std::setprecision(2);
    os << "===== FACTURA #" << id_ << " =====\n";
    os << "Pacient: " << pacient_->getNume() << "\n";
    os << "Data emiterii: " << dataEmitere_.toString() << "\n";
    os << "---------------------------------\n";
    for (const auto& serviciu : servicii_) {
        os << "  - " << serviciu->descriere() << "\n";
    }
    if (costInternare_ > 0.0) {
        os << "  - Internare: " << costInternare_ << " lei\n";
    }
    os << "---------------------------------\n";
    os << "Subtotal: " << subtotal() << " lei\n";
    if (tvaProcent_ > 0.0) {
        os << "TVA (" << (tvaProcent_ * 100.0) << "%): " << valoareTva() << " lei\n";
    }
    os << "TOTAL: " << total() << " lei\n";
    os << "Stare: " << (emisa_ ? "EMISA" : "ciorna") << "\n";
    return os.str();
}
