#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <mpi.h>

//#define N 1000000
#define N 100000

int main(int argc, char **argv) {
	int nproc, pid, sproc;
	int i, j, tmp, exists, size, size2, *vector, finished_processes=0;
	int si, sa, d;
	clock_t start, end;
	MPI_Status status;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &pid);

	sproc = nproc - 1;
	
	start = clock();
	
	if(N % sproc == 0) {
		
		/* Si los datos se pueden distribuir equitativamente size es igual para
		 * todos
		 */
		size = N / sproc;
	} else {
		/* Si los datos no se pueden distribuir equitativamente el ultimo
		 * proceso tendra menos datos (size2) y los demas tendran size datos
		 */
		size = (N / sproc) + 1;
		size2 = N - (size * (sproc - 1));
	}

	if(pid == 0) { 	// Proceso maestro
		vector = (int *)malloc(sizeof(int) * N);

		for(i=0; i<N; i++) {
			vector[i] = i+1;
		}

		for(i=1; i<nproc; i++) {
			if(N % sproc == 0 || i != nproc - 1) {
				MPI_Send(&vector[(i-1)*size],size, MPI_INT, i, i, MPI_COMM_WORLD);
			} else {
				MPI_Send(&vector[(i-1)*size],size2, MPI_INT, i, i, MPI_COMM_WORLD);
			}
		}

		while(finished_processes < sproc) {
			/* Se va a recibir de cualquier proceso y la etiqueta puede ser 0
			 * o 1
			 */
			MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
					 MPI_COMM_WORLD, &status);

			if(status.MPI_TAG == 0) { // Tag 0 es preguntar al proceso maestro
				exists = 0;

				for(i=tmp-1; i<N; i++) {
					if(vector[i] == tmp) {
						exists = 1;
						break;
					}
				}
				
				/* Si tmp existe en el vector principal
				 * enviar un 1 confirmando o un 0 para indicar que
				 * no existe el numero
				 */
				MPI_Send(&exists, 1, MPI_INT, status.MPI_SOURCE, status.MPI_SOURCE, 
						 MPI_COMM_WORLD);

			} else {  // Tag 1 es indicar que el proceso esclavo ya termino
				/* Cuando finished_processes sea igual a sproc significa que
				 * todos los procesos han terminado
				 */
				finished_processes++;
			}
		}
	} else {		// Procesos esclavos
		vector = (int *)malloc(sizeof(int) * size);
		
		/* Si los datos no se pueden repartir equitativamente entre los procesos
		 * el ultimo proceso va a tener menos datos, asi que se ajusta size al
		 * valor que tiene size2 para ese proceso
		 */
		if(N % sproc != 0 && pid == nproc - 1) {
			size = size2;
		}

		MPI_Recv(vector, size, MPI_INT, 0, pid, MPI_COMM_WORLD, &status);

		for(i=0; i<size; i++) {
			si = 0;

			d = vector[i] / 2;

			for(j=1; j <= d; j++) {
				if(vector[i] % j == 0) {
					si += j;
				}
			}

			// Si el posible amigo no es el mismo numero
			if(vector[i] != si) {
				//Verificar si 'si' existe
				MPI_Send(&si, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
				MPI_Recv(&exists, 1, MPI_INT, 0, pid, MPI_COMM_WORLD, &status);

				// Si existe el posible amigo en el arreglo
				if(exists) {   
					sa = 0;

					d = si / 2;

					for(j=1; j <= d; j++) {
						if(si % j == 0) {
							sa += j;
						}
					}

					if (sa == vector[i] && si > sa) {
						printf("%d %d\n", sa, si);
					}
				}
			}
		}
		MPI_Send(&size, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
	}
	
	if(pid == 0) {
		end = clock();

		int h,m;
		float s;
		s = (float)(end - start) / CLOCKS_PER_SEC;
		
		m = ((int)s / 60) % 60;
		h = s / 3600;
		
		while(s >= 60) { // Como no es int no se puede usar modulo
			s -= 60.0;
		}
		
		printf("Tiempo total: %02d:%02d:%09.06f\n", h, m, s);
	}
	
	MPI_Finalize();
	return 0;
}
