---
title: 'Implementar indicador de distancia con ESP32'
type: 'feature'
created: '2026-09-08'
status: 'in-review'
baseline_commit: '4dea33c7bc85795f851b1bdb24c08ef8e3a4295d'
route: 'dispatch'
review_loop_iteration: 0
context:
  - '{project-root}/_bmad-output/planning-artifacts/architecture/architecture-indicador-distancia-2026-09-07/ARCHITECTURE-SPINE.md'
---

<frozen-after-approval reason="Intención del usuario; modificar solo por decisión humana">

## Intent

**Problema:** El proyecto contiene únicamente la plantilla Arduino; todavía no mide distancia ni controla los LEDs.

**Enfoque:** Implementar el firmware completo del PRD y su arquitectura, con adquisición no bloqueante, lógica verificable sin hardware y documentación de ejecución. Adoptar para el software los supuestos S-01 a S-04 documentados; su validación física permanece pendiente.

## Boundaries & Constraints

**Siempre:** C++17 orientado a objetos; código y comentarios en inglés, documentación en español. Mantener placa `esp32doit-devkit-v1`, Arduino y plataforma `espressif32@7.1.1`. Capturar Echo mediante interrupciones y proteger el estado compartido. Actualizar LEDs en cada iteración. Conservar el alcance eléctrico del PRD.

**Nunca:** Wi-Fi, Bluetooth, filtros, histéresis, componentes nuevos, `pulseIn` o esperas de Echo. No cargar firmware ni declarar validación física con P-01/P-02 pendientes. No conectar Echo incompatible. P-03 limita la detección a lecturas inválidas reconocidas.

## I/O & Edge-Case Matrix

| Escenario | Entrada | Resultado |
|---|---|---|
| Rojo | 2 y 9,99 cm | Solo rojo |
| Amarillo | 10 y 29,99 cm | Solo amarillo |
| Verde | 30 y 400 cm | Solo verde |
| Inválida | Ausencia, negativo, NaN, infinito, 1,99 y 400,01 cm | Tres LEDs alternan cada 250 ms |
| Arranque/error nuevo | Inicialización o válida → inválida | Fase encendida desde la transición |
| Error repetido | Inválidas sucesivas | Mantener fase |
| Recuperación | Inválida → 20 cm | Amarillo en esa iteración |
| Captura | Pulso ordenado de 580 µs | 10 cm |
| Timeout | Sin pareja completa antes de 30 000 µs | Una lectura inválida |
| Consumo | Ningún resultado nuevo | Devolver false sin modificar destino |

</frozen-after-approval>

## Code Map

- `src/main.cpp`: plantilla con `myFunction`; sustituir por composición y bucle.
- `platformio.ini`: único entorno ESP32 sin versiones fijadas; conservarlo y añadir entorno nativo.
- `include/`, `lib/`, `test/`: solo archivos README de plantilla; no hay lógica reutilizable.
- PRD e informe en `_bmad-output/planning-artifacts/prds/prd-indicador-distancia-2026-09-07/`: requisitos y casos PU-01 a PU-09.
- Arquitectura indicada en `context`: contratos AD-1 a AD-6; prevalece sobre ejemplos preliminares del informe.
- Árbol limpio al investigar, rama `main`, base `4dea33c7bc85795f851b1bdb24c08ef8e3a4295d`.
- PlatformIO disponible en `C:/Users/JOSEFRANZ/.platformio/penv/Scripts/platformio.exe`; compilador anfitrión no localizado en PATH. Preparar y registrar herramienta nativa antes de las pruebas.

## Tasks & Acceptance

**Ejecución:**
- [x] `platformio.ini`: fijar plataforma, resolución Arduino y C++17; configurar pruebas nativas Unity con versiones registradas.
- [x] `include/Reading.h`, `include/LedOutput.h`: definir contratos compartidos sin Arduino.
- [x] `include/DistanceIndicator.h`: implementar clasificación y fase con reloj explícito.
- [x] `include/EchoCapture.h`: implementar secuencia, vencimiento y consumo único de pulsos.
- [x] `include/UltrasonicSensor.h`, `src/UltrasonicSensor.cpp`: integrar captura CHANGE, sección crítica, conversión y disparos separados al menos 100 ms.
- [x] `include/LedDriver.h`, `src/LedDriver.cpp`, `src/main.cpp`: inicializar GPIO 18/19/25/26/27 y coordinar adquisición, consumo y salida activa alta.
- [x] `test/test_indicator/`, `test/test_echo_capture/`, `test/test_sensor/`: comprobar matriz, límites temporales y contrato del adaptador mediante dobles de hardware cuando corresponda.
- [x] `README.md`: documentar comandos, pines propuestos, versiones, evidencia y pendientes físicos.

**Criterios de aceptación:**
- Dado el entorno nativo, cuando se ejecutan las pruebas sobre las clases del firmware, entonces toda la matriz pasa sin esperas reales.
- Dado el reloj controlado, cuando cruza 249/250/499/500 ms, salta varias medias fases o desborda uint32_t, entonces conserva la fase correcta.
- Dada una captura, cuando llegan flancos aislados, anteriores, tardíos o justo al límite, entonces no se publica un pulso válido indebido; un pulso completo en plazo sobrevive a observación tardía.
- Dado Echo alto al inicio o un resultado pendiente, cuando corresponde disparar, entonces se publica inválida sin Trigger en el primer caso y se difiere adquisición en el segundo.
- Dado un ciclo atrasado, cuando vuelve el bucle, entonces no hay ráfaga de disparos ni sobrescritura de resultados.
- Dado el entorno ESP32, cuando se compila el firmware completo, entonces finaliza correctamente y se registra la salida; la tolerancia física de 10 ms permanece por medir.

## Implementation Notes

- 2026-09-08: implementación completada; 15 pruebas Unity aprobadas y firmware ESP32 compilado. MSVC 19.44.35217, toolset 14.44.35207; native 1.2.1, Unity 2.6.1 y SCons 4.11.1. Scripts Windows en scripts/native_toolchain.py y scripts/native_msvc.py; GCC/Clang en otras plataformas, aún sin ejecutar allí.
- Matriz auditada: límites y exclusión en test_boundaries_and_exclusion; inválidas en test_invalid_values; arranque y fase en test_phase_and_repeated_invalid; recuperación y nueva fase en test_recovery_and_new_error; conversión y consumo en test_conversion_and_no_overwrite; timeout en test_timeout_and_wrap. Todos ejecutados y aprobados. test_firmware_start_timeout_blink_and_recovery comprueba además la composición real.
- Evidencia: [pruebas nativas](verification-native.txt) y [compilación ESP32](verification-esp32.txt). Ensayos físicos pendientes conforme al alcance aprobado.

## Spec Change Log

## Review Triage Log

## Design Notes

ISR y bucle comparten captura bajo el mismo bloqueo; convertir fuera de ISR. El plazo se evalúa por marcas desde Trigger, no por orden de observación. Usar resta sin signo para relojes. Consumir antes de iniciar otro ciclo; no confundir ausencia de novedades con lectura inválida. El único retardo admitido es el pulso Trigger mínimo de 10 µs.

## Verification

- `C:/Users/JOSEFRANZ/.platformio/penv/Scripts/platformio.exe test -e native`: todas las pruebas aprobadas.
- `C:/Users/JOSEFRANZ/.platformio/penv/Scripts/platformio.exe run -e esp32doit-devkit-v1`: compilación correcta.
- `git diff --check`: sin errores de espacios.
- Registrar versiones y resultados efectivos; no sustituir pruebas físicas por simulaciones.
