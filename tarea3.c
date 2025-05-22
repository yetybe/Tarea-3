#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char nombre[100];
    int valor;
    int peso;
} Item;

typedef struct {
    int id;
    char nombre[100];
    char descripcion[100];
    int final;
    List *Items;
    int arriba, abajo, izquierda, derecha; 
} Escenario;

typedef struct {
    List *Items;
    int tiempo;
    int puntuacion;
    int pesoTotal;
} Jugador;

typedef struct{
    Escenario *current_scene;
    Jugador *current_player;
} EstadoJuego;

void mostrarMenuPrincipal() {
  puts("========================================");
  puts("      GraphQuest");
  puts("========================================");
  puts("1) Cargar Laberinto");
  puts("2) Iniciar Partida");
  puts("3) Buscar Escenario");
  puts("4) Ver estado jugador");
  puts("5) Salir");
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
  // Leer y parsear una línea del archivo CSV. La función devuelve un array de
  // strings, donde cada elemento representa un campo de la línea CSV procesada.
  //leer_linea_csv(archivo, ','); // Lee los encabezados del CSV


  // Lee cada línea del archivo CSV hasta el final
  while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
    Escenario *newEscenario = malloc(sizeof(Escenario));
    newEscenario->id = atoi(campos[0]);
    strcpy(newEscenario->nombre, campos[1]);
    strcpy(newEscenario->descripcion, campos[2]);
    newEscenario->items = list_create();
    newEscenario->arriba = atoi(campos[4]);
    newEscenario->abajo = atoi(campos[5]);
    newEscenario->izquierda = atoi(campos[6]);
    newEscenario->derecha = atoi(campos[7]);
    newEscenario->final = atoi(campos[8]);

    /*printf("ID: %d\n", atoi(campos[0]));
    printf("Nombre: %s\n", campos[1]);
    printf("Descripción: %s\n", campos[2]);*/

    List* itemsRaw = split_string(campos[3], ";");

    //printf("Items: \n");
    for(char *itemStr = list_first(itemsRaw); itemStr != NULL; 
        itemStr = list_next(itemsRaw)){

        List* values = split_string(itemStr, ",");
        Item *new_item = malloc(sizeof(Item));

        strcpy(new_item->nombre, list_first(values));
        new_item->valor = atoi(list_next(values));
        new_item->peso = atoi(list_next(values));

        list_pushBack(newEscenario->Items,new_item)
        list_clean(values);
        free(values);
    }
    list_clean(itemsRaw);
    free(itemsRaw);

    /*if (arriba != -1) printf("Arriba: %d\n", arriba);
    if (abajo != -1) printf("Abajo: %d\n", abajo);
    if (izquierda != -1) printf("Izquierda: %d\n", izquierda);
    if (derecha != -1) printf("Derecha: %d\n", derecha);*/

    list_pushBack(all_escenarios, newEscenario);
    int *key = malloc(sizeof(int));
    *key = newEscenario->id;
    map_insert(grafo,key,newEscenario);
    /*int is_final = atoi(campos[8]);
    if (is_final) printf("Es final\n");*/
  }
  fclose(archivo); // Cierra el archivo después de leer todas las líneas
}


//Cada vez que se entra a un nuevo escenario o se realiza una acción, se muestra el siguiente menú:
void mostrar_estado_actual(Escenario *newEsc, Jugador *player){
    printf("\n===== Escenario: %s =====\n", newEsc->nombre);
    printf("Descripcion escenario: %s\n", newEsc->descripcion);
    printf("Items disponibles en escenario: \n");

    if(list_size(newEsc->Items) == 0){
        printf("No hay items disponibles :(\n");
    } else {
        int i = 1;
        Item *item = list_first(newEsc->Items);
        while (item != NULL){
            printf(" N° %d -- Nombre: %s\n Valor: %d--- Peso: %d\n",i, item->nombre,item->valor, item->peso);
            printf("===============================\n");
            item = list_next(newEsc->Items);
            i++;
        } 

    }
    printf("Direcciones disponibles:\n");
    if (newEsc->arriba != -1) printf(" Arriba: %d\n", newEsc->arriba);
    if (newEsc->abajo != -1) printf(" Abajo: %d\n", newEsc->abajo);
    if (newEsc->izquierda != -1) printf(" Izquierda: %d\n", newEsc->izquierda);
    if (newEsc->derecha != -1) printf(" Derecha: %d\n", newEsc->derecha);

    printf("\n===== Estado del Jugador =====\n");
    printf("Tiempo restante: %d\n", player->tiempo);
    printf("Puntaje acumulado: %d\n", player->puntuacion);
    printf("Peso total: %d\n", player->pesoTotal);

    
    if(list_size(player->Items) == 0){
        printf("Inventario vacio :0\n");
    } else {
        printf("Inventario:\n");
        int i = 1;
        Item *item = list_first(player->Items);
        while (item != NULL){
            printf(" N° %d -- Nombre: %s\n Valor: %d--- Peso: %d\n",i, item->nombre,item->valor, item->peso);
            printf("===============================\n");
            item = list_next(player->Items);
            i++;
        } 

    }
}

void recoger_items(Escenario *newEsc, Jugador *player){
    if(list_size(newEsc->Items) == 0){
        printf("No hay items para recoger en escenario.\n");
        return;
    } 
    printf("Items para recoger en escenario: \n");
    int i = 1;
    Item *item = list_first(newEsc->Items);
    while (item != NULL){
        printf(" N° %d -- Nombre: %s\n Valor: %d--- Peso: %d\n",i, item->nombre,item->valor, item->peso);
        printf("===============================\n");
        item = list_next(newEsc->Items);
        i++;
    } 
    printf("Ingrese numero item recoger: \n");
    int opcion;
    scanf("%d", &opcion);
    getchar();
    if(opcion <= 0 || opcion > list_size(newEsc->Items)){
        printf("Numero invalido.\n");
        return;
    }
    item = list_first(newEsc->Items);
    for(int i = 1; i < opcion; i++){
        item = list_next(newEsc->Items);
    }

    Item *nuevo_item = malloc(sizeof(Item));
    *nuevo_item = *item;
    list_pushBack(player->Items, nuevo_item);

    player->pesoTotal += item->peso;
    player->puntuacion += item->valor;
    player->tiempo -= 1;

    list_clean(newEsc->Items, opcion - 1);
    printf("Item %s recogido.\n", item->nombre);
}

void descartar_items(Jugador *player){
    if(list_size(player->Items) == 0){
        printf("No hay items para descartar.\n");
        return;
    } 
    printf("Items inventario: \n");
    int i = 1;
    Item *item = list_first(player->Items);
    while (item != NULL){
        printf(" N° %d -- Nombre: %s\n Valor: %d--- Peso: %d\n",i, item->nombre,item->valor, item->peso);
        printf("===============================\n");
        item = list_next(player->Items);
        i++;
    } 
    printf("Ingrese numero item recoger: \n");
    int opcion;
    scanf("%d", &opcion);
    getchar();
    if(opcion <= 0 || opcion > list_size(player->Items)){
        printf("Numero invalido.\n");
        return;
    }
    item = list_first(player->Items);
    for(int i = 1; i < opcion; i++){
        item = list_next(player->Items);
    }

    player->pesoTotal -= item->peso;
    player->puntuacion -= item->valor;
    player->tiempo -= 1;

    Item *eliminar = list_clean(player->Items, opcion - 1);
    free(eliminar);
    printf("Item descartado.\n");
}

void mover(Map *grafo, EstadoJuego *estado, char *comprobar){
    int new_id = -1;
    if(strcmp(comprobar, "arriba") == 0){
        new_id = estado->current_scene->arriba;
    } else if(strcmp(comprobar, "abajo") == 0){
        new_id = estado->current_scene->abajo;
    } else if(strcmp(comprobar, "izquierda") == 0){
        new_id = estado->current_scene->izquierda;
    } else if(strcmp(comprobar, "derecha") == 0){
        new_id = estado->current_scene->derecha;
    } else {
        printf("Direccion invalida.\n");
        return;
    }
    if(new_id == -1){
        printf("No hay escenario con ea direccion.\n");
        return;
    }
    Escenario *new_esc = map_search(grafo, &new_id);
    if(!new_id){
        printf("Escenario destino no existe.\n");
        return;
    }
    int tiempo = calcular_tiempo(estado->current_player->pesoTotal);
    if(estado->current_player->tiempo - tiempo < 0){
        printf("No hay suficiente.\n");
        return;
    }
    estado->current_player->tiempo -= tiempo;
    estado->current_scene = new_esc;

    printf("Se movio a : %s\n", new_esc->nombre);

    if(new_esc->final){
        printf("Escenario final!\n");
        printf("Puntaje final: %d\n", estado->current_player->puntuacion);
        printf("Inventario final:\n");
        int i = 1;
        Item *item = list_first(estado->current_player->Items);
        while(item != NULL){
            printf(" N° %d -- Nombre: %s\n Valor: %d--- Peso: %d\n",i, item->nombre,item->valor, item->peso);
            printf("===============================\n");
            item = list_next(estado->current_player>Items);
            i++;
        }
        estado->current_player->tiempo = 0;

     }
    

}









void iniciarPartida(Map *grafo, Escenario *inicio) {
    int key = 0;
    Escenario *inicio = map_search(grafo, &key);
    if(!inicio){
        printf("Escenario no encontrado.\n");
        return;
    }
    Jugador *player = malloc(sizeof(Jugador));
    player->items = list_create();
    player->tiempo = 10;
    player->puntuacion = 0;
    player->pesoTotal = 0;

    EstadoJuego estado;
    estado.current_scene = inicio;
    estado.current_player = player;

    printf("Te encuntras en :");
}




int main() {
    Map *grafo = map_create();
    List *all_escenarios = list_create();

    int opcion;
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
                iniciarPartida(grafo);
                break;
            case 5:
                printf("Saliendo del juego...\n");
                break;
            default:
                printf("Opcion no valida.\n");
        }
    } while (opcion != 5);

  return 0;
}
