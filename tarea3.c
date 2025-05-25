#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    char nombre[100];
    int valor;
    int peso;
} Item;

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
    Mapvecinos;

} Nodo;


typedef struct {
    List *Items;
    int tiempo;
    int puntuacion;
    int pesoTotal;
} Jugador;

typedef struct{
    Nodo *current;
    Jugador *Jugador;
} EstadoJuego;

typedef struct {
    Jugador *jugador1;
    Jugador *jugador2;
    Nodo *nodo_j1;
    Nodo *nodo_j2;
    int turno_actual;
} EstadoMultijugador;

void mostrarMenuPrincipal() {
  puts("========================================");
  puts("      GraphQuest");
  puts("========================================");
  puts("1) Cargar Laberinto");
  puts("2) Iniciar Partida Individual");
  puts("3) Iniciar Partida Multijugador");
  puts("4) Salir");
}

int int_equal(void *a, void *b) {
    return (*(int *)a == *(int *)b);
    //return strcmp((char *)a, (char *)b) == 0;
}

/*void leer_escenarios(Map *grafo) {
  // Intenta abrir el archivo CSV que contiene datos de películas
  FILE *archivo = fopen("data/graphquest.csv", "r");
  if (archivo == NULL) {
    perror("Error al abrir el archivo"); // Informa si el archivo no puede abrirse
    return;
  }
  leer_linea_csv(archivo, ',');
  char **campos;
  Map *referencias = map_create(int_equal);

  while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
    Nodo *newNodo = malloc(sizeof(Nodo));
    newNodo->id = atoi(campos[0]);
    strcpy(newNodo->nombre, campos[1]);
    strcpy(newNodo->descripcion, campos[2]);
    newNodo->Items = list_create();
    newNodo->vecinos = map_create(int_equal);
    newNodo->final = atoi(campos[5]);


    List *itemsRaw = split_string(campos[3], ";");
    for(char *itemStr = list_first(itemsRaw); itemStr != NULL; 
        itemStr = list_next(itemsRaw)){

        List *values = split_string(itemStr, ",");
        Item *new_item = malloc(sizeof(Item));

        strcpy(new_item->nombre, list_first(values));
        new_item->valor = atoi(list_next(values));
        new_item->peso = atoi(list_next(values));

        list_pushBack(newNodo->Items,new_item);
        list_clean(values);
        free(values);
    }
    list_clean(itemsRaw);
    free(itemsRaw);

    
    int *key = malloc(sizeof(int));
    *key = newNodo->id;
    map_insert(grafo,key,newNodo);
    map_insert(referencias,key,strdup(campos[4]));
  }

  MapPair *p = map_first(grafo);
  while(p){
    Nodo *n = p->value;
    int id = *(int *)p->key;
    char *conns = (char *)map_search(referencias, &id);
    if (!conns) {
        p = map_next(grafo);
        continue;
    }
    List *dirList = split_string(conns, ";");

    for (char *d = list_first(dirList); d; d = list_next(dirList)) {
        char *dir = strtok(d, ":");
        int dest = atoi(strtok(NULL, ":"));
        Nodo *vecino = (Nodo *)map_search(grafo, &dest);
        if (vecino && dir)
            map_insert(n->vecinos, strdup(dir), vecino);
    }
    list_clean(dirList);
    free(dirList);
    p = map_next(grafo);
  }

  map_clean(referencias);
  free(referencias);
  fclose(archivo); // Cierra el archivo después de leer todas las líneas
}*/
/*void leer_escenarios(Map *grafo) {
    FILE *archivo = fopen("data/graphquest.csv", "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return;
    }

    // Leer y descartar la cabecera
    leer_linea_csv(archivo, ',');
    
    char **campos;
    Map *referencias = map_create(int_equal);

    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        // Verificar que haya suficientes campos
        if (campos[0] == NULL || campos[1] == NULL) continue;

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

        newNodo->final = (campos[5] != NULL) ? atoi(campos[5]) : 0;
        newNodo->Items = list_create();
        newNodo->vecinos = map_create(int_equal);

        // Procesar items
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

        // Guardar conexiones para procesar después
        int *key = malloc(sizeof(int));
        *key = id;
        char *conexiones = (campos[4] != NULL) ? strdup(campos[4]) : strdup("");
        map_insert(referencias, key, conexiones);

        // Insertar nodo en el grafo
        int *graphKey = malloc(sizeof(int));
        *graphKey = id;
        map_insert(grafo, graphKey, newNodo);
    }

    // Procesar conexiones
    MapPair *pair = map_first(grafo);
    while (pair != NULL) {
        int id = *(int *)pair->key;
        Nodo *nodo = pair->value;
        
        char *conexionesStr = (char *)map_search(referencias, &id);
        if (conexionesStr != NULL && strlen(conexionesStr) > 0) {
            List *conexiones = split_string(conexionesStr, ";");
            char *conexion;
            while ((conexion = list_popFront(conexiones)) != NULL) {
                char *dir = strtok(conexion, ":");
                char *idDestStr = strtok(NULL, ":");
                if (dir && idDestStr) {
                    int idDest = atoi(idDestStr);
                    int *keyDest = malloc(sizeof(int));
                    *keyDest = idDest;
                    Nodo *destino = (Nodo *)map_search(grafo, keyDest);
                    free(keyDest);
                    
                    if (destino) {
                        map_insert(nodo->vecinos, strdup(dir), destino);
                    }
                }
                free(conexion);
            }
            list_clean(conexiones);
            free(conexiones);
        }
        pair = map_next(grafo);
    }

    // Limpieza
    MapPair *refPair;
    while ((refPair = map_first(referencias)) != NULL) {
        free(refPair->key);
        free(refPair->value);
        map_remove(referencias, refPair->key);
    }
    map_clean(referencias);
    fclose(archivo);
}*/
void leer_escenarios(Map *grafo) {
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

        newNodo->final = (campos[5] != NULL) ? atoi(campos[5]) : 0;
        newNodo->Items = list_create();
        newNodo->vecinos = map_create(int_equal);

        // Procesar items
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

        // Insertar nodo en el grafo
        int *graphKey = malloc(sizeof(int));
        *graphKey = id;
        map_insert(grafo, graphKey, newNodo);
    }
    
    MapPair *pair = map_first(grafo);
    while (pair != NULL) 
    {
        Nodo *actual = (Nodo *)pair->value;

        // Conexi贸n arriba
        if (actual->arriba != -1) 
        {
            int *key = malloc(sizeof(int));
            *key = actual->arriba;
            Nodo *vecino = (Nodo *)map_search(grafo, key);
            free(key);
    
            if (vecino != NULL) {
                map_insert(actual->vecinos, "arriba", vecino);
            }
        }
    
        // Conexi贸n abajo (independiente de arriba)
        if (actual->abajo != -1) 
        {
            int *key = malloc(sizeof(int));
            *key = actual->abajo;
            Nodo *vecino = (Nodo *)map_search(grafo, key);
            free(key);
    
            if (vecino != NULL) {
                map_insert(actual->vecinos, "abajo", vecino);
            }
        }

        // Conexi贸n izquierda
        if (actual->izquierda != -1) 
        {
            int *key = malloc(sizeof(int));
            *key = actual->izquierda;
            Nodo *vecino = (Nodo *)map_search(grafo, key);
            free(key);
    
            if (vecino != NULL) {
                map_insert(actual->vecinos, "izquierda", vecino);
            }
        }

        // Conexi贸n derecha
        if (actual->derecha != -1) 
        {
            int *key = malloc(sizeof(int));
            *key = actual->derecha;
            Nodo *vecino = (Nodo *)map_search(grafo, key);
            free(key);
    
            if (vecino != NULL) {
                map_insert(actual->vecinos, "derecha", vecino);
            }
        }

        pair = map_next(grafo);  // No olvides avanzar al siguiente nodo
    }

    fclose(archivo);
}

//Cada vez que se entra a un nuevo escenario o se realiza una acción, se muestra el siguiente menú:
void mostrar_estado_actual(EstadoJuego *est_jug){
    printf("\n===== Escenario: %s =====\n", est_jug->current->nombre);
    printf("Descripcion escenario: %s\n", est_jug->current->descripcion);
    printf("Inventario.\n");
    if (!est_jug || !est_jug->Jugador || !est_jug->Jugador->Items) {
        printf("Error: estado o jugador no inicializado correctamente.\n");
        return;
    }
    if(list_size(est_jug->Jugador->Items) == 0){
        printf("No hay items disponibles :(\n");
    } else {
        Item *item = list_first(est_jug->Jugador->Items);
        int i = 1;
        while (item ){
            printf(" N° %d -- Nombre: %s\n Valor: %d--- Peso: %d\n",i, item->nombre,item->valor, item->peso);
            printf("===============================\n");
            item = list_next(est_jug->Jugador->Items);
            i++;
        } 

    }

    printf("Items disponibles en escenario: \n");
    if(list_size(est_jug->current->Items) == 0){
        printf("Inventario vacio :0\n");
    } else {
        Item *item = list_first(est_jug->current->Items);
        int i = 1;
        while (item){
            printf(" N° %d -- Nombre: %s\n Valor: %d--- Peso: %d\n",i, item->nombre,item->valor, item->peso);
            printf("===============================\n");
            item = list_next(est_jug->current->Items);
            i++;
        } 

    }

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
    if(list_size(estado->current->Items) == 0){
        printf("No hay items para recoger en escenario.\n");
        return;
    } 
    printf("Items para recoger en escenario: \n");
    int i = 1;
    Item *item = list_first(estado->current->Items);
    while (item){
        printf(" N° %d -- Nombre: %s\n Valor: %d--- Peso: %d\n",i, item->nombre,item->valor, item->peso);
        printf("===============================\n");
        item = list_next(estado->current->Items);
        i++;
    } 
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
    list_popCurrent(estado->current->Items);
    printf("Item %s recogido.\n", item->nombre);
}

void descartar_items(EstadoJuego *estado){
    if(list_size(estado->Jugador->Items) == 0){
        printf("No hay items para descartar.\n");
        return;
    } 
    printf("Items inventario: \n");
    int i = 1;
    Item *item = list_first(estado->Jugador->Items);
    while (item != NULL){
        printf(" N° %d -- Nombre: %s\n Valor: %d--- Peso: %d\n",i, item->nombre,item->valor, item->peso);
        printf("===============================\n");
        item = list_next(estado->Jugador->Items);
        i++;
    } 
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

    char guardar[100];
    strcpy(guardar,item2->nombre);

    list_popCurrent(estado->Jugador->Items);
    free(item2);
    printf("Item %s descartado.\n", guardar);
}

int calcular_tiempo(int peso){
    return (peso + 9 ) / 10;
}

/*void mover(EstadoJuego *estado){
    char direccion[50];
    printf("Direccion (arriba,abajo,izquierda,derecha): ");
    fgets(direccion,sizeof(direccion),stdin);
    direccion[strcspn(direccion,"\n")] = 0;
    
    Nodo *vecino = (Nodo *)map_search(estado->current->vecinos, direccion);
    if(!vecino){
        printf("Direccion invalida.\n");
        return;
    }
    int tiempo = calcular_tiempo(estado->Jugador->pesoTotal);
    if(estado->Jugador->tiempo < tiempo ){
        printf("No hay suficiente tiempo.\n");
        return;
    }
    estado->Jugador->tiempo -= tiempo;
    estado->current = vecino;

    printf("Se movio a : %s\n", vecino->nombre);

    if(vecino->final){
        printf("Escenario final!\n");
        printf("Puntaje final: %d\n", estado->Jugador->puntuacion);
        estado->Jugador->tiempo = 0;

    }
}*/

void mover(EstadoJuego estado){

    Nodohabitacion = estado->current;
    printf("Direcciones validas:\n");

    if(habitacion->arriba != - 1)
        printf("Ariiba\n");
    if(habitacion->abajo != - 1)
        printf("Abajo\n");
    if(habitacion->izquierda != - 1)
        printf("Izquierda\n");
    if(habitacion->derecha != - 1)
        printf("Derecha\n");

    char direccion[50];
    printf("\nIngrese la direccion deseada: ");
    fgets(direccion,sizeof(direccion),stdin);
    direccion[strcspn(direccion,"\n")] = 0;

    Nodo vecino = (Nodo)map_search(estado->current->vecinos, direccion);
    if(!vecino){
        printf("Direccion invalida.\n");
        return;
    }
    int tiempo = calcular_tiempo(estado->Jugador->pesoTotal);
    if(estado->Jugador->tiempo < tiempo ){
        printf("No hay suficiente tiempo.\n");
        return;
    }
    estado->Jugador->tiempo -= tiempo;
    estado->current = vecino;

    printf("Se movio a : %s\n", vecino->nombre);

    if(vecino->final == 1){
        printf("Escenario final!\n");
        printf("Puntaje final: %d\n", estado->Jugador->puntuacion);
        estado->Jugador->tiempo = 0;

    }
}

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



EstadoJuego *reiniciar(Nodo *inicio, EstadoJuego *currentPlay) {
    liberar(currentPlay->Jugador->Items);
    free(currentPlay->Jugador);
    free(currentPlay);
    
    
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



void iniciarPartida(Map *grafo, Nodo *inicio) {
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
    while(!salir && player->tiempo > 0){
        mostrar_estado_actual(estado);
        //arreglar
        //ver eso de listclean y listPop
        printf("\nOpciones:\n1. Recoger Item\n2. Descartar Item\n3. Avanzar\n4. Reiniciar\n5. Salir\n");
        scanf("%d",&opcion);
        getchar();

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
    printf("Fin del juego. Tiempo agotado o final alcanzado.\n");
    liberar(player->Items);
    free(player);
    free(estado);
}
int pedir_id_escenario_inicial(Map *grafo) {
    printf("Escenarios disponibles:\n");
    MapPair *p = map_first(grafo);
    while (p) {
        Nodo *n = (Nodo *)p->value;
        int id = *(int *)p->key;
        printf("ID (key): %d - ID (nodo): %d - Nombre: %s\n", id, n->id, n->nombre);
        p = map_next(grafo);
    }

    int id_elegido;
    printf("Ingrese el ID del escenario inicial: ");
    scanf("%d", &id_elegido);
    getchar();  // limpiar buffer

    return id_elegido;
}


void iniciarPartidaMultijador(Map *grafo, Nodo *inicio){
    EstadoMultijugador *estado = malloc(sizeof(EstadoMultijugador));

    estado->jugador1 = malloc(sizeof(Jugador));
    estado->jugador1->Items = list_create();
    estado->jugador1->tiempo = 10;
    estado->jugador1->puntuacion = 0;
    estado->jugador1->pesoTotal = 0;

    estado->jugador2 = malloc(sizeof(Jugador));
    estado->jugador2->Items = list_create();
    estado->jugador2->tiempo = 10;
    estado->jugador2->puntuacion = 0;
    estado->jugador2->pesoTotal = 0;

    estado->nodo_j1 = inicio;
    estado->nodo_j2 = inicio;
    estado->turno_actual = 0;

    int salir = 0;

    while(!salir){
        Jugador *jug;
        Nodo *nodo;
        if(estado->turno_actual = 0) {
            printf("\n=== Turno del jugador 1 ===\n");
            jug = estado->jugador1;
            nodo = estado->nodo_j1;
        } else {
            printf("\n=== Turno del jugador 2 ===\n");
            jug = estado->jugador2;
            nodo = estado->nodo_j2;
        }
        EstadoJuego temp;
        temp.Jugador = jug;
        temp.current = nodo;

        mostrar_estado_actual(&temp);

        int acciones = 2;
        while(acciones > 0 && jug->tiempo > 0){
            printf("\nOpciones:\n1. Recoger Item\n2. Descartar Item\n3. Avanzar\n4. Terminar Turno\n");
            int opcion;
            scanf("%d",&opcion);
            getchar();

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
            
                default:
                    printf("Opcion no valida.\n");
            }
        }
        if( estado->jugador1->tiempo <= 0 && estado->jugador2->tiempo <= 0){
            printf("\nFin del juego. Ambos jugadores terminaron.\n");
            printf("Jugador 1 - Puntaje: %d\n", estado->jugador1->puntuacion);
            printf("Jugador 2 - Puntaje: %d\n", estado->jugador2->puntuacion);
            salir = 1;
        }
        estado->turno_actual = (estado->turno_actual + 1) % 2;
    }

    liberar(estado->jugador1->Items);
    liberar(estado->jugador2->Items);
    free(estado->jugador1);
    free(estado->jugador2);
    free(estado);
}









int main() {
    Map *grafo = map_create(int_equal);
    Nodo *esc = NULL;
    int opcion;
    printf("Ingresar opcion: \n");

    do {
        mostrarMenuPrincipal();
        scanf("%d", &opcion);
        getchar();

        switch (opcion){
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
                //printf("Laberinto cargado correctamente.\n");
                //break;
            case 2:
                /*if (grafo == NULL || map_first(grafo) == 0) {
                    printf("Primero debes cargar el laberinto.\n");
                } else {
                    int id = pedir_id_escenario_inicial(grafo);
                    int *keyBusqueda = malloc(sizeof(int));
                    *keyBusqueda = id;
                    Nodo *escenario_inicial = (Nodo *)map_search(grafo, keyBusqueda);
                    free(keyBusqueda);

                    if (!escenario_inicial) {
                        printf("No existe un escenario con ese ID.\n");
                    } else {
                        iniciarPartida(grafo, escenario_inicial);
                    }
                }*/
                iniciarPartida(grafo, esc);
                break;
            case 3:
                if(grafo == NULL || map_first(grafo) == 0){
                    printf("Debes cargar el laberinto.\n");
                }else {
                    iniciarPartidaMultijador(grafo, esc);
                }
                break;
            case 4:
                printf("Saliendo del juego...\n");
                break;
            default:
                printf("Opcion no valida.\n");
        }
    } while (opcion != 4);

  return 0;
}




//gcc tarea3.c tdas/list.c tdas/map.c tdas/extra.c -o graphquest
//./graphquest
