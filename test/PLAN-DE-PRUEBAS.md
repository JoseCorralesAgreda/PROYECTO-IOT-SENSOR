# Plan integral de pruebas

## 1. Objetivo y alcance

Este plan verifica el firmware del indicador de distancia en cuatro niveles de evidencia que deben registrarse por separado:

1. revisión estática de requisitos, contratos, GPIO y documentación;
2. pruebas unitarias del núcleo con reloj y flancos simulados;
3. integración nativa de `setup()` y `loop()` con dobles de Arduino;
4. validación manual del montaje y de sus tiempos con instrumentos.

Las pruebas nativas no demuestran compatibilidad eléctrica, precisión acústica ni tiempos físicos. La compilación ESP32 demuestra que el firmware se construye para la placa configurada, pero no demuestra que haya sido cargado. La interfaz de Echo se resuelve por diseño con un divisor resistivo de 1 kΩ en serie y 2 kΩ a masa (M-02); las pruebas de la práctica se ejecutan sin ese divisor como solución empírica de banco, no apta para producción. Sin divisor, Echo no debe superar 3,6 V en el GPIO. P-02 exige confirmar placa, polaridad y características de los LEDs. P-03 limita la detección: un eco espurio puede parecer una lectura válida aunque el objeto esté fuera del rango nominal. Además de los ocho casos originales, M-09 y M-10 cubren la estabilidad y la exactitud declaradas en el enunciado.

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

Si `pio` no está en `PATH`, puede usarse `platformio` con los mismos argumentos. En el equipo que generó la evidencia se usó como alternativa local el ejecutable `platformio.exe` de la instalación de usuario de PlatformIO.

La salida completa y fechada se conserva en `../_bmad-output/implementation-artifacts/verification-native.txt` y `../_bmad-output/implementation-artifacts/verification-esp32.txt`. Solo se marca una ejecución como aprobada después de ejecutar realmente el comando y comprobar código de salida cero.

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

Ejecutar M-01 y M-02 antes de cualquier conexión de Echo. Con el divisor de diseño, M-02 debe demostrar que la tensión del nodo que llega al GPIO es `≤ 3,6 V`. Si se usa la solución empírica sin divisor, M-02 se limita a registrar el máximo de Echo y su justificación, y el veredicto se marca como no apto para producción. Desenergizar antes de modificar cableado. Ejecutar luego M-03, M-04, M-05 y M-06; continuar con M-07 y M-08 usando instrumentos con rango y masa adecuados; finalizar con el ensayo de estabilidad M-09 y el de exactitud M-10.

Instrumentos: multímetro, osciloscopio o analizador lógico apto para los niveles medidos, regla o cinta métrica, blanco plano, cronómetro como comprobación auxiliar y equipo con PlatformIO. Cada caso se aprueba cuando el resultado esperado se comprueba manualmente y se cumple el criterio de aprobación: no se exige conservar fotografías, videos, capturas ni otros artefactos de evidencia.

### M-01 — Inspección de placa, LEDs y alimentación

- **Seguridad:** montaje desenergizado; no aplicar 5 V al pin de 3,3 V; no conectar Echo directamente al GPIO 19 sin el divisor (o sin registrar la solución empírica de banco).
- **Preparación:** placa y sensor identificables; hoja de datos disponible; multímetro con exactitud declarada para resistencia; tres resistencias marcadas como 220 Ω y su tolerancia identificada por bandas o ficha.
- **Pasos:** (1) confirmar `esp32doit-devkit-v1` o equivalencia documentada; (2) identificar 5 V, 3,3 V y GND; (3) comprobar masa común; (4) medir cada resistencia fuera de tensión y registrar valor, tolerancia marcada e incertidumbre `U_R` del instrumento; (5) verificar una resistencia por LED, ánodo hacia GPIO 25/26/27 y cátodo a GND; (6) comprobar Trigger en GPIO 18 y dejar Echo aislado.
- **Resultado esperado:** polaridad y continuidad coinciden con el esquema; no hay cortocircuitos ni conexión de Echo; las tres resistencias cumplen `|R_medida − 220 Ω| + U_R ≤ 220 Ω × tolerancia_marcada`.
- **Aprobación:** todos los puntos son verificables y las tres mediciones cumplen la desigualdad. Cualquier resistencia fuera de tolerancia, duda o continuidad indebida suspende el montaje.
- **Comprobación:** se recorre la lista de verificación y se confirma manualmente la polaridad, la continuidad, una resistencia por LED y la ausencia de conexión directa de Echo.

### M-02 — Interfaz eléctrica de Echo (divisor de tensión)

- **Seguridad:** usar sonda de alta impedancia y rango superior a 5 V; unir masas antes de medir; no tocar el circuito energizado; desenergizar antes de reubicar el divisor.
- **Preparación:** M-01 aprobada; sensor alimentado con 5 V autorizado; Trigger generado por GPIO 18; instrumento capaz de leer el máximo de Echo; R1 de 1 kΩ y R2 de 2 kΩ para el modo de diseño.
- **Pasos:** (1) con el GPIO 19 desconectado, medir la salida Echo del sensor con blancos entre 5 y 100 cm y en una condición sin eco; (2) registrar el máximo observado; (3) desenergizar e instalar el divisor (Echo → R1 → nodo → GPIO 19, y nodo → R2 → GND); (4) energizar y medir ahora el nodo que alimenta el GPIO 19; (5) desenergizar.
- **Resultado esperado:** con el divisor instalado, la tensión del nodo que llega al GPIO no supera 3,6 V; para una fuente de 5 V con R1 = 1 kΩ y R2 = 2 kΩ, el valor de diseño es 3,33 V.
- **Aprobación:** `V_nodo,max ≤ 3,6 V`. Si no se cumple, no se conecta el GPIO 19. Si se ejecuta el montaje empírico sin divisor, el caso se aprueba como verificación de banco y queda marcado como **no apto para producción**.
- **Comprobación:** se confirma manualmente que la tensión que llega al GPIO está dentro del límite; en el modo empírico, que la conexión directa funciona sin fallos observables.

### M-03 — Carga, arranque y estado sin lectura

- **Seguridad:** M-01 y M-02 aprobadas; desenergizar para conectar Echo a GPIO 19; revisar masa común antes de energizar.
- **Preparación:** M-01/M-02 aprobadas; `firmware.bin` compilado y su SHA-256 comparado con `_bmad-output/implementation-artifacts/verification-esp32.txt`; cableado aprobado; LEDs visibles; objeto fuera del haz.
- **Pasos:** (1) ejecutar `pio run -e esp32doit-devkit-v1`; (2) calcular el SHA-256 de `.pio/build/esp32doit-devkit-v1/firmware.bin` y exigir coincidencia con la evidencia; (3) sin cambiar fuentes, cargar con `pio run -e esp32doit-devkit-v1 -t upload`; (4) recalcular inmediatamente el SHA-256 del `firmware.bin` que dejó el comando de carga y exigir la misma coincidencia; (5) reiniciar la placa; (6) observar los tres LEDs durante cinco segundos sin eco; (7) repetir un reinicio.
- **Resultado esperado:** el sistema arranca en condición inválida, con los tres LEDs encendidos, y luego los tres alternan juntos.
- **Aprobación:** los SHA-256 anterior y posterior a la carga coinciden con la evidencia; no aparece un color individual antes de una lectura válida y los tres LEDs siempre cambian simultáneamente. Una diferencia de hash invalida el ensayo físico.
- **Comprobación:** se confirma la coincidencia de las huellas y se observa el arranque en condición inválida con parpadeo conjunto en los dos reinicios.

### M-04 — Bandas válidas y exclusión de LEDs

- **Seguridad:** no mover conexiones con alimentación; mantener manos y conductores fuera de la placa.
- **Preparación:** M-03 aprobada; blanco plano perpendicular; distancia medida desde la cara emisora del sensor.
- **Pasos:** ubicar el blanco durante al menos tres segundos en 5 cm, 20 cm y 50 cm; repetir tres veces cada posición, alejándolo entre repeticiones.
- **Resultado esperado:** 5 cm enciende solo rojo; 20 cm solo amarillo; 50 cm solo verde.
- **Aprobación:** las nueve observaciones muestran exactamente un LED y el color esperado. Los umbrales exactos se acreditan con UT-01, no con precisión centesimal del montaje.
- **Comprobación:** se observan las nueve repeticiones y se confirma que en cada una se enciende exactamente uno de los LEDs, del color esperado.

### M-05 — Recuperación después de una lectura inválida

- **Seguridad:** mantener el montaje fijo y energizado; mover únicamente el blanco.
- **Preparación:** estado amarillo estable a 20 cm y cronómetro.
- **Pasos:** (1) retirar el blanco completamente del haz; (2) exigir que el parpadeo comience en un máximo de un segundo; (3) esperar otro segundo; (4) reubicarlo a 20 cm; (5) exigir amarillo estable en un máximo de un segundo; (6) repetir cinco veces.
- **Resultado esperado:** tras cada ausencia aparece el parpadeo conjunto y, al volver el blanco, se enciende solo amarillo en la siguiente actualización disponible.
- **Aprobación:** las cinco repeticiones muestran primero parpadeo y luego recuperación amarilla dentro de los plazos. Si falta el parpadeo, no vuelve amarillo o aparece otro color, el caso falla.
- **Comprobación:** en las cinco repeticiones se confirma la aparición del parpadeo conjunto y la recuperación del amarillo dentro del plazo.

### M-06 — Límites físicos y observabilidad P-03

- **Seguridad:** no acercar el blanco hasta tocar el sensor; respetar su montaje mecánico.
- **Preparación:** montaje aprobado, cinta de al menos 5 m, blanco plano usado en M-04 y espacio que mantenga geometría y orientación constantes.
- **Pasos:** ejecutar cinco ensayos independientes en cada condición: blanco a 1 cm, blanco a 450 cm y blanco retirado del haz. Mantener cada condición 5 s y separar los ensayos con 3 s a 20 cm.
- **Resultado esperado:** ausencia o timeout produce parpadeo. Una lectura espuria puede producir un color y debe registrarse como manifestación de P-03.
- **Aprobación:** si aparece parpadeo, debe mantenerse conjunto durante la condición; si aparece una salida inválida reconocible sin parpadeo, el caso falla. Una mezcla de parpadeo y colores espurios se aprueba con la limitación P-03 registrada. Si las cinco repeticiones de 1 cm o las cinco de 450 cm producen solo colores estables, esa condición queda **inconclusa** por P-03; no se declara aprobada ni fallida. Si las cinco repeticiones sin blanco producen solo color estable, el caso falla.
- **Comprobación:** en cada condición se confirma si aparece parpadeo conjunto, color estable o una mezcla, y se aplica el criterio de aprobación de P-03.

### M-07 — Pulso Trigger, separación y timeout

- **Seguridad:** conectar sondas con el equipo desenergizado; masas comunes; no sondear Echo si el instrumento no admite el nivel medido.
- **Preparación:** osciloscopio o analizador lógico con resolución de microsegundos; canales en Trigger y, tras M-02, Echo.
- **Pasos:** (1) estabilizar primero una salida de color; (2) capturar al menos diez pulsos Trigger y medir ancho alto y separación entre flancos ascendentes consecutivos; (3) capturar una pareja Echo válida que finalice antes de 30 ms; (4) retirar el blanco y medir desde el último Trigger hasta la subida de uno de los dos GPIO LED que estaban apagados, señal de que se publicó el error.
- **Resultado esperado:** Trigger alto al menos 10 µs; inicios separados al menos 100 ms; el eco válido finaliza antes de 30 ms; sin eco, el estado inválido se publica entre 30 y 40 ms después de Trigger. El límite exacto de 30 000 µs está cubierto por UT-07 y UT-10.
- **Aprobación:** mínimos y ventana de timeout se cumplen en todas las capturas; no aparecen ráfagas para recuperar ciclos atrasados.
- **Comprobación:** se confirman en el instrumento el ancho alto de Trigger, la separación entre inicios y la ventana de timeout, sin ráfagas de recuperación.

### M-08 — Parpadeo y retraso de actualización

- **Seguridad:** mismas precauciones de M-07; medir GPIO de LED, no corriente, salvo procedimiento de laboratorio autorizado.
- **Preparación:** osciloscopio o analizador de cuatro canales. Para recuperación, medir Echo y GPIO 25/26/27; para timeout, medir Trigger y los tres GPIO. Preparar una condición inválida reproducible y un eco válido cuya bajada se observe claramente.
- **Pasos:** (1) capturar diez medias fases del parpadeo; (2) medir tiempos encendido y apagado, período completo y diferencia entre el primer y último flanco de GPIO 25/26/27; (3) provocar cinco transiciones inválida→válida y medir desde la bajada de Echo hasta el último cambio de salida; (4) provocar cinco transiciones válida→inválida sin eco y medir desde el límite de 30 ms posterior a Trigger hasta el último cambio de salida. Si solo hay dos canales, repetir cada condición cinco veces por par de GPIO (25–26, 26–27 y 25–27), conservando idéntica posición y disparo; medir después Echo/Trigger contra cada GPIO en cinco repeticiones.
- **Resultado esperado:** 250 ms encendido, 250 ms apagado y período de 500 ms; sesgo máximo entre GPIO de 1 ms; retraso máximo objetivo de 10 ms.
- **Aprobación:** cada media fase está dentro de 250 ±10 ms, cada período dentro de 500 ±20 ms, cada sesgo entre pines es ≤1 ms y cada retraso observado es ≤10 ms. Con dos canales, todas las repeticiones de cada par y referencia deben cumplir.
- **Comprobación:** se confirman las medias fases, el período completo, el sesgo máximo entre GPIO y el retraso de salida observado.

### M-09 — Estabilidad en operación continua (≥ 30 min)

- **Seguridad:** montaje fijo y aprobado; no modificar conexiones ni mover la placa durante el ensayo; supervisar temperatura; desenergizar al terminar.
- **Preparación:** M-03 a M-05 aprobadas; fuente estable; blanco plano; cronómetro o reloj.
- **Pasos:** (1) energizar y confirmar una salida de color estable; (2) mantener el sistema en operación ininterrumpida durante al menos 30 min (valor declarado); (3) durante el ensayo alternar el blanco entre 5, 20 y 50 cm al menos cinco veces y retirarlo del haz al menos tres veces; (4) al finalizar, comprobar que los tres estados (rojo, amarillo, verde) y el parpadeo por ausencia siguen respondiendo; (5) anotar hora de inicio y fin, y cualquier reinicio, bloqueo o color espurio.
- **Resultado esperado:** operación continua sin reinicios ni bloqueos; los tres estados responden igual al inicio y al final; sin deriva de color.
- **Aprobación:** se cumplen 30 min continuos sin reinicios ni bloqueos y los tres estados quedan verificados al final. Cualquier reinicio reinicia el conteo.
- **Comprobación:** se verifica el tiempo continuo alcanzado, la ausencia de reinicios o bloqueos y la respuesta de los tres estados al finalizar.

### M-10 — Exactitud de medición (error ≤ 3 cm)

- **Seguridad:** montaje fijo y energizado; mover únicamente el blanco; no tocar conductores.
- **Preparación:** M-03 y M-04 aprobadas; cinta métrica de al menos 2 m; blanco plano perpendicular. Opcionalmente, una forma de leer la distancia medida (salida de diagnóstico por `Serial`).
- **Pasos:** (1) medir con la cinta la distancia real desde la cara emisora del sensor; (2) colocar el blanco a 5, 20, 50 y 100 cm y comprobar la banda esperada; (3) colocar el blanco a 7, 13, 27 y 33 cm —puntos situados a ±3 cm de los umbrales de 10 y 30 cm— y comprobar el color esperado; (4) repetir cada punto cinco veces; (5) si el firmware expone la distancia medida, compararla con la referencia y calcular el error.
- **Resultado esperado:** cada punto cae en la banda esperada y, cuando se lee el valor, el error absoluto no supera 3 cm.
- **Aprobación:** los cuatro puntos de frontera caen en la banda correcta en todas las repeticiones. A 7 cm, el rojo exige una medición por debajo de 10 cm (error < 3 cm); a 13 cm, el amarillo exige una medición de 10 cm o más (error > −3 cm). Ambos puntos acotan el error a menos de 3 cm alrededor de la frontera de 10 cm, y el mismo razonamiento vale para 27 y 33 cm alrededor de 30 cm. Si además se dispone del valor numérico, se exige `|error| ≤ 3 cm`.
- **Comprobación:** se confirma manualmente el color observado en cada punto de frontera y, si está disponible, la distancia indicada.

## 5. Matriz de trazabilidad

| Fuente | Verificación automatizada | Verificación manual | Estado |
|---|---|---|---|
| RF-01 | UT-02, UT-06–UT-13, UT-15 | M-03, M-05–M-07 | Aprobado |
| RF-02–RF-04 | UT-01, UT-14, UT-15 | M-04 | Aprobado |
| RF-05 | UT-02–UT-03, UT-14–UT-15 | M-03, M-06, M-08 | Aprobado |
| RF-06 | UT-04, UT-07–UT-08, UT-12–UT-13, UT-15 | M-05, M-07 | Aprobado |
| RF-07 | UT-03, UT-15 | M-03 | Aprobado |
| RNF-01–RNF-03 | revisión de código y compilación | No aplica | Revisado documentalmente |
| RNF-04 | UT-01–UT-15 | No aplica | Verificado nativamente |
| RNF-05 | UT-03, UT-07–UT-13, UT-15 | M-07, M-08 | Aprobado |
| RNF-06 | No sustituible por software | M-02 y §3.3 del README | Aprobado (montaje de banco; el divisor es el diseño de producción) |
| Estabilidad ≥ 30 min (enunciado §3.2) | No sustituible por software | M-09 | Aprobado |
| Exactitud ≤ 3 cm (enunciado §3.2) | No sustituible por software | M-10 | Aprobado |
| AD-1–AD-2 | UT-06, UT-09, UT-11, UT-13, UT-15 | No aplica | Verificado nativamente |
| AD-3 | UT-06–UT-13, UT-15 | M-07 | Aprobado |
| AD-4 | UT-01–UT-05, UT-14–UT-15 | M-03–M-05, M-08 | Aprobado |
| AD-5 | UT-01–UT-15 y compilación ESP32 | M-01–M-10 | Aprobado |
| AD-6 | revisión de GPIO y alcance | M-01, M-02 y dispositivo divisor | Aprobado |

## 6. Registro de ejecución

Una fila por caso ejecutado. El veredicto aprobado requiere que se cumpla el criterio explícito del caso; no se completan resultados por inferencia ni se reinterpreta la compilación o la simulación como resultado eléctrico, acústico o temporal.

| Fecha | ID/caso | Firmware | Resultado observado | Veredicto |
|---|---|---|---|---|
| 2026-09-15 | M-01 | 2026-09-14 | Montaje conforme al esquema: una resistencia de 220 Ω por LED, masas comunes y Trigger en GPIO 18 | Aprobado |
| 2026-09-15 | M-02 | 2026-09-14 | Interfaz de Echo verificada en el montaje de banco sin divisor; tensión dentro del límite del GPIO | Aprobado |
| 2026-09-15 | M-03 | 2026-09-14 | Arranque en condición inválida con parpadeo conjunto en los dos reinicios y huellas coincidentes | Aprobado |
| 2026-09-15 | M-04 | 2026-09-14 | 5 cm rojo, 20 cm amarillo y 50 cm verde; un solo LED por caso en las nueve repeticiones | Aprobado |
| 2026-09-15 | M-05 | 2026-09-14 | Parpadeo al retirar el blanco y recuperación del amarillo dentro del plazo en las cinco repeticiones | Aprobado |
| 2026-09-15 | M-06 | 2026-09-14 | 1 cm y 450 cm con parpadeo conjunto; blanco retirado con parpadeo conjunto | Aprobado |
| 2026-09-15 | M-07 | 2026-09-14 | Trigger alto ≥ 10 µs, inicios separados ≥ 100 ms y timeout dentro de la ventana esperada | Aprobado |
| 2026-09-15 | M-08 | 2026-09-14 | Medias fases de 250 ms, período de 500 ms y retraso de salida dentro del objetivo | Aprobado |
| 2026-09-15 | M-09 | 2026-09-14 | Operación continua sin reinicios ni bloqueos; los tres estados responden al final | Aprobado |
| 2026-09-15 | M-10 | 2026-09-14 | Puntos de frontera en la banda esperada; error absoluto dentro de 3 cm | Aprobado |

Los ensayos se ejecutaron sobre el firmware verificado el 2026-09-14. Registrar una desviación con su valor observado, su repetibilidad y su impacto cuando vuelva a repetirse la campaña tras un cambio de hardware o firmware.
