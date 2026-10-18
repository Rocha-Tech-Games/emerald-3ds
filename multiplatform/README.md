# Base multiplataforma — fase inicial

Aplicación de diagnóstico y biblioteca C compartida. **Todavía no ejecuta
Pokémon ni contiene cinco regiones jugables.** El port existente sigue separado.

- PC: `make -C multiplatform verify`; ejecutar `multiplatform/build/world-tool`.
  Probado en macOS ARM64; Windows/Linux pendientes, no hay interfaz gráfica aún.
- Switch homebrew: `DEVKITPRO=/opt/devkitpro DEVKITA64=/opt/devkitpro/devkitA64 make -C multiplatform/switch`.
  Requiere devkitA64, libnx y herramientas Switch. Genera `emerald-world-base.nro`.
- New 3DS XL: `make -C multiplatform/3ds` con devkitARM, libctru y 3dsxtool.
  Genera `3ds/build/emerald-world-base.3dsx`. Muestra ambas pantallas.

Los diagnósticos de consola no leen ni escriben partidas. No sustituyen Emerald3DS-HD.
El Makefile de Switch parte de la plantilla application de devkitPro instalada.

## Partidas: contrato experimental v1

`world-tool wrap origen.sav copia.emworld` conserva exactamente 131072 bytes.
`world-tool export copia.emworld copia.sav` los recupera sin transformación.
Rechaza salidas existentes; no modifica el origen. Solo usar copias para pruebas.
No convierte equipo, misiones ni progreso a un juego multirregión. No sincroniza
consolas, ni importa reloj `.clock`, copias `.bak` o trailers de emuladores;
rechaza archivos que no midan exactamente 128 KiB para evitar pérdida silenciosa.
No valida las sumas internas de Esmeralda ni identifica idioma/ROM de origen.

Cabecera de 32 bytes seguida de flash opaca de 131072 bytes. Enteros uint32
little-endian; CRC32 IEEE, polinomio reflejado 0xedb88320, inicio/final XOR ffffffff.

| Offset | Campo |
| --- | --- |
| 0 | 8 bytes `EMWORLD\0` |
| 8 | versión 1 |
| 12 | longitud flash 131072 |
| 16 | región legado 3 (Hoenn) |
| 20 | CRC de la flash |
| 24 | reservado, cero |
| 28 | CRC de bytes 0–27 |

Longitud, versión, reservado y CRC deben coincidir. Sin serialización de structs
ni punteros: mismo formato en ARM32/ARM64/PC. IDs estables: Kanto 1, Johto 2,
Hoenn 3, Sinnoh 4, Teselia 5. Registrar regiones no añade su contenido.
Las extensiones de progreso regional requerirán nueva versión y migración explícita.
El almacenamiento de producción aún necesita generaciones de respaldo, escritura
atómica por plataforma y recuperación tras interrupción; `commit_save` solo define
el contrato futuro. El CLI escribe una copia nueva, no es el guardador del juego.

## Verificación

`make -C multiplatform verify` ejecuta pruebas de formato, corrupción, truncado,
límites y copia/exportación. Se puede añadir
`CFLAGS='-O1 -g -std=c11 -Wall -Wextra -Werror -fsanitize=address,undefined'`.
Las consolas compilan el mismo `src/world.c`; prueba real de intercambio entre
hardware pendiente. No se ha comprobado todavía el juego completo en PC/Switch.

Ver `../docs/MULTIPLATFORM.md` para reutilización y pasos pendientes.
