#ifndef SPITAL_SPITAL_H
#define SPITAL_SPITAL_H

#include "Pacient.h"
#include "Angajat.h"
#include "Medic.h"
#include "Asistent.h"
#include "Programare.h"
#include "Internare.h"
#include "Factura.h"

#include <memory>
#include <vector>
#include <string>

// ---------------------------------------------------------------------------
// Spital: clasa "manager" (fatada) care agrega toate entitatile si orchestreaza
// operatiunile aplicatiei.
//
// Responsabilitati:
//   - stocheaza pacientii si angajatii (polimorfic, prin shared_ptr<Angajat>);
//   - valideaza programarile si detecteaza conflictele de orar;
//   - inregistreaza in Logger operatiunile critice (internare, emitere factura);
//   - filtreaza pacientii dupa diagnostic (cerinta facultativa);
//   - salveaza / incarca starea in format JSON (cerinta facultativa).
// ---------------------------------------------------------------------------

class Spital {
    std::string nume_;

    std::vector<std::shared_ptr<Pacient>> pacienti_;
    std::vector<std::shared_ptr<Angajat>> angajati_;
    std::vector<Programare>               programari_;
    std::vector<Internare>                internari_;
    std::vector<Factura>                  facturi_;

public:
    explicit Spital(std::string nume);

    const std::string& getNume() const { return nume_; }

    // --- Pacienti ---
    std::shared_ptr<Pacient> adaugaPacient(std::shared_ptr<Pacient> pacient);
    std::shared_ptr<Pacient> gasestePacient(int id) const; // arunca daca nu exista
    const std::vector<std::shared_ptr<Pacient>>& getPacienti() const { return pacienti_; }

    // --- Angajati (Medic / Asistent) ---
    std::shared_ptr<Angajat> adaugaAngajat(std::shared_ptr<Angajat> angajat);
    std::shared_ptr<Medic>   gasesteMedic(int id) const; // arunca daca nu exista
    std::vector<std::shared_ptr<Medic>>    getMedici() const;
    std::vector<std::shared_ptr<Asistent>> getAsistenti() const;
    const std::vector<std::shared_ptr<Angajat>>& getAngajati() const { return angajati_; }

    // --- Programari (cu detectarea conflictelor de orar) ---
    const Programare& adaugaProgramare(const Programare& programare);
    const std::vector<Programare>& getProgramari() const { return programari_; }

    // --- Operatiuni critice (logate) ---
    const Internare& interneazaPacient(const Internare& internare);
    const Factura&   emiteFactura(Factura factura);
    const std::vector<Internare>& getInternari() const { return internari_; }
    const std::vector<Factura>&   getFacturi() const   { return facturi_; }

    // --- Filtrare (cerinta facultativa) ---
    std::vector<std::shared_ptr<Pacient>>
    filtreazaDupaDiagnostic(const std::string& diagnostic) const;

    // --- Persistenta JSON (cerinta facultativa) ---
    json::Value toJson() const;
    void salveaza(const std::string& cale) const;
    void incarca(const std::string& cale); // reconstruieste pacientii si angajatii
};

#endif // SPITAL_SPITAL_H
