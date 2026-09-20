#include "Habilidad.h"

Habilidad::Habilidad() : nombre(""), nivel(NivelHabilidad::Basico) {}

Habilidad::Habilidad(std::string pNombre, NivelHabilidad pNivel)
    : nombre(pNombre), nivel(pNivel) {
}

std::string Habilidad::getNombre() const { return nombre; }

NivelHabilidad Habilidad::getNivel() const { return nivel; }

void Habilidad::setNivel(NivelHabilidad pNivel) { nivel = pNivel; }

std::string Habilidad::nivelToString() const {
    switch (nivel) {
    case NivelHabilidad::Basico:     return "Basico";
    case NivelHabilidad::Intermedio: return "Intermedio";
    case NivelHabilidad::Avanzado:   return "Avanzado";
    case NivelHabilidad::Experto:    return "Experto";
    }
    return "Desconocido";
}

bool Habilidad::mismoNombre(const Habilidad& otra) const {
    return nombre == otra.nombre;
}