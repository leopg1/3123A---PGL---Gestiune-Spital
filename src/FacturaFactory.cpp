#include "FacturaFactory.h"

#include <utility>

Factura FacturaFactory::dinProgramare(int id, const Programare& programare,
                                      const DataOra& dataEmitere, double tva) {
    Factura factura(id, programare.getPacient(), dataEmitere, tva);
    factura.adaugaServiciu(programare.getServiciu());
    return factura;
}

Factura FacturaFactory::dinInternare(int id, const Internare& internare,
                                     const DataOra& dataEmitere, double tva) {
    Factura factura(id, internare.getPacient(), dataEmitere, tva);
    factura.setCostInternare(internare.calculeazaCost());
    return factura;
}

Factura FacturaFactory::dinServicii(int id, std::shared_ptr<Pacient> pacient,
                                    std::vector<std::shared_ptr<ServiciuMedical>> servicii,
                                    const DataOra& dataEmitere, double tva) {
    Factura factura(id, std::move(pacient), dataEmitere, tva);
    for (auto& serviciu : servicii) {
        factura.adaugaServiciu(std::move(serviciu));
    }
    return factura;
}
