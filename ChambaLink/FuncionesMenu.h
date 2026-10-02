#pragma once
#include <iostream>
#include <string>
#include "FuncionesInicio.h"
#include "RedProfesional.h"
#include "GestorArchivos.h"

using std::cout;
using std::string;

// Menu despues de iniciar sesion (individuo y empresa).
// Mismo estilo que FuncionesInicio.h: funciones sueltas y posiciones fijas.
//
// Distribucion (30 filas x 120 columnas):
//   filas 1-4   logo pequeno              arriba a la derecha: fecha y notificaciones
//   filas 6-8   nombre, titular (o sector y distrito) e ID
//   fila 9      linea separadora
//   filas 11+   menu lateral con flecha en la columna 1
//   columnas 55 a 118, filas 11 a 25: zona de contenido de cada seccion
//   fila 28     pie con las teclas
//
// Cada funcion esta escrita antes de la que la usa.

// ======================= Partes comunes =======================

void dibujarLogoPequeno() {
    ubicar(1, 1); cout << "  ___ _               _          _    _      _";
    ubicar(1, 2); cout << " / __| |_  __ _ _ __ | |__  __ _| |  (_)_ _ | |__";
    ubicar(1, 3); cout << "| (__| ' \\/ _` | '  \\| '_ \\/ _` | |__| | ' \\| / /";
    ubicar(1, 4); cout << " \\___|_||_\\__,_|_|_|_|_.__/\\__,_|____|_|_||_|_\\_\\";
}

void dibujarPie() {
    ubicar(1, 28); cout << "Flechas: moverse     ENTER: abrir     ESC: volver";
}

// Borra la zona de contenido (columnas 55 a 118, filas 11 a 25).
void limpiarZonaContenido() {
    for (int y = 11; y <= 25; y++) {
        ubicar(55, y);
        cout << string(64, ' ');
    }
}

// Pantalla provisional mientras una seccion no esta programada.
// Espera ESC para volver al menu.
void mostrarSeccionPendiente(string titulo) {
    limpiarZonaContenido();
    ubicar(55, 11); cout << titulo;
    ubicar(55, 13); cout << "Seccion en construccion";
    while (leerTecla() != TECLA_ESC) {}
}

// Error en rojo dentro de la zona de contenido.
// Devuelve true con ENTER (intentar de nuevo) y false con ESC (cancelar).
bool mostrarErrorZona(string mensaje) {
    colorRojo();
    ubicar(55, 22); cout << mensaje;
    colorBlanco();
    ubicar(55, 23); cout << "ENTER: intentar de nuevo     ESC: cancelar";
    while (true) {
        int tecla = leerTecla();
        if (tecla == TECLA_ENTER) return true;
        if (tecla == TECLA_ESC) return false;
    }
}

// ======================= Individuo: Mi perfil =======================

// Si el texto no cabe en el ancho, lo corta y termina en "..."
string recortar(string texto, int ancho) {
    if ((int)texto.length() <= ancho) return texto;
    return texto.substr(0, ancho - 3) + "...";
}

// Si un dato opcional esta vacio, se muestra "(no registrado)".
string valorOVacio(string valor) {
    if (valor == "") return "(no registrado)";
    return valor;
}

// Habilidades del usuario en una sola linea: "C++, SQL, Python"
string textoHabilidades(RedProfesional& red, int idUsuario) {
    const Usuario* usuario = red.buscarUsuario(idUsuario);
    if (usuario == nullptr) return "";
    string texto = "";
    usuario->paraCadaHabilidad([&texto](const Habilidad& h) {
        if (texto != "") texto = texto + ", ";
        texto = texto + h.getNombre();
        });
    return texto;
}

// Datos de un usuario. Solo lectura; ESC vuelve. La contrasena no se muestra.
// La usan "Mi perfil" y la empresa al ver un profesional.
void mostrarPerfil(RedProfesional& red, int idUsuario, string titulo) {
    const Usuario* usuario = red.buscarUsuario(idUsuario);
    if (usuario == nullptr) return;

    limpiarZonaContenido();
    ubicar(55, 11); cout << titulo;
    ubicar(55, 12); cout << "----------------------------------------------------------------";
    ubicar(55, 14); cout << "Nombre:";      ubicar(68, 14); cout << usuario->getNombre();
    ubicar(55, 15); cout << "Apellido:";    ubicar(68, 15); cout << usuario->getApellido();
    ubicar(55, 16); cout << "Titular:";     ubicar(68, 16); cout << valorOVacio(usuario->getTitular());
    ubicar(55, 17); cout << "Distrito:";    ubicar(68, 17); cout << valorOVacio(usuario->getUbicacion());
    ubicar(55, 18); cout << "Correo:";      ubicar(68, 18); cout << usuario->getCorreo();
    ubicar(55, 19); cout << "ID:";          ubicar(68, 19); cout << usuario->getId();
    ubicar(55, 20); cout << "Habilidades:"; ubicar(68, 20); cout << recortar(valorOVacio(textoHabilidades(red, idUsuario)), 50);

    while (leerTecla() != TECLA_ESC) {}
}

void seccionMiPerfil(RedProfesional& red, int idSesion) {
    mostrarPerfil(red, idSesion, "MI PERFIL");
}

// ======================= Individuo: Mis certificaciones =======================

// Formulario en la zona de contenido. Al guardar (o con ESC) vuelve a la lista.
void formularioCertificacion(RedProfesional& red, int idSesion) {
    string nombre, institucion, fecha, codigo;
    while (true) {
        limpiarZonaContenido();
        ubicar(55, 11); cout << "AGREGAR CERTIFICACION";
        ubicar(55, 12); cout << "----------------------------------------------------------------";
        ubicar(55, 13); cout << "Nombre:";
        ubicar(55, 15); cout << "Institucion:";
        ubicar(55, 17); cout << "Fecha (aaaa/mm/dd):";
        ubicar(55, 19); cout << "Codigo (opcional):";

        if (!leerCampoEn(76, 13, RedProfesional::MAX_CERTIFICACION, nombre)) return;
        if (!leerCampoEn(76, 15, RedProfesional::MAX_INSTITUCION, institucion)) return;
        if (!leerCampoEn(76, 17, 10, fecha)) return;
        if (!leerCampoEn(76, 19, RedProfesional::MAX_CODIGO, codigo)) return;

        if (red.agregarCertificacion(idSesion, nombre, institucion, fecha, codigo)) {
            GestorArchivos::guardarTodo(red);   // se guarda enseguida
            return;
        }
        if (!mostrarErrorZona(red.getUltimoError())) return;
    }
}

// Lista de certificaciones en orden cronologico (MergeSort), 3 por pagina,
// y debajo la opcion Agregar certificacion.
//   <- ->  cambiar de pagina     ENTER  agregar     ESC  volver al menu
void seccionMisCertificaciones(RedProfesional& red, int idSesion) {
    int pagina = 0;
    while (true) {
        // Se ordena cada vez que se muestra, por si se acaba de agregar una.
        Lista<Certificacion> lista = red.certificacionesOrdenadas(idSesion);
        int total = (int)lista.longitud();
        int paginas = (total + 2) / 3;          // 3 certificaciones por pagina
        if (pagina >= paginas && paginas > 0) pagina = paginas - 1;

        limpiarZonaContenido();
        ubicar(55, 11); cout << "MIS CERTIFICACIONES (" << total << ")";
        if (paginas > 1) cout << "     Pagina " << pagina + 1 << " de " << paginas << "  (<- ->)";
        ubicar(55, 12); cout << "----------------------------------------------------------------";

        if (total == 0) {
            ubicar(55, 13); cout << "Aun no tienes certificaciones";
        }

        // Cada certificacion ocupa 3 filas y deja 1 libre: filas 13, 17 y 21.
        int i = 0;
        for (Certificacion& c : lista) {
            if (i >= pagina * 3 && i < pagina * 3 + 3) {
                int y = 13 + (i - pagina * 3) * 4;
                ubicar(55, y);     cout << c.getNombre();
                ubicar(57, y + 1); cout << c.getInstitucion() << " - " << c.getFechaObtencion();
                ubicar(57, y + 2); cout << "Codigo: " << (c.getCodigoCredencial() == "" ? "-" : c.getCodigoCredencial());
            }
            i++;
        }

        ubicar(55, 25); cout << "-> Agregar certificacion";

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        if (tecla == TECLA_ENTER) formularioCertificacion(red, idSesion);
        else if (tecla == TECLA_IZQUIERDA && pagina > 0) pagina--;
        else if (tecla == TECLA_DERECHA && pagina < paginas - 1) pagina++;
    }
}

// ======================= Individuo: Publicaciones =======================
// Todas las publicaciones de la red. Se conectan por ids:
//   publicaciones.txt  id | idAutor | texto | fecha
//   comentarios.txt    id | idAutor | idPublicacion | idPadre | texto | fecha
//   me_gusta.txt       idPublicacion | idUsuario

// Escribe el texto de una publicacion en dos lineas.
// Si no cabe en una, corta en el ultimo espacio antes de la columna 60
// para no partir una palabra por la mitad.
void escribirTextoPublicacion(string texto, int x, int y) {
    if (texto.length() <= 60) {
        ubicar(x, y); cout << texto;
        return;
    }
    size_t corte = texto.rfind(' ', 60);
    if (corte == string::npos || corte == 0) corte = 60;
    ubicar(x, y);     cout << texto.substr(0, corte);
    ubicar(x, y + 1); cout << recortar(texto.substr(corte + (texto[corte] == ' ' ? 1 : 0)), 61);
}

void formularioPublicar(RedProfesional& red, int idSesion) {
    string linea1, linea2;
    while (true) {
        limpiarZonaContenido();
        ubicar(55, 11); cout << "NUEVA PUBLICACION";
        ubicar(55, 12); cout << "----------------------------------------------------------------";
        ubicar(55, 14); cout << "Linea 1:";
        ubicar(55, 17); cout << "Linea 2 (opcional):";

        if (!leerCampoEn(55, 15, 60, linea1)) return;
        if (!leerCampoEn(55, 18, 60, linea2)) return;

        string texto = linea1;
        if (linea2 != "") texto = texto + " " + linea2;

        if (red.publicar(idSesion, texto) != -1) {
            GestorArchivos::guardarTodo(red);
            return;
        }
        if (!mostrarErrorZona(red.getUltimoError())) return;
    }
}

// Comentario nuevo (idPadre = 0) o respuesta a un comentario (idPadre = su id).
void formularioComentario(RedProfesional& red, int idSesion, int idPublicacion, int idPadre, string titulo) {
    string texto;
    while (true) {
        limpiarZonaContenido();
        ubicar(55, 11); cout << recortar(titulo, 64);
        ubicar(55, 12); cout << "----------------------------------------------------------------";
        ubicar(55, 14); cout << "Comentario:";

        if (!leerCampoEn(55, 15, RedProfesional::MAX_COMENTARIO, texto)) return;

        if (red.comentar(idSesion, idPublicacion, texto, idPadre) != -1) {
            GestorArchivos::guardarTodo(red);
            return;
        }
        if (!mostrarErrorZona(red.getUltimoError())) return;
    }
}

// Detalle de una publicacion: me gusta, comentar y el hilo de comentarios.
// Opciones: 0 = Me gusta / Quitar me gusta, 1 = Comentar,
//           2 en adelante = comentarios de la pagina (ENTER responde a ese comentario).
void verPublicacion(RedProfesional& red, int idSesion, int idPublicacion) {
    int seleccion = 0;
    int pagina = 0;
    while (true) {
        const Publicacion* p = red.obtenerPublicacion(idPublicacion);
        if (p == nullptr) return;

        // El hilo se arma con la funcion recursiva y se guarda en dos listas:
        // el texto de cada linea (con sangria segun el nivel) y el id del comentario.
        Lista<string> lineas;
        Lista<int> ids;
        red.paraCadaComentarioEnHilo(idPublicacion, [&](const Comentario& c, int nivel) {
            string linea = string(nivel * 3, ' ');
            if (nivel > 0) linea = linea + "|_ ";
            linea = linea + red.nombreDe(c.getIdAutor()) + ": " + c.getTexto();
            lineas.agregaFinal(recortar(linea, 61));
            ids.agregaFinal(c.getId());
            });

        int total = (int)lineas.longitud();
        int paginas = (total + 5) / 6;          // 6 comentarios por pagina
        if (pagina >= paginas && paginas > 0) pagina = paginas - 1;
        int enPagina = total - pagina * 6;
        if (enPagina > 6) enPagina = 6;
        if (seleccion > 1 + enPagina) seleccion = 1 + enPagina;

        limpiarZonaContenido();
        ubicar(55, 11); cout << recortar(red.nombreDe(p->getIdAutor()) + " - " + p->getFecha(), 64);
        ubicar(55, 12); cout << "----------------------------------------------------------------";
        escribirTextoPublicacion(p->getTexto(), 55, 13);
        ubicar(55, 15); cout << p->getMeGusta() << " me gusta     " << total << " comentarios";

        ubicar(58, 17); cout << (p->leGustaA(idSesion) ? "Quitar me gusta" : "Me gusta");
        ubicar(58, 18); cout << "Comentar";

        ubicar(55, 19); cout << "COMENTARIOS";
        if (paginas > 1) cout << "   Pagina " << pagina + 1 << " de " << paginas << "  (<- ->)";

        // Comentarios de la pagina actual en las filas 20 a 25.
        int i = 0;
        for (string& linea : lineas) {
            if (i >= pagina * 6 && i < pagina * 6 + 6) {
                ubicar(58, 20 + (i - pagina * 6)); cout << linea;
            }
            i++;
        }

        // Flecha: opciones 0 y 1 en las filas 17 y 18; comentarios desde la fila 20.
        int filaFlecha = (seleccion < 2) ? 17 + seleccion : 20 + (seleccion - 2);
        ubicar(55, filaFlecha); cout << "->";

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        else if (tecla == TECLA_ARRIBA && seleccion > 0) seleccion--;
        else if (tecla == TECLA_ABAJO && seleccion < 1 + enPagina) seleccion++;
        else if (tecla == TECLA_IZQUIERDA && pagina > 0) { pagina--; seleccion = 1; }
        else if (tecla == TECLA_DERECHA && pagina < paginas - 1) { pagina++; seleccion = 1; }
        else if (tecla == TECLA_ENTER) {
            if (seleccion == 0) {
                if (p->leGustaA(idSesion)) red.quitarMeGusta(idSesion, idPublicacion);
                else red.darMeGusta(idSesion, idPublicacion);
                GestorArchivos::guardarTodo(red);
            }
            else if (seleccion == 1) {
                formularioComentario(red, idSesion, idPublicacion, 0, "COMENTAR");
            }
            else {
                int posicion = pagina * 6 + (seleccion - 2);
                int idComentario = ids.obtenerPos(posicion);
                formularioComentario(red, idSesion, idPublicacion, idComentario,
                    "RESPONDER A: " + lineas.obtenerPos(posicion));
            }
        }
    }
}

// Feed de publicaciones. Con soloMias = true muestra solo las del usuario
// (seccion "Mis publicaciones"); con false, todas las de la red.
// Opciones: 0 = Nueva publicacion, 1 = Ordenar por (fecha / popularidad),
//           2 y 3 = las publicaciones de la pagina (ENTER abre el detalle).
void seccionPublicaciones(RedProfesional& red, int idSesion, bool soloMias) {
    bool porPopularidad = false;
    int seleccion = 0;
    int pagina = 0;
    while (true) {
        // MergeSort en cada vuelta: asi aparecen los cambios (nuevas, me gusta...).
        Lista<Publicacion> ordenadas;
        if (porPopularidad) ordenadas = red.feedPorPopularidad();
        else ordenadas = red.feedPorFecha();

        // En "Mis publicaciones" se quedan solo las del usuario (ya ordenadas).
        Lista<Publicacion> feed;
        for (Publicacion& p : ordenadas) {
            if (!soloMias || p.esDeAutor(idSesion)) feed.agregaFinal(p);
        }

        int total = (int)feed.longitud();
        int paginas = (total + 1) / 2;          // 2 publicaciones por pagina
        if (pagina >= paginas && paginas > 0) pagina = paginas - 1;
        int enPagina = total - pagina * 2;
        if (enPagina > 2) enPagina = 2;
        if (seleccion > 1 + enPagina) seleccion = 1 + enPagina;

        limpiarZonaContenido();
        ubicar(55, 11); cout << (soloMias ? "MIS PUBLICACIONES (" : "PUBLICACIONES (") << total << ")";
        if (paginas > 1) cout << "     Pagina " << pagina + 1 << " de " << paginas << "  (<- ->)";
        ubicar(55, 12); cout << "----------------------------------------------------------------";
        ubicar(58, 13); cout << "Nueva publicacion";
        ubicar(58, 14); cout << "Ordenar por: " << (porPopularidad ? "popularidad" : "fecha");

        if (total == 0) {
            ubicar(58, 16); cout << (soloMias ? "Aun no has publicado nada" : "Aun no hay publicaciones");
        }

        // Cada publicacion ocupa 4 filas: autor y fecha, texto (2 lineas) y contadores.
        int idsPagina[2] = { 0, 0 };
        int i = 0;
        for (Publicacion& p : feed) {
            if (i >= pagina * 2 && i < pagina * 2 + 2) {
                int posicion = i - pagina * 2;
                int y = 16 + posicion * 5;
                idsPagina[posicion] = p.getId();
                ubicar(58, y); cout << recortar(red.nombreDe(p.getIdAutor()) + " - " + p.getFecha(), 61);
                escribirTextoPublicacion(p.getTexto(), 58, y + 1);
                ubicar(58, y + 3); cout << p.getMeGusta() << " me gusta     "
                    << red.contarComentarios(p.getId()) << " comentarios";
            }
            i++;
        }

        // Flecha: opciones 0 y 1 en las filas 13 y 14; publicaciones en las filas 16 y 21.
        int filaFlecha = (seleccion < 2) ? 13 + seleccion : 16 + (seleccion - 2) * 5;
        ubicar(55, filaFlecha); cout << "->";

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        else if (tecla == TECLA_ARRIBA && seleccion > 0) seleccion--;
        else if (tecla == TECLA_ABAJO && seleccion < 1 + enPagina) seleccion++;
        else if (tecla == TECLA_IZQUIERDA && pagina > 0) pagina--;
        else if (tecla == TECLA_DERECHA && pagina < paginas - 1) pagina++;
        else if (tecla == TECLA_ENTER) {
            if (seleccion == 0) formularioPublicar(red, idSesion);
            else if (seleccion == 1) { porPopularidad = !porPopularidad; pagina = 0; }
            else verPublicacion(red, idSesion, idsPagina[seleccion - 2]);
        }
    }
}

// ======================= Individuo =======================

void dibujarEncabezadoIndividuo(RedProfesional& red, int idSesion) {
    const Usuario* usuario = red.buscarUsuario(idSesion);
    if (usuario == nullptr) return;

    ubicar(108, 1); cout << RedProfesional::fechaHoy();
    ubicar(100, 2); cout << "Notificaciones (" << usuario->cantidadNotificaciones() << ")";

    ubicar(1, 6); cout << usuario->getNombreCompleto();
    ubicar(1, 7); cout << usuario->getTitular();
    ubicar(1, 8); cout << "ID: " << usuario->getId();
    ubicar(1, 9); cout << "------------------------------------------------";
}

void dibujarOpcionesIndividuo() {
    ubicar(4, 11); cout << "Mi perfil";
    ubicar(4, 12); cout << "Mis certificaciones";
    ubicar(4, 13); cout << "Mis postulaciones";
    ubicar(4, 14); cout << "Ver recomendaciones";
    ubicar(4, 15); cout << "Mi red";
    ubicar(4, 16); cout << "Publicaciones";
    ubicar(4, 17); cout << "Mis publicaciones";
    ubicar(4, 18); cout << "Empleos";
    ubicar(4, 19); cout << "Mensajes";
    ubicar(4, 20); cout << "Notificaciones";
    ubicar(4, 22); cout << "Cerrar sesion";
}

// Fila de cada opcion: 0 a 9 van de la fila 11 a la 20; Cerrar sesion (10) va en la 22.
int filaOpcionIndividuo(int opcion) {
    if (opcion == 10) return 22;
    return 11 + opcion;
}

// Aqui se conecta cada opcion con su seccion.
// Cuando una seccion este lista, se reemplaza su mostrarSeccionPendiente.
void abrirSeccionIndividuo(RedProfesional& red, int idSesion, int opcion) {
    switch (opcion) {
    case 0: seccionMiPerfil(red, idSesion); break;
    case 1: seccionMisCertificaciones(red, idSesion); break;
    case 2: mostrarSeccionPendiente("Mis postulaciones"); break;
    case 3: mostrarSeccionPendiente("Ver recomendaciones"); break;
    case 4: mostrarSeccionPendiente("Mi red"); break;
    case 5: seccionPublicaciones(red, idSesion, false); break;   // todas las de la red
    case 6: seccionPublicaciones(red, idSesion, true); break;    // solo las mias
    case 7: mostrarSeccionPendiente("Empleos"); break;
    case 8: mostrarSeccionPendiente("Mensajes"); break;
    case 9: mostrarSeccionPendiente("Notificaciones"); break;
    default: break;
    }
}

// Menu del individuo. Termina cuando elige Cerrar sesion.
void menuIndividuo(RedProfesional& red, int idSesion) {
    int opcion = 0;
    while (true) {
        // Se dibuja todo de nuevo al volver de una seccion
        // (por si cambio algo del encabezado, como las notificaciones).
        limpiarPantalla();
        dibujarLogoPequeno();
        dibujarEncabezadoIndividuo(red, idSesion);
        dibujarOpcionesIndividuo();
        dibujarPie();

        // Mover la flecha hasta que se presione ENTER.
        while (true) {
            ubicar(1, filaOpcionIndividuo(opcion)); cout << "->";

            int tecla = leerTecla();
            if (tecla == TECLA_ENTER) break;

            int anterior = opcion;
            if (tecla == TECLA_ARRIBA && opcion > 0) opcion--;
            else if (tecla == TECLA_ABAJO && opcion < 10) opcion++;

            // Se borra la flecha de la opcion anterior.
            if (opcion != anterior) {
                ubicar(1, filaOpcionIndividuo(anterior)); cout << "  ";
            }
        }

        if (opcion == 10) return;   // Cerrar sesion: vuelve al inicio de todo
        abrirSeccionIndividuo(red, idSesion, opcion);
    }
}

// ======================= Empresa =======================

void dibujarEncabezadoEmpresa(RedProfesional& red, int idSesion) {
    const Empresa* empresa = red.obtenerEmpresa(idSesion);
    if (empresa == nullptr) return;

    ubicar(108, 1); cout << RedProfesional::fechaHoy();

    ubicar(1, 6); cout << empresa->getNombre();
    ubicar(1, 7); cout << empresa->getSector();
    if (empresa->getSector() != "" && empresa->getUbicacion() != "") cout << " - ";
    cout << empresa->getUbicacion();
    ubicar(1, 8); cout << "ID: " << empresa->getId();
    ubicar(1, 9); cout << "------------------------------------------------";
}

// Espera ENTER o ESC (para avisos que solo hay que leer).
void esperarEnter() {
    while (true) {
        int tecla = leerTecla();
        if (tecla == TECLA_ENTER || tecla == TECLA_ESC) return;
    }
}

// Barra de compatibilidad: [######----] 60%
string barraCompatibilidad(int porcentaje) {
    int llenos = porcentaje / 10;
    return "[" + string(llenos, '#') + string(10 - llenos, '-') + "] " + std::to_string(porcentaje) + "%";
}

// ---------- Publicar vacante ----------

void formularioVacante(RedProfesional& red, int idSesion) {
    string titulo, modalidad, descripcion1, descripcion2, requisitos;
    while (true) {
        limpiarZonaContenido();
        ubicar(55, 11); cout << "PUBLICAR VACANTE";
        ubicar(55, 12); cout << "----------------------------------------------------------------";
        ubicar(55, 13); cout << "Titulo:";
        ubicar(55, 15); cout << "Modalidad:";
        ubicar(55, 17); cout << "Descripcion:";
        ubicar(55, 20); cout << "Requisitos:";
        ubicar(55, 21); cout << "(separados por comas, ej: C++, SQL, Git)";

        if (!leerCampoEn(69, 13, 40, titulo)) return;
        if (!leerCampoEn(69, 15, 20, modalidad)) return;
        if (!leerCampoEn(69, 17, 48, descripcion1)) return;
        if (!leerCampoEn(69, 18, 48, descripcion2)) return;
        if (!leerCampoEn(69, 20, 48, requisitos)) return;

        string descripcion = descripcion1;
        if (descripcion2 != "") descripcion = descripcion + " " + descripcion2;

        if (red.publicarVacante(idSesion, titulo, descripcion, requisitos, modalidad) != -1) {
            GestorArchivos::guardarTodo(red);
            ubicar(55, 23); cout << "Vacante publicada. Presione ENTER para continuar";
            esperarEnter();
            return;
        }
        if (!mostrarErrorZona(red.getUltimoError())) return;
    }
}

// ---------- Revisar postulantes (cola de la vacante) ----------

// Muestra al primero de la cola SIN sacarlo y la empresa decide:
// Aceptar / Rechazar lo sacan de la cola; Revisar despues lo pasa al final (rotar).
void revisarPostulantes(RedProfesional& red, int idSesion, int idVacante) {
    int opcion = 0;
    while (true) {
        const Vacante* vacante = red.obtenerVacante(idVacante);
        const Postulacion* postulacion = red.verSiguientePostulacion(idSesion, idVacante);

        limpiarZonaContenido();
        if (vacante == nullptr) return;
        ubicar(55, 11); cout << recortar(vacante->getTitulo(), 40) << " - Por revisar: " << vacante->cantidadPorRevisar();
        ubicar(55, 12); cout << "----------------------------------------------------------------";

        if (postulacion == nullptr) {
            ubicar(55, 14); cout << "No hay postulantes por revisar";
            while (leerTecla() != TECLA_ESC) {}
            return;
        }

        int idUsuario = postulacion->getIdUsuario();
        const Usuario* usuario = red.buscarUsuario(idUsuario);
        if (usuario == nullptr) return;

        ubicar(55, 14); cout << usuario->getNombreCompleto();
        ubicar(96, 14); cout << "Postulo: " << postulacion->getFecha();
        ubicar(55, 15); cout << recortar(valorOVacio(usuario->getTitular()) + " - " + valorOVacio(usuario->getUbicacion()), 64);
        ubicar(55, 16); cout << "Habilidades: " << recortar(valorOVacio(textoHabilidades(red, idUsuario)), 51);
        ubicar(55, 17); cout << "Compatibilidad: " << barraCompatibilidad(red.calcularCompatibilidad(idUsuario, idVacante));

        ubicar(58, 19); cout << "Aceptar";
        ubicar(58, 20); cout << "Rechazar";
        ubicar(58, 21); cout << "Revisar despues";
        ubicar(55, 19 + opcion); cout << "->";

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        else if (tecla == TECLA_ARRIBA && opcion > 0) opcion--;
        else if (tecla == TECLA_ABAJO && opcion < 2) opcion++;
        else if (tecla == TECLA_ENTER) {
            if (opcion == 0) red.revisarSiguientePostulacion(idSesion, idVacante, true);
            else if (opcion == 1) red.revisarSiguientePostulacion(idSesion, idVacante, false);
            else red.rotarPostulacionesVacante(idVacante);
            GestorArchivos::guardarTodo(red);
            opcion = 0;
        }
    }
}

// ---------- Detalle de una vacante ----------

void detalleVacante(RedProfesional& red, int idSesion, int idVacante) {
    int opcion = 0;
    while (true) {
        const Vacante* vacante = red.obtenerVacante(idVacante);
        if (vacante == nullptr) return;

        limpiarZonaContenido();
        ubicar(55, 11); cout << vacante->getTitulo();
        ubicar(55, 12); cout << "----------------------------------------------------------------";
        ubicar(55, 13); cout << "Modalidad: " << valorOVacio(vacante->getModalidad())
            << "     Estado: " << (vacante->estaActiva() ? "Activa" : "Cancelada");
        escribirTextoPublicacion(vacante->getDescripcion(), 55, 14);
        ubicar(55, 16); cout << "Requisitos: " << recortar(valorOVacio(vacante->getRequisitos()), 52);
        ubicar(55, 17); cout << "Postulantes por revisar: " << vacante->cantidadPorRevisar();

        // Una vacante cancelada solo se puede ver.
        if (!vacante->estaActiva()) {
            while (leerTecla() != TECLA_ESC) {}
            return;
        }

        ubicar(58, 19); cout << "Revisar postulantes";
        ubicar(58, 20); cout << "Cancelar vacante";
        ubicar(55, 19 + opcion); cout << "->";

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        else if (tecla == TECLA_ARRIBA) opcion = 0;
        else if (tecla == TECLA_ABAJO) opcion = 1;
        else if (tecla == TECLA_ENTER) {
            if (opcion == 0) revisarPostulantes(red, idSesion, idVacante);
            else {
                ubicar(55, 22); cout << "Cancelar esta vacante?  ENTER: si     ESC: no";
                if (leerTecla() == TECLA_ENTER) {
                    red.cancelarVacante(idSesion, idVacante);
                    GestorArchivos::guardarTodo(red);
                }
            }
        }
    }
}

// ---------- Mis vacantes (HeapSort por titulo) ----------

void seccionMisVacantes(RedProfesional& red, int idSesion) {
    int seleccion = 0;
    int pagina = 0;
    while (true) {
        red.ordenarVacantesHeap();   // HeapSort de Piero, por titulo

        Lista<Vacante> mias;
        red.paraCadaVacanteDe(idSesion, [&mias](const Vacante& v) { mias.agregaFinal(v); });

        int total = (int)mias.longitud();
        int paginas = (total + 3) / 4;          // 4 vacantes por pagina
        if (pagina >= paginas && paginas > 0) pagina = paginas - 1;
        int enPagina = total - pagina * 4;
        if (enPagina > 4) enPagina = 4;
        if (seleccion >= enPagina) seleccion = enPagina - 1;
        if (seleccion < 0) seleccion = 0;

        limpiarZonaContenido();
        ubicar(55, 11); cout << "MIS VACANTES (" << total << ")";
        if (paginas > 1) cout << "     Pagina " << pagina + 1 << " de " << paginas << "  (<- ->)";
        ubicar(55, 12); cout << "----------------------------------------------------------------";

        if (total == 0) {
            ubicar(55, 14); cout << "Aun no has publicado vacantes";
            while (leerTecla() != TECLA_ESC) {}
            return;
        }

        // Cada vacante ocupa 2 filas y deja 1 libre: filas 14, 17, 20 y 23.
        int idsPagina[4] = { 0, 0, 0, 0 };
        int i = 0;
        for (Vacante& v : mias) {
            if (i >= pagina * 4 && i < pagina * 4 + 4) {
                int posicion = i - pagina * 4;
                int y = 14 + posicion * 3;
                idsPagina[posicion] = v.getId();
                ubicar(58, y);     cout << recortar(v.getTitulo(), 40);
                ubicar(104, y);    cout << (v.estaActiva() ? "Activa" : "Cancelada");
                ubicar(58, y + 1); cout << "Modalidad: " << recortar(valorOVacio(v.getModalidad()), 20)
                    << "   Por revisar: " << v.cantidadPorRevisar();
            }
            i++;
        }
        ubicar(55, 14 + seleccion * 3); cout << "->";

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        else if (tecla == TECLA_ARRIBA && seleccion > 0) seleccion--;
        else if (tecla == TECLA_ABAJO && seleccion < enPagina - 1) seleccion++;
        else if (tecla == TECLA_IZQUIERDA && pagina > 0) { pagina--; seleccion = 0; }
        else if (tecla == TECLA_DERECHA && pagina < paginas - 1) { pagina++; seleccion = 0; }
        else if (tecla == TECLA_ENTER) detalleVacante(red, idSesion, idsPagina[seleccion]);
    }
}

// ---------- Mis contrataciones ----------

void seccionMisContrataciones(RedProfesional& red, int idSesion) {
    int pagina = 0;
    while (true) {
        Lista<Postulacion> contratadas;
        red.paraCadaContratacion(idSesion, [&contratadas](const Postulacion& p) { contratadas.agregaFinal(p); });

        int total = (int)contratadas.longitud();
        int paginas = (total + 3) / 4;          // 4 por pagina
        if (pagina >= paginas && paginas > 0) pagina = paginas - 1;

        limpiarZonaContenido();
        ubicar(55, 11); cout << "MIS CONTRATACIONES (" << total << ")";
        if (paginas > 1) cout << "     Pagina " << pagina + 1 << " de " << paginas << "  (<- ->)";
        ubicar(55, 12); cout << "----------------------------------------------------------------";
        if (total == 0) { ubicar(55, 14); cout << "Aun no tienes contrataciones"; }

        int i = 0;
        for (Postulacion& p : contratadas) {
            if (i >= pagina * 4 && i < pagina * 4 + 4) {
                int y = 14 + (i - pagina * 4) * 3;
                const Vacante* v = red.obtenerVacante(p.getIdVacante());
                ubicar(55, y);     cout << red.nombreDe(p.getIdUsuario());
                ubicar(57, y + 1); cout << recortar((v != nullptr ? v->getTitulo() : string("")) + " - Postulo: " + p.getFecha(), 62);
            }
            i++;
        }

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        else if (tecla == TECLA_IZQUIERDA && pagina > 0) pagina--;
        else if (tecla == TECLA_DERECHA && pagina < paginas - 1) pagina++;
    }
}

// ---------- Buscar profesionales ----------

// Resultados de la busqueda (ya ordenados con HeapSort). ENTER ve el perfil.
void mostrarResultadosBusqueda(RedProfesional& red, Lista<Coincidencia>& resultados, int totalPalabras) {
    int seleccion = 0;
    int pagina = 0;
    int total = (int)resultados.longitud();
    int paginas = (total + 2) / 3;              // 3 por pagina
    while (true) {
        int enPagina = total - pagina * 3;
        if (enPagina > 3) enPagina = 3;

        limpiarZonaContenido();
        ubicar(55, 11); cout << "RESULTADOS: " << total << " profesionales";
        if (paginas > 1) cout << "     Pagina " << pagina + 1 << " de " << paginas << "  (<- ->)";
        ubicar(55, 12); cout << "----------------------------------------------------------------";

        if (total == 0) {
            ubicar(55, 14); cout << "No se encontraron profesionales con esas palabras clave";
            while (leerTecla() != TECLA_ESC) {}
            return;
        }

        // Cada resultado ocupa 3 filas y deja 1 libre: filas 14, 18 y 22.
        int idsPagina[3] = { 0, 0, 0 };
        int i = 0;
        for (Coincidencia& c : resultados) {
            if (i >= pagina * 3 && i < pagina * 3 + 3) {
                int posicion = i - pagina * 3;
                int y = 14 + posicion * 4;
                idsPagina[posicion] = c.idUsuario;
                const Usuario* u = red.buscarUsuario(c.idUsuario);
                if (u != nullptr) {
                    ubicar(58, y);     cout << recortar(u->getNombreCompleto(), 40);
                    ubicar(100, y);    cout << "Coincide: " << c.cantidad << " de " << totalPalabras;
                    ubicar(60, y + 1); cout << recortar(valorOVacio(u->getTitular()) + " - " + valorOVacio(u->getUbicacion()), 58);
                    ubicar(60, y + 2); cout << "Habilidades: " << recortar(valorOVacio(textoHabilidades(red, c.idUsuario)), 45);
                }
            }
            i++;
        }
        ubicar(55, 14 + seleccion * 4); cout << "->";

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        else if (tecla == TECLA_ARRIBA && seleccion > 0) seleccion--;
        else if (tecla == TECLA_ABAJO && seleccion < enPagina - 1) seleccion++;
        else if (tecla == TECLA_IZQUIERDA && pagina > 0) { pagina--; seleccion = 0; }
        else if (tecla == TECLA_DERECHA && pagina < paginas - 1) { pagina++; seleccion = 0; }
        else if (tecla == TECLA_ENTER) mostrarPerfil(red, idsPagina[seleccion], "PERFIL DEL PROFESIONAL");
    }
}

// Pide las palabras clave y muestra los resultados. ESC en los resultados
// vuelve a pedir palabras; ESC en las palabras vuelve al menu.
void seccionBuscarProfesionales(RedProfesional& red) {
    string palabras;
    while (true) {
        limpiarZonaContenido();
        ubicar(55, 11); cout << "BUSCAR PROFESIONALES";
        ubicar(55, 12); cout << "----------------------------------------------------------------";
        ubicar(55, 14); cout << "Palabras clave:";
        ubicar(55, 15); cout << "(separadas por comas, ej: sql, python)";

        if (!leerCampoEn(71, 14, 45, palabras)) return;

        int totalPalabras = 0;
        Lista<Coincidencia> resultados = red.buscarProfesionales(palabras, totalPalabras);
        mostrarResultadosBusqueda(red, resultados, totalPalabras);
    }
}

// ---------- Ver mi empresa ----------

void seccionMiEmpresa(RedProfesional& red, int idSesion) {
    const Empresa* empresa = red.obtenerEmpresa(idSesion);
    if (empresa == nullptr) return;

    int activas = 0, canceladas = 0, contrataciones = 0;
    red.paraCadaVacanteDe(idSesion, [&activas, &canceladas](const Vacante& v) {
        if (v.estaActiva()) activas++;
        else canceladas++;
        });
    red.paraCadaContratacion(idSesion, [&contrataciones](const Postulacion& p) { contrataciones++; });

    limpiarZonaContenido();
    ubicar(55, 11); cout << "MI EMPRESA";
    ubicar(55, 12); cout << "----------------------------------------------------------------";
    ubicar(55, 14); cout << "Nombre:";    ubicar(67, 14); cout << empresa->getNombre();
    ubicar(55, 15); cout << "Sector:";    ubicar(67, 15); cout << valorOVacio(empresa->getSector());
    ubicar(55, 16); cout << "Distrito:";  ubicar(67, 16); cout << valorOVacio(empresa->getUbicacion());
    ubicar(55, 17); cout << "Correo:";    ubicar(67, 17); cout << empresa->getCorreo();
    ubicar(55, 18); cout << "ID:";        ubicar(67, 18); cout << empresa->getId();
    ubicar(55, 20); cout << "Vacantes activas: " << activas << "     Canceladas: " << canceladas
        << "     Contrataciones: " << contrataciones;

    while (leerTecla() != TECLA_ESC) {}
}

// ---------- Menu de empresa ----------

void dibujarOpcionesEmpresa() {
    ubicar(4, 11); cout << "Publicar vacante";
    ubicar(4, 12); cout << "Mis vacantes";
    ubicar(4, 13); cout << "Mis contrataciones";
    ubicar(4, 14); cout << "Buscar profesionales";
    ubicar(4, 15); cout << "Ver mi empresa";
    ubicar(4, 17); cout << "Cerrar sesion";
}

// 0 a 4 van de la fila 11 a la 15; Cerrar sesion (5) va en la 17.
int filaOpcionEmpresa(int opcion) {
    if (opcion == 5) return 17;
    return 11 + opcion;
}

void abrirSeccionEmpresa(RedProfesional& red, int idSesion, int opcion) {
    switch (opcion) {
    case 0: formularioVacante(red, idSesion); break;
    case 1: seccionMisVacantes(red, idSesion); break;
    case 2: seccionMisContrataciones(red, idSesion); break;
    case 3: seccionBuscarProfesionales(red); break;
    case 4: seccionMiEmpresa(red, idSesion); break;
    default: break;
    }
}

void menuEmpresa(RedProfesional& red, int idSesion) {
    int opcion = 0;
    while (true) {
        limpiarPantalla();
        dibujarLogoPequeno();
        dibujarEncabezadoEmpresa(red, idSesion);
        dibujarOpcionesEmpresa();
        dibujarPie();

        while (true) {
            ubicar(1, filaOpcionEmpresa(opcion)); cout << "->";

            int tecla = leerTecla();
            if (tecla == TECLA_ENTER) break;

            int anterior = opcion;
            if (tecla == TECLA_ARRIBA && opcion > 0) opcion--;
            else if (tecla == TECLA_ABAJO && opcion < 5) opcion++;

            if (opcion != anterior) {
                ubicar(1, filaOpcionEmpresa(anterior)); cout << "  ";
            }
        }

        if (opcion == 5) return;    // Cerrar sesion
        abrirSeccionEmpresa(red, idSesion, opcion);
    }
}

// ======================= Punto de entrada =======================

// Se llama desde Main despues del login. Al volver, se regresa al inicio de todo.
void mostrarMenu(RedProfesional& red, int idSesion, bool esEmpresa) {
    if (esEmpresa) menuEmpresa(red, idSesion);
    else menuIndividuo(red, idSesion);
}