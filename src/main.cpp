// ===========================================================================
//  Aplicatie de gestiune a unui spital  -  Proiect POO (C++17)
//
//  Punctul de intrare: populeaza spitalul cu date demonstrative si ofera un
//  meniu interactiv in consola pentru toate operatiunile (pacienti, medici,
//  programari, internari, facturi, filtrare, persistenta JSON).
//
//  Toate erorile de domeniu sunt tratate prin exceptii personalizate, captate
//  central in bucla de meniu.
// ===========================================================================

#include "Spital.h"
#include "ServiciuMedical.h"
#include "FacturaFactory.h"
#include "Logger.h"
#include "Exceptii.h"

#include <iostream>
#include <limits>
#include <memory>
#include <string>

namespace {

// --- Ajutoare pentru citirea de la tastatura -------------------------------

std::string citesteLinie(const std::string& prompt) {
    std::cout << prompt;
    std::string linie;
    std::getline(std::cin, linie);
    return linie;
}

int citesteInt(const std::string& prompt) {
    while (true) {
        std::string linie = citesteLinie(prompt);
        try {
            return std::stoi(linie);
        } catch (...) {
            std::cout << "  Valoare numerica invalida, incearca din nou.\n";
        }
    }
}

double citesteDouble(const std::string& prompt) {
    while (true) {
        std::string linie = citesteLinie(prompt);
        try {
            return std::stod(linie);
        } catch (...) {
            std::cout << "  Valoare numerica invalida, incearca din nou.\n";
        }
    }
}

void linieSeparatoare() {
    std::cout << "------------------------------------------------------------\n";
}

// --- Date demonstrative ----------------------------------------------------

void populeazaDemo(Spital& spital) {
    // Pacienti (CNP-uri fictive de 13 cifre).
    spital.adaugaPacient(std::make_shared<Pacient>(1, "Ion Popescu",      "1900101221144", 35, "pneumonie"));
    spital.adaugaPacient(std::make_shared<Pacient>(2, "Maria Ionescu",    "2920202221155", 42, "fractura"));
    spital.adaugaPacient(std::make_shared<Pacient>(3, "Andrei Vasilescu", "1850303221166", 50, "pneumonie"));
    spital.adaugaPacient(std::make_shared<Pacient>(4, "Elena Dumitru",    "2780404221177", 29, "hipertensiune"));

    // Angajati: medici si asistenti (mostenire Angajat -> Medic/Asistent).
    spital.adaugaAngajat(std::make_shared<Medic>(10, "Mihai Popa",     "1700505221188", 8000, "Cardiologie", 0.30));
    spital.adaugaAngajat(std::make_shared<Medic>(11, "Ana Georgescu",  "2720606221199", 7500, "Ortopedie",   0.25));
    spital.adaugaAngajat(std::make_shared<Asistent>(20, "Ioana Stan",  "2880707221100", 4000, "Cardiologie", 600));
    spital.adaugaAngajat(std::make_shared<Asistent>(21, "Radu Marin",  "1830808221111", 3800, "Urgente",     700));

    // Programari demonstrative.
    auto consultatie = std::make_shared<Consultatie>("Consult cardiologic", 200.0, false);
    spital.adaugaProgramare(Programare(1, spital.gasestePacient(1), spital.gasesteMedic(10),
                                       DataOra(2025, 6, 12, 9, 0), consultatie));

    auto interventie = std::make_shared<Interventie>("Reducere fractura", 500.0, 90, 1.5);
    spital.adaugaProgramare(Programare(2, spital.gasestePacient(2), spital.gasesteMedic(11),
                                       DataOra(2025, 6, 12, 10, 30), interventie));
}

// --- Actiuni de meniu ------------------------------------------------------

void afiseazaPacienti(const Spital& spital) {
    linieSeparatoare();
    std::cout << "PACIENTI (" << spital.getPacienti().size() << "):\n";
    for (const auto& p : spital.getPacienti()) {
        std::cout << "  " << p->descriere() << "\n";
    }
}

void afiseazaAngajati(const Spital& spital) {
    linieSeparatoare();
    std::cout << "MEDICI:\n";
    for (const auto& m : spital.getMedici()) {
        std::cout << "  " << m->descriere()
                  << " | salariu: " << m->calculeazaSalariu() << " lei\n";
    }
    std::cout << "ASISTENTI:\n";
    for (const auto& a : spital.getAsistenti()) {
        std::cout << "  " << a->descriere()
                  << " | salariu: " << a->calculeazaSalariu() << " lei\n";
    }
}

void afiseazaProgramari(const Spital& spital) {
    linieSeparatoare();
    std::cout << "PROGRAMARI (" << spital.getProgramari().size() << "):\n";
    for (const auto& p : spital.getProgramari()) {
        std::cout << "  " << p.descriere() << "\n";
    }
}

void adaugaPacientInteractiv(Spital& spital) {
    int id        = citesteInt("  Id pacient: ");
    std::string nume = citesteLinie("  Nume: ");
    std::string cnp  = citesteLinie("  CNP (13 cifre): ");
    int varsta    = citesteInt("  Varsta: ");
    std::string diag = citesteLinie("  Diagnostic: ");
    spital.adaugaPacient(std::make_shared<Pacient>(id, nume, cnp, varsta, diag));
    std::cout << "  Pacient adaugat.\n";
}

// Construieste interactiv un serviciu medical (demonstreaza polimorfismul).
std::shared_ptr<ServiciuMedical> alegeServiciu() {
    std::cout << "  Tip serviciu: 1=Consultatie  2=Analiza  3=Interventie\n";
    int tip = citesteInt("  Alegere: ");
    std::string nume = citesteLinie("  Nume serviciu: ");
    switch (tip) {
        case 1: {
            double tarif = citesteDouble("  Tarif: ");
            int urg = citesteInt("  Urgenta? (1=da, 0=nu): ");
            return std::make_shared<Consultatie>(nume, tarif, urg == 1);
        }
        case 2: {
            double tarif = citesteDouble("  Tarif per proba: ");
            int probe = citesteInt("  Numar probe: ");
            return std::make_shared<Analiza>(nume, tarif, probe);
        }
        case 3: {
            double tarif = citesteDouble("  Tarif de baza: ");
            int durata = citesteInt("  Durata (minute): ");
            double cplx = citesteDouble("  Factor complexitate (ex: 1.5): ");
            return std::make_shared<Interventie>(nume, tarif, durata, cplx);
        }
        default:
            throw DateInvalideException("tip de serviciu necunoscut");
    }
}

void adaugaProgramareInteractiv(Spital& spital) {
    int idProg   = citesteInt("  Id programare: ");
    int idPac    = citesteInt("  Id pacient: ");
    int idMed    = citesteInt("  Id medic: ");
    std::string data = citesteLinie("  Data si ora (AAAA-LL-ZZ HH:MM): ");
    auto serviciu = alegeServiciu();

    Programare prog(idProg, spital.gasestePacient(idPac), spital.gasesteMedic(idMed),
                    DataOra::dinString(data), serviciu);
    spital.adaugaProgramare(prog);
    std::cout << "  Programare adaugata.\n";
}

void interneazaInteractiv(Spital& spital) {
    int idInt  = citesteInt("  Id internare: ");
    int idPac  = citesteInt("  Id pacient: ");
    std::string salon = citesteLinie("  Salon: ");
    std::string data  = citesteLinie("  Data internarii (AAAA-LL-ZZ): ");
    double costZi = citesteDouble("  Cost pe zi: ");
    int zile = citesteInt("  Numar de zile: ");

    Internare internare(idInt, spital.gasestePacient(idPac), salon,
                        DataOra::dinString(data), costZi, zile);
    spital.interneazaPacient(internare);
    std::cout << "  Pacient internat.\n";
}

void emiteFacturaInteractiv(Spital& spital) {
    int idFact = citesteInt("  Id factura: ");
    int idPac  = citesteInt("  Id pacient: ");
    auto pacient = spital.gasestePacient(idPac);

    std::vector<std::shared_ptr<ServiciuMedical>> servicii;
    while (true) {
        servicii.push_back(alegeServiciu());
        std::string raspuns = citesteLinie("  Mai adaugi un serviciu? (d/n): ");
        if (raspuns != "d" && raspuns != "D") break;
    }
    double tva = citesteDouble("  Procent TVA (ex: 0.09, sau 0): ");

    // Folosim sablonul Factory pentru a crea factura.
    Factura factura = FacturaFactory::dinServicii(idFact, pacient, servicii, DataOra(), tva);
    const Factura& emisa = spital.emiteFactura(factura);
    linieSeparatoare();
    std::cout << emisa.descriere();
}

void filtreazaInteractiv(const Spital& spital) {
    std::string diag = citesteLinie("  Diagnostic cautat: ");
    auto rezultat = spital.filtreazaDupaDiagnostic(diag);
    linieSeparatoare();
    std::cout << "Pacienti cu diagnosticul '" << diag << "' (" << rezultat.size() << "):\n";
    for (const auto& p : rezultat) {
        std::cout << "  " << p->descriere() << "\n";
    }
}

void afiseazaMeniu() {
    linieSeparatoare();
    std::cout << "  MENIU - Gestiune Spital\n";
    linieSeparatoare();
    std::cout << "   1. Afiseaza pacienti\n"
                 "   2. Afiseaza medici si asistenti (cu salarii)\n"
                 "   3. Afiseaza programari\n"
                 "   4. Adauga pacient\n"
                 "   5. Adauga programare\n"
                 "   6. Interneaza pacient        (operatiune critica)\n"
                 "   7. Emite factura             (operatiune critica)\n"
                 "   8. Filtreaza pacienti dupa diagnostic\n"
                 "   9. Salveaza datele in JSON\n"
                 "  10. Incarca datele din JSON\n"
                 "  11. Afiseaza raportul JSON complet\n"
                 "   0. Iesire\n";
}

} // namespace

int main() {
    Logger::instanta().setFisier("spital.log");
    Logger::instanta().info("Aplicatia a pornit.");

    Spital spital("Spitalul Judetean de Urgenta");
    try {
        populeazaDemo(spital);
    } catch (const SpitalException& e) {
        std::cerr << "Eroare la initializarea datelor demo: " << e.what() << "\n";
    }

    std::cout << "Bun venit la aplicatia de gestiune a spitalului \""
              << spital.getNume() << "\".\n";

    bool ruleaza = true;
    while (ruleaza) {
        afiseazaMeniu();
        std::string optiune = citesteLinie("  Alegere: ");
        if (std::cin.eof()) break; // iesire la EOF (ex: input redirectionat)

        try {
            if      (optiune == "1")  afiseazaPacienti(spital);
            else if (optiune == "2")  afiseazaAngajati(spital);
            else if (optiune == "3")  afiseazaProgramari(spital);
            else if (optiune == "4")  adaugaPacientInteractiv(spital);
            else if (optiune == "5")  adaugaProgramareInteractiv(spital);
            else if (optiune == "6")  interneazaInteractiv(spital);
            else if (optiune == "7")  emiteFacturaInteractiv(spital);
            else if (optiune == "8")  filtreazaInteractiv(spital);
            else if (optiune == "9")  { spital.salveaza("data/spital.json");
                                        std::cout << "  Date salvate in data/spital.json\n"; }
            else if (optiune == "10") { spital.incarca("data/spital.json");
                                        std::cout << "  Date incarcate din data/spital.json\n"; }
            else if (optiune == "11") std::cout << spital.toJson().dump(2) << "\n";
            else if (optiune == "0")  ruleaza = false;
            else std::cout << "  Optiune necunoscuta.\n";
        } catch (const SpitalException& e) {
            // Captam central toate erorile de domeniu.
            std::cout << "  [EROARE] " << e.what() << "\n";
            Logger::instanta().atentie(std::string("Eroare tratata: ") + e.what());
        }
    }

    std::cout << "La revedere!\n";
    Logger::instanta().info("Aplicatia s-a inchis.");
    return 0;
}
