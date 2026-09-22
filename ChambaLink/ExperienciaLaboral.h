#pragma once
#include <string>
#include <stdexcept>

// Un puesto dentro de la trayectoria laboral del usuario.
// idEmpresa enlaza con una empresa registrada en la red; si vale 0 significa
// que la empresa no esta registrada y solo se guarda su nombre.
// anioFin igual a 0 significa que es el trabajo actual.
class ExperienciaLaboral {
private:
    int idEmpresa;
    std::string nombreEmpresa;
    std::string cargo;
    int anioInicio;
    int anioFin;
    std::string descripcion;

public:
    ExperienciaLaboral() {
        idEmpresa = 0;
        nombreEmpresa = "";
        cargo = "";
        anioInicio = 0;
        anioFin = 0;
        descripcion = "";
    }

    ExperienciaLaboral(int pIdEmpresa, std::string pNombreEmpresa, std::string pCargo,
        int pAnioInicio, int pAnioFin, std::string pDescripcion) {
        // Se valida al construir para que nunca entre a la lista un dato invalido.
        if (pAnioFin != 0 && pAnioFin < pAnioInicio)
            throw std::invalid_argument("El anio de fin no puede ser anterior al de inicio");
        idEmpresa = pIdEmpresa;
        nombreEmpresa = pNombreEmpresa;
        cargo = pCargo;
        anioInicio = pAnioInicio;
        anioFin = pAnioFin;
        descripcion = pDescripcion;
    }

    int getIdEmpresa() const { return idEmpresa; }
    std::string getNombreEmpresa() const { return nombreEmpresa; }
    std::string getCargo() const { return cargo; }
    int getAnioInicio() const { return anioInicio; }
    int getAnioFin() const { return anioFin; }
    std::string getDescripcion() const { return descripcion; }

    void setDescripcion(std::string pDescripcion) { descripcion = pDescripcion; }

    // Cierra un trabajo que estaba marcado como actual.
    void finalizar(int pAnioFin) {
        if (pAnioFin < anioInicio)
            throw std::invalid_argument("El anio de fin no puede ser anterior al de inicio");
        anioFin = pAnioFin;
    }

    bool esActual() const { return anioFin == 0; }
    bool esDeEmpresa(int pIdEmpresa) const { return idEmpresa == pIdEmpresa; }

    int duracionEnAnios(int anioActual) const {
        int fin = anioFin;
        if (esActual()) fin = anioActual;
        return fin - anioInicio;
    }
};