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