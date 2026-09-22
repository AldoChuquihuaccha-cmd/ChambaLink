#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include "RedProfesional.h"

// Guarda y carga toda la informacion de ChambaLink en archivos de texto.
//
// Formato: un archivo por clase principal y un archivo por relacion, con los
// campos separados por el caracter '|'. Ejemplo de usuarios.txt:
//     1|Ana|Torres|Ingeniera de Software|Lima
// Los enum se guardan como numero y los bool como 0 o 1.
//
// Las listas que estan dentro de un objeto no caben en la misma linea, por eso
// las relaciones van en archivos aparte: contactos.txt, habilidades.txt, etc.
//
// La pila de acciones no se guarda: contiene funciones lambda, que no se pueden
// escribir en un archivo. Cada sesion empieza con el historial de deshacer vacio.
//
// Esta clase es amiga de RedProfesional para poder escribir los catalogos con
// sus ids originales al cargar, en lugar de generar ids nuevos.
class GestorArchivos {
private:
    // ---------- Apoyo para escribir ----------

    // Quita el separador del texto para que no rompa la linea.
    static std::string limpiar(std::string texto) {
        for (uint i = 0; i < texto.length(); i++)
            if (texto[i] == '|') texto[i] = ' ';
        return texto;
    }

    static std::string deEntero(int numero) { return std::to_string(numero); }

    static std::string deBool(bool valor) {
        if (valor) return "1";
        return "0";
    }

    // ---------- Apoyo para leer ----------

    // Separa una linea por '|' y devuelve los campos en una lista.
    static Lista<std::string> partir(std::string linea) {
        Lista<std::string> campos;
        std::stringstream flujo(linea);
        std::string campo;
        while (std::getline(flujo, campo, '|'))
            campos.agregaFinal(campo);
        return campos;
    }

    static int aEntero(std::string texto) {
        if (texto == "") return 0;
        return std::stoi(texto);
    }

    static bool aBool(std::string texto) { return texto == "1"; }

    // Deja el contador en el id mas alto encontrado mas uno.
    static void actualizarContador(int& contador, int idLeido) {
        if (idLeido >= contador) contador = idLeido + 1;
    }

public:
    // ============================================================
    //  Guardar
    // ============================================================
    static bool guardarTodo(const RedProfesional& red) {
        std::ofstream usuarios("usuarios.txt");
        std::ofstream contactos("contactos.txt");
        std::ofstream habilidades("habilidades.txt");
        std::ofstream experiencias("experiencias.txt");
        std::ofstream certificaciones("certificaciones.txt");
        std::ofstream solicitudes("solicitudes.txt");
        std::ofstream notificaciones("notificaciones.txt");

        if (!usuarios.is_open()) return false;

        red.usuarios.paraCada([&](const Usuario& u) {
            int idUsuario = u.getId();
            usuarios << idUsuario << "|" << limpiar(u.getNombre()) << "|"
                << limpiar(u.getApellido()) << "|" << limpiar(u.getTitular()) << "|"
                << limpiar(u.getUbicacion()) << "\n";

            u.paraCadaContacto([&](const int& idContacto) {
                contactos << idUsuario << "|" << idContacto << "\n";
                });

            u.paraCadaHabilidad([&](const Habilidad& h) {
                habilidades << idUsuario << "|" << limpiar(h.getNombre()) << "|"
                    << (int)h.getNivel() << "\n";
                });

            u.paraCadaExperiencia([&](const ExperienciaLaboral& e) {
                experiencias << idUsuario << "|" << e.getIdEmpresa() << "|"
                    << limpiar(e.getNombreEmpresa()) << "|" << limpiar(e.getCargo()) << "|"
                    << e.getAnioInicio() << "|" << e.getAnioFin() << "|"
                    << limpiar(e.getDescripcion()) << "\n";
                }, false);

            u.paraCadaCertificacion([&](const Certificacion& c) {
                certificaciones << idUsuario << "|" << c.getId() << "|"
                    << limpiar(c.getNombre()) << "|" << limpiar(c.getInstitucion()) << "|"
                    << limpiar(c.getFechaObtencion()) << "|"
                    << limpiar(c.getCodigoCredencial()) << "\n";
                });

            // Solo las pendientes, en el orden en que estan en la cola.
            u.paraCadaSolicitud([&](const SolicitudConexion& s) {
                solicitudes << s.getId() << "|" << s.getIdEmisor() << "|"
                    << s.getIdReceptor() << "|" << limpiar(s.getMensaje()) << "|"
                    << s.getFecha() << "\n";
                });

            u.paraCadaNotificacion([&](const Notificacion& n) {
                notificaciones << n.getId() << "|" << n.getIdDestino() << "|"
                    << (int)n.getTipo() << "|" << limpiar(n.getMensaje()) << "|"
                    << n.getFecha() << "|" << deBool(n.estaLeida()) << "\n";
                });
            });

        std::ofstream empresas("empresas.txt");
        red.empresas.paraCada([&](const Empresa& e) {
            empresas << e.getId() << "|" << limpiar(e.getNombre()) << "|"
                << limpiar(e.getSector()) << "|" << limpiar(e.getUbicacion()) << "\n";
            });

        std::ofstream vacantes("vacantes.txt");
        std::ofstream porRevisar("postulaciones_por_revisar.txt");
        red.vacantes.paraCada([&](const Vacante& v) {
            vacantes << v.getId() << "|" << v.getIdEmpresa() << "|" << limpiar(v.getTitulo()) << "|"
                << limpiar(v.getDescripcion()) << "|" << limpiar(v.getRequisitos()) << "|"
                << limpiar(v.getModalidad()) << "|" << deBool(v.estaActiva()) << "\n";

            v.paraCadaPostulacionPorRevisar([&](const int& idPostulacion) {
                porRevisar << v.getId() << "|" << idPostulacion << "\n";
                });
            });

        std::ofstream postulaciones("postulaciones.txt");
        red.postulaciones.paraCada([&](const Postulacion& p) {
            postulaciones << p.getId() << "|" << p.getIdUsuario() << "|" << p.getIdVacante() << "|"
                << p.getFecha() << "|" << (int)p.getEstado() << "\n";
            });

        std::ofstream grupos("grupos.txt");
        std::ofstream miembros("miembros.txt");
        red.grupos.paraCada([&](const GrupoProfesional& g) {
            grupos << g.getId() << "|" << limpiar(g.getNombre()) << "|"
                << limpiar(g.getDescripcion()) << "|" << limpiar(g.getEspecialidad()) << "\n";

            g.paraCadaMiembro([&](const int& idUsuario) {
                miembros << g.getId() << "|" << idUsuario << "\n";
                });
            });

        std::ofstream publicaciones("publicaciones.txt");
        std::ofstream meGusta("me_gusta.txt");
        red.publicaciones.paraCada([&](const Publicacion& p) {
            publicaciones << p.getId() << "|" << p.getIdAutor() << "|" << limpiar(p.getTexto()) << "|"
                << p.getFecha() << "\n";

            p.paraCadaMeGusta([&](const int& idUsuario) {
                meGusta << p.getId() << "|" << idUsuario << "\n";
                });
            });

        std::ofstream comentarios("comentarios.txt");
        red.comentarios.paraCada([&](const Comentario& c) {
            comentarios << c.getId() << "|" << c.getIdAutor() << "|" << c.getIdPublicacion() << "|"
                << c.getIdPadre() << "|" << limpiar(c.getTexto()) << "|" << c.getFecha() << "\n";
            });

        std::ofstream recomendaciones("recomendaciones.txt");
        red.recomendaciones.paraCada([&](const Recomendacion& r) {
            recomendaciones << r.getId() << "|" << r.getIdEmisor() << "|" << r.getIdReceptor() << "|"
                << limpiar(r.getTexto()) << "|" << r.getFecha() << "\n";
            });

        std::ofstream mensajes("mensajes.txt");
        red.mensajes.paraCada([&](const Mensaje& m) {
            mensajes << m.getId() << "|" << m.getIdEmisor() << "|" << m.getIdReceptor() << "|"
                << limpiar(m.getTexto()) << "|" << m.getFecha() << "|" << deBool(m.fueLeido()) << "\n";
            });

        return true;
    }

    // ============================================================
    //  Cargar
    // ============================================================
    // Devuelve false si no existe usuarios.txt (primera ejecucion).
    // El orden importa: primero los objetos y despues las relaciones.
    static bool cargarTodo(RedProfesional& red) {
        std::ifstream usuarios("usuarios.txt");
        if (!usuarios.is_open()) return false;

        std::string linea;
        while (std::getline(usuarios, linea)) {
            if (linea == "") continue;
            Lista<std::string> c = partir(linea);
            if (c.longitud() < 5) continue;
            int id = aEntero(c.obtenerPos(0));
            red.usuarios.agregaFinal(Usuario(id, c.obtenerPos(1), c.obtenerPos(2),
                c.obtenerPos(3), c.obtenerPos(4)));
            actualizarContador(red.sigUsuario, id);
        }

        std::ifstream empresas("empresas.txt");
        while (std::getline(empresas, linea)) {
            if (linea == "") continue;
            Lista<std::string> c = partir(linea);
            if (c.longitud() < 4) continue;
            int id = aEntero(c.obtenerPos(0));
            red.empresas.agregaFinal(Empresa(id, c.obtenerPos(1), c.obtenerPos(2), c.obtenerPos(3)));
            actualizarContador(red.sigEmpresa, id);
        }

        std::ifstream vacantes("vacantes.txt");
        while (std::getline(vacantes, linea)) {
            if (linea == "") continue;
            Lista<std::string> c = partir(linea);
            if (c.longitud() < 7) continue;
            int id = aEntero(c.obtenerPos(0));
            Vacante v(id, aEntero(c.obtenerPos(1)), c.obtenerPos(2), c.obtenerPos(3),
                c.obtenerPos(4), c.obtenerPos(5));
            if (!aBool(c.obtenerPos(6))) v.cerrar();
            red.vacantes.agregaFinal(v);
            actualizarContador(red.sigVacante, id);
        }

        std::ifstream postulaciones("postulaciones.txt");
        while (std::getline(postulaciones, linea)) {
            if (linea == "") continue;
            Lista<std::string> c = partir(linea);
            if (c.longitud() < 5) continue;
            int id = aEntero(c.obtenerPos(0));
            Postulacion p(id, aEntero(c.obtenerPos(1)), aEntero(c.obtenerPos(2)), c.obtenerPos(3));
            int estado = aEntero(c.obtenerPos(4));
            if (estado == (int)EstadoPostulacion::Revisada) p.revisar();
            if (estado == (int)EstadoPostulacion::Aceptada) p.aceptar();
            if (estado == (int)EstadoPostulacion::Rechazada) p.rechazar();
            red.postulaciones.agregaFinal(p);
            actualizarContador(red.sigPostulacion, id);
        }

        std::ifstream grupos("grupos.txt");
        while (std::getline(grupos, linea)) {
            if (linea == "") continue;
            Lista<std::string> c = partir(linea);
            if (c.longitud() < 4) continue;
            int id = aEntero(c.obtenerPos(0));
            red.grupos.agregaFinal(GrupoProfesional(id, c.obtenerPos(1), c.obtenerPos(2), c.obtenerPos(3)));
            actualizarContador(red.sigGrupo, id);
        }

        std::ifstream publicaciones("publicaciones.txt");
        while (std::getline(publicaciones, linea)) {
            if (linea == "") continue;
            Lista<std::string> c = partir(linea);
            if (c.longitud() < 4) continue;
            int id = aEntero(c.obtenerPos(0));
            red.publicaciones.agregaFinal(Publicacion(id, aEntero(c.obtenerPos(1)),
                c.obtenerPos(2), c.obtenerPos(3)));
            actualizarContador(red.sigPublicacion, id);
        }

        std::ifstream comentarios("comentarios.txt");
        while (std::getline(comentarios, linea)) {
            if (linea == "") continue;
            Lista<std::string> c = partir(linea);
            if (c.longitud() < 6) continue;
            int id = aEntero(c.obtenerPos(0));
            red.comentarios.agregaFinal(Comentario(id, aEntero(c.obtenerPos(1)),
                aEntero(c.obtenerPos(2)), c.obtenerPos(4), c.obtenerPos(5), aEntero(c.obtenerPos(3))));
            actualizarContador(red.sigComentario, id);
        }

        std::ifstream recomendaciones("recomendaciones.txt");
        while (std::getline(recomendaciones, linea)) {
            if (linea == "") continue;
            Lista<std::string> c = partir(linea);
            if (c.longitud() < 5) continue;
            int id = aEntero(c.obtenerPos(0));
            red.recomendaciones.agregaFinal(Recomendacion(id, aEntero(c.obtenerPos(1)),
                aEntero(c.obtenerPos(2)), c.obtenerPos(3), c.obtenerPos(4)));
            actualizarContador(red.sigRecomendacion, id);
        }

        std::ifstream mensajes("mensajes.txt");
        while (std::getline(mensajes, linea)) {
            if (linea == "") continue;
            Lista<std::string> c = partir(linea);
            if (c.longitud() < 6) continue;
            int id = aEntero(c.obtenerPos(0));
            Mensaje m(id, aEntero(c.obtenerPos(1)), aEntero(c.obtenerPos(2)),
                c.obtenerPos(3), c.obtenerPos(4));
            if (aBool(c.obtenerPos(5))) m.marcarLeido();
            red.mensajes.agregaFinal(m);
            actualizarContador(red.sigMensaje, id);
        }

        // ---------- Relaciones: se cargan despues de los objetos ----------

        std::ifstream contactos("contactos.txt");
        while (std::getline(contactos, linea)) {
            if (linea == "") continue;
            Lista<std::string> c = partir(linea);
            if (c.longitud() < 2) continue;
            Usuario* u = red.buscarUsuario(aEntero(c.obtenerPos(0)));
            if (u != nullptr) u->agregarContacto(aEntero(c.obtenerPos(1)));
        }

        std::ifstream habilidades("habilidades.txt");
        while (std::getline(habilidades, linea)) {
            if (linea == "") continue;
            Lista<std::string> c = partir(linea);
            if (c.longitud() < 3) continue;
            Usuario* u = red.buscarUsuario(aEntero(c.obtenerPos(0)));
            if (u != nullptr)
                u->agregarHabilidad(Habilidad(c.obtenerPos(1), (NivelHabilidad)aEntero(c.obtenerPos(2))));
        }

        std::ifstream experiencias("experiencias.txt");
        while (std::getline(experiencias, linea)) {
            if (linea == "") continue;
            Lista<std::string> c = partir(linea);
            if (c.longitud() < 7) continue;
            Usuario* u = red.buscarUsuario(aEntero(c.obtenerPos(0)));
            if (u != nullptr)
                u->agregarExperiencia(ExperienciaLaboral(aEntero(c.obtenerPos(1)), c.obtenerPos(2),
                    c.obtenerPos(3), aEntero(c.obtenerPos(4)), aEntero(c.obtenerPos(5)), c.obtenerPos(6)));
        }

        std::ifstream certificaciones("certificaciones.txt");
        while (std::getline(certificaciones, linea)) {
            if (linea == "") continue;
            Lista<std::string> c = partir(linea);
            if (c.longitud() < 6) continue;
            Usuario* u = red.buscarUsuario(aEntero(c.obtenerPos(0)));
            int id = aEntero(c.obtenerPos(1));
            if (u != nullptr)
                u->agregarCertificacion(Certificacion(id, c.obtenerPos(2), c.obtenerPos(3),
                    c.obtenerPos(4), c.obtenerPos(5)));
            actualizarContador(red.sigCertificacion, id);
        }

        std::ifstream solicitudes("solicitudes.txt");
        while (std::getline(solicitudes, linea)) {
            if (linea == "") continue;
            Lista<std::string> c = partir(linea);
            if (c.longitud() < 5) continue;
            int id = aEntero(c.obtenerPos(0));
            Usuario* receptor = red.buscarUsuario(aEntero(c.obtenerPos(2)));
            if (receptor != nullptr)
                receptor->recibirSolicitud(SolicitudConexion(id, aEntero(c.obtenerPos(1)),
                    aEntero(c.obtenerPos(2)), c.obtenerPos(3), c.obtenerPos(4)));
            actualizarContador(red.sigSolicitud, id);
        }

        std::ifstream notificaciones("notificaciones.txt");
        while (std::getline(notificaciones, linea)) {
            if (linea == "") continue;
            Lista<std::string> c = partir(linea);
            if (c.longitud() < 6) continue;
            int id = aEntero(c.obtenerPos(0));
            Usuario* destino = red.buscarUsuario(aEntero(c.obtenerPos(1)));
            if (destino != nullptr) {
                Notificacion n(id, aEntero(c.obtenerPos(1)), (TipoNotificacion)aEntero(c.obtenerPos(2)),
                    c.obtenerPos(3), c.obtenerPos(4));
                if (aBool(c.obtenerPos(5))) n.marcarLeida();
                destino->recibirNotificacion(n);
            }
            actualizarContador(red.sigNotificacion, id);
        }

        std::ifstream miembros("miembros.txt");
        while (std::getline(miembros, linea)) {
            if (linea == "") continue;
            Lista<std::string> c = partir(linea);
            if (c.longitud() < 2) continue;
            GrupoProfesional* g = red.buscarGrupo(aEntero(c.obtenerPos(0)));
            if (g != nullptr) g->agregarMiembro(aEntero(c.obtenerPos(1)));
        }

        std::ifstream meGusta("me_gusta.txt");
        while (std::getline(meGusta, linea)) {
            if (linea == "") continue;
            Lista<std::string> c = partir(linea);
            if (c.longitud() < 2) continue;
            Publicacion* p = red.buscarPublicacion(aEntero(c.obtenerPos(0)));
            if (p != nullptr) p->darMeGusta(aEntero(c.obtenerPos(1)));
        }

        std::ifstream porRevisar("postulaciones_por_revisar.txt");
        while (std::getline(porRevisar, linea)) {
            if (linea == "") continue;
            Lista<std::string> c = partir(linea);
            if (c.longitud() < 2) continue;
            Vacante* v = red.buscarVacante(aEntero(c.obtenerPos(0)));
            if (v != nullptr) v->recibirPostulacion(aEntero(c.obtenerPos(1)));
        }

        return true;
    }
};