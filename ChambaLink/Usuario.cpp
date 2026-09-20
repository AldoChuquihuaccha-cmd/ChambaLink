#include "Usuario.h"

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