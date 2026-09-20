#pragma once
#include <string>

class Vacante {
private:
    int id;
    int idEmpresa;
    std::string titulo;
    std::string descripcion;
    std::string requisitos;
    std::string modalidad; // Remoto, Presencial, Hibrido
    bool activa;

public:
    Vacante();

    Vacante(int pId, int pIdEmpresa, std::string pTitulo,
            std::string pDescripcion, std::string pRequisitos,
            std::string pModalidad);

    int getId() const;
    int getIdEmpresa() const;
    std::string getTitulo() const;
    std::string getDescripcion() const;
    std::string getRequisitos() const;
    std::string getModalidad() const;
    bool estaActiva() const;

    void cerrarVacante();

    bool mismoId(const Vacante& otra) const;
};
