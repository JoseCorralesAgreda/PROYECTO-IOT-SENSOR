# Revisión de contratos e integración

Revisión documental independiente, 2026-09-07. No se ejecutaron pruebas ni se inspeccionó un montaje.

## Hallazgos

1. **Media — Orden de consumo e inicio ambiguo (AD-2, AD-3 y secuencia).** AD-2 exige consumir antes de iniciar otra adquisición, pero el diagrama actualiza y puede disparar antes de `takeReading`. Un adaptador puede bloquear el inicio ante un resultado pendiente y esperar otra iteración; otro puede asumir que el coordinador ya lo consumió. La composición deja esa precondición sin garantizar. Fijar un orden único: resolver captura/timeout, consumir y entregar resultado, iniciar si corresponde y no hay resultado pendiente, actualizar LEDs. También es válido definir expresamente que la actualización difiere el disparo mientras exista un resultado pendiente.

2. **Media — Carrera entre comprobación inicial de Echo, armado y Trigger (AD-3).** Se exige rechazar Echo inicialmente alto y aceptar parejas dentro del ciclo, pero no se fija el instante de armado ni se excluyen flancos anteriores al inicio temporal del disparo. Una implementación arma antes de Trigger y admite una subida ajena en esa ventana; otra arma después y puede perder una subida temprana. Fijar una operación de inicio protegida, su marca temporal y la regla de que solo se aceptan flancos pertenecientes al intervalo iniciado por Trigger; comprobar además que el pulso de 10 µs no impide la captura por mantener una sección crítica innecesariamente larga.

3. **Baja — Superficie del núcleo de captura pendiente (AD-1 a AD-3).** `takeReading` y los tipos compartidos están definidos; `EchoCapture` carece de operaciones y tipos de entrada/salida explícitos para iniciar, registrar flancos y vencer timeout. Si adaptador y núcleo se implementan por separado pueden elegir APIs incompatibles aunque ambos satisfagan las reglas. Documentar un contrato mínimo de estas operaciones, incluida la responsabilidad de sincronización en el adaptador; no hace falta introducir interfaces virtuales.

## Límite de la garantía temporal

Las marcas de una ISR GPIO representan el momento de atención, no necesariamente el instante físico exacto del flanco. Interpretar el límite de 30 000 µs respecto a las marcas capturadas evita prometer exactitud física en el borde. El presupuesto de latencia de ISR queda correctamente pendiente de validación real, pero conviene vincularlo explícitamente a esa interpretación.

No se identificaron conflictos en umbrales, representación de ausencia de novedades, consumo único, fase del parpadeo o separación de dependencias.
