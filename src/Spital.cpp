#include "Spital.h"
#include "Exceptii.h"
#include "Logger.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <utility>

Spital::Spital(std::string nume) : nume_(std::move(nume)) {
    Logger::instanta().info("Spital '" + nume_ + "' initializat.");
}

// ===========================================================================
// Pacienti
// ===========================================================================
std::shared_ptr<Pacient> Spital::adaugaPacient(std::shared_ptr<Pacient> pacient) {
    if (!pacient) {
        throw EntitateInexistentaException("pacient nul adaugat in spital");
    }
    for (const auto& p : pacienti_) {
        if (p->getId() == pacient->getId()) {
            throw DateInvalideException("exista deja un pacient cu id-ul " +
                                        std::to_string(pacient->getId()));
        }
    }
    pacienti_.push_back(pacient);
    Logger::instanta().info("Pacient adaugat: " + pacient->descriere());
    return pacient;
}

std::shared_ptr<Pacient> Spital::gasestePacient(int id) const {
    for (const auto& p : pacienti_) {
        if (p->getId() == id) return p;
    }
    throw EntitateInexistentaException("pacientul cu id-ul " + std::to_string(id) +
                                       " nu exista");
}

// ===========================================================================
// Angajati
// ===========================================================================
std::shared_ptr<Angajat> Spital::adaugaAngajat(std::shared_ptr<Angajat> angajat) {
    if (!angajat) {
        throw EntitateInexistentaException("angajat nul adaugat in spital");
    }
    angajati_.push_back(angajat);
    Logger::instanta().info("Angajat adaugat: " + angajat->descriere());
    return angajat;
}

std::shared_ptr<Medic> Spital::gasesteMedic(int id) const {
    for (const auto& a : angajati_) {
        auto medic = std::dynamic_pointer_cast<Medic>(a); // RTTI / polimorfism
        if (medic && medic->getId() == id) return medic;
    }
    throw EntitateInexistentaException("medicul cu id-ul " + std::to_string(id) +
                                       " nu exista");
}

std::vector<std::shared_ptr<Medic>> Spital::getMedici() const {
    std::vector<std::shared_ptr<Medic>> rezultat;
    for (const auto& a : angajati_) {
        if (auto medic = std::dynamic_pointer_cast<Medic>(a)) {
            rezultat.push_back(medic);
        }
    }
    return rezultat;
}

std::vector<std::shared_ptr<Asistent>> Spital::getAsistenti() const {
    std::vector<std::shared_ptr<Asistent>> rezultat;
    for (const auto& a : angajati_) {
        if (auto asistent = std::dynamic_pointer_cast<Asistent>(a)) {
            rezultat.push_back(asistent);
        }
    }
    return rezultat;
}

// ===========================================================================
// Programari
// ===========================================================================
const Programare& Spital::adaugaProgramare(const Programare& programare) {
    // Regula de business: un medic nu poate avea doua programari la aceeasi ora.
    for (const auto& existenta : programari_) {
        if (existenta.inConflictCu(programare)) {
            throw ProgramareInvalidaException(
                "medicul " + programare.getMedic()->getNume() +
                " are deja o programare la " + programare.getDataOra().toString());
        }
    }
    programari_.push_back(programare);
    Logger::instanta().info("Programare adaugata: " + programare.descriere());
    return programari_.back();
}

// ===========================================================================
// Operatiuni critice
// ===========================================================================
const Internare& Spital::interneazaPacient(const Internare& internare) {
    internari_.push_back(internare);
    // Internarea este o operatiune CRITICA -> log + afisare in consola.
    Logger::instanta().critic("INTERNARE: " + internare.descriere());
    return internari_.back();
}

const Factura& Spital::emiteFactura(Factura factura) {
    factura.emite();
    facturi_.push_back(std::move(factura));
    const Factura& ref = facturi_.back();
    // Emiterea facturii este o operatiune CRITICA.
    std::ostringstream os;
    os << "EMITERE FACTURA #" << ref.getId()
       << " pentru " << ref.getPacient()->getNume()
       << ", total " << ref.total() << " lei";
    Logger::instanta().critic(os.str());
    return ref;
}

// ===========================================================================
// Filtrare dupa diagnostic (cerinta facultativa)
// ===========================================================================
std::vector<std::shared_ptr<Pacient>>
Spital::filtreazaDupaDiagnostic(const std::string& diagnostic) const {
    std::vector<std::shared_ptr<Pacient>> rezultat;
    std::copy_if(pacienti_.begin(), pacienti_.end(), std::back_inserter(rezultat),
                 [&diagnostic](const std::shared_ptr<Pacient>& p) {
                     return p->getDiagnostic() == diagnostic;
                 });
    return rezultat;
}

// ===========================================================================
// Persistenta JSON (cerinta facultativa)
// ===========================================================================
json::Value Spital::toJson() const {
    json::Value root = json::Value::obiect();
    root["spital"] = nume_;

    json::Value listaPacienti = json::Value::array();
    for (const auto& p : pacienti_) {
        listaPacienti.push(p->toJson());
    }
    root["pacienti"] = listaPacienti;

    json::Value listaAngajati = json::Value::array();
    for (const auto& a : angajati_) {
        listaAngajati.push(a->toJson());
    }
    root["angajati"] = listaAngajati;

    json::Value listaFacturi = json::Value::array();
    for (const auto& f : facturi_) {
        listaFacturi.push(f.toJson());
    }
    root["facturi"] = listaFacturi;

    return root;
}

void Spital::salveaza(const std::string& cale) const {
    std::ofstream fisier(cale);
    if (!fisier.is_open()) {
        throw PersistentaException("nu pot deschide fisierul '" + cale + "' pentru scriere");
    }
    fisier << toJson().dump(2);
    Logger::instanta().info("Stare salvata in '" + cale + "'.");
}

void Spital::incarca(const std::string& cale) {
    std::ifstream fisier(cale);
    if (!fisier.is_open()) {
        throw PersistentaException("nu pot deschide fisierul '" + cale + "' pentru citire");
    }
    std::stringstream buffer;
    buffer << fisier.rdbuf();

    json::Value root;
    try {
        root = json::Value::parse(buffer.str());
    } catch (const std::exception& e) {
        throw PersistentaException(std::string("JSON invalid: ") + e.what());
    }

    pacienti_.clear();
    angajati_.clear();

    if (root.are("spital")) {
        nume_ = root.get("spital").asSir();
    }
    if (root.are("pacienti")) {
        for (const auto& v : root.get("pacienti").elemente()) {
            pacienti_.push_back(std::make_shared<Pacient>(Pacient::fromJson(v)));
        }
    }
    if (root.are("angajati")) {
        for (const auto& v : root.get("angajati").elemente()) {
            angajati_.push_back(Angajat::fromJson(v));
        }
    }
    Logger::instanta().info("Stare incarcata din '" + cale + "' (" +
                            std::to_string(pacienti_.size()) + " pacienti, " +
                            std::to_string(angajati_.size()) + " angajati).");
}
