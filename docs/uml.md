# Diagrame UML — Aplicatie de Gestiune a unui Spital

Diagramele de mai jos sunt in format text (ASCII), pentru a fi vizibile direct
in orice editor sau pe GitHub. Notatii: `+` public, `-` privat, `#` protejat,
`<<abstract>>` clasa abstracta, sageata pline `<|--` mostenire, `o-->` agregare.

---

## 1. Ierarhia persoanelor (mostenire)

```
                      +---------------------------+
                      |      <<abstract>>         |
                      |        Persoana           |
                      +---------------------------+
                      | # nume : string           |
                      | # cnp  : string           |
                      +---------------------------+
                      | + getNume() : string      |
                      | + getCnp()  : string      |
                      | + tip() : string {pur}    |
                      | + descriere() : string    |
                      +---------------------------+
                                  ^
                  +---------------+----------------+
                  |                                |
       +----------------------+        +-------------------------+
       |       Pacient        |        |     <<abstract>>        |
       +----------------------+        |        Angajat          |
       | - id : int           |        +-------------------------+
       | - varsta : int       |        | # id : int              |
       | - diagnostic : string|        | # salariuBaza : double  |
       +----------------------+        +-------------------------+
       | + tip() = "Pacient"  |        | + calculeazaSalariu()   |
       | + toJson()/fromJson()|        |        {pur}            |
       +----------------------+        +-------------------------+
                                                  ^
                                  +---------------+---------------+
                                  |                               |
                       +----------------------+      +-----------------------+
                       |        Medic         |      |       Asistent        |
                       +----------------------+      +-----------------------+
                       | - specializare       |      | - sectie              |
                       | - sporSpecializare   |      | - sporTura            |
                       +----------------------+      +-----------------------+
                       | + calculeazaSalariu()|      | + calculeazaSalariu() |
                       | = baza*(1+spor)      |      | = baza + sporTura     |
                       | + tip() = "Medic"    |      | + tip() = "Asistent"  |
                       +----------------------+      +-----------------------+
```

---

## 2. Ierarhia serviciilor medicale (polimorfism cost)

```
                  +-------------------------------+
                  |        <<abstract>>           |
                  |       ServiciuMedical         |
                  +-------------------------------+
                  | # nume : string               |
                  +-------------------------------+
                  | + calculeazaCost() : double {pur} |
                  | + tipServiciu() : string {pur}    |
                  | + descriere() : string            |
                  +-------------------------------+
                                  ^
          +----------------------+----------------------+
          |                      |                      |
  +----------------+   +------------------+   +------------------------+
  |  Consultatie   |   |     Analiza      |   |      Interventie       |
  +----------------+   +------------------+   +------------------------+
  | - tarif        |   | - tarifPerProba  |   | - tarifBaza            |
  | - urgenta      |   | - nrProbe        |   | - durataMinute         |
  +----------------+   +------------------+   | - factorComplexitate   |
  | cost = tarif   |   | cost = tarif *   |   +------------------------+
  | (*1.5 urgenta) |   |        nrProbe   |   | cost = (baza + min*5)  |
  +----------------+   +------------------+   |        * complexitate  |
                                              +------------------------+
```

---

## 3. Relatii intre entitati (agregare / asociere)

```
   +-----------+        +--------------+        +-----------------+
   |  Pacient  |<-------|  Programare  |------->|      Medic      |
   +-----------+        +--------------+        +-----------------+
                              |
                              v
                     +-----------------+
                     | ServiciuMedical |
                     +-----------------+

   +-----------+        +--------------+
   |  Pacient  |<-------|  Internare   |
   +-----------+        +--------------+

   +-----------+        +--------------+   o-->  *  +-----------------+
   |  Pacient  |<-------|   Factura    |----------->| ServiciuMedical |
   +-----------+        +--------------+            +-----------------+
                        | + subtotal() |
                        | + total()    |
                        +--------------+
```

---

## 4. Managerul si serviciile transversale

```
                         +-----------------------------+
                         |           Spital            |
                         +-----------------------------+
                         | - pacienti   : vector       |
                         | - angajati   : vector       |
                         | - programari : vector       |
                         | - internari  : vector       |
                         | - facturi    : vector       |
                         +-----------------------------+
                         | + adaugaPacient()           |
                         | + adaugaProgramare()        |
                         | + interneazaPacient()  [LOG]|
                         | + emiteFactura()       [LOG]|
                         | + filtreazaDupaDiagnostic() |
                         | + salveaza()/incarca() (JSON)|
                         +-----------------------------+
                            |              |          |
                            v              v          v
                    +-------------+ +-----------+ +------------------+
                    | FacturaFactory| |  Logger  | |   json::Value    |
                    |  <<Factory>>  | |<<Singleton>>| | <<persistenta>>|
                    +-------------+ +-----------+ +------------------+

                    Exceptii:  std::runtime_error <|-- SpitalException
                               SpitalException <|-- DateInvalideException
                               SpitalException <|-- ProgramareInvalidaException
                               SpitalException <|-- EntitateInexistentaException
                               SpitalException <|-- PersistentaException
```
