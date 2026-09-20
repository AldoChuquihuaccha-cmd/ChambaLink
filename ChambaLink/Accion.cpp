#include "Accion.h"

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