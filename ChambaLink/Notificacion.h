#pragma once
#include <string>

// Motivo por el que se genera una notificacion.
enum class TipoNotificacion { NuevaConexion, NuevoComentario, MeGusta, NuevaRecomendacion, NuevoMensaje, NuevaVacante };

class Notificacion {
private:
    int id;
    int idDestino;          // id del Usuario que recibe la notificacion
    TipoNotificacion tipo;
    std::string mensaje;
    std::string fecha;      // formato "dd/mm/aaaa"
    bool leida;

public:
    Notificacion();
    Notificacion(int pId, int pIdDestino, TipoNotificacion pTipo, std::string pMensaje, std::string pFecha);

    int getId() const;
    int getIdDestino() const;
    TipoNotificacion getTipo() const;
    std::string getMensaje() const;
    std::string getFecha() const;
    bool estaLeida() const;

    void marcarLeida();

    std::string tipoToString() const;

    // Comparadores; se usan como predicado lambda al buscar en Lista<Notificacion>.
    bool mismoId(const Notificacion& otra) const;
    bool esPara(int pIdDestino) const;
};

Notificacion::Notificacion()
    : id(0), idDestino(0), tipo(TipoNotificacion::NuevoMensaje), mensaje(""), fecha(""), leida(false) {
}

Notificacion::Notificacion(int pId, int pIdDestino, TipoNotificacion pTipo, std::string pMensaje, std::string pFecha)
    : id(pId), idDestino(pIdDestino), tipo(pTipo), mensaje(pMensaje), fecha(pFecha), leida(false) {
}

int Notificacion::getId() const { return id; }
int Notificacion::getIdDestino() const { return idDestino; }
TipoNotificacion Notificacion::getTipo() const { return tipo; }
std::string Notificacion::getMensaje() const { return mensaje; }
std::string Notificacion::getFecha() const { return fecha; }
bool Notificacion::estaLeida() const { return leida; }

void Notificacion::marcarLeida() { leida = true; }

std::string Notificacion::tipoToString() const {
    switch (tipo) {
    case TipoNotificacion::NuevaConexion:      return "Nueva conexion";
    case TipoNotificacion::NuevoComentario:    return "Nuevo comentario";
    case TipoNotificacion::MeGusta:            return "Me gusta";
    case TipoNotificacion::NuevaRecomendacion: return "Nueva recomendacion";
    case TipoNotificacion::NuevoMensaje:       return "Nuevo mensaje";
    case TipoNotificacion::NuevaVacante:       return "Nueva vacante";
    }
    return "Desconocido";
}

bool Notificacion::mismoId(const Notificacion& otra) const { return id == otra.id; }
bool Notificacion::esPara(int pIdDestino) const { return idDestino == pIdDestino; }