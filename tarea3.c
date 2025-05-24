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
    char descripcion[100];
    int id;
    char nombre[100];
    int final;
    List *Items;
    Map *vecinos;
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

void mostrarMenuPrincipal() {
  puts("========================================");
  puts("      GraphQuest");
  puts("========================================");
  puts("1) Cargar Laberinto");
  puts("2) Iniciar Partida");
  puts("3) Salir");
}

int int_equal(void *a, void *b) {
    return (*(int *)a == *(int *)b);
}

/**
 * Carga canciones desde un archivo CSV
 */
void leer_escenarios(Map *grafo) {
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
    char *conns = map_search(referencias, &id);
    List *dirList = split_string(conns, ";");

    for (char *d = list_first(dirList); d; d = list_next(dirList)) {
        char *dir = strtok(d, ":");
        int dest = atoi(strtok(NULL, ":"));
        Nodo *vecino = map_search(grafo, &dest);
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
}


//Cada vez que se entra a un nuevo escenario o se realiza una acción, se muestra el siguiente menú:
void mostrar_estado_actual(EstadoJuego *est_jug){
    printf("\n===== Escenario: %s =====\n", est_jug->current->nombre);
    printf("Descripcion escenario: %s\n", est_jug->current->descripcion);
    printf("Inventario.\n");

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
    Item *item = list_first(estado->Jugador->Items);
    for(int i = 1; i < opcion; i++){
        item = list_next(estado->Jugador->Items);
    }
    estado->Jugador->pesoTotal -= item->peso;
    estado->Jugador->puntuacion -= item->valor;
    estado->Jugador->tiempo -= 1;

    char guardar[100];
    strcpy(guardar,item->nombre);

    list_popCurrent(estado->Jugador->Items);
    free(item);
    printf("Item %s descartado.\n", guardar);
}

int calcular_tiempo(int peso){
    return (peso + 9 ) / 10;
}

void mover(EstadoJuego *estado){
    char direccion[50];
    printf("Direccion (arriba,abajo,izquierda,derecha): ");
    fgets(direccion,sizeof(direccion),stdin);
    direccion[strcspn(direccion,"\n")] = 0;
    
    Nodo *vecino = map_search(estado->current->vecinos, direccion);
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
}

void liberar(List *items){
    if(!items) return;
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
                printf("Laberinto cargado correctamente.\n");
                break;
            case 2:
                if(!esc){
                    int idI = 1;
                    esc = map_search(grafo, &idI);
                }
                if(!esc){
                    printf("No existe escenario incial.\n");
                    break;
                }
                iniciarPartida(grafo,esc);
                break;
            case 3:
                printf("Saliendo del juego...\n");
                break;
            default:
                printf("Opcion no valida.\n");
        }
    } while (opcion != 3);

  return 0;
}



/*case 2:
       printf("Ingrese ID escenario inical: ");
       int id;
       scanf("%d", &id);
       getchar();
       esc = map_search(grafo, &id);

       if(!esc){
          printf("No existe un escenario con ese ID.\n");
          break;
        }
        iniciarPartida(grafo,esc);
        break;




*/
