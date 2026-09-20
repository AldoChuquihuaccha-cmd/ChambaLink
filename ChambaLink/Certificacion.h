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
    Certificacion();

    Certificacion(int pId,
                  std::string pNombre,
                  std::string pInstitucion,
                  std::string pFecha,
                  std::string pCodigo);

    int getId() const;

    std::string getNombre() const;
    std::string getInstitucion() const;
    std::string getFechaObtencion() const;
    std::string getCodigo() const;

    bool mismoId(const Certificacion& otra) const;
};
