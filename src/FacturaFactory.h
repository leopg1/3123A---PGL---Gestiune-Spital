#ifndef SPITAL_FACTURAFACTORY_H
#define SPITAL_FACTURAFACTORY_H

#include "Factura.h"
#include "Programare.h"
#include "Internare.h"

#include <memory>
#include <vector>

// ---------------------------------------------------------------------------
// FacturaFactory: implementeaza sablonul de proiectare FACTORY (cerinta
// facultativa).
//
// Centralizeaza logica de creare a facturilor pentru situatii diferite, astfel
// incat restul codului sa nu trebuiasca sa stie cum se "asambleaza" o factura:
//   - dintr-o programare (un singur serviciu);
//   - dintr-o internare (cost de spitalizare);
//   - dintr-o lista de servicii (factura compusa).
// ---------------------------------------------------------------------------

class FacturaFactory {
public:
    // Factura pentru serviciul dintr-o programare.
    static Factura dinProgramare(int id, const Programare& programare,
                                 const DataOra& dataEmitere = DataOra(),
                                 double tva = 0.0);

    // Factura pentru costul unei internari.
    static Factura dinInternare(int id, const Internare& internare,
                                const DataOra& dataEmitere = DataOra(),
                                double tva = 0.0);

    // Factura compusa din mai multe servicii pentru un pacient.
    static Factura dinServicii(int id, std::shared_ptr<Pacient> pacient,
                               std::vector<std::shared_ptr<ServiciuMedical>> servicii,
                               const DataOra& dataEmitere = DataOra(),
                               double tva = 0.0);
};

#endif // SPITAL_FACTURAFACTORY_H
