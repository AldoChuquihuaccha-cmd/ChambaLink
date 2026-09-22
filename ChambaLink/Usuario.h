#pragma once
#include <string>
#include <functional>
#include "Lista.h"
#include "ListaDoble.h"
#include "Pila.h"
#include "Cola.h"
#include "Habilidad.h"
#include "Certificacion.h"
#include "ExperienciaLaboral.h"
#include "SolicitudConexion.h"
#include "Accion.h"
#include "Notificacion.h"

// Usuario de ChambaLink. Es la entidad principal del perfil y guarda dentro
// de si las partes que solo tienen sentido dentro de un perfil:
//   habilidades y certificaciones -> Lista (se agregan y se quitan seguido)
//   historial laboral             -> ListaDoble (se recorre en los dos sentidos)
//   solicitudes y notificaciones  -> Cola (se atienden en orden de llegada)
//   acciones recientes            -> Pila (se deshace primero la mas reciente)
//   contactos                     -> Lista de ids de otros usuarios
//
// La clase no expone sus estructuras: quien la usa solo llama a sus metodos.
// Los recorridos entregan referencias constantes a traves de lambdas, asi que
// no se copian los objetos guardados.
//
// Regla importante: un usuario guardado en la red se modifica siempre a traves
// del puntero que devuelve RedProfesional::buscarUsuario, nunca de una copia.
class Usuario {
private:
    int id;
    std::string nombre;
    std::string apellido;
    std::string titular;
    std::string ubicacion;

    Lista<Habilidad> habilidades;
    Lista<Certificacion> certificaciones;
    ListaDoble<ExperienciaLaboral> historialLaboral;
    Cola<SolicitudConexion> solicitudesPendientes;
    Pila<Accion> historialAcciones;
    Cola<Notificacion> notificaciones;
    Lista<int> contactos;

public:
    Usuario() {
        id = 0;
        nombre = "";
        apellido = "";
        titular = "";
        ubicacion = "";
    }

    Usuario(int pId, std::string pNombre, std::string pApellido,
        std::string pTitular, std::string pUbicacion) {
        id = pId;
        nombre = pNombre;
        apellido = pApellido;
        titular = pTitular;
        ubicacion = pUbicacion;
    }

    // ---------- Datos basicos ----------
    int getId() const { return id; }
    std::string getNombre() const { return nombre; }
    std::string getApellido() const { return apellido; }
    std::string getNombreCompleto() const { return nombre + " " + apellido; }
    std::string getTitular() const { return titular; }
    std::string getUbicacion() const { return ubicacion; }

    void setTitular(std::string pTitular) { titular = pTitular; }
    void setUbicacion(std::string pUbicacion) { ubicacion = pUbicacion; }

    bool mismoId(const Usuario& otro) const { return id == otro.id; }

    // ---------- Habilidades ----------
    bool tieneHabilidad(std::string nombreHabilidad) const {
        return habilidades.existe([nombreHabilidad](const Habilidad& h) {
            return h.tieneNombre(nombreHabilidad);
            });
    }

    // Si la habilidad ya existe solo actualiza su nivel y devuelve false.
    bool agregarHabilidad(const Habilidad& h) {
        bool actualizada = habilidades.modificarSi(
            [&h](const Habilidad& x) { return x.mismoNombre(h); },
            [&h](Habilidad& x) { x.setNivel(h.getNivel()); });
        if (actualizada) return false;
        habilidades.agregaFinal(h);
        return true;
    }

    bool eliminarHabilidad(std::string nombreHabilidad) {
        return habilidades.eliminaSi([nombreHabilidad](const Habilidad& h) {
            return h.tieneNombre(nombreHabilidad);
            });
    }

    uint totalHabilidades() const { return habilidades.longitud(); }

    void paraCadaHabilidad(std::function<void(const Habilidad&)> accion) const {
        habilidades.paraCada(accion);
    }

    // ---------- Certificaciones ----------
    void agregarCertificacion(const Certificacion& c) { certificaciones.agregaFinal(c); }

    bool eliminarCertificacion(int idCertificacion) {
        return certificaciones.eliminaSi([idCertificacion](const Certificacion& c) {
            return c.getId() == idCertificacion;
            });
    }

    uint totalCertificaciones() const { return certificaciones.longitud(); }

    void paraCadaCertificacion(std::function<void(const Certificacion&)> accion) const {
        certificaciones.paraCada(accion);
    }

    // ---------- Historial laboral ----------
    // Se agrega al final, asi el ultimo de la lista es el puesto mas reciente.
    void agregarExperiencia(const ExperienciaLaboral& e) { historialLaboral.agregaFinal(e); }

    uint totalExperiencias() const { return historialLaboral.longitud(); }

    // La lambda suma la duracion de cada puesto capturando total por referencia.
    int antiguedadTotalEnAnios(int anioActual) const {
        int total = 0;
        historialLaboral.paraCada([&total, anioActual](const ExperienciaLaboral& e) {
            total = total + e.duracionEnAnios(anioActual);
            });
        return total;
    }

    // Recorre el historial con el iterador de la lista doble.
    // Si desdeMasReciente es true empieza por el ultimo puesto y usa --.
    void paraCadaExperiencia(std::function<void(const ExperienciaLaboral&)> accion,
        bool desdeMasReciente) const {
        if (desdeMasReciente) {
            for (ListaDoble<ExperienciaLaboral>::Iterador it = historialLaboral.ultimo(); it.valido(); --it)
                accion(*it);
        }
        else {
            for (ListaDoble<ExperienciaLaboral>::Iterador it = historialLaboral.primero(); it.valido(); ++it)
                accion(*it);
        }
    }

    // ---------- Solicitudes de conexion recibidas ----------
    void recibirSolicitud(const SolicitudConexion& s) { solicitudesPendientes.encolar(s); }

    bool tieneSolicitudesPendientes() const { return !solicitudesPendientes.esVacia(); }

    uint cantidadSolicitudesPendientes() const { return solicitudesPendientes.longitud(); }

    bool tieneSolicitudDe(int idEmisor) const {
        return solicitudesPendientes.existe([idEmisor](const SolicitudConexion& s) {
            return s.esDe(idEmisor);
            });
    }

    const SolicitudConexion& verSiguienteSolicitud() const { return solicitudesPendientes.frente(); }

    // Retira de la cola la solicitud que llego primero.
    SolicitudConexion atenderSiguienteSolicitud() { return solicitudesPendientes.desencolar(); }

    void paraCadaSolicitud(std::function<void(const SolicitudConexion&)> accion) const {
        solicitudesPendientes.paraCada(accion);
    }

    // ---------- Acciones recientes ----------
    // Ejecuta la accion y la guarda en la pila en un solo paso.
    void registrarAccion(const Accion& a) {
        Accion copia = a;
        copia.ejecutar();
        historialAcciones.apilar(copia);
    }

    bool tieneAccionesRegistradas() const { return !historialAcciones.esVacia(); }

    // Primero se retira de la pila y despues se deshace, por si la lambda
    // de deshacer vuelve a tocar a este mismo usuario.
    std::string deshacerUltimaAccion() {
        if (!tieneAccionesRegistradas()) return "";
        Accion ultima = historialAcciones.desapilar();
        ultima.deshacerAccion();
        return ultima.getDescripcion();
    }

    void paraCadaAccion(std::function<void(const Accion&)> accion) const {
        historialAcciones.paraCada(accion);
    }

    // ---------- Notificaciones ----------
    void recibirNotificacion(const Notificacion& n) { notificaciones.encolar(n); }

    bool tieneNotificaciones() const { return !notificaciones.esVacia(); }

    uint cantidadNotificaciones() const { return notificaciones.longitud(); }

    // Recorre las notificaciones sin retirarlas de la cola.
    void paraCadaNotificacion(std::function<void(const Notificacion&)> accion) const {
        notificaciones.paraCada(accion);
    }

    Notificacion leerSiguienteNotificacion() {
        Notificacion n = notificaciones.desencolar();
        n.marcarLeida();
        return n;
    }

    // ---------- Contactos ----------
    bool esContactoDirecto(int idUsuario) const {
        return contactos.existe([idUsuario](const int& c) { return c == idUsuario; });
    }

    // Devuelve false si ya era contacto o si es el mismo usuario.
    bool agregarContacto(int idUsuario) {
        if (idUsuario == id || esContactoDirecto(idUsuario)) return false;
        contactos.agregaFinal(idUsuario);
        return true;
    }

    bool eliminarContacto(int idUsuario) {
        return contactos.eliminaSi([idUsuario](const int& c) { return c == idUsuario; });
    }

    uint totalContactos() const { return contactos.longitud(); }

    void paraCadaContacto(std::function<void(const int&)> accion) const {
        contactos.paraCada(accion);
    }
};