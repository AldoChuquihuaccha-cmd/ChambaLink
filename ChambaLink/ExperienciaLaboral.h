#pragma once
#include <string>

class ExperienciaLaboral {
private:
    std::string empresa;
    std::string cargo;
    int anioInicio;
    int anioFin;          // 0 significa "trabajo actual"
    std::string descripcion;

public:
    ExperienciaLaboral();
    ExperienciaLaboral(std::string pEmpresa, std::string pCargo,
        int pAnioInicio, int pAnioFin, std::string pDescripcion);

    std::string getEmpresa() const;
    std::string getCargo() const;
    int getAnioInicio() const;
    int getAnioFin() const;
    std::string getDescripcion() const;

    bool esActual() const;
    int duracionEnAnios(int anioActual) const;
};