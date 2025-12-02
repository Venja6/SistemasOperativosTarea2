Compilación (Necesita main.c + monitor_barrera.c + monitor_barrera.h):

gcc main.c monitor_barrera.c -o barrera_reutilizable -pthread


El programa pedirá condiciones iniciales para la cantidad de barreras y etapas.