# Plan integral de pruebas

## 1. Objetivo y alcance

Este plan verifica el firmware del indicador de distancia en cuatro niveles de evidencia que deben registrarse por separado:

1. revisión estática de requisitos, contratos, GPIO y documentación;
2. pruebas unitarias del núcleo con reloj y flancos simulados;
3. integración nativa de `setup()` y `loop()` con dobles de Arduino;
4. validación manual del montaje y de sus tiempos con instrumentos.

Las pruebas nativas no demuestran compatibilidad eléctrica, precisión acústica ni tiempos físicos. La compilación ESP32 demuestra que el firmware se construye para la placa configurada, pero no demuestra que haya sido cargado. P-01 bloquea la conexión de Echo al GPIO 19 hasta medir una tensión máxima no superior a 3,6 V. P-02 exige confirmar placa, polaridad y características de los LEDs. P-03 limita la detección: un eco espurio puede parecer una lectura válida aunque el objeto esté fuera del rango nominal.

## 2. Entorno y evidencia automatizada

| Elemento | Configuración |
|---|---|
| Lenguaje | C++17 |
| Framework embebido | Arduino ESP32 `3.20017.241212+sha.dcc1105b` |
| Plataforma ESP32 | `espressif32@7.1.1` |
| Placa | `esp32doit-devkit-v1` |
| Entorno nativo | PlatformIO `native@1.2.1`, Unity `2.6.1` |
| GPIO | Trigger 18, Echo 19, rojo 25, amarillo 26, verde 27 |

Comandos desde la raíz del repositorio:

```powershell
pio test -e native
pio run -e esp32doit-devkit-v1
git diff --check
```

Si `pio` no está en `PATH`, puede usarse `platformio` con los mismos argumentos. En el equipo que generó la evidencia se usó como alternativa local `C:/Users/JOSEFRANZ/.platformio/penv/Scripts/platformio.exe`.

La salida completa y fechada se conserva en `_bmad-output/implementation-artifacts/verification-native.txt` y `_bmad-output/implementation-artifacts/verification-esp32.txt`. Solo se marca una ejecución como aprobada después de ejecutar realmente el comando y comprobar código de salida cero.

## 3. Inventario de pruebas Unity

| ID | Suite y caso Unity | Propósito | Requisitos/contratos | Estado registrado |
|---|---|---|---|---|
| UT-01 | `test_indicator/test_boundaries_and_exclusion` | Límites 2/10/30/400 cm y exclusión de LEDs | RF-02–RF-04, AD-4, PU-01, PU-05 | Aprobado 2026-09-14 |
| UT-02 | `test_indicator/test_invalid_values` | Ausencia, negativos, fuera de rango, NaN e infinito | RF-01, RF-05, AD-4, PU-02 | Aprobado 2026-09-14 |
| UT-03 | `test_indicator/test_phase_and_repeated_invalid` | Arranque, marcas 249/250/499/500 ms, saltos y error repetido | RF-05, RF-07, RNF-05, AD-4, PU-03, PU-04, PU-08 | Aprobado 2026-09-14 |
| UT-04 | `test_indicator/test_recovery_and_new_error` | Recuperación inmediata y reinicio de fase al aparecer un error nuevo | RF-06, AD-4, PU-06 | Aprobado 2026-09-14 |
| UT-05 | `test_indicator/test_wrap` | Parpadeo al desbordar `uint32_t` | RNF-04, AD-4, PU-07 | Aprobado 2026-09-14 |
| UT-06 | `test_echo_capture/test_ordered_pulse_and_consumption` | Orden de flancos, pulso de 580 µs y consumo único | RF-01, AD-2, AD-3, PU-09 | Aprobado 2026-09-14 |
| UT-07 | `test_echo_capture/test_timeout_boundaries` | Fin antes del límite y timeout a 30 000 µs | RF-06, RNF-05, AD-3 | Aprobado 2026-09-14 |
| UT-08 | `test_echo_capture/test_partial_missing_late_and_old_edges` | Flancos parciales, ausentes, tardíos o anteriores | RF-01, RF-06, AD-3 | Aprobado 2026-09-14 |
| UT-09 | `test_echo_capture/test_pending_protection_and_wrap` | Resultado pendiente y desbordamiento del reloj | RNF-04, RNF-05, AD-2, AD-3 | Aprobado 2026-09-14 |
| UT-10 | `test_echo_capture/test_rise_at_deadline_and_duplicate_edges` | Subida al límite, duplicados y orden temporal | RF-01, AD-3 | Aprobado 2026-09-14 |
| UT-11 | `test_sensor/test_conversion_and_no_overwrite` | Configuración GPIO/CHANGE, mismo mutex, Trigger de 10 µs, conversión y consumo | RF-01, RNF-02, RNF-05, AD-1–AD-3, AD-5, PU-09 | Aprobado 2026-09-14 |
| UT-12 | `test_sensor/test_echo_high_and_period` | Echo alto inicial sin Trigger y período mínimo de 100 ms | RF-01, RF-06, RNF-05, AD-3 | Aprobado 2026-09-14 |
| UT-13 | `test_sensor/test_timeout_and_wrap` | Timeout, consumo único, período y desbordamiento | RF-06, RNF-05, AD-2, AD-3 | Aprobado 2026-09-14 |
| UT-14 | `test_sensor/test_led_gpio` | GPIO 25/26/27 activos altos y escritura de los tres LEDs | RF-02–RF-05, AD-4, AD-6 | Aprobado 2026-09-14 |
| UT-15 | `test_application/test_firmware_start_timeout_blink_and_recovery` | Integración real de `setup()`/`loop()`: arranque, timeout, parpadeo y recuperación | RF-01–RF-07, RNF-02, RNF-05, AD-1–AD-5 | Aprobado 2026-09-14 |

## 4. Orden seguro de validación manual

Ejecutar M-01 y M-02 antes de cualquier conexión de Echo. Si M-02 no demuestra `Echo ≤ 3,6 V`, registrar el bloqueo y no ejecutar M-03 a M-08 con Echo conectado al GPIO 19. Desenergizar antes de modificar cableado. Ejecutar luego M-03, M-04, M-05 y M-06; finalizar con M-07 y M-08 usando instrumentos con rango y masa adecuados.

Instrumentos: multímetro, osciloscopio o analizador lógico apto para los niveles medidos, regla o cinta métrica, blanco plano, cronómetro como comprobación auxiliar y equipo con PlatformIO. Las capturas deben mostrar escala, canales, puntos de medida y fecha.

### M-01 — Inspección de placa, LEDs y alimentación

- **Seguridad:** montaje desenergizado; no aplicar 5 V al pin de 3,3 V; no conectar Echo a GPIO 19.
- **Preparación:** placa y sensor identificables; hoja de datos disponible; multímetro con exactitud declarada para resistencia; tres resistencias marcadas como 220 Ω y su tolerancia identificada por bandas o ficha.
- **Pasos:** (1) confirmar `esp32doit-devkit-v1` o equivalencia documentada; (2) identificar 5 V, 3,3 V y GND; (3) comprobar masa común; (4) medir cada resistencia fuera de tensión y registrar valor, tolerancia marcada e incertidumbre `U_R` del instrumento; (5) verificar una resistencia por LED, ánodo hacia GPIO 25/26/27 y cátodo a GND; (6) comprobar Trigger en GPIO 18 y dejar Echo aislado.
- **Resultado esperado:** polaridad y continuidad coinciden con el esquema; no hay cortocircuitos ni conexión de Echo; las tres resistencias cumplen `|R_medida − 220 Ω| + U_R ≤ 220 Ω × tolerancia_marcada`.
- **Aprobación:** todos los puntos son verificables y las tres mediciones cumplen la desigualdad. Cualquier resistencia fuera de tolerancia, duda o continuidad indebida suspende el montaje.
- **Evidencia:** fotografías generales y de detalle, identificación de placa/sensor, bandas o ficha de tolerancia, modelo/rango/exactitud del multímetro y tabla de continuidad y resistencia.

### M-02 — Compatibilidad eléctrica de Echo (puerta P-01)

- **Seguridad:** GPIO 19 desconectado; usar sonda de alta impedancia y rango superior a 5 V; unir masas antes de medir; no tocar el circuito energizado.
- **Preparación:** M-01 aprobada; sensor alimentado con 5 V autorizado; Trigger generado por GPIO 18; osciloscopio o instrumento que capture el máximo de Echo y cuya exactitud permita declarar una incertidumbre expandida `U_V` en ese rango.
- **Pasos:** (1) conectar la sonda entre Echo y GND sin unir Echo al ESP32; (2) documentar sonda, atenuación, ancho de banda, rango, exactitud y `U_V`; (3) energizar; (4) provocar al menos diez ecos con blancos entre 5 y 100 cm y una condición sin eco; (5) registrar el máximo de todas las capturas; (6) desenergizar.
- **Resultado esperado:** forma de pulso estable y margen demostrado frente al límite mediante `V_Echo,max + U_V ≤ 3,6 V`.
- **Aprobación:** la desigualdad se cumple para el máximo de todas las capturas. Si no se cumple o no puede cuantificarse `U_V`, P-01 queda bloqueado y no se conecta GPIO 19.
- **Evidencia:** capturas con escala vertical, máximo medido, conexión y atenuación de sonda, modelo/rango/exactitud del instrumento, cálculo de `U_V` y fecha.

### M-03 — Carga, arranque y estado sin lectura

- **Seguridad:** M-01 y M-02 aprobadas; desenergizar para conectar Echo a GPIO 19; revisar masa común antes de energizar.
- **Preparación:** M-01/M-02 aprobadas; `firmware.bin` compilado y su SHA-256 comparado con `_bmad-output/implementation-artifacts/verification-esp32.txt`; cableado aprobado; LEDs visibles; objeto fuera del haz.
- **Pasos:** (1) ejecutar `pio run -e esp32doit-devkit-v1`; (2) calcular el SHA-256 de `.pio/build/esp32doit-devkit-v1/firmware.bin` y exigir coincidencia con la evidencia; (3) sin cambiar fuentes, cargar con `pio run -e esp32doit-devkit-v1 -t upload` y conservar la salida que identifica el artefacto usado; (4) recalcular inmediatamente el SHA-256 del `firmware.bin` que dejó el comando de carga y exigir la misma coincidencia; (5) reiniciar la placa; (6) observar los tres LEDs durante cinco segundos sin eco; (7) repetir un reinicio.
- **Resultado esperado:** el sistema arranca en condición inválida, con los tres LEDs encendidos, y luego los tres alternan juntos.
- **Aprobación:** los SHA-256 anterior y posterior a la carga coinciden con la evidencia; no aparece un color individual antes de una lectura válida y los tres LEDs siempre cambian simultáneamente. Una diferencia de hash invalida el ensayo físico.
- **Evidencia:** SHA-256 verificado, log de compilación/carga, versión y puerto de placa, y video continuo de ambos reinicios.

### M-04 — Bandas válidas y exclusión de LEDs

- **Seguridad:** no mover conexiones con alimentación; mantener manos y conductores fuera de la placa.
- **Preparación:** M-03 aprobada; blanco plano perpendicular; distancia medida desde la cara emisora del sensor.
- **Pasos:** ubicar el blanco durante al menos tres segundos en 5 cm, 20 cm y 50 cm; repetir tres veces cada posición, alejándolo entre repeticiones.
- **Resultado esperado:** 5 cm enciende solo rojo; 20 cm solo amarillo; 50 cm solo verde.
- **Aprobación:** las nueve observaciones muestran exactamente un LED y el color esperado. Los umbrales exactos se acreditan con UT-01, no con precisión centesimal del montaje.
- **Evidencia:** tabla con distancia de referencia, color observado, repetición, condiciones del blanco y fotografías o video.

### M-05 — Recuperación después de una lectura inválida

- **Seguridad:** mantener el montaje fijo y energizado; mover únicamente el blanco.
- **Preparación:** estado amarillo estable a 20 cm y cronómetro o video.
- **Pasos:** (1) retirar el blanco completamente del haz; (2) exigir que el parpadeo comience en un máximo de un segundo; (3) esperar otro segundo; (4) reubicarlo a 20 cm; (5) exigir amarillo estable en un máximo de un segundo; (6) repetir cinco veces.
- **Resultado esperado:** tras cada ausencia aparece el parpadeo conjunto y, al volver el blanco, se enciende solo amarillo en la siguiente actualización disponible.
- **Aprobación:** las cinco repeticiones muestran primero parpadeo y luego recuperación amarilla dentro de los plazos. Si falta el parpadeo, no vuelve amarillo o aparece otro color, el caso falla.
- **Evidencia:** video continuo y tabla con instante de reposición, salida observada y veredicto.

### M-06 — Límites físicos y observabilidad P-03

- **Seguridad:** no acercar el blanco hasta tocar el sensor; respetar su montaje mecánico.
- **Preparación:** montaje aprobado, cinta de al menos 5 m, blanco plano usado en M-04 y espacio que mantenga geometría y orientación constantes.
- **Pasos:** ejecutar cinco ensayos independientes en cada condición: blanco a 1 cm, blanco a 450 cm y blanco retirado del haz. Mantener cada condición 5 s, separar ensayos con 3 s a 20 cm y registrar todo el intervalo en video.
- **Resultado esperado:** ausencia o timeout produce parpadeo. Una lectura espuria puede producir un color y debe registrarse como manifestación de P-03.
- **Aprobación:** si aparece parpadeo, debe mantenerse conjunto durante la condición; si aparece una salida inválida reconocible sin parpadeo, el caso falla. Una mezcla de parpadeo y colores espurios se aprueba con la limitación P-03 registrada. Si las cinco repeticiones de 1 cm o las cinco de 450 cm producen solo colores estables, esa condición queda **inconclusa** por P-03; no se declara aprobada ni fallida. Si las cinco repeticiones sin blanco producen solo color estable, el caso falla.
- **Evidencia:** tabla de distancia/condición, salida observada, geometría del blanco, repeticiones y eventos espurios.

### M-07 — Pulso Trigger, separación y timeout

- **Seguridad:** conectar sondas con el equipo desenergizado; masas comunes; no sondear Echo si el instrumento no admite el nivel medido.
- **Preparación:** osciloscopio o analizador lógico con resolución de microsegundos; canales en Trigger y, tras M-02, Echo.
- **Pasos:** (1) estabilizar primero una salida de color; (2) capturar al menos diez pulsos Trigger y medir ancho alto y separación entre flancos ascendentes consecutivos; (3) capturar una pareja Echo válida que finalice antes de 30 ms; (4) retirar el blanco y medir desde el último Trigger hasta la subida de uno de los dos GPIO LED que estaban apagados, señal de que se publicó el error.
- **Resultado esperado:** Trigger alto al menos 10 µs; inicios separados al menos 100 ms; el eco válido finaliza antes de 30 ms; sin eco, el estado inválido se publica entre 30 y 40 ms después de Trigger. El límite exacto de 30 000 µs está cubierto por UT-07 y UT-10.
- **Aprobación:** mínimos y ventana de timeout se cumplen en todas las capturas; no aparecen ráfagas para recuperar ciclos atrasados.
- **Evidencia:** capturas con cursores para ancho, separación y timeout, más una tabla de diez mediciones.

### M-08 — Parpadeo y retraso de actualización

- **Seguridad:** mismas precauciones de M-07; medir GPIO de LED, no corriente, salvo procedimiento de laboratorio autorizado.
- **Preparación:** osciloscopio o analizador de cuatro canales. Para recuperación, medir Echo y GPIO 25/26/27; para timeout, medir Trigger y los tres GPIO. Preparar una condición inválida reproducible y un eco válido cuya bajada se observe claramente.
- **Pasos:** (1) capturar diez medias fases del parpadeo; (2) medir tiempos encendido y apagado, período completo y diferencia entre el primer y último flanco de GPIO 25/26/27; (3) provocar cinco transiciones inválida→válida y medir desde la bajada de Echo hasta el último cambio de salida; (4) provocar cinco transiciones válida→inválida sin eco y medir desde el límite de 30 ms posterior a Trigger hasta el último cambio de salida. Si solo hay dos canales, repetir cada condición cinco veces por par de GPIO (25–26, 26–27 y 25–27), conservando idéntica posición y disparo; medir después Echo/Trigger contra cada GPIO en cinco repeticiones.
- **Resultado esperado:** 250 ms encendido, 250 ms apagado y período de 500 ms; sesgo máximo entre GPIO de 1 ms; retraso máximo objetivo de 10 ms.
- **Aprobación:** cada media fase está dentro de 250 ±10 ms, cada período dentro de 500 ±20 ms, cada sesgo entre pines es ≤1 ms y cada retraso observado es ≤10 ms. Con dos canales, todas las repeticiones de cada par y referencia deben cumplir.
- **Evidencia:** capturas superpuestas de GPIO 25/26/27, cursores temporales y tabla de transiciones.

## 5. Matriz de trazabilidad

| Fuente | Evidencia automatizada | Evidencia manual | Estado físico |
|---|---|---|---|
| RF-01 | UT-02, UT-06–UT-13, UT-15 | M-03, M-05–M-07 | Pendiente |
| RF-02–RF-04 | UT-01, UT-14, UT-15 | M-04 | Pendiente |
| RF-05 | UT-02–UT-03, UT-14–UT-15 | M-03, M-06, M-08 | Pendiente |
| RF-06 | UT-04, UT-07–UT-08, UT-12–UT-13, UT-15 | M-05, M-07 | Pendiente |
| RF-07 | UT-03, UT-15 | M-03 | Pendiente |
| RNF-01–RNF-03 | revisión de código y compilación | No aplica | Revisado documentalmente |
| RNF-04 | UT-01–UT-15 | No aplica | Evidencia nativa |
| RNF-05 | UT-03, UT-07–UT-13, UT-15 | M-07, M-08 | Pendiente |
| RNF-06 | No sustituible por software | M-01, M-02 | Bloqueado hasta ejecutar |
| AD-1–AD-2 | UT-06, UT-09, UT-11, UT-13, UT-15 | No aplica | Evidencia nativa |
| AD-3 | UT-06–UT-13, UT-15 | M-07 | Pendiente |
| AD-4 | UT-01–UT-05, UT-14–UT-15 | M-03–M-05, M-08 | Pendiente |
| AD-5 | UT-01–UT-15 y compilación ESP32 | M-01–M-08 | Parcial |
| AD-6 | revisión de GPIO y alcance | M-01, M-02 | Bloqueado hasta ejecutar |

## 6. Registro de ejecución

Copiar una fila por ejecución; no completar resultados por inferencia.

| Fecha/hora | ID/caso | Versión o commit | Equipo/instrumento | Precondiciones | Entrada | Resultado observado | Evidencia (ruta) | Veredicto | Responsable |
|---|---|---|---|---|---|---|---|---|---|
| Pendiente | Pendiente | Pendiente | Pendiente | Pendiente | Pendiente | Pendiente | Pendiente | No ejecutado | Pendiente |

Un veredicto aprobado requiere que se cumpla el criterio explícito del caso y que la evidencia sea legible. Registrar desviaciones con valor observado, repetibilidad e impacto. No reinterpretar compilación o simulación como resultado eléctrico, acústico o temporal.
