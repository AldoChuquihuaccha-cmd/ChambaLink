#pragma once
#include <string>
#include <fstream>
#include <cstdio>
#include "RedProfesional.h"

#define ARCHIVO_BINARIO "chambalink.bin"
#define FIRMA "CHAMBA02"     // 8 letras al inicio del binario para reconocerlo

// Persistencia de ChambaLink. Cada vez que se guarda, los datos quedan en dos formatos:
//
//   1. Texto: un archivo .txt por clase principal (usuarios.txt, vacantes.txt...).
//      Cada linea es un registro y los campos van separados por '|'.
//      Se puede abrir con el Bloc de notas para revisar los datos.
//   2. Binario: todo en chambalink.bin. Cada numero se escribe con sus 4 bytes
//      y cada texto como su largo seguido de sus caracteres.
//
// Al iniciar se lee el texto. Si no estan los .txt se recupera desde el binario,
// y si tampoco existe se cargan los datos de ejemplo.
//
// Los ids se guardan tal cual; al cargar, cada contador queda en el id mas alto mas uno.
// La pila de acciones no se guarda: sus lambdas no se pueden escribir en un archivo,
// asi que cada sesion empieza con el historial de deshacer vacio.
class GestorArchivos {
private:
    // ==================== Apoyo ====================

    // Deja el contador en el id mas alto encontrado mas uno.
    static void actualizarContador(int& contador, int id) {
        if (id >= contador) contador = id + 1;
    }

    // Quita '|' y saltos de linea de un texto para que no rompan el formato.
    static std::string limpiar(std::string texto) {
        for (size_t i = 0; i < texto.length(); i++)
            if (texto[i] == '|' || texto[i] == '\n' || texto[i] == '\r') texto[i] = ' ';
        return texto;
    }

    // Separa una linea por '|'. Se recorre caracter por caracter para no perder
    // el ultimo campo cuando esta vacio: "1|Ana|" da 3 campos. O(n)
    static Lista<std::string> partir(std::string linea) {
        Lista<std::string> campos;
        std::string campo;
        for (size_t i = 0; i < linea.length(); i++) {
            if (linea[i] == '|') { campos.agregaFinal(campo); campo = ""; }
            else if (linea[i] != '\r') campo += linea[i];
        }
        campos.agregaFinal(campo);
        return campos;
    }

    // Convierte el campo pos a numero. Si no es un numero valido devuelve 0.
    static int entero(const Lista<std::string>& c, uint pos) {
        std::string t = c.obtenerPos(pos);
        if (t == "" || t.length() > 9) return 0;
        for (size_t i = 0; i < t.length(); i++)
            if ((t[i] < '0' || t[i] > '9') && !(i == 0 && t[i] == '-')) return 0;
        if (t == "-") return 0;
        return std::stoi(t);
    }
    static std::string campo(const Lista<std::string>& c, uint pos) { return c.obtenerPos(pos); }

    // Lee todas las lineas de un archivo de texto en una lista.
    static Lista<std::string> leerLineas(std::string nombre) {
        Lista<std::string> lineas;
        std::ifstream in(nombre);
        std::string linea;
        while (std::getline(in, linea))
            if (linea != "" && linea != "\r") lineas.agregaFinal(linea);
        return lineas;
    }

    // ==================== Texto: guardar ====================

    static void guardarTexto(const RedProfesional& red) {
        std::ofstream usuarios("usuarios.txt"), contactos("contactos.txt"), habilidades("habilidades.txt"),
            experiencias("experiencias.txt"), certificaciones("certificaciones.txt"),
            solicitudes("solicitudes.txt"), notificaciones("notificaciones.txt");

        red.usuarios.paraCada([&](const Usuario& u) {
            int id = u.getId();
            usuarios << id << "|" << limpiar(u.getCorreo()) << "|" << limpiar(u.getContrasena()) << "|"
                << limpiar(u.getNombre()) << "|" << limpiar(u.getApellido()) << "|"
                << limpiar(u.getTitular()) << "|" << limpiar(u.getUbicacion()) << "\n";
            u.paraCadaContacto([&](const int& idContacto) { contactos << id << "|" << idContacto << "\n"; });
            u.paraCadaHabilidad([&](const Habilidad& h) {
                habilidades << id << "|" << limpiar(h.getNombre()) << "|" << (int)h.getNivel() << "\n";
                });
            u.paraCadaExperiencia([&](const ExperienciaLaboral& e) {
                experiencias << id << "|" << e.getIdEmpresa() << "|" << limpiar(e.getNombreEmpresa()) << "|"
                    << limpiar(e.getCargo()) << "|" << e.getAnioInicio() << "|" << e.getAnioFin() << "|"
                    << limpiar(e.getDescripcion()) << "\n";
                }, false);
            u.paraCadaCertificacion([&](const Certificacion& c) {
                certificaciones << id << "|" << c.getId() << "|" << limpiar(c.getNombre()) << "|"
                    << limpiar(c.getInstitucion()) << "|" << c.getFechaObtencion() << "|"
                    << limpiar(c.getCodigoCredencial()) << "\n";
                });
            // Las colas se escriben en orden de llegada para recuperarlas igual.
            u.paraCadaSolicitud([&](const SolicitudConexion& s) {
                solicitudes << s.getId() << "|" << s.getIdEmisor() << "|" << s.getIdReceptor() << "|"
                    << limpiar(s.getMensaje()) << "|" << s.getFecha() << "\n";
                });
            u.paraCadaNotificacion([&](const Notificacion& n) {
                notificaciones << n.getId() << "|" << n.getIdDestino() << "|" << (int)n.getTipo() << "|"
                    << limpiar(n.getMensaje()) << "|" << n.getFecha() << "|" << (n.estaLeida() ? 1 : 0) << "\n";
                });
            });

        std::ofstream empresas("empresas.txt");
        red.empresas.paraCada([&](const Empresa& e) {
            empresas << e.getId() << "|" << limpiar(e.getCorreo()) << "|" << limpiar(e.getContrasena()) << "|"
                << limpiar(e.getNombre()) << "|" << limpiar(e.getSector()) << "|" << limpiar(e.getUbicacion()) << "\n";
            });

        std::ofstream vacantes("vacantes.txt"), porRevisar("postulaciones_por_revisar.txt");
        red.vacantes.paraCada([&](const Vacante& v) {
            vacantes << v.getId() << "|" << v.getIdEmpresa() << "|" << limpiar(v.getTitulo()) << "|"
                << limpiar(v.getDescripcion()) << "|" << limpiar(v.getRequisitos()) << "|"
                << limpiar(v.getModalidad()) << "|" << (v.estaActiva() ? 1 : 0) << "\n";
            v.paraCadaPostulacionPorRevisar([&](const int& idPostulacion) {
                porRevisar << v.getId() << "|" << idPostulacion << "\n";
                });
            });

        std::ofstream postulaciones("postulaciones.txt");
        red.postulaciones.paraCada([&](const Postulacion& p) {
            postulaciones << p.getId() << "|" << p.getIdUsuario() << "|" << p.getIdVacante() << "|"
                << p.getFecha() << "|" << (int)p.getEstado() << "\n";
            });

        std::ofstream grupos("grupos.txt"), miembros("miembros.txt");
        red.grupos.paraCada([&](const GrupoProfesional& g) {
            grupos << g.getId() << "|" << limpiar(g.getNombre()) << "|" << limpiar(g.getDescripcion()) << "|"
                << limpiar(g.getEspecialidad()) << "\n";
            g.paraCadaMiembro([&](const int& idUsuario) { miembros << g.getId() << "|" << idUsuario << "\n"; });
            });

        std::ofstream publicaciones("publicaciones.txt"), meGusta("me_gusta.txt");
        red.publicaciones.paraCada([&](const Publicacion& p) {
            publicaciones << p.getId() << "|" << p.getIdAutor() << "|" << limpiar(p.getTexto()) << "|" << p.getFecha() << "\n";
            p.paraCadaMeGusta([&](const int& idUsuario) { meGusta << p.getId() << "|" << idUsuario << "\n"; });
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
                << limpiar(m.getTexto()) << "|" << m.getFecha() << "|" << (m.fueLeido() ? 1 : 0) << "\n";
            });
    }

    // ==================== Texto: cargar ====================
    // Primero las cuentas, porque los demas archivos se enlazan a ellas por id.
    // Una linea con menos campos de los esperados se ignora.

    static bool cargarTexto(RedProfesional& red) {
        Lista<std::string> lineas = leerLineas("usuarios.txt");
        if (lineas.esVacia()) return false;

        lineas.paraCada([&](const std::string& l) {
            Lista<std::string> c = partir(l);
            if (c.longitud() < 7) return;
            int id = entero(c, 0);
            red.usuarios.agregaFinal(Usuario(id, campo(c, 3), campo(c, 4), campo(c, 5), campo(c, 6),
                campo(c, 1), campo(c, 2)));
            actualizarContador(red.sigUsuario, id);
            });

        leerLineas("contactos.txt").paraCada([&](const std::string& l) {
            Lista<std::string> c = partir(l);
            if (c.longitud() < 2) return;
            Usuario* u = red.buscarUsuario(entero(c, 0));
            if (u != nullptr) u->agregarContacto(entero(c, 1));
            });

        leerLineas("habilidades.txt").paraCada([&](const std::string& l) {
            Lista<std::string> c = partir(l);
            if (c.longitud() < 3) return;
            Usuario* u = red.buscarUsuario(entero(c, 0));
            int nivel = entero(c, 2);
            if (u != nullptr && nivel >= 0 && nivel <= 3)
                u->agregarHabilidad(Habilidad(campo(c, 1), (NivelHabilidad)nivel));
            });

        leerLineas("experiencias.txt").paraCada([&](const std::string& l) {
            Lista<std::string> c = partir(l);
            if (c.longitud() < 7) return;
            Usuario* u = red.buscarUsuario(entero(c, 0));
            int inicio = entero(c, 4);
            int fin = entero(c, 5);
            if (u != nullptr && (fin == 0 || fin >= inicio))
                u->agregarExperiencia(ExperienciaLaboral(entero(c, 1), campo(c, 2), campo(c, 3), inicio, fin, campo(c, 6)));
            });

        leerLineas("certificaciones.txt").paraCada([&](const std::string& l) {
            Lista<std::string> c = partir(l);
            if (c.longitud() < 6) return;
            Usuario* u = red.buscarUsuario(entero(c, 0));
            if (u == nullptr) return;
            int id = entero(c, 1);
            u->agregarCertificacion(Certificacion(id, campo(c, 2), campo(c, 3), campo(c, 4), campo(c, 5)));
            actualizarContador(red.sigCertificacion, id);
            });

        leerLineas("solicitudes.txt").paraCada([&](const std::string& l) {
            Lista<std::string> c = partir(l);
            if (c.longitud() < 5) return;
            Usuario* receptor = red.buscarUsuario(entero(c, 2));
            if (receptor == nullptr) return;
            int id = entero(c, 0);
            receptor->recibirSolicitud(SolicitudConexion(id, entero(c, 1), entero(c, 2), campo(c, 3), campo(c, 4)));
            actualizarContador(red.sigSolicitud, id);
            });

        leerLineas("notificaciones.txt").paraCada([&](const std::string& l) {
            Lista<std::string> c = partir(l);
            if (c.longitud() < 6) return;
            Usuario* destino = red.buscarUsuario(entero(c, 1));
            int tipo = entero(c, 2);
            if (destino == nullptr || tipo < 0 || tipo > (int)TipoNotificacion::EstadoPostulacion) return;
            int id = entero(c, 0);
            Notificacion n(id, entero(c, 1), (TipoNotificacion)tipo, campo(c, 3), campo(c, 4));
            if (campo(c, 5) == "1") n.marcarLeida();
            destino->recibirNotificacion(n);
            actualizarContador(red.sigNotificacion, id);
            });

        leerLineas("empresas.txt").paraCada([&](const std::string& l) {
            Lista<std::string> c = partir(l);
            if (c.longitud() < 6) return;
            int id = entero(c, 0);
            red.empresas.agregaFinal(Empresa(id, campo(c, 3), campo(c, 4), campo(c, 5), campo(c, 1), campo(c, 2)));
            actualizarContador(red.sigEmpresa, id);
            });

        leerLineas("vacantes.txt").paraCada([&](const std::string& l) {
            Lista<std::string> c = partir(l);
            if (c.longitud() < 7) return;
            int id = entero(c, 0);
            Vacante v(id, entero(c, 1), campo(c, 2), campo(c, 3), campo(c, 4), campo(c, 5));
            if (campo(c, 6) == "0") v.cerrar();
            red.vacantes.agregaFinal(v);
            actualizarContador(red.sigVacante, id);
            });

        leerLineas("postulaciones_por_revisar.txt").paraCada([&](const std::string& l) {
            Lista<std::string> c = partir(l);
            if (c.longitud() < 2) return;
            Vacante* v = red.buscarVacante(entero(c, 0));
            if (v != nullptr) v->recibirPostulacion(entero(c, 1));
            });

        leerLineas("postulaciones.txt").paraCada([&](const std::string& l) {
            Lista<std::string> c = partir(l);
            if (c.longitud() < 5) return;
            int id = entero(c, 0);
            int estado = entero(c, 4);
            Postulacion p(id, entero(c, 1), entero(c, 2), campo(c, 3));
            if (estado == (int)EstadoPostulacion::Revisada) p.revisar();
            else if (estado == (int)EstadoPostulacion::Aceptada) p.aceptar();
            else if (estado == (int)EstadoPostulacion::Rechazada) p.rechazar();
            red.postulaciones.agregaFinal(p);
            actualizarContador(red.sigPostulacion, id);
            });

        leerLineas("grupos.txt").paraCada([&](const std::string& l) {
            Lista<std::string> c = partir(l);
            if (c.longitud() < 4) return;
            int id = entero(c, 0);
            red.grupos.agregaFinal(GrupoProfesional(id, campo(c, 1), campo(c, 2), campo(c, 3)));
            actualizarContador(red.sigGrupo, id);
            });

        leerLineas("miembros.txt").paraCada([&](const std::string& l) {
            Lista<std::string> c = partir(l);
            if (c.longitud() < 2) return;
            GrupoProfesional* g = red.buscarGrupo(entero(c, 0));
            if (g != nullptr) g->agregarMiembro(entero(c, 1));
            });

        leerLineas("publicaciones.txt").paraCada([&](const std::string& l) {
            Lista<std::string> c = partir(l);
            if (c.longitud() < 4) return;
            int id = entero(c, 0);
            red.publicaciones.agregaFinal(Publicacion(id, entero(c, 1), campo(c, 2), campo(c, 3)));
            actualizarContador(red.sigPublicacion, id);
            });

        leerLineas("me_gusta.txt").paraCada([&](const std::string& l) {
            Lista<std::string> c = partir(l);
            if (c.longitud() < 2) return;
            Publicacion* p = red.buscarPublicacion(entero(c, 0));
            if (p != nullptr) p->darMeGusta(entero(c, 1));
            });

        leerLineas("comentarios.txt").paraCada([&](const std::string& l) {
            Lista<std::string> c = partir(l);
            if (c.longitud() < 6) return;
            int id = entero(c, 0);
            red.comentarios.agregaFinal(Comentario(id, entero(c, 1), entero(c, 2), campo(c, 4), campo(c, 5), entero(c, 3)));
            actualizarContador(red.sigComentario, id);
            });

        leerLineas("recomendaciones.txt").paraCada([&](const std::string& l) {
            Lista<std::string> c = partir(l);
            if (c.longitud() < 5) return;
            int id = entero(c, 0);
            red.recomendaciones.agregaFinal(Recomendacion(id, entero(c, 1), entero(c, 2), campo(c, 3), campo(c, 4)));
            actualizarContador(red.sigRecomendacion, id);
            });

        leerLineas("mensajes.txt").paraCada([&](const std::string& l) {
            Lista<std::string> c = partir(l);
            if (c.longitud() < 6) return;
            int id = entero(c, 0);
            Mensaje m(id, entero(c, 1), entero(c, 2), campo(c, 3), campo(c, 4));
            if (campo(c, 5) == "1") m.marcarLeido();
            red.mensajes.agregaFinal(m);
            actualizarContador(red.sigMensaje, id);
            });

        return true;
    }

    // ==================== Binario ====================

    static void escribirEntero(std::ofstream& out, int numero) {
        out.write((const char*)&numero, sizeof(int));
    }

    // Primero el largo y despues los caracteres, asi al leer se sabe cuantos tomar.
    static void escribirTexto(std::ofstream& out, const std::string& t) {
        escribirEntero(out, (int)t.length());
        out.write(t.c_str(), t.length());
    }

    static int leerEntero(std::ifstream& in) {
        int numero = 0;
        in.read((char*)&numero, sizeof(int));
        return numero;
    }

    static std::string leerTexto(std::ifstream& in) {
        int largo = leerEntero(in);
        if (!in || largo < 0 || largo > 100000) { in.setstate(std::ios::failbit); return ""; }
        std::string t(largo, ' ');
        if (largo > 0) in.read(&t[0], largo);
        return t;
    }

    static void guardarBinario(const RedProfesional& red) {
        std::ofstream out(ARCHIVO_BINARIO, std::ios::binary);
        if (!out.is_open()) return;
        out.write(FIRMA, 8);

        escribirEntero(out, red.usuarios.longitud());
        red.usuarios.paraCada([&](const Usuario& u) {
            escribirEntero(out, u.getId());
            escribirTexto(out, u.getCorreo());
            escribirTexto(out, u.getContrasena());
            escribirTexto(out, u.getNombre());
            escribirTexto(out, u.getApellido());
            escribirTexto(out, u.getTitular());
            escribirTexto(out, u.getUbicacion());

            escribirEntero(out, u.totalContactos());
            u.paraCadaContacto([&](const int& idContacto) { escribirEntero(out, idContacto); });

            escribirEntero(out, u.totalHabilidades());
            u.paraCadaHabilidad([&](const Habilidad& h) {
                escribirTexto(out, h.getNombre());
                escribirEntero(out, (int)h.getNivel());
                });

            escribirEntero(out, u.totalExperiencias());
            u.paraCadaExperiencia([&](const ExperienciaLaboral& e) {
                escribirEntero(out, e.getIdEmpresa());
                escribirTexto(out, e.getNombreEmpresa());
                escribirTexto(out, e.getCargo());
                escribirEntero(out, e.getAnioInicio());
                escribirEntero(out, e.getAnioFin());
                escribirTexto(out, e.getDescripcion());
                }, false);

            escribirEntero(out, u.totalCertificaciones());
            u.paraCadaCertificacion([&](const Certificacion& c) {
                escribirEntero(out, c.getId());
                escribirTexto(out, c.getNombre());
                escribirTexto(out, c.getInstitucion());
                escribirTexto(out, c.getFechaObtencion());
                escribirTexto(out, c.getCodigoCredencial());
                });

            escribirEntero(out, u.cantidadSolicitudesPendientes());
            u.paraCadaSolicitud([&](const SolicitudConexion& s) {
                escribirEntero(out, s.getId());
                escribirEntero(out, s.getIdEmisor());
                escribirTexto(out, s.getMensaje());
                escribirTexto(out, s.getFecha());
                });

            escribirEntero(out, u.cantidadNotificaciones());
            u.paraCadaNotificacion([&](const Notificacion& n) {
                escribirEntero(out, n.getId());
                escribirEntero(out, (int)n.getTipo());
                escribirTexto(out, n.getMensaje());
                escribirTexto(out, n.getFecha());
                escribirEntero(out, n.estaLeida() ? 1 : 0);
                });
            });

        escribirEntero(out, red.empresas.longitud());
        red.empresas.paraCada([&](const Empresa& e) {
            escribirEntero(out, e.getId());
            escribirTexto(out, e.getCorreo());
            escribirTexto(out, e.getContrasena());
            escribirTexto(out, e.getNombre());
            escribirTexto(out, e.getSector());
            escribirTexto(out, e.getUbicacion());
            });

        escribirEntero(out, red.vacantes.longitud());
        red.vacantes.paraCada([&](const Vacante& v) {
            escribirEntero(out, v.getId());
            escribirEntero(out, v.getIdEmpresa());
            escribirTexto(out, v.getTitulo());
            escribirTexto(out, v.getDescripcion());
            escribirTexto(out, v.getRequisitos());
            escribirTexto(out, v.getModalidad());
            escribirEntero(out, v.estaActiva() ? 1 : 0);
            escribirEntero(out, v.cantidadPorRevisar());
            v.paraCadaPostulacionPorRevisar([&](const int& idPostulacion) { escribirEntero(out, idPostulacion); });
            });

        escribirEntero(out, red.postulaciones.longitud());
        red.postulaciones.paraCada([&](const Postulacion& p) {
            escribirEntero(out, p.getId());
            escribirEntero(out, p.getIdUsuario());
            escribirEntero(out, p.getIdVacante());
            escribirTexto(out, p.getFecha());
            escribirEntero(out, (int)p.getEstado());
            });

        escribirEntero(out, red.grupos.longitud());
        red.grupos.paraCada([&](const GrupoProfesional& g) {
            escribirEntero(out, g.getId());
            escribirTexto(out, g.getNombre());
            escribirTexto(out, g.getDescripcion());
            escribirTexto(out, g.getEspecialidad());
            escribirEntero(out, g.getCantidadMiembros());
            g.paraCadaMiembro([&](const int& idUsuario) { escribirEntero(out, idUsuario); });
            });

        escribirEntero(out, red.publicaciones.longitud());
        red.publicaciones.paraCada([&](const Publicacion& p) {
            escribirEntero(out, p.getId());
            escribirEntero(out, p.getIdAutor());
            escribirTexto(out, p.getTexto());
            escribirTexto(out, p.getFecha());
            escribirEntero(out, p.getMeGusta());
            p.paraCadaMeGusta([&](const int& idUsuario) { escribirEntero(out, idUsuario); });
            });

        escribirEntero(out, red.comentarios.longitud());
        red.comentarios.paraCada([&](const Comentario& c) {
            escribirEntero(out, c.getId());
            escribirEntero(out, c.getIdAutor());
            escribirEntero(out, c.getIdPublicacion());
            escribirEntero(out, c.getIdPadre());
            escribirTexto(out, c.getTexto());
            escribirTexto(out, c.getFecha());
            });

        escribirEntero(out, red.recomendaciones.longitud());
        red.recomendaciones.paraCada([&](const Recomendacion& r) {
            escribirEntero(out, r.getId());
            escribirEntero(out, r.getIdEmisor());
            escribirEntero(out, r.getIdReceptor());
            escribirTexto(out, r.getTexto());
            escribirTexto(out, r.getFecha());
            });

        escribirEntero(out, red.mensajes.longitud());
        red.mensajes.paraCada([&](const Mensaje& m) {
            escribirEntero(out, m.getId());
            escribirEntero(out, m.getIdEmisor());
            escribirEntero(out, m.getIdReceptor());
            escribirTexto(out, m.getTexto());
            escribirTexto(out, m.getFecha());
            escribirEntero(out, m.fueLeido() ? 1 : 0);
            });
    }

    // Lee en el mismo orden en que se escribio. Si algo falla (archivo cortado o de
    // otra version) devuelve false y la red queda vacia.
    static bool cargarBinario(RedProfesional& red) {
        std::ifstream in(ARCHIVO_BINARIO, std::ios::binary);
        if (!in.is_open()) return false;
        char firma[8] = {};
        in.read(firma, 8);
        if (!in || std::string(firma, 8) != std::string(FIRMA, 8)) return false;

        int cantidad = leerEntero(in);
        for (int i = 0; i < cantidad && in; i++) {
            int id = leerEntero(in);
            std::string correo = leerTexto(in), contrasena = leerTexto(in), nombre = leerTexto(in),
                apellido = leerTexto(in), titular = leerTexto(in), ubicacion = leerTexto(in);
            Usuario u(id, nombre, apellido, titular, ubicacion, correo, contrasena);
            actualizarContador(red.sigUsuario, id);

            int n = leerEntero(in);
            for (int j = 0; j < n && in; j++) u.agregarContacto(leerEntero(in));

            n = leerEntero(in);
            for (int j = 0; j < n && in; j++) {
                std::string nombreHabilidad = leerTexto(in);
                int nivel = leerEntero(in);
                if (nivel >= 0 && nivel <= 3) u.agregarHabilidad(Habilidad(nombreHabilidad, (NivelHabilidad)nivel));
            }

            n = leerEntero(in);
            for (int j = 0; j < n && in; j++) {
                int idEmpresa = leerEntero(in);
                std::string empresa = leerTexto(in), cargo = leerTexto(in);
                int inicio = leerEntero(in);
                int fin = leerEntero(in);
                std::string descripcion = leerTexto(in);
                if (fin == 0 || fin >= inicio)
                    u.agregarExperiencia(ExperienciaLaboral(idEmpresa, empresa, cargo, inicio, fin, descripcion));
            }

            n = leerEntero(in);
            for (int j = 0; j < n && in; j++) {
                int idCertificacion = leerEntero(in);
                std::string nom = leerTexto(in), institucion = leerTexto(in), fecha = leerTexto(in), codigo = leerTexto(in);
                u.agregarCertificacion(Certificacion(idCertificacion, nom, institucion, fecha, codigo));
                actualizarContador(red.sigCertificacion, idCertificacion);
            }

            n = leerEntero(in);
            for (int j = 0; j < n && in; j++) {
                int idSolicitud = leerEntero(in);
                int idEmisor = leerEntero(in);
                std::string mensaje = leerTexto(in), fecha = leerTexto(in);
                u.recibirSolicitud(SolicitudConexion(idSolicitud, idEmisor, id, mensaje, fecha));
                actualizarContador(red.sigSolicitud, idSolicitud);
            }

            n = leerEntero(in);
            for (int j = 0; j < n && in; j++) {
                int idNotificacion = leerEntero(in);
                int tipo = leerEntero(in);
                std::string mensaje = leerTexto(in), fecha = leerTexto(in);
                bool leida = leerEntero(in) == 1;
                if (tipo < 0 || tipo >(int)TipoNotificacion::EstadoPostulacion) continue;
                Notificacion noti(idNotificacion, id, (TipoNotificacion)tipo, mensaje, fecha);
                if (leida) noti.marcarLeida();
                u.recibirNotificacion(noti);
                actualizarContador(red.sigNotificacion, idNotificacion);
            }
            red.usuarios.agregaFinal(u);
        }

        cantidad = leerEntero(in);
        for (int i = 0; i < cantidad && in; i++) {
            int id = leerEntero(in);
            std::string correo = leerTexto(in), contrasena = leerTexto(in), nombre = leerTexto(in),
                sector = leerTexto(in), ubicacion = leerTexto(in);
            red.empresas.agregaFinal(Empresa(id, nombre, sector, ubicacion, correo, contrasena));
            actualizarContador(red.sigEmpresa, id);
        }

        cantidad = leerEntero(in);
        for (int i = 0; i < cantidad && in; i++) {
            int id = leerEntero(in);
            int idEmpresa = leerEntero(in);
            std::string titulo = leerTexto(in), descripcion = leerTexto(in), requisitos = leerTexto(in),
                modalidad = leerTexto(in);
            Vacante v(id, idEmpresa, titulo, descripcion, requisitos, modalidad);
            if (leerEntero(in) == 0) v.cerrar();
            int n = leerEntero(in);
            for (int j = 0; j < n && in; j++) v.recibirPostulacion(leerEntero(in));
            red.vacantes.agregaFinal(v);
            actualizarContador(red.sigVacante, id);
        }

        cantidad = leerEntero(in);
        for (int i = 0; i < cantidad && in; i++) {
            int id = leerEntero(in);
            int idUsuario = leerEntero(in);
            int idVacante = leerEntero(in);
            std::string fecha = leerTexto(in);
            int estado = leerEntero(in);
            Postulacion p(id, idUsuario, idVacante, fecha);
            if (estado == (int)EstadoPostulacion::Revisada) p.revisar();
            else if (estado == (int)EstadoPostulacion::Aceptada) p.aceptar();
            else if (estado == (int)EstadoPostulacion::Rechazada) p.rechazar();
            red.postulaciones.agregaFinal(p);
            actualizarContador(red.sigPostulacion, id);
        }

        cantidad = leerEntero(in);
        for (int i = 0; i < cantidad && in; i++) {
            int id = leerEntero(in);
            std::string nombre = leerTexto(in), descripcion = leerTexto(in), especialidad = leerTexto(in);
            GrupoProfesional g(id, nombre, descripcion, especialidad);
            int n = leerEntero(in);
            for (int j = 0; j < n && in; j++) g.agregarMiembro(leerEntero(in));
            red.grupos.agregaFinal(g);
            actualizarContador(red.sigGrupo, id);
        }

        cantidad = leerEntero(in);
        for (int i = 0; i < cantidad && in; i++) {
            int id = leerEntero(in);
            int idAutor = leerEntero(in);
            std::string textoPublicacion = leerTexto(in), fecha = leerTexto(in);
            Publicacion p(id, idAutor, textoPublicacion, fecha);
            int n = leerEntero(in);
            for (int j = 0; j < n && in; j++) p.darMeGusta(leerEntero(in));
            red.publicaciones.agregaFinal(p);
            actualizarContador(red.sigPublicacion, id);
        }

        cantidad = leerEntero(in);
        for (int i = 0; i < cantidad && in; i++) {
            int id = leerEntero(in);
            int idAutor = leerEntero(in);
            int idPublicacion = leerEntero(in);
            int idPadre = leerEntero(in);
            std::string textoComentario = leerTexto(in), fecha = leerTexto(in);
            red.comentarios.agregaFinal(Comentario(id, idAutor, idPublicacion, textoComentario, fecha, idPadre));
            actualizarContador(red.sigComentario, id);
        }

        cantidad = leerEntero(in);
        for (int i = 0; i < cantidad && in; i++) {
            int id = leerEntero(in);
            int idEmisor = leerEntero(in);
            int idReceptor = leerEntero(in);
            std::string textoRecomendacion = leerTexto(in), fecha = leerTexto(in);
            red.recomendaciones.agregaFinal(Recomendacion(id, idEmisor, idReceptor, textoRecomendacion, fecha));
            actualizarContador(red.sigRecomendacion, id);
        }

        cantidad = leerEntero(in);
        for (int i = 0; i < cantidad && in; i++) {
            int id = leerEntero(in);
            int idEmisor = leerEntero(in);
            int idReceptor = leerEntero(in);
            std::string textoMensaje = leerTexto(in), fecha = leerTexto(in);
            Mensaje m(id, idEmisor, idReceptor, textoMensaje, fecha);
            if (leerEntero(in) == 1) m.marcarLeido();
            red.mensajes.agregaFinal(m);
            actualizarContador(red.sigMensaje, id);
        }

        if (!in) {   // archivo incompleto: no se usa nada de lo leido
            vaciar(red);
            return false;
        }
        return true;
    }

    static void vaciar(RedProfesional& red) {
        red.usuarios.vaciar();
        red.empresas.vaciar();
        red.vacantes.vaciar();
        red.postulaciones.vaciar();
        red.grupos.vaciar();
        red.publicaciones.vaciar();
        red.comentarios.vaciar();
        red.recomendaciones.vaciar();
        red.mensajes.vaciar();
    }

public:
    // Guarda en los dos formatos.
    static void guardarTodo(const RedProfesional& red) {
        guardarTexto(red);
        guardarBinario(red);
    }

    // Texto primero; si no hay .txt, el binario; si tampoco, los datos de ejemplo.
    static void cargarTodo(RedProfesional& red) {
        if (cargarTexto(red)) return;
        if (cargarBinario(red)) return;
        cargarDatosDeEjemplo(red);
        guardarTodo(red);
    }

    // Datos para la demostracion. Para volver a ellos se borran los .txt y chambalink.bin.
    // Todas las cuentas tienen la contrasena 1234.
    // Las habilidades y conexiones se agregan directo (sin Accion) para que la pila
    // de deshacer empiece vacia.
    static void cargarDatosDeEjemplo(RedProfesional& red) {
        int joao = red.registrarUsuario("Joao", "Rivero", "Desarrollador C++", "Ancon", "joao@demo.pe", "1234");
        int ana = red.registrarUsuario("Ana", "Torres", "Analista de datos", "Miraflores", "ana@demo.pe", "1234");
        int luis = red.registrarUsuario("Luis", "Paredes", "Backend Java", "Surco", "luis@demo.pe", "1234");
        int maria = red.registrarUsuario("Maria", "Quispe", "Disenadora UX", "Lince", "maria@demo.pe", "1234");
        int carlos = red.registrarUsuario("Carlos", "Huaman", "DevOps", "San Miguel", "carlos@demo.pe", "1234");
        int rosa = red.registrarUsuario("Rosa", "Flores", "Docente de algoritmos", "Barranco", "rosa@demo.pe", "1234");
        int diego = red.registrarUsuario("Diego", "Salas", "Practicante de sistemas", "Comas", "diego@demo.pe", "1234");
        int lucia = red.registrarUsuario("Lucia", "Ramos", "Product Manager", "San Borja", "lucia@demo.pe", "1234");

        habilidad(red, joao, "C++", 3);    habilidad(red, joao, "Git", 2);     habilidad(red, joao, "Estructuras de datos", 3);
        habilidad(red, ana, "SQL", 4);     habilidad(red, ana, "Python", 3);   habilidad(red, ana, "Excel", 3);
        habilidad(red, luis, "Java", 4);   habilidad(red, luis, "SQL", 3);     habilidad(red, luis, "Docker", 2);
        habilidad(red, maria, "Figma", 4); habilidad(red, maria, "UX", 3);
        habilidad(red, carlos, "Linux", 4); habilidad(red, carlos, "Docker", 4); habilidad(red, carlos, "Git", 3);
        habilidad(red, rosa, "C++", 4);    habilidad(red, rosa, "Algoritmos", 4);
        habilidad(red, diego, "Python", 2); habilidad(red, diego, "Git", 1);
        habilidad(red, lucia, "Scrum", 4); habilidad(red, lucia, "Excel", 3);

        // Red: Joao conoce a Ana y Luis. Segundo grado: Rosa (2 en comun), Maria y Carlos.
        // Diego esta a tercer grado, por eso no debe salir en las sugerencias de Joao.
        red.conectar(joao, ana);   red.conectar(joao, luis);  red.conectar(ana, luis);
        red.conectar(ana, maria);  red.conectar(luis, carlos); red.conectar(carlos, diego);
        red.conectar(rosa, ana);   red.conectar(rosa, luis);

        // Solicitudes pendientes para Joao (Cola: se atienden en orden de llegada).
        red.enviarSolicitud(lucia, joao, "Hola Joao, conectemos");
        red.enviarSolicitud(diego, joao, "Vi tu perfil de C++");

        int tech = red.registrarEmpresa("TechPeru", "Tecnologia", "San Isidro", "rrhh@techperu.pe", "1234");
        int data = red.registrarEmpresa("DataAndes", "Analitica", "Miraflores", "talento@dataandes.pe", "1234");
        // Experiencia ingresada desordenada: la lista doble la deja por anio de inicio.
        red.agregarExperiencia(joao, "TechPeru", "Practicante de desarrollo", "2024", "2025");
        red.agregarExperiencia(joao, "Cibertec", "Soporte TI", "2022", "2023");
        red.agregarExperiencia(joao, "DataAndes", "Desarrollador C++", "2025", "");
        red.agregarExperiencia(ana, "DataAndes", "Analista de datos", "2021", "");

        red.agregarCertificacion(joao, "Git y GitHub", "Platzi", "2025/03/15", "GIT-2291");
        red.agregarCertificacion(joao, "C++ Intermedio", "Coursera", "2024/08/02", "");
        red.agregarCertificacion(joao, "Scrum Fundamentals", "SCRUMstudy", "2025/11/20", "SF-7781");

        int v1 = red.publicarVacante(tech, "Desarrollador C++ Junior", "Desarrollo de modulos en C++",
            "C++, Git, Estructuras de datos", "Hibrido");
        int v2 = red.publicarVacante(tech, "Analista SQL", "Reportes y consultas", "SQL, Excel, Python", "Remoto");
        red.publicarVacante(data, "Cientifico de datos", "Modelos predictivos", "Python, SQL", "Presencial");
        int v4 = red.publicarVacante(data, "Backend Java", "APIs para clientes", "Java, SQL, Docker", "Hibrido");
        red.postular(joao, v1);
        red.postular(ana, v2);
        red.postular(diego, v2);
        red.postular(luis, v4);

        // Publicaciones con fechas distintas para que se note el MergeSort.
        int p1 = publicacion(red, ana, "Comparto mi dashboard de ventas hecho en Python.", "2026/09/12");
        int p2 = publicacion(red, joao, "Termine mi lista doblemente enlazada con iteradores en C++.", "2026/09/28");
        int p3 = publicacion(red, luis, "Buscamos practicantes de backend en mi equipo.", "2026/09/20");
        publicacion(red, rosa, "Recuerden: QuickSort es O(n log n) en promedio.", "2026/10/01");
        red.darMeGusta(ana, p2);  red.darMeGusta(luis, p2);  red.darMeGusta(rosa, p2);
        red.darMeGusta(joao, p1); red.darMeGusta(joao, p3);
        int c1 = red.comentar(rosa, p2, "Muy bien, ahora analiza su complejidad.");
        red.comentar(joao, p2, "Gracias profe, ya lo agregue al informe.", c1);

        int g1 = red.crearGrupo("Programadores C++", "Dudas y proyectos en C++", "C++");
        int g2 = red.crearGrupo("Datos Lima", "Analitica y bases de datos", "SQL");
        red.unirseAGrupo(joao, g1); red.unirseAGrupo(rosa, g1); red.unirseAGrupo(diego, g1);
        red.unirseAGrupo(ana, g2);  red.unirseAGrupo(luis, g2);

        red.recomendar(ana, joao, "Joao es muy ordenado y explica bien sus soluciones.");
        red.enviarMensaje(ana, joao, "Hola Joao, viste la vacante de TechPeru?");
    }

private:
    static void habilidad(RedProfesional& red, int idUsuario, std::string nombre, int nivel) {
        red.buscarUsuario(idUsuario)->agregarHabilidad(Habilidad(nombre, (NivelHabilidad)(nivel - 1)));
    }

    static int publicacion(RedProfesional& red, int idAutor, std::string texto, std::string fecha) {
        int id = red.sigPublicacion;
        red.sigPublicacion++;
        red.publicaciones.agregaFinal(Publicacion(id, idAutor, texto, fecha));
        return id;
    }
};
