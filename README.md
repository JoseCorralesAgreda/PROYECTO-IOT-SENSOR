# Indicador de distancia con ESP32

Firmware Arduino en C++17: rojo entre 2 y menos de 10 cm, amarillo entre 10 y menos de 30 cm y verde entre 30 y 400 cm inclusive. Una lectura ausente, no finita o fuera del intervalo hace parpadear los tres LEDs simultáneamente: 250 ms encendidos y 250 ms apagados. Arranca encendido en condición inválida, conserva la fase entre errores consecutivos y recupera el color al consumir una lectura válida.

## Organización y ejecución

`DistanceIndicator` decide la salida con tiempo explícito. `EchoCapture` comprueba los flancos y publica un único resultado. `UltrasonicSensor` captura Echo mediante interrupción CHANGE y protege el estado con el mismo bloqueo en ISR y tarea. `LedDriver` escribe los tres GPIO en cada iteración. No se usa conectividad, filtrado ni histéresis.

Las adquisiciones se separan al menos 100 ms entre inicios reales; no se recuperan ciclos atrasados mediante ráfagas. El único retardo es el pulso Trigger de 10 µs. Una pareja de flancos debe completarse antes de 30 000 µs desde Trigger; a igualdad vence el plazo. La conversión es duración en microsegundos / 58, sin redondeo previo. Los relojes deben atenderse antes de completar una vuelta de sus contadores de 32 bits.

Desde la raíz, en PowerShell con PlatformIO disponible:

En Windows se utiliza Visual Studio 2022 con herramientas C++ x64, toolset `14.44.35207` y Windows SDK. Preparar una vez los módulos MSVC que la distribución reducida de SCons de PlatformIO omite:

```powershell
& 'C:/Users/JOSEFRANZ/.platformio/penv/Scripts/python.exe' -m pip install --target .pio/tools/scons scons==4.11.1
```

En Linux/macOS se requiere GCC o Clang con C++17 en PATH. Estas plataformas no se ejecutaron en esta entrega. Con PlatformIO disponible en `PATH`:

```powershell
pio test -e native
pio run -e esp32doit-devkit-v1
git diff --check
```

También puede usarse `platformio` en lugar de `pio`. La evidencia local se generó con `C:/Users/JOSEFRANZ/.platformio/penv/Scripts/platformio.exe` como alternativa porque el comando no estaba en `PATH`.

El entorno nativo compila las clases reales y los adaptadores con dobles de GPIO, interrupciones y reloj; no utiliza esperas reales. Incluye límites, exclusión, recuperación, fase, desbordamientos, captura ordenada, vencimiento, resultados pendientes, conversión, configuración GPIO/CHANGE, mutex compartido y separación de disparos. El inventario de los 15 casos, la trazabilidad y los procedimientos físicos reproducibles están en el [plan integral de pruebas](PLAN-DE-PRUEBAS.md).

## Versiones y evidencia

| Herramienta | Versión |
|---|---|
| PlatformIO Core observado | 6.2.0 |
| Espressif32 fijada | 7.1.1 |
| Arduino ESP32 fijado | 3.20017.241212+sha.dcc1105b |
| Compilador ESP32 observado | Xtensa GCC 8.4.0+2021r2-patch5 |
| Plataforma nativa fijada | 1.2.1 |
| Unity fijada | 2.6.1 |
| Toolset MSVC nativo fijado | 14.44.35207 (Visual Studio 2022) |
| SCons completo para MSVC fijado | 4.11.1 |

Verificación del 2026-09-14: **15 pruebas nativas aprobadas** en cuatro suites, incluyendo `setup()` y `loop()` reales con hardware simulado. Compilación ESP32: `SUCCESS`. Evidencia: [pruebas nativas](_bmad-output/implementation-artifacts/verification-native.txt) y [compilación ESP32](_bmad-output/implementation-artifacts/verification-esp32.txt). No se cargó firmware ni se ejecutaron ensayos físicos.

## Pines implementados y pendientes físicos

| Función | GPIO |
|---|---:|
| Trigger | 18 |
| Echo, conexión pendiente | 19 |
| LED rojo | 25 |
| LED amarillo | 26 |
| LED verde | 27 |

Las salidas LED son activas altas, con una resistencia de 220 Ω por LED y cátodo a masa. El alcance permanece limitado a ESP32, sensor de 5 V, tres LEDs y esas tres resistencias.

P-01/P-02 siguen pendientes: confirmar referencia del sensor, placa, polaridad de LEDs y nivel de Echo. Mantener Echo sin conexión al GPIO hasta demostrar compatibilidad con el límite de 3,6 V; no conectar directamente una salida de 5 V ni añadir adaptación sin resolver expresamente el alcance. No cargar ni validar el montaje mientras estos pendientes bloqueen la integración.

Los supuestos S-01 a S-04 definen el comportamiento implementado del software. P-03 limita el error a lecturas inválidas reconocidas: un eco espurio puede parecer válido aunque el objeto esté fuera de rango. Quedan por medir precisión, latencia de ISR, separación física de Trigger, parpadeo y retraso máximo de actualización de 10 ms. Las pruebas simuladas y la compilación no acreditan estos resultados.

El [PRD](_bmad-output/planning-artifacts/prds/prd-indicador-distancia-2026-09-07/prd.md), el [informe técnico](_bmad-output/planning-artifacts/prds/prd-indicador-distancia-2026-09-07/informe-tecnico.md) y la [arquitectura](_bmad-output/planning-artifacts/architecture/architecture-indicador-distancia-2026-09-07/ARCHITECTURE-SPINE.md) describen el firmware implementado y distinguen la evidencia automatizada de las validaciones físicas pendientes.
