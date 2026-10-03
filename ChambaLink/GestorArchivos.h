#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <string>
#include "RedProfesional.h"

// Gestion de archivos de ChambaLink con persistencia binaria.
//
// En lugar de escribir la memoria cruda de los objetos (lo cual NO es valido para
// std::string, listas, colas, etc.), cada campo se serializa de forma explicita.
// Esto permite reconstruir correctamente objetos, relaciones, colas e IDs.
//
// Archivo principal: chambalink.bin
// Formato: firma + version + contadores + catalogos + relaciones embebidas.
//
// La pila de Accion no se persiste porque contiene funciones lambda; al iniciar
// una nueva sesion el historial de deshacer comienza vacio, igual que antes.
class GestorArchivos {
private:
    static const std::uint32_t VERSION = 1;
    static const std::uint32_t MAX_CADENA = 16u * 1024u * 1024u;
    static const std::uint32_t MAX_REGISTROS = 1000000u;

    static std::string archivoPredeterminado() {
        return "chambalink.bin";
    }

    static void escribirU32(std::ostream& out, std::uint32_t valor) {
        out.write(reinterpret_cast<const char*>(&valor), sizeof(valor));
    }

    static void escribirI32(std::ostream& out, std::int32_t valor) {
        out.write(reinterpret_cast<const char*>(&valor), sizeof(valor));
    }

    static void escribirBool(std::ostream& out, bool valor) {
        std::uint8_t dato = valor ? 1u : 0u;
        out.write(reinterpret_cast<const char*>(&dato), sizeof(dato));
    }

    static void escribirCadena(std::ostream& out, const std::string& texto) {
        std::uint32_t longitud = static_cast<std::uint32_t>(texto.size());
        escribirU32(out, longitud);
        if (longitud > 0)
            out.write(texto.data(), static_cast<std::streamsize>(longitud));
    }

    static bool leerU32(std::istream& in, std::uint32_t& valor) {
        return static_cast<bool>(in.read(reinterpret_cast<char*>(&valor), sizeof(valor)));
    }

    static bool leerI32(std::istream& in, std::int32_t& valor) {
        return static_cast<bool>(in.read(reinterpret_cast<char*>(&valor), sizeof(valor)));
    }

    static bool leerBool(std::istream& in, bool& valor) {
        std::uint8_t dato = 0;
        if (!in.read(reinterpret_cast<char*>(&dato), sizeof(dato))) return false;
        if (dato > 1u) return false;
        valor = dato == 1u;
        return true;
    }

    static bool leerCadena(std::istream& in, std::string& texto) {
        std::uint32_t longitud = 0;
        if (!leerU32(in, longitud) || longitud > MAX_CADENA) return false;
        texto.assign(longitud, '\0');
        if (longitud > 0 && !in.read(&texto[0], static_cast<std::streamsize>(longitud)))
            return false;
        return true;
    }

    static bool leerConteo(std::istream& in, std::uint32_t& cantidad) {
        return leerU32(in, cantidad) && cantidad <= MAX_REGISTROS;
    }

    static void asegurarContador(int& contador, int id, int minimo = 1) {
        if (contador < minimo) contador = minimo;
        if (id >= contador) contador = id + 1;
    }

    static void copiarEstado(RedProfesional& destino, const RedProfesional& origen) {
        destino.usuarios = origen.usuarios;
        destino.empresas = origen.empresas;
        destino.vacantes = origen.vacantes;
        destino.postulaciones = origen.postulaciones;
        destino.grupos = origen.grupos;
        destino.publicaciones = origen.publicaciones;
        destino.comentarios = origen.comentarios;
        destino.recomendaciones = origen.recomendaciones;
        destino.mensajes = origen.mensajes;

        destino.sigUsuario = origen.sigUsuario;
        destino.sigEmpresa = origen.sigEmpresa;
        destino.sigVacante = origen.sigVacante;
        destino.sigPostulacion = origen.sigPostulacion;
        destino.sigGrupo = origen.sigGrupo;
        destino.sigPublicacion = origen.sigPublicacion;
        destino.sigComentario = origen.sigComentario;
        destino.sigRecomendacion = origen.sigRecomendacion;
        destino.sigMensaje = origen.sigMensaje;
        destino.sigSolicitud = origen.sigSolicitud;
        destino.sigNotificacion = origen.sigNotificacion;
        destino.sigCertificacion = origen.sigCertificacion;
        destino.ultimoError = "";
    }

    static void escribirUsuario(std::ostream& out, const Usuario& u) {
        escribirI32(out, u.getId());
        escribirCadena(out, u.getCorreo());
        escribirCadena(out, u.getContrasena());
        escribirCadena(out, u.getNombre());
        escribirCadena(out, u.getApellido());
        escribirCadena(out, u.getTitular());
        escribirCadena(out, u.getUbicacion());

        escribirU32(out, static_cast<std::uint32_t>(u.totalContactos()));
        u.paraCadaContacto([&](const int& idContacto) {
            escribirI32(out, idContacto);
        });

        escribirU32(out, static_cast<std::uint32_t>(u.totalHabilidades()));
        u.paraCadaHabilidad([&](const Habilidad& h) {
            escribirCadena(out, h.getNombre());
            escribirI32(out, static_cast<std::int32_t>(h.getNivel()));
        });

        escribirU32(out, static_cast<std::uint32_t>(u.totalExperiencias()));
        u.paraCadaExperiencia([&](const ExperienciaLaboral& e) {
            escribirI32(out, e.getIdEmpresa());
            escribirCadena(out, e.getNombreEmpresa());
            escribirCadena(out, e.getCargo());
            escribirI32(out, e.getAnioInicio());
            escribirI32(out, e.getAnioFin());
            escribirCadena(out, e.getDescripcion());
        }, false);

        escribirU32(out, static_cast<std::uint32_t>(u.totalCertificaciones()));
        u.paraCadaCertificacion([&](const Certificacion& c) {
            escribirI32(out, c.getId());
            escribirCadena(out, c.getNombre());
            escribirCadena(out, c.getInstitucion());
            escribirCadena(out, c.getFechaObtencion());
            escribirCadena(out, c.getCodigoCredencial());
        });

        escribirU32(out, static_cast<std::uint32_t>(u.cantidadSolicitudesPendientes()));
        u.paraCadaSolicitud([&](const SolicitudConexion& s) {
            escribirI32(out, s.getId());
            escribirI32(out, s.getIdEmisor());
            escribirI32(out, s.getIdReceptor());
            escribirCadena(out, s.getMensaje());
            escribirCadena(out, s.getFecha());
        });

        escribirU32(out, static_cast<std::uint32_t>(u.cantidadNotificaciones()));
        u.paraCadaNotificacion([&](const Notificacion& n) {
            escribirI32(out, n.getId());
            escribirI32(out, n.getIdDestino());
            escribirI32(out, static_cast<std::int32_t>(n.getTipo()));
            escribirCadena(out, n.getMensaje());
            escribirCadena(out, n.getFecha());
            escribirBool(out, n.estaLeida());
        });
    }

    static bool leerUsuario(std::istream& in, RedProfesional& red, Usuario& u) {
        std::int32_t id = 0;
        std::string correo, contrasena, nombre, apellido, titular, ubicacion;
        if (!leerI32(in, id) || !leerCadena(in, correo) || !leerCadena(in, contrasena) ||
            !leerCadena(in, nombre) || !leerCadena(in, apellido) || !leerCadena(in, titular) ||
            !leerCadena(in, ubicacion)) return false;

        u = Usuario(id, nombre, apellido, titular, ubicacion, correo, contrasena);
        asegurarContador(red.sigUsuario, id, 1000);

        std::uint32_t cantidad = 0;
        if (!leerConteo(in, cantidad)) return false;
        for (std::uint32_t i = 0; i < cantidad; ++i) {
            std::int32_t idContacto = 0;
            if (!leerI32(in, idContacto)) return false;
            u.agregarContacto(idContacto);
        }

        if (!leerConteo(in, cantidad)) return false;
        for (std::uint32_t i = 0; i < cantidad; ++i) {
            std::string nombreHabilidad;
            std::int32_t nivel = 0;
            if (!leerCadena(in, nombreHabilidad) || !leerI32(in, nivel)) return false;
            if (nivel < static_cast<int>(NivelHabilidad::Basico) ||
                nivel > static_cast<int>(NivelHabilidad::Experto)) return false;
            u.agregarHabilidad(Habilidad(nombreHabilidad, static_cast<NivelHabilidad>(nivel)));
        }

        if (!leerConteo(in, cantidad)) return false;
        for (std::uint32_t i = 0; i < cantidad; ++i) {
            std::int32_t idEmpresa = 0, anioInicio = 0, anioFin = 0;
            std::string nombreEmpresa, cargo, descripcion;
            if (!leerI32(in, idEmpresa) || !leerCadena(in, nombreEmpresa) || !leerCadena(in, cargo) ||
                !leerI32(in, anioInicio) || !leerI32(in, anioFin) || !leerCadena(in, descripcion))
                return false;
            try {
                u.agregarExperiencia(ExperienciaLaboral(idEmpresa, nombreEmpresa, cargo,
                    anioInicio, anioFin, descripcion));
            }
            catch (...) {
                return false;
            }
        }

        if (!leerConteo(in, cantidad)) return false;
        for (std::uint32_t i = 0; i < cantidad; ++i) {
            std::int32_t idCert = 0;
            std::string nom, institucion, fecha, codigo;
            if (!leerI32(in, idCert) || !leerCadena(in, nom) || !leerCadena(in, institucion) ||
                !leerCadena(in, fecha) || !leerCadena(in, codigo)) return false;
            u.agregarCertificacion(Certificacion(idCert, nom, institucion, fecha, codigo));
            asegurarContador(red.sigCertificacion, idCert);
        }

        if (!leerConteo(in, cantidad)) return false;
        for (std::uint32_t i = 0; i < cantidad; ++i) {
            std::int32_t idSol = 0, idEmisor = 0, idReceptor = 0;
            std::string mensaje, fecha;
            if (!leerI32(in, idSol) || !leerI32(in, idEmisor) || !leerI32(in, idReceptor) ||
                !leerCadena(in, mensaje) || !leerCadena(in, fecha)) return false;
            u.recibirSolicitud(SolicitudConexion(idSol, idEmisor, idReceptor, mensaje, fecha));
            asegurarContador(red.sigSolicitud, idSol);
        }

        if (!leerConteo(in, cantidad)) return false;
        for (std::uint32_t i = 0; i < cantidad; ++i) {
            std::int32_t idNoti = 0, idDestino = 0, tipo = 0;
            std::string mensaje, fecha;
            bool leida = false;
            if (!leerI32(in, idNoti) || !leerI32(in, idDestino) || !leerI32(in, tipo) ||
                !leerCadena(in, mensaje) || !leerCadena(in, fecha) || !leerBool(in, leida))
                return false;
            if (tipo < static_cast<int>(TipoNotificacion::NuevaSolicitud) ||
                tipo > static_cast<int>(TipoNotificacion::EstadoPostulacion)) return false;
            Notificacion n(idNoti, idDestino, static_cast<TipoNotificacion>(tipo), mensaje, fecha);
            if (leida) n.marcarLeida();
            u.recibirNotificacion(n);
            asegurarContador(red.sigNotificacion, idNoti);
        }

        return true;
    }

    static bool leerArchivo(std::istream& in, RedProfesional& red) {
        const char firmaEsperada[8] = { 'C','H','A','M','B','A','B','1' };
        char firma[8] = {};
        if (!in.read(firma, sizeof(firma))) return false;
        if (std::memcmp(firma, firmaEsperada, sizeof(firma)) != 0) return false;

        std::uint32_t version = 0;
        if (!leerU32(in, version) || version != VERSION) return false;

        RedProfesional cargada;

        std::int32_t contadores[12] = {};
        for (int i = 0; i < 12; ++i)
            if (!leerI32(in, contadores[i])) return false;

        cargada.sigUsuario = std::max(1000, static_cast<int>(contadores[0]));
        cargada.sigEmpresa = std::max(1000, static_cast<int>(contadores[1]));
        cargada.sigVacante = std::max(1, static_cast<int>(contadores[2]));
        cargada.sigPostulacion = std::max(1, static_cast<int>(contadores[3]));
        cargada.sigGrupo = std::max(1, static_cast<int>(contadores[4]));
        cargada.sigPublicacion = std::max(1, static_cast<int>(contadores[5]));
        cargada.sigComentario = std::max(1, static_cast<int>(contadores[6]));
        cargada.sigRecomendacion = std::max(1, static_cast<int>(contadores[7]));
        cargada.sigMensaje = std::max(1, static_cast<int>(contadores[8]));
        cargada.sigSolicitud = std::max(1, static_cast<int>(contadores[9]));
        cargada.sigNotificacion = std::max(1, static_cast<int>(contadores[10]));
        cargada.sigCertificacion = std::max(1, static_cast<int>(contadores[11]));

        std::uint32_t cantidad = 0;

        // Usuarios y sus relaciones internas.
        if (!leerConteo(in, cantidad)) return false;
        for (std::uint32_t i = 0; i < cantidad; ++i) {
            Usuario u;
            if (!leerUsuario(in, cargada, u)) return false;
            cargada.usuarios.agregaFinal(u);
        }

        // Empresas.
        if (!leerConteo(in, cantidad)) return false;
        for (std::uint32_t i = 0; i < cantidad; ++i) {
            std::int32_t id = 0;
            std::string correo, contrasena, nombre, sector, ubicacion;
            if (!leerI32(in, id) || !leerCadena(in, correo) || !leerCadena(in, contrasena) ||
                !leerCadena(in, nombre) || !leerCadena(in, sector) || !leerCadena(in, ubicacion))
                return false;
            cargada.empresas.agregaFinal(Empresa(id, nombre, sector, ubicacion, correo, contrasena));
            asegurarContador(cargada.sigEmpresa, id, 1000);
        }

        // Vacantes y cola de postulaciones por revisar.
        if (!leerConteo(in, cantidad)) return false;
        for (std::uint32_t i = 0; i < cantidad; ++i) {
            std::int32_t id = 0, idEmpresa = 0;
            std::string titulo, descripcion, requisitos, modalidad;
            bool activa = true;
            if (!leerI32(in, id) || !leerI32(in, idEmpresa) || !leerCadena(in, titulo) ||
                !leerCadena(in, descripcion) || !leerCadena(in, requisitos) ||
                !leerCadena(in, modalidad) || !leerBool(in, activa)) return false;
            Vacante v(id, idEmpresa, titulo, descripcion, requisitos, modalidad);
            if (!activa) v.cerrar();

            std::uint32_t pendientes = 0;
            if (!leerConteo(in, pendientes)) return false;
            for (std::uint32_t j = 0; j < pendientes; ++j) {
                std::int32_t idPost = 0;
                if (!leerI32(in, idPost)) return false;
                v.recibirPostulacion(idPost);
            }
            cargada.vacantes.agregaFinal(v);
            asegurarContador(cargada.sigVacante, id);
        }

        // Postulaciones.
        if (!leerConteo(in, cantidad)) return false;
        for (std::uint32_t i = 0; i < cantidad; ++i) {
            std::int32_t id = 0, idUsuario = 0, idVacante = 0, estado = 0;
            std::string fecha;
            if (!leerI32(in, id) || !leerI32(in, idUsuario) || !leerI32(in, idVacante) ||
                !leerCadena(in, fecha) || !leerI32(in, estado)) return false;
            if (estado < static_cast<int>(EstadoPostulacion::Pendiente) ||
                estado > static_cast<int>(EstadoPostulacion::Rechazada)) return false;
            Postulacion p(id, idUsuario, idVacante, fecha);
            if (estado == static_cast<int>(EstadoPostulacion::Revisada)) p.revisar();
            else if (estado == static_cast<int>(EstadoPostulacion::Aceptada)) p.aceptar();
            else if (estado == static_cast<int>(EstadoPostulacion::Rechazada)) p.rechazar();
            cargada.postulaciones.agregaFinal(p);
            asegurarContador(cargada.sigPostulacion, id);
        }

        // Grupos y miembros.
        if (!leerConteo(in, cantidad)) return false;
        for (std::uint32_t i = 0; i < cantidad; ++i) {
            std::int32_t id = 0;
            std::string nombre, descripcion, especialidad;
            if (!leerI32(in, id) || !leerCadena(in, nombre) || !leerCadena(in, descripcion) ||
                !leerCadena(in, especialidad)) return false;
            GrupoProfesional g(id, nombre, descripcion, especialidad);
            std::uint32_t miembros = 0;
            if (!leerConteo(in, miembros)) return false;
            for (std::uint32_t j = 0; j < miembros; ++j) {
                std::int32_t idUsuario = 0;
                if (!leerI32(in, idUsuario)) return false;
                g.agregarMiembro(idUsuario);
            }
            cargada.grupos.agregaFinal(g);
            asegurarContador(cargada.sigGrupo, id);
        }

        // Publicaciones y me gusta.
        if (!leerConteo(in, cantidad)) return false;
        for (std::uint32_t i = 0; i < cantidad; ++i) {
            std::int32_t id = 0, idAutor = 0;
            std::string texto, fecha;
            if (!leerI32(in, id) || !leerI32(in, idAutor) || !leerCadena(in, texto) ||
                !leerCadena(in, fecha)) return false;
            Publicacion p(id, idAutor, texto, fecha);
            std::uint32_t likes = 0;
            if (!leerConteo(in, likes)) return false;
            for (std::uint32_t j = 0; j < likes; ++j) {
                std::int32_t idUsuario = 0;
                if (!leerI32(in, idUsuario)) return false;
                p.darMeGusta(idUsuario);
            }
            cargada.publicaciones.agregaFinal(p);
            asegurarContador(cargada.sigPublicacion, id);
        }

        // Comentarios.
        if (!leerConteo(in, cantidad)) return false;
        for (std::uint32_t i = 0; i < cantidad; ++i) {
            std::int32_t id = 0, idAutor = 0, idPublicacion = 0, idPadre = 0;
            std::string texto, fecha;
            if (!leerI32(in, id) || !leerI32(in, idAutor) || !leerI32(in, idPublicacion) ||
                !leerI32(in, idPadre) || !leerCadena(in, texto) || !leerCadena(in, fecha))
                return false;
            cargada.comentarios.agregaFinal(Comentario(id, idAutor, idPublicacion, texto, fecha, idPadre));
            asegurarContador(cargada.sigComentario, id);
        }

        // Recomendaciones.
        if (!leerConteo(in, cantidad)) return false;
        for (std::uint32_t i = 0; i < cantidad; ++i) {
            std::int32_t id = 0, idEmisor = 0, idReceptor = 0;
            std::string texto, fecha;
            if (!leerI32(in, id) || !leerI32(in, idEmisor) || !leerI32(in, idReceptor) ||
                !leerCadena(in, texto) || !leerCadena(in, fecha)) return false;
            cargada.recomendaciones.agregaFinal(Recomendacion(id, idEmisor, idReceptor, texto, fecha));
            asegurarContador(cargada.sigRecomendacion, id);
        }

        // Mensajes.
        if (!leerConteo(in, cantidad)) return false;
        for (std::uint32_t i = 0; i < cantidad; ++i) {
            std::int32_t id = 0, idEmisor = 0, idReceptor = 0;
            std::string texto, fecha;
            bool leido = false;
            if (!leerI32(in, id) || !leerI32(in, idEmisor) || !leerI32(in, idReceptor) ||
                !leerCadena(in, texto) || !leerCadena(in, fecha) || !leerBool(in, leido))
                return false;
            Mensaje m(id, idEmisor, idReceptor, texto, fecha);
            if (leido) m.marcarLeido();
            cargada.mensajes.agregaFinal(m);
            asegurarContador(cargada.sigMensaje, id);
        }

        // Si el archivo tiene bytes extra no es un error: permite ampliar el formato
        // manteniendo compatibilidad hacia delante dentro de una misma version.
        copiarEstado(red, cargada);
        return true;
    }


    // ---------- Compatibilidad con el formato de texto anterior ----------
    // Solo se usa una vez si no existe chambalink.bin y se encuentran los
    // antiguos usuarios.csv o empresas.csv. Despues se migra al binario.
    // ---------- Apoyo para leer ----------

    // Separa una linea por el separador y devuelve los campos en una lista.
    // Se recorre caracter por caracter (y no con getline) para no perder el
    // ultimo campo cuando esta vacio: "1|Ana|" debe dar 3 campos, no 2.
    static Lista<std::string> partir(std::string linea, char separador = '|') {
        Lista<std::string> campos;
        std::string campo;
        for (char ch : linea) {
            if (ch == separador) { campos.agregaFinal(campo); campo = ""; }
            else if (ch != '\r') campo += ch;
        }
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

    static bool cargarFormatoTextoAnterior(RedProfesional& red) {
        std::ifstream usuarios("usuarios.csv");
        std::ifstream empresas("empresas.csv");
        bool hayCuentas = usuarios.is_open() || empresas.is_open();

        // usuarios.csv: id,correo,contrasena,nombre,apellido,titular,distrito
        std::string linea;
        while (std::getline(usuarios, linea)) {
            if (linea == "") continue;
            Lista<std::string> c = partir(linea, ',');
            if (c.longitud() < 7) continue;
            int id = aEntero(c.obtenerPos(0));
            red.usuarios.agregaFinal(Usuario(id, c.obtenerPos(3), c.obtenerPos(4),
                c.obtenerPos(5), c.obtenerPos(6), c.obtenerPos(1), c.obtenerPos(2)));
            actualizarContador(red.sigUsuario, id);
        }

        // empresas.csv: id,correo,contrasena,nombre,sector,distrito
        while (std::getline(empresas, linea)) {
            if (linea == "") continue;
            Lista<std::string> c = partir(linea, ',');
            if (c.longitud() < 6) continue;
            int id = aEntero(c.obtenerPos(0));
            red.empresas.agregaFinal(Empresa(id, c.obtenerPos(3), c.obtenerPos(4),
                c.obtenerPos(5), c.obtenerPos(1), c.obtenerPos(2)));
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

        return hayCuentas;
    }


public:
    static bool guardarTodo(const RedProfesional& red,
        const std::string& archivo = archivoPredeterminado()) {
        const std::string temporal = archivo + ".tmp";
        std::ofstream out(temporal, std::ios::binary | std::ios::trunc);
        if (!out.is_open()) return false;

        const char firma[8] = { 'C','H','A','M','B','A','B','1' };
        out.write(firma, sizeof(firma));
        escribirU32(out, VERSION);

        // Se guardan los siguientes IDs para no reutilizar identificadores de
        // elementos que pudieron haberse eliminado antes de cerrar el programa.
        escribirI32(out, red.sigUsuario);
        escribirI32(out, red.sigEmpresa);
        escribirI32(out, red.sigVacante);
        escribirI32(out, red.sigPostulacion);
        escribirI32(out, red.sigGrupo);
        escribirI32(out, red.sigPublicacion);
        escribirI32(out, red.sigComentario);
        escribirI32(out, red.sigRecomendacion);
        escribirI32(out, red.sigMensaje);
        escribirI32(out, red.sigSolicitud);
        escribirI32(out, red.sigNotificacion);
        escribirI32(out, red.sigCertificacion);

        escribirU32(out, static_cast<std::uint32_t>(red.usuarios.longitud()));
        red.usuarios.paraCada([&](const Usuario& u) { escribirUsuario(out, u); });

        escribirU32(out, static_cast<std::uint32_t>(red.empresas.longitud()));
        red.empresas.paraCada([&](const Empresa& e) {
            escribirI32(out, e.getId());
            escribirCadena(out, e.getCorreo());
            escribirCadena(out, e.getContrasena());
            escribirCadena(out, e.getNombre());
            escribirCadena(out, e.getSector());
            escribirCadena(out, e.getUbicacion());
        });

        escribirU32(out, static_cast<std::uint32_t>(red.vacantes.longitud()));
        red.vacantes.paraCada([&](const Vacante& v) {
            escribirI32(out, v.getId());
            escribirI32(out, v.getIdEmpresa());
            escribirCadena(out, v.getTitulo());
            escribirCadena(out, v.getDescripcion());
            escribirCadena(out, v.getRequisitos());
            escribirCadena(out, v.getModalidad());
            escribirBool(out, v.estaActiva());
            escribirU32(out, static_cast<std::uint32_t>(v.cantidadPorRevisar()));
            v.paraCadaPostulacionPorRevisar([&](const int& idPostulacion) {
                escribirI32(out, idPostulacion);
            });
        });

        escribirU32(out, static_cast<std::uint32_t>(red.postulaciones.longitud()));
        red.postulaciones.paraCada([&](const Postulacion& p) {
            escribirI32(out, p.getId());
            escribirI32(out, p.getIdUsuario());
            escribirI32(out, p.getIdVacante());
            escribirCadena(out, p.getFecha());
            escribirI32(out, static_cast<std::int32_t>(p.getEstado()));
        });

        escribirU32(out, static_cast<std::uint32_t>(red.grupos.longitud()));
        red.grupos.paraCada([&](const GrupoProfesional& g) {
            escribirI32(out, g.getId());
            escribirCadena(out, g.getNombre());
            escribirCadena(out, g.getDescripcion());
            escribirCadena(out, g.getEspecialidad());
            escribirU32(out, static_cast<std::uint32_t>(g.getCantidadMiembros()));
            g.paraCadaMiembro([&](const int& idUsuario) { escribirI32(out, idUsuario); });
        });

        escribirU32(out, static_cast<std::uint32_t>(red.publicaciones.longitud()));
        red.publicaciones.paraCada([&](const Publicacion& p) {
            escribirI32(out, p.getId());
            escribirI32(out, p.getIdAutor());
            escribirCadena(out, p.getTexto());
            escribirCadena(out, p.getFecha());
            escribirU32(out, static_cast<std::uint32_t>(p.getMeGusta()));
            p.paraCadaMeGusta([&](const int& idUsuario) { escribirI32(out, idUsuario); });
        });

        escribirU32(out, static_cast<std::uint32_t>(red.comentarios.longitud()));
        red.comentarios.paraCada([&](const Comentario& c) {
            escribirI32(out, c.getId());
            escribirI32(out, c.getIdAutor());
            escribirI32(out, c.getIdPublicacion());
            escribirI32(out, c.getIdPadre());
            escribirCadena(out, c.getTexto());
            escribirCadena(out, c.getFecha());
        });

        escribirU32(out, static_cast<std::uint32_t>(red.recomendaciones.longitud()));
        red.recomendaciones.paraCada([&](const Recomendacion& r) {
            escribirI32(out, r.getId());
            escribirI32(out, r.getIdEmisor());
            escribirI32(out, r.getIdReceptor());
            escribirCadena(out, r.getTexto());
            escribirCadena(out, r.getFecha());
        });

        escribirU32(out, static_cast<std::uint32_t>(red.mensajes.longitud()));
        red.mensajes.paraCada([&](const Mensaje& m) {
            escribirI32(out, m.getId());
            escribirI32(out, m.getIdEmisor());
            escribirI32(out, m.getIdReceptor());
            escribirCadena(out, m.getTexto());
            escribirCadena(out, m.getFecha());
            escribirBool(out, m.fueLeido());
        });

        out.flush();
        bool correcto = out.good();
        out.close();
        if (!correcto) {
            std::remove(temporal.c_str());
            return false;
        }

        // Reemplazo al final para evitar dejar un archivo principal incompleto
        // si ocurre un error durante la escritura.
        std::remove(archivo.c_str());
        if (std::rename(temporal.c_str(), archivo.c_str()) != 0) {
            std::remove(temporal.c_str());
            return false;
        }
        return true;
    }

    static bool cargarTodo(RedProfesional& red,
        const std::string& archivo = archivoPredeterminado()) {
        std::ifstream in(archivo, std::ios::binary);
        if (in.is_open())
            return leerArchivo(in, red);

        // Compatibilidad con la version previa del proyecto: si no existe aun
        // chambalink.bin, intenta recuperar los CSV/TXT y los migra al binario.
        // Solo se invoca el cargador viejo si realmente existe al menos un
        // archivo de cuentas, para no mezclar archivos de datos huerfanos.
        std::ifstream usuariosAnteriores("usuarios.csv");
        std::ifstream empresasAnteriores("empresas.csv");
        if (!usuariosAnteriores.is_open() && !empresasAnteriores.is_open())
            return false;
        usuariosAnteriores.close();
        empresasAnteriores.close();

        bool cargoFormatoAnterior = cargarFormatoTextoAnterior(red);
        if (cargoFormatoAnterior)
            guardarTodo(red, archivo);
        return cargoFormatoAnterior;
    }
};
