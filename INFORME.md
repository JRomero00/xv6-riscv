# Informe Tarea 0 Grupo A

Se ejecuto xv6 y se tuvo esta captura:
![Captura de ejecución de xv6](Comando%20T0.jpg)

## 1. Pasos seguidos para instalar xv6

1. **Clonar el repositorio**
   ```bash
   git clone https://github.com/JRomero00/xv6-riscv.git
   cd xv6-riscv

2. **Instalar dependencias**
sudo apt update
sudo apt install build-essential gdb-multiarch qemu-system-misc

3. **Compilar xv6**
make

4. **Ejecutar xv6 en QEMU**
make qemu


2. Problemas encontrados y soluciones
Problema: No podía acceder a mis carpetas de usuario desde la terminal de Linux porque estaba dentro de la consola de xv6. Solución: Salir de QEMU con Ctrl + a y luego x para volver al shell normal.

Problema: No podía hacer git push porque GitHub no acepta contraseñas. Solución: Crear un Personal Access Token y usarlo como contraseña, además de apuntar el remoto a mi fork.


