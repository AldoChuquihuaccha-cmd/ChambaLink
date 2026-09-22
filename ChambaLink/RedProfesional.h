#pragma once
#include <string>
#include <ctime>
#include <functional>
#include "Lista.h"
#include "Usuario.h"
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

    Comentario* buscarComentario(int idComentario) {
        return comentarios.buscarPtr([idComentario](const Comentario& c) { return c.getId() == idComentario; });
    }

public:
    RedProfesional() {
        sigUsuario = 1;
        sigEmpresa = 1;
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

    // Devuelve el id del nuevo usuario, o -1 si los datos no son validos.
    int registrarUsuario(std::string nombre, std::string apellido,
        std::string titular, std::string ubicacion) {
        if (nombre == "" || apellido == "") {
            fallar("El nombre y el apellido son obligatorios");
            return -1;
        }
        int id = sigUsuario;
        sigUsuario++;
        usuarios.agregaFinal(Usuario(id, nombre, apellido, titular, ubicacion));
        return id;
    }

    Usuario* buscarUsuario(int idUsuario) {
        return usuarios.buscarPtr([idUsuario](const Usuario& u) { return u.getId() == idUsuario; });
    }

    const Usuario* buscarUsuario(int idUsuario) const {
        return usuarios.buscarPtr([idUsuario](const Usuario& u) { return u.getId() == idUsuario; });
    }

    bool existeUsuario(int idUsuario) const { return buscarUsuario(idUsuario) != nullptr; }

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
            Accion accion("Aceptar conexion con " + nombreDe(idEmisor),
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

        Accion accion("Eliminar contacto " + nombreDe(idContacto),
            [this, idUsuario, idContacto]() { desconectar(idUsuario, idContacto); },
            [this, idUsuario, idContacto]() { conectar(idUsuario, idContacto); });
        usuario->registrarAccion(accion);
        return true;
    }

    bool deshacerUltimaAccion(int idUsuario, std::string& descripcionDeshecha) {
        Usuario* usuario = buscarUsuario(idUsuario);
        if (usuario == nullptr) return fallar("El usuario no existe");
        if (!usuario->tieneAccionesRegistradas()) return fallar("No hay acciones para deshacer");
        descripcionDeshecha = usuario->deshacerUltimaAccion();
        return true;
    }

    bool agregarCertificacion(int idUsuario, std::string nombre, std::string institucion,
        std::string fechaObtencion, std::string codigoCredencial) {
        Usuario* usuario = buscarUsuario(idUsuario);
        if (usuario == nullptr) return fallar("El usuario no existe");
        usuario->agregarCertificacion(Certificacion(sigCertificacion, nombre, institucion,
            fechaObtencion, codigoCredencial));
        sigCertificacion++;
        return true;
    }

    // Pendiente (Joao): busqueda recursiva de conexiones con control de visitados,
    // sugerencias por contactos en comun y ordenamiento con QuickSort.

    // ==================== Contenido y comunicacion (Aldo) ====================

    int publicar(int idAutor, std::string texto) {
        if (!existeUsuario(idAutor)) { fallar("El usuario no existe"); return -1; }
        if (texto == "") { fallar("La publicacion no puede estar vacia"); return -1; }
        int id = sigPublicacion;
        sigPublicacion++;
        publicaciones.agregaFinal(Publicacion(id, idAutor, texto, fechaHoy()));
        return id;
    }

    // Si idComentarioPadre es distinto de 0, el comentario es una respuesta.
    int comentar(int idAutor, int idPublicacion, std::string texto, int idComentarioPadre = 0) {
        if (!existeUsuario(idAutor)) { fallar("El usuario no existe"); return -1; }
        Publicacion* publicacion = buscarPublicacion(idPublicacion);
        if (publicacion == nullptr) { fallar("La publicacion no existe"); return -1; }
        if (idComentarioPadre != 0) {
            Comentario* padre = buscarComentario(idComentarioPadre);
            if (padre == nullptr || !padre->esDePublicacion(idPublicacion)) {
                fallar("El comentario al que respondes no pertenece a esta publicacion");
                return -1;
            }
        }
        int id = sigComentario;
        sigComentario++;
        comentarios.agregaFinal(Comentario(id, idAutor, idPublicacion, texto, fechaHoy(), idComentarioPadre));
        if (publicacion->getIdAutor() != idAutor)
            notificar(publicacion->getIdAutor(), TipoNotificacion::NuevoComentario,
                nombreDe(idAutor) + " comento tu publicacion");
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

    int recomendar(int idEmisor, int idReceptor, std::string texto) {
        const Usuario* emisor = buscarUsuario(idEmisor);
        if (emisor == nullptr || !existeUsuario(idReceptor)) { fallar("El usuario no existe"); return -1; }
        if (!emisor->esContactoDirecto(idReceptor)) { fallar("Solo puedes recomendar a tus contactos"); return -1; }
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

    // Pendiente (Aldo): mostrar los hilos de comentarios de forma recursiva
    // usando idPadre y ordenar las publicaciones con MergeSort.

    // ==================== Empleo y grupos (Piero) ====================

    int registrarEmpresa(std::string nombre, std::string sector, std::string ubicacion) {
        if (nombre == "") { fallar("El nombre de la empresa es obligatorio"); return -1; }
        bool repetida = empresas.existe([nombre](const Empresa& e) { return e.getNombre() == nombre; });
        if (repetida) { fallar("Ya existe una empresa con ese nombre"); return -1; }
        int id = sigEmpresa;
        sigEmpresa++;
        empresas.agregaFinal(Empresa(id, nombre, sector, ubicacion));
        return id;
    }

    int publicarVacante(int idEmpresa, std::string titulo, std::string descripcion,
        std::string requisitos, std::string modalidad) {
        if (buscarEmpresa(idEmpresa) == nullptr) { fallar("La empresa no existe"); return -1; }
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

    // Revisa la postulacion mas antigua de la vacante y avisa al postulante.
    bool revisarSiguientePostulacion(int idVacante, bool aceptar) {
        Vacante* vacante = buscarVacante(idVacante);
        if (vacante == nullptr) return fallar("La vacante no existe");
        if (!vacante->tienePostulacionesPorRevisar()) return fallar("No hay postulaciones por revisar");

        int idPostulacion = vacante->siguientePostulacionPorRevisar();
        Postulacion* postulacion = buscarPostulacion(idPostulacion);
        if (postulacion == nullptr) return fallar("La postulacion ya no existe");

        if (aceptar) postulacion->aceptar();
        else postulacion->rechazar();
        notificar(postulacion->getIdUsuario(), TipoNotificacion::EstadoPostulacion,
            "Tu postulacion a " + vacante->getTitulo() + " fue " + postulacion->estadoToString());
        return true;
    }

    bool cerrarVacante(int idVacante) {
        Vacante* vacante = buscarVacante(idVacante);
        if (vacante == nullptr) return fallar("La vacante no existe");
        if (!vacante->estaActiva()) return fallar("La vacante ya estaba cerrada");
        vacante->cerrar();
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

    void paraCadaPostulacionDe(int idUsuario, std::function<void(const Postulacion&)> accion) const {
        postulaciones.paraCada([idUsuario, &accion](const Postulacion& p) {
            if (p.esDeUsuario(idUsuario)) accion(p);
            });
    }

    void paraCadaGrupo(std::function<void(const GrupoProfesional&)> accion) const {
        grupos.paraCada(accion);
    }

    // Pendiente (Piero): comparar los requisitos de una vacante con las
    // habilidades del usuario de forma recursiva, ordenar con HeapSort y
    // guardar y cargar cada catalogo en archivos de texto.
};