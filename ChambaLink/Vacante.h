#pragma once
#include <string>
#include <functional>
#include "Cola.h"

// Oferta de empleo publicada por una empresa.
// Las postulaciones que recibe esperan en una cola: se revisan en el mismo
// orden en que llegaron.
class Vacante {
private:
    int id;
    int idEmpresa;
    std::string titulo;
    std::string descripcion;
    std::string requisitos;
    std::string modalidad;
    bool activa;
    Cola<int> postulacionesPorRevisar;

public:
    Vacante() {
        id = 0;
        idEmpresa = 0;
        titulo = "";
        descripcion = "";
        requisitos = "";
        modalidad = "";
        activa = true;
    }

    Vacante(int pId, int pIdEmpresa, std::string pTitulo, std::string pDescripcion,
        std::string pRequisitos, std::string pModalidad) {
        id = pId;
        idEmpresa = pIdEmpresa;
        titulo = pTitulo;
        descripcion = pDescripcion;
        requisitos = pRequisitos;
        modalidad = pModalidad;
        activa = true;
    }

    int getId() const { return id; }
    int getIdEmpresa() const { return idEmpresa; }
    std::string getTitulo() const { return titulo; }
    std::string getDescripcion() const { return descripcion; }
    std::string getRequisitos() const { return requisitos; }
    std::string getModalidad() const { return modalidad; }
    bool estaActiva() const { return activa; }

    void setDescripcion(std::string pDescripcion) { descripcion = pDescripcion; }
    void setRequisitos(std::string pRequisitos) { requisitos = pRequisitos; }
    void cerrar() { activa = false; }

    void recibirPostulacion(int idPostulacion) { postulacionesPorRevisar.encolar(idPostulacion); }
    bool tienePostulacionesPorRevisar() const { return !postulacionesPorRevisar.esVacia(); }
    uint cantidadPorRevisar() const { return postulacionesPorRevisar.longitud(); }

    // Retira de la cola la postulacion mas antigua y devuelve su id.
    int siguientePostulacionPorRevisar() { return postulacionesPorRevisar.desencolar(); }

    // Recorre la cola sin retirar nada, en orden de llegada.
    void paraCadaPostulacionPorRevisar(std::function<void(const int&)> accion) const {
        postulacionesPorRevisar.paraCada(accion);
    }

    bool mismoId(const Vacante& otra) const { return id == otra.id; }
    bool esDeEmpresa(int pIdEmpresa) const { return idEmpresa == pIdEmpresa; }
};