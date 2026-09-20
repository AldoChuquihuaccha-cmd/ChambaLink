#pragma once
#include <string>

enum class EstadoSolicitud { Pendiente, Aceptada, Rechazada };

class SolicitudConexion {
private:
    std::string codigoEmisor;
    std::string codigoReceptor;
    std::string mensaje;
    EstadoSolicitud estado;

public:
    SolicitudConexion();
    SolicitudConexion(std::string pEmisor, std::string pReceptor, std::string pMensaje);

    std::string getCodigoEmisor() const;
    std::string getCodigoReceptor() const;
    std::string getMensaje() const;
    EstadoSolicitud getEstado() const;

    void aceptar();
    void rechazar();
    bool estaPendiente() const;
};






SolicitudConexion::SolicitudConexion()
    : codigoEmisor(""), codigoReceptor(""), mensaje(""),
    estado(EstadoSolicitud::Pendiente) {
}

SolicitudConexion::SolicitudConexion(std::string pEmisor, std::string pReceptor, std::string pMensaje)
    : codigoEmisor(pEmisor), codigoReceptor(pReceptor), mensaje(pMensaje),
    estado(EstadoSolicitud::Pendiente) {
}

std::string SolicitudConexion::getCodigoEmisor() const { return codigoEmisor; }
std::string SolicitudConexion::getCodigoReceptor() const { return codigoReceptor; }
std::string SolicitudConexion::getMensaje() const { return mensaje; }
EstadoSolicitud SolicitudConexion::getEstado() const { return estado; }

void SolicitudConexion::aceptar() { estado = EstadoSolicitud::Aceptada; }
void SolicitudConexion::rechazar() { estado = EstadoSolicitud::Rechazada; }
bool SolicitudConexion::estaPendiente() const { return estado == EstadoSolicitud::Pendiente; }

