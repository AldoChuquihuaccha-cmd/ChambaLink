#pragma once
#include <string>

// Nivel de dominio que el usuario declara para una habilidad.
// Al guardarlo en archivo conviene escribirlo como numero con (int)nivel
// y leerlo de vuelta con (NivelHabilidad)numero.
enum class NivelHabilidad { Basico, Intermedio, Avanzado, Experto };

// Forma parte del perfil del usuario: no existe fuera de el.
class Habilidad {
private:
    std::string nombre;
    NivelHabilidad nivel;

public:
    Habilidad() {
        nombre = "";
        nivel = NivelHabilidad::Basico;
    }

    Habilidad(std::string pNombre, NivelHabilidad pNivel) {
        nombre = pNombre;
        nivel = pNivel;
    }

    std::string getNombre() const { return nombre; }
    NivelHabilidad getNivel() const { return nivel; }
    void setNivel(NivelHabilidad pNivel) { nivel = pNivel; }

    std::string nivelToString() const {
        switch (nivel) {
        case NivelHabilidad::Basico:     return "Basico";
        case NivelHabilidad::Intermedio: return "Intermedio";
        case NivelHabilidad::Avanzado:   return "Avanzado";
        case NivelHabilidad::Experto:    return "Experto";
        }
        return "Desconocido";
    }

    // Se usan como condicion en las lambdas al buscar en la lista.
    bool mismoNombre(const Habilidad& otra) const { return nombre == otra.nombre; }
    bool tieneNombre(std::string pNombre) const { return nombre == pNombre; }
};
