#include "ExperienciaLaboral.h"

ExperienciaLaboral::ExperienciaLaboral()
    : empresa(""), cargo(""), anioInicio(0), anioFin(0), descripcion("") {
}

ExperienciaLaboral::ExperienciaLaboral(std::string pEmpresa, std::string pCargo,
    int pAnioInicio, int pAnioFin, std::string pDescripcion)
    : empresa(pEmpresa), cargo(pCargo), anioInicio(pAnioInicio),
    anioFin(pAnioFin), descripcion(pDescripcion) {
}

std::string ExperienciaLaboral::getEmpresa() const { return empresa; }
std::string ExperienciaLaboral::getCargo() const { return cargo; }
int ExperienciaLaboral::getAnioInicio() const { return anioInicio; }
int ExperienciaLaboral::getAnioFin() const { return anioFin; }
std::string ExperienciaLaboral::getDescripcion() const { return descripcion; }

bool ExperienciaLaboral::esActual() const { return anioFin == 0; }

int ExperienciaLaboral::duracionEnAnios(int anioActual) const {
    int fin = esActual() ? anioActual : anioFin;
    return fin - anioInicio;
}