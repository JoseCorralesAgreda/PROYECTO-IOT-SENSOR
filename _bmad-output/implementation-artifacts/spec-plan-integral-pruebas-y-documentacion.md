---
title: 'Plan integral de pruebas y actualización documental'
type: 'chore'
created: '2026-09-14'
status: 'done'
route: 'dispatch'
review_loop_iteration: 0
baseline_commit: 'ab5ea5c86b0ffa5e9027e2ae70e61e28a0029a38'
context:
  - '{project-root}/_bmad-output/implementation-artifacts/spec-implementar-indicador-distancia.md'
---

<frozen-after-approval reason="human-owned intent — do not modify unless human renegotiates">

## Intent

**Problem:** El firmware y sus 15 pruebas unitarias ya están implementados y cuentan con evidencia de ejecución, pero el informe técnico, el PRD, la arquitectura y la guía de pruebas aún contienen afirmaciones históricas que los presentan como propuestas o trabajo no ejecutado. Tampoco existe un procedimiento manual completo y trazable para comprobar el montaje, los estados LED, la adquisición y los tiempos.

**Approach:** Crear un plan integral que inventaríe y trace las pruebas unitarias reales, defina validaciones manuales reproducibles con precondiciones, pasos, resultados esperados y registro de evidencia, y actualizar toda la documentación para que describa fielmente el código y distinga la verificación automatizada aprobada de los ensayos físicos pendientes.

## Boundaries & Constraints

**Always:** Documentación en español y código/pruebas en inglés; mantener los requisitos RF-01 a RF-07, RNF-01 a RNF-06, los contratos AD-1 a AD-6 y los GPIO 18/19/25/26/27. Cada prueba manual debe indicar seguridad, preparación, pasos, resultado esperado, criterio de aprobación y evidencia. La trazabilidad debe referirse a los 15 casos Unity existentes y a cualquier caso añadido durante la auditoría. Solo registrar como ejecutada la evidencia obtenida realmente.

**Never:** Declarar validación física sin realizarla; conectar Echo al GPIO 19 sin verificar que no exceda 3,6 V; confundir compilación o simulación con precisión acústica, compatibilidad eléctrica o tiempos medidos; añadir conectividad, filtrado, histéresis o hardware al alcance del firmware.

## I/O & Edge-Case Matrix

| Scenario | Input / State | Expected Output / Behavior | Error Handling |
|---|---|---|---|
| Bandas válidas | 2–<10, 10–<30 y 30–400 cm | Solo rojo, amarillo o verde, respectivamente | Repetir y registrar desviaciones |
| Lectura inválida | Ausencia, timeout, <2 cm, >400 cm o Echo alto inicial | Tres LEDs parpadean juntos: 250 ms encendidos y 250 ms apagados | Registrar ecos espurios como límite P-03 |
| Recuperación | Inválida seguida de lectura válida | Color correcto en la siguiente actualización | Fallo si conserva el error o un color anterior |
| Límites temporales | Trigger, separación de ciclos, timeout y parpadeo | 10 µs mínimo, 100 ms mínimo, 30 ms y período 500 ms | Medir con instrumento y conservar capturas |
| Seguridad eléctrica | Alimentación, masas, LEDs y Echo | Polaridad correcta; Echo ≤3,6 V antes de GPIO 19 | Suspender montaje si no se demuestra compatibilidad |

</frozen-after-approval>

## Code Map

- `test/test_indicator/test_main.cpp` — cinco casos reales para umbrales, valores inválidos, fase, recuperación y desbordamiento; conservar reloj simulado.
- `test/test_echo_capture/test_main.cpp` — cinco casos reales para orden de flancos, timeout, consumo único, pendientes y desbordamiento.
- `test/test_sensor/test_main.cpp` — cuatro casos reales para conversión, contrato de consumo, Echo alto, período, timeout y GPIO; completar solo contratos observables que la auditoría encuentre sin aserción.
- `test/test_application/test_main.cpp` — caso integral de `setup()`/`loop()` con dobles de Arduino; conservarlo como prueba de integración nativa.
- `test/support/Arduino.h` — doble de GPIO, reloj, interrupción y secciones críticas reutilizable por las pruebas.
- `src/` e `include/` — fuente de verdad del comportamiento; no cambiar contratos salvo que una prueba revele un defecto real.
- `PLAN-DE-PRUEBAS.md` — nuevo documento maestro con alcance, matriz unitaria, procedimientos manuales, trazabilidad y formatos de evidencia.
- `README.md` y `test/README` — puntos de entrada para ejecutar y entender las pruebas.
- `_bmad-output/planning-artifacts/prds/prd-indicador-distancia-2026-09-07/{prd.md,informe-tecnico.md}` — actualizar estado, implementación, pruebas, resultados y referencia al plan; el informe es el documento principal.
- `_bmad-output/planning-artifacts/architecture/architecture-indicador-distancia-2026-09-07/ARCHITECTURE-SPINE.md` — reemplazar lenguaje de propuesta por arquitectura implementada y firmas reales, manteniendo pendientes físicos.
- `_bmad-output/implementation-artifacts/verification-{native,esp32}.txt` — refrescar evidencia únicamente con salidas ejecutadas en esta entrega.

## Tasks & Acceptance

**Execution:**
- [x] `PLAN-DE-PRUEBAS.md` — documentar estrategia, entorno, inventario de pruebas unitarias, matriz requisito-prueba, casos manuales completos, orden seguro de ejecución y plantilla de registro.
- [x] `test/test_sensor/test_main.cpp` y pruebas relacionadas — auditar cobertura contra los contratos implementados y añadir aserciones útiles solo donde exista una brecha verificable.
- [x] `test/README` y `README.md` — explicar suites, comandos, evidencia, plan manual y limitaciones físicas actuales.
- [x] `prd.md`, `informe-tecnico.md` y `ARCHITECTURE-SPINE.md` — sincronizar estados, diagramas, firmas, resultados y trazabilidad con el código real; ampliar especialmente pruebas y validación del informe.
- [x] `verification-native.txt` y `verification-esp32.txt` — ejecutar pruebas y compilación actuales, guardar resultados verificables y revisar formato Markdown con `git diff --check`.

**Acceptance Criteria:**
- Dado el repositorio actual, cuando se consulta el plan, entonces cada caso Unity existente aparece identificado por suite, propósito, requisito y estado de ejecución.
- Dado un técnico con el montaje autorizado e instrumentos indicados, cuando sigue cada prueba manual, entonces puede decidir aprobado/no aprobado sin inferir pasos ni tolerancias.
- Dado que Echo no ha sido validado eléctricamente, cuando se sigue el orden de pruebas, entonces el procedimiento bloquea su conexión al GPIO 19 hasta comprobar ≤3,6 V.
- Dado cualquier documento del proyecto, cuando describe implementación o pruebas, entonces coincide con las clases, firmas, GPIO, versiones y evidencia actuales.
- Dado el entorno nativo y ESP32, cuando se ejecutan los comandos documentados, entonces las pruebas terminan aprobadas, el firmware compila y la evidencia registra fecha, comando y resultado reales.

## Implementation Notes

- 2026-09-14: creado `PLAN-DE-PRUEBAS.md` con cuatro niveles de evidencia, inventario UT-01 a UT-15, procedimientos M-01 a M-08, puerta de seguridad de 3,6 V, trazabilidad y plantilla de registro.
- La auditoría conservó los 15 casos y amplió `test_conversion_and_no_overwrite` para comprobar GPIO 18/19, interrupción `CHANGE` y mutex compartido entre tarea e ISR.
- README, PRD, informe y arquitectura sincronizados con clases, firmas, versiones, GPIO y evidencia actuales. Los ensayos físicos permanecen explícitamente pendientes.
- Verificación real del 2026-09-14: 15/15 casos nativos aprobados; compilación ESP32 aprobada; `git diff --check` sin errores.
- Revisión independiente: se endurecieron aserciones de inicialización/mutex, tolerancias e incertidumbre manual, resultados inconclusos, skew de GPIO, portabilidad de comandos y vinculación de evidencia mediante SHA-256. Verificación final principal: 15/15 casos aprobados, compilación ESP32 aprobada, huellas reproducidas y enlaces locales válidos.

## Spec Change Log

## Review Triage Log

| ID | Veredicto y evidencia | Ruta |
|---|---|---|
| BH-01 | `medium`: M-02 acepta 3,6 V medidos sin considerar incertidumbre u overshoot; puede aprobar una señal real superior al límite. | `patch` |
| BH-02 | `medium`: M-01 usa continuidad para una resistencia nominal; esa función no demuestra 220 Ω. | `patch` |
| BH-03 | `medium`: M-06 puede aprobar de forma vacía si todos los intentos fuera de rango parecen válidos. | `patch` |
| BH-04 | `medium`: M-06 no fija posiciones, permanencia ni repeticiones y dos técnicos podrían ejecutar ensayos distintos. | `patch` |
| BH-05 | `false`: AD-3 exige un pulso Trigger de al menos 10 µs; no existe un límite superior aprobado que el plan deba verificar. | Rechazado |
| BH-06 | `medium`: “cambian juntos” carece de tolerancia de skew aunque `LedDriver` escribe los GPIO secuencialmente. | `patch` |
| BH-07 | `medium`: M-08 pide cuatro señales simultáneas sin exigir instrumento de cuatro canales ni ofrecer alternativa. | `patch` |
| BH-08 | `medium`: las evidencias fechadas no identifican de forma estable las fuentes y el binario probados. | `patch` |
| BH-09 | `low`: la spec afirma `git diff --check` aprobado, pero ningún artefacto conserva esa salida. | `patch` |
| BH-10 | `medium`: el PRD atribuye al reloj simulado la comprobación del objetivo de actualización de 10 ms que sigue pendiente de hardware. | `patch` |
| BH-11 | `low`: el diagrama de dependencias de arquitectura omite las pruebas directas de `UltrasonicSensor`, `LedDriver` y `setup()`/`loop()`. | `patch` |
| BH-12 | `medium`: comparar solo el primer mutex de tarea no demuestra que las operaciones posteriores compartan el mutex de ISR. | `patch` |
| BH-13 | `medium`: los comandos absolutos de este equipo se presentan como invocación normal y reducen la reproducibilidad en otro puesto. | `patch` |
| BH-14 | `medium`: M-03 no exige relacionar el firmware cargado con el binario verificado. | `patch` |
| VG-01 | `medium`: `INPUT` y `LOW` valen cero igual que el estado inicial del doble; las nuevas aserciones sobreviven si se eliminan ambas inicializaciones. | `patch` |
| VG-02 | `medium`: la aserción del mutex solo cubre la primera entrada de tarea y puede omitir un bloqueo diferente en `update()`. | `patch` |
| EC-01 | `medium`: el modo Echo parte como `INPUT` en el doble, duplicado verificado de VG-01. | `patch` |
| EC-02 | `medium`: Trigger parte en `LOW` en el doble, duplicado verificado de VG-01. | `patch` |
| EC-03 | `medium`: las secciones críticas posteriores no se comparan, duplicado verificado de VG-02. | `patch` |
| EC-04 | `low`: M-05 no asigna veredicto explícito si al retirar el blanco nunca aparece parpadeo. | `patch` |
| EC-05 | `medium`: M-06 no llega a un veredicto reproducible cuando todos los intentos producen ecos aparentemente válidos, duplicado de BH-03/BH-04. | `patch` |
| EC-06 | `low`: el origen de captura se toma antes de elevar Trigger, por lo que el plazo físico es unos microsegundos conservador; es preexistente, evita perder un flanco y no afecta las ventanas de aceptación documentadas. | Rechazado |

## Design Notes

El plan separará cuatro niveles de evidencia: análisis estático documental, pruebas unitarias del núcleo, integración nativa con hardware simulado y validación manual del montaje. La matriz de trazabilidad evitará presentar un nivel como sustituto de otro y señalará explícitamente los bloqueos P-01 a P-03.

## Verification

**Commands:**
- `C:/Users/JOSEFRANZ/.platformio/penv/Scripts/platformio.exe test -e native` — todas las suites y casos aprobados.
- `C:/Users/JOSEFRANZ/.platformio/penv/Scripts/platformio.exe run -e esp32doit-devkit-v1` — compilación ESP32 correcta.
- `git diff --check` — sin errores de espacios ni marcadores conflictivos.

**Manual checks (if no CLI):**
- Comparar cada tabla y diagrama documental con `include/`, `src/`, `platformio.ini` y los nombres reales de los casos en `test/`.
