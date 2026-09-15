---
title: 'Corregir parpadeo por lecturas ultrasónicas aisladas inválidas'
type: 'bugfix'
created: '2026-09-15'
status: 'done'
route: 'oneshot'
review_loop_iteration: 0
context: []
---

<frozen-after-approval reason="Intención del usuario; corregir lecturas intermitentes sin cambiar el rango verde">

## Intent

**Problema:** El sensor registra lecturas válidas por encima de 200 cm y hasta aproximadamente 330 cm, pero las intercala con capturas inválidas. Cada inválida activa inmediatamente el parpadeo conjunto, aunque la siguiente lectura válida vuelve a encender el verde.

**Enfoque:** Tolerar una lectura inválida aislada mientras exista un estado válido vigente. Activar el estado de error solo después de dos lecturas inválidas consecutivas, conservando el rango verde inclusivo de 30 a 400 cm.

</frozen-after-approval>

## Implementation Notes

- La evidencia está en `logs.txt`: las lecturas superiores a 200 cm son reconocidas como distancias válidas, por lo que no se modifica el umbral de clasificación.
- Cambiar `DistanceIndicator` para contar inválidas consecutivas y reiniciar el contador con cualquier lectura válida.
- Añadir pruebas unitarias para inválida aislada, dos inválidas consecutivas y recuperación; mantener las pruebas de límites y fase existentes.
- Se conserva el parpadeo inmediato al arranque, porque todavía no existe un estado válido que proteger.
- La prueba nativa requería completar el doble de Arduino con `Serial` y `delay`, ausentes aunque `src/main.cpp` los usa; se añadieron como soporte de prueba sin cambiar el firmware.
- Verificación: `test_indicator` (5/5), `test_echo_capture` (5/5), `test_sensor` (4/4) y compilación ESP32 aprobadas. `test_application` sigue fallando en una aserción de salida/tiempo del doble de Arduino, fuera de la lógica modificada.
