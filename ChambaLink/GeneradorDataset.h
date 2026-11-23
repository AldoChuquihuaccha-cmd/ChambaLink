#pragma once

#include <algorithm>
#include <iomanip>
#include <random>
#include <sstream>
#include <string>
#include <vector>

#include "RedProfesional.h"

// Generador reproducible de datos de prueba para ChambaLink (RF18).
// Se utiliza sobre una RedProfesional nueva para evitar duplicar registros.
// La misma semilla produce la misma secuencia de datos en el mismo entorno.
class GeneradorDataset {
private:
    std::mt19937 motor;

    // Listas base utilizadas por el generador.
    static const std::vector<std::string>& nombres() {
        static const std::vector<std::string> datos = {
            "Aldo", "Ana", "Andrea", "Bruno", "Camila", "Carlos", "Daniel",
            "Daniela", "Diego", "Elena", "Fernando", "Gabriela", "Hugo",
            "Lucia", "Luis", "Mariana", "Mateo", "Miguel", "Paola", "Rosa",
            "Sofia", "Valeria", "Victor", "Ximena", "Jose", "Renato",
            "Fiorella", "Javier", "Patricia", "Rodrigo"
        };
        return datos;
    }

    static const std::vector<std::string>& apellidos() {
        static const std::vector<std::string> datos = {
            "Alvarez", "Arias", "Barrera", "Benavides", "Castillo", "Chavez",
            "Espinoza", "Fernandez", "Flores", "Garcia", "Gomez", "Gonzales",
            "Gutierrez", "Huaman", "Lopez", "Medina", "Mendoza", "Morales",
            "Paredes", "Quispe", "Ramirez", "Rivera", "Rojas", "Salazar",
            "Sanchez", "Torres", "Vargas", "Vasquez", "Vega", "Zapata"
        };
        return datos;
    }

    static const std::vector<std::string>& titulares() {
        static const std::vector<std::string> datos = {
            "Desarrollador de software", "Analista de datos", "Ingeniero de sistemas",
            "Disenador UX UI", "Especialista en redes", "Administrador de proyectos",
            "Desarrollador backend", "Desarrollador frontend", "Ingeniero de datos",
            "Especialista en ciberseguridad", "Administrador de bases de datos",
            "Estudiante de ciencias de la computacion", "Consultor tecnologico",
            "Analista de calidad de software", "Ingeniero cloud"
        };
        return datos;
    }

    static const std::vector<std::string>& distritos() {
        // Los nombres coinciden con los 50 vertices definidos en GrafoDistritos.h.
        static const std::vector<std::string> datos = {
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

    static const std::vector<std::string>& habilidades() {
        static const std::vector<std::string> datos = {
            "C++", "Java", "Python", "JavaScript", "SQL", "Excel", "Power BI",
            "Git", "Docker", "Linux", "Estructuras de datos", "Algoritmos",
            "HTML CSS", "React", "Node.js", "AWS", "Azure", "Scrum", "Figma",
            "Ciberseguridad", "Machine Learning", "Analisis de datos"
        };
        return datos;
    }

    static const std::vector<std::string>& sectores() {
        static const std::vector<std::string> datos = {
            "Tecnologia", "Educacion", "Finanzas", "Salud", "Comercio", "Consultoria",
            "Telecomunicaciones", "Logistica", "Construccion", "Analitica", "Marketing",
            "Servicios digitales"
        };
        return datos;
    }

    static const std::vector<std::string>& nombresEmpresas() {
        static const std::vector<std::string> datos = {
            "InnovaTech", "Andes Digital", "Nexo Solutions", "DataLab", "Kallpa Systems",
            "QoriSoft", "Nova Consultores", "Pixel Factory", "Ayni Tech", "Peru Analytics",
            "Sumaq Digital", "Codigo Andino", "Vector TI", "ByteWorks", "Conecta Peru"
        };
        return datos;
    }

    static const std::vector<std::string>& titulosVacantes() {
        static const std::vector<std::string> datos = {
            "Desarrollador C++ Junior", "Analista de datos", "Desarrollador Java",
            "Ingeniero de software", "Especialista en redes", "Analista QA",
            "Desarrollador frontend", "Desarrollador backend", "Ingeniero cloud",
            "Administrador de bases de datos", "Especialista en ciberseguridad",
            "Cientifico de datos", "Analista de sistemas", "Disenador UX UI",
            "Ingeniero DevOps", "Asistente de soporte TI"
        };
        return datos;
    }

    static const std::vector<std::string>& descripcionesVacantes() {
        static const std::vector<std::string> datos = {
            "Desarrollo y mantenimiento de aplicaciones informaticas.",
            "Analisis de informacion y elaboracion de reportes para el negocio.",
            "Implementacion de servicios, pruebas y documentacion tecnica.",
            "Soporte de plataformas digitales y mejora de procesos tecnologicos.",
            "Participacion en proyectos de innovacion y transformacion digital.",
            "Diseno, desarrollo y optimizacion de soluciones de software."
        };
        return datos;
    }

    static const std::vector<std::string>& modalidades() {
        static const std::vector<std::string> datos = { "Remoto", "Hibrido", "Presencial" };
        return datos;
    }

    int aleatorio(int minimo, int maximo) {
        std::uniform_int_distribution<int> distribucion(minimo, maximo);
        return distribucion(motor);
    }

    // Plantilla generica: permite seleccionar elementos de cualquiera de las listas base.
    template <typename T>
    const T& elegir(const std::vector<T>& lista) {
        std::uniform_int_distribution<std::size_t> distribucion(0, lista.size() - 1);
        return lista[distribucion(motor)];
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
        std::vector<std::string> opciones = habilidades();
        std::shuffle(opciones.begin(), opciones.end(), motor);

        const int cantidad = aleatorio(2, 4);
        std::ostringstream salida;
        for (int i = 0; i < cantidad; ++i) {
            if (i > 0) salida << ", ";
            salida << opciones[i];
        }
        return salida.str();
    }

    bool generarUsuarios(RedProfesional& red, int cantidad,
        std::vector<int>& idsUsuarios) {
        idsUsuarios.reserve(cantidad);

        for (int i = 1; i <= cantidad; ++i) {
            const std::string correo = crearCorreo("persona", i);
            const int id = red.registrarUsuario(
                elegir(nombres()),
                elegir(apellidos()),
                elegir(titulares()),
                elegir(distritos()),
                correo,
                "Demo1234"
            );

            if (id < 0) return false;
            idsUsuarios.push_back(id);
        }

        // Se agregan habilidades directamente al perfil para no llenar la pila
        // de acciones reversibles con operaciones internas del generador.
        for (std::size_t i = 0; i < idsUsuarios.size(); ++i) {
            Usuario* usuario = red.buscarUsuario(idsUsuarios[i]);
            if (usuario == nullptr) return false;

            std::vector<std::string> opciones = habilidades();
            std::shuffle(opciones.begin(), opciones.end(), motor);
            const int cantidadHabilidades = aleatorio(2, 5);

            for (int j = 0; j < cantidadHabilidades; ++j) {
                const NivelHabilidad nivel = static_cast<NivelHabilidad>(aleatorio(0, 3));
                usuario->agregarHabilidad(Habilidad(opciones[j], nivel));
            }
        }

        return true;
    }

    bool generarEmpresas(RedProfesional& red, int cantidad,
        std::vector<int>& idsEmpresas) {
        idsEmpresas.reserve(cantidad);

        for (int i = 1; i <= cantidad; ++i) {
            const std::string nombre = elegir(nombresEmpresas()) + " " + numeroConCeros(i);
            const int id = red.registrarEmpresa(
                nombre,
                elegir(sectores()),
                elegir(distritos()),
                crearCorreo("empresa", i),
                "Demo1234"
            );

            if (id < 0) return false;
            idsEmpresas.push_back(id);
        }

        return true;
    }

    bool generarVacantes(RedProfesional& red, int cantidad,
        const std::vector<int>& idsEmpresas) {
        if (cantidad > 0 && idsEmpresas.empty()) return false;

        for (int i = 0; i < cantidad; ++i) {
            const int idEmpresa = idsEmpresas[aleatorio(0, (int)idsEmpresas.size() - 1)];
            const int id = red.publicarVacante(
                idEmpresa,
                elegir(titulosVacantes()),
                elegir(descripcionesVacantes()),
                crearRequisitos(),
                elegir(modalidades())
            );

            if (id < 0) return false;
        }

        return true;
    }

public:
    static const int MAX_REGISTROS = 10000;

    explicit GeneradorDataset(unsigned int semilla) : motor(semilla) {}

    // Genera exactamente las cantidades solicitadas.
    // La red debe ser nueva; al finalizar se reconstruyen los indices hash y AVL.
    bool generar(RedProfesional& red, int cantidadUsuarios,
        int cantidadEmpresas, int cantidadVacantes) {
        if (cantidadUsuarios < 1 || cantidadEmpresas < 1 || cantidadVacantes < 0 ||
            cantidadUsuarios > MAX_REGISTROS || cantidadEmpresas > MAX_REGISTROS ||
            cantidadVacantes > MAX_REGISTROS) {
            return false;
        }

        std::vector<int> idsUsuarios;
        std::vector<int> idsEmpresas;

        if (!generarUsuarios(red, cantidadUsuarios, idsUsuarios)) return false;
        if (!generarEmpresas(red, cantidadEmpresas, idsEmpresas)) return false;
        if (!generarVacantes(red, cantidadVacantes, idsEmpresas)) return false;

        // Las habilidades se insertaron directamente en cada Usuario.
        // Esta reconstruccion las incorpora al AVL y vuelve a indexar los correos.
        red.reconstruirIndicesPublico();
        return true;
    }
};

