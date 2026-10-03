# Análisis del UML de ChambaLink vs. código

Comparación del archivo `UML - ChambaLink.drawio` con el código actual (3 oct 2026), miembro por miembro.

Notación: `+` público, `-` privado, `#` protegido. Las líneas ya están en el mismo formato del diagrama, listas para copiar.

## Resumen

| | Cantidad |
|---|---|
| Clases que ya coinciden con el código | 16 (Certificacion, Comentario, Contenido, ExperienciaLaboral, GrupoProfesional, Habilidad, Lista::Iterador, ListaDoble::Iterador, Mensaje, Nodo, NodoDoble, Notificacion, Postulacion, Publicacion, Recomendacion, SolicitudConexion) |
| Clases con cambios | 10 |
| Miembros que faltan | 121 |
| Firmas distintas (hay que corregirlas) | 7 |
| Miembros que ya no existen (hay que borrarlos) | 4 |
| Clases, structs y enums que faltan | 2 structs + 4 enums |
| Módulos de funciones sin clase | Ordenamiento (11), FuncionesInicio (21), FuncionesMenu (70) |

## 1. Cambios por clase

### Accion

**Agregar atributos:**

```
- categoria: string
```

**Agregar métodos:**

```
+ Accion(pCategoria: string, pDescripcion: string, pHacer: function, pDeshacer: function)
+ getCategoria(): string
```

### Cola

**Agregar métodos:**

```
+ rotar(): void
+ transferirA(destino: Cola<T>&): void
```

### Empresa

**Agregar atributos:**

```
- correo: string
- contrasena: string
```

**Agregar métodos:**

```
+ Empresa(pId: int, pNombre: string, pSector: string, pUbicacion: string, pCorreo: string, pContrasena: string)
+ getCorreo(): string
+ getContrasena(): string
+ tieneCorreo(pCorreo: string): bool
+ contrasenaCorrecta(pContrasena: string): bool
```

### GestorArchivos

**Corregir (la firma cambió):**

| En el UML | Debe decir |
|---|---|
| `- actualizarContador(contador: int&, idLeido: int): void {static}` | `- actualizarContador(contador: int&, id: int): void {static}` |
| `+ guardarTodo(red: RedProfesional): bool {static}` | `+ guardarTodo(red: RedProfesional): void {static}` |
| `+ cargarTodo(red: RedProfesional&): bool {static}` | `+ cargarTodo(red: RedProfesional&): void {static}` |

**Agregar métodos:**

```
- entero(c: Lista<string>, pos: uint): int {static}
- campo(c: Lista<string>, pos: uint): string {static}
- leerLineas(nombre: string): Lista<string> {static}
- guardarTexto(red: RedProfesional): void {static}
- cargarTexto(red: RedProfesional&): bool {static}
- escribirEntero(out: ofstream&, numero: int): void {static}
- escribirTexto(out: ofstream&, t: string): void {static}
- leerEntero(in: ifstream&): int {static}
- leerTexto(in: ifstream&): string {static}
- guardarBinario(red: RedProfesional): void {static}
- cargarBinario(red: RedProfesional&): bool {static}
- vaciar(red: RedProfesional&): void {static}
+ cargarDatosDeEjemplo(red: RedProfesional&): void {static}
- habilidad(red: RedProfesional&, idUsuario: int, nombre: string, nivel: int): void {static}
- publicacion(red: RedProfesional&, idAutor: int, texto: string, fecha: string): int {static}
```

**Borrar (ya no existen):**

```
- deEntero(numero: int): string {static}
- deBool(valor: bool): string {static}
- aEntero(texto: string): int {static}
- aBool(texto: string): bool {static}
```

### Lista

**Agregar métodos:**

```
+ insertarOrdenado(elem: T, antesQue: function): void
+ maximoSegun(valor: function): T
```

### ListaDoble

- Corregir el título: dice **"ClasListaDoble"**, debe decir **"Class ListaDoble"**.
**Agregar métodos:**

```
+ insertarOrdenado(elem: T, antesQue: function): void
+ eliminaPos(pos: uint): void
+ contarSi(criterio: function): uint
```

### Pila

**Agregar métodos:**

```
+ contarSi(criterio: function): int
+ invertir(): void
```

### RedProfesional

**Corregir (la firma cambió):**

| En el UML | Debe decir |
|---|---|
| `+ registrarUsuario(nombre: string, apellido: string, titular: string, ubicacion: string): int` | `+ registrarUsuario(nombre: string, apellido: string, titular: string, ubicacion: string, correo: string, contrasena: string): int` |
| `+ registrarEmpresa(nombre: string, sector: string, ubicacion: string): int` | `+ registrarEmpresa(nombre: string, sector: string, ubicacion: string, correo: string, contrasena: string): int` |
| `+ revisarSiguientePostulacion(idVacante: int, aceptar: bool): bool` | `+ revisarSiguientePostulacion(idEmpresa: int, idVacante: int, aceptar: bool): bool` |

**Agregar atributos:**

```
+ MAX_NOMBRE: int = 30 {static}
+ MAX_APELLIDO: int = 30 {static}
+ MAX_TITULAR: int = 50 {static}
+ MAX_UBICACION: int = 30 {static}
+ MAX_CORREO: int = 50 {static}
+ MAX_CONTRASENA: int = 20 {static}
+ MAX_NOMBRE_EMPRESA: int = 50 {static}
+ MAX_SECTOR: int = 30 {static}
+ MAX_CERTIFICACION: int = 40 {static}
+ MAX_INSTITUCION: int = 40 {static}
+ MAX_CODIGO: int = 30 {static}
+ MAX_PUBLICACION: int = 121 {static}
+ MAX_COMENTARIO: int = 60 {static}
+ MAX_MENSAJE: int = 58 {static}
+ MAX_RECOMENDACION: int = 121 {static}
+ MAX_HABILIDAD: int = 30 {static}
+ MAX_CARGO: int = 40 {static}
```

**Agregar métodos:**

```
- separarRequisitos(texto: string): Lista<string>
- compararRequisitosRec(actual: Lista<string>::Iterador, fin: Lista<string>::Iterador, usuario: Usuario): int
- textoMinusculas(texto: string): string {static}
- aAnio(texto: string): int {static}
- haySolicitudPendienteEntre(idA: int, idB: int): bool
- contactosEnComun(idA: int, idB: int): int
- explorarConexionesRec(idActual: int, profundidad: int, visitados: Lista<int>&, descubiertos: Lista<int>&): void
- recorrerRespuestas(idPublicacion: int, idPadre: int, nivel: int, accion: function): void
- validarCampo(valor: string, nombreCampo: string, largoMaximo: int, obligatorio: bool): bool
- correoEnUso(correo: string): bool
- validarCredenciales(correo: string, contrasena: string): bool
+ RedProfesional(otra: RedProfesional)
+ iniciarSesionUsuario(correo: string, contrasena: string): int
+ obtenerEmpresa(idEmpresa: int): Empresa*
+ nombreNivel(nivel: int): string {static}
+ agregarHabilidad(idUsuario: int, nombre: string, nivel: int): bool
+ eliminarHabilidad(idUsuario: int, nombre: string): bool
+ agregarExperiencia(idUsuario: int, empresa: string, cargo: string, anioInicio: string, anioFin: string): bool
+ eliminarExperiencia(idUsuario: int, pos: int): bool
+ esFechaValida(fecha: string): bool {static}
+ certificacionesOrdenadas(idUsuario: int): Lista<Certificacion>
+ contactosOrdenadosPorNombre(idUsuario: int): Lista<int>
+ buscarPersonas(idUsuario: int, texto: string): Lista<int>
+ sugerenciasConexion(idUsuario: int): Lista<SugerenciaConexion>
+ sonContactos(idA: int, idB: int): bool
+ existeSolicitudPendienteEntre(idA: int, idB: int): bool
+ quitarMeGusta(idUsuario: int, idPublicacion: int): bool
+ obtenerPublicacion(idPublicacion: int): Publicacion*
+ contarComentarios(idPublicacion: int): int
+ feedPorFecha(): Lista<Publicacion>
+ feedPorPopularidad(): Lista<Publicacion>
+ paraCadaComentarioEnHilo(idPublicacion: int, accion: function): void
+ cantidadMensajesNoLeidos(idUsuario: int): uint
+ cantidadMensajesNoLeidosDe(idUsuario: int, idContacto: int): uint
+ cantidadMensajesEntre(idA: int, idB: int): uint
+ obtenerConversacion(idA: int, idB: int): Lista<Mensaje>
+ paraCadaRecomendacionDe(idEmisor: int, accion: function): void
+ obtenerRecomendacion(idRecomendacion: int): Recomendacion*
+ recomendacionesRecibidas(idReceptor: int): Lista<Recomendacion>
+ recomendacionesEnviadas(idEmisor: int): Lista<Recomendacion>
+ iniciarSesionEmpresa(correo: string, contrasena: string): int
+ verSiguientePostulacion(idEmpresa: int, idVacante: int): Postulacion*
+ cancelarVacante(idEmpresa: int, idVacante: int): bool
+ rotarPostulacionesVacante(idVacante: int): bool
+ obtenerVacante(idVacante: int): Vacante*
+ paraCadaVacanteDe(idEmpresa: int, accion: function): void
+ paraCadaContratacion(idEmpresa: int, accion: function): void
+ contiene(texto: string, palabra: string): bool {static}
+ buscarProfesionales(palabrasClave: string, totalPalabras: int&): Lista<Coincidencia>
+ vacantesOrdenadasDe(idEmpresa: int): Lista<Vacante>
+ calcularCompatibilidad(idUsuario: int, idVacante: int): int
```

### Usuario

**Corregir (la firma cambió):**

| En el UML | Debe decir |
|---|---|
| `+ paraCadaAccion(accion: function): void` | `+ paraCadaAccion(accion: function, desdeMasAntigua: bool): void` |

**Agregar atributos:**

```
- correo: string
- contrasena: string
```

**Agregar métodos:**

```
+ Usuario(pId: int, pNombre: string, pApellido: string, pTitular: string, pUbicacion: string, pCorreo: string, pContrasena: string)
+ getCorreo(): string
+ getContrasena(): string
+ tieneCorreo(pCorreo: string): bool
+ contrasenaCorrecta(pContrasena: string): bool
+ buscarHabilidad(nombreHabilidad: string): Habilidad*
+ habilidadPrincipal(): string
+ eliminarExperiencia(pos: uint): bool
+ cantidadExperienciasActuales(): uint
+ totalAcciones(): uint
+ ultimaAccion(): Accion
+ contarAcciones(categoria: string): int
+ cantidadNotificacionesNoLeidas(): uint
+ marcarNotificacionLeida(idNotificacion: int): bool
+ marcarTodasNotificacionesLeidas(): void
```

### Vacante

**Agregar métodos:**

```
+ rotarPostulaciones(): void
+ verSiguientePostulacion(): int
```

## 2. Lo que no está en el diagrama

### Structs (en RedProfesional.h)

**struct Coincidencia**

```
+ idUsuario: int
+ cantidad: int
```

**struct SugerenciaConexion**

```
+ idUsuario: int
+ contactosEnComun: int
```

### Enumeraciones («enumeration»)

- **NivelHabilidad** (Habilidad.h): Basico, Intermedio, Avanzado, Experto
- **TipoNotificacion** (Notificacion.h): NuevaSolicitud, NuevaConexion, NuevoComentario, MeGusta, NuevaRecomendacion, NuevoMensaje, NuevaVacante, EstadoPostulacion
- **EstadoPostulacion** (Postulacion.h): Pendiente, Revisada, Aceptada, Rechazada
- **EstadoSolicitud** (SolicitudConexion.h): Pendiente, Aceptada, Rechazada

### «utility» Ordenamiento (Ordenamiento.h)

Funciones template libres. Todas son `template <class T>`.

```
+ intercambiar(a: T&, b: T&): void
+ listaAArreglo(lista: Lista<T>): T*
+ arregloALista(arreglo: T*, cantidad: uint): Lista<T>
+ particionar(arreglo: T*, inicio: int, final: int, antesQue: function): int
+ quickSortArreglo(arreglo: T*, inicio: int, final: int, antesQue: function): void
+ quickSort(lista: Lista<T>, antesQue: function): Lista<T>
+ hundir(arreglo: T*, tamanio: int, posicion: int, antesQue: function): void
+ heapSort(lista: Lista<T>, antesQue: function): Lista<T>
+ mezclar(arreglo: T*, auxiliar: T*, inicio: int, medio: int, final: int, antesQue: function): void
+ mergeSortArreglo(arreglo: T*, auxiliar: T*, inicio: int, final: int, antesQue: function): void
+ mergeSort(lista: Lista<T>, antesQue: function): Lista<T>
```

### «utility» FuncionesInicio (FuncionesInicio.h) — interfaz de consola

```
+ ubicar(x: int, y: int): void
+ colorBlanco(): void
+ colorRojo(): void
+ limpiarPantalla(): void
+ configurarConsola(): void
+ leerTecla(): int
+ dibujarChambalink(): void
+ dibujarIndividuo(x: int, y: int): void
+ dibujarEmpresa(x: int, y: int): void
+ elegirTipo(esEmpresa: bool&): bool
+ elegirAccion(esEmpresa: bool, opcion: int&): bool
+ dibujarFormulario(esEmpresa: bool): void
+ leerCampoEn(x: int, y: int, maximo: int, valor: string&): bool
+ leerCampo(y: int, maximo: int, valor: string&): bool
+ mostrarError(mensaje: string): bool
+ mostrarExito(id: int): void
+ formularioInicioSesion(red: RedProfesional&, esEmpresa: bool, idSesion: int&): bool
+ formularioRegistroIndividuo(red: RedProfesional&): void
+ formularioRegistroEmpresa(red: RedProfesional&): void
+ menuCuenta(red: RedProfesional&, esEmpresa: bool, idSesion: int&): bool
+ mostrarLogin(red: RedProfesional&, idSesion: int&, esEmpresa: bool&): bool
```

### «utility» FuncionesMenu (FuncionesMenu.h) — interfaz de consola

```
+ dibujarLogoPequeno(): void
+ dibujarPie(): void
+ limpiarZonaContenido(): void
+ separadorConOrden(ordenamiento: string): void
+ mostrarSeccionPendiente(titulo: string): void
+ mostrarErrorZona(mensaje: string): bool
+ recortar(texto: string, ancho: int): string
+ valorOVacio(valor: string): string
+ textoHabilidades(red: RedProfesional&, idUsuario: int): string
+ mostrarPerfil(red: RedProfesional&, idUsuario: int, titulo: string): void
+ confirmarZona(y: int, pregunta: string): bool
+ formularioHabilidad(red: RedProfesional&, idSesion: int): void
+ seccionHabilidades(red: RedProfesional&, idSesion: int): void
+ formularioExperiencia(red: RedProfesional&, idSesion: int): void
+ seccionExperiencia(red: RedProfesional&, idSesion: int): void
+ seccionHistorialAcciones(red: RedProfesional&, idSesion: int): void
+ seccionMiPerfil(red: RedProfesional&, idSesion: int): void
+ formularioCertificacion(red: RedProfesional&, idSesion: int): void
+ seccionMisCertificaciones(red: RedProfesional&, idSesion: int): void
+ escribirTextoPublicacion(texto: string, x: int, y: int): void
+ formularioPublicar(red: RedProfesional&, idSesion: int): void
+ formularioComentario(red: RedProfesional&, idSesion: int, idPublicacion: int, idPadre: int, titulo: string): void
+ verPublicacion(red: RedProfesional&, idSesion: int, idPublicacion: int): void
+ seccionPublicaciones(red: RedProfesional&, idSesion: int, soloMias: bool): void
+ obtenerPostulacionUsuarioVacante(red: RedProfesional&, idUsuario: int, idVacante: int): Postulacion*
+ detalleEmpleoUsuario(red: RedProfesional&, idSesion: int, idVacante: int): void
+ seccionEmpleos(red: RedProfesional&, idSesion: int): void
+ seccionMisPostulaciones(red: RedProfesional&, idSesion: int): void
+ avisoRecomendaciones(mensaje: string): void
+ detalleRecomendacion(red: RedProfesional&, idSesion: int, idRecomendacion: int): void
+ listaRecomendaciones(red: RedProfesional&, idSesion: int, recibidas: bool): void
+ formularioRecomendacion(red: RedProfesional&, idSesion: int, idContacto: int): void
+ seleccionarContactoParaRecomendar(red: RedProfesional&, idSesion: int): void
+ seccionRecomendaciones(red: RedProfesional&, idSesion: int): void
+ avisoRed(mensaje: string): void
+ formularioSolicitudConexion(red: RedProfesional&, idSesion: int, idDestino: int): void
+ detallePersonaRed(red: RedProfesional&, idSesion: int, idPersona: int): void
+ seccionContactosRed(red: RedProfesional&, idSesion: int): void
+ seccionSolicitudesRed(red: RedProfesional&, idSesion: int): void
+ seccionBuscarPersonasRed(red: RedProfesional&, idSesion: int): void
+ seccionSugerenciasRed(red: RedProfesional&, idSesion: int): void
+ deshacerAccionRed(red: RedProfesional&, idSesion: int): void
+ seccionMiRed(red: RedProfesional&, idSesion: int): void
+ avisoMensajes(mensaje: string): void
+ formularioNuevoMensaje(red: RedProfesional&, idSesion: int, idContacto: int): void
+ conversacionMensajes(red: RedProfesional&, idSesion: int, idContacto: int): void
+ seccionMensajes(red: RedProfesional&, idSesion: int): void
+ detalleNotificacion(red: RedProfesional&, idSesion: int, idNotificacion: int): void
+ seccionNotificaciones(red: RedProfesional&, idSesion: int): void
+ dibujarEncabezadoIndividuo(red: RedProfesional&, idSesion: int): void
+ dibujarOpcionesIndividuo(): void
+ filaOpcionIndividuo(opcion: int): int
+ abrirSeccionIndividuo(red: RedProfesional&, idSesion: int, opcion: int): void
+ menuIndividuo(red: RedProfesional&, idSesion: int): void
+ dibujarEncabezadoEmpresa(red: RedProfesional&, idSesion: int): void
+ esperarEnter(): void
+ barraCompatibilidad(porcentaje: int): string
+ formularioVacante(red: RedProfesional&, idSesion: int): void
+ revisarPostulantes(red: RedProfesional&, idSesion: int, idVacante: int): void
+ detalleVacante(red: RedProfesional&, idSesion: int, idVacante: int): void
+ seccionMisVacantes(red: RedProfesional&, idSesion: int): void
+ seccionMisContrataciones(red: RedProfesional&, idSesion: int): void
+ mostrarResultadosBusqueda(red: RedProfesional&, resultados: Lista<Coincidencia>&, totalPalabras: int): void
+ seccionBuscarProfesionales(red: RedProfesional&): void
+ seccionMiEmpresa(red: RedProfesional&, idSesion: int): void
+ dibujarOpcionesEmpresa(): void
+ filaOpcionEmpresa(opcion: int): int
+ abrirSeccionEmpresa(red: RedProfesional&, idSesion: int, opcion: int): void
+ menuEmpresa(red: RedProfesional&, idSesion: int): void
+ mostrarMenu(red: RedProfesional&, idSesion: int, esEmpresa: bool): void
```

## 3. Relaciones

### Corregir

| Relación | En el UML | Debe ser | Por qué |
|---|---|---|---|
| Lista::Iterador → Nodo | Herencia (triángulo) | Asociación (flecha simple) | El iterador **tiene** `actual: Nodo<T>*`; no hereda de Nodo |
| Nodo → Nodo | Herencia | Auto-asociación 0..1 (`sgte`) | Es el puntero al siguiente nodo del mismo tipo |
| NodoDoble → NodoDoble | Herencia | Auto-asociación 0..1 (`ant`, `sgte`) | Igual que Nodo, en los dos sentidos |
| GestorArchivos → RedProfesional | Punteada con triángulo (realización) | Dependencia «friend» (punteada, flecha abierta) | GestorArchivos no implementa RedProfesional; la usa y es su `friend` |
| Línea de composición entre Usuario y Mensaje | Suelta, sin conectar | Borrarla o conectarla a SolicitudConexion | Usuario no contiene Mensajes (los mensajes están en RedProfesional) |

### Agregar

| Relación | Tipo | Multiplicidad |
|---|---|---|
| Usuario ◆— SolicitudConexion | Composición (`Cola<SolicitudConexion>`) | 1 — 0..* |
| Lista ⊕— Lista::Iterador y ListaDoble ⊕— ListaDoble::Iterador | Clase anidada (o asociación) | — |
| ListaDoble::Iterador → NodoDoble | Asociación (`actual`) | 0..1 |
| Habilidad → NivelHabilidad, Postulacion → EstadoPostulacion, SolicitudConexion → EstadoSolicitud, Notificacion → TipoNotificacion | Asociación con el enum | 1 |
| RedProfesional ⇢ Ordenamiento | Dependencia «use» | — |
| RedProfesional ⇢ Coincidencia, SugerenciaConexion | Dependencia (las devuelve) | — |
| Usuario — Usuario (contactos) | Auto-asociación por id (`Lista<int>`) | 0..* |
| Postulacion → Usuario, Postulacion → Vacante, Vacante → Empresa | Asociación por id | * — 1 |
| ExperienciaLaboral → Empresa | Asociación por id (0 si no está registrada) | * — 0..1 |
| GrupoProfesional — Usuario (miembros) | Asociación por id (`Lista<int>`) | * — * |
| Contenido → Usuario (autor), Mensaje/Recomendación → Usuario (receptor) | Asociación por id | * — 1 |

### Ya están bien

- Herencia de Publicacion, Comentario, Recomendacion y Mensaje desde Contenido.
- Composición de Usuario, Empresa, Vacante, Postulacion, GrupoProfesional, Publicacion, Comentario, Recomendacion y Mensaje en RedProfesional.
- Composición de Habilidad, Certificacion, ExperienciaLaboral, Accion y Notificacion en Usuario.
- Nodo en Lista, Pila y Cola; NodoDoble en ListaDoble; Lista::Iterador y ListaDoble::Iterador asociados a su lista.

Nota: varias líneas del diagrama tienen los extremos sueltos (no pegados a la caja). Se ven bien, pero si mueven una clase la línea no la sigue.
