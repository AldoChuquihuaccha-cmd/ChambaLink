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
    Postulacion();

    Postulacion(int pId, int pUsuario, int pVacante,
                std::string pFecha);

    int getId() const;
    int getIdUsuario() const;
    int getIdVacante() const;
    std::string getFecha() const;
    EstadoPostulacion getEstado() const;

    void aceptar();
    void rechazar();
    void revisar();

    bool esDeUsuario(int idUsuario) const;
};
