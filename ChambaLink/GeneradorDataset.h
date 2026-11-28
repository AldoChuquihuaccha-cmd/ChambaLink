#pragma once

#include <iomanip>
#include <random>
#include <sstream>
#include <string>

#include "RedProfesional.h"

// Generador reproducible de datos de prueba para ChambaLink (RF18).
// Utiliza las estructuras propias del proyecto: Lista<int> para conservar
// los identificadores y arreglos estaticos para los catalogos.
// No utiliza contenedores de la STL ni algoritmos de ordenamiento de la STL.
class GeneradorDataset {
private:
    std::mt19937 motor;

    enum { TOTAL_HABILIDADES = 22 };

    // ==================== Catalogos estaticos ====================

    static const char* const* nombres() {
        static const char* const datos[] = {
            "Aldo", "Ana", "Andrea", "Bruno", "Camila", "Carlos", "Daniel",
            "Daniela", "Diego", "Elena", "Fernando", "Gabriela", "Hugo",
            "Lucia", "Luis", "Mariana", "Mateo", "Miguel", "Paola", "Rosa",
            "Sofia", "Valeria", "Victor", "Ximena", "Jose", "Renato",
            "Fiorella", "Javier", "Patricia", "Rodrigo"
        };
        return datos;
    }

    static const char* const* apellidos() {
        static const char* const datos[] = {
            "Alvarez", "Arias", "Barrera", "Benavides", "Castillo", "Chavez",
            "Espinoza", "Fernandez", "Flores", "Garcia", "Gomez", "Gonzales",
            "Gutierrez", "Huaman", "Lopez", "Medina", "Mendoza", "Morales",
            "Paredes", "Quispe", "Ramirez", "Rivera", "Rojas", "Salazar",
            "Sanchez", "Torres", "Vargas", "Vasquez", "Vega", "Zapata"
        };
        return datos;
    }

    static const char* const* titulares() {
        static const char* const datos[] = {
            "Desarrollador de software", "Analista de datos", "Ingeniero de sistemas",
            "Disenador UX UI", "Especialista en redes", "Administrador de proyectos",
            "Desarrollador backend", "Desarrollador frontend", "Ingeniero de datos",
            "Especialista en ciberseguridad", "Administrador de bases de datos",
            "Estudiante de ciencias de la computacion", "Consultor tecnologico",
            "Analista de calidad de software", "Ingeniero cloud"
        };
        return datos;
    }

    static const char* const* distritos() {
        // Los nombres coinciden con los vertices de GrafoDistritos.h.
        static const char* const datos[] = {
            "Ancon", "Ate", "Barranco", "Brena", "Carabayllo", "Chaclacayo",
            "Chorrillos", "Cieneguilla", "Comas", "El Agustino", "Independencia",
            "Jesus Maria", "La Molina", "La Victoria", "Lima", "Lince", "Los Olivos",
            "Lurigancho", "Lurin", "Magdalena del Mar", "Miraflores", "Pachacamac",
            "Pucusana", "Pueblo Libre", "Puente Piedra", "Punta Hermosa", "Punta Negra",
            "Rimac", "San Bartolo", "San Borja", "San Isidro", "San Juan de Lurigancho",
            "San Juan de Miraflores", "San Luis", "San Martin de Porres", "San Miguel",
            "Santa Anita", "Santa Maria del Mar", "Santa Rosa", "Santiago de Surco",
            "Surquillo", "Villa El Salvador", "Villa Maria del Triunfo", "Callao",
            "Bellavista", "Carmen de la Legua", "La Perla", "La Punta", "Mi Peru", "Ventanilla"
        };
        return datos;
    }

    static const char* const* habilidades() {
        static const char* const datos[TOTAL_HABILIDADES] = {
            "C++", "Java", "Python", "JavaScript", "SQL", "Excel", "Power BI",
            "Git", "Docker", "Linux", "Estructuras de datos", "Algoritmos",
            "HTML CSS", "React", "Node.js", "AWS", "Azure", "Scrum", "Figma",
            "Ciberseguridad", "Machine Learning", "Analisis de datos"
        };
        return datos;
    }

    static const char* const* sectores() {
        static const char* const datos[] = {
            "Tecnologia", "Educacion", "Finanzas", "Salud", "Comercio", "Consultoria",
            "Telecomunicaciones", "Logistica", "Construccion", "Analitica", "Marketing",
            "Servicios digitales"
        };
        return datos;
    }

    static const char* const* nombresEmpresas() {
        static const char* const datos[] = {
            "InnovaTech", "Andes Digital", "Nexo Solutions", "DataLab", "Kallpa Systems",
            "QoriSoft", "Nova Consultores", "Pixel Factory", "Ayni Tech", "Peru Analytics",
            "Sumaq Digital", "Codigo Andino", "Vector TI", "ByteWorks", "Conecta Peru"
        };
        return datos;
    }

    static const char* const* titulosVacantes() {
        static const char* const datos[] = {
            "Desarrollador C++ Junior", "Analista de datos", "Desarrollador Java",
            "Ingeniero de software", "Especialista en redes", "Analista QA",
            "Desarrollador frontend", "Desarrollador backend", "Ingeniero cloud",
            "Administrador de bases de datos", "Especialista en ciberseguridad",
            "Cientifico de datos", "Analista de sistemas", "Disenador UX UI",
            "Ingeniero DevOps", "Asistente de soporte TI"
        };
        return datos;
    }

    static const char* const* descripcionesVacantes() {
        static const char* const datos[] = {
            "Desarrollo y mantenimiento de aplicaciones informaticas.",
            "Analisis de informacion y elaboracion de reportes para el negocio.",
            "Implementacion de servicios, pruebas y documentacion tecnica.",
            "Soporte de plataformas digitales y mejora de procesos tecnologicos.",
            "Participacion en proyectos de innovacion y transformacion digital.",
            "Diseno, desarrollo y optimizacion de soluciones de software."
        };
        return datos;
    }

    static const char* const* modalidades() {
        static const char* const datos[] = {
            "Remoto", "Hibrido", "Presencial"
        };
        return datos;
    }

    static const char* const* textosPublicacion() {
        static const char* const datos[] = {
            "Comparto un proyecto que desarrolle recientemente.",
            "Estoy aprendiendo nuevas herramientas de desarrollo.",
            "Busco colaborar en proyectos de tecnologia.",
            "Hoy termine un curso de analisis de datos.",
            "Comparto algunos avances de mi portafolio.",
            "Me interesa conectar con profesionales del sector.",
            "Estoy trabajando en una solucion de software.",
            "Consejos para mejorar nuestras habilidades digitales."
        };
        return datos;
    }

    static const char* const* textosComentario() {
        static const char* const datos[] = {
            "Excelente aporte, gracias por compartir.",
            "Muy interesante. Me gustaria conocer mas.",
            "Buen trabajo, sigue compartiendo tus avances.",
            "Coincido, es una habilidad muy importante.",
            "Gracias por compartir esta experiencia.",
            "Felicitaciones por el avance del proyecto."
        };
        return datos;
    }

    // ==================== Funciones auxiliares ====================

    int aleatorio(int minimo, int maximo) {
        std::uniform_int_distribution<int> distribucion(minimo, maximo);
        return distribucion(motor);
    }

    std::string elegir(const char* const lista[], int cantidad) {
        return lista[aleatorio(0, cantidad - 1)];
    }

    int elegirId(const Lista<int>& ids) {
        if (ids.esVacia()) return -1;
        int posicion = aleatorio(0, (int)ids.longitud() - 1);
        return ids.obtenerPos((uint)posicion);
    }

    int elegirOtroId(const Lista<int>& ids, int idExcluir) {
        if (ids.longitud() < 2) return elegirId(ids);

        for (int intento = 0; intento < (int)ids.longitud() * 2; ++intento) {
            int candidato = elegirId(ids);
            if (candidato != idExcluir) return candidato;
        }

        for (uint i = 0; i < ids.longitud(); ++i) {
            int candidato = ids.obtenerPos(i);
            if (candidato != idExcluir) return candidato;
        }

        return idExcluir;
    }

    // Fisher-Yates: mezcla un arreglo de indices mediante intercambios propios.
    void mezclarIndices(int indices[], int cantidad) {
        for (int i = cantidad - 1; i > 0; --i) {
            int j = aleatorio(0, i);
            int auxiliar = indices[i];
            indices[i] = indices[j];
            indices[j] = auxiliar;
        }
    }

    static std::string numeroConCeros(int numero) {
        std::ostringstream salida;
        salida << std::setw(6) << std::setfill('0') << numero;
        return salida.str();
    }

    static std::string crearCorreo(const std::string& tipo, int numero) {
        return tipo + numeroConCeros(numero) + "@dataset.pe";
    }

    std::string crearRequisitos() {
        int indices[TOTAL_HABILIDADES];

        for (int i = 0; i < TOTAL_HABILIDADES; ++i)
            indices[i] = i;

        mezclarIndices(indices, TOTAL_HABILIDADES);

        int cantidad = aleatorio(2, 4);
        std::ostringstream salida;

        for (int i = 0; i < cantidad; ++i) {
            if (i > 0) salida << ", ";
            salida << habilidades()[indices[i]];
        }

        return salida.str();
    }

    // ==================== Generacion de catalogos ====================

    bool generarUsuarios(RedProfesional& red, int cantidad,
        Lista<int>& idsUsuarios) {

        for (int i = 1; i <= cantidad; ++i) {
            int id = red.registrarUsuario(
                elegir(nombres(), 30),
                elegir(apellidos(), 30),
                elegir(titulares(), 15),
                elegir(distritos(), 50),
                crearCorreo("persona", i),
                "Demo1234"
            );

            if (id < 0) return false;
            idsUsuarios.agregaFinal(id);
        }

        // Se asignan habilidades distintas a cada usuario mediante indices mezclados.
        // Se agregan directamente para no llenar la pila de acciones reversibles.
        for (uint i = 0; i < idsUsuarios.longitud(); ++i) {
            Usuario* usuario = red.buscarUsuario(idsUsuarios.obtenerPos(i));
            if (usuario == nullptr) return false;

            int indices[TOTAL_HABILIDADES];
            for (int j = 0; j < TOTAL_HABILIDADES; ++j)
                indices[j] = j;

            mezclarIndices(indices, TOTAL_HABILIDADES);

            int cantidadHabilidades = aleatorio(2, 5);
            for (int j = 0; j < cantidadHabilidades; ++j) {
                NivelHabilidad nivel = (NivelHabilidad)aleatorio(0, 3);
                usuario->agregarHabilidad(Habilidad(habilidades()[indices[j]], nivel));
            }
        }

        return true;
    }

    bool generarEmpresas(RedProfesional& red, int cantidad,
        Lista<int>& idsEmpresas) {

        for (int i = 1; i <= cantidad; ++i) {
            std::string nombre = elegir(nombresEmpresas(), 15) + " " + numeroConCeros(i);

            int id = red.registrarEmpresa(
                nombre,
                elegir(sectores(), 12),
                elegir(distritos(), 50),
                crearCorreo("empresa", i),
                "Demo1234"
            );

            if (id < 0) return false;
            idsEmpresas.agregaFinal(id);
        }

        return true;
    }

    bool generarVacantes(RedProfesional& red, int cantidad,
        const Lista<int>& idsEmpresas, Lista<int>& idsVacantes) {

        if (cantidad > 0 && idsEmpresas.esVacia()) return false;

        for (int i = 0; i < cantidad; ++i) {
            int idEmpresa = elegirId(idsEmpresas);

            int id = red.publicarVacante(
                idEmpresa,
                elegir(titulosVacantes(), 16),
                elegir(descripcionesVacantes(), 6),
                crearRequisitos(),
                elegir(modalidades(), 3)
            );

            if (id < 0) return false;
            idsVacantes.agregaFinal(id);
        }

        return true;
    }

    // ==================== Generacion de relaciones ====================

    bool generarContactos(RedProfesional& red, const Lista<int>& idsUsuarios) {
        int cantidad = (int)idsUsuarios.longitud();
        if (cantidad < 2) return true;

        // Se forma un ciclo: cada usuario queda conectado con el siguiente.
        // Con dos usuarios se crea una sola conexion, no dos solicitudes duplicadas.
        int conexiones = (cantidad == 2) ? 1 : cantidad;

        for (int i = 0; i < conexiones; ++i) {
            int idEmisor = idsUsuarios.obtenerPos((uint)i);
            int idReceptor = idsUsuarios.obtenerPos((uint)((i + 1) % cantidad));

            if (!red.enviarSolicitud(idEmisor, idReceptor,
                "Hola, me gustaria conectar contigo.")) {
                return false;
            }

            // Se acepta inmediatamente para generar un contacto bidireccional
            // utilizando el flujo normal de la aplicacion.
            if (!red.responderSiguienteSolicitud(idReceptor, true))
                return false;
        }

        return true;
    }

    bool generarSolicitudesPendientes(RedProfesional& red,
        const Lista<int>& idsUsuarios) {

        int cantidadUsuarios = (int)idsUsuarios.longitud();
        if (cantidadUsuarios < 4) return true;

        // Se deja aproximadamente una solicitud pendiente por cada diez usuarios.
        int cantidadSolicitudes = (cantidadUsuarios + 9) / 10;

        for (int i = 0; i < cantidadSolicitudes; ++i) {
            int idEmisor = idsUsuarios.obtenerPos((uint)i);
            int idReceptor = idsUsuarios.obtenerPos((uint)(i + 2));

            // En el ciclo de contactos, los usuarios separados por dos posiciones
            // no son contactos directos. Se valida igualmente para evitar duplicados.
            if (red.sonContactos(idEmisor, idReceptor) ||
                red.existeSolicitudPendienteEntre(idEmisor, idReceptor)) {
                continue;
            }

            if (!red.enviarSolicitud(idEmisor, idReceptor,
                "Hola, vi tu perfil y quisiera conectar.")) {
                return false;
            }
        }

        return true;
    }

    // ==================== Generacion de contenido ====================

    bool generarPublicaciones(RedProfesional& red,
        const Lista<int>& idsUsuarios) {

        int cantidadUsuarios = (int)idsUsuarios.longitud();
        if (cantidadUsuarios == 0) return false;

        // Se genera aproximadamente una publicacion por cada cinco usuarios.
        // Para conjuntos pequenos siempre se crea al menos una.
        int cantidadPublicaciones = (cantidadUsuarios + 4) / 5;

        for (int i = 0; i < cantidadPublicaciones; ++i) {
            int posicionAutor = i % cantidadUsuarios;
            int idAutor = idsUsuarios.obtenerPos((uint)posicionAutor);

            int idPublicacion = red.publicar(
                idAutor,
                elegir(textosPublicacion(), 8)
            );

            if (idPublicacion < 0) return false;

            // Cada publicacion recibe entre uno y tres me gusta, si hay usuarios
            // suficientes. Se excluye al autor y se evita repetir usuarios.
            if (cantidadUsuarios > 1) {
                int maximoMeGusta = cantidadUsuarios - 1;
                if (maximoMeGusta > 3) maximoMeGusta = 3;

                int cantidadMeGusta = aleatorio(1, maximoMeGusta);

                for (int j = 0; j < cantidadMeGusta; ++j) {
                    int posicion = (posicionAutor + 1 + j) % cantidadUsuarios;
                    int idUsuario = idsUsuarios.obtenerPos((uint)posicion);

                    if (!red.darMeGusta(idUsuario, idPublicacion))
                        return false;
                }
            }

            // Se crea al menos un comentario por publicacion.
            int idComentarista = elegirOtroId(idsUsuarios, idAutor);
            int idComentario = red.comentar(
                idComentarista,
                idPublicacion,
                elegir(textosComentario(), 6)
            );

            if (idComentario < 0) return false;

            // En publicaciones alternas se agrega una respuesta para probar
            // la estructura de hilos de comentarios.
            if (i % 2 == 0) {
                int idRespondedor = elegirOtroId(idsUsuarios, idComentarista);
                int idRespuesta = red.comentar(
                    idRespondedor,
                    idPublicacion,
                    elegir(textosComentario(), 6),
                    idComentario
                );

                if (idRespuesta < 0) return false;
            }
        }

        return true;
    }

    // ==================== Generacion de postulaciones ====================

    bool generarPostulaciones(RedProfesional& red,
        const Lista<int>& idsUsuarios, const Lista<int>& idsVacantes) {

        int cantidadUsuarios = (int)idsUsuarios.longitud();
        if (cantidadUsuarios == 0) return false;

        for (uint i = 0; i < idsVacantes.longitud(); ++i) {
            int idVacante = idsVacantes.obtenerPos(i);

            // Cada vacante recibe de uno a tres postulantes distintos,
            // segun la cantidad de usuarios disponibles.
            int maximoPostulantes = cantidadUsuarios;
            if (maximoPostulantes > 3) maximoPostulantes = 3;

            int cantidadPostulantes = aleatorio(1, maximoPostulantes);
            int inicio = aleatorio(0, cantidadUsuarios - 1);

            for (int j = 0; j < cantidadPostulantes; ++j) {
                int posicion = (inicio + j) % cantidadUsuarios;
                int idUsuario = idsUsuarios.obtenerPos((uint)posicion);

                if (red.postular(idUsuario, idVacante) < 0)
                    return false;
            }
        }

        return true;
    }

public:
    static const int MAX_REGISTROS = 10000;

    explicit GeneradorDataset(unsigned int semilla) : motor(semilla) {}

    // La red debe estar vacia. Se generan catalogos, relaciones e interacciones.
    // La semilla permite repetir la misma secuencia aleatoria.
    bool generar(RedProfesional& red, int cantidadUsuarios,
        int cantidadEmpresas, int cantidadVacantes) {

        if (cantidadUsuarios < 1 || cantidadEmpresas < 1 || cantidadVacantes < 0 ||
            cantidadUsuarios > MAX_REGISTROS || cantidadEmpresas > MAX_REGISTROS ||
            cantidadVacantes > MAX_REGISTROS) {
            return false;
        }

        Lista<int> idsUsuarios;
        Lista<int> idsEmpresas;
        Lista<int> idsVacantes;

        if (!generarUsuarios(red, cantidadUsuarios, idsUsuarios)) return false;
        if (!generarEmpresas(red, cantidadEmpresas, idsEmpresas)) return false;
        if (!generarVacantes(red, cantidadVacantes, idsEmpresas, idsVacantes)) return false;

        if (!generarContactos(red, idsUsuarios)) return false;
        if (!generarSolicitudesPendientes(red, idsUsuarios)) return false;
        if (!generarPublicaciones(red, idsUsuarios)) return false;
        if (!generarPostulaciones(red, idsUsuarios, idsVacantes)) return false;

        // Reconstruye los indices AVL y Hash al finalizar la carga.
        red.reconstruirIndicesPublico();
        return true;
    }
};
