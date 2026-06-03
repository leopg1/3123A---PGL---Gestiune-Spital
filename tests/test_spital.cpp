// ===========================================================================
//  Teste unitare pentru aplicatia de gestiune a spitalului.
//
//  Nu folosim biblioteci externe; un mic set de macrouri tine evidenta
//  verificarilor. Programul intoarce cod 0 daca toate testele trec si 1 daca
//  exista esecuri (potrivit pentru "make test" / integrare continua).
// ===========================================================================

#include "DataOra.h"
#include "Pacient.h"
#include "Medic.h"
#include "Asistent.h"
#include "ServiciuMedical.h"
#include "Programare.h"
#include "Internare.h"
#include "Factura.h"
#include "FacturaFactory.h"
#include "Spital.h"
#include "Json.h"
#include "Exceptii.h"

#include <cmath>
#include <iostream>
#include <memory>
#include <string>

// --- Mini-framework de testare ---------------------------------------------
namespace {

int g_rulate = 0;
int g_esuate = 0;

bool aprox(double a, double b, double eps = 1e-6) {
    return std::fabs(a - b) < eps;
}

} // namespace

#define VERIFICA(cond)                                                        \
    do {                                                                      \
        ++g_rulate;                                                           \
        if (!(cond)) {                                                        \
            ++g_esuate;                                                       \
            std::cout << "  [ESEC] " << #cond                                 \
                      << "  (linia " << __LINE__ << ")\n";                    \
        }                                                                     \
    } while (0)

#define VERIFICA_ARUNCA(expr, Exceptie)                                       \
    do {                                                                      \
        ++g_rulate;                                                           \
        bool aAruncat = false;                                                \
        try { expr; }                                                         \
        catch (const Exceptie&) { aAruncat = true; }                          \
        catch (...) {}                                                        \
        if (!aAruncat) {                                                      \
            ++g_esuate;                                                       \
            std::cout << "  [ESEC] nu a aruncat " #Exceptie                   \
                      << "  (linia " << __LINE__ << ")\n";                    \
        }                                                                     \
    } while (0)

// ---------------------------------------------------------------------------
// 1. DataOra: validare si comparatii
// ---------------------------------------------------------------------------
void test_dataora() {
    std::cout << "[Test] DataOra\n";

    VERIFICA_ARUNCA(DataOra(2025, 13, 1), DateInvalideException); // luna invalida
    VERIFICA_ARUNCA(DataOra(2025, 2, 29), DateInvalideException); // 2025 nu e bisect
    VERIFICA_ARUNCA(DataOra(2025, 1, 1, 25, 0), DateInvalideException); // ora invalida
    VERIFICA_ARUNCA(DataOra::dinString("data-gresita"), DateInvalideException);

    // 2024 este bisect -> 29 februarie este valid.
    bool ok = true;
    try { DataOra(2024, 2, 29); } catch (...) { ok = false; }
    VERIFICA(ok);

    DataOra a(2025, 6, 12, 9, 0);
    DataOra b(2025, 6, 12, 10, 30);
    VERIFICA(a < b);
    VERIFICA(!(b < a));
    VERIFICA(a == DataOra(2025, 6, 12, 9, 0));
    VERIFICA(a.toString() == "2025-06-12 09:00");

    DataOra d = DataOra::dinString("2025-06-12 09:00");
    VERIFICA(d == a);
}

// ---------------------------------------------------------------------------
// 2. Polimorfism: calculul costului serviciilor medicale
// ---------------------------------------------------------------------------
void test_servicii_polimorfism() {
    std::cout << "[Test] Servicii medicale (polimorfism cost)\n";

    Consultatie consultatie("Consult", 200.0, false);
    Consultatie urgenta("Consult urgenta", 200.0, true);
    Analiza analiza("Set analize", 50.0, 3);
    Interventie interventie("Operatie", 500.0, 90, 1.5);

    VERIFICA(aprox(consultatie.calculeazaCost(), 200.0));
    VERIFICA(aprox(urgenta.calculeazaCost(), 300.0));        // +50%
    VERIFICA(aprox(analiza.calculeazaCost(), 150.0));        // 50 * 3
    VERIFICA(aprox(interventie.calculeazaCost(), 1425.0));   // (500 + 90*5) * 1.5

    // Insumare polimorfica printr-un vector de pointeri la clasa de baza.
    std::vector<std::shared_ptr<ServiciuMedical>> servicii = {
        std::make_shared<Consultatie>("C", 100.0, false),
        std::make_shared<Analiza>("A", 20.0, 2),
        std::make_shared<Interventie>("I", 100.0, 10, 1.0)};
    double suma = 0.0;
    for (const auto& s : servicii) suma += s->calculeazaCost();
    VERIFICA(aprox(suma, 100.0 + 40.0 + 150.0));
}

// ---------------------------------------------------------------------------
// 3. Programari: validare si detectarea conflictelor
// ---------------------------------------------------------------------------
void test_programari() {
    std::cout << "[Test] Programari\n";

    auto pac1 = std::make_shared<Pacient>(1, "Ion", "1900101221144", 30, "gripa");
    auto pac2 = std::make_shared<Pacient>(2, "Ana", "2900101221144", 25, "raceala");
    auto medic = std::make_shared<Medic>(10, "Popa", "1700101221144", 8000, "ORL");
    auto serviciu = std::make_shared<Consultatie>("Consult", 150.0);

    DataOra ora(2025, 6, 12, 9, 0);
    Programare p1(1, pac1, medic, ora, serviciu);
    Programare p2(2, pac2, medic, ora, serviciu); // acelasi medic, aceeasi ora
    Programare p3(3, pac2, medic, DataOra(2025, 6, 12, 10, 0), serviciu);

    VERIFICA(p1.inConflictCu(p2));   // conflict de orar
    VERIFICA(!p1.inConflictCu(p3));  // ore diferite -> fara conflict

    // Programare fara medic -> exceptie.
    VERIFICA_ARUNCA(Programare(4, pac1, nullptr, ora, serviciu),
                    ProgramareInvalidaException);

    // La nivel de spital, a doua programare in conflict trebuie respinsa.
    Spital spital("Test");
    spital.adaugaPacient(pac1);
    spital.adaugaPacient(pac2);
    spital.adaugaAngajat(medic);
    spital.adaugaProgramare(p1);
    VERIFICA_ARUNCA(spital.adaugaProgramare(p2), ProgramareInvalidaException);

    bool ok = true;
    try { spital.adaugaProgramare(p3); } catch (...) { ok = false; }
    VERIFICA(ok); // ora diferita -> acceptata
}

// ---------------------------------------------------------------------------
// 4. Factura: calculul totalului (servicii + internare + TVA)
// ---------------------------------------------------------------------------
void test_factura() {
    std::cout << "[Test] Factura\n";

    auto pac = std::make_shared<Pacient>(1, "Ion", "1900101221144", 30, "pneumonie");

    Factura f(100, pac);
    f.adaugaServiciu(std::make_shared<Consultatie>("Consult", 200.0));
    f.adaugaServiciu(std::make_shared<Analiza>("Analize", 50.0, 3));      // 150
    f.adaugaServiciu(std::make_shared<Interventie>("Operatie", 500.0, 90, 1.5)); // 1425

    VERIFICA(aprox(f.subtotal(), 1775.0));
    VERIFICA(aprox(f.total(), 1775.0)); // fara TVA

    f.setTva(0.09);
    VERIFICA(aprox(f.valoareTva(), 1775.0 * 0.09));
    VERIFICA(aprox(f.total(), 1775.0 * 1.09));

    // Factura cu si fara cost de internare.
    Factura f2(101, pac);
    f2.setCostInternare(1200.0);
    f2.adaugaServiciu(std::make_shared<Consultatie>("Consult control", 100.0));
    VERIFICA(aprox(f2.subtotal(), 1300.0));

    // Factura fara pacient -> exceptie.
    VERIFICA_ARUNCA(Factura(102, nullptr), EntitateInexistentaException);
}

// ---------------------------------------------------------------------------
// 5. FacturaFactory (sablonul Factory)
// ---------------------------------------------------------------------------
void test_factura_factory() {
    std::cout << "[Test] FacturaFactory\n";

    auto pac = std::make_shared<Pacient>(1, "Ion", "1900101221144", 30, "fractura");
    auto medic = std::make_shared<Medic>(10, "Popa", "1700101221144", 8000, "Ortopedie");
    auto serviciu = std::make_shared<Consultatie>("Consult", 250.0);

    Programare prog(1, pac, medic, DataOra(2025, 6, 12, 9, 0), serviciu);
    Factura din_prog = FacturaFactory::dinProgramare(1, prog);
    VERIFICA(aprox(din_prog.total(), 250.0));
    VERIFICA(din_prog.getServicii().size() == 1);

    Internare internare(1, pac, "A12", DataOra(2025, 6, 12), 300.0, 4);
    Factura din_int = FacturaFactory::dinInternare(2, internare);
    VERIFICA(aprox(din_int.total(), 1200.0)); // 300 * 4

    std::vector<std::shared_ptr<ServiciuMedical>> servicii = {
        std::make_shared<Consultatie>("C", 100.0),
        std::make_shared<Analiza>("A", 30.0, 2)};
    Factura din_serv = FacturaFactory::dinServicii(3, pac, servicii);
    VERIFICA(aprox(din_serv.total(), 160.0)); // 100 + 60
}

// ---------------------------------------------------------------------------
// 6. Internare
// ---------------------------------------------------------------------------
void test_internare() {
    std::cout << "[Test] Internare\n";

    auto pac = std::make_shared<Pacient>(1, "Ion", "1900101221144", 30, "pneumonie");
    Internare i(1, pac, "B7", DataOra(2025, 6, 10), 250.0, 5);
    VERIFICA(aprox(i.calculeazaCost(), 1250.0)); // 250 * 5

    VERIFICA_ARUNCA(Internare(2, pac, "B8", DataOra(2025, 6, 10), 250.0, 0),
                    DateInvalideException); // 0 zile invalid
    VERIFICA_ARUNCA(Internare(3, nullptr, "B9", DataOra(2025, 6, 10), 250.0, 3),
                    EntitateInexistentaException);
}

// ---------------------------------------------------------------------------
// 7. Filtrarea pacientilor dupa diagnostic
// ---------------------------------------------------------------------------
void test_filtrare() {
    std::cout << "[Test] Filtrare dupa diagnostic\n";

    Spital spital("Test");
    spital.adaugaPacient(std::make_shared<Pacient>(1, "A", "1900101221144", 30, "pneumonie"));
    spital.adaugaPacient(std::make_shared<Pacient>(2, "B", "2900101221144", 40, "fractura"));
    spital.adaugaPacient(std::make_shared<Pacient>(3, "C", "1800101221144", 50, "pneumonie"));

    auto rez = spital.filtreazaDupaDiagnostic("pneumonie");
    VERIFICA(rez.size() == 2);
    VERIFICA(spital.filtreazaDupaDiagnostic("inexistent").empty());
}

// ---------------------------------------------------------------------------
// 8. Exceptii la validarea CNP
// ---------------------------------------------------------------------------
void test_validare_cnp() {
    std::cout << "[Test] Validare CNP\n";
    VERIFICA_ARUNCA(Pacient(1, "X", "123", 30), DateInvalideException);        // prea scurt
    VERIFICA_ARUNCA(Pacient(1, "X", "190010122114a", 30), DateInvalideException); // litera
    bool ok = true;
    try { Pacient(1, "X", "1900101221144", 30); } catch (...) { ok = false; }
    VERIFICA(ok);
}

// ---------------------------------------------------------------------------
// 9. Modulul JSON: parsare, serializare, round-trip
// ---------------------------------------------------------------------------
void test_json() {
    std::cout << "[Test] JSON\n";

    const std::string text =
        "{ \"nume\": \"Spital\", \"nr\": 42, \"activ\": true, "
        "\"lista\": [1, 2.5, \"trei\"], \"gol\": null }";

    json::Value v = json::Value::parse(text);
    VERIFICA(v.get("nume").asSir() == "Spital");
    VERIFICA(v.get("nr").asInt() == 42);
    VERIFICA(v.get("activ").asBool() == true);
    VERIFICA(v.get("lista").elemente().size() == 3);
    VERIFICA(aprox(v.get("lista").elemente()[1].asNumar(), 2.5));
    VERIFICA(v.get("gol").esteNull());

    // Round-trip: dump apoi parse din nou -> aceleasi valori.
    std::string serializat = v.dump(2);
    json::Value v2 = json::Value::parse(serializat);
    VERIFICA(v2.get("nume").asSir() == "Spital");
    VERIFICA(v2.get("nr").asInt() == 42);

    // JSON invalid -> exceptie.
    VERIFICA_ARUNCA(json::Value::parse("{ stricat"), std::exception);
}

// ---------------------------------------------------------------------------
// 10. Persistenta: round-trip pentru Pacient si Angajat
// ---------------------------------------------------------------------------
void test_persistenta_entitati() {
    std::cout << "[Test] Persistenta entitati (toJson/fromJson)\n";

    Pacient p(7, "Ion Popescu", "1900101221144", 35, "pneumonie");
    Pacient p2 = Pacient::fromJson(p.toJson());
    VERIFICA(p2.getId() == 7);
    VERIFICA(p2.getNume() == "Ion Popescu");
    VERIFICA(p2.getVarsta() == 35);
    VERIFICA(p2.getDiagnostic() == "pneumonie");

    Medic m(10, "Popa", "1700101221144", 8000, "Cardiologie", 0.30);
    auto reconstruit = Angajat::fromJson(m.toJson());
    VERIFICA(reconstruit->tip() == "Medic");
    VERIFICA(aprox(reconstruit->calculeazaSalariu(), 8000.0 * 1.30));

    Asistent a(20, "Stan", "2880707221100", 4000, "Cardiologie", 600);
    auto reconstruitA = Angajat::fromJson(a.toJson());
    VERIFICA(reconstruitA->tip() == "Asistent");
    VERIFICA(aprox(reconstruitA->calculeazaSalariu(), 4600.0));
}

int main() {
    std::cout << "==== Teste unitare - Gestiune Spital ====\n";

    test_dataora();
    test_servicii_polimorfism();
    test_programari();
    test_factura();
    test_factura_factory();
    test_internare();
    test_filtrare();
    test_validare_cnp();
    test_json();
    test_persistenta_entitati();

    std::cout << "=========================================\n";
    std::cout << "Verificari rulate: " << g_rulate
              << " | esuate: " << g_esuate << "\n";
    if (g_esuate == 0) {
        std::cout << "TOATE TESTELE AU TRECUT.\n";
        return 0;
    }
    std::cout << "EXISTA TESTE ESUATE!\n";
    return 1;
}
