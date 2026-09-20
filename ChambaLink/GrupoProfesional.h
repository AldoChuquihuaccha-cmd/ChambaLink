#pragma once
#include <string>

class GrupoProfesional {
private:
    int id;
    std::string nombre;
    std::string descripcion;
    std::string especialidad;
    int cantidadMiembros;

public:
    GrupoProfesional();

    GrupoProfesional(int pId, std::string pNombre,
                     std::string pDescripcion,
                     std::string pEspecialidad);

    int getId() const;

    std::string getNombre() const;
    std::string getDescripcion() const;
    std::string getEspecialidad() const;

    int getCantidadMiembros() const;

    void agregarMiembro();
    void eliminarMiembro();

    bool mismoId(const GrupoProfesional& otro) const;
};
