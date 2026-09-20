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


    GrupoProfesional()
        : id(0), nombre(""), descripcion(""),
          especialidad(""), cantidadMiembros(0) {}


    GrupoProfesional(int pId, std::string pNombre,
                     std::string pDescripcion,
                     std::string pEspecialidad)
        : id(pId),
          nombre(pNombre),
          descripcion(pDescripcion),
          especialidad(pEspecialidad),
          cantidadMiembros(0) {}



    int getId() const {
        return id;
    }


    std::string getNombre() const {
        return nombre;
    }


    std::string getDescripcion() const {
        return descripcion;
    }


    std::string getEspecialidad() const {
        return especialidad;
    }


    int getCantidadMiembros() const {
        return cantidadMiembros;
    }


    void agregarMiembro() {
        cantidadMiembros++;
    }


    void eliminarMiembro() {

        if(cantidadMiembros > 0)
            cantidadMiembros--;
    }


    bool mismoId(const GrupoProfesional& otro) const {
        return id == otro.id;
    }
};
