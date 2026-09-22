// Prueba de integracion de ChambaLink (temporal).
// Verifica que las clases se conectan bien antes de construir el menu.
// Cuando exista Menu.h, este main se reemplaza por el menu de consola.
#include <iostream>
#include "RedProfesional.h"

using namespace std;

static void mostrarContactos(const RedProfesional& red, int idUsuario) {
    const Usuario* u = red.buscarUsuario(idUsuario);
    cout << "Contactos de " << u->getNombreCompleto() << ": ";
    u->paraCadaContacto([&red](const int& id) { cout << red.nombreDe(id) << " | "; });
    cout << "(" << u->totalContactos() << ")\n";
}

static void verificar(bool condicion, const string& texto) {
    cout << (condicion ? "[OK]    " : "[FALLA] ") << texto << "\n";
}

int main() {
    RedProfesional red;

    int ana = red.registrarUsuario("Ana", "Torres", "Ingeniera de Software", "Lima");
    int luis = red.registrarUsuario("Luis", "Rojas", "Analista de Datos", "Arequipa");
    int eva = red.registrarUsuario("Eva", "Diaz", "Disenadora UX", "Lima");

    // ---------- Perfil: se modifica el usuario REAL, no una copia ----------
    Usuario* pAna = red.buscarUsuario(ana);
    pAna->agregarHabilidad(Habilidad("C++", NivelHabilidad::Intermedio));
    pAna->agregarHabilidad(Habilidad("C++", NivelHabilidad::Avanzado));   // actualiza el nivel
    pAna->agregarExperiencia(ExperienciaLaboral(0, "Startup X", "Practicante", 2022, 2023, "Backend"));
    pAna->agregarExperiencia(ExperienciaLaboral(0, "Banco Y", "Desarrolladora", 2023, 0, "APIs"));
    red.agregarCertificacion(ana, "AWS Cloud Practitioner", "Amazon", "2025/05/10", "AWS-123");
    verificar(red.buscarUsuario(ana)->totalHabilidades() == 1, "Habilidad repetida actualiza en vez de duplicar");

    cout << "Historial de Ana (reciente -> antiguo): ";
    pAna->paraCadaExperiencia([](const ExperienciaLaboral& e) { cout << e.getCargo() << " | "; }, true);
    cout << "\n";

    // ---------- Solicitudes (cola) y contactos ----------
    verificar(red.enviarSolicitud(luis, ana, "Hola Ana"), "Luis envia solicitud a Ana");
    verificar(red.enviarSolicitud(eva, ana, "Conectemos"), "Eva envia solicitud a Ana");
    // Se evalua la operacion ANTES de leer getUltimoError (el orden de evaluacion
    // de los argumentos de una funcion no esta garantizado en C++).
    bool ok = red.enviarSolicitud(luis, ana, "Otra vez");
    verificar(!ok, "Solicitud duplicada rechazada: " + red.getUltimoError());
    ok = red.enviarSolicitud(ana, ana, "Yo");
    verificar(!ok, "Solicitud a uno mismo rechazada: " + red.getUltimoError());

    red.responderSiguienteSolicitud(ana, true);    // FIFO: primero Luis
    red.responderSiguienteSolicitud(ana, true);    // luego Eva
    mostrarContactos(red, ana);
    verificar(red.buscarUsuario(luis)->esContactoDirecto(ana), "La conexion es simetrica (Luis tambien tiene a Ana)");

    // ---------- Deshacer (pila + Command) ----------
    string deshecho;
    red.deshacerUltimaAccion(ana, deshecho);
    cout << "Deshecho: " << deshecho << "\n";
    mostrarContactos(red, ana);
    verificar(!red.buscarUsuario(eva)->esContactoDirecto(ana), "Deshacer quito la conexion en ambos lados");

    // ---------- Contenido (herencia + polimorfismo) ----------
    int pub = red.publicar(ana, "Busco equipo para un proyecto en C++");
    int com = red.comentar(luis, pub, "Me interesa");
    red.comentar(ana, pub, "Te escribo por mensaje", com);
    verificar(red.darMeGusta(luis, pub), "Luis da me gusta");
    verificar(!red.darMeGusta(luis, pub), "Me gusta repetido rechazado");
    red.recomendar(ana, luis, "Excelente analista");
    red.enviarMensaje(ana, luis, "Hola Luis");
    ok = red.enviarMensaje(ana, eva, "Hola Eva") == -1;
    verificar(ok, "No se escribe a no contactos: " + red.getUltimoError());

    cout << "Actividad de Ana (polimorfica):\n";
    Lista<const Contenido*> actividad = red.actividadDe(ana);
    for (const Contenido* c : actividad) cout << "  " << c->resumen() << "\n";

    // ---------- Empleo (cola por vacante) ----------
    int emp = red.registrarEmpresa("TechPeru", "Tecnologia", "Lima");
    int vac = red.publicarVacante(emp, "Dev C++ Jr", "Backend", "C++, POO", "Hibrido");
    red.postular(luis, vac);
    red.postular(eva, vac);
    verificar(red.postular(luis, vac) == -1, "Postulacion duplicada rechazada");
    red.revisarSiguientePostulacion(vac, true);    // FIFO: primero Luis
    red.paraCadaPostulacionDe(luis, [&red](const Postulacion& p) {
        cout << "Postulacion de Luis: " << p.estadoToString() << "\n";
        });

    // ---------- Grupos ----------
    int grp = red.crearGrupo("C++ Peru", "Comunidad", "Programacion");
    red.unirseAGrupo(ana, grp);
    verificar(!red.unirseAGrupo(ana, grp), "No se puede unir dos veces a un grupo");

    // ---------- Notificaciones (cola) ----------
    Usuario* pLuis = red.buscarUsuario(luis);
    cout << "Notificaciones de Luis (" << pLuis->cantidadNotificaciones() << "):\n";
    while (pLuis->tieneNotificaciones()) {
        Notificacion n = pLuis->leerSiguienteNotificacion();
        cout << "  [" << n.tipoToString() << "] " << n.getMensaje() << "\n";
    }
    return 0;
}
