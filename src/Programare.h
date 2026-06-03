#ifndef SPITAL_PROGRAMARE_H
#define SPITAL_PROGRAMARE_H

#include "Pacient.h"
#include "Medic.h"
#include "ServiciuMedical.h"
#include "DataOra.h"

#include <memory>
#include <string>

// ---------------------------------------------------------------------------
// Programare: leaga un pacient, un medic, un moment (DataOra) si un serviciu.
//
// Constructorul valideaza relatiile (pacient/medic/serviciu existente) si
// arunca ProgramareInvalidaException pentru date incomplete. Detectarea
// conflictelor de orar (acelasi medic la aceeasi ora) se face cu inConflictCu()
// si este testata unitar.
// ---------------------------------------------------------------------------

class Programare {
    int                              id_;
    std::shared_ptr<Pacient>         pacient_;
    std::shared_ptr<Medic>           medic_;
    DataOra                          dataOra_;
    std::shared_ptr<ServiciuMedical> serviciu_;

public:
    Programare(int id,
               std::shared_ptr<Pacient> pacient,
               std::shared_ptr<Medic> medic,
               const DataOra& dataOra,
               std::shared_ptr<ServiciuMedical> serviciu);

    int getId() const                                       { return id_; }
    const std::shared_ptr<Pacient>& getPacient() const      { return pacient_; }
    const std::shared_ptr<Medic>& getMedic() const          { return medic_; }
    const DataOra& getDataOra() const                       { return dataOra_; }
    const std::shared_ptr<ServiciuMedical>& getServiciu() const { return serviciu_; }

    // Doua programari sunt in conflict daca au acelasi medic la aceeasi data/ora.
    bool inConflictCu(const Programare& alta) const;

    std::string descriere() const;
};

#endif // SPITAL_PROGRAMARE_H
