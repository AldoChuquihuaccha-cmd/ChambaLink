#pragma once
#include <string>
#include <functional>
#include "Lista.h"
#include "Habilidad.h"
#include "ExperienciaLaboral.h"
#include "SolicitudConexion.h"
#include "Accion.h"

// Entidad raiz del dominio de ChambaLink. Compone a las demas entidades
// a traves de Lista<T> (TDA generico) y NO expone esos contenedores
// directamente: solo comportamiento de dominio (encapsulacion / Ley
// de Demeter). Quien use Usuario no sabe -ni le importa- si por debajo
// hay una Lista, una Pila o una Cola.
//
// Nota de diseno: solicitudesPendientes se comporta como una Cola
// (agregaFinal + eliminaInicial = FIFO, ambas O(1)) e historialAcciones
// como una Pila (agregaInicial + eliminaInicial = LIFO, ambas O(1)),
// aunque hoy ambas se apoyan en Lista<T>. Cuando Cola<T> y Pila<T>
// existan como clases propias, solo cambia el TIPO del atributo
// privado; la interfaz publica de Usuario no cambia. Lo mismo aplica
// a historialLaboral cuando exista ListaDoble<T>.
class Usuario {
private:
    std::string codigo;
    std::string nombre;
    std::string apellido;
    std::string titular;      // encabezado profesional, ej. "Ingeniera de Software"
    std::string ubicacion;

    Lista<Habilidad> habilidades;
    Lista<ExperienciaLaboral> historialLaboral;
    Lista<SolicitudConexion> solicitudesPendientes;
    Lista<Accion> historialAcciones;
    Lista<std::string> contactos;   // codigos de otros Usuario (asociacion reflexiva)

public:
    Usuario();
    Usuario(std::string pCodigo, std::string pNombre, std::string pApellido,
        std::string pTitular, std::string pUbicacion);

    // ---------- Datos basicos ----------
    std::string getCodigo() const;
    std::string getNombreCompleto() const;
    std::string getTitular() const;
    std::string getUbicacion() const;

    // ---------- Habilidades (lista simple) ----------
    void agregarHabilidad(Habilidad h);
    uint totalHabilidades() const;
    void paraCadaHabilidad(std::function<void(const Habilidad&)> accion) const;

    // ---------- Historial laboral (sera lista doble) ----------
    void agregarExperiencia(ExperienciaLaboral e);
    int antiguedadTotalEnAnios(int anioActual) const;
    void paraCadaExperiencia(std::function<void(const ExperienciaLaboral&)> accion) const;

    // ---------- Solicitudes de conexion (comportamiento de Cola) ----------
    void registrarSolicitud(SolicitudConexion s);
    bool tieneSolicitudesPendientes() const;
    SolicitudConexion atenderSiguienteSolicitud();

    // ---------- Acciones recientes (comportamiento de Pila, patron Command) ----------
    void registrarAccion(Accion a);
    bool tieneAccionesRegistradas() const;
    void deshacerUltimaAccion();

    // ---------- Contactos (asociacion reflexiva Usuario-Usuario) ----------
    void agregarContacto(std::string codigoUsuario);
    bool esContactoDirecto(std::string codigoUsuario) const;
    uint totalContactos() const;
    void paraCadaContacto(std::function<void(const std::string&)> accion) const;
};