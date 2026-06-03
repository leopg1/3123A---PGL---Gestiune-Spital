#ifndef SPITAL_PACIENT_H
#define SPITAL_PACIENT_H

#include "Persoana.h"
#include "Json.h"
#include <string>

// ---------------------------------------------------------------------------
// Pacient: persoana care primeste servicii medicale.
//
// Are un identificator unic, varsta si un diagnostic (folosit la cerinta
// facultativa de filtrare a pacientilor dupa diagnostic).
// ---------------------------------------------------------------------------

class Pacient : public Persoana {
    int         id_;
    int         varsta_;
    std::string diagnostic_;

public:
    Pacient(int id, std::string nume, std::string cnp, int varsta,
            std::string diagnostic = "");

    int getId() const                    { return id_; }
    int getVarsta() const                { return varsta_; }
    const std::string& getDiagnostic() const { return diagnostic_; }
    void setDiagnostic(const std::string& diagnostic) { diagnostic_ = diagnostic; }

    std::string tip() const override { return "Pacient"; }
    std::string descriere() const override;

    // --- Persistenta JSON ---
    json::Value toJson() const;
    static Pacient fromJson(const json::Value& v);
};

#endif // SPITAL_PACIENT_H
