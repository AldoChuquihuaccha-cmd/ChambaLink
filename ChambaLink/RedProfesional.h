#pragma once
#include <string>
#include <ctime>
#include <functional>
#include <cctype>
#include <sstream>
#include "Lista.h"
#include "Usuario.h"
#include "Ordenamiento.h"
#include "Empresa.h"
#include "Vacante.h"
#include "Postulacion.h"
#include "GrupoProfesional.h"
#include "Publicacion.h"
#include "Comentario.h"
#include "Recomendacion.h"
#include "Mensaje.h"

// RedProfesional es el centro de la aplicacion:
//   1. guarda todos los catalogos, una lista por entidad,
//   2. genera los ids unicos,
//   3. se encarga de todo lo que involucra a mas de una clase: validar que
//      los ids existan, conectar a los dos usuarios, avisar con notificaciones.
//
// Las entidades se relacionan por id. Aqui esos ids se convierten en objetos
// con buscarPtr, que devuelve el objeto real y no una copia.
//
// Cuando una operacion falla, el motivo queda en ultimoError para poder
// mostrarlo en el menu de consola.
//
// Esta clase no se puede copiar: las acciones guardadas en los usuarios
// capturan el puntero this, asi que una copia dejaria punteros invalidos.
//
// Reparto del trabajo:
//   Red y conexiones ......... Joao
//   Contenido y comunicacion . Aldo
//   Empleo y grupos .......... Piero
// GestorArchivos necesita leer y escribir los catalogos directamente
// para poder guardarlos y volver a cargarlos con sus ids originales.
class GestorArchivos;

// Resultado de buscarProfesionales: el usuario y cuantas palabras clave le coincidieron.
struct Coincidencia {
    int idUsuario = 0;
    int cantidad = 0;
};

// Resultado para la seccion Mi red. Guarda el usuario sugerido y
// cuantos contactos directos tiene en comun con la sesion actual.
struct SugerenciaConexion {
    int idUsuario = 0;
    int contactosEnComun = 0;
};

class RedProfesional {
    friend class GestorArchivos;

private:
    Lista<Usuario> usuarios;
    Lista<Empresa> empresas;
    Lista<Vacante> vacantes;
    Lista<Postulacion> postulaciones;
    Lista<GrupoProfesional> grupos;
    Lista<Publicacion> publicaciones;
    Lista<Comentario> comentarios;
    Lista<Recomendacion> recomendaciones;
    Lista<Mensaje> mensajes;

    // Contadores de ids. Al cargar desde archivo hay que dejarlos en el id
    // mas alto encontrado mas uno.
    int sigUsuario;
    int sigEmpresa;
    int sigVacante;
    int sigPostulacion;
    int sigGrupo;
    int sigPublicacion;
    int sigComentario;
    int sigRecomendacion;
    int sigMensaje;
    int sigSolicitud;
    int sigNotificacion;
    int sigCertificacion;

    std::string ultimoError;

    // Convierte "C++, SQL,Java" en la lista ["C++", "SQL", "Java"]. O(n)
    Lista<std::string> separarRequisitos(std::string texto) const {
        Lista<std::string> resultado;
        std::stringstream ss(texto);
        std::string requisito;
        while (getline(ss, requisito, ',')) {
            while (requisito != "" && requisito[0] == ' ') requisito.erase(0, 1);   // espacios iniciales
            if (requisito != "") resultado.agregaFinal(requisito);
        }
        return resultado;
    }

    // Cuenta, de forma recursiva, cuantos requisitos de la vacante tiene el usuario.
    // Se recorre la lista con su iterador para avanzar en O(1) por llamada.
    // Caso base: el iterador llego al final.
    // Caso recursivo: revisa el requisito actual y sigue con el siguiente.
    // Complejidad: O(r * h), r = requisitos y h = habilidades del usuario.
    int compararRequisitosRec(Lista<std::string>::Iterador actual,
        Lista<std::string>::Iterador fin, const Usuario& usuario) const {
        if (!(actual != fin)) return 0;

        int encontrado = 0;
        if (usuario.tieneHabilidad(*actual)) encontrado = 1;

        ++actual;
        return encontrado + compararRequisitosRec(actual, fin, usuario);
    }

    // Guarda el motivo del error y devuelve false, para escribir una sola
    // linea en cada validacion.
    bool fallar(std::string motivo) {
        ultimoError = motivo;
        return false;
    }

    void notificar(int idDestino, TipoNotificacion tipo, std::string mensaje) {
        Usuario* destino = buscarUsuario(idDestino);
        if (destino == nullptr) return;
        destino->recibirNotificacion(Notificacion(sigNotificacion, idDestino, tipo, mensaje, fechaHoy()));
        sigNotificacion++;
    }

    // La conexion es de ida y vuelta: siempre se actualizan los dos usuarios.
    void conectar(int idA, int idB) {
        Usuario* a = buscarUsuario(idA);
        Usuario* b = buscarUsuario(idB);
        if (a == nullptr || b == nullptr) return;
        a->agregarContacto(idB);
        b->agregarContacto(idA);
    }

    void desconectar(int idA, int idB) {
        Usuario* a = buscarUsuario(idA);
        Usuario* b = buscarUsuario(idB);
        if (a != nullptr) a->eliminarContacto(idB);
        if (b != nullptr) b->eliminarContacto(idA);
    }

    // Convierte texto a minusculas para realizar busquedas sin distinguir
    // mayusculas de minusculas. Se trabaja caracter por caracter.
    static std::string textoMinusculas(std::string texto) {
        for (size_t i = 0; i < texto.length(); i++)
            texto[i] = (char)std::tolower((unsigned char)texto[i]);
        return texto;
    }

    // "2019" -> 2019. Devuelve -1 si no son 4 digitos.
    static int aAnio(std::string texto) {
        if (texto.length() != 4) return -1;
        for (int i = 0; i < 4; i++)
            if (texto[i] < '0' || texto[i] > '9') return -1;
        return std::stoi(texto);
    }

    bool haySolicitudPendienteEntre(int idA, int idB) const {
        const Usuario* a = buscarUsuario(idA);
        const Usuario* b = buscarUsuario(idB);
        if (a == nullptr || b == nullptr) return false;
        return a->tieneSolicitudDe(idB) || b->tieneSolicitudDe(idA);
    }

    int contactosEnComun(int idA, int idB) const {
        const Usuario* a = buscarUsuario(idA);
        const Usuario* b = buscarUsuario(idB);
        if (a == nullptr || b == nullptr) return 0;

        int total = 0;
        a->paraCadaContacto([b, &total](const int& idContacto) {
            if (b->esContactoDirecto(idContacto)) total++;
            });
        return total;
    }

    // Recorre la red de forma recursiva hasta la profundidad indicada.
    // Caso base: profundidad 0. La profundidad asegura que la recursion termina
    // aunque haya ciclos (A conoce a B y B conoce a A).
    // visitados solo evita que una persona se repita en descubiertos; por eso
    // siempre se sigue bajando, aunque el contacto ya se haya visto por otro camino.
    void explorarConexionesRec(int idActual, int profundidad,
        Lista<int>& visitados, Lista<int>& descubiertos) const {
        if (profundidad <= 0) return;
        const Usuario* actual = buscarUsuario(idActual);
        if (actual == nullptr) return;

        actual->paraCadaContacto([this, profundidad, &visitados, &descubiertos](const int& idContacto) {
            bool yaVisitado = visitados.existe([idContacto](const int& id) { return id == idContacto; });
            if (!yaVisitado) {
                visitados.agregaFinal(idContacto);
                descubiertos.agregaFinal(idContacto);
            }
            explorarConexionesRec(idContacto, profundidad - 1, visitados, descubiertos);
            });
    }

    Empresa* buscarEmpresa(int idEmpresa) {
        return empresas.buscarPtr([idEmpresa](const Empresa& e) { return e.getId() == idEmpresa; });
    }

    Vacante* buscarVacante(int idVacante) {
        return vacantes.buscarPtr([idVacante](const Vacante& v) { return v.getId() == idVacante; });
    }

    Postulacion* buscarPostulacion(int idPostulacion) {
        return postulaciones.buscarPtr([idPostulacion](const Postulacion& p) { return p.getId() == idPostulacion; });
    }

    GrupoProfesional* buscarGrupo(int idGrupo) {
        return grupos.buscarPtr([idGrupo](const GrupoProfesional& g) { return g.getId() == idGrupo; });
    }

    Publicacion* buscarPublicacion(int idPublicacion) {
        return publicaciones.buscarPtr([idPublicacion](const Publicacion& p) { return p.getId() == idPublicacion; });
    }

    // Recorre las respuestas de idPadre y, por cada una, sus propias respuestas (recursivo).
    // idPadre = 0 significa "comentarios directos a la publicacion".
    // No hay ciclos: un comentario solo puede responder a uno que ya existia.
    void recorrerRespuestas(int idPublicacion, int idPadre, int nivel,
        std::function<void(const Comentario&, int)>& accion) const {
        comentarios.paraCada([this, idPublicacion, idPadre, nivel, &accion](const Comentario& c) {
            if (c.esDePublicacion(idPublicacion) && c.esRespuestaA(idPadre)) {
                accion(c, nivel);                                               // el comentario
                recorrerRespuestas(idPublicacion, c.getId(), nivel + 1, accion); // sus respuestas
            }
            });
    }

    Comentario* buscarComentario(int idComentario) {
        return comentarios.buscarPtr([idComentario](const Comentario& c) { return c.getId() == idComentario; });
    }

    // ---------- Validacion de cuentas ----------

    // Reglas comunes a todos los campos del registro:
    //   - obligatorio o no,
    //   - sin comas, porque la coma separa listas (por ejemplo los requisitos),
    //   - largo maximo, para que quepa en pantalla.
    bool validarCampo(const std::string& valor, const std::string& nombreCampo,
        int largoMaximo, bool obligatorio) {
        if (obligatorio && valor == "") return fallar("El campo " + nombreCampo + " es obligatorio");
        if (valor.find(',') != std::string::npos) return fallar("No se permiten comas");
        if ((int)valor.length() > largoMaximo)
            return fallar("El campo " + nombreCampo + " admite como maximo "
                + std::to_string(largoMaximo) + " caracteres");
        return true;
    }

    // Un correo no puede usarse en dos cuentas, sea de individuo o de empresa.
    bool correoEnUso(const std::string& correo) const {
        bool enUsuarios = usuarios.existe([&correo](const Usuario& u) { return u.tieneCorreo(correo); });
        bool enEmpresas = empresas.existe([&correo](const Empresa& e) { return e.tieneCorreo(correo); });
        return enUsuarios || enEmpresas;
    }

    bool validarCredenciales(const std::string& correo, const std::string& contrasena) {
        if (!validarCampo(correo, "correo", MAX_CORREO, true)) return false;
        if (!validarCampo(contrasena, "contrasena", MAX_CONTRASENA, true)) return false;
        if (correoEnUso(correo)) return fallar("Ese correo ya esta registrado");
        return true;
    }

public:
    // Largos maximos de los campos de registro. Login los usa para no dejar
    // escribir mas de la cuenta y RedProfesional para validar.
    static const int MAX_NOMBRE = 30;
    static const int MAX_APELLIDO = 30;
    static const int MAX_TITULAR = 50;
    static const int MAX_UBICACION = 30;
    static const int MAX_CORREO = 50;
    static const int MAX_CONTRASENA = 20;
    static const int MAX_NOMBRE_EMPRESA = 50;
    static const int MAX_SECTOR = 30;
    static const int MAX_CERTIFICACION = 40;   // nombre de la certificacion
    static const int MAX_INSTITUCION = 40;
    static const int MAX_CODIGO = 30;
    static const int MAX_PUBLICACION = 121;    // dos lineas de 60 en pantalla
    static const int MAX_COMENTARIO = 60;
    static const int MAX_MENSAJE = 58;
    static const int MAX_RECOMENDACION = 121; // dos lineas de 60 en pantalla
    static const int MAX_HABILIDAD = 30;
    static const int MAX_CARGO = 40;

    RedProfesional() {
        // Las cuentas empiezan en 1000 para que su id siempre tenga 4 cifras.
        sigUsuario = 1000;
        sigEmpresa = 1000;
        sigVacante = 1;
        sigPostulacion = 1;
        sigGrupo = 1;
        sigPublicacion = 1;
        sigComentario = 1;
        sigRecomendacion = 1;
        sigMensaje = 1;
        sigSolicitud = 1;
        sigNotificacion = 1;
        sigCertificacion = 1;
        ultimoError = "";
    }

    // Se prohibe copiar la red (ver el comentario de la clase).
    RedProfesional(const RedProfesional& otra) = delete;
    RedProfesional& operator=(const RedProfesional& otra) = delete;

    // Fecha del sistema en formato "aaaa/mm/dd".
    static std::string fechaHoy() {
        time_t ahora = time(nullptr);
        tm fecha;
#ifdef _MSC_VER
        localtime_s(&fecha, &ahora);
#else
        localtime_r(&ahora, &fecha);
#endif
        char texto[11];
        strftime(texto, sizeof(texto), "%Y/%m/%d", &fecha);
        return std::string(texto);
    }

    std::string getUltimoError() const { return ultimoError; }

    // ==================== Usuarios ====================

    // Devuelve el id del nuevo usuario, o -1 si los datos no son validos
    // (el motivo queda en getUltimoError). Titular y distrito son opcionales.
    int registrarUsuario(std::string nombre, std::string apellido, std::string titular,
        std::string ubicacion, std::string correo, std::string contrasena) {
        if (!validarCampo(nombre, "nombre", MAX_NOMBRE, true)) return -1;
        if (!validarCampo(apellido, "apellido", MAX_APELLIDO, true)) return -1;
        if (!validarCampo(titular, "titular", MAX_TITULAR, false)) return -1;
        if (!validarCampo(ubicacion, "distrito", MAX_UBICACION, false)) return -1;
        if (!validarCredenciales(correo, contrasena)) return -1;

        int id = sigUsuario;
        sigUsuario++;
        usuarios.agregaFinal(Usuario(id, nombre, apellido, titular, ubicacion, correo, contrasena));
        return id;
    }

    // Busqueda lineal O(n) en la lista de usuarios.
    // Devuelve el id del usuario, o -1 si el correo o la contrasena no coinciden.
    // El mensaje es el mismo en los dos casos para no revelar que correos existen.
    int iniciarSesionUsuario(std::string correo, std::string contrasena) {
        if (correo == "" || contrasena == "") { fallar("Ingrese su correo y su contrasena"); return -1; }
        const Usuario* u = usuarios.buscarPtr([&correo](const Usuario& x) { return x.tieneCorreo(correo); });
        if (u == nullptr || !u->contrasenaCorrecta(contrasena)) {
            fallar("Correo o contrasena incorrectos");
            return -1;
        }
        return u->getId();
    }

    Usuario* buscarUsuario(int idUsuario) {
        return usuarios.buscarPtr([idUsuario](const Usuario& u) { return u.getId() == idUsuario; });
    }

    const Usuario* buscarUsuario(int idUsuario) const {
        return usuarios.buscarPtr([idUsuario](const Usuario& u) { return u.getId() == idUsuario; });
    }

    bool existeUsuario(int idUsuario) const { return buscarUsuario(idUsuario) != nullptr; }

    // Para leer los datos de una empresa sin poder modificarla (ej. encabezado del menu).
    const Empresa* obtenerEmpresa(int idEmpresa) const {
        return empresas.buscarPtr([idEmpresa](const Empresa& e) { return e.getId() == idEmpresa; });
    }

    std::string nombreDe(int idUsuario) const {
        const Usuario* u = buscarUsuario(idUsuario);
        if (u == nullptr) return "(usuario desconocido)";
        return u->getNombreCompleto();
    }

    uint totalUsuarios() const { return usuarios.longitud(); }

    void paraCadaUsuario(std::function<void(const Usuario&)> accion) const {
        usuarios.paraCada(accion);
    }

    // ==================== Red y conexiones (Joao) ====================

    bool enviarSolicitud(int idEmisor, int idReceptor, std::string mensaje) {
        Usuario* emisor = buscarUsuario(idEmisor);
        Usuario* receptor = buscarUsuario(idReceptor);
        if (emisor == nullptr || receptor == nullptr) return fallar("El usuario no existe");
        if (idEmisor == idReceptor) return fallar("No puedes enviarte una solicitud a ti mismo");
        if (emisor->esContactoDirecto(idReceptor)) return fallar("Ya son contactos");
        if (receptor->tieneSolicitudDe(idEmisor)) return fallar("Ya enviaste una solicitud a este usuario");
        if (emisor->tieneSolicitudDe(idReceptor)) return fallar("Este usuario ya te envio una solicitud, revisa tus pendientes");

        receptor->recibirSolicitud(SolicitudConexion(sigSolicitud, idEmisor, idReceptor, mensaje, fechaHoy()));
        sigSolicitud++;
        notificar(idReceptor, TipoNotificacion::NuevaSolicitud,
            emisor->getNombreCompleto() + " quiere conectar contigo");
        return true;
    }

    // Atiende la solicitud mas antigua. Si se acepta, la conexion se guarda
    // como una accion para poder deshacerla despues.
    bool responderSiguienteSolicitud(int idReceptor, bool aceptar) {
        Usuario* receptor = buscarUsuario(idReceptor);
        if (receptor == nullptr) return fallar("El usuario no existe");
        if (!receptor->tieneSolicitudesPendientes()) return fallar("No tienes solicitudes pendientes");

        SolicitudConexion solicitud = receptor->atenderSiguienteSolicitud();
        int idEmisor = solicitud.getIdEmisor();
        if (!existeUsuario(idEmisor)) return fallar("El emisor ya no existe, la solicitud se descarto");

        if (aceptar) {
            solicitud.aceptar();
            // Las lambdas capturan la red y los ids, nunca punteros a Usuario.
            Accion accion("Red", "Aceptar conexion con " + nombreDe(idEmisor),
                [this, idReceptor, idEmisor]() { conectar(idReceptor, idEmisor); },
                [this, idReceptor, idEmisor]() { desconectar(idReceptor, idEmisor); });
            receptor->registrarAccion(accion);
            notificar(idEmisor, TipoNotificacion::NuevaConexion,
                receptor->getNombreCompleto() + " acepto tu solicitud");
        }
        else {
            solicitud.rechazar();
        }
        return true;
    }

    bool eliminarContacto(int idUsuario, int idContacto) {
        Usuario* usuario = buscarUsuario(idUsuario);
        if (usuario == nullptr) return fallar("El usuario no existe");
        if (!usuario->esContactoDirecto(idContacto)) return fallar("Ese usuario no esta en tus contactos");

        Accion accion("Red", "Eliminar contacto " + nombreDe(idContacto),
            [this, idUsuario, idContacto]() { desconectar(idUsuario, idContacto); },
            [this, idUsuario, idContacto]() { conectar(idUsuario, idContacto); });
        usuario->registrarAccion(accion);
        return true;
    }

    // ---------- Perfil: habilidades y experiencia (Joao) ----------

    static std::string nombreNivel(int nivel) {
        if (nivel == 1) return "Basico";
        if (nivel == 2) return "Intermedio";
        if (nivel == 3) return "Avanzado";
        return "Experto";
    }

    // nivel: 1 Basico, 2 Intermedio, 3 Avanzado, 4 Experto.
    // Se registra como Accion en la pila para poder deshacerla.
    bool agregarHabilidad(int idUsuario, std::string nombre, int nivel) {
        Usuario* usuario = buscarUsuario(idUsuario);
        if (usuario == nullptr) return fallar("El usuario no existe");
        if (!validarCampo(nombre, "habilidad", MAX_HABILIDAD, true)) return false;
        if (nivel < 1 || nivel > 4) return fallar("El nivel debe ser 1, 2, 3 o 4");
        if (usuario->buscarHabilidad(nombre) != nullptr) return fallar("Ya tienes esa habilidad");

        Habilidad h(nombre, (NivelHabilidad)(nivel - 1));
        Accion accion("Perfil", "Agregar habilidad " + nombre,
            [this, idUsuario, h]() { buscarUsuario(idUsuario)->agregarHabilidad(h); },
            [this, idUsuario, nombre]() { buscarUsuario(idUsuario)->eliminarHabilidad(nombre); });
        usuario->registrarAccion(accion);
        return true;
    }

    bool eliminarHabilidad(int idUsuario, std::string nombre) {
        Usuario* usuario = buscarUsuario(idUsuario);
        if (usuario == nullptr) return fallar("El usuario no existe");
        const Habilidad* guardada = usuario->buscarHabilidad(nombre);
        if (guardada == nullptr) return fallar("No tienes esa habilidad");

        Habilidad h = *guardada;   // copia para poder restaurarla con el mismo nivel
        Accion accion("Perfil", "Quitar habilidad " + nombre,
            [this, idUsuario, nombre]() { buscarUsuario(idUsuario)->eliminarHabilidad(nombre); },
            [this, idUsuario, h]() { buscarUsuario(idUsuario)->agregarHabilidad(h); });
        usuario->registrarAccion(accion);
        return true;
    }

    // Los anios llegan como texto desde el formulario. anioFin vacio = trabajo actual.
    // Si la empresa esta registrada en la red se guarda su id; si no, solo el nombre.
    bool agregarExperiencia(int idUsuario, std::string empresa, std::string cargo,
        std::string anioInicio, std::string anioFin) {
        Usuario* usuario = buscarUsuario(idUsuario);
        if (usuario == nullptr) return fallar("El usuario no existe");
        if (!validarCampo(empresa, "empresa", MAX_NOMBRE_EMPRESA, true)) return false;
        if (!validarCampo(cargo, "cargo", MAX_CARGO, true)) return false;

        int anioActual = std::stoi(fechaHoy().substr(0, 4));
        int inicio = aAnio(anioInicio);
        int fin = 0;
        if (inicio < 1950 || inicio > anioActual) return fallar("Anio de inicio invalido");
        if (anioFin != "") {
            fin = aAnio(anioFin);
            if (fin < inicio || fin > anioActual) return fallar("Anio de fin invalido");
        }

        int idEmpresa = 0;
        const Empresa* registrada = empresas.buscarPtr([&empresa](const Empresa& e) {
            return textoMinusculas(e.getNombre()) == textoMinusculas(empresa);
            });
        if (registrada != nullptr) idEmpresa = registrada->getId();

        usuario->agregarExperiencia(ExperienciaLaboral(idEmpresa, empresa, cargo, inicio, fin, ""));
        return true;
    }

    // pos cuenta desde el puesto mas antiguo (0).
    bool eliminarExperiencia(int idUsuario, int pos) {
        Usuario* usuario = buscarUsuario(idUsuario);
        if (usuario == nullptr) return fallar("El usuario no existe");
        if (pos < 0 || !usuario->eliminarExperiencia((uint)pos)) return fallar("Ese puesto no existe");
        return true;
    }

    bool deshacerUltimaAccion(int idUsuario, std::string& descripcionDeshecha) {
        Usuario* usuario = buscarUsuario(idUsuario);
        if (usuario == nullptr) return fallar("El usuario no existe");
        if (!usuario->tieneAccionesRegistradas()) return fallar("No hay acciones para deshacer");
        descripcionDeshecha = usuario->deshacerUltimaAccion();
        return true;
    }

    // Revisa que la fecha tenga el formato aaaa/mm/dd, con mes y dia validos.
    // Ese formato es el que permite ordenar las fechas comparandolas como texto.
    static bool esFechaValida(const std::string& fecha) {
        if (fecha.length() != 10 || fecha[4] != '/' || fecha[7] != '/') return false;
        for (int i = 0; i < 10; i++) {
            if (i == 4 || i == 7) continue;
            if (fecha[i] < '0' || fecha[i] > '9') return false;
        }
        int mes = std::stoi(fecha.substr(5, 2));
        int dia = std::stoi(fecha.substr(8, 2));
        return mes >= 1 && mes <= 12 && dia >= 1 && dia <= 31;
    }

    // Nombre, institucion y fecha son obligatorios; el codigo es opcional.
    bool agregarCertificacion(int idUsuario, std::string nombre, std::string institucion,
        std::string fechaObtencion, std::string codigoCredencial) {
        Usuario* usuario = buscarUsuario(idUsuario);
        if (usuario == nullptr) return fallar("El usuario no existe");
        if (nombre == "") return fallar("El nombre de la certificacion es obligatorio");
        if (institucion == "") return fallar("La institucion es obligatoria");
        if (!esFechaValida(fechaObtencion)) return fallar("La fecha debe tener el formato aaaa/mm/dd");
        if (fechaObtencion > fechaHoy()) return fallar("La fecha no puede ser futura");

        usuario->agregarCertificacion(Certificacion(sigCertificacion, nombre, institucion,
            fechaObtencion, codigoCredencial));
        sigCertificacion++;
        return true;
    }

    // Certificaciones del usuario en orden cronologico (la mas antigua primero),
    // ordenadas con MergeSort. Devuelve una COPIA ordenada: la lista del usuario
    // y el archivo no cambian de orden.
    // MergeSort es estable: si dos tienen la misma fecha, queda primero la que se agrego antes.
    Lista<Certificacion> certificacionesOrdenadas(int idUsuario) const {
        Lista<Certificacion> copia;
        const Usuario* usuario = buscarUsuario(idUsuario);
        if (usuario == nullptr) return copia;
        usuario->paraCadaCertificacion([&copia](const Certificacion& c) { copia.agregaFinal(c); });
        return mergeSort<Certificacion>(copia, [](const Certificacion& a, const Certificacion& b) {
            return a.getFechaObtencion() < b.getFechaObtencion();
            });
    }

    // Contactos directos ordenados alfabeticamente con QuickSort.
    // Se devuelve una copia de ids para no alterar el orden de la lista de contactos.
    Lista<int> contactosOrdenadosPorNombre(int idUsuario) const {
        Lista<int> copia;
        const Usuario* usuario = buscarUsuario(idUsuario);
        if (usuario == nullptr) return copia;

        usuario->paraCadaContacto([&copia](const int& idContacto) {
            copia.agregaFinal(idContacto);
            });

        return quickSort<int>(copia, [this](const int& a, const int& b) {
            std::string nombreA = textoMinusculas(nombreDe(a));
            std::string nombreB = textoMinusculas(nombreDe(b));
            if (nombreA == nombreB) return a < b;
            return nombreA < nombreB;
            });
    }

    // Busca por nombre completo o titular y ordena los resultados con QuickSort.
    // No incluye al propio usuario.
    Lista<int> buscarPersonas(int idUsuario, std::string texto) const {
        Lista<int> encontrados;
        std::string consulta = textoMinusculas(texto);
        if (consulta == "") return encontrados;

        usuarios.paraCada([&](const Usuario& u) {
            if (u.getId() == idUsuario) return;
            std::string nombre = textoMinusculas(u.getNombreCompleto());
            std::string titular = textoMinusculas(u.getTitular());
            if (nombre.find(consulta) != std::string::npos ||
                titular.find(consulta) != std::string::npos)
                encontrados.agregaFinal(u.getId());
            });

        return quickSort<int>(encontrados, [this](const int& a, const int& b) {
            std::string nombreA = textoMinusculas(nombreDe(a));
            std::string nombreB = textoMinusculas(nombreDe(b));
            if (nombreA == nombreB) return a < b;
            return nombreA < nombreB;
            });
    }

    // Obtiene personas de hasta segundo grado mediante recorrido recursivo.
    // Luego calcula contactos en comun y usa QuickSort para dejar primero
    // las sugerencias mas cercanas. En empate, ordena alfabeticamente.
    Lista<SugerenciaConexion> sugerenciasConexion(int idUsuario) const {
        Lista<SugerenciaConexion> sugerencias;
        const Usuario* usuario = buscarUsuario(idUsuario);
        if (usuario == nullptr) return sugerencias;

        Lista<int> visitados;
        Lista<int> descubiertos;
        visitados.agregaFinal(idUsuario);
        explorarConexionesRec(idUsuario, 2, visitados, descubiertos);

        descubiertos.paraCada([&](const int& idCandidato) {
            if (idCandidato == idUsuario) return;
            if (usuario->esContactoDirecto(idCandidato)) return;
            if (haySolicitudPendienteEntre(idUsuario, idCandidato)) return;

            int comunes = contactosEnComun(idUsuario, idCandidato);
            if (comunes <= 0) return;

            SugerenciaConexion s;
            s.idUsuario = idCandidato;
            s.contactosEnComun = comunes;
            sugerencias.agregaFinal(s);
            });

        return quickSort<SugerenciaConexion>(sugerencias, [this](const SugerenciaConexion& a, const SugerenciaConexion& b) {
            if (a.contactosEnComun != b.contactosEnComun)
                return a.contactosEnComun > b.contactosEnComun;
            std::string nombreA = textoMinusculas(nombreDe(a.idUsuario));
            std::string nombreB = textoMinusculas(nombreDe(b.idUsuario));
            if (nombreA == nombreB) return a.idUsuario < b.idUsuario;
            return nombreA < nombreB;
            });
    }

    bool sonContactos(int idA, int idB) const {
        const Usuario* a = buscarUsuario(idA);
        return a != nullptr && a->esContactoDirecto(idB);
    }

    bool existeSolicitudPendienteEntre(int idA, int idB) const {
        return haySolicitudPendienteEntre(idA, idB);
    }

    // ==================== Contenido y comunicacion (Aldo) ====================

    int publicar(int idAutor, std::string texto) {
        if (!existeUsuario(idAutor)) { fallar("El usuario no existe"); return -1; }
        if (texto == "") { fallar("La publicacion no puede estar vacia"); return -1; }
        if ((int)texto.length() > MAX_PUBLICACION) { fallar("La publicacion es demasiado larga"); return -1; }
        int id = sigPublicacion;
        sigPublicacion++;
        publicaciones.agregaFinal(Publicacion(id, idAutor, texto, fechaHoy()));
        return id;
    }

    // Si idComentarioPadre es distinto de 0, el comentario es una respuesta.
    int comentar(int idAutor, int idPublicacion, std::string texto, int idComentarioPadre = 0) {
        if (!existeUsuario(idAutor)) { fallar("El usuario no existe"); return -1; }
        if (texto == "") { fallar("El comentario no puede estar vacio"); return -1; }
        if ((int)texto.length() > MAX_COMENTARIO) { fallar("El comentario es demasiado largo"); return -1; }
        Publicacion* publicacion = buscarPublicacion(idPublicacion);
        if (publicacion == nullptr) { fallar("La publicacion no existe"); return -1; }
        int idAutorPadre = 0;
        if (idComentarioPadre != 0) {
            Comentario* padre = buscarComentario(idComentarioPadre);
            if (padre == nullptr || !padre->esDePublicacion(idPublicacion)) {
                fallar("El comentario al que respondes no pertenece a esta publicacion");
                return -1;
            }
            idAutorPadre = padre->getIdAutor();
        }
        int id = sigComentario;
        sigComentario++;
        comentarios.agregaFinal(Comentario(id, idAutor, idPublicacion, texto, fechaHoy(), idComentarioPadre));
        if (publicacion->getIdAutor() != idAutor)
            notificar(publicacion->getIdAutor(), TipoNotificacion::NuevoComentario,
                nombreDe(idAutor) + " comento tu publicacion");
        // Aviso al autor del comentario respondido (sin repetir avisos).
        if (idAutorPadre != 0 && idAutorPadre != idAutor && idAutorPadre != publicacion->getIdAutor())
            notificar(idAutorPadre, TipoNotificacion::NuevoComentario,
                nombreDe(idAutor) + " respondio tu comentario");
        return id;
    }

    bool darMeGusta(int idUsuario, int idPublicacion) {
        if (!existeUsuario(idUsuario)) return fallar("El usuario no existe");
        Publicacion* publicacion = buscarPublicacion(idPublicacion);
        if (publicacion == nullptr) return fallar("La publicacion no existe");
        if (!publicacion->darMeGusta(idUsuario)) return fallar("Ya diste me gusta a esta publicacion");
        if (publicacion->getIdAutor() != idUsuario)
            notificar(publicacion->getIdAutor(), TipoNotificacion::MeGusta,
                nombreDe(idUsuario) + " dio me gusta a tu publicacion");
        return true;
    }

    bool quitarMeGusta(int idUsuario, int idPublicacion) {
        Publicacion* publicacion = buscarPublicacion(idPublicacion);
        if (publicacion == nullptr) return fallar("La publicacion no existe");
        if (!publicacion->quitarMeGusta(idUsuario)) return fallar("No habias dado me gusta");
        return true;
    }

    // Para leer una publicacion sin poder modificarla (pantalla de detalle).
    const Publicacion* obtenerPublicacion(int idPublicacion) const {
        return publicaciones.buscarPtr([idPublicacion](const Publicacion& p) { return p.getId() == idPublicacion; });
    }

    // Cantidad de comentarios (incluidas las respuestas) de una publicacion. O(n)
    int contarComentarios(int idPublicacion) const {
        int total = 0;
        comentarios.paraCada([idPublicacion, &total](const Comentario& c) {
            if (c.esDePublicacion(idPublicacion)) total++;
            });
        return total;
    }

    // Feed con MergeSort: la publicacion mas reciente primero.
    // A igual fecha, la de id mayor (se publico despues).
    // Devuelve una COPIA ordenada: el catalogo y el archivo no cambian de orden.
    Lista<Publicacion> feedPorFecha() const {
        return mergeSort<Publicacion>(publicaciones, [](const Publicacion& a, const Publicacion& b) {
            if (a.getFecha() != b.getFecha()) return a.getFecha() > b.getFecha();
            return a.getId() > b.getId();
            });
    }

    // Feed por cantidad de me gusta. Se ordena el feed por fecha y, como
    // MergeSort es estable, a igual cantidad de me gusta se mantiene la mas reciente primero.
    Lista<Publicacion> feedPorPopularidad() const {
        return mergeSort<Publicacion>(feedPorFecha(), [](const Publicacion& a, const Publicacion& b) {
            return a.getMeGusta() > b.getMeGusta();
            });
    }

    // Recorre los comentarios de una publicacion en forma de hilo:
    // cada comentario seguido de sus respuestas. La accion recibe tambien el
    // nivel (0 = comentario directo, 1 = respuesta, 2 = respuesta a una respuesta...).
    void paraCadaComentarioEnHilo(int idPublicacion,
        std::function<void(const Comentario&, int)> accion) const {
        recorrerRespuestas(idPublicacion, 0, 0, accion);
    }

    int recomendar(int idEmisor, int idReceptor, std::string texto) {
        const Usuario* emisor = buscarUsuario(idEmisor);
        if (emisor == nullptr || !existeUsuario(idReceptor)) { fallar("El usuario no existe"); return -1; }
        if (!emisor->esContactoDirecto(idReceptor)) { fallar("Solo puedes recomendar a tus contactos"); return -1; }
        if (texto == "") { fallar("La recomendacion no puede estar vacia"); return -1; }
        if ((int)texto.length() > MAX_RECOMENDACION) { fallar("La recomendacion es demasiado larga"); return -1; }
        int id = sigRecomendacion;
        sigRecomendacion++;
        recomendaciones.agregaFinal(Recomendacion(id, idEmisor, idReceptor, texto, fechaHoy()));
        notificar(idReceptor, TipoNotificacion::NuevaRecomendacion,
            nombreDe(idEmisor) + " escribio una recomendacion sobre ti");
        return id;
    }

    int enviarMensaje(int idEmisor, int idReceptor, std::string texto) {
        const Usuario* emisor = buscarUsuario(idEmisor);
        if (emisor == nullptr || !existeUsuario(idReceptor)) { fallar("El usuario no existe"); return -1; }
        if (!emisor->esContactoDirecto(idReceptor)) { fallar("Solo puedes escribir a tus contactos"); return -1; }
        if (texto == "") { fallar("El mensaje no puede estar vacio"); return -1; }
        if ((int)texto.length() > MAX_MENSAJE) { fallar("El mensaje es demasiado largo"); return -1; }
        int id = sigMensaje;
        sigMensaje++;
        mensajes.agregaFinal(Mensaje(id, idEmisor, idReceptor, texto, fechaHoy()));
        notificar(idReceptor, TipoNotificacion::NuevoMensaje, "Nuevo mensaje de " + nombreDe(idEmisor));
        return id;
    }

    // Marca como leidos los mensajes que el otro usuario le envio al lector.
    // Se usa el iterador porque hay que modificar los mensajes guardados.
    uint marcarConversacionLeida(int idLector, int idOtro) {
        uint marcados = 0;
        for (Mensaje& m : mensajes) {
            if (m.getIdEmisor() == idOtro && m.getIdReceptor() == idLector && !m.fueLeido()) {
                m.marcarLeido();
                marcados++;
            }
        }
        return marcados;
    }

    // Devuelve cuantos mensajes recibidos por el usuario siguen sin leer.
    // No se cuentan los mensajes que el propio usuario envio.
    uint cantidadMensajesNoLeidos(int idUsuario) const {
        const Usuario* usuario = buscarUsuario(idUsuario);
        if (usuario == nullptr) return 0;

        uint total = 0;
        mensajes.paraCada([idUsuario, usuario, &total](const Mensaje& m) {
            if (m.getIdReceptor() == idUsuario && !m.fueLeido()
                && usuario->esContactoDirecto(m.getIdEmisor())) total++;
            });
        return total;
    }

    // Mensajes no leidos que llegaron especificamente desde un contacto.
    uint cantidadMensajesNoLeidosDe(int idUsuario, int idContacto) const {
        uint total = 0;
        mensajes.paraCada([idUsuario, idContacto, &total](const Mensaje& m) {
            if (m.getIdEmisor() == idContacto && m.getIdReceptor() == idUsuario && !m.fueLeido()) total++;
            });
        return total;
    }

    uint cantidadMensajesEntre(int idA, int idB) const {
        uint total = 0;
        mensajes.paraCada([idA, idB, &total](const Mensaje& m) {
            if (m.esConversacionEntre(idA, idB)) total++;
            });
        return total;
    }

    // Copia solo los mensajes de una conversacion. La lista conserva el orden
    // de insercion, que coincide con el orden en que fueron enviados.
    Lista<Mensaje> obtenerConversacion(int idA, int idB) const {
        Lista<Mensaje> resultado;
        mensajes.paraCada([idA, idB, &resultado](const Mensaje& m) {
            if (m.esConversacionEntre(idA, idB)) resultado.agregaFinal(m);
            });
        return resultado;
    }

    void paraCadaPublicacion(std::function<void(const Publicacion&)> accion) const {
        publicaciones.paraCada(accion);
    }

    void paraCadaComentarioDe(int idPublicacion, std::function<void(const Comentario&)> accion) const {
        comentarios.paraCada([idPublicacion, &accion](const Comentario& c) {
            if (c.esDePublicacion(idPublicacion)) accion(c);
            });
    }

    void paraCadaMensajeEntre(int idA, int idB, std::function<void(const Mensaje&)> accion) const {
        mensajes.paraCada([idA, idB, &accion](const Mensaje& m) {
            if (m.esConversacionEntre(idA, idB)) accion(m);
            });
    }

    void paraCadaRecomendacionPara(int idReceptor, std::function<void(const Recomendacion&)> accion) const {
        recomendaciones.paraCada([idReceptor, &accion](const Recomendacion& r) {
            if (r.esPara(idReceptor)) accion(r);
            });
    }

    void paraCadaRecomendacionDe(int idEmisor, std::function<void(const Recomendacion&)> accion) const {
        recomendaciones.paraCada([idEmisor, &accion](const Recomendacion& r) {
            if (r.getIdEmisor() == idEmisor) accion(r);
            });
    }

    const Recomendacion* obtenerRecomendacion(int idRecomendacion) const {
        return recomendaciones.buscarPtr([idRecomendacion](const Recomendacion& r) {
            return r.getId() == idRecomendacion;
            });
    }

    // Copias ordenadas de forma descendente por fecha e id para que las mas
    // recientes aparezcan primero sin alterar el orden del archivo original.
    Lista<Recomendacion> recomendacionesRecibidas(int idReceptor) const {
        Lista<Recomendacion> resultado;
        paraCadaRecomendacionPara(idReceptor, [&resultado](const Recomendacion& r) {
            resultado.agregaFinal(r);
            });
        return mergeSort<Recomendacion>(resultado, [](const Recomendacion& a, const Recomendacion& b) {
            if (a.getFecha() != b.getFecha()) return a.getFecha() > b.getFecha();
            return a.getId() > b.getId();
            });
    }

    Lista<Recomendacion> recomendacionesEnviadas(int idEmisor) const {
        Lista<Recomendacion> resultado;
        paraCadaRecomendacionDe(idEmisor, [&resultado](const Recomendacion& r) {
            resultado.agregaFinal(r);
            });
        return mergeSort<Recomendacion>(resultado, [](const Recomendacion& a, const Recomendacion& b) {
            if (a.getFecha() != b.getFecha()) return a.getFecha() > b.getFecha();
            return a.getId() > b.getId();
            });
    }

    // Junta en una sola lista las publicaciones, comentarios y recomendaciones
    // que escribio el usuario. Como se guardan punteros a la clase base, cada
    // elemento responde segun su clase real: esto es el polimorfismo.
    // Los mensajes no entran porque son privados.
    Lista<const Contenido*> actividadDe(int idUsuario) const {
        Lista<const Contenido*> actividad;
        publicaciones.paraCada([&actividad, idUsuario](const Publicacion& p) {
            if (p.esDeAutor(idUsuario)) actividad.agregaFinal(&p);
            });
        comentarios.paraCada([&actividad, idUsuario](const Comentario& c) {
            if (c.esDeAutor(idUsuario)) actividad.agregaFinal(&c);
            });
        recomendaciones.paraCada([&actividad, idUsuario](const Recomendacion& r) {
            if (r.esDeAutor(idUsuario)) actividad.agregaFinal(&r);
            });
        return actividad;
    }

    // Los hilos de comentarios se recorren de forma recursiva usando idPadre,
    // y las publicaciones se ordenan con MergeSort en los metodos anteriores.

    // ==================== Empleo y grupos (Piero) ====================

    // Sector y distrito son opcionales.
    int registrarEmpresa(std::string nombre, std::string sector, std::string ubicacion,
        std::string correo, std::string contrasena) {
        if (!validarCampo(nombre, "nombre", MAX_NOMBRE_EMPRESA, true)) return -1;
        bool repetida = empresas.existe([nombre](const Empresa& e) { return e.getNombre() == nombre; });
        if (repetida) { fallar("Ya existe una empresa con ese nombre"); return -1; }
        if (!validarCampo(sector, "sector", MAX_SECTOR, false)) return -1;
        if (!validarCampo(ubicacion, "distrito", MAX_UBICACION, false)) return -1;
        if (!validarCredenciales(correo, contrasena)) return -1;

        int id = sigEmpresa;
        sigEmpresa++;
        empresas.agregaFinal(Empresa(id, nombre, sector, ubicacion, correo, contrasena));
        return id;
    }

    // Igual que iniciarSesionUsuario, pero busca en la lista de empresas.
    int iniciarSesionEmpresa(std::string correo, std::string contrasena) {
        if (correo == "" || contrasena == "") { fallar("Ingrese su correo y su contrasena"); return -1; }
        const Empresa* e = empresas.buscarPtr([&correo](const Empresa& x) { return x.tieneCorreo(correo); });
        if (e == nullptr || !e->contrasenaCorrecta(contrasena)) {
            fallar("Correo o contrasena incorrectos");
            return -1;
        }
        return e->getId();
    }

    int publicarVacante(int idEmpresa, std::string titulo, std::string descripcion,
        std::string requisitos, std::string modalidad) {
        if (buscarEmpresa(idEmpresa) == nullptr) { fallar("La empresa no existe"); return -1; }
        if (titulo == "") { fallar("El titulo es obligatorio"); return -1; }
        int id = sigVacante;
        sigVacante++;
        vacantes.agregaFinal(Vacante(id, idEmpresa, titulo, descripcion, requisitos, modalidad));
        return id;
    }

    int postular(int idUsuario, int idVacante) {
        if (!existeUsuario(idUsuario)) { fallar("El usuario no existe"); return -1; }
        Vacante* vacante = buscarVacante(idVacante);
        if (vacante == nullptr) { fallar("La vacante no existe"); return -1; }
        if (!vacante->estaActiva()) { fallar("La vacante esta cerrada"); return -1; }
        bool yaPostulo = postulaciones.existe([idUsuario, idVacante](const Postulacion& p) {
            return p.esDeUsuario(idUsuario) && p.esDeVacante(idVacante);
            });
        if (yaPostulo) { fallar("Ya postulaste a esta vacante"); return -1; }

        int id = sigPostulacion;
        sigPostulacion++;
        postulaciones.agregaFinal(Postulacion(id, idUsuario, idVacante, fechaHoy()));
        vacante->recibirPostulacion(id);
        return id;
    }

    // Revisa la postulacion mas antigua de la vacante (la primera de la cola) y avisa al postulante.
    bool revisarSiguientePostulacion(int idEmpresa, int idVacante, bool aceptar) {
        Vacante* vacante = buscarVacante(idVacante);
        if (vacante == nullptr || !vacante->esDeEmpresa(idEmpresa)) return fallar("La vacante no es de esta empresa");
        if (!vacante->tienePostulacionesPorRevisar()) return fallar("No hay postulaciones por revisar");

        int idPostulacion = vacante->siguientePostulacionPorRevisar();
        Postulacion* postulacion = buscarPostulacion(idPostulacion);
        if (postulacion == nullptr) return fallar("La postulacion ya no existe");

        if (aceptar) postulacion->aceptar();
        else postulacion->rechazar();
        notificar(postulacion->getIdUsuario(), TipoNotificacion::EstadoPostulacion,
            "Tu postulacion a " + vacante->getTitulo() + " de " + nombreEmpresa(idEmpresa)
            + " fue " + postulacion->estadoToString());
        return true;
    }

    // Primera postulacion en la cola de la vacante, sin sacarla (frente de la cola).
    // Devuelve nullptr si no hay postulantes o si la vacante no es de la empresa.
    const Postulacion* verSiguientePostulacion(int idEmpresa, int idVacante) const {
        const Vacante* vacante = obtenerVacante(idVacante);
        if (vacante == nullptr || !vacante->esDeEmpresa(idEmpresa)) return nullptr;
        if (!vacante->tienePostulacionesPorRevisar()) return nullptr;
        int idPostulacion = vacante->verSiguientePostulacion();
        return postulaciones.buscarPtr([idPostulacion](const Postulacion& p) { return p.getId() == idPostulacion; });
    }

    // Cierra la vacante por completo: los que seguian en la cola pasan a Rechazada
    // y se avisa a ellos y a los aceptados.
    bool cancelarVacante(int idEmpresa, int idVacante) {
        Vacante* vacante = buscarVacante(idVacante);
        if (vacante == nullptr || !vacante->esDeEmpresa(idEmpresa)) return fallar("La vacante no es de esta empresa");
        if (!vacante->estaActiva()) return fallar("La vacante ya estaba cancelada");

        vacante->cerrar();
        std::string puesto = vacante->getTitulo() + " de " + nombreEmpresa(idEmpresa);

        // Se vacia la cola: cada postulante pendiente queda rechazado.
        while (vacante->tienePostulacionesPorRevisar()) {
            Postulacion* p = buscarPostulacion(vacante->siguientePostulacionPorRevisar());
            if (p == nullptr) continue;
            p->rechazar();
            notificar(p->getIdUsuario(), TipoNotificacion::EstadoPostulacion,
                "Gracias por tu interes en " + puesto + " (" + vacante->getModalidad()
                + "). La vacante fue cerrada y tu postulacion no continuara en el proceso.");
        }

        // A los aceptados se les avisa que su contratacion se mantiene.
        Lista<int> aceptados;
        postulaciones.paraCada([idVacante, &aceptados](const Postulacion& p) {
            if (p.esDeVacante(idVacante) && p.getEstado() == EstadoPostulacion::Aceptada)
                aceptados.agregaFinal(p.getIdUsuario());
            });
        aceptados.paraCada([this, &puesto](const int& idUsuario) {
            notificar(idUsuario, TipoNotificacion::EstadoPostulacion,
                "La vacante " + puesto + " fue cerrada. Tu contratacion se mantiene.");
            });
        return true;
    }

    bool cerrarVacante(int idVacante) {
        Vacante* vacante = buscarVacante(idVacante);
        if (vacante == nullptr) return fallar("La vacante no existe");
        if (!vacante->estaActiva()) return fallar("La vacante ya estaba cerrada");
        vacante->cerrar();
        return true;
    }
    bool rotarPostulacionesVacante(int idVacante)
    {
        Vacante* v = buscarVacante(idVacante);
        if (v == nullptr) return fallar("La vacante no existe");
        if (!v->tienePostulacionesPorRevisar()) return fallar("No hay postulaciones pendientes");
        v->rotarPostulaciones();
        return true;
    }
    int crearGrupo(std::string nombre, std::string descripcion, std::string especialidad) {
        if (nombre == "") { fallar("El nombre del grupo es obligatorio"); return -1; }
        int id = sigGrupo;
        sigGrupo++;
        grupos.agregaFinal(GrupoProfesional(id, nombre, descripcion, especialidad));
        return id;
    }

    bool unirseAGrupo(int idUsuario, int idGrupo) {
        if (!existeUsuario(idUsuario)) return fallar("El usuario no existe");
        GrupoProfesional* grupo = buscarGrupo(idGrupo);
        if (grupo == nullptr) return fallar("El grupo no existe");
        if (!grupo->agregarMiembro(idUsuario)) return fallar("Ya eres miembro de este grupo");
        return true;
    }

    bool salirDeGrupo(int idUsuario, int idGrupo) {
        GrupoProfesional* grupo = buscarGrupo(idGrupo);
        if (grupo == nullptr) return fallar("El grupo no existe");
        if (!grupo->eliminarMiembro(idUsuario)) return fallar("No eres miembro de este grupo");
        return true;
    }

    std::string nombreEmpresa(int idEmpresa) const {
        const Empresa* empresa = empresas.buscarPtr([idEmpresa](const Empresa& e) {
            return e.getId() == idEmpresa;
            });
        if (empresa == nullptr) return "(empresa desconocida)";
        return empresa->getNombre();
    }

    void paraCadaEmpresa(std::function<void(const Empresa&)> accion) const {
        empresas.paraCada(accion);
    }

    void paraCadaVacanteActiva(std::function<void(const Vacante&)> accion) const {
        vacantes.paraCada([&accion](const Vacante& v) {
            if (v.estaActiva()) accion(v);
            });
    }

    // Para leer una vacante sin poder modificarla.
    const Vacante* obtenerVacante(int idVacante) const {
        return vacantes.buscarPtr([idVacante](const Vacante& v) { return v.getId() == idVacante; });
    }

    // Todas las vacantes de una empresa (activas y canceladas), en el orden del catalogo.
    void paraCadaVacanteDe(int idEmpresa, std::function<void(const Vacante&)> accion) const {
        vacantes.paraCada([idEmpresa, &accion](const Vacante& v) {
            if (v.esDeEmpresa(idEmpresa)) accion(v);
            });
    }

    // Postulaciones aceptadas en las vacantes de la empresa.
    void paraCadaContratacion(int idEmpresa, std::function<void(const Postulacion&)> accion) const {
        postulaciones.paraCada([this, idEmpresa, &accion](const Postulacion& p) {
            if (p.getEstado() != EstadoPostulacion::Aceptada) return;
            const Vacante* v = obtenerVacante(p.getIdVacante());
            if (v != nullptr && v->esDeEmpresa(idEmpresa)) accion(p);
            });
    }

    // ---------- Buscar profesionales ----------

    // Si la palabra aparece dentro del texto, sin importar mayusculas.
    static bool contiene(const std::string& texto, const std::string& palabra) {
        return textoMinusculas(texto).find(textoMinusculas(palabra)) != std::string::npos;
    }

    // Busqueda lineal: por cada usuario se revisa si cada palabra clave aparece en
    // su titular o en alguna de sus habilidades. Entra si coincide al menos una.
    // Los resultados se ordenan con HeapSort: mas coincidencias primero.
    // Complejidad: O(u * k * h) la busqueda + O(r log r) el ordenamiento.
    // En totalPalabras deja cuantas palabras clave se buscaron.
    Lista<Coincidencia> buscarProfesionales(std::string palabrasClave, int& totalPalabras) {
        Lista<std::string> palabras = separarRequisitos(palabrasClave);
        totalPalabras = (int)palabras.longitud();

        Lista<Coincidencia> resultados;
        usuarios.paraCada([&palabras, &resultados](const Usuario& u) {
            int cantidad = 0;
            palabras.paraCada([&u, &cantidad](const std::string& palabra) {
                bool encontrada = contiene(u.getTitular(), palabra);
                u.paraCadaHabilidad([&palabra, &encontrada](const Habilidad& h) {
                    if (contiene(h.getNombre(), palabra)) encontrada = true;
                    });
                if (encontrada) cantidad++;
                });
            if (cantidad > 0) {
                Coincidencia c;
                c.idUsuario = u.getId();
                c.cantidad = cantidad;
                resultados.agregaFinal(c);
            }
            });

        std::function<bool(const Coincidencia&, const Coincidencia&)> criterio =
            [](const Coincidencia& a, const Coincidencia& b) { return a.cantidad > b.cantidad; };
        return heapSort(resultados, criterio);
    }
    // Vacantes de una empresa ordenadas por titulo con HeapSort.
    // Devuelve una copia: la lista principal de vacantes no cambia de orden.
    Lista<Vacante> vacantesOrdenadasDe(int idEmpresa) const {
        Lista<Vacante> copia;
        vacantes.paraCada([idEmpresa, &copia](const Vacante& v) {
            if (v.esDeEmpresa(idEmpresa)) copia.agregaFinal(v);
            });
        std::function<bool(const Vacante&, const Vacante&)> criterio =
            [](const Vacante& a, const Vacante& b) { return a.getTitulo() < b.getTitulo(); };
        return heapSort(copia, criterio);
    }

    void paraCadaPostulacionDe(int idUsuario, std::function<void(const Postulacion&)> accion) const {
        postulaciones.paraCada([idUsuario, &accion](const Postulacion& p) {
            if (p.esDeUsuario(idUsuario)) accion(p);
            });
    }

    void paraCadaGrupo(std::function<void(const GrupoProfesional&)> accion) const {
        grupos.paraCada(accion);
    }

    // Porcentaje de requisitos de la vacante que el usuario tiene como habilidad.
    int calcularCompatibilidad(int idUsuario, int idVacante) const {
        const Usuario* usuario = buscarUsuario(idUsuario);
        const Vacante* vacante = obtenerVacante(idVacante);
        if (usuario == nullptr || vacante == nullptr) return 0;

        Lista<std::string> requisitos = separarRequisitos(vacante->getRequisitos());
        int total = (int)requisitos.longitud();
        if (total == 0) return 0;

        int coincidencias = compararRequisitosRec(requisitos.begin(), requisitos.end(), *usuario);
        return (coincidencias * 100) / total;
    }
};