# Revisión independiente de evidencia tecnológica

Fecha: 2026-09-07. Artefacto: `ARCHITECTURE-SPINE.md`. Alcance: contraste de afirmaciones técnicas con configuración, manifiestos locales y fuentes primarias; sin compilación ni ensayos físicos.

## Dictamen

Sin hallazgos bloqueantes en esta revisión. El documento distingue adecuadamente el entorno observado, las decisiones propuestas y la validación pendiente. No se hallaron versiones inventadas ni afirmaciones de pruebas ejecutadas.

## Evidencia contrastada

| Afirmación | Evidencia | Resultado |
|---|---|---|
| Plataforma instalada 7.1.1 | `C:/Users/JOSEFRANZ/.platformio/platforms/espressif32/platform.json`, campo `version`; [publicación oficial](https://github.com/platformio/platform-espressif32/releases/tag/v7.1.1) | Coincide; la fijación futura se distingue de la configuración actual sin versión. |
| Paquete Arduino instalado `3.20017.241212+sha.dcc1105b` | `C:/Users/JOSEFRANZ/.platformio/packages/framework-arduinoespressif32/package.json` | Coincide; está identificado como versión del paquete PlatformIO. |
| Placa declarada y framework | `platformio.ini`: `esp32doit-devkit-v1`, `arduino` | Coincide; no se confunde con identificación física. |
| Disponibilidad de `attachInterruptArg` | Encabezado instalado `cores/esp32/esp32-hal-gpio.h:79` | La firma existe; esto no acredita todavía corrección temporal o seguridad de ISR del futuro código. |
| Rango, disparo y conversión del HC-SR04 | [Ficha de ElecFreaks](https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf), páginas 1 y 2 | Apoya 2–400 cm, disparo de al menos 10 µs y µs/58; el período propuesto de 100 ms supera la recomendación de más de 60 ms. La identidad de la unidad real sigue pendiente. |
| Tolerancia GPIO 3,6 V | [FAQ oficial de Espressif](https://docs.espressif.com/projects/esp-faq/en/latest/hardware-related/hardware-design.html#what-is-the-voltage-tolerance-of-gpios-of-esp-chips) | Apoya mantener Echo desconectado mientras su compatibilidad no esté resuelta. No se afirma que alimentar la placa con 5 V permita aplicarlos a GPIO. |
| Estado de pruebas, C++17 y Unity | Etiquetas y pendientes del propio documento, contrastados con `platformio.ini` | Correctamente propuestos; la configuración actual no declara C++17 ni entorno anfitrión. |

## Límites del dictamen

No verifica montaje, modelo exacto del sensor, desempeño de captura, compilación, instalación de Unity ni resultados unitarios. Estos puntos siguen diferidos de forma explícita. El plazo de 30 ms y el retraso máximo de salida de 10 ms son decisiones de diseño, no especificaciones certificadas por las fuentes ni resultados medidos. El bloqueo físico P-01/P-02 y la limitación de observabilidad P-03 permanecen pertinentes.
