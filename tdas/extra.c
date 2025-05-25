#include "extra.h"


#define MAX_LINE_LENGTH 1024
#define MAX_FIELDS 300

/*char **leer_linea_csv(FILE *archivo, char separador) {
  static char linea[MAX_LINE_LENGTH];
  static char *campos[MAX_FIELDS];
  char *ptr, *start;
  int idx = 0;

  if (fgets(linea, MAX_LINE_LENGTH, archivo) == NULL) {
    return NULL; // No hay más líneas para leer
  }

  // Eliminar salto de linea
  linea[strcspn(linea, "\n")] = '\0';

  ptr = start = linea;
  while (*ptr) {
    if (idx >= MAX_FIELDS - 1)
      break;

    if (*ptr == '\"') { // Inicio de un campo entrecomillado
      start = ++ptr;    // Saltar la comilla inicial
      while (*ptr && !(*ptr == '\"' && *(ptr + 1) == separador))
        ptr++;
    } else { // Campo sin comillas
      start = ptr;
      while (*ptr && *ptr != separador)
        ptr++;
    }

    if (*ptr) {
      *ptr = '\0'; // Reemplazar comilla final o separador por terminación
      ptr++;
      if (*ptr == separador)
        ptr++;
    }

    // Quitar comilla final si existe
    if (*(ptr - 2) == '\"') {
      *(ptr - 2) = '\0';
    }

    campos[idx++] = start;
  }

  campos[idx] = NULL; // Marcar el final del array
  return campos;
}*/
char **leer_linea_csv(FILE *f, char delim) {
    char buffer[2048];
    if (!fgets(buffer, sizeof(buffer), f)) return NULL;

    int len = strlen(buffer);
    if (len > 0 && buffer[len-1] == '\n') buffer[len-1] = '\0';

    char **campos = malloc(20 * sizeof(char*)); // máximo 20 campos
    int campo_idx = 0;

    int in_quotes = 0;
    int start = 0;
    for (int i = 0; ; i++) {
        char c = buffer[i];
        int end_of_field = 0;

        if (c == '"') {
            in_quotes = !in_quotes;
        } 

        if ((c == delim && !in_quotes) || c == '\0') {
            end_of_field = 1;
            int size = i - start;
            // Ignorar comillas al inicio y fin
            int real_start = start;
            int real_end = i - 1;
            if (size > 1 && buffer[real_start] == '"' && buffer[real_end] == '"') {
                real_start++;
                real_end--;
                size = real_end - real_start + 1;
            }

            // Copiar campo con malloc
            char *campo = malloc(size + 1);
            if (campo == NULL) {
                // manejar error malloc
                for (int j = 0; j < campo_idx; j++) free(campos[j]);
                free(campos);
                return NULL;
            }
            memcpy(campo, buffer + real_start, size);
            campo[size] = '\0';

            // Eliminar espacios al inicio y fin del campo
            // Quitar espacios a la izquierda
            int inicio_campo = 0;
            while (campo[inicio_campo] == ' ' && campo[inicio_campo] != '\0') inicio_campo++;

            // Quitar espacios a la derecha
            int fin_campo = strlen(campo) - 1;
            while (fin_campo >= 0 && campo[fin_campo] == ' ') {
                campo[fin_campo] = '\0';
                fin_campo--;
            }

            if (inicio_campo > 0) {
                memmove(campo, campo + inicio_campo, strlen(campo + inicio_campo) + 1);
            }

            campos[campo_idx++] = campo;
            start = i + 1;
        }

        if (c == '\0') break;
    }

    campos[campo_idx] = NULL; // marca fin
    return campos;
}



List *split_string(const char *str, const char *delim) {
  List *result = list_create();
  char *token = strtok((char *)str, delim);

  while (token != NULL) {
    // Eliminar espacios en blanco al inicio del token
    while (*token == ' ') {
      token++;
    }

    // Eliminar espacios en blanco al final del token
    char *end = token + strlen(token) - 1;
    while (*end == ' ' && end > token) {
      *end = '\0';
      end--;
    }

    // Copiar el token en un nuevo string
    char *new_token = strdup(token);

    // Agregar el nuevo string a la lista
    list_pushBack(result, new_token);

    // Obtener el siguiente token
    token = strtok(NULL, delim);
  }

  return result;
}

// Función para limpiar la pantalla
void limpiarPantalla() { system("clear"); }

void presioneTeclaParaContinuar() {
  puts("Presione una tecla para continuar...");
  getchar(); // Consume el '\n' del buffer de entrada
  getchar(); // Espera a que el usuario presione una tecla
}