# Reconciliación de entradas y revisión por rúbrica

Fecha: 2026-09-07. Revisión independiente, sin modificar la arquitectura.

**Veredicto:** coherente con el PRD y el informe; apta para guiar implementación simulada, con una mejora menor de cobertura de pruebas. No habilita el montaje físico ni acredita resultados ejecutados.

## Entradas reconciliadas

- `prds/prd-indicador-distancia-2026-09-07/prd.md`: RF-01 a RF-07, RNF-01 a RNF-06, S-01 a S-04 y P-01 a P-03.
- `prds/prd-indicador-distancia-2026-09-07/informe-tecnico.md`: diseño, núcleo de referencia y PU-01 a PU-09.
- Artefacto revisado: `ARCHITECTURE-SPINE.md` de esta carpeta de arquitectura.

| Restricción o contrato de entrada | Evidencia en arquitectura | Evaluación |
|---|---|---|
| Componentes limitados, práctica sin conectividad ni ampliaciones | AD-6 | Conservado |
| Umbrales 10/30 cm y rango provisional inclusivo 2–400 cm | AD-4 | Conservado |
| Error prioritario, 2 Hz simultáneos, 250 ms por fase | AD-4 | Conservado |
| Arranque, recuperación y conservación de fase ante inválidas sucesivas | AD-4 | Conservado |
| OO, inglés en código y español en documentación; comentarios mínimos | AD-1 | Conservado |
| Núcleo sin Arduino, entradas y reloj controlables | Paradigma, AD-1, AD-5 | Conservado |
| Adquisición periódica 100 ms y timeout 30 ms sin bloquear parpadeo | AD-3, AD-4 | Concretado |
| Diferenciar ausencia de novedades y resultado inválido | AD-2 | Concretado mediante consumo único |
| Echo pendiente sin adaptación autorizada; placa y LEDs por verificar | AD-6, pendientes | Conservado |
| Límite físico de observabilidad del sensor | P-03 y AD-3 | Conservado |
| No atribuir ejecución a código o pruebas propuestas | Apertura, AD-5, cierre | Conservado |

## Hallazgos

No hay hallazgos críticos ni altos.

**R-01 — Bajo: completar los casos explícitos de AD-5.** El informe define PU-08 para saltos de varias medias fases y PU-09 para convertir un pulso conocido. AD-4 conserva el contrato de saltos y AD-2 conserva la conversión, pero AD-5 no pide expresamente comprobar ambos resultados. Una implementación podría cubrir orden de flancos y marcas adyacentes sin verificar la paridad de un salto largo o la conversión compartida del adaptador. **Acción: autofix documental.** Añadir a AD-5 saltos de varias medias fases, pares e impares, y un pulso conocido cuya conversión produzca una distancia esperada mediante el mismo código utilizado por firmware.

## Rúbrica de calidad

| Criterio | Resultado |
|---|---|
| Fija puntos reales de divergencia al nivel de clases y adaptadores | Cumple: unidades, lectura/evento, dueño del estado, adquisición y fase |
| Reglas exigibles que previenen su divergencia declarada | Cumple: contratos y límites verificables |
| Decisiones diferidas no permiten interfaces incompatibles | Cumple: versiones de herramientas y detalles internos no alteran contratos; montaje queda bloqueado |
| Tecnología identificada y verificada | Evidencia local y fuentes primarias consignadas; Unity y entorno anfitrión se reconocen pendientes, sin afirmar ejecución |
| Respeta repositorio previo | Conserva Arduino, placa y PlatformIO; no afirma haber sustituido la plantilla |
| Cobertura de capacidades de las entradas | Cumple; mejora menor R-01 sobre plan de prueba |
| Invariantes de un padre | No se identifica arquitectura padre; restricciones del PRD conservadas |
| Dimensiones operativas y ambientales | Cumple: anfitrión para unitarias, única placa para ejecución, carga local, reinicio, sin servicios remotos |
| Escala apropiada | Seis decisiones para el alcance acotado; captura independiente se justifica por temporización y pruebas |

Las exclusiones detalladas del PRD siguen vinculantes por referencia. No se interpreta el documento como autorización para filtrar lecturas, introducir histéresis o cambiar umbrales. Los bloqueos eléctricos y de aceptación ya están identificados correctamente y no requieren ampliar el alcance de esta revisión.
