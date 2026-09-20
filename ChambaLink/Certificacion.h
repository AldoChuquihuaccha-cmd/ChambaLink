#pragma once
#include <string>


class Certificacion {

private:

    int id;
    std::string nombre;
    std::string institucion;
    std::string fechaObtencion;
    std::string codigo;


public:


    Certificacion()
        : id(0), nombre(""), institucion(""),
          fechaObtencion(""), codigo("") {}



    Certificacion(int pId,
                  std::string pNombre,
                  std::string pInstitucion,
                  std::string pFecha,
                  std::string pCodigo)
        : id(pId),
          nombre(pNombre),
          institucion(pInstitucion),
          fechaObtencion(pFecha),
          codigo(pCodigo) {}



    int getId() const {
        return id;
    }


    std::string getNombre() const {
        return nombre;
    }


    std::string getInstitucion() const {
        return institucion;
    }


    std::string getFechaObtencion() const {
        return fechaObtencion;
    }


    std::string getCodigo() const {
        return codigo;
    }


    bool mismoId(const Certificacion& otra) const {
        return id == otra.id;
    }
};
