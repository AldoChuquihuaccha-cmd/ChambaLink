#pragma once
#include <string>


enum class EstadoPostulacion {
    Pendiente,
    Revisada,
    Aceptada,
    Rechazada
};


class Postulacion {

private:

    int id;
    int idUsuario;
    int idVacante;
    std::string fecha;
    EstadoPostulacion estado;


public:

    Postulacion()
        : id(0), idUsuario(0), idVacante(0),
          fecha(""), estado(EstadoPostulacion::Pendiente) {}


    Postulacion(int pId, int pUsuario, int pVacante,
                std::string pFecha)
        : id(pId), idUsuario(pUsuario),
          idVacante(pVacante), fecha(pFecha),
          estado(EstadoPostulacion::Pendiente) {}


    int getId() const {
        return id;
    }


    int getIdUsuario() const {
        return idUsuario;
    }


    int getIdVacante() const {
        return idVacante;
    }


    std::string getFecha() const {
        return fecha;
    }


    EstadoPostulacion getEstado() const {
        return estado;
    }


    void aceptar() {
        estado = EstadoPostulacion::Aceptada;
    }


    void rechazar() {
        estado = EstadoPostulacion::Rechazada;
    }


    void revisar() {
        estado = EstadoPostulacion::Revisada;
    }


    bool esDeUsuario(int pIdUsuario) const {
        return idUsuario == pIdUsuario;
    }
};
