#ifndef SPITAL_JSON_H
#define SPITAL_JSON_H

#include <string>
#include <vector>
#include <utility>

// ---------------------------------------------------------------------------
// Modul JSON minimal, scris de la zero (fara biblioteci externe).
//
// Suporta tipurile standard: null, bool, numar, sir, array si obiect.
// Obiectele isi pastreaza ordinea de inserare a cheilor (folosim un vector de
// perechi) pentru ca fisierele generate sa fie lizibile si stabile.
//
// Folosit pentru cerinta facultativa de persistenta a datelor spitalului.
// ---------------------------------------------------------------------------

namespace json {

enum class Tip { Null, Bool, Numar, Sir, Array, Obiect };

class Value {
public:
    Value();                       // null
    Value(bool b);                 // bool
    Value(int n);                  // numar (din intreg)
    Value(double n);               // numar
    Value(const char* s);          // sir
    Value(const std::string& s);   // sir

    static Value array();          // creeaza un array gol
    static Value obiect();         // creeaza un obiect gol

    Tip tip() const { return tip_; }
    bool esteNull() const { return tip_ == Tip::Null; }

    // Accesori (arunca std::runtime_error daca tipul nu corespunde).
    bool               asBool() const;
    double             asNumar() const;
    int                asInt() const;
    const std::string& asSir() const;

    // --- Operatii pentru array ---
    void push(const Value& v);
    const std::vector<Value>& elemente() const;

    // --- Operatii pentru obiect ---
    // operator[] creeaza cheia daca nu exista (transforma valoarea in obiect).
    Value& operator[](const std::string& cheie);
    bool        are(const std::string& cheie) const;
    const Value& get(const std::string& cheie) const; // arunca daca lipseste

    // Serializare cu indentare (indent = numar de spatii pe nivel).
    std::string dump(int indent = 2) const;

    // Parsare dintr-un text JSON. Arunca std::runtime_error la sintaxa gresita.
    static Value parse(const std::string& text);

private:
    Tip         tip_;
    bool        b_ = false;
    double      n_ = 0.0;
    std::string s_;
    std::vector<Value> arr_;
    std::vector<std::pair<std::string, Value>> obj_;

    void dumpInto(std::string& out, int indent, int adancime) const;
};

} // namespace json

#endif // SPITAL_JSON_H
