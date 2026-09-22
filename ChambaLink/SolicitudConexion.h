#pragma once
#include <string>

enum class EstadoSolicitud { Pendiente, Aceptada, Rechazada };

// Solicitud de conexion entre dos usuarios.
// Mientras esta pendiente espera en la cola de solicitudes del receptor.
class SolicitudConexion {
private:
    int id;
    int idEmisor;
    int idReceptor;
    std::string mensaje;
    std::string fecha;
    EstadoSolicitud estado;

public:
    SolicitudConexion() {
        id = 0;
        idEmisor = 0;
        idReceptor = 0;
        mensaje = "";
        fecha = "";
        estado = EstadoSolicitud::Pendiente;
    }

    SolicitudConexion(int pId, int pIdEmisor, int pIdReceptor, std::string pMensaje, std::string pFecha) {
        id = pId;
        idEmisor = pIdEmisor;
        idReceptor = pIdReceptor;
        mensaje = pMensaje;
        fecha = pFecha;
        estado = EstadoSolicitud::Pendiente;
    }

    int getId() const { return id; }
    int getIdEmisor() const { return idEmisor; }
    int getIdReceptor() const { return idReceptor; }
    std::string getMensaje() const { return mensaje; }
    std::string getFecha() const { return fecha; }
    EstadoSolicitud getEstado() const { return estado; }

    void aceptar() { estado = EstadoSolicitud::Aceptada; }
    void rechazar() { estado = EstadoSolicitud::Rechazada; }

    bool estaPendiente() const { return estado == EstadoSolicitud::Pendiente; }
    bool esDe(int pIdEmisor) const { return idEmisor == pIdEmisor; }

    std::string estadoToString() const {
        switch (estado) {
        case EstadoSolicitud::Pendiente: return "Pendiente";
        case EstadoSolicitud::Aceptada:  return "Aceptada";
        case EstadoSolicitud::Rechazada: return "Rechazada";
        }
        return "Desconocido";
    }
};
