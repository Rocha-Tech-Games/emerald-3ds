# Evaluación y base para cinco regiones

## Decisión

Compartir reglas, datos y guardado entre aplicaciones nativas de PC, Switch
homebrew (libnx) y New 3DS XL (libctru). Evitar concatenar ROMs: mapas, scripts,
combates, especies y progreso tienen formatos y motores diferentes entre generaciones.
La base actual está aislada en `multiplatform/`; no altera la jugabilidad, los
sprites, la doble pantalla o el guardado del port 3DS existente.

## Qué podemos reutilizar

| Componente existente | Uso y adaptación |
| --- | --- |
| `3ds_port/compat/port_platform.h` | Referencia para separar lógica y plataforma; conserva dependencias del port que deben retirarse gradualmente. |
| Lógica de combate y mapas de Esmeralda preparada por bootstrap | Base de Hoenn; no incluye otras regiones ni mecánicas completas de generaciones IV/V. |
| `3ds_port/src/3ds_pak.c` y pipeline builder | Índice y empaquetado de datos reutilizables; desacoplar acceso a archivos y comprobar ABI. No se han conectado a la nueva base. |
| Conversión Showdown y atlas HD | Reutilizar fuentes y temporización; el orden Morton/texturas Citro requiere backend diferente en PC/Switch. Catálogo actual: 386; objetivo para cinco generaciones: 649 especies, más formas. |
| Decodificadores de vídeo e input sin SDK y sus pruebas | Referencia comprobable para portabilidad. El renderizado completo utiliza Citro2D/3D. |
| Motor de audio m4a/MP2K | Reglas y mezcla aprovechables; salida NDSP necesita sustitución. |
| Flash de Esmeralda de 128 KiB | Preservación binaria mediante contenedor nuevo; el juego aún no lo consume. |
| UI inferior, controles, reloj, archivos, red local | Adaptadores específicos; PC/Switch deben presentar también la información inferior. |

El port mantiene un modelo de registros/VRAM GBA y llamadas de plataforma desde
el bucle principal. No basta recompilar: revisar punteros de 32 bits, manifiestos,
relocaciones y rutas de scripts al pasar a PC/Switch de 64 bits. Mantener idénticos
IDs y reglas antes de prometer compatibilidad de una partida multirregión.

Existe otro port con backend SDL2/OpenGL que merece una evaluación de compilación
para PC: https://github.com/gradenGnostic/pokeemerald-multiplatform . Ya figura
como procedencia del voxel en NOTICE del fork; no se ha incorporado código suyo
en esta base. Su existencia no garantiza compatibilidad con este fork.
Switch usa homebrew libnx: https://github.com/switchbrew/libnx . Evitar depender
de un backend Switch de SDK comercial. Las recetas de consola son diagnósticos,
no ports jugables terminados.

## Implementado ahora

Biblioteca C sin SDK: nombres/IDs de cinco regiones, contenedor v1 explícito en
little-endian con CRC de cabecera y contenido; herramienta PC de copia y exportación;
diagnósticos Switch y 3DS que compilan la biblioteca común. Contrato inicial
`platform.h` para input, dos superficies, audio, tiempo y almacenamiento, todavía
sin conexión al motor. No se han creado mapas, eventos o historias nuevos.

## Orden de integración

1. Ejecutar Hoenn en PC: separar plataforma del bucle, portar vídeo/input/audio,
   validar punteros de 64 bits y comparar juego y partidas con el port 3DS.
2. Conectar guardado común con migración explícita, reloj portátil, identidad del
   contenido y recuperación ante interrupciones; probar intercambio en hardware.
3. Repetir Hoenn en Switch y conservar el backend de doble pantalla en New 3DS XL.
4. Extender reglas y datos hasta generación V; equipo/cajas/especies y progreso
   necesitarán un esquema nuevo. No caben automáticamente en la flash de Esmeralda.
5. Integrar una zona de Kanto como corte vertical con viaje y retorno a Hoenn;
   después Johto, Sinnoh y Teselia, cada una con mapas, scripts y pruebas propios.

La 3DS fija el presupuesto: carga de mapas por zonas, texturas de combatientes
activos únicamente, atlas de resolución adaptada y animación por tiempo. La
configuración HD actual usa aproximadamente 2 MiB para cuatro atlas activos;
no cargar la Pokédex completa en RAM. Presupuestos finales requieren mediciones
sobre consola; no hay cifras de rendimiento del nuevo motor todavía.

## Resultado de pruebas de esta fase

Compilación nativa macOS ARM64 y pruebas con AddressSanitizer/UndefinedBehaviorSanitizer:
correctas. Formato fijo, preservación de bytes, cabecera corrupta, muestras de
corrupción de contenido, todas las longitudes truncadas, límites e IDs: correctos.
CLI: ida/vuelta idéntica, rechazo de sobrescritura y de entrada inválida: correcto.
Diagnósticos `.nro` (devkitA64/libnx local) y `.3dsx` (devkitARM/libctru en el entorno
Docker existente): compilados. No probados en consolas. No se recompiló el juego
HD completo porque esta fase no modifica sus fuentes ni assets.
