#pragma once
#include <string>

// Certificacion obtenida por el usuario. Forma parte de su perfil,
// asi que el dueno queda definido por la lista que la contiene.
class Certificacion {
private:
    int id;
    std::string nombre;
    std::string institucion;
    std::string fechaObtencion;
    std::string codigoCredencial;

public:
    Certificacion() {
        id = 0;
        nombre = "";
        institucion = "";
        fechaObtencion = "";
        codigoCredencial = "";
    }

    Certificacion(int pId, std::string pNombre, std::string pInstitucion,
        std::string pFechaObtencion, std::string pCodigoCredencial) {
        id = pId;
        nombre = pNombre;
        institucion = pInstitucion;
        fechaObtencion = pFechaObtencion;
        codigoCredencial = pCodigoCredencial;
    }

    int getId() const { return id; }
    std::string getNombre() const { return nombre; }
    std::string getInstitucion() const { return institucion; }
    std::string getFechaObtencion() const { return fechaObtencion; }
    std::string getCodigoCredencial() const { return codigoCredencial; }

    bool mismoId(const Certificacion& otra) const { return id == otra.id; }
    bool esDeInstitucion(std::string pInstitucion) const { return institucion == pInstitucion; }
};
