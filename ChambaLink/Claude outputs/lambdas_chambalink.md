# Lambdas de ChambaLink

Total: **113 lambdas** en 8 archivos (revisado el 3 oct 2026).

## Resumen por tipo

| Para qué sirve | Cantidad |
|---|---|
| Recorrido / acumulación | 65 |
| Condición de búsqueda | 32 |
| Criterio de ordenamiento | 10 |
| Hacer / deshacer (Accion en la Pila) | 4 |
| Cambio a aplicar (modificarSi) | 1 |
| Condición para contar (Pila) | 1 |

## Resumen por archivo

| Archivo | Cantidad |
|---|---|
| `RedProfesional.h` | 65 |
| `Usuario.h` | 11 |
| `GestorArchivos.h` | 18 |
| `FuncionesMenu.h` | 11 |
| `MenuPrincipal.h` | 3 |
| `GrupoProfesional.h` | 2 |
| `Publicacion.h` | 2 |
| `Ordenamiento.h` | 1 |

## RedProfesional.h (65)

| Línea | Método | Sección | Captura | Para qué |
|---|---|---|---|---|
| 219 | `contactosEnComun` | General | `[b, &total]` | Recorrido / acumulación |
| 233 | `explorarConexionesRec` | General | `[this, profundidad, &visitados, &descubiertos]` | Recorrido / acumulación |
| 234 | `explorarConexionesRec` | General | `[idContacto]` | Condición de búsqueda |
| 244 | `buscarEmpresa` | General | `[idEmpresa]` | Condición de búsqueda |
| 248 | `buscarVacante` | General | `[idVacante]` | Condición de búsqueda |
| 252 | `buscarPostulacion` | General | `[idPostulacion]` | Condición de búsqueda |
| 256 | `buscarGrupo` | General | `[idGrupo]` | Condición de búsqueda |
| 260 | `buscarPublicacion` | General | `[idPublicacion]` | Condición de búsqueda |
| 268 | `recorrerRespuestas` | General | `[this, idPublicacion, idPadre, nivel, &accion]` | Recorrido / acumulación |
| 277 | `buscarComentario` | General | `[idComentario]` | Condición de búsqueda |
| 298 | `correoEnUso` | General | `[&correo]` | Condición de búsqueda |
| 299 | `correoEnUso` | General | `[&correo]` | Condición de búsqueda |
| 389 | `iniciarSesionUsuario` | General | `[&correo]` | Condición de búsqueda |
| 398 | `buscarUsuario` | General | `[idUsuario]` | Condición de búsqueda |
| 402 | `buscarUsuario` | General | `[idUsuario]` | Condición de búsqueda |
| 409 | `obtenerEmpresa` | General | `[idEmpresa]` | Condición de búsqueda |
| 457 | `responderSiguienteSolicitud` | Joao | `[this, idReceptor, idEmisor]` | Hacer / deshacer (Accion en la Pila) |
| 458 | `responderSiguienteSolicitud` | Joao | `[this, idReceptor, idEmisor]` | Hacer / deshacer (Accion en la Pila) |
| 475 | `eliminarContacto` | Joao | `[this, idUsuario, idContacto]` | Hacer / deshacer (Accion en la Pila) |
| 476 | `eliminarContacto` | Joao | `[this, idUsuario, idContacto]` | Hacer / deshacer (Accion en la Pila) |
| 526 | `certificacionesOrdenadas` | Joao | `[&copia]` | Recorrido / acumulación |
| 527 | `certificacionesOrdenadas` | Joao | `[]` | Criterio de ordenamiento |
| 539 | `contactosOrdenadosPorNombre` | Joao | `[&copia]` | Recorrido / acumulación |
| 543 | `contactosOrdenadosPorNombre` | Joao | `[this]` | Criterio de ordenamiento |
| 558 | `buscarPersonas` | Joao | `[&]` | Recorrido / acumulación |
| 567 | `buscarPersonas` | Joao | `[this]` | Criterio de ordenamiento |
| 588 | `sugerenciasConexion` | Joao | `[&]` | Recorrido / acumulación |
| 602 | `sugerenciasConexion` | Joao | `[this]` | Criterio de ordenamiento |
| 682 | `obtenerPublicacion` | Aldo | `[idPublicacion]` | Condición de búsqueda |
| 688 | `contarComentarios` | Aldo | `[idPublicacion, &total]` | Recorrido / acumulación |
| 698 | `feedPorFecha` | Aldo | `[]` | Criterio de ordenamiento |
| 707 | `feedPorPopularidad` | Aldo | `[]` | Criterio de ordenamiento |
| 767 | `cantidadMensajesNoLeidos` | Aldo | `[idUsuario, usuario, &total]` | Recorrido / acumulación |
| 777 | `cantidadMensajesNoLeidosDe` | Aldo | `[idUsuario, idContacto, &total]` | Recorrido / acumulación |
| 785 | `cantidadMensajesEntre` | Aldo | `[idA, idB, &total]` | Recorrido / acumulación |
| 795 | `obtenerConversacion` | Aldo | `[idA, idB, &resultado]` | Recorrido / acumulación |
| 806 | `paraCadaComentarioDe` | Aldo | `[idPublicacion, &accion]` | Recorrido / acumulación |
| 812 | `paraCadaMensajeEntre` | Aldo | `[idA, idB, &accion]` | Recorrido / acumulación |
| 818 | `paraCadaRecomendacionPara` | Aldo | `[idReceptor, &accion]` | Recorrido / acumulación |
| 824 | `paraCadaRecomendacionDe` | Aldo | `[idEmisor, &accion]` | Recorrido / acumulación |
| 830 | `obtenerRecomendacion` | Aldo | `[idRecomendacion]` | Condición de búsqueda |
| 839 | `recomendacionesRecibidas` | Aldo | `[&resultado]` | Recorrido / acumulación |
| 842 | `recomendacionesRecibidas` | Aldo | `[]` | Criterio de ordenamiento |
| 850 | `recomendacionesEnviadas` | Aldo | `[&resultado]` | Recorrido / acumulación |
| 853 | `recomendacionesEnviadas` | Aldo | `[]` | Criterio de ordenamiento |
| 865 | `actividadDe` | Aldo | `[&actividad, idUsuario]` | Recorrido / acumulación |
| 868 | `actividadDe` | Aldo | `[&actividad, idUsuario]` | Recorrido / acumulación |
| 871 | `actividadDe` | Aldo | `[&actividad, idUsuario]` | Recorrido / acumulación |
| 886 | `registrarEmpresa` | Piero | `[nombre]` | Condición de búsqueda |
| 901 | `iniciarSesionEmpresa` | Piero | `[&correo]` | Condición de búsqueda |
| 924 | `postular` | Piero | `[idUsuario, idVacante]` | Condición de búsqueda |
| 961 | `verSiguientePostulacion` | Piero | `[idPostulacion]` | Condición de búsqueda |
| 986 | `cancelarVacante` | Piero | `[idVacante, &aceptados]` | Recorrido / acumulación |
| 990 | `cancelarVacante` | Piero | `[this, &puesto]` | Recorrido / acumulación |
| 1036 | `nombreEmpresa` | Piero | `[idEmpresa]` | Condición de búsqueda |
| 1048 | `paraCadaVacanteActiva` | Piero | `[&accion]` | Recorrido / acumulación |
| 1055 | `obtenerVacante` | Piero | `[idVacante]` | Condición de búsqueda |
| 1060 | `paraCadaVacanteDe` | Piero | `[idEmpresa, &accion]` | Recorrido / acumulación |
| 1067 | `paraCadaContratacion` | Piero | `[this, idEmpresa, &accion]` | Recorrido / acumulación |
| 1097 | `buscarProfesionales` | Piero | `[&palabras, &resultados]` | Recorrido / acumulación |
| 1099 | `buscarProfesionales` | Piero | `[&u, &cantidad]` | Recorrido / acumulación |
| 1101 | `buscarProfesionales` | Piero | `[&palabra, &encontrada]` | Recorrido / acumulación |
| 1115 | `buscarProfesionales` | Piero | `[]` | Criterio de ordenamiento |
| 1121 | `ordenarVacantesHeap` | Piero | `[]` | Criterio de ordenamiento |
| 1129 | `paraCadaPostulacionDe` | Piero | `[idUsuario, &accion]` | Recorrido / acumulación |

## Usuario.h (11)

| Línea | Método | Captura | Para qué |
|---|---|---|---|
| 93 | `tieneHabilidad` | `[nombreHabilidad]` | Condición de búsqueda |
| 101 | `agregarHabilidad` | `[&h]` | Condición de búsqueda |
| 102 | `agregarHabilidad` | `[&h]` | Cambio a aplicar (modificarSi) |
| 109 | `eliminarHabilidad` | `[nombreHabilidad]` | Condición de búsqueda |
| 124 | `eliminarCertificacion` | `[idCertificacion]` | Condición de búsqueda |
| 144 | `antiguedadTotalEnAnios` | `[&total, anioActual]` | Recorrido / acumulación |
| 172 | `tieneSolicitudDe` | `[idEmisor]` | Condición de búsqueda |
| 208 | `contarAccionesEjecutadas` | `[]` | Condición para contar (Pila) |
| 219 | `cantidadNotificacionesNoLeidas` | `[&total]` | Recorrido / acumulación |
| 263 | `esContactoDirecto` | `[idUsuario]` | Condición de búsqueda |
| 274 | `eliminarContacto` | `[idUsuario]` | Condición de búsqueda |

## GestorArchivos.h (18)

| Línea | Método | Captura | Para qué |
|---|---|---|---|
| 122 | `escribirUsuario` | `[&]` | Recorrido / acumulación |
| 127 | `escribirUsuario` | `[&]` | Recorrido / acumulación |
| 133 | `escribirUsuario` | `[&]` | Recorrido / acumulación |
| 143 | `escribirUsuario` | `[&]` | Recorrido / acumulación |
| 152 | `escribirUsuario` | `[&]` | Recorrido / acumulación |
| 161 | `escribirUsuario` | `[&]` | Recorrido / acumulación |
| 702 | `guardarTodo` | `[&]` | Recorrido / acumulación |
| 705 | `guardarTodo` | `[&]` | Recorrido / acumulación |
| 715 | `guardarTodo` | `[&]` | Recorrido / acumulación |
| 724 | `guardarTodo` | `[&]` | Recorrido / acumulación |
| 730 | `guardarTodo` | `[&]` | Recorrido / acumulación |
| 739 | `guardarTodo` | `[&]` | Recorrido / acumulación |
| 745 | `guardarTodo` | `[&]` | Recorrido / acumulación |
| 749 | `guardarTodo` | `[&]` | Recorrido / acumulación |
| 755 | `guardarTodo` | `[&]` | Recorrido / acumulación |
| 759 | `guardarTodo` | `[&]` | Recorrido / acumulación |
| 769 | `guardarTodo` | `[&]` | Recorrido / acumulación |
| 778 | `guardarTodo` | `[&]` | Recorrido / acumulación |

## FuncionesMenu.h (11)

| Línea | Método | Captura | Para qué |
|---|---|---|---|
| 87 | `textoHabilidades` | `[&texto]` | Recorrido / acumulación |
| 264 | `verPublicacion` | `[&]` | Recorrido / acumulación |
| 408 | `obtenerPostulacionUsuarioVacante` | `[&encontrada, idVacante]` | Recorrido / acumulación |
| 490 | `seccionEmpleos` | `[&disponibles]` | Recorrido / acumulación |
| 556 | `seccionMisPostulaciones` | `[&mias]` | Recorrido / acumulación |
| 1393 | `detalleNotificacion` | `[&]` | Recorrido / acumulación |
| 1434 | `seccionNotificaciones` | `[&]` | Recorrido / acumulación |
| 1744 | `seccionMisVacantes` | `[&mias]` | Recorrido / acumulación |
| 1798 | `seccionMisContrataciones` | `[&contratadas]` | Recorrido / acumulación |
| 1907 | `seccionMiEmpresa` | `[&activas, &canceladas]` | Recorrido / acumulación |
| 1911 | `seccionMiEmpresa` | `[&contrataciones]` | Recorrido / acumulación |

## MenuPrincipal.h (3)

| Línea | Método | Captura | Para qué |
|---|---|---|---|
| 63 | `menuEmpresa` | `[]` | Recorrido / acumulación |
| 86 | `menuUsuario` | `[&hay]` | Recorrido / acumulación |
| 112 | `menuUsuario` | `[]` | Recorrido / acumulación |

## GrupoProfesional.h (2)

| Línea | Método | Captura | Para qué |
|---|---|---|---|
| 40 | `esMiembro` | `[idUsuario]` | Condición de búsqueda |
| 51 | `eliminarMiembro` | `[idUsuario]` | Condición de búsqueda |

## Publicacion.h (2)

| Línea | Método | Captura | Para qué |
|---|---|---|---|
| 24 | `leGustaA` | `[idUsuario]` | Condición de búsqueda |
| 40 | `quitarMeGusta` | `[idUsuario]` | Condición de búsqueda |

## Ordenamiento.h (1)

| Línea | Método | Captura | Para qué |
|---|---|---|---|
| 39 | `listaAArreglo` | `[&arreglo, &i]` | Recorrido / acumulación |
