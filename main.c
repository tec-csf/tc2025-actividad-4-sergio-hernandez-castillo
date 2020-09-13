/*
Autór: Sergio Hernandez Castillo
Matrícula: A01025210
Descripción: Actividad 4 - Procesos

Nota: Trabajé junto con Daniel Roa en este ejercicio.
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>

typedef struct {
    int id;
    int promedio;
} Hijo;

void histograma(Hijo * hijos, Hijo * fin){
    int maximo = 0;

    for (Hijo * h = hijos; h < fin; ++h){
        if (h->promedio > maximo){
            maximo = h->promedio;
        }
    }

    printf("PID Hijo \tPromedio \tHistograma\n\n");

    for (Hijo * h = hijos; h < fin; ++h){
        printf("%d \t\t%d \t\t", h->id, h->promedio);

        for (int i = 0; i < ((h->promedio * 5) / maximo); ++i){
            printf("*");
        }

        printf("\n\n");
    }
}

int main(int argc, char * const * argv){
    int dato;
    int cantidadDeHijos = 0;
    char * cvalue = NULL;

    opterr = 0;

    while ((dato = getopt(argc, argv, "n:")) != -1){
        switch (dato){
            case 'n':
                cvalue = optarg;

                if (isdigit(*cvalue) > 0){
                    cantidadDeHijos = atoi(cvalue);
                    printf("Cantidad de hijos: %d\n\n", cantidadDeHijos);
                }

                else {
                    printf("No proporcionó un valor numérico\n");
                }
            break;

            case '?':
                if (optopt == 'n'){
                    fprintf(stderr, "Opción -%c requiere un argumento.\n", optopt);
                }
                    
                else if (isprint (optopt)){
                    fprintf (stderr, "Opción desconocida '-%c'.\n", optopt);
                }
                    
                else {
                    fprintf (stderr, "Opción desconocida '\\x%x'.\n", optopt);
                }
            
                return 1;

            default:
                abort();
            break;
        }
    }

    pid_t * ids = malloc(sizeof(pid_t) * cantidadDeHijos);
    pid_t * total = ids + cantidadDeHijos;
    pid_t * pos = ids;
    int i = 0;
    int hijosCreados = 0;
    int valorPromedio;
    pid_t pid;

    while ((pos < total) && (i < cantidadDeHijos)){
        pid = fork();
        *pos = pid;

        if (pid == -1){
            printf("Hubo un error al crear el proceso hijo %d\n", i);
            printf("Procesos hijos creados hasta el momento: %d\n\n", hijosCreados);

            break;
        }

        else if (pid == 0){
            int promedio = (getppid() + getpid()) / 2;
            printf("Estamos en el proceso hijo con PID = %d, su padre es PPID = %d y su promedio es %d\n", getpid(), getppid(), promedio);
            
            sleep(1);

            exit(promedio);
        }

        else {
            ++hijosCreados;
            printf("Hijos creados: %d\n\n", hijosCreados);
        }

        ++pos;
        ++i;
    }

    if (hijosCreados == 0){
        printf("No se pudo crear más de un hijo.");

        free(ids);

        return 0;
    }

    else if (hijosCreados > 0){
        Hijo * hijos = (Hijo *) malloc(sizeof(Hijo) * hijosCreados);
        Hijo * fin = hijos + hijosCreados;
        Hijo * h = hijos;
        pos = ids;

        while ((h < fin) && (pos < total)){
            if (waitpid(*pos, &valorPromedio, 0) != -1){
                if (WIFEXITED(valorPromedio)){
                    h->id = *pos;
                    h->promedio = WEXITSTATUS(valorPromedio);
                }
            }

            ++pos;
            ++h;
        }

        printf("\n");

        histograma(hijos, fin);

        free(ids);
        free(hijos);

        return 0;
    }
}
