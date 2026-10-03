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

// Linea separadora que muestra el ordenamiento usado en la lista de abajo:
//   ---- (QuickSort por nombre) -----------------------------
void separadorConOrden(string ordenamiento) {
    string texto = "---- (" + ordenamiento + ") ";
    ubicar(55, 12); cout << texto << string(64 - texto.length(), '-');
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
    ubicar(55, 21); cout << "Principal:";   ubicar(68, 21); cout << valorOVacio(usuario->habilidadPrincipal());
    ubicar(55, 22); cout << "Experiencia:"; ubicar(68, 22); cout << usuario->totalExperiencias() << " puesto(s), "
        << usuario->antiguedadTotalEnAnios(std::stoi(RedProfesional::fechaHoy().substr(0, 4))) << " anio(s) en total";

    while (leerTecla() != TECLA_ESC) {}
}

// Pide ENTER para confirmar o ESC para cancelar en la fila y.
bool confirmarZona(int y, string pregunta) {
    ubicar(55, y); cout << string(64, ' ');
    ubicar(55, y + 1); cout << string(64, ' ');
    ubicar(55, y); cout << pregunta;
    ubicar(55, y + 1); cout << "ENTER: confirmar     ESC: cancelar";
    while (true) {
        int tecla = leerTecla();
        if (tecla == TECLA_ENTER) return true;
        if (tecla == TECLA_ESC) return false;
    }
}

// ---------- Mis habilidades (Lista simple) ----------

void formularioHabilidad(RedProfesional& red, int idSesion) {
    string nombre, nivel;
    while (true) {
        limpiarZonaContenido();
        ubicar(55, 11); cout << "AGREGAR HABILIDAD";
        ubicar(55, 12); cout << "----------------------------------------------------------------";
        ubicar(55, 14); cout << "Nombre:";
        ubicar(55, 16); cout << "Nivel:";
        ubicar(55, 17); cout << "(1 Basico  2 Intermedio  3 Avanzado  4 Experto)";

        if (!leerCampoEn(68, 14, RedProfesional::MAX_HABILIDAD, nombre)) return;
        if (!leerCampoEn(68, 16, 1, nivel)) return;

        int numero = 0;
        if (nivel != "") numero = nivel[0] - '0';
        if (red.agregarHabilidad(idSesion, nombre, numero)) {
            GestorArchivos::guardarTodo(red);
            return;
        }
        if (!mostrarErrorZona(red.getUltimoError())) return;
    }
}

// Lista de habilidades, 8 por pagina. La ultima opcion es Agregar habilidad.
// ENTER sobre una habilidad la quita (se puede deshacer en Historial de acciones).
void seccionHabilidades(RedProfesional& red, int idSesion) {
    int seleccion = 0;
    int pagina = 0;
    while (true) {
        const Usuario* usuario = red.buscarUsuario(idSesion);
        if (usuario == nullptr) return;

        Lista<Habilidad> lista;
        usuario->paraCadaHabilidad([&lista](const Habilidad& h) { lista.agregaFinal(h); });

        int total = (int)lista.longitud();
        int paginas = total == 0 ? 1 : (total + 7) / 8;
        if (pagina >= paginas) pagina = paginas - 1;
        int enPagina = total - pagina * 8;
        if (enPagina > 8) enPagina = 8;
        if (seleccion > enPagina) seleccion = enPagina;   // enPagina = opcion Agregar

        limpiarZonaContenido();
        ubicar(55, 11); cout << "MIS HABILIDADES (" << total << ")";
        if (paginas > 1) cout << "     Pagina " << pagina + 1 << " de " << paginas << "  (<- ->)";
        ubicar(55, 12); cout << "----------------------------------------------------------------";
        if (total == 0) { ubicar(58, 14); cout << "Aun no registras habilidades"; }

        string nombres[8];
        int i = 0;
        for (Habilidad& h : lista) {
            if (i >= pagina * 8 && i < pagina * 8 + 8) {
                int posicion = i - pagina * 8;
                nombres[posicion] = h.getNombre();
                ubicar(58, 14 + posicion); cout << recortar(h.getNombre(), 30);
                ubicar(92, 14 + posicion); cout << h.nivelToString();
            }
            i++;
        }
        ubicar(58, 23); cout << "Agregar habilidad";
        ubicar(55, seleccion < enPagina ? 14 + seleccion : 23); cout << "->";
        ubicar(55, 25); cout << "ENTER: quitar / agregar     ESC: volver";

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        else if (tecla == TECLA_ARRIBA && seleccion > 0) seleccion--;
        else if (tecla == TECLA_ABAJO && seleccion < enPagina) seleccion++;
        else if (tecla == TECLA_IZQUIERDA && pagina > 0) { pagina--; seleccion = 0; }
        else if (tecla == TECLA_DERECHA && pagina < paginas - 1) { pagina++; seleccion = 0; }
        else if (tecla == TECLA_ENTER) {
            if (seleccion == enPagina) formularioHabilidad(red, idSesion);
            else if (confirmarZona(24, "Quitar " + recortar(nombres[seleccion], 30) + "?")) {
                if (red.eliminarHabilidad(idSesion, nombres[seleccion])) GestorArchivos::guardarTodo(red);
            }
        }
    }
}

// ---------- Mi experiencia laboral (Lista doble) ----------

void formularioExperiencia(RedProfesional& red, int idSesion) {
    string empresa, cargo, inicio, fin;
    while (true) {
        limpiarZonaContenido();
        ubicar(55, 11); cout << "AGREGAR EXPERIENCIA";
        ubicar(55, 12); cout << "----------------------------------------------------------------";
        ubicar(55, 14); cout << "Empresa:";
        ubicar(55, 16); cout << "Cargo:";
        ubicar(55, 18); cout << "Anio inicio:";
        ubicar(55, 20); cout << "Anio fin:";
        ubicar(55, 21); cout << "(vacio si es tu trabajo actual)";

        if (!leerCampoEn(69, 14, 40, empresa)) return;
        if (!leerCampoEn(69, 16, RedProfesional::MAX_CARGO, cargo)) return;
        if (!leerCampoEn(69, 18, 4, inicio)) return;
        if (!leerCampoEn(69, 20, 4, fin)) return;

        if (red.agregarExperiencia(idSesion, empresa, cargo, inicio, fin)) {
            GestorArchivos::guardarTodo(red);
            return;
        }
        if (!mostrarErrorZona(red.getUltimoError())) return;
    }
}

// Historial laboral con el iterador de la lista doble, 4 puestos por pagina.
// La primera opcion cambia el sentido: mas reciente primero (--) o mas antiguo primero (++).
// ENTER sobre un puesto lo elimina (eliminaPos llega desde el extremo mas cercano).
void seccionExperiencia(RedProfesional& red, int idSesion) {
    bool recientePrimero = true;
    int seleccion = 0;   // 0 = cambiar orden, 1..4 = puestos, ultima = agregar
    int pagina = 0;
    int anioActual = std::stoi(RedProfesional::fechaHoy().substr(0, 4));
    while (true) {
        const Usuario* usuario = red.buscarUsuario(idSesion);
        if (usuario == nullptr) return;

        int total = (int)usuario->totalExperiencias();
        int paginas = total == 0 ? 1 : (total + 3) / 4;
        if (pagina >= paginas) pagina = paginas - 1;
        int enPagina = total - pagina * 4;
        if (enPagina > 4) enPagina = 4;
        if (enPagina < 0) enPagina = 0;
        if (seleccion > enPagina + 1) seleccion = enPagina + 1;

        limpiarZonaContenido();
        ubicar(55, 11); cout << "MI EXPERIENCIA (" << total << ")   En curso: " << usuario->cantidadExperienciasActuales();
        if (paginas > 1) cout << "   Pag. " << pagina + 1 << "/" << paginas << " (<- ->)";
        separadorConOrden("Lista doble ordenada por anio de inicio");
        ubicar(58, 13); cout << "Orden: " << (recientePrimero ? "mas reciente primero (iterador --)" : "mas antiguo primero (iterador ++)");
        if (total == 0) { ubicar(58, 15); cout << "Aun no registras experiencia laboral"; }

        // posiciones[] guarda la posicion real en la lista (0 = mas antiguo) para eliminar.
        int posiciones[4] = { 0, 0, 0, 0 };
        int i = 0;
        usuario->paraCadaExperiencia([&](const ExperienciaLaboral& e) {
            if (i >= pagina * 4 && i < pagina * 4 + 4) {
                int fila = i - pagina * 4;
                int y = 15 + fila * 2;
                posiciones[fila] = recientePrimero ? total - 1 - i : i;
                ubicar(58, y); cout << recortar(e.getCargo() + " - " + e.getNombreEmpresa(), 58);
                ubicar(60, y + 1); cout << e.getAnioInicio() << " - ";
                if (e.esActual()) cout << "Actual"; else cout << e.getAnioFin();
                cout << "  (" << e.duracionEnAnios(anioActual) << " anios)";
            }
            i++;
            }, recientePrimero);

        ubicar(58, 23); cout << "Agregar experiencia";
        int filaFlecha = 13;
        if (seleccion >= 1 && seleccion <= enPagina) filaFlecha = 15 + (seleccion - 1) * 2;
        if (seleccion == enPagina + 1) filaFlecha = 23;
        ubicar(55, filaFlecha); cout << "->";
        ubicar(55, 25); cout << "Lista doble     ENTER: elegir     ESC: volver";

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        else if (tecla == TECLA_ARRIBA && seleccion > 0) seleccion--;
        else if (tecla == TECLA_ABAJO && seleccion < enPagina + 1) seleccion++;
        else if (tecla == TECLA_IZQUIERDA && pagina > 0) { pagina--; seleccion = 0; }
        else if (tecla == TECLA_DERECHA && pagina < paginas - 1) { pagina++; seleccion = 0; }
        else if (tecla == TECLA_ENTER) {
            if (seleccion == 0) { recientePrimero = !recientePrimero; pagina = 0; }
            else if (seleccion == enPagina + 1) formularioExperiencia(red, idSesion);
            else if (confirmarZona(24, "Eliminar este puesto?")) {
                if (red.eliminarExperiencia(idSesion, posiciones[seleccion - 1])) GestorArchivos::guardarTodo(red);
            }
        }
    }
}

// ---------- Historial de acciones (Pila) ----------

// Muestra la pila sin desapilar. ENTER deshace la accion del tope.
// Las flechas izquierda/derecha cambian el orden (la copia se invierte con Pila::invertir).
void seccionHistorialAcciones(RedProfesional& red, int idSesion) {
    bool masAntiguaPrimero = false;
    while (true) {
        const Usuario* usuario = red.buscarUsuario(idSesion);
        if (usuario == nullptr) return;

        limpiarZonaContenido();
        ubicar(55, 11); cout << "HISTORIAL DE ACCIONES (" << usuario->totalAcciones() << ")   Red: "
            << usuario->contarAcciones("Red") << "   Perfil: " << usuario->contarAcciones("Perfil");
        ubicar(55, 12); cout << "----------------------------------------------------------------";
        ubicar(55, 13); cout << "Orden: " << (masAntiguaPrimero ? "mas antigua primero (pila invertida)" : "mas reciente primero (tope)");

        if (usuario->totalAcciones() == 0) {
            ubicar(58, 15); cout << "No hay acciones en esta sesion";
        }
        int i = 0;
        usuario->paraCadaAccion([&i](const Accion& a) {
            if (i < 8) {
                ubicar(58, 15 + i); cout << "[" << a.getCategoria() << "] " << recortar(a.getDescripcion(), 50);
            }
            i++;
            }, masAntiguaPrimero);

        ubicar(55, 24); cout << "Pila (LIFO): ENTER deshace la mas reciente";
        ubicar(55, 25); cout << "<- ->: cambiar orden     ESC: volver";

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        else if (tecla == TECLA_IZQUIERDA || tecla == TECLA_DERECHA) masAntiguaPrimero = !masAntiguaPrimero;
        else if (tecla == TECLA_ENTER && usuario->totalAcciones() > 0) {
            limpiarZonaContenido();
            ubicar(55, 11); cout << "DESHACER";
            if (!confirmarZona(14, "Deshacer: " + recortar(usuario->ultimaAccion().getDescripcion(), 50))) continue;
            string descripcion;
            if (red.deshacerUltimaAccion(idSesion, descripcion)) GestorArchivos::guardarTodo(red);
        }
    }
}

// Menu interno de Mi perfil.
void seccionMiPerfil(RedProfesional& red, int idSesion) {
    int opcion = 0;
    const int totalOpciones = 4;
    while (true) {
        const Usuario* usuario = red.buscarUsuario(idSesion);
        if (usuario == nullptr) return;

        limpiarZonaContenido();
        ubicar(55, 11); cout << "MI PERFIL";
        ubicar(55, 12); cout << "----------------------------------------------------------------";
        ubicar(58, 14); cout << "Ver mis datos";
        ubicar(58, 16); cout << "Mis habilidades (" << usuario->totalHabilidades() << ")";
        ubicar(58, 18); cout << "Mi experiencia laboral (" << usuario->totalExperiencias() << ")";
        ubicar(58, 20); cout << "Historial de acciones (" << usuario->totalAcciones() << ")";
        ubicar(55, 14 + opcion * 2); cout << "->";
        ubicar(55, 25); cout << "Lista, Lista doble y Pila     ESC: volver";

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        else if (tecla == TECLA_ARRIBA && opcion > 0) opcion--;
        else if (tecla == TECLA_ABAJO && opcion < totalOpciones - 1) opcion++;
        else if (tecla == TECLA_ENTER) {
            if (opcion == 0) mostrarPerfil(red, idSesion, "MI PERFIL");
            else if (opcion == 1) seccionHabilidades(red, idSesion);
            else if (opcion == 2) seccionExperiencia(red, idSesion);
            else if (opcion == 3) seccionHistorialAcciones(red, idSesion);
        }
    }
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
        separadorConOrden("MergeSort por fecha");

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
// Todas las publicaciones de la red. Comentarios y me gusta se enlazan por ids.

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
        separadorConOrden(porPopularidad ? "MergeSort por me gusta" : "MergeSort por fecha");
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


// ======================= Individuo: Empleos y postulaciones =======================

// Busca la postulacion del usuario para una vacante concreta.
// Devuelve nullptr cuando aun no ha postulado.
const Postulacion* obtenerPostulacionUsuarioVacante(RedProfesional& red, int idUsuario, int idVacante) {
    const Postulacion* encontrada = nullptr;
    red.paraCadaPostulacionDe(idUsuario, [&encontrada, idVacante](const Postulacion& p) {
        if (p.esDeVacante(idVacante)) encontrada = &p;
        });
    return encontrada;
}

// Detalle de una oferta para el usuario. Si la vacante esta activa y todavia
// no postulo, ENTER permite confirmar la postulacion.
void detalleEmpleoUsuario(RedProfesional& red, int idSesion, int idVacante) {
    while (true) {
        const Vacante* vacante = red.obtenerVacante(idVacante);
        if (vacante == nullptr) return;

        const Postulacion* postulacion = obtenerPostulacionUsuarioVacante(red, idSesion, idVacante);
        int compatibilidad = red.calcularCompatibilidad(idSesion, idVacante);

        limpiarZonaContenido();
        ubicar(55, 11); cout << recortar(vacante->getTitulo(), 63);
        ubicar(55, 12); cout << "----------------------------------------------------------------";
        ubicar(55, 13); cout << "Empresa: " << recortar(red.nombreEmpresa(vacante->getIdEmpresa()), 54);
        ubicar(55, 14); cout << "Modalidad: " << valorOVacio(vacante->getModalidad());
        ubicar(55, 15); cout << "Compatibilidad: " << compatibilidad << "%";
        ubicar(55, 16); cout << "Vacante: " << (vacante->estaActiva() ? "Activa" : "Cerrada");

        ubicar(55, 18); cout << "Descripcion:";
        escribirTextoPublicacion(recortar(valorOVacio(vacante->getDescripcion()), 118), 57, 19);
        ubicar(55, 21); cout << "Requisitos: " << recortar(valorOVacio(vacante->getRequisitos()), 51);

        if (postulacion != nullptr) {
            ubicar(55, 23); cout << "Postulacion: " << postulacion->estadoToString()
                << "     Fecha: " << postulacion->getFecha();
            ubicar(55, 24); cout << "ESC: volver";

            while (leerTecla() != TECLA_ESC) {}
            return;
        }

        if (!vacante->estaActiva()) {
            ubicar(55, 23); cout << "Esta vacante ya no recibe postulaciones";
            ubicar(55, 24); cout << "ESC: volver";
            while (leerTecla() != TECLA_ESC) {}
            return;
        }

        ubicar(55, 23); cout << "-> Postular a esta vacante";
        ubicar(55, 24); cout << "ENTER: postular     ESC: volver";

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        if (tecla != TECLA_ENTER) continue;

        limpiarZonaContenido();
        ubicar(55, 14); cout << "CONFIRMAR POSTULACION";
        ubicar(55, 16); cout << recortar(vacante->getTitulo(), 60);
        ubicar(55, 17); cout << recortar(red.nombreEmpresa(vacante->getIdEmpresa()), 60);
        ubicar(55, 20); cout << "ENTER: confirmar     ESC: cancelar";

        int confirmar = leerTecla();
        if (confirmar == TECLA_ESC) continue;
        if (confirmar != TECLA_ENTER) continue;

        if (red.postular(idSesion, idVacante) != -1) {
            GestorArchivos::guardarTodo(red);
            limpiarZonaContenido();
            ubicar(55, 15); cout << "Postulacion realizada correctamente";
            ubicar(55, 17); cout << "ENTER: continuar";
            while (leerTecla() != TECLA_ENTER) {}
        }
        else {
            if (!mostrarErrorZona(red.getUltimoError())) return;
        }
    }
}

// Lista las vacantes activas. ENTER abre el detalle y permite postular.
// Se muestran 4 ofertas por pagina.
void seccionEmpleos(RedProfesional& red, int idSesion) {
    int pagina = 0;
    int seleccion = 0;

    while (true) {
        Lista<Vacante> disponibles;
        red.paraCadaVacanteActiva([&disponibles](const Vacante& v) {
            disponibles.agregaFinal(v);
            });

        int total = (int)disponibles.longitud();
        int paginas = (total + 3) / 4;
        if (paginas == 0) pagina = 0;
        else if (pagina >= paginas) pagina = paginas - 1;

        int inicio = pagina * 4;
        int enPagina = total - inicio;
        if (enPagina > 4) enPagina = 4;
        if (enPagina <= 0) seleccion = 0;
        else if (seleccion >= enPagina) seleccion = enPagina - 1;

        limpiarZonaContenido();
        ubicar(55, 11); cout << "EMPLEOS DISPONIBLES (" << total << ")";
        if (paginas > 1) cout << "     Pagina " << pagina + 1 << " de " << paginas << "  (<- ->)";
        ubicar(55, 12); cout << "----------------------------------------------------------------";

        if (total == 0) {
            ubicar(55, 14); cout << "No hay vacantes activas por el momento";
            ubicar(55, 16); cout << "ESC: volver";
        }

        int idsPagina[4] = { 0, 0, 0, 0 };
        int i = 0;
        for (Vacante& v : disponibles) {
            if (i >= inicio && i < inicio + 4) {
                int pos = i - inicio;
                int y = 14 + pos * 3;
                idsPagina[pos] = v.getId();

                const Postulacion* p = obtenerPostulacionUsuarioVacante(red, idSesion, v.getId());
                ubicar(58, y); cout << recortar(v.getTitulo(), 43);
                if (p != nullptr) cout << "  [" << p->estadoToString() << "]";
                ubicar(58, y + 1); cout << recortar(red.nombreEmpresa(v.getIdEmpresa())
                    + " - " + valorOVacio(v.getModalidad()), 42);
                ubicar(104, y + 1); cout << "Compat. " << red.calcularCompatibilidad(idSesion, v.getId()) << "%";
            }
            i++;
        }

        if (enPagina > 0) {
            ubicar(55, 14 + seleccion * 3); cout << "->";
        }

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        else if (tecla == TECLA_ARRIBA && seleccion > 0) seleccion--;
        else if (tecla == TECLA_ABAJO && seleccion < enPagina - 1) seleccion++;
        else if (tecla == TECLA_IZQUIERDA && pagina > 0) { pagina--; seleccion = 0; }
        else if (tecla == TECLA_DERECHA && pagina < paginas - 1) { pagina++; seleccion = 0; }
        else if (tecla == TECLA_ENTER && enPagina > 0)
            detalleEmpleoUsuario(red, idSesion, idsPagina[seleccion]);
    }
}

// Historial de postulaciones del usuario. Muestra el puesto, empresa,
// fecha y estado actual. ENTER abre nuevamente el detalle de la vacante.
void seccionMisPostulaciones(RedProfesional& red, int idSesion) {
    int pagina = 0;
    int seleccion = 0;

    while (true) {
        Lista<Postulacion> mias;
        red.paraCadaPostulacionDe(idSesion, [&mias](const Postulacion& p) {
            mias.agregaFinal(p);
            });

        int total = (int)mias.longitud();
        int paginas = (total + 3) / 4;
        if (paginas == 0) pagina = 0;
        else if (pagina >= paginas) pagina = paginas - 1;

        int inicio = pagina * 4;
        int enPagina = total - inicio;
        if (enPagina > 4) enPagina = 4;
        if (enPagina <= 0) seleccion = 0;
        else if (seleccion >= enPagina) seleccion = enPagina - 1;

        limpiarZonaContenido();
        ubicar(55, 11); cout << "MIS POSTULACIONES (" << total << ")";
        if (paginas > 1) cout << "     Pagina " << pagina + 1 << " de " << paginas << "  (<- ->)";
        ubicar(55, 12); cout << "----------------------------------------------------------------";

        if (total == 0) {
            ubicar(55, 14); cout << "Aun no has postulado a ninguna vacante";
            ubicar(55, 16); cout << "Puedes hacerlo desde la seccion Empleos";
        }

        int idsVacante[4] = { 0, 0, 0, 0 };
        int i = 0;
        for (Postulacion& p : mias) {
            if (i >= inicio && i < inicio + 4) {
                int pos = i - inicio;
                int y = 14 + pos * 3;
                const Vacante* v = red.obtenerVacante(p.getIdVacante());
                idsVacante[pos] = p.getIdVacante();

                string titulo = (v != nullptr ? v->getTitulo() : string("Vacante no disponible"));
                string empresa = (v != nullptr ? red.nombreEmpresa(v->getIdEmpresa()) : string("Empresa no disponible"));

                ubicar(58, y); cout << recortar(titulo, 42) << "  [" << p.estadoToString() << "]";
                ubicar(58, y + 1); cout << recortar(empresa + " - Postulo: " + p.getFecha(), 58);
            }
            i++;
        }

        if (enPagina > 0) {
            ubicar(55, 14 + seleccion * 3); cout << "->";
        }

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        else if (tecla == TECLA_ARRIBA && seleccion > 0) seleccion--;
        else if (tecla == TECLA_ABAJO && seleccion < enPagina - 1) seleccion++;
        else if (tecla == TECLA_IZQUIERDA && pagina > 0) { pagina--; seleccion = 0; }
        else if (tecla == TECLA_DERECHA && pagina < paginas - 1) { pagina++; seleccion = 0; }
        else if (tecla == TECLA_ENTER && enPagina > 0 && idsVacante[seleccion] != 0)
            detalleEmpleoUsuario(red, idSesion, idsVacante[seleccion]);
    }
}

// ======================= Individuo: Recomendaciones =======================

void avisoRecomendaciones(string mensaje) {
    limpiarZonaContenido();
    ubicar(55, 15); cout << mensaje;
    ubicar(55, 18); cout << "ENTER: continuar     ESC: volver";
    while (true) {
        int tecla = leerTecla();
        if (tecla == TECLA_ENTER || tecla == TECLA_ESC) return;
    }
}

// Muestra una recomendacion completa. Solo se abre desde las listas recibidas
// o enviadas del usuario que tiene la sesion activa.
void detalleRecomendacion(RedProfesional& red, int idSesion, int idRecomendacion) {
    const Recomendacion* r = red.obtenerRecomendacion(idRecomendacion);
    if (r == nullptr) return;
    if (r->getIdEmisor() != idSesion && r->getIdReceptor() != idSesion) return;

    limpiarZonaContenido();
    ubicar(55, 11); cout << "DETALLE DE RECOMENDACION";
    ubicar(55, 12); cout << "----------------------------------------------------------------";
    ubicar(55, 14); cout << "De:   " << recortar(red.nombreDe(r->getIdEmisor()), 55);
    ubicar(55, 15); cout << "Para: " << recortar(red.nombreDe(r->getIdReceptor()), 55);
    ubicar(55, 16); cout << "Fecha: " << r->getFecha();
    ubicar(55, 18); cout << "Recomendacion:";
    escribirTextoPublicacion(r->getTexto(), 55, 19);
    ubicar(55, 23); cout << "ESC: volver";

    while (leerTecla() != TECLA_ESC) {}
}

// Recibidas o enviadas, ordenadas por fecha descendente desde RedProfesional.
// Se muestran tres por pagina y ENTER abre el texto completo.
void listaRecomendaciones(RedProfesional& red, int idSesion, bool recibidas) {
    int pagina = 0;
    int seleccion = 0;
    const int porPagina = 3;

    while (true) {
        Lista<Recomendacion> lista = recibidas
            ? red.recomendacionesRecibidas(idSesion)
            : red.recomendacionesEnviadas(idSesion);

        int total = (int)lista.longitud();
        int paginas = total == 0 ? 1 : (total + porPagina - 1) / porPagina;
        if (pagina >= paginas) pagina = paginas - 1;
        if (pagina < 0) pagina = 0;

        int inicio = pagina * porPagina;
        int enPagina = total - inicio;
        if (enPagina > porPagina) enPagina = porPagina;
        if (enPagina <= 0) seleccion = 0;
        else if (seleccion >= enPagina) seleccion = enPagina - 1;

        limpiarZonaContenido();
        ubicar(55, 11); cout << (recibidas ? "RECOMENDACIONES RECIBIDAS (" : "RECOMENDACIONES ENVIADAS (") << total << ")";
        if (paginas > 1) cout << "  Pag. " << pagina + 1 << "/" << paginas;
        separadorConOrden("MergeSort por fecha");

        if (total == 0) {
            ubicar(55, 15); cout << (recibidas ? "Aun no has recibido recomendaciones" : "Aun no has escrito recomendaciones");
        }

        int idsPagina[3] = { 0, 0, 0 };
        for (int i = 0; i < enPagina; i++) {
            const Recomendacion& r = lista.obtenerPos(inicio + i);
            idsPagina[i] = r.getId();
            int y = 14 + i * 4;
            string persona = recibidas ? red.nombreDe(r.getIdEmisor()) : red.nombreDe(r.getIdReceptor());
            ubicar(58, y); cout << (recibidas ? "De: " : "Para: ") << recortar(persona, 48);
            ubicar(58, y + 1); cout << recortar(r.getTexto(), 57);
            ubicar(58, y + 2); cout << r.getFecha();
        }

        if (enPagina > 0) ubicar(55, 14 + seleccion * 4), cout << "->";
        ubicar(55, 25); cout << "ENTER: ver detalle   <- -> pagina   ESC: volver";

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        else if (tecla == TECLA_ARRIBA && seleccion > 0) seleccion--;
        else if (tecla == TECLA_ABAJO && seleccion < enPagina - 1) seleccion++;
        else if (tecla == TECLA_IZQUIERDA && pagina > 0) { pagina--; seleccion = 0; }
        else if (tecla == TECLA_DERECHA && pagina < paginas - 1) { pagina++; seleccion = 0; }
        else if (tecla == TECLA_ENTER && enPagina > 0)
            detalleRecomendacion(red, idSesion, idsPagina[seleccion]);
    }
}

// Formulario de dos lineas, igual que una publicacion, para no desbordar la
// consola. RedProfesional valida otra vez la relacion y la longitud.
void formularioRecomendacion(RedProfesional& red, int idSesion, int idContacto) {
    const Usuario* contacto = red.buscarUsuario(idContacto);
    if (contacto == nullptr) return;

    while (true) {
        string linea1, linea2;
        limpiarZonaContenido();
        ubicar(55, 11); cout << "ESCRIBIR RECOMENDACION";
        ubicar(55, 12); cout << "----------------------------------------------------------------";
        ubicar(55, 13); cout << "Para: " << recortar(contacto->getNombreCompleto(), 55);
        ubicar(55, 15); cout << "Linea 1:";
        ubicar(55, 18); cout << "Linea 2 (opcional):";

        if (!leerCampoEn(55, 16, 60, linea1)) return;
        if (!leerCampoEn(55, 19, 60, linea2)) return;

        string texto = linea1;
        if (linea2 != "") texto = texto + " " + linea2;

        limpiarZonaContenido();
        ubicar(55, 14); cout << "Enviar recomendacion a " << recortar(contacto->getNombreCompleto(), 36) << "?";
        ubicar(55, 17); escribirTextoPublicacion(texto, 55, 17);
        ubicar(55, 21); cout << "ENTER: confirmar     ESC: cancelar";
        if (leerTecla() != TECLA_ENTER) return;

        if (red.recomendar(idSesion, idContacto, texto) != -1) {
            GestorArchivos::guardarTodo(red);
            avisoRecomendaciones("Recomendacion enviada correctamente");
            return;
        }
        if (!mostrarErrorZona(red.getUltimoError())) return;
    }
}

// Seleccion de un contacto directo. Se reutiliza la lista alfabetica que ya
// usa QuickSort en el modulo Mi red.
void seleccionarContactoParaRecomendar(RedProfesional& red, int idSesion) {
    int pagina = 0;
    int seleccion = 0;
    const int porPagina = 4;

    while (true) {
        Lista<int> contactos = red.contactosOrdenadosPorNombre(idSesion);
        int total = (int)contactos.longitud();
        int paginas = total == 0 ? 1 : (total + porPagina - 1) / porPagina;
        if (pagina >= paginas) pagina = paginas - 1;

        int inicio = pagina * porPagina;
        int enPagina = total - inicio;
        if (enPagina > porPagina) enPagina = porPagina;
        if (enPagina <= 0) seleccion = 0;
        else if (seleccion >= enPagina) seleccion = enPagina - 1;

        limpiarZonaContenido();
        ubicar(55, 11); cout << "ELEGIR CONTACTO PARA RECOMENDAR";
        separadorConOrden("QuickSort por nombre");

        if (total == 0) {
            ubicar(55, 15); cout << "Necesitas al menos un contacto para recomendarlo";
            ubicar(55, 17); cout << "Puedes agregar contactos desde Mi red";
        }

        int idsPagina[4] = { 0, 0, 0, 0 };
        int i = 0;
        for (int& idContacto : contactos) {
            if (i >= inicio && i < inicio + porPagina) {
                int pos = i - inicio;
                int y = 14 + pos * 3;
                idsPagina[pos] = idContacto;
                const Usuario* u = red.buscarUsuario(idContacto);
                if (u != nullptr) {
                    ubicar(58, y); cout << recortar(u->getNombreCompleto(), 55);
                    ubicar(58, y + 1); cout << recortar(valorOVacio(u->getTitular()), 55);
                }
            }
            i++;
        }

        if (enPagina > 0) ubicar(55, 14 + seleccion * 3), cout << "->";
        ubicar(55, 25); cout << "ENTER: seleccionar   <- -> pagina   ESC: volver";

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        else if (tecla == TECLA_ARRIBA && seleccion > 0) seleccion--;
        else if (tecla == TECLA_ABAJO && seleccion < enPagina - 1) seleccion++;
        else if (tecla == TECLA_IZQUIERDA && pagina > 0) { pagina--; seleccion = 0; }
        else if (tecla == TECLA_DERECHA && pagina < paginas - 1) { pagina++; seleccion = 0; }
        else if (tecla == TECLA_ENTER && enPagina > 0) {
            formularioRecomendacion(red, idSesion, idsPagina[seleccion]);
            return;
        }
    }
}

// Menu del modulo. Las recomendaciones recibidas y enviadas se conservan como
// historial; escribir una nueva solo esta permitido para contactos directos.
void seccionRecomendaciones(RedProfesional& red, int idSesion) {
    int opcion = 0;
    const int totalOpciones = 3;

    while (true) {
        Lista<Recomendacion> recibidas = red.recomendacionesRecibidas(idSesion);
        Lista<Recomendacion> enviadas = red.recomendacionesEnviadas(idSesion);

        limpiarZonaContenido();
        ubicar(55, 11); cout << "RECOMENDACIONES";
        ubicar(55, 12); cout << "----------------------------------------------------------------";
        ubicar(58, 14); cout << "Recibidas (" << recibidas.longitud() << ")";
        ubicar(58, 16); cout << "Enviadas (" << enviadas.longitud() << ")";
        ubicar(58, 18); cout << "Escribir recomendacion";
        ubicar(55, 14 + opcion * 2); cout << "->";
        ubicar(55, 22); cout << "Solo puedes recomendar a contactos directos";
        ubicar(55, 25); cout << "Flechas: moverse   ENTER: abrir   ESC: volver";

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        else if (tecla == TECLA_ARRIBA && opcion > 0) opcion--;
        else if (tecla == TECLA_ABAJO && opcion < totalOpciones - 1) opcion++;
        else if (tecla == TECLA_ENTER) {
            if (opcion == 0) listaRecomendaciones(red, idSesion, true);
            else if (opcion == 1) listaRecomendaciones(red, idSesion, false);
            else seleccionarContactoParaRecomendar(red, idSesion);
        }
    }
}

// ======================= Individuo: Mi red =======================

// Muestra un aviso corto dentro del panel derecho y espera ENTER o ESC.
void avisoRed(string mensaje) {
    limpiarZonaContenido();
    ubicar(55, 15); cout << mensaje;
    ubicar(55, 18); cout << "ENTER: continuar     ESC: volver";
    while (true) {
        int tecla = leerTecla();
        if (tecla == TECLA_ENTER || tecla == TECLA_ESC) return;
    }
}

// Formulario sencillo para enviar una solicitud de conexion.
void formularioSolicitudConexion(RedProfesional& red, int idSesion, int idDestino) {
    const Usuario* destino = red.buscarUsuario(idDestino);
    if (destino == nullptr) return;

    while (true) {
        string mensaje;
        limpiarZonaContenido();
        ubicar(55, 11); cout << "ENVIAR SOLICITUD DE CONEXION";
        ubicar(55, 12); cout << "----------------------------------------------------------------";
        ubicar(55, 14); cout << "A: " << recortar(destino->getNombreCompleto(), 57);
        ubicar(55, 15); cout << recortar(valorOVacio(destino->getTitular()), 60);
        ubicar(55, 17); cout << "Mensaje (opcional, max. 55):";
        ubicar(55, 20); cout << "ENTER: enviar     ESC: cancelar";

        if (!leerCampoEn(55, 18, 55, mensaje)) return;
        if (mensaje == "") mensaje = "Me gustaria conectar contigo";

        if (red.enviarSolicitud(idSesion, idDestino, mensaje)) {
            GestorArchivos::guardarTodo(red);
            avisoRed("Solicitud enviada correctamente a " + recortar(destino->getNombreCompleto(), 36));
            return;
        }
        if (!mostrarErrorZona(red.getUltimoError())) return;
    }
}

// Ficha de una persona dentro de Mi red. Desde aqui se puede enviar una
// solicitud o eliminar el contacto existente.
void detallePersonaRed(RedProfesional& red, int idSesion, int idPersona) {
    while (true) {
        const Usuario* persona = red.buscarUsuario(idPersona);
        if (persona == nullptr) return;

        bool contacto = red.sonContactos(idSesion, idPersona);
        bool pendiente = red.existeSolicitudPendienteEntre(idSesion, idPersona);

        limpiarZonaContenido();
        ubicar(55, 11); cout << "PERFIL DE RED";
        ubicar(55, 12); cout << "----------------------------------------------------------------";
        ubicar(55, 14); cout << "Nombre:";       ubicar(68, 14); cout << recortar(persona->getNombreCompleto(), 48);
        ubicar(55, 15); cout << "Titular:";      ubicar(68, 15); cout << recortar(valorOVacio(persona->getTitular()), 48);
        ubicar(55, 16); cout << "Distrito:";     ubicar(68, 16); cout << recortar(valorOVacio(persona->getUbicacion()), 48);
        ubicar(55, 17); cout << "ID:";           ubicar(68, 17); cout << persona->getId();
        ubicar(55, 19); cout << "Relacion:";
        ubicar(68, 19);
        if (contacto) cout << "Contacto directo";
        else if (pendiente) cout << "Solicitud pendiente";
        else cout << "Sin conexion";

        if (contacto) {
            ubicar(55, 22); cout << "-> Eliminar de mis contactos";
            ubicar(55, 24); cout << "ENTER: eliminar     ESC: volver";
        }
        else if (!pendiente) {
            ubicar(55, 22); cout << "-> Enviar solicitud de conexion";
            ubicar(55, 24); cout << "ENTER: conectar     ESC: volver";
        }
        else {
            ubicar(55, 22); cout << "Ya existe una solicitud pendiente entre ambos";
            ubicar(55, 24); cout << "ESC: volver";
        }

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        if (tecla != TECLA_ENTER) continue;

        if (!contacto && pendiente) continue;
        if (!contacto) {
            formularioSolicitudConexion(red, idSesion, idPersona);
            continue;
        }

        limpiarZonaContenido();
        ubicar(55, 15); cout << "Eliminar a " << recortar(persona->getNombreCompleto(), 43) << " de tus contactos?";
        ubicar(55, 18); cout << "ENTER: confirmar     ESC: cancelar";
        int confirmar = leerTecla();
        if (confirmar != TECLA_ENTER) continue;

        if (red.eliminarContacto(idSesion, idPersona)) {
            GestorArchivos::guardarTodo(red);
            avisoRed("Contacto eliminado correctamente");
            return;
        }
        if (!mostrarErrorZona(red.getUltimoError())) return;
    }
}

// Contactos directos. La lista viene ordenada alfabeticamente mediante
// QuickSort desde RedProfesional::contactosOrdenadosPorNombre.
void seccionContactosRed(RedProfesional& red, int idSesion) {
    int pagina = 0;
    int seleccion = 0;

    while (true) {
        Lista<int> contactos = red.contactosOrdenadosPorNombre(idSesion);
        int total = (int)contactos.longitud();
        int paginas = (total + 3) / 4;
        if (paginas == 0) pagina = 0;
        else if (pagina >= paginas) pagina = paginas - 1;

        int inicio = pagina * 4;
        int enPagina = total - inicio;
        if (enPagina > 4) enPagina = 4;
        if (enPagina <= 0) seleccion = 0;
        else if (seleccion >= enPagina) seleccion = enPagina - 1;

        limpiarZonaContenido();
        ubicar(55, 11); cout << "MIS CONTACTOS (" << total << ")";
        if (paginas > 1) cout << "  Pagina " << pagina + 1 << " de " << paginas << " (<- ->)";
        separadorConOrden("QuickSort por nombre");

        if (total == 0) {
            ubicar(55, 14); cout << "Aun no tienes contactos";
            ubicar(55, 16); cout << "Busca personas o revisa las sugerencias de conexion";
        }

        int idsPagina[4] = { 0, 0, 0, 0 };
        int i = 0;
        for (int& idContacto : contactos) {
            if (i >= inicio && i < inicio + 4) {
                int pos = i - inicio;
                int y = 14 + pos * 3;
                idsPagina[pos] = idContacto;
                const Usuario* u = red.buscarUsuario(idContacto);
                if (u != nullptr) {
                    ubicar(58, y); cout << recortar(u->getNombreCompleto(), 55);
                    ubicar(58, y + 1); cout << recortar(valorOVacio(u->getTitular()), 55);
                }
            }
            i++;
        }

        if (enPagina > 0) ubicar(55, 14 + seleccion * 3), cout << "->";

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        else if (tecla == TECLA_ARRIBA && seleccion > 0) seleccion--;
        else if (tecla == TECLA_ABAJO && seleccion < enPagina - 1) seleccion++;
        else if (tecla == TECLA_IZQUIERDA && pagina > 0) { pagina--; seleccion = 0; }
        else if (tecla == TECLA_DERECHA && pagina < paginas - 1) { pagina++; seleccion = 0; }
        else if (tecla == TECLA_ENTER && enPagina > 0)
            detallePersonaRed(red, idSesion, idsPagina[seleccion]);
    }
}

// Las solicitudes se atienden en el mismo orden en que llegaron porque
// Usuario las guarda en una Cola. Se muestra la solicitud mas antigua.
void seccionSolicitudesRed(RedProfesional& red, int idSesion) {
    int seleccion = 0; // 0 aceptar, 1 rechazar

    while (true) {
        const Usuario* usuario = red.buscarUsuario(idSesion);
        if (usuario == nullptr) return;

        limpiarZonaContenido();
        ubicar(55, 11); cout << "SOLICITUDES RECIBIDAS (" << usuario->cantidadSolicitudesPendientes() << ")";
        ubicar(55, 12); cout << "----------------------------------------------------------------";

        if (!usuario->tieneSolicitudesPendientes()) {
            ubicar(55, 15); cout << "No tienes solicitudes pendientes";
            ubicar(55, 18); cout << "ESC: volver";
            while (leerTecla() != TECLA_ESC) {}
            return;
        }

        const SolicitudConexion& solicitud = usuario->verSiguienteSolicitud();
        const Usuario* emisor = red.buscarUsuario(solicitud.getIdEmisor());
        string nombre = (emisor != nullptr ? emisor->getNombreCompleto() : string("Usuario no disponible"));

        ubicar(55, 14); cout << "De: " << recortar(nombre, 58);
        ubicar(55, 15); cout << "Fecha: " << solicitud.getFecha();
        ubicar(55, 17); cout << "Mensaje:";
        ubicar(57, 18); cout << recortar(valorOVacio(solicitud.getMensaje()), 58);
        ubicar(58, 21); cout << "Aceptar solicitud";
        ubicar(58, 22); cout << "Rechazar solicitud";
        ubicar(55, 21 + seleccion); cout << "->";

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        else if (tecla == TECLA_ARRIBA) seleccion = 0;
        else if (tecla == TECLA_ABAJO) seleccion = 1;
        else if (tecla == TECLA_ENTER) {
            bool aceptar = seleccion == 0;
            if (red.responderSiguienteSolicitud(idSesion, aceptar)) {
                GestorArchivos::guardarTodo(red);
                avisoRed(aceptar ? "Solicitud aceptada. Ahora son contactos." : "Solicitud rechazada.");
            }
            else if (!mostrarErrorZona(red.getUltimoError())) return;
        }
    }
}

// Busca usuarios por nombre o titular. Los resultados tambien se entregan
// ordenados con QuickSort para mantener una presentacion alfabetica.
void seccionBuscarPersonasRed(RedProfesional& red, int idSesion) {
    string consulta;
    limpiarZonaContenido();
    ubicar(55, 11); cout << "BUSCAR PERSONAS";
    ubicar(55, 12); cout << "----------------------------------------------------------------";
    ubicar(55, 14); cout << "Nombre o titular:";
    ubicar(55, 17); cout << "ENTER: buscar     ESC: volver";
    if (!leerCampoEn(74, 14, 35, consulta)) return;

    Lista<int> resultados = red.buscarPersonas(idSesion, consulta);
    int pagina = 0;
    int seleccion = 0;

    while (true) {
        int total = (int)resultados.longitud();
        int paginas = (total + 3) / 4;
        int inicio = pagina * 4;
        int enPagina = total - inicio;
        if (enPagina > 4) enPagina = 4;
        if (enPagina <= 0) seleccion = 0;
        else if (seleccion >= enPagina) seleccion = enPagina - 1;

        limpiarZonaContenido();
        ubicar(55, 11); cout << "RESULTADOS PARA: " << recortar(consulta, 30) << " (" << total << ")";
        if (paginas > 1) cout << "  Pag. " << pagina + 1 << "/" << paginas;
        separadorConOrden("QuickSort por nombre");

        if (total == 0) ubicar(55, 15), cout << "No se encontraron personas";

        int idsPagina[4] = { 0, 0, 0, 0 };
        int i = 0;
        for (int& idPersona : resultados) {
            if (i >= inicio && i < inicio + 4) {
                int pos = i - inicio;
                int y = 14 + pos * 3;
                idsPagina[pos] = idPersona;
                const Usuario* u = red.buscarUsuario(idPersona);
                if (u != nullptr) {
                    string estado = red.sonContactos(idSesion, idPersona) ? " [Contacto]" :
                        (red.existeSolicitudPendienteEntre(idSesion, idPersona) ? " [Pendiente]" : "");
                    ubicar(58, y); cout << recortar(u->getNombreCompleto() + estado, 58);
                    ubicar(58, y + 1); cout << recortar(valorOVacio(u->getTitular()), 58);
                }
            }
            i++;
        }

        if (enPagina > 0) ubicar(55, 14 + seleccion * 3), cout << "->";

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        else if (tecla == TECLA_ARRIBA && seleccion > 0) seleccion--;
        else if (tecla == TECLA_ABAJO && seleccion < enPagina - 1) seleccion++;
        else if (tecla == TECLA_IZQUIERDA && pagina > 0) { pagina--; seleccion = 0; }
        else if (tecla == TECLA_DERECHA && pagina < paginas - 1) { pagina++; seleccion = 0; }
        else if (tecla == TECLA_ENTER && enPagina > 0)
            detallePersonaRed(red, idSesion, idsPagina[seleccion]);
    }
}

// Personas de segundo grado. RedProfesional las obtiene con un recorrido
// recursivo con control de visitados y luego las ordena con QuickSort:
// mas contactos en comun primero y, en empate, alfabeticamente.
void seccionSugerenciasRed(RedProfesional& red, int idSesion) {
    int pagina = 0;
    int seleccion = 0;

    while (true) {
        Lista<SugerenciaConexion> sugerencias = red.sugerenciasConexion(idSesion);
        int total = (int)sugerencias.longitud();
        int paginas = (total + 3) / 4;
        if (paginas == 0) pagina = 0;
        else if (pagina >= paginas) pagina = paginas - 1;

        int inicio = pagina * 4;
        int enPagina = total - inicio;
        if (enPagina > 4) enPagina = 4;
        if (enPagina <= 0) seleccion = 0;
        else if (seleccion >= enPagina) seleccion = enPagina - 1;

        limpiarZonaContenido();
        ubicar(55, 11); cout << "SUGERENCIAS DE CONEXION (" << total << ")";
        if (paginas > 1) cout << "  Pag. " << pagina + 1 << "/" << paginas;
        separadorConOrden("QuickSort por contactos en comun");

        if (total == 0) {
            ubicar(55, 14); cout << "No hay sugerencias nuevas por ahora";
            ubicar(55, 16); cout << "Las sugerencias se basan en contactos en comun";
        }

        int idsPagina[4] = { 0, 0, 0, 0 };
        int i = 0;
        for (SugerenciaConexion& sug : sugerencias) {
            if (i >= inicio && i < inicio + 4) {
                int pos = i - inicio;
                int y = 14 + pos * 3;
                idsPagina[pos] = sug.idUsuario;
                const Usuario* u = red.buscarUsuario(sug.idUsuario);
                if (u != nullptr) {
                    ubicar(58, y); cout << recortar(u->getNombreCompleto(), 40);
                    ubicar(101, y); cout << sug.contactosEnComun << " en comun";
                    ubicar(58, y + 1); cout << recortar(valorOVacio(u->getTitular()), 58);
                }
            }
            i++;
        }

        if (enPagina > 0) ubicar(55, 14 + seleccion * 3), cout << "->";

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        else if (tecla == TECLA_ARRIBA && seleccion > 0) seleccion--;
        else if (tecla == TECLA_ABAJO && seleccion < enPagina - 1) seleccion++;
        else if (tecla == TECLA_IZQUIERDA && pagina > 0) { pagina--; seleccion = 0; }
        else if (tecla == TECLA_DERECHA && pagina < paginas - 1) { pagina++; seleccion = 0; }
        else if (tecla == TECLA_ENTER && enPagina > 0)
            detallePersonaRed(red, idSesion, idsPagina[seleccion]);
    }
}

void deshacerAccionRed(RedProfesional& red, int idSesion) {
    limpiarZonaContenido();
    ubicar(55, 14); cout << "DESHACER ULTIMA ACCION DE RED";
    ubicar(55, 16); cout << "Revierte la ultima accion de la pila (red o perfil).";
    ubicar(55, 19); cout << "ENTER: confirmar     ESC: cancelar";
    if (leerTecla() != TECLA_ENTER) return;

    string descripcion;
    if (red.deshacerUltimaAccion(idSesion, descripcion)) {
        GestorArchivos::guardarTodo(red);
        avisoRed("Deshecho: " + recortar(descripcion, 48));
    }
    else {
        mostrarErrorZona(red.getUltimoError());
    }
}

// Menu interno de Mi red. Mantiene juntas las operaciones relacionadas con
// conexiones y deja visible el uso de QuickSort en contactos y sugerencias.
void seccionMiRed(RedProfesional& red, int idSesion) {
    int opcion = 0;
    const int totalOpciones = 5;

    while (true) {
        const Usuario* usuario = red.buscarUsuario(idSesion);
        if (usuario == nullptr) return;

        limpiarZonaContenido();
        ubicar(55, 11); cout << "MI RED";
        ubicar(55, 12); cout << "----------------------------------------------------------------";
        ubicar(58, 14); cout << "Mis contactos (" << usuario->totalContactos() << ")";
        ubicar(58, 16); cout << "Solicitudes recibidas (" << usuario->cantidadSolicitudesPendientes() << ")";
        ubicar(58, 18); cout << "Buscar personas";
        ubicar(58, 20); cout << "Sugerencias para ti";
        ubicar(58, 22); cout << "Deshacer ultima accion";
        ubicar(55, 14 + opcion * 2); cout << "->";
        ubicar(55, 25); cout << "QuickSort: contactos y sugerencias     ESC: volver";

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        else if (tecla == TECLA_ARRIBA && opcion > 0) opcion--;
        else if (tecla == TECLA_ABAJO && opcion < totalOpciones - 1) opcion++;
        else if (tecla == TECLA_ENTER) {
            if (opcion == 0) seccionContactosRed(red, idSesion);
            else if (opcion == 1) seccionSolicitudesRed(red, idSesion);
            else if (opcion == 2) seccionBuscarPersonasRed(red, idSesion);
            else if (opcion == 3) seccionSugerenciasRed(red, idSesion);
            else if (opcion == 4) deshacerAccionRed(red, idSesion);
        }
    }
}

// ======================= Individuo: Mensajes =======================

void avisoMensajes(string mensaje) {
    limpiarZonaContenido();
    ubicar(55, 15); cout << mensaje;
    ubicar(55, 18); cout << "ENTER: continuar     ESC: volver";
    while (true) {
        int tecla = leerTecla();
        if (tecla == TECLA_ENTER || tecla == TECLA_ESC) return;
    }
}

// Formulario de una sola linea para enviar un mensaje a un contacto.
// RedProfesional vuelve a validar que ambos sean contactos antes de guardarlo.
void formularioNuevoMensaje(RedProfesional& red, int idSesion, int idContacto) {
    const Usuario* contacto = red.buscarUsuario(idContacto);
    if (contacto == nullptr) return;

    while (true) {
        string texto;
        limpiarZonaContenido();
        ubicar(55, 11); cout << "NUEVO MENSAJE";
        ubicar(55, 12); cout << "----------------------------------------------------------------";
        ubicar(55, 14); cout << "Para: " << recortar(contacto->getNombreCompleto(), 55);
        ubicar(55, 16); cout << "Mensaje (max. 58 caracteres):";
        ubicar(55, 20); cout << "ENTER: enviar     ESC: cancelar";

        if (!leerCampoEn(55, 17, RedProfesional::MAX_MENSAJE, texto)) return;

        int idMensaje = red.enviarMensaje(idSesion, idContacto, texto);
        if (idMensaje >= 0) {
            GestorArchivos::guardarTodo(red);
            avisoMensajes("Mensaje enviado correctamente");
            return;
        }
        if (!mostrarErrorZona(red.getUltimoError())) return;
    }
}

// Historial entre dos contactos. Se muestran cuatro mensajes por pagina.
// Al abrir la conversacion, los mensajes recibidos pendientes pasan a leidos.
void conversacionMensajes(RedProfesional& red, int idSesion, int idContacto) {
    const Usuario* contacto = red.buscarUsuario(idContacto);
    if (contacto == nullptr) return;

    uint marcados = red.marcarConversacionLeida(idSesion, idContacto);
    if (marcados > 0) GestorArchivos::guardarTodo(red);

    int pagina = -1; // -1 significa abrir directamente en los mensajes mas recientes.

    while (true) {
        Lista<Mensaje> conversacion = red.obtenerConversacion(idSesion, idContacto);
        int total = (int)conversacion.longitud();
        int paginas = (total + 3) / 4;

        if (pagina < 0) pagina = (paginas > 0 ? paginas - 1 : 0);
        if (paginas == 0) pagina = 0;
        else if (pagina >= paginas) pagina = paginas - 1;

        int inicio = pagina * 4;

        limpiarZonaContenido();
        ubicar(55, 11); cout << "CONVERSACION CON " << recortar(contacto->getNombreCompleto(), 42);
        if (paginas > 1) cout << "  " << pagina + 1 << "/" << paginas;
        ubicar(55, 12); cout << "----------------------------------------------------------------";

        if (total == 0) {
            ubicar(55, 15); cout << "Aun no hay mensajes en esta conversacion";
            ubicar(55, 17); cout << "Presiona ENTER para enviar el primero";
        }

        int i = 0;
        for (Mensaje& m : conversacion) {
            if (i >= inicio && i < inicio + 4) {
                int pos = i - inicio;
                int y = 13 + pos * 3;
                bool mio = m.getIdEmisor() == idSesion;
                string autor = mio ? "Tu" : contacto->getNombre();

                ubicar(55, y); cout << recortar(autor, 23) << "  [" << m.getFecha() << "]";
                ubicar(57, y + 1); cout << recortar(m.getTexto(), 59);
            }
            i++;
        }

        ubicar(55, 25); cout << "<- -> historial     ENTER: escribir     ESC: volver";

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        else if (tecla == TECLA_IZQUIERDA && pagina > 0) pagina--;
        else if (tecla == TECLA_DERECHA && pagina < paginas - 1) pagina++;
        else if (tecla == TECLA_ENTER) {
            formularioNuevoMensaje(red, idSesion, idContacto);
            // Despues de enviar, volver a la ultima pagina para ver el mensaje nuevo.
            pagina = -1;
        }
    }
}

// Bandeja de mensajes. Como solo se puede escribir a conexiones directas,
// los contactos funcionan como lista de conversaciones. Tambien se muestran
// contactos sin historial para poder iniciar un chat nuevo desde aqui.
void seccionMensajes(RedProfesional& red, int idSesion) {
    int pagina = 0;
    int seleccion = 0;

    while (true) {
        Lista<int> contactos = red.contactosOrdenadosPorNombre(idSesion);
        int total = (int)contactos.longitud();
        int paginas = (total + 3) / 4;
        if (paginas == 0) pagina = 0;
        else if (pagina >= paginas) pagina = paginas - 1;

        int inicio = pagina * 4;
        int enPagina = total - inicio;
        if (enPagina > 4) enPagina = 4;
        if (enPagina <= 0) seleccion = 0;
        else if (seleccion >= enPagina) seleccion = enPagina - 1;

        uint noLeidosTotal = red.cantidadMensajesNoLeidos(idSesion);

        limpiarZonaContenido();
        ubicar(55, 11); cout << "MENSAJES (" << noLeidosTotal << " sin leer)";
        if (paginas > 1) cout << "  Pagina " << pagina + 1 << " de " << paginas << " (<- ->)";
        separadorConOrden("QuickSort por nombre");

        if (total == 0) {
            ubicar(55, 14); cout << "No tienes contactos para iniciar una conversacion";
            ubicar(55, 16); cout << "Primero agrega conexiones desde Mi red";
        }

        int idsPagina[4] = { 0, 0, 0, 0 };
        int i = 0;
        for (int& idContacto : contactos) {
            if (i >= inicio && i < inicio + 4) {
                int pos = i - inicio;
                int y = 14 + pos * 3;
                idsPagina[pos] = idContacto;
                const Usuario* u = red.buscarUsuario(idContacto);

                if (u != nullptr) {
                    uint nuevos = red.cantidadMensajesNoLeidosDe(idSesion, idContacto);
                    uint cantidad = red.cantidadMensajesEntre(idSesion, idContacto);
                    string estado;
                    if (nuevos > 0) estado = " [" + std::to_string(nuevos) + " nuevo" + (nuevos == 1 ? "" : "s") + "]";
                    else if (cantidad == 0) estado = " [sin mensajes]";
                    else estado = " [" + std::to_string(cantidad) + " mensajes]";

                    ubicar(58, y); cout << recortar(u->getNombreCompleto() + estado, 58);
                    ubicar(58, y + 1); cout << recortar(valorOVacio(u->getTitular()), 58);
                }
            }
            i++;
        }

        if (enPagina > 0) ubicar(55, 14 + seleccion * 3), cout << "->";
        ubicar(55, 25); cout << "ENTER: abrir conversacion     ESC: volver";

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        else if (tecla == TECLA_ARRIBA && seleccion > 0) seleccion--;
        else if (tecla == TECLA_ABAJO && seleccion < enPagina - 1) seleccion++;
        else if (tecla == TECLA_IZQUIERDA && pagina > 0) { pagina--; seleccion = 0; }
        else if (tecla == TECLA_DERECHA && pagina < paginas - 1) { pagina++; seleccion = 0; }
        else if (tecla == TECLA_ENTER && enPagina > 0 && idsPagina[seleccion] != 0)
            conversacionMensajes(red, idSesion, idsPagina[seleccion]);
    }
}

// ======================= Individuo: Notificaciones =======================

// Muestra el contenido completo de una notificacion. Al abrirla se marca como leida
// y se guarda el cambio para que el estado se conserve al reiniciar el programa.
void detalleNotificacion(RedProfesional& red, int idSesion, int idNotificacion) {
    Usuario* usuario = red.buscarUsuario(idSesion);
    if (usuario == nullptr) return;

    usuario->marcarNotificacionLeida(idNotificacion);
    GestorArchivos::guardarTodo(red);

    Notificacion seleccionada;
    bool encontrada = false;
    usuario->paraCadaNotificacion([&](const Notificacion& n) {
        if (n.getId() == idNotificacion) {
            seleccionada = n;
            encontrada = true;
        }
        });
    if (!encontrada) return;

    limpiarZonaContenido();
    ubicar(55, 11); cout << "DETALLE DE NOTIFICACION";
    ubicar(55, 12); cout << "----------------------------------------------------------------";
    ubicar(55, 14); cout << "Tipo:";
    ubicar(67, 14); cout << seleccionada.tipoToString();
    ubicar(55, 15); cout << "Fecha:";
    ubicar(67, 15); cout << seleccionada.getFecha();
    ubicar(55, 17); cout << "Mensaje:";

    string mensaje = seleccionada.getMensaje();
    const int ancho = 58;
    int y = 18;
    for (int inicio = 0; inicio < (int)mensaje.length() && y <= 22; inicio += ancho, y++)
        ubicar(58, y), cout << mensaje.substr(inicio, ancho);

    ubicar(55, 25); cout << "ESC: volver a notificaciones";
    while (leerTecla() != TECLA_ESC) {}
}

// Bandeja de notificaciones. Se muestran primero las mas recientes.
// ENTER abre y marca una como leida; L marca todo el historial como leido.
void seccionNotificaciones(RedProfesional& red, int idSesion) {
    Usuario* usuario = red.buscarUsuario(idSesion);
    if (usuario == nullptr) return;

    int pagina = 0;
    int seleccion = 0;
    const int porPagina = 4;

    while (true) {
        // La cola se conserva en FIFO, pero para la interfaz se copia a una lista
        // insertando al inicio, de modo que lo mas nuevo aparezca primero.
        Lista<Notificacion> ordenadas;
        usuario->paraCadaNotificacion([&](const Notificacion& n) {
            ordenadas.agregaInicial(n);
            });

        int total = (int)ordenadas.longitud();
        int paginas = total == 0 ? 1 : (total + porPagina - 1) / porPagina;
        if (pagina >= paginas) pagina = paginas - 1;
        if (pagina < 0) pagina = 0;

        int inicio = pagina * porPagina;
        int enPagina = total - inicio;
        if (enPagina > porPagina) enPagina = porPagina;
        if (enPagina <= 0) seleccion = 0;
        else if (seleccion >= enPagina) seleccion = enPagina - 1;

        limpiarZonaContenido();
        ubicar(55, 11); cout << "NOTIFICACIONES";
        ubicar(55, 12); cout << "Nuevas: " << usuario->cantidadNotificacionesNoLeidas()
                            << "   Total: " << usuario->cantidadNotificaciones();

        if (total == 0) {
            ubicar(55, 15); cout << "No tienes notificaciones.";
        }
        else {
            for (int i = 0; i < enPagina; i++) {
                const Notificacion& n = ordenadas.obtenerPos(inicio + i);
                int y = 14 + i * 2;
                ubicar(58, y);
                cout << (n.estaLeida() ? "[LEIDA] " : "[NUEVA] ")
                     << recortar(n.tipoToString(), 46);
                ubicar(58, y + 1);
                cout << recortar(n.getMensaje(), 52) << "  " << n.getFecha();
            }
            ubicar(55, 14 + seleccion * 2); cout << "->";
        }

        ubicar(55, 23); cout << "Pagina " << (pagina + 1) << "/" << paginas;
        ubicar(55, 24); cout << "ENTER: abrir   L: marcar todas leidas";
        ubicar(55, 25); cout << "Flechas: navegar/cambiar pagina   ESC: volver";

        int tecla = leerTecla();
        if (tecla == TECLA_ESC) return;
        else if (tecla == TECLA_ARRIBA && seleccion > 0) seleccion--;
        else if (tecla == TECLA_ABAJO && seleccion < enPagina - 1) seleccion++;
        else if (tecla == TECLA_IZQUIERDA && pagina > 0) { pagina--; seleccion = 0; }
        else if (tecla == TECLA_DERECHA && pagina < paginas - 1) { pagina++; seleccion = 0; }
        else if ((tecla == 'l' || tecla == 'L') && total > 0) {
            usuario->marcarTodasNotificacionesLeidas();
            GestorArchivos::guardarTodo(red);
        }
        else if (tecla == TECLA_ENTER && enPagina > 0) {
            int idNotificacion = ordenadas.obtenerPos(inicio + seleccion).getId();
            detalleNotificacion(red, idSesion, idNotificacion);
        }
    }
}

// ======================= Individuo =======================

void dibujarEncabezadoIndividuo(RedProfesional& red, int idSesion) {
    const Usuario* usuario = red.buscarUsuario(idSesion);
    if (usuario == nullptr) return;

    ubicar(108, 1); cout << RedProfesional::fechaHoy();
    ubicar(92, 2); cout << "Notificaciones nuevas (" << usuario->cantidadNotificacionesNoLeidas() << ")";
    ubicar(92, 3); cout << "Mensajes nuevos (" << red.cantidadMensajesNoLeidos(idSesion) << ")";

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

// Aqui se conecta cada opcion del menu de individuo con su seccion funcional.
void abrirSeccionIndividuo(RedProfesional& red, int idSesion, int opcion) {
    switch (opcion) {
    case 0: seccionMiPerfil(red, idSesion); break;
    case 1: seccionMisCertificaciones(red, idSesion); break;
    case 2: seccionMisPostulaciones(red, idSesion); break;
    case 3: seccionRecomendaciones(red, idSesion); break;
    case 4: seccionMiRed(red, idSesion); break;
    case 5: seccionPublicaciones(red, idSesion, false); break;   // todas las de la red
    case 6: seccionPublicaciones(red, idSesion, true); break;    // solo las mias
    case 7: seccionEmpleos(red, idSesion); break;
    case 8: seccionMensajes(red, idSesion); break;
    case 9: seccionNotificaciones(red, idSesion); break;
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
        Lista<Vacante> mias = red.vacantesOrdenadasDe(idSesion);   // HeapSort por titulo

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
        separadorConOrden("HeapSort por titulo");

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
        separadorConOrden("HeapSort por coincidencias");

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
    red.paraCadaContratacion(idSesion, [&contrataciones](const Postulacion&) { contrataciones++; });

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