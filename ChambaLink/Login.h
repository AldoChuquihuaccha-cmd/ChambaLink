#pragma once
#include <string>
#include "Consola.h"
#include "RedProfesional.h"
#include "GestorArchivos.h"

enum class TipoCuenta { Individuo, Empresa };

// Pantallas de inicio de ChambaLink:
//   1. Logo y eleccion entre INDIVIDUO y EMPRESA (flechas izquierda/derecha).
//   2. Menu Iniciar sesion / Registrarse (flechas arriba/abajo).
//   3. Formularios de inicio de sesion y de registro.
//   4. Bienvenida.
// En todos los menus la flecha "->" se queda quieta si ya esta en un extremo,
// ENTER confirma y ESC sale o retrocede. Cualquier otra tecla no hace nada.
//
// Esta clase solo dibuja y lee el teclado. Las reglas (validar, registrar,
// buscar la cuenta) estan en RedProfesional, y los errores que muestra en
// rojo son los que deja RedProfesional en getUltimoError().
class Login {
private:
    RedProfesional& red;
    int idSesion;
    TipoCuenta tipoSesion;

    // ================= Distribucion de la pantalla (filas 0 a 29) =================
    static const int FILA_LOGO = 2;
    static const int FILA_PREGUNTA = 11;
    static const int FILA_OPCIONES = 14;        // arte de INDIVIDUO y EMPRESA
    static const int SEPARACION_OPCIONES = 10;  // espacio entre los dos dibujos

    static const int FILA_TITULO = 1;           // arte del tipo de cuenta en las demas pantallas
    static const int FILA_SUBTITULO = 8;
    static const int FILA_NOTA = 9;
    static const int FILA_INICIAR = 13;         // menu Iniciar sesion / Registrarse
    static const int FILA_REGISTRAR = 15;
    static const int FILA_PRIMER_CAMPO = 11;    // los campos van cada 2 filas
    static const int COLUMNA_ETIQUETA = 22;
    static const int COLUMNA_CAMPO = 53;
    static const int FILA_MENSAJE = 23;         // error en rojo o aviso de exito

    static const int FILA_AYUDA_1 = 25;
    static const int FILA_AYUDA_2 = 27;

    // ================= Arte =================
    // Cada dibujo es un arreglo de strings del mismo ancho. Se imprime con un
    // solo for: una vuelta por fila.
    static const int ALTO_LOGO = 7;
    static const int ALTO_OPCION = 5;

    static const std::string* arteLogo() {
        static const std::string LINEAS[] = {
            " ######  ##     ##    ###    ##     ## ########     ###    ##       #### ##    ## ##    ##",
            "##    ## ##     ##   ## ##   ###   ### ##     ##   ## ##   ##        ##  ###   ## ##   ## ",
            "##       ##     ##  ##   ##  #### #### ##     ##  ##   ##  ##        ##  ####  ## ##  ##  ",
            "##       ######### ##     ## ## ### ## ########  ##     ## ##        ##  ## ## ## #####   ",
            "##       ##     ## ######### ##     ## ##     ## ######### ##        ##  ##  #### ##  ##  ",
            "##    ## ##     ## ##     ## ##     ## ##     ## ##     ## ##        ##  ##   ### ##   ## ",
            " ######  ##     ## ##     ## ##     ## ########  ##     ## ######## #### ##    ## ##    ##",
        };
        return LINEAS;
    }

    static const std::string* arteIndividuo() {
        static const std::string LINEAS[] = {
            " ___ _   _ ____ _____     _____ ____  _   _  ___  ",
            "|_ _| \\ | |  _ \\_ _\\ \\   / /_ _|  _ \\| | | |/ _ \\ ",
            " | ||  \\| | | | | | \\ \\ / / | || | | | | | | | | |",
            " | || |\\  | |_| | |  \\ V /  | || |_| | |_| | |_| |",
            "|___|_| \\_|____/___|  \\_/  |___|____/ \\___/ \\___/ ",
        };
        return LINEAS;
    }

    static const std::string* arteEmpresa() {
        static const std::string LINEAS[] = {
            " _____ __  __ ____  ____  _____ ____    _    ",
            "| ____|  \\/  |  _ \\|  _ \\| ____/ ___|  / \\   ",
            "|  _| | |\\/| | |_) | |_) |  _| \\___ \\ / _ \\  ",
            "| |___| |  | |  __/|  _ <| |___ ___) / ___ \\ ",
            "|_____|_|  |_|_|   |_| \\_\\_____|____/_/   \\_\\",
        };
        return LINEAS;
    }

    static int ancho(const std::string* arte) { return (int)arte[0].length(); }

    static void dibujar(const std::string* arte, int alto, int fila, int columna) {
        for (int i = 0; i < alto; i++)
            Consola::escribir(fila + i, columna, arte[i]);
    }

    static void dibujarCentrado(const std::string* arte, int alto, int fila) {
        dibujar(arte, alto, fila, Consola::columnaCentrada(ancho(arte)));
    }

    static const std::string* arteDe(TipoCuenta tipo) {
        if (tipo == TipoCuenta::Individuo) return arteIndividuo();
        return arteEmpresa();
    }

    // Espera ENTER o ESC e ignora las demas teclas. Devuelve true si fue ENTER.
    static bool esperarEnterOEsc() {
        while (true) {
            Tecla tecla = Consola::leerTecla();
            if (tecla == Tecla::Enter) return true;
            if (tecla == Tecla::Esc) return false;
        }
    }

    // ================= Pantalla 1: Individuo o Empresa =================
    // Devuelve false si se presiono ESC (salir del programa).
    bool elegirTipo(TipoCuenta& tipo) {
        // "-> " + INDIVIDUO + separacion + "-> " + EMPRESA, todo centrado.
        int anchoTotal = 3 + ancho(arteIndividuo()) + SEPARACION_OPCIONES + 3 + ancho(arteEmpresa());
        int columnaFlechaIndividuo = Consola::columnaCentrada(anchoTotal);
        int columnaIndividuo = columnaFlechaIndividuo + 3;
        int columnaFlechaEmpresa = columnaIndividuo + ancho(arteIndividuo()) + SEPARACION_OPCIONES;
        int columnaEmpresa = columnaFlechaEmpresa + 3;
        int filaFlecha = FILA_OPCIONES + ALTO_OPCION / 2;   // a media altura del dibujo

        Consola::limpiar();
        dibujarCentrado(arteLogo(), ALTO_LOGO, FILA_LOGO);
        Consola::escribirCentrado(FILA_PREGUNTA, "Como deseas ingresar");
        dibujar(arteIndividuo(), ALTO_OPCION, FILA_OPCIONES, columnaIndividuo);
        dibujar(arteEmpresa(), ALTO_OPCION, FILA_OPCIONES, columnaEmpresa);
        Consola::escribirCentrado(FILA_AYUDA_1, "Presione ENTER para seleccionar");
        Consola::escribirCentrado(FILA_AYUDA_2, "Presione ESC para salir");

        while (true) {
            // Solo se redibuja la flecha, no toda la pantalla.
            Consola::escribir(filaFlecha, columnaFlechaIndividuo, tipo == TipoCuenta::Individuo ? "->" : "  ");
            Consola::escribir(filaFlecha, columnaFlechaEmpresa, tipo == TipoCuenta::Empresa ? "->" : "  ");

            Tecla tecla = Consola::leerTecla();
            if (tecla == Tecla::Izquierda) tipo = TipoCuenta::Individuo;
            else if (tecla == Tecla::Derecha) tipo = TipoCuenta::Empresa;
            else if (tecla == Tecla::Enter) return true;
            else if (tecla == Tecla::Esc) return false;
        }
    }

    // ================= Pantalla 2: Iniciar sesion o Registrarse =================
    // opcion: 0 = Iniciar sesion, 1 = Registrarse. Devuelve false si se presiono ESC.
    bool elegirAccion(TipoCuenta tipo, int& opcion) {
        int columnaFlecha = Consola::columnaCentrada(3 + 14);   // "-> " + "Iniciar sesion"
        int columnaTexto = columnaFlecha + 3;

        Consola::limpiar();
        dibujarCentrado(arteDe(tipo), ALTO_OPCION, FILA_TITULO);
        Consola::escribir(FILA_INICIAR, columnaTexto, "Iniciar sesion");
        Consola::escribir(FILA_REGISTRAR, columnaTexto, "Registrarse");
        Consola::escribirCentrado(FILA_AYUDA_1, "Presione ENTER para seleccionar");
        Consola::escribirCentrado(FILA_AYUDA_2, "Presione ESC para retroceder");

        while (true) {
            Consola::escribir(FILA_INICIAR, columnaFlecha, opcion == 0 ? "->" : "  ");
            Consola::escribir(FILA_REGISTRAR, columnaFlecha, opcion == 1 ? "->" : "  ");

            Tecla tecla = Consola::leerTecla();
            if (tecla == Tecla::Arriba) opcion = 0;
            else if (tecla == Tecla::Abajo) opcion = 1;
            else if (tecla == Tecla::Enter) return true;
            else if (tecla == Tecla::Esc) return false;
        }
    }

    // ================= Formularios =================
    void dibujarFormulario(TipoCuenta tipo, const std::string& subtitulo, const std::string& nota,
        const std::string etiquetas[], int cantidad) {
        Consola::limpiar();
        dibujarCentrado(arteDe(tipo), ALTO_OPCION, FILA_TITULO);
        Consola::escribirCentrado(FILA_SUBTITULO, subtitulo);
        if (nota != "") Consola::escribirCentrado(FILA_NOTA, nota);
        for (int i = 0; i < cantidad; i++)
            Consola::escribir(FILA_PRIMER_CAMPO + 2 * i, COLUMNA_ETIQUETA, etiquetas[i]);
        Consola::escribirCentrado(FILA_AYUDA_1, "Presione ENTER para pasar al siguiente campo");
        Consola::escribirCentrado(FILA_AYUDA_2, "Presione ESC para retroceder");
    }

    // Lee un campo tecla por tecla con _getch. No deja escribir mas de
    // largoMaximo caracteres. Devuelve false si se presiono ESC.
    bool leerCampo(int fila, int largoMaximo, std::string& valor) {
        Consola::mostrarCursor(true);
        while (true) {
            int columnaActual = COLUMNA_CAMPO + (int)valor.length();
            Consola::moverCursor(fila, columnaActual);

            char caracter;
            Tecla tecla = Consola::leerTecla(caracter);
            if (tecla == Tecla::Enter || tecla == Tecla::Esc) {
                Consola::mostrarCursor(false);
                return tecla == Tecla::Enter;
            }
            if (tecla == Tecla::Retroceso && valor != "") {
                valor.pop_back();
                Consola::escribir(fila, columnaActual - 1, " ");
            }
            else if (tecla == Tecla::Caracter && (int)valor.length() < largoMaximo) {
                Consola::escribir(fila, columnaActual, std::string(1, caracter));
                valor += caracter;
            }
        }
    }

    // Llena los campos en orden; ENTER en el ultimo envia el formulario.
    // Devuelve false si se presiono ESC en cualquiera.
    bool llenarCampos(const int largos[], std::string valores[], int cantidad) {
        for (int i = 0; i < cantidad; i++) valores[i] = "";
        for (int i = 0; i < cantidad; i++)
            if (!leerCampo(FILA_PRIMER_CAMPO + 2 * i, largos[i], valores[i])) return false;
        return true;
    }

    // Muestra el error en rojo debajo del formulario.
    // Devuelve true con ENTER (reintentar) y false con ESC (retroceder).
    // Al reintentar se vuelve a dibujar el formulario: se borra el rojo y los campos.
    bool mostrarError(const std::string& mensaje) {
        Consola::limpiarFila(FILA_AYUDA_1);   // ya no aplica "pasar al siguiente campo"
        Consola::escribirCentradoRojo(FILA_MENSAJE, mensaje);
        Consola::escribirCentrado(FILA_MENSAJE + 1, "Presione ENTER para intentar de nuevo");
        return esperarEnterOEsc();
    }

    // Devuelve true si la sesion se inicio, false si se presiono ESC.
    bool iniciarSesion(TipoCuenta tipo) {
        const std::string etiquetas[] = { "Correo:", "Contrasena:" };
        const int largos[] = { RedProfesional::MAX_CORREO, RedProfesional::MAX_CONTRASENA };
        std::string valores[2];

        while (true) {
            dibujarFormulario(tipo, "Iniciar sesion", "", etiquetas, 2);
            if (!llenarCampos(largos, valores, 2)) return false;

            int id;
            if (tipo == TipoCuenta::Individuo) id = red.iniciarSesionUsuario(valores[0], valores[1]);
            else id = red.iniciarSesionEmpresa(valores[0], valores[1]);

            if (id != -1) {
                idSesion = id;
                tipoSesion = tipo;
                return true;
            }
            if (!mostrarError(red.getUltimoError())) return false;
        }
    }

    // Al terminar (con exito o con ESC) se vuelve al menu Iniciar sesion / Registrarse.
    void registrar(TipoCuenta tipo) {
        bool esIndividuo = (tipo == TipoCuenta::Individuo);

        const std::string etiquetasIndividuo[] = { "Nombre:", "Apellido:", "Titular:",
            "Distrito de residencia:", "Correo:", "Contrasena:" };
        const int largosIndividuo[] = { RedProfesional::MAX_NOMBRE, RedProfesional::MAX_APELLIDO,
            RedProfesional::MAX_TITULAR, RedProfesional::MAX_UBICACION,
            RedProfesional::MAX_CORREO, RedProfesional::MAX_CONTRASENA };

        const std::string etiquetasEmpresa[] = { "Nombre de la empresa:", "Sector:",
            "Distrito del establecimiento:", "Correo:", "Contrasena:" };
        const int largosEmpresa[] = { RedProfesional::MAX_NOMBRE_EMPRESA, RedProfesional::MAX_SECTOR,
            RedProfesional::MAX_UBICACION, RedProfesional::MAX_CORREO, RedProfesional::MAX_CONTRASENA };

        const std::string* etiquetas = esIndividuo ? etiquetasIndividuo : etiquetasEmpresa;
        const int* largos = esIndividuo ? largosIndividuo : largosEmpresa;
        int cantidad = esIndividuo ? 6 : 5;
        std::string nota = esIndividuo ? "El titular y el distrito son opcionales"
            : "El sector y el distrito son opcionales";
        std::string valores[6];

        while (true) {
            dibujarFormulario(tipo, "Registro", nota, etiquetas, cantidad);
            if (!llenarCampos(largos, valores, cantidad)) return;

            int id;
            if (esIndividuo)
                id = red.registrarUsuario(valores[0], valores[1], valores[2], valores[3], valores[4], valores[5]);
            else
                id = red.registrarEmpresa(valores[0], valores[1], valores[2], valores[3], valores[4]);

            if (id != -1) {
                // Se guarda enseguida: si la ventana se cierra con la X, la cuenta no se pierde.
                GestorArchivos::guardarTodo(red);
                Consola::limpiarFila(FILA_AYUDA_1);
                Consola::escribirCentrado(FILA_MENSAJE, "Cuenta creada con exito. Tu codigo es " + std::to_string(id));
                Consola::escribirCentrado(FILA_MENSAJE + 1, "Presione ENTER para continuar");
                esperarEnterOEsc();
                return;
            }
            if (!mostrarError(red.getUltimoError())) return;
        }
    }

    // Menu de una cuenta: repite Iniciar sesion / Registrarse hasta que alguien
    // entre (true) o se presione ESC para volver a la primera pantalla (false).
    bool menuCuenta(TipoCuenta tipo) {
        int opcion = 0;
        while (true) {
            if (!elegirAccion(tipo, opcion)) return false;
            if (opcion == 0) {
                if (iniciarSesion(tipo)) return true;
            }
            else {
                registrar(tipo);
            }
        }
    }

public:
    Login(RedProfesional& pRed) : red(pRed) {
        idSesion = -1;
        tipoSesion = TipoCuenta::Individuo;
    }

    // Muestra el inicio hasta que alguien inicie sesion (true)
    // o se presione ESC en la primera pantalla (false).
    bool mostrar() {
        TipoCuenta tipo = TipoCuenta::Individuo;
        while (true) {
            if (!elegirTipo(tipo)) return false;
            if (menuCuenta(tipo)) return true;
        }
    }

    int getIdSesion() const { return idSesion; }
    bool esEmpresa() const { return tipoSesion == TipoCuenta::Empresa; }

    // Por ahora solo saluda. ESC cierra la sesion y vuelve al inicio.
    void mostrarBienvenida() {
        std::string nombre;
        if (esEmpresa()) nombre = red.nombreEmpresa(idSesion);
        else nombre = red.nombreDe(idSesion);

        Consola::limpiar();
        dibujarCentrado(arteLogo(), ALTO_LOGO, FILA_LOGO);
        Consola::escribirCentrado(14, "Bienvenido, " + nombre);
        Consola::escribirCentrado(FILA_AYUDA_2, "Presione ESC para cerrar sesion");
        while (Consola::leerTecla() != Tecla::Esc) {}
    }
};
