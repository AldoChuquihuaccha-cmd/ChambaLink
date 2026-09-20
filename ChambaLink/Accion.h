#pragma once
#include <string>
#include <functional>

// Patron Command: cada Accion sabe ejecutarse y deshacerse a si misma.
// Las lambdas 'hacer' y 'deshacer' las define quien construye la Accion
// (el Servicio correspondiente); esta clase no conoce el detalle del
// cambio que representa, solo sabe invocarlo. Eso es lo que permite
// apilarlas en una Pila<Accion> genuinamente generica.
class Accion {
private:
    std::string descripcion;
    std::function<void()> hacer;
    std::function<void()> deshacer;
    bool ejecutada;

public:
    Accion();
    Accion(std::string pDescripcion, std::function<void()> pHacer, std::function<void()> pDeshacer);

    void ejecutar();
    void deshacerAccion();

    std::string getDescripcion() const;
    bool fueEjecutada() const;
};

Accion::Accion() : descripcion(""), hacer(nullptr), deshacer(nullptr), ejecutada(false) {}

Accion::Accion(std::string pDescripcion, std::function<void()> pHacer, std::function<void()> pDeshacer)
    : descripcion(pDescripcion), hacer(pHacer), deshacer(pDeshacer), ejecutada(false) {
}

void Accion::ejecutar() {
    if (hacer) hacer();
    ejecutada = true;
}

void Accion::deshacerAccion() {
    if (deshacer) deshacer();
    ejecutada = false;
}

std::string Accion::getDescripcion() const { return descripcion; }
bool Accion::fueEjecutada() const { return ejecutada; }