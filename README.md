Se implemta un juego llamado GraphQuest. Este juego consiste en recorrer un laberinto representado mediante un grafo, en el que cada nodo es un escenario con decisiones posibles y elementos coleccionables. El jugador comienza en un escenario inicial (Entrada principal) y debe avanzar hasta un escenario final de salida, tomando decisiones estratégicas sobre el camino a seguir y qué ítems recoger.
Programa:
El juego GraphQuest se ha realizado en lenguaje C y se puede ejecutar en Visual Studio Code con extensión C/C++. Para trabajar con sistema se deberán seguir los siguientes pasos:
Requisitos previos:
•   Haber descargado Visual Studio Code.
•   Tener el compilador C/C++, haber descargado el MinGW para Windows o el entorno WSL.
•   Tener la extensión C/C++ en Visual Studio Code.
Compilar y ejecutar
•   Se debe descargar esta carpeta y se descomprime el archivo .zip.
•   En Visual Studio Code se va a, file y se escoge open folder, luego selecciona la carpeta descomprimida.
•   Se debe abrir el archivo tarea3.c para visualizar código. 
•   Se abre su terminal y se debe compilar el programa con siguiente comando:
                  gcc tarea3.c tdas/list.c tdas/map.c tdas/extra.c -o graphquest

•   Ya compilado, para ejecutar el código se debe escribir en terminal:
                  ./graphquest


ejemplo mostrando como interactuar con la aplicación.
El programa comienza preguntando que opción desea realizar:
========================================
      GraphQuest
========================================
1) Cargar Laberinto
2) Iniciar Partida Individual
3) Iniciar Partida Multijugador
4) Salir

1.Se debe seleccionar opción para cagar el laberinto :
•	Laberinto cargado correctamente. Nodo inicio: Entrada principal
•	Presione una tecla para continuar...
2. Si se desea jugar de forma individual se debe escoger la opción 2, en donde saldrá toda la información disponible:
===== Escenario: Entrada principal =====
•	Descripcion escenario: Una puerta rechinante abre paso a esta mansion olvidada por los dioses y los conserjes. El aire huele a humedad y a misterios sin resolver.
•	Inventario.
•	No hay items disponibles :(
•	Items disponibles en escenario:
•	Inventario vacio :0
•	===== Estado del Jugador =====
•	Tiempo restante: 10
•	Puntaje acumulado: 0
•	Peso total: 0
-	abajo
•	Opciones:
•	Recoger Item
•	Descartar Item
•	Avanzar
•	Reiniciar
•	Salir
2.1 luego aparecerán opciones en donde se podrá recoger los ítems, descartar ítems, avanzar, reiniciar o salir.
2.2 en el caso que se escoja avanzar para recorrer los escenarios se mostrara la información de los otros escenarios.
3. Si se desea jugar de forma multijugador, aparecera toda la información de los jugadores:
3
=== Turno del jugador 2 ===

•	===== Escenario: Entrada principal =====
•	Descripcion escenario: Una puerta rechinante abre paso a esta mansion olvidada por los dioses y los conserjes. El aire huele a humedad y a misterios sin resolver.
•	Inventario.
•	No hay items disponibles :(
•	Items disponibles en escenario:
•	Inventario vacio :0

•	===== Estado del Jugador =====
•	Tiempo restante: 10
•	Puntaje acumulado: 0
•	Peso total: 0
-	abajo
•	Opciones:
•	Recoger Item
•	Descartar Item
•	Avanzar
•	Terminar Turno 
4. se sale del juego.
Funcionalidades
Funcionando correctamente:
mostrar_estado_actual: muestra el nombre y la descripción del escenario actual, los items en inventario, items en el escenario. También muestra el tiempo restante, puntaje acumulado y peso total del jugador, además de las direcciones válidas para moverse. Se asegura de que el estado y el jugador estén correctamente inicializados antes de imprimir esta información.
recoger_items: permite al jugador ver los items disponibles en el escenario y elegir uno para agregarlo a su inventario, actualizando el peso total, puntaje y restando tiempo. 
descartar_items: permite al jugador eliminar un item de su inventario, ajustando su peso total, puntaje y restando tiempo.
calcular_tiempo: calcula el tiempo requerido para moverse basado en el peso total que lleva el jugador. 
Reiniciar: limpia el estado actual para volver a jugar.
iniciarPartida: maneja el flujo del juego en modo individual mostrando el estado actual, y ofreciendo opciones para recoger item, descartar item, avanzar, reiniciar o salir del juego. 
iniciarPartidaMultijador: crea dos jugadores con estados separados y alterna turnos entre ellos mostrando opciones similares pero también una opción para salir del juego.

Problemas conocidos:
La función leer_escenarios tiene mucho contenido, lo que podría provocar errores cuando se quiere almacenar muchos datos, además no almacena correctamente las direcciones para mejorar en avanzar.
La función mover no esta implementada correctamente, ya que no tiene almacenada correctamente las direcciones, cuando se elige una dirección aparece como no valida.
El programa busca si existe en la lista, pero esta esta vacía o no coincide con los que se muestra. Aunque parece que "abajo" debería funcionar, el programa no lo reconoce como válido y dice que la dirección es inválida.
El problema es que el juego muestra unas direcciones que en realidad no tiene guardadas para moverse, por eso no puedes avanzar en esas rutas, aunque parezca que sí.
A mejorar:
La interfaz del programa puede ser confusa con muchos prints
Las funciones leer_escenarios y mover.
Contribuciones:
Benjamín Vargas:
•	Implementar la base de datos al programa.
•	Diseñar todas las funciones del código.
•	Verificar posibles errores de compilación.
•	Se encargo de comentar el código.
•	Autoevaluación: 3/3 (Aporte excelente).
•	Coevaluación : 3/3 (Aporte excelente)

Francisca Zamora:
•	Diseñar todas las funciones del código.
•	Modificar la salida e interfaz del programa.
•	Verificar posibles errores de compilación.
•	Diseño y redacción del README.
•	Autoevaluación: 3/3 (Aporte excelente).
•	Coevaluación : 3/3 (Aporte excelente)
