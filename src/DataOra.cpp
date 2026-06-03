#include "DataOra.h"
#include "Exceptii.h"

#include <cstdio>
#include <sstream>
#include <iomanip>

DataOra::DataOra() : an_(2025), luna_(1), zi_(1), ora_(0), minut_(0) {}

DataOra::DataOra(int an, int luna, int zi, int ora, int minut)
    : an_(an), luna_(luna), zi_(zi), ora_(ora), minut_(minut) {
    valideaza();
}

bool DataOra::esteBisect(int an) {
    return (an % 4 == 0 && an % 100 != 0) || (an % 400 == 0);
}

int DataOra::zileInLuna(int an, int luna) {
    static const int zile[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (luna < 1 || luna > 12) {
        return 0; // luna invalida; valideaza() trateaza eroarea
    }
    if (luna == 2 && esteBisect(an)) {
        return 29;
    }
    return zile[luna - 1];
}

void DataOra::valideaza() const {
    if (an_ < 1900 || an_ > 2200) {
        throw DateInvalideException("anul " + std::to_string(an_) + " este in afara intervalului permis");
    }
    if (luna_ < 1 || luna_ > 12) {
        throw DateInvalideException("luna " + std::to_string(luna_) + " nu exista");
    }
    if (zi_ < 1 || zi_ > zileInLuna(an_, luna_)) {
        throw DateInvalideException("ziua " + std::to_string(zi_) +
                                    " nu este valida pentru luna data");
    }
    if (ora_ < 0 || ora_ > 23) {
        throw DateInvalideException("ora " + std::to_string(ora_) + " nu este valida");
    }
    if (minut_ < 0 || minut_ > 59) {
        throw DateInvalideException("minutul " + std::to_string(minut_) + " nu este valid");
    }
}

long long DataOra::caMinute() const {
    // Aproximare suficienta pentru comparatii si durate intre programari:
    // numaram minutele de la inceputul anului 1900 folosind o luna de 31 zile
    // ca unitate stabila. Monoton crescator => comparabil corect cu ==/<.
    long long zileTotale = static_cast<long long>(an_) * 372 + (luna_ - 1) * 31 + (zi_ - 1);
    return ((zileTotale * 24) + ora_) * 60 + minut_;
}

bool DataOra::operator==(const DataOra& alt) const {
    return an_ == alt.an_ && luna_ == alt.luna_ && zi_ == alt.zi_ &&
           ora_ == alt.ora_ && minut_ == alt.minut_;
}

bool DataOra::operator<(const DataOra& alt) const {
    return caMinute() < alt.caMinute();
}

std::string DataOra::toString() const {
    std::ostringstream os;
    os << std::setfill('0')
       << std::setw(4) << an_ << '-'
       << std::setw(2) << luna_ << '-'
       << std::setw(2) << zi_ << ' '
       << std::setw(2) << ora_ << ':'
       << std::setw(2) << minut_;
    return os.str();
}

DataOra DataOra::dinString(const std::string& text) {
    int an = 0, luna = 0, zi = 0, ora = 0, minut = 0;
    // Acceptam "AAAA-LL-ZZ HH:MM" sau doar "AAAA-LL-ZZ".
    int citite = std::sscanf(text.c_str(), "%d-%d-%d %d:%d",
                             &an, &luna, &zi, &ora, &minut);
    if (citite < 3) {
        throw DateInvalideException("formatul datei '" + text +
                                    "' nu este 'AAAA-LL-ZZ HH:MM'");
    }
    return DataOra(an, luna, zi, ora, minut);
}
