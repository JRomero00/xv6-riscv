# Tarea 2: Lottery Scheduling en XV6

**Grupo:** Grupo A  
**Integrantes:** Tomas Hernandez, Eugenio Gigogne, Jorge Romero

## Descripción General

Esta tarea implementa un sistema de Lottery Scheduling (planificación por lotería) en XV6, reemplazando el planificador Round-Robin original. En este esquema, cada proceso recibe una fracción de tiempo de CPU proporcional a la cantidad de "tickets" que posee.

## Funcionamiento y Lógica de la Implementación

### Principio de Lottery Scheduling

El Lottery Scheduling funciona como una lotería donde:
- Cada proceso posee un número de "tickets"
- En cada ronda de planificación, se sortea un ticket ganador
- El proceso que posee ese ticket es ejecutado
- Procesos con más tickets tienen mayor probabilidad de ser elegidos

### Algoritmo Implementado

1. **Cálculo del total de tickets:** Se suman los tickets de todos los procesos en estado RUNNABLE
2. **Generación del número ganador:** Se genera un número aleatorio entre 1 y el total de tickets
3. **Selección del proceso:** Se recorren los procesos acumulando tickets hasta encontrar al ganador
4. **Ejecución:** El proceso seleccionado cambia a estado RUNNING y se ejecuta

### Ejemplo de Funcionamiento

Si tenemos 3 procesos:
- Proceso A: 100 tickets
- Proceso B: 200 tickets  
- Proceso C: 300 tickets

Total = 600 tickets

Si el número aleatorio es 350:
- Proceso A: acumulado = 100 (no ganador)
- Proceso B: acumulado = 300 (no ganador)
- Proceso C: acumulado = 600 (¡ganador!)

El Proceso C será ejecutado porque su rango acumulado (301-600) contiene al número 350.

## Modificaciones Realizadas

### 1. Archivos del Kernel

#### kernel/proc.h
- **Agregado:** Campos tickets (int) y run_slices (int) a struct proc
- **Propósito:** Almacenar la cantidad de tickets y llevar estadísticas de ejecución

#### kernel/proc.c
- **Función allocproc():** Inicialización de tickets = 100 y run_slices = 0
- **Función scheduler():** Implementación completa del algoritmo de lottery scheduling
  - Cálculo de tickets totales
  - Generación de número aleatorio
  - Selección del proceso ganador
  - Incremento del contador run_slices
- **Función random():** Generador de números pseudoaleatorios usando un LCG (Linear Congruential Generator)

#### kernel/sysproc.c
- **Función sys_settickets():** Implementación de la syscall que permite modificar los tickets de un proceso
- **Validación:** Si n < 1, automáticamente se asigna 1 ticket

### 2. Archivos de Sistema de Llamadas

#### kernel/syscall.h
- **Agregado:** #define SYS_settickets 22

#### kernel/syscall.c
- **Agregado:** Prototipo extern uint64 sys_settickets(void);
- **Agregado:** Entrada en array syscalls[] para mapear la syscall

### 3. Archivos de Espacio de Usuario

#### user/usys.pl
- **Agregado:** entry("settickets"); para generar el stub de la syscall

#### user/user.h
- **Agregado:** Prototipo int settickets(int); para que los programas de usuario puedan usar la syscall

### 4. Programa de Prueba

#### user/demo.c
- Crea 10 procesos hijos usando fork()
- Asigna tickets de forma incremental: 50, 100, 150, ..., 500
- Cada proceso realiza trabajo intensivo de CPU
- Imprime información sobre tickets asignados y finalización

#### Makefile
- **Agregado:** $U/_demo\ a la lista de programas de usuario (UPROGS)

## Dificultades Encontradas y Soluciones

### 1. Generación de Números Aleatorios
**Problema:** XV6 no incluye una biblioteca estándar con funciones aleatorias.

**Solución:** Implementamos un generador LCG (Linear Congruential Generator) simple pero efectivo. Utiliza la fórmula: seed = seed * 1103515245 + 12345, que es ampliamente usada y probada.

### 2. Condición de Carrera en el Scheduler
**Problema:** Múltiples accesos concurrentes a la estructura proc podían causar inconsistencias.

**Solución:** Uso correcto de acquire() y release() para proteger las secciones críticas. Cada acceso a p->state y p->tickets se realiza dentro de un lock.

### 3. Procesos sin Tickets
**Problema:** Si todos los procesos tienen 0 tickets, el scheduler se bloquea.

**Solución:** 
- Inicialización por defecto a 100 tickets en allocproc()
- Validación en sys_settickets() que asegura mínimo 1 ticket
- Verificación de total_tickets == 0 en el scheduler para continuar si no hay procesos

### 4. Seed Estático del Generador Aleatorio
**Problema:** El generador siempre inicia con la misma semilla, produciendo secuencias predecibles.

**Solución Actual:** Semilla estática funcional para pruebas básicas.

**Mejora Futura:** Inicializar la semilla con valores del hardware (ticks del timer, dirección de memoria, etc.) para mayor aleatoriedad.

## Posibles Problemas del Lottery Scheduling

### 1. **Starvation (Inanición)**
**Descripción:** Un proceso con muy pocos tickets puede no ser seleccionado durante largos períodos, especialmente si hay muchos procesos con más tickets.

**Ejemplo:** Un proceso con 1 ticket compitiendo contra 99 procesos con 100 tickets cada uno tiene solo 0.01% de probabilidad de ejecución.

**Mitigación:** Implementar un sistema de "aging" que incremente gradualmente los tickets de procesos que han esperado mucho tiempo.

### 2. **Falta de Garantías de Tiempo Real**
**Descripción:** No hay garantías determinísticas sobre cuándo se ejecutará un proceso. Es probabilístico, no determinístico.

**Impacto:** No es adecuado para sistemas de tiempo real donde se requieren garantías estrictas de latencia.

### 3. **Complejidad del Algoritmo (O(n))**
**Descripción:** En cada iteración del scheduler, se deben recorrer todos los procesos dos veces:
- Una vez para calcular el total de tickets
- Otra vez para encontrar el ganador

**Impacto:** Con muchos procesos, el overhead del scheduler aumenta linealmente.

**Optimización Posible:** Mantener un contador global de tickets y actualizarlo solo cuando cambia el estado de un proceso.

### 4. **Calidad del Generador Aleatorio**
**Descripción:** Un generador pseudoaleatorio simple puede tener patrones predecibles o sesgos estadísticos.

**Impacto:** La distribución de CPU puede no ser perfectamente proporcional a los tickets, especialmente en períodos cortos.

**Mejora:** Usar generadores más sofisticados como Mersenne Twister o obtener entropía del hardware.

### 5. **Manipulación de Tickets**
**Descripción:** Procesos maliciosos podrían intentar asignarse muchos tickets para monopolizar la CPU.

**Impacto:** Problema de seguridad y fairness.

**Mitigación:** 
- Implementar límites máximos de tickets por proceso
- Requerir privilegios especiales para modificar tickets
- Implementar un sistema de "moneda" donde los tickets se consumen o tienen un costo

### 6. **Dificultad para Predecir Rendimiento**
**Descripción:** A diferencia de Round-Robin donde cada proceso recibe un quantum fijo, aquí el tiempo de CPU es probabilístico.

**Impacto:** Dificulta la planificación de recursos y el debugging de problemas de rendimiento.

### 7. **Fragmentación de CPU en Cargas Asimétricas**
**Descripción:** Si un proceso con muchos tickets termina abruptamente, su "cuota" de CPU no se redistribuye inmediatamente de forma eficiente.

**Impacto:** Puede haber períodos donde la distribución de CPU no refleja adecuadamente las prioridades actuales.

## Pruebas Realizadas

### Ejecución de demo
```bash
$ demo
Demo Lottery Scheduling - Iniciando 10 procesos
Proceso 3: tickets=50
Proceso 4: tickets=100
Proceso 5: tickets=150
...
Proceso 12: tickets=500
Proceso 3 finalizado
Proceso 4 finalizado
...
Demo completado
```

### Verificación de Proporcionalidad
Para verificar que los procesos con más tickets reciben más CPU, se puede:
1. Agregar prints en `scheduler()` mostrando `p->run_slices`
2. Comparar los valores finales de `run_slices` entre procesos
3. Los procesos con más tickets deberían tener valores de `run_slices` proporcionalmente mayores

## Compilación y Ejecución

```bash
# Compilar XV6
make qemu

# Dentro de XV6, ejecutar el programa de prueba
$ demo

# Para salir de QEMU
Ctrl+A, luego X
```

## Conclusiones

La implementación de Lottery Scheduling en XV6 fue exitosa y demuestra un método alternativo de planificación basado en probabilidades. Aunque tiene limitaciones (starvation potencial, falta de garantías de tiempo real), ofrece una forma simple y efectiva de asignar prioridades a procesos mediante un mecanismo intuitivo de "tickets".

El sistema es flexible y permite ajustar dinámicamente las prioridades de los procesos, lo que podría ser útil en sistemas donde la importancia relativa de las tareas cambia con el tiempo.

## Referencias

- XV6: A simple, Unix-like teaching operating system
- "Lottery Scheduling: Flexible Proportional-Share Resource Management" - Waldspurger y Weihl (1994)
- Documentación de XV6-RISCV: https://github.com/mit-pdos/xv6-riscv
