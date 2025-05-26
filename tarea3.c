#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Estructura para representar un ítem del juego */
typedef struct {
    char nombre[100];
    int valor;
    int peso;
} Item;


/* Estructura para representar un nodo/escenario del laberinto */
typedef struct Nodo {
    char descripcion[200];
    int id;
    char nombre[100];
    int arriba;
    int abajo;
    int izquierda;
    int derecha;
    int final;
    List *Items;
    Map *vecinos;

} Nodo;

/* Estructura para representar al jugador */
typedef struct {
    List *Items;
    int tiempo;
    int puntuacion;
    int pesoTotal;
} Jugador;

/* Estructura para el estado del juego (individual) */
typedef struct{
    Nodo *current;
    Jugador *Jugador;
} EstadoJuego;

/* Estructura para el estado del juego multijugador */
typedef struct {
    Jugador *jugador1;
    Jugador *jugador2;
    Nodo *nodo_j1;
    Nodo *nodo_j2;
    int turno_actual;
} EstadoMultijugador;



/* Muestra el menú principal del juego */
void mostrarMenuPrincipal() 
{
    limpiarPantalla();
  puts("========================================");
  puts("      GraphQuest");
  puts("========================================");
  puts("1) Cargar Laberinto");
  puts("2) Iniciar Partida Individual");
  puts("3) Iniciar Partida Multijugador");
  puts("4) Salir");
}


int int_equal(void *a, void *b) {
    if (a == NULL || b == NULL) return 0; // Manejo seguro de punteros nulos
    return (*(int *)a == *(int *)b);
}



/* Carga los escenarios desde un archivo CSV y construye el grafo del laberinto */
void leer_escenarios(Map *grafo) 
{
    FILE *archivo = fopen("data/graphquest.csv", "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return;
    }

    // Leer y descartar la cabecera
    leer_linea_csv(archivo, ',');
    
    char **campos;

    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        // Verificar que haya suficientes campos
        if (campos[0] == NULL || campos[1] == NULL) continue;
        
        /*Se crea nuevo nodo*/
        Nodo *newNodo = malloc(sizeof(Nodo));
        
        // Convertir ID a entero correctamente
        int id = atoi(campos[0]);
        newNodo->id = id;
        strncpy(newNodo->nombre, campos[1], sizeof(newNodo->nombre) - 1);
        newNodo->nombre[sizeof(newNodo->nombre) - 1] = '\0';
        
        if (campos[2] != NULL) {
            strncpy(newNodo->descripcion, campos[2], sizeof(newNodo->descripcion) - 1);
            newNodo->descripcion[sizeof(newNodo->descripcion) - 1] = '\0';
        }

        // Se copian las  direcciones que uno puede tomar estando en la habtiacion en cuestion
        newNodo->arriba = (campos[4] != NULL && strcmp(campos[4], "-1") != 0) ? atoi(campos[4]) : -1;
        newNodo->abajo = (campos[5] != NULL && strcmp(campos[5], "-1") != 0) ? atoi(campos[5]) : -1;
        newNodo->izquierda = (campos[6] != NULL && strcmp(campos[6], "-1") != 0) ? atoi(campos[6]) : -1;
        newNodo->derecha = (campos[7] != NULL && strcmp(campos[7], "-1") != 0) ? atoi(campos[7]) : -1;
        
        if (strcmp(campos[8] , "Si") == 0)
        {
            newNodo->final = 1;
        }
        else{
            newNodo->final = 0;
        }
        
        newNodo->Items = list_create();
        newNodo->vecinos = map_create(int_equal);

        // Procesar itemss
        if (campos[3] != NULL && strlen(campos[3]) > 0) {
            List *itemsRaw = split_string(campos[3], ";");
            char *itemStr;
            while ((itemStr = list_popFront(itemsRaw)) != NULL) {
                List *values = split_string(itemStr, ",");
                if (list_size(values) >= 3) {
                    Item *item = malloc(sizeof(Item));
                    strncpy(item->nombre, list_popFront(values), sizeof(item->nombre) - 1);
                    item->nombre[sizeof(item->nombre) - 1] = '\0';
                    item->valor = atoi(list_popFront(values));
                    item->peso = atoi(list_popFront(values));
                    list_pushBack(newNodo->Items, item);
                }
                list_clean(values);
                free(values);
                free(itemStr);
            }
            list_clean(itemsRaw);
            free(itemsRaw);
        }

        // Guardar nodo en el grafo
        int *key = malloc(sizeof(int));
        *key = id;
        map_insert(grafo, key, newNodo);
    }

MapPair *pair = map_first(grafo);
while (pair != NULL) 
{
    Nodo *actual = (Nodo *)pair->value;

    // Conexión arriba
    if (actual->arriba != -1) 
    {
        int *key = malloc(sizeof(int));  // Reservar memoria para la clave
        *key = actual->arriba;           // Asignar el valor del ID
        MapPair *vecino = (MapPair *)map_search(grafo, key); // Se encuentra un par dento del grafo que posea el mismo key
        
        if (vecino != NULL) {
            char *dir_key = strdup("arriba");
            Nodo * node = vecino->value;
            map_insert(actual->vecinos, dir_key, node); // Se inserta en el mapa de nodos vecinos del nodo
        }
        free(key);  // Liberar la memoria temporal
    }

    // Conexión abajo
    if (actual->abajo != -1) 
    {
        int *key = malloc(sizeof(int));
        *key = actual->abajo;
        MapPair *vecino = (MapPair *)map_search(grafo, key);;
        
        if (vecino != NULL) {
            char *dir_key = strdup("abajo");
            Nodo * node = vecino->value;
            map_insert(actual->vecinos, dir_key, node);
        }
        free(key);
    }

    // Conexión izquierda
    if (actual->izquierda != -1) 
    {
        int *key = malloc(sizeof(int));
        *key = actual->izquierda;
        MapPair *vecino = (MapPair *)map_search(grafo, key);
        
        if (vecino != NULL) {
            char *dir_key = strdup("izquierda");
            Nodo * node = vecino->value;
            map_insert(actual->vecinos, dir_key, node);
        }
        free(key);
    }

    // Conexión derecha
    if (actual->derecha != -1) 
    {
        int *key = malloc(sizeof(int));
        *key = actual->derecha;
        MapPair *vecino = (MapPair *)map_search(grafo, key);
        
        if (vecino != NULL) {
            char *dir_key = strdup("derecha");
            Nodo * node = vecino->value;
            map_insert(actual->vecinos, dir_key, node);
        }
        free(key);
    }

    pair = map_next(grafo); // Se pasa al siguiente par
}
    fclose(archivo); // Se cierra el archivo
}

//Cada vez que se entra a un nuevo escenario o se realiza una acción, se muestra el siguiente menú:
void mostrar_estado_actual(EstadoJuego *est_jug){

    //Se muestra la descripcion del escenario y el inventario del jugador
    printf("\n===== Escenario: %s =====\n", est_jug->current->nombre);
    printf("Descripcion escenario: %s\n", est_jug->current->descripcion);
    printf("\nInventario:\n");
    if (!est_jug || !est_jug->Jugador || !est_jug->Jugador->Items) {
        printf("Error: estado o jugador no inicializado correctamente.\n");
        return;
    }
    if(list_size(est_jug->Jugador->Items) == 0){
        printf("No tienes ningun item en tu inventario.\n");
    } else {
        Item *item = list_first(est_jug->Jugador->Items);
        int i = 1;
        while (item ){
            printf(" Numero %d -- Nombre: %s\n Valor: %d--- Peso: %d\n",i, item->nombre,item->valor, item->peso);
            printf("===============================\n");
            item = list_next(est_jug->Jugador->Items);
            i++;
        } 

    }

    //Se muestra los items disponibles en el escenario
    printf("\n=====Items disponibles en escenario=====\n");
    if(list_size(est_jug->current->Items) == 0){
        printf("No hay items disponibles en este lugar.\n");
    } else {
        Item *item = list_first(est_jug->current->Items);
        int i = 1;
        while (item){
            printf("\nNumero %d -- Nombre: %s\n Valor: %d--- Peso: %d\n",i, item->nombre,item->valor, item->peso);
            printf("===============================\n");
            item = list_next(est_jug->current->Items);
            i++;
        } 

    }

    //Se muestra el estado del Jugador
    printf("\n===== Estado del Jugador =====\n");
    printf("Tiempo restante: %d\n", est_jug->Jugador->tiempo);
    printf("Puntaje acumulado: %d\n", est_jug->Jugador->puntuacion);
    printf("Peso total: %d\n", est_jug->Jugador->pesoTotal);

    MapPair *par = map_first(est_jug->current->vecinos);
    while(par != NULL){
        printf(" - %s\n", (char*)par->key);
        par =map_next(est_jug->current->vecinos);
    }   
}

void recoger_items(EstadoJuego *estado){

    //Se veririfica que haya items en el escenario
    if(list_size(estado->current->Items) == 0){
        printf("No hay items para recoger en escenario.\n");
        return;
    } 
    printf("Items para recoger en escenario: \n");
    int i = 1;
    Item *item = list_first(estado->current->Items);

    //En caso de haber items en el ecenario se muestran
    while (item){
        printf(" N° %d -- Nombre: %s\n Valor: %d--- Peso: %d\n",i, item->nombre,item->valor, item->peso);
        printf("===============================\n");
        item = list_next(estado->current->Items);
        i++;
    }
    
    //El jugador el item que quiere llevarse
    printf("Ingrese numero item recoger: \n");
    int opcion;
    scanf("%d", &opcion);
    getchar();
    if(opcion <= 0 || opcion > list_size(estado->current->Items)){
        printf("Numero invalido.\n");
        return;
    }
    Item *item2 = list_first(estado->current->Items);
    for(int i = 1; i < opcion; i++){
        item2 = list_next(estado->current->Items);
    }

    Item *nuevo_item = malloc(sizeof(Item));
    *nuevo_item = *item2;

    list_pushBack(estado->Jugador->Items, nuevo_item);

    estado->Jugador->pesoTotal += nuevo_item->peso;
    estado->Jugador->puntuacion += nuevo_item->valor;
    estado->Jugador->tiempo -= 1;

    item = list_first(estado->current->Items);
    for(int i = 1; i < opcion; i++){
        item = list_next(estado->current->Items);
    }

    //Se agrega el item a su inventario
    list_popCurrent(estado->current->Items);
    printf("Item %s recogido.\n", item->nombre);
}

void descartar_items(EstadoJuego *estado){

    //Se veridica que haya items para descartar
    if(list_size(estado->Jugador->Items) == 0){
        printf("No hay items para descartar.\n");
        return;
    } 

    //Se muestran los items que lleva el jugador
    printf("Items inventario: \n");
    int i = 1;
    Item *item = list_first(estado->Jugador->Items);
    while (item != NULL){
        printf(" N° %d -- Nombre: %s\n Valor: %d--- Peso: %d\n",i, item->nombre,item->valor, item->peso);
        printf("===============================\n");
        item = list_next(estado->Jugador->Items);
        i++;
    } 

    //Se selecciona el item que quese va a descartar
    printf("Ingrese numero item descartar: \n");
    int opcion;
    scanf("%d", &opcion);
    getchar();
    if(opcion <= 0 || opcion > list_size(estado->Jugador->Items)){
        printf("Item invalido.\n");
        return;
    }
    Item *item2 = list_first(estado->Jugador->Items);
    for(int i = 1; i < opcion; i++){
        item2 = list_next(estado->Jugador->Items);
    }
    estado->Jugador->pesoTotal -= item2->peso;
    estado->Jugador->puntuacion -= item2->valor;
    estado->Jugador->tiempo -= 1;

    //Se elimina el item seleccionado
    char guardar[100];
    strcpy(guardar,item2->nombre);

    list_popCurrent(estado->Jugador->Items);
    free(item2);
    printf("Item %s descartado.\n", guardar);
}

//Funion cuyo objetivo es calcular el tiempo de juego que le queda al usuario
int calcular_tiempo(int peso){
    return (peso + 9 ) / 10;
}



/* Mueve al jugador a otro escenario */
void mover(EstadoJuego *estado)
{
    //Se le muestran al jugador las dirrecciones validas a las cuale puede ir
    Nodo *habitacion = estado->current;
    printf("Direcciones válidas:\n");

    if (habitacion->arriba != -1) printf("arriba\n");
    if (habitacion->abajo != -1) printf("abajo\n");
    if (habitacion->izquierda != -1) printf("izquierda\n");
    if (habitacion->derecha != -1) printf("derecha\n");

    //El jugador ingresa la dirrecion en la que quiere ur
    char direccion[50];
    printf("\nIngrese la dirección deseada: ");
    scanf("%49s", direccion);
    getchar(); // Limpiar buffer

    MapPair* vecino = (MapPair*) map_search(habitacion->vecinos, direccion);

    //Si la dirreccion es invalida se le notifica al jugador
    if (!vecino) {
        printf("Dirección inválida: '%s'\n", direccion);
        MapPair* p = map_first(habitacion->vecinos);
        while (p) {
            printf(" - %s\n", (char*)p->key);
            p = map_next(habitacion->vecinos);
        }
        return;
    }

    Nodo* nodo = (Nodo*) vecino->value;

    //Si la dirrecion es valida se actualiza el estado del jugador
    int tiempo = calcular_tiempo(estado->Jugador->pesoTotal);
    if (estado->Jugador->tiempo < tiempo) {
        printf("No hay suficiente tiempo para moverse.\n");
        return;
    }

    estado->Jugador->tiempo -= tiempo;
    estado->current = nodo;

    //Se muestra a que habitacion se movio el jugador
    printf("Se movió a: %s\n", nodo->nombre);

    if (nodo->final == 1) {
        printf("¡Escenario final!\n");
        printf("Puntaje final: %d\n", estado->Jugador->puntuacion);
        estado->Jugador->tiempo = 0;
    }
}

/* Libera la memoria de una lista de ítems */
void liberar(List *items){
    if (!items) return;

    Item *item = list_first(items);
    while(item != NULL){
        free(item);
        item = list_next(items);
    }
    list_clean(items);
    free(items);
}



/* Reinicia el estado del juego */
EstadoJuego *reiniciar(Nodo *inicio, EstadoJuego *currentPlay) {

    //Se libera y borra todo el progreso realizado hasta el momento
    liberar(currentPlay->Jugador->Items);
    free(currentPlay->Jugador);
    free(currentPlay);
    
    // Crear nuevo estado
    EstadoJuego *estado = malloc(sizeof(EstadoJuego));
    Jugador *player = malloc(sizeof(Jugador));
    player->Items = list_create();
    player->tiempo = 10;
    player->puntuacion = 0;
    player->pesoTotal = 0;

    estado->Jugador = player;
    estado->current = inicio;
    return estado;
}



/* Inicia una partida individual */
void iniciarPartida(Map *grafo, Nodo *inicio) {

    // Crear jugador y estado inicial
    Jugador *player = malloc(sizeof(Jugador));
    player->Items = list_create();
    player->tiempo = 10;
    player->puntuacion = 0;
    player->pesoTotal = 0;

    EstadoJuego *estado = malloc(sizeof(EstadoJuego));
    estado->current = inicio;
    estado->Jugador = player;
    int opcion;
    int salir = 0;

    // Bucle principal del juego
    while(!salir && player->tiempo > 0){
        mostrar_estado_actual(estado);

        //Se muestran las opciones que puede realiazr el jugador
        printf("\nOpciones:\n1. Recoger Item\n2. Descartar Item\n3. Avanzar\n4. Reiniciar\n5. Salir\n");
        scanf("%d",&opcion);
        getchar();

        //El jugador debe de eleigir una entre las 5 opciones
        switch (opcion) {
            case 1:
                 recoger_items(estado);
                 break;
            case 2:
                descartar_items(estado);
                break;
            case 3: {
                mover(estado);
                if(estado->current->final){
                    printf("Has llegado escenario final.\n");
                    salir = 1;
                }
                break;
            }
            case 4:
                estado = reiniciar(inicio,estado);
                player = estado->Jugador;
                break;
            case 5:
                salir = 1;
                return;
            default:
                printf("Opcion no valida.\n");
        }
    }
    if(player->tiempo <= 0){
        printf("Se acabo el tiempo.\n");
    }
    //Si se agota el tiempo se le notifica al jugador
    printf("Fin del juego. Tiempo agotado o final alcanzado.\n");
    liberar(player->Items);
    free(player);
    free(estado);
}






void iniciarPartidaMultijador(Map *grafo, Nodo *inicio){
    //reserva memoria para jugar en multijuador
    EstadoMultijugador *estado = malloc(sizeof(EstadoMultijugador));
    //reserva memoria y configura al jugador 1
    estado->jugador1 = malloc(sizeof(Jugador));
    estado->jugador1->Items = list_create(); //incializa la lista de items
    estado->jugador1->tiempo = 10;     //tiempo
    estado->jugador1->puntuacion = 0; //puntuacion
    estado->jugador1->pesoTotal = 0; //peso total
    //reserva memoria y configura jugador 2
    estado->jugador2 = malloc(sizeof(Jugador));
    estado->jugador2->Items = list_create();
    estado->jugador2->tiempo = 10;
    estado->jugador2->puntuacion = 0;
    estado->jugador2->pesoTotal = 0;
    //establece nodo incial para ambos jugadores
    estado->nodo_j1 = inicio;
    estado->nodo_j2 = inicio;
    //controlar la salida del juego empieza con 0
    estado->turno_actual = 0;

    int salir = 0; //salir del bucle (variable)

    while(!salir){
        Jugador *jug;
        Nodo *nodo;
        //que jugador esta en turno y asigna punteros
        if(estado->turno_actual == 0) {
            printf("\n=== Turno del jugador 1 ===\n");
            jug = estado->jugador1;
            nodo = estado->nodo_j1;
        } else {
            printf("\n=== Turno del jugador 2 ===\n");
            jug = estado->jugador2;
            nodo = estado->nodo_j2;
        }
        //un estado temporal con jugador y su nodo actual
        EstadoJuego temp;
        temp.Jugador = jug;
        temp.current = nodo;

        mostrar_estado_actual(&temp); //muestar estado actual

        int acciones = 2; //acciones que puede hacer
        while(acciones > 0 && jug->tiempo > 0){
            printf("\nOpciones:\n1. Recoger Item\n2. Descartar Item\n3. Avanzar\n4. Terminar Turno\n5. Salir del Juego\n\n");
            int opcion;
            scanf("%d",&opcion);
            getchar();
            //llama a las funciones 
            switch (opcion){
                case  1:
                    recoger_items(&temp);
                    acciones--;
                    break;
                case 2:
                    descartar_items(&temp);
                    acciones--;
                    break;
                case 3:
                    mover(&temp);
                    if (estado->turno_actual == 0){
                        estado->nodo_j1 = temp.current;
                    }else {
                        estado->nodo_j2 = temp.current;
                    }
                    acciones--;
                    if(temp.current->final){
                        printf("¡Jugador %d llegó al escenario final!\n", estado->turno_actual + 1);
                        jug->tiempo = 0;
                        acciones = 0;
                    }
                    break;
                case 4:
                    acciones = 0;
                    break;
                case 5:
                    printf("Juego terminado por el jugador.\n");
                    salir = 1;
                    acciones = 0;
                    break;
                default:
                    printf("Opcion no valida.\n");
            }
        }
        //si ambos estan sin tiempo se dice 
        if( estado->jugador1->tiempo <= 0 && estado->jugador2->tiempo <= 0){
            printf("\nFin del juego. Ambos jugadores terminaron.\n");
            printf("Jugador 1 - Puntaje: %d\n", estado->jugador1->puntuacion);
            printf("Jugador 2 - Puntaje: %d\n", estado->jugador2->puntuacion);
            salir = 1;
        }
        estado->turno_actual = (estado->turno_actual + 1) % 2;
    }
    //limpia la memoria al finalizar 
    liberar(estado->jugador1->Items);
    liberar(estado->jugador2->Items);
    free(estado->jugador1);
    free(estado->jugador2);
    free(estado);
}







iint main() {
    //Se inicializan las tdas para el correcto funcionamiento del programa
    Map *grafo = map_create(int_equal);
    Nodo *esc = NULL;
    int opcion;
    
    do {
        //Se muestra el menu principal y guarda la opcion que escoge el usuario
        mostrarMenuPrincipal();
        scanf("%d", &opcion);
        getchar();

        switch (opcion) {

            //Si se escoge la opcion uno se carga el laberinto
            case 1:
                leer_escenarios(grafo);
                MapPair *p = map_first(grafo);
                if (p != NULL) {
                    esc = (Nodo *)p->value;
                    printf("Laberinto cargado correctamente. Nodo inicio: %s\n", esc->nombre);
                } else {
                    printf("Laberinto cargado pero sin nodos.\n");
                }
                break;
        //Se inicia una partida individual
          case 2:
          if (esc == NULL) {
                    printf("Primero debes cargar el laberinto (Opción 1).\n");
                } else {
                    iniciarPartida(grafo, esc);
                }
                break;
            //Se inicia un partida multijugador      
            case 3:
                if (esc == NULL) {
                    printf("Debes cargar el laberinto primero (Opción 1).\n");
                } else {
                    iniciarPartidaMultijador(grafo, esc);
                }
                break;
            //Se sale del juego y de la terminal
            case 4:
                printf("Saliendo del juego...\n");
                break;
            //Cualquier otra opcion no valida es descatada y aparece un mensjae
            default:
                printf("Opción no válida.\n");
        }

        if (opcion != 4) {
            presioneTeclaParaContinuar();
        }
        
    } while (opcion != 4);

    return 0;
}



//gcc tarea3.c tdas/list.c tdas/map.c tdas/extra.c -o graphquest
//./graphquest
