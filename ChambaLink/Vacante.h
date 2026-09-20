#pragma once
#include <string>

class Vacante {
private:
    int id;
    int idEmpresa;
    std::string titulo;
    std::string descripcion;
    std::string requisitos;
    std::string modalidad;
    bool activa;

public:

    Vacante()
        : id(0), idEmpresa(0), titulo(""), descripcion(""),
          requisitos(""), modalidad(""), activa(true) {}

    Vacante(int pId, int pIdEmpresa, std::string pTitulo,
            std::string pDescripcion, std::string pRequisitos,
            std::string pModalidad)
        : id(pId), idEmpresa(pIdEmpresa), titulo(pTitulo),
          descripcion(pDescripcion), requisitos(pRequisitos),
          modalidad(pModalidad), activa(true) {}


    int getId() const { return id; }

    int getIdEmpresa() const { return idEmpresa; }

    std::string getTitulo() const { return titulo; }

    std::string getDescripcion() const { return descripcion; }

    std::string getRequisitos() const { return requisitos; }

    std::string getModalidad() const { return modalidad; }

    bool estaActiva() const { return activa; }


    void cerrarVacante() {
        activa = false;
    }


    bool mismoId(const Vacante& otra) const {
        return id == otra.id;
    }
};
