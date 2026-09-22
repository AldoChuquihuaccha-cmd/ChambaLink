#pragma once
#include <string>

enum class EstadoPostulacion { Pendiente, Revisada, Aceptada, Rechazada };

// Relaciona a un usuario con una vacante a la que postulo.
class Postulacion {
private:
    int id;
    int idUsuario;
    int idVacante;
    std::string fecha;
    EstadoPostulacion estado;

public:
    Postulacion() {
        id = 0;
        idUsuario = 0;
        idVacante = 0;
        fecha = "";
        estado = EstadoPostulacion::Pendiente;
    }

    Postulacion(int pId, int pIdUsuario, int pIdVacante, std::string pFecha) {
        id = pId;
        idUsuario = pIdUsuario;
        idVacante = pIdVacante;
        fecha = pFecha;
        estado = EstadoPostulacion::Pendiente;
    }

    int getId() const { return id; }
    int getIdUsuario() const { return idUsuario; }
    int getIdVacante() const { return idVacante; }
    std::string getFecha() const { return fecha; }
    EstadoPostulacion getEstado() const { return estado; }

    void revisar() { estado = EstadoPostulacion::Revisada; }
    void aceptar() { estado = EstadoPostulacion::Aceptada; }
    void rechazar() { estado = EstadoPostulacion::Rechazada; }

    bool estaPendiente() const { return estado == EstadoPostulacion::Pendiente; }

    std::string estadoToString() const {
        switch (estado) {
        case EstadoPostulacion::Pendiente: return "Pendiente";
        case EstadoPostulacion::Revisada:  return "Revisada";
        case EstadoPostulacion::Aceptada:  return "Aceptada";
        case EstadoPostulacion::Rechazada: return "Rechazada";
        }
        return "Desconocido";
    }

    bool esDeUsuario(int pIdUsuario) const { return idUsuario == pIdUsuario; }
    bool esDeVacante(int pIdVacante) const { return idVacante == pIdVacante; }
};
