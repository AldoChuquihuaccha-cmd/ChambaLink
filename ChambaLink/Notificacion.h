#pragma once
#include <string>

// Motivo por el que se genera una notificacion.
enum class TipoNotificacion {
    NuevaSolicitud, NuevaConexion, NuevoComentario, MeGusta,
    NuevaRecomendacion, NuevoMensaje, NuevaVacante, EstadoPostulacion
};

// Aviso del sistema para un usuario. Espera en la cola de notificaciones
// hasta que el usuario la lee.
class Notificacion {
private:
    int id;
    int idDestino;
    TipoNotificacion tipo;
    std::string mensaje;
    std::string fecha;
    bool leida;

public:
    Notificacion() {
        id = 0;
        idDestino = 0;
        tipo = TipoNotificacion::NuevoMensaje;
        mensaje = "";
        fecha = "";
        leida = false;
    }

    Notificacion(int pId, int pIdDestino, TipoNotificacion pTipo, std::string pMensaje, std::string pFecha) {
        id = pId;
        idDestino = pIdDestino;
        tipo = pTipo;
        mensaje = pMensaje;
        fecha = pFecha;
        leida = false;
    }

    int getId() const { return id; }
    int getIdDestino() const { return idDestino; }
    TipoNotificacion getTipo() const { return tipo; }
    std::string getMensaje() const { return mensaje; }
    std::string getFecha() const { return fecha; }
    bool estaLeida() const { return leida; }

    void marcarLeida() { leida = true; }

    std::string tipoToString() const {
        switch (tipo) {
        case TipoNotificacion::NuevaSolicitud:     return "Nueva solicitud";
        case TipoNotificacion::NuevaConexion:      return "Nueva conexion";
        case TipoNotificacion::NuevoComentario:    return "Nuevo comentario";
        case TipoNotificacion::MeGusta:            return "Me gusta";
        case TipoNotificacion::NuevaRecomendacion: return "Nueva recomendacion";
        case TipoNotificacion::NuevoMensaje:       return "Nuevo mensaje";
        case TipoNotificacion::NuevaVacante:       return "Nueva vacante";
        case TipoNotificacion::EstadoPostulacion:  return "Estado de postulacion";
        }
        return "Desconocido";
    }

    bool esPara(int pIdDestino) const { return idDestino == pIdDestino; }
};
