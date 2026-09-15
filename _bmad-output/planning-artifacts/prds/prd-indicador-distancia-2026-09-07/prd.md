---
title: Indicador de distancia con ESP32
status: implemented-software
created: 2026-09-07
updated: 2026-09-14
---

# PRD: indicador de distancia con ESP32

## Propósito y alcance

Práctica educativa de JOSE FRANZ: indicar la distancia mediante tres LEDs. Este documento establece los requisitos implementados y la forma de evaluar el ejercicio. El [informe técnico](informe-tecnico.md) describe la implementación y el [plan integral de pruebas](../../../../PLAN-DE-PRUEBAS.md) reúne la verificación automatizada y manual. Estado: firmware implementado, 15 casos Unity aprobados y compilación ESP32 aprobada; montaje y mediciones físicas pendientes.

Componentes autorizados: ESP32, sensor ultrasónico identificado por el usuario como HC-R04 de 5 V, LED rojo, amarillo y verde, y resistencias de 220 Ω para los LEDs. Se considera una resistencia independiente por LED. No se incorporan componentes adicionales.

## Requerimientos funcionales

- **RF-01:** medir periódicamente la distancia y distinguir una medición válida de una medición inválida o ausente.
- **RF-02:** para una medición válida menor de 10 cm, encender solamente el LED rojo.
- **RF-03:** desde 10 cm inclusive hasta menos de 30 cm, encender solamente el LED amarillo.
- **RF-04:** desde 30 cm inclusive hasta el límite superior válido, encender solamente el LED verde.
- **RF-05:** ante medición fuera del rango válido, encender y apagar simultáneamente los tres LEDs a 2 Hz: período de 500 ms, con 250 ms encendidos y 250 ms apagados. La condición inválida tiene prioridad sobre RF-02 a RF-04.
- **RF-06:** [SUPUESTO S-02] tratar ausencia de eco o tiempo de espera agotado como medición inválida; recuperar la indicación normal al recibir la siguiente medición válida. No conservar indefinidamente un color anterior.
- **RF-07:** [SUPUESTO S-03] comenzar en condición inválida hasta obtener la primera medición válida. Las lecturas inválidas consecutivas no reinician la fase de parpadeo.

## Requerimientos no funcionales

- **RNF-01:** C++ moderno e idiomático, orientado a objetos; identificadores y comentarios del código en inglés. Documentación explicativa en español.
- **RNF-02:** separar adquisición, decisión y salida lo suficiente para probar la lógica sin hardware; evitar abstracciones o dependencias sin utilidad en este alcance.
- **RNF-03:** comentarios únicamente cuando aporten información que no expresa el código; estructura de archivos pequeña y navegable.
- **RNF-04:** pruebas unitarias de límites, exclusión de LEDs, errores, recuperación y temporización mediante reloj controlable; no depender de esperas reales.
- **RNF-05:** ninguna espera indefinida de Echo; adquisición y salida deben permitir mantener el parpadeo. [SUPUESTO S-04] período de adquisición de 100 ms, espera máxima de 30 ms y actualización de salida con retraso máximo de 10 ms respecto de su instante programado; son objetivos de diseño pendientes de validar.
- **RNF-06:** no conectar una señal que exceda la tolerancia del GPIO del ESP32. La incompatibilidad de nivel pendiente impide aprobar el circuito completo.

## Criterios de aceptación

| Entrada de prueba | Salida esperada |
|---|---|
| Inválida, ausente, menor de 2 cm o mayor de 400 cm | Tres LEDs a 2 Hz |
| 2; 9,99 cm | Solo rojo |
| 10; 29,99 cm | Solo amarillo |
| 30; 400 cm | Solo verde |
| Inválida seguida de 20 cm | Amarillo en la siguiente actualización |

Los límites 2 y 400 cm dependen de S-01. Los valores exactos de la tabla se inyectan en pruebas unitarias; no implican precisión física centesimal. Los 15 casos unitarios están aprobados. El cierre del producto exige además comprobar físicamente cada estado después de resolver la conexión. Contramétrica: no aumentar la frecuencia de medición a costa de ecos residuales o temporización incorrecta.

## Estado de implementación y verificación

El firmware implementa `DistanceIndicator`, `EchoCapture`, `UltrasonicSensor`, `LedDriver` y la composición en `setup()`/`loop()`. Usa GPIO 18/19/25/26/27, interrupción `CHANGE`, timeout de 30 ms, separación mínima de 100 ms entre adquisiciones y Trigger alto durante al menos 10 µs. Las firmas y responsabilidades se detallan en la [arquitectura](../../architecture/architecture-indicador-distancia-2026-09-07/ARCHITECTURE-SPINE.md).

La ejecución del 2026-09-14 aprobó los 15 casos Unity en cuatro suites y la compilación para `esp32doit-devkit-v1`. Las salidas completas están en [`verification-native.txt`](../../../implementation-artifacts/verification-native.txt) y [`verification-esp32.txt`](../../../implementation-artifacts/verification-esp32.txt). Esta evidencia cubre el software y la construcción del firmware; no acredita carga en placa, tensión de Echo, precisión acústica ni tiempos reales. Los casos manuales M-01 a M-08 permanecen sin ejecutar.

## Exclusiones

Sin Wi-Fi, Bluetooth, servicios remotos, pantallas, aplicaciones, almacenamiento, buzzer ni sensores adicionales. Sin filtrado, histéresis o calibración configurables que alteren los umbrales solicitados. Las herramientas de prueba no son componentes del montaje.

## Supuestos y pendientes

- **S-01:** se interpreta HC-R04 como HC-SR04 convencional, con rango nominal inclusivo de 2 a 400 cm. Responsable: JOSE FRANZ; confirmar referencia exacta antes de cerrar diseño eléctrico. La tensión de alimentación por sí sola no identifica la variante.
- **S-02/S-03:** ausencia de eco como lectura inválida y arranque en estado inválido, implementados y cubiertos por las pruebas nativas. Responsable: JOSE FRANZ; confirmar su aceptación durante la validación física.
- **S-04:** el período de adquisición de 100 ms y el timeout de 30 ms están implementados y verificados con tiempo simulado. El objetivo de actualización de 10 ms está implementado como actualización en cada iteración, pero su cumplimiento temporal no fue verificado; permanece pendiente de M-08.
- **P-01, bloqueante:** verificar nivel de Echo de la unidad concreta. Para una salida de 5 V no es admisible la conexión directa al ESP32. Las tres resistencias destinadas a los LEDs no resuelven esta interfaz. No se autoriza añadir adaptación ni sustituir el sensor en este documento.
- **P-02:** confirmar placa física y polaridad/tensión directa de los LEDs antes del cableado; PlatformIO declara `esp32doit-devkit-v1` con Arduino.
- **P-03, límite de observabilidad:** un objeto fuera del rango puede producir un eco aparentemente válido. Con este sensor no se puede garantizar detectar todas las distancias físicas inválidas. RF-05 se verifica sobre lecturas detectadas como inválidas; esta limitación debe aceptarse antes de declarar cumplimiento físico.

## Fuentes técnicas

- [Ficha HC-SR04 de ElecFreaks, alojada por SparkFun](https://cdn.sparkfun.com/datasheets/Sensors/Proximity/HCSR04.pdf): rango nominal y temporización.
- [Espressif: tolerancia de GPIO](https://docs.espressif.com/projects/esp-faq/en/latest/hardware-related/hardware-design.html#what-is-the-voltage-tolerance-of-gpios-of-esp-chips): límite de 3,6 V.
