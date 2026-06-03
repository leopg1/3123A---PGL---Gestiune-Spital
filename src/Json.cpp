#include "Json.h"

#include <cctype>
#include <cmath>
#include <cstdlib>
#include <sstream>
#include <stdexcept>

namespace json {

// ===========================================================================
// Constructori
// ===========================================================================
Value::Value() : tip_(Tip::Null) {}
Value::Value(bool b) : tip_(Tip::Bool), b_(b) {}
Value::Value(int n) : tip_(Tip::Numar), n_(static_cast<double>(n)) {}
Value::Value(double n) : tip_(Tip::Numar), n_(n) {}
Value::Value(const char* s) : tip_(Tip::Sir), s_(s) {}
Value::Value(const std::string& s) : tip_(Tip::Sir), s_(s) {}

Value Value::array() {
    Value v;
    v.tip_ = Tip::Array;
    return v;
}

Value Value::obiect() {
    Value v;
    v.tip_ = Tip::Obiect;
    return v;
}

// ===========================================================================
// Accesori
// ===========================================================================
bool Value::asBool() const {
    if (tip_ != Tip::Bool) throw std::runtime_error("JSON: valoarea nu este bool");
    return b_;
}

double Value::asNumar() const {
    if (tip_ != Tip::Numar) throw std::runtime_error("JSON: valoarea nu este numar");
    return n_;
}

int Value::asInt() const {
    return static_cast<int>(asNumar());
}

const std::string& Value::asSir() const {
    if (tip_ != Tip::Sir) throw std::runtime_error("JSON: valoarea nu este sir");
    return s_;
}

// ===========================================================================
// Array
// ===========================================================================
void Value::push(const Value& v) {
    if (tip_ != Tip::Array) throw std::runtime_error("JSON: push pe o valoare care nu e array");
    arr_.push_back(v);
}

const std::vector<Value>& Value::elemente() const {
    if (tip_ != Tip::Array) throw std::runtime_error("JSON: elemente() pe o valoare care nu e array");
    return arr_;
}

// ===========================================================================
// Obiect
// ===========================================================================
Value& Value::operator[](const std::string& cheie) {
    if (tip_ == Tip::Null) tip_ = Tip::Obiect; // promovam null -> obiect la prima scriere
    if (tip_ != Tip::Obiect) throw std::runtime_error("JSON: operator[] pe o valoare care nu e obiect");
    for (auto& pereche : obj_) {
        if (pereche.first == cheie) return pereche.second;
    }
    obj_.emplace_back(cheie, Value());
    return obj_.back().second;
}

bool Value::are(const std::string& cheie) const {
    if (tip_ != Tip::Obiect) return false;
    for (const auto& pereche : obj_) {
        if (pereche.first == cheie) return true;
    }
    return false;
}

const Value& Value::get(const std::string& cheie) const {
    if (tip_ != Tip::Obiect) throw std::runtime_error("JSON: get() pe o valoare care nu e obiect");
    for (const auto& pereche : obj_) {
        if (pereche.first == cheie) return pereche.second;
    }
    throw std::runtime_error("JSON: cheia '" + cheie + "' nu exista");
}

// ===========================================================================
// Serializare (dump)
// ===========================================================================
static void scrieSirEscapat(std::string& out, const std::string& s) {
    out += '"';
    for (char c : s) {
        switch (c) {
            case '"':  out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\n': out += "\\n";  break;
            case '\t': out += "\\t";  break;
            case '\r': out += "\\r";  break;
            case '\b': out += "\\b";  break;
            case '\f': out += "\\f";  break;
            default:   out += c;      break;
        }
    }
    out += '"';
}

static std::string numarToString(double n) {
    // Afisam intregii fara zecimale pentru fisiere curate.
    if (std::isfinite(n) && n == std::floor(n) && std::fabs(n) < 1e15) {
        return std::to_string(static_cast<long long>(n));
    }
    std::ostringstream os;
    os.precision(10);
    os << n;
    return os.str();
}

void Value::dumpInto(std::string& out, int indent, int adancime) const {
    const bool frumos = indent > 0;
    const std::string pad(static_cast<size_t>(indent) * (adancime + 1), ' ');
    const std::string padInchidere(static_cast<size_t>(indent) * adancime, ' ');

    switch (tip_) {
        case Tip::Null:  out += "null"; break;
        case Tip::Bool:  out += (b_ ? "true" : "false"); break;
        case Tip::Numar: out += numarToString(n_); break;
        case Tip::Sir:   scrieSirEscapat(out, s_); break;

        case Tip::Array:
            if (arr_.empty()) { out += "[]"; break; }
            out += '[';
            for (size_t i = 0; i < arr_.size(); ++i) {
                if (frumos) { out += '\n'; out += pad; }
                arr_[i].dumpInto(out, indent, adancime + 1);
                if (i + 1 < arr_.size()) out += ',';
            }
            if (frumos) { out += '\n'; out += padInchidere; }
            out += ']';
            break;

        case Tip::Obiect:
            if (obj_.empty()) { out += "{}"; break; }
            out += '{';
            for (size_t i = 0; i < obj_.size(); ++i) {
                if (frumos) { out += '\n'; out += pad; }
                scrieSirEscapat(out, obj_[i].first);
                out += (frumos ? ": " : ":");
                obj_[i].second.dumpInto(out, indent, adancime + 1);
                if (i + 1 < obj_.size()) out += ',';
            }
            if (frumos) { out += '\n'; out += padInchidere; }
            out += '}';
            break;
    }
}

std::string Value::dump(int indent) const {
    std::string out;
    dumpInto(out, indent, 0);
    return out;
}

// ===========================================================================
// Parsare (descendenta recursiva)
// ===========================================================================
namespace {

struct Parser {
    const std::string& s;
    size_t i = 0;
    explicit Parser(const std::string& text) : s(text) {}

    [[noreturn]] void eroare(const std::string& m) const {
        throw std::runtime_error("JSON eroare la pozitia " + std::to_string(i) + ": " + m);
    }

    void sariSpatii() {
        while (i < s.size() &&
               (s[i] == ' ' || s[i] == '\t' || s[i] == '\n' || s[i] == '\r')) {
            ++i;
        }
    }

    char peek() const { return i < s.size() ? s[i] : '\0'; }

    Value parseValoare() {
        sariSpatii();
        char c = peek();
        switch (c) {
            case '{': return parseObiect();
            case '[': return parseArray();
            case '"': return Value(parseSir());
            case 't': case 'f': return parseBool();
            case 'n': return parseNull();
            default:
                if (c == '-' || (c >= '0' && c <= '9')) return parseNumar();
                eroare("token neasteptat");
        }
    }

    std::string parseSir() {
        if (s[i] != '"') eroare("se astepta '\"'");
        ++i;
        std::string out;
        while (true) {
            if (i >= s.size()) eroare("sir neterminat");
            char c = s[i++];
            if (c == '"') break;
            if (c == '\\') {
                if (i >= s.size()) eroare("escape neterminat");
                char e = s[i++];
                switch (e) {
                    case '"':  out += '"';  break;
                    case '\\': out += '\\'; break;
                    case '/':  out += '/';  break;
                    case 'n':  out += '\n'; break;
                    case 't':  out += '\t'; break;
                    case 'r':  out += '\r'; break;
                    case 'b':  out += '\b'; break;
                    case 'f':  out += '\f'; break;
                    case 'u': {
                        if (i + 4 > s.size()) eroare("secventa \\u incompleta");
                        int cp = std::stoi(s.substr(i, 4), nullptr, 16);
                        i += 4;
                        // Codare UTF-8 minimala pentru caractere din planul de baza.
                        if (cp < 0x80) {
                            out += static_cast<char>(cp);
                        } else if (cp < 0x800) {
                            out += static_cast<char>(0xC0 | (cp >> 6));
                            out += static_cast<char>(0x80 | (cp & 0x3F));
                        } else {
                            out += static_cast<char>(0xE0 | (cp >> 12));
                            out += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                            out += static_cast<char>(0x80 | (cp & 0x3F));
                        }
                        break;
                    }
                    default: eroare("escape invalid");
                }
            } else {
                out += c;
            }
        }
        return out;
    }

    Value parseNumar() {
        size_t start = i;
        if (peek() == '-') ++i;
        while (i < s.size()) {
            char c = s[i];
            if ((c >= '0' && c <= '9') || c == '.' || c == 'e' || c == 'E' ||
                c == '+' || c == '-') {
                ++i;
            } else {
                break;
            }
        }
        try {
            return Value(std::stod(s.substr(start, i - start)));
        } catch (...) {
            eroare("numar invalid");
        }
    }

    Value parseBool() {
        if (s.compare(i, 4, "true") == 0)  { i += 4; return Value(true); }
        if (s.compare(i, 5, "false") == 0) { i += 5; return Value(false); }
        eroare("se astepta true/false");
    }

    Value parseNull() {
        if (s.compare(i, 4, "null") == 0) { i += 4; return Value(); }
        eroare("se astepta null");
    }

    Value parseArray() {
        Value v = Value::array();
        ++i; // sare peste '['
        sariSpatii();
        if (peek() == ']') { ++i; return v; }
        while (true) {
            v.push(parseValoare());
            sariSpatii();
            char c = peek();
            if (c == ',') { ++i; continue; }
            if (c == ']') { ++i; break; }
            eroare("se astepta ',' sau ']'");
        }
        return v;
    }

    Value parseObiect() {
        Value v = Value::obiect();
        ++i; // sare peste '{'
        sariSpatii();
        if (peek() == '}') { ++i; return v; }
        while (true) {
            sariSpatii();
            if (peek() != '"') eroare("se astepta cheie sir");
            std::string cheie = parseSir();
            sariSpatii();
            if (peek() != ':') eroare("se astepta ':'");
            ++i;
            v[cheie] = parseValoare();
            sariSpatii();
            char c = peek();
            if (c == ',') { ++i; continue; }
            if (c == '}') { ++i; break; }
            eroare("se astepta ',' sau '}'");
        }
        return v;
    }
};

} // namespace

Value Value::parse(const std::string& text) {
    Parser p(text);
    Value v = p.parseValoare();
    p.sariSpatii();
    if (p.i != text.size()) {
        throw std::runtime_error("JSON: continut suplimentar dupa valoarea radacina");
    }
    return v;
}

} // namespace json
