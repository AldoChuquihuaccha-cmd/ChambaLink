#pragma once
#include <string>

// Empresa que publica vacantes y que aparece en la experiencia laboral
// de los usuarios. Una empresa puede tener muchas vacantes: cada Vacante
// guarda el id de su empresa.
class Empresa {
private:
    int id;
    std::string nombre;
    std::string sector;
    std::string ubicacion;    // distrito del establecimiento
    std::string correo;       // con el que inicia sesion (se guarda en minusculas)
    std::string contrasena;

public:
    Empresa() {
        id = 0;
        nombre = "";
        sector = "";
        ubicacion = "";
        correo = "";
        contrasena = "";
    }

    Empresa(int pId, std::string pNombre, std::string pSector, std::string pUbicacion,
        std::string pCorreo, std::string pContrasena) {
        id = pId;
        nombre = pNombre;
        sector = pSector;
        ubicacion = pUbicacion;
        correo = pCorreo;
        contrasena = pContrasena;
    }

    int getId() const { return id; }
    std::string getNombre() const { return nombre; }
    std::string getSector() const { return sector; }
    std::string getUbicacion() const { return ubicacion; }
    std::string getCorreo() const { return correo; }

    // Solo la usa GestorArchivos para guardar la cuenta en empresas.csv.
    std::string getContrasena() const { return contrasena; }

    void setSector(std::string pSector) { sector = pSector; }
    void setUbicacion(std::string pUbicacion) { ubicacion = pUbicacion; }

    bool mismoId(const Empresa& otra) const { return id == otra.id; }
    bool mismoNombre(const Empresa& otra) const { return nombre == otra.nombre; }
    bool esDelSector(std::string pSector) const { return sector == pSector; }

    bool tieneCorreo(std::string pCorreo) const { return correo == pCorreo; }
    bool contrasenaCorrecta(std::string pContrasena) const { return contrasena == pContrasena; }
};