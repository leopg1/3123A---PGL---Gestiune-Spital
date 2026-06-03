#ifndef SPITAL_FACTURA_H
#define SPITAL_FACTURA_H

#include "Pacient.h"
#include "ServiciuMedical.h"
#include "DataOra.h"

#include <memory>
#include <vector>
#include <string>

// ---------------------------------------------------------------------------
// Factura: documentul de plata emis unui pacient.
//
// Agrega o lista de servicii medicale eterogene (prin pointeri la clasa de
// baza ServiciuMedical) si insumeaza costurile POLIMORFIC, fara sa cunoasca
// tipul concret al fiecarui serviciu. Poate include si costul unei internari.
//
// Clasa este intentionat "pura" (fara dependenta de Logger), pentru a fi usor
// de testat unitar. Logarea emiterii (operatiune critica) o face managerul
// Spital.
// ---------------------------------------------------------------------------

class Factura {
    int                                           id_;
    std::shared_ptr<Pacient>                      pacient_;
    std::vector<std::shared_ptr<ServiciuMedical>> servicii_;
    DataOra                                       dataEmitere_;
    double                                        costInternare_;
    double                                        tvaProcent_; // ex: 0.09 = 9%
    bool                                          emisa_;

public:
    Factura(int id, std::shared_ptr<Pacient> pacient,
            const DataOra& dataEmitere = DataOra(), double tvaProcent = 0.0);

    void adaugaServiciu(std::shared_ptr<ServiciuMedical> serviciu);
    void setCostInternare(double cost);
    void setTva(double procent);

    int getId() const                          { return id_; }
    const std::shared_ptr<Pacient>& getPacient() const { return pacient_; }
    const std::vector<std::shared_ptr<ServiciuMedical>>& getServicii() const { return servicii_; }
    const DataOra& getDataEmitere() const      { return dataEmitere_; }
    bool esteEmisa() const                     { return emisa_; }

    // --- Calcule (insumarea polimorfica a costurilor) ---
    double subtotal() const;    // suma serviciilor + costul internarii
    double valoareTva() const;  // subtotal * tvaProcent
    double total() const;       // subtotal + TVA

    // Marcheaza factura ca emisa (logarea o face Spital).
    void emite();

    json::Value toJson() const;
    std::string descriere() const;
};

#endif // SPITAL_FACTURA_H
