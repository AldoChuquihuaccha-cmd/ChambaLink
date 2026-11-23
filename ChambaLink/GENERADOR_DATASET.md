# Generador de dataset de ChambaLink (RF18)

El proyecto incorpora `GeneradorDataset.h`, que crea usuarios, empresas y vacantes a partir de listas base y una semilla aleatoria.

## Ejecucion desde Visual Studio

1. Abrir `ChambaLink.sln` y compilar el proyecto.
2. Entrar a **Proyecto > Propiedades > Depuracion**.
3. En **Directorio de trabajo**, colocar `$(ProjectDir)` para que los TXT se guarden junto a los archivos actuales del proyecto.
4. En **Argumentos de comando**, colocar, por ejemplo:

   ```text
   --generar-dataset 1000 50 300 20261122
   ```

5. Ejecutar una vez. El programa generara 1000 usuarios, 50 empresas y 300 vacantes.
6. Borrar los argumentos de comando y volver a ejecutar para ingresar a la aplicacion con el dataset generado.

Tambien se puede ejecutar desde una terminal, situandose primero en la carpeta `ChambaLink-master/ChambaLink`, donde se encuentran los archivos TXT actuales:

```text
ChambaLink.exe --generar-dataset 1000 50 300 20261122
```

## Parametros

```text
--generar-dataset <usuarios> <empresas> <vacantes> <semilla>
```

- `usuarios`: cantidad de perfiles personales (minimo 1).
- `empresas`: cantidad de cuentas de empresa (minimo 1).
- `vacantes`: cantidad total de ofertas; se distribuyen aleatoriamente entre las empresas.
- `semilla`: numero entero sin signo que controla la secuencia pseudoaleatoria.

Se admiten hasta 10000 registros por tipo. Las cuentas generadas usan la contrasena `Demo1234`. Los correos tienen el formato `persona000001@dataset.pe` o `empresa000001@dataset.pe`.

## Reproducibilidad y persistencia

La misma semilla y las mismas cantidades producen los mismos datos en el mismo entorno de compilacion. El generador crea una instancia nueva de `RedProfesional`; por ello, al guardar, **reemplaza** los archivos de datos existentes (`usuarios.txt`, `empresas.txt`, `vacantes.txt`, los archivos relacionados y `chambalink.bin`). Se recomienda hacer una copia de seguridad antes de ejecutarlo si se desea conservar datos previos.

Al terminar, se reconstruyen los indices de la tabla hash y de los arboles AVL/ABB. Luego `GestorArchivos::guardarTodo()` persiste la informacion en los archivos TXT y binario.
