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




Usuario::Usuario() : codigo(""), nombre(""), apellido(""), titular(""), ubicacion("") {}

Usuario::Usuario(std::string pCodigo, std::string pNombre, std::string pApellido,
    std::string pTitular, std::string pUbicacion)
    : codigo(pCodigo), nombre(pNombre), apellido(pApellido),
    titular(pTitular), ubicacion(pUbicacion) {
}

std::string Usuario::getCodigo() const { return codigo; }
std::string Usuario::getNombreCompleto() const { return nombre + " " + apellido; }
std::string Usuario::getTitular() const { return titular; }
std::string Usuario::getUbicacion() const { return ubicacion; }

// ---------- Habilidades ----------
void Usuario::agregarHabilidad(Habilidad h) { habilidades.agregaFinal(h); }

uint Usuario::totalHabilidades() const { return habilidades.longitud(); }

void Usuario::paraCadaHabilidad(std::function<void(const Habilidad&)> accion) const {
    habilidades.paraCada(accion);
}

// ---------- Historial laboral ----------
void Usuario::agregarExperiencia(ExperienciaLaboral e) { historialLaboral.agregaFinal(e); }

// Lambda que acumula por efecto lateral (captura 'total' por referencia).
int Usuario::antiguedadTotalEnAnios(int anioActual) const {
    int total = 0;
    historialLaboral.paraCada([&total, anioActual](const ExperienciaLaboral& e) {
        total += e.duracionEnAnios(anioActual);
        });
    return total;
}

void Usuario::paraCadaExperiencia(std::function<void(const ExperienciaLaboral&)> accion) const {
    historialLaboral.paraCada(accion);
}

// ---------- Solicitudes: FIFO manual sobre Lista (entra al final, sale del inicio) ----------
void Usuario::registrarSolicitud(SolicitudConexion s) { solicitudesPendientes.agregaFinal(s); }

bool Usuario::tieneSolicitudesPendientes() const { return !solicitudesPendientes.esVacia(); }

SolicitudConexion Usuario::atenderSiguienteSolicitud() {
    SolicitudConexion primera = solicitudesPendientes.obtenerInicial();
    solicitudesPendientes.eliminaInicial();
    return primera;
}

// ---------- Acciones: LIFO manual sobre Lista (entra y sale por el inicio) ----------
// registrarAccion ejecuta Y guarda en un solo paso ("hacer y recordar").
void Usuario::registrarAccion(Accion a) {
    a.ejecutar();
    historialAcciones.agregaInicial(a);
}

bool Usuario::tieneAccionesRegistradas() const { return !historialAcciones.esVacia(); }

void Usuario::deshacerUltimaAccion() {
    if (!tieneAccionesRegistradas()) return;
    Accion ultima = historialAcciones.obtenerInicial();
    ultima.deshacerAccion();
    historialAcciones.eliminaInicial();
}

// ---------- Contactos ----------
void Usuario::agregarContacto(std::string codigoUsuario) {
    if (!esContactoDirecto(codigoUsuario)) contactos.agregaFinal(codigoUsuario);
}

bool Usuario::esContactoDirecto(std::string codigoUsuario) const {
    return contactos.existe([&codigoUsuario](const std::string& c) { return c == codigoUsuario; });
}

uint Usuario::totalContactos() const { return contactos.longitud(); }

void Usuario::paraCadaContacto(std::function<void(const std::string&)> accion) const {
    contactos.paraCada(accion);
}