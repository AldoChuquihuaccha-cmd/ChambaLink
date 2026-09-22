#pragma once
#include <string>
#include <functional>
#include "Lista.h"

// Grupo de interes profesional. Guarda los ids de sus miembros en una lista,
// no solo un contador, para poder saber quienes son.
class GrupoProfesional {
private:
    int id;
    std::string nombre;
    std::string descripcion;
    std::string especialidad;
    Lista<int> miembros;

public:
    GrupoProfesional() {
        id = 0;
        nombre = "";
        descripcion = "";
        especialidad = "";
    }

    GrupoProfesional(int pId, std::string pNombre, std::string pDescripcion, std::string pEspecialidad) {
        id = pId;
        nombre = pNombre;
        descripcion = pDescripcion;
        especialidad = pEspecialidad;
    }

    int getId() const { return id; }
    std::string getNombre() const { return nombre; }
    std::string getDescripcion() const { return descripcion; }
    std::string getEspecialidad() const { return especialidad; }
    uint getCantidadMiembros() const { return miembros.longitud(); }

    void setDescripcion(std::string pDescripcion) { descripcion = pDescripcion; }

    bool esMiembro(int idUsuario) const {
        return miembros.existe([idUsuario](const int& id) { return id == idUsuario; });
    }

    // Devuelve false si el usuario ya era miembro.
    bool agregarMiembro(int idUsuario) {
        if (esMiembro(idUsuario)) return false;
        miembros.agregaFinal(idUsuario);
        return true;
    }

    bool eliminarMiembro(int idUsuario) {
        return miembros.eliminaSi([idUsuario](const int& id) { return id == idUsuario; });
    }

    void paraCadaMiembro(std::function<void(const int&)> accion) const {
        miembros.paraCada(accion);
    }

    bool mismoId(const GrupoProfesional& otro) const { return id == otro.id; }
};
