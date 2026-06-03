# Documentatie tehnica — Aplicatie de Gestiune a unui Spital

Document care descrie clasele, conceptele POO folosite si deciziile de
proiectare. Pentru diagrame, vezi [uml.md](uml.md).

---

## 1. Privire de ansamblu

Aplicatia modeleaza activitatea administrativa a unui spital. Entitatile
principale sunt **pacientii**, **personalul** (medici si asistenti),
**programarile**, **internarile** si **facturile**. Clasa `Spital` joaca rolul
de manager (fatada) care detine toate entitatile si aplica regulile de business.

Codul este organizat pe principiul *o responsabilitate pe clasa*, cu separare
clara intre:

- **model** (entitatile de domeniu),
- **servicii transversale** (`Logger`, modulul `json`, `Exceptii`),
- **orchestrare** (`Spital`),
- **prezentare** (`main.cpp`).

---

## 2. Ierarhiile de clase

### 2.1. Ierarhia persoanelor

```
Persoana (abstracta)
├── Pacient
└── Angajat (abstracta)
    ├── Medic
    └── Asistent
```

- **`Persoana`** — clasa de baza abstracta. Contine `nume` si `cnp` (incapsulate,
  `protected`), valideaza CNP-ul (13 cifre) in constructor si declara metoda
  virtuala pura `tip()`. Are si o metoda virtuala cu implementare implicita,
  `descriere()`, pe care subclasele o pot suprascrie.

- **`Pacient`** — adauga `id`, `varsta`, `diagnostic`. Diagnosticul este folosit
  pentru cerinta de filtrare. Implementeaza `toJson()`/`fromJson()`.

- **`Angajat`** — clasa de baza abstracta pentru personal. Adauga `id` si
  `salariuBaza` si declara metoda virtuala pura `calculeazaSalariu()`. Metoda
  statica `fromJson()` reconstruieste tipul concret corect (Medic sau Asistent)
  pe baza unui camp discriminator `tip`.

- **`Medic`** — salariul = `salariuBaza * (1 + sporSpecializare)`.

- **`Asistent`** — salariul = `salariuBaza + sporTura` (bonus fix).

### 2.2. Ierarhia serviciilor medicale (polimorfism)

```
ServiciuMedical (abstracta)
├── Consultatie    -> cost = tarif (x1.5 daca este urgenta)
├── Analiza        -> cost = tarif_per_proba * nr_probe
└── Interventie    -> cost = (tarif_baza + durata_min * 5) * complexitate
```

Aici se concentreaza cerinta de **polimorfism**: `calculeazaCost()` este
virtuala pura, iar fiecare serviciu o implementeaza dupa propria formula. O
`Factura` aduna servicii eterogene prin `std::shared_ptr<ServiciuMedical>` si le
insumeaza costurile fara sa cunoasca tipul concret.

---

## 3. Clasele de legatura

- **`Programare`** — asociaza un `Pacient`, un `Medic`, o `DataOra` si un
  `ServiciuMedical`. Constructorul valideaza ca toate componentele exista
  (altfel arunca `ProgramareInvalidaException`). Metoda `inConflictCu()` detecteaza
  cand doua programari folosesc acelasi medic la aceeasi ora.

- **`Internare`** — spitalizarea unui pacient intr-un salon, pe un numar de zile.
  Costul = `costPeZi * nrZile`. Validare: numar de zile `>= 1`, cost `>= 0`.

- **`Factura`** — agrega o lista de servicii (+ optional costul unei internari) si
  calculeaza `subtotal()`, `valoareTva()` si `total()`. Este intentionat o clasa
  "pura" (fara dependenta de `Logger`), ca sa fie usor de testat. Logarea
  emiterii o face `Spital`.

- **`DataOra`** — data + ora cu validare calendaristica completa (luni, zile in
  functie de anii bisecti, ore, minute). Ofera operatori de comparatie si
  conversie din/in string `AAAA-LL-ZZ HH:MM`.

---

## 4. Servicii transversale

### 4.1. Exceptii personalizate (`Exceptii.h`)

```
std::runtime_error
└── SpitalException
    ├── DateInvalideException
    ├── ProgramareInvalidaException
    ├── EntitateInexistentaException
    └── PersistentaException
```

Radacina comuna permite un singur `catch(const SpitalException&)` in bucla de
meniu, pastrand totodata tipuri specifice pentru tratare granulara.

### 4.2. Logger (Singleton)

`Logger` este un Singleton (`Logger::instanta()`) care scrie mesaje cu marca de
timp si nivel (`INFO`, `ATENTIE`, `CRITIC`) intr-un fisier de log. Operatiunile
**critice** (internare, emitere factura) sunt afisate suplimentar in consola.

### 4.3. Modulul JSON (`Json.{h,cpp}`)

Implementare proprie a unui document JSON (`json::Value`), cu:

- tipuri: `null`, `bool`, `numar`, `sir`, `array`, `obiect`;
- **parser** recursiv-descendent (cu tratarea secventelor de escape);
- **serializer** cu indentare;
- pastrarea ordinii cheilor pentru fisiere lizibile.

Este folosit de `Spital::salveaza()`/`incarca()` pentru persistenta.

---

## 5. Managerul `Spital`

`Spital` detine vectorii de pacienti, angajati, programari, internari si facturi
si expune operatiunile:

- `adaugaPacient`, `adaugaAngajat` — cu validare si logare;
- `gasestePacient`, `gasesteMedic` — cautare (arunca `EntitateInexistentaException`);
- `getMedici`, `getAsistenti` — filtrare polimorfica prin `dynamic_pointer_cast`;
- `adaugaProgramare` — verifica conflictele de orar inainte de adaugare;
- `interneazaPacient`, `emiteFactura` — **operatiuni critice**, logate;
- `filtreazaDupaDiagnostic` — cerinta facultativa, implementata cu `std::copy_if`;
- `salveaza`/`incarca` — persistenta JSON.

---

## 6. Decizii de proiectare

- **`shared_ptr` pentru entitati partajate** — o `Programare` si o `Factura`
  refera acelasi `Pacient`/`Medic` ca obiectele detinute de `Spital`, fara
  copii si fara pericol de pointeri suspendati.
- **Factura fara dependente de logare** — pentru testabilitate; `Spital`
  ataseaza logarea la emitere.
- **Modul JSON propriu** — respecta cerinta "fara biblioteci externe complexe"
  si demonstreaza lucrul cu structuri recursive si parsare.
- **Test runner minimal** — un set de macrouri (`VERIFICA`, `VERIFICA_ARUNCA`)
  evita dependenta de un framework extern si returneaza cod de iesire potrivit
  pentru CI.

---

## 7. Concepte POO — pe scurt

| Concept | Exemplu concret |
|---|---|
| Incapsulare | campuri `private`/`protected` + validare in constructori |
| Mostenire | `Angajat -> Medic, Asistent`; `Persoana -> Pacient, Angajat` |
| Polimorfism | `ServiciuMedical::calculeazaCost`, `Angajat::calculeazaSalariu` |
| Abstractizare | clasele abstracte `Persoana`, `Angajat`, `ServiciuMedical` |
| Suprascriere | `descriere()`, `tip()`, `toJson()` |
| Exceptii | ierarhia `SpitalException` |
| STL | `vector`, `shared_ptr`, `copy_if`, `dynamic_pointer_cast` |
| Sabloane de proiectare | Factory (`FacturaFactory`), Singleton (`Logger`) |

---

## 8. Posibile imbunatatiri

- Persistenta completa a programarilor si internarilor (cu reconstructia
  legaturilor prin id-uri).
- Indexare a pacientilor dupa id intr-un `std::unordered_map` pentru cautari O(1).
- Rapoarte statistice (venituri pe sectie, grad de ocupare a saloanelor).
- Validare suplimentara a CNP-ului (cifra de control).
