#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100000

int main()
{
	int i, j, k, d;
	long long int *a, *a1, *a2, si, sa;
	clock_t comienzo, final;

	comienzo=clock();

    // Creando vector principal
    a=(long long int *) malloc(sizeof(long long int)*N);
    
	for(i=0; i<N; i++) a[i]=i+1;
    
    // Crear subarreglos para guardar los amigos
    a1=(long long int *) malloc(sizeof(long long int)*N);
    a2=(long long int *) malloc(sizeof(long long int)*N);
    
    // Comienza la busqueda
    for(i=0; i<N; i++)
    {
        si=0;
        
        d=a[i]/2;
        
        for(j=1; j<=d; j++)
        {
            if (a[i]%j==0)
            {
                si+=j;
            }
        }
        
        // Si el posible amigo no es el mismo numero
        if (a[i]!=si)
        {
            // Confirmar amistad con a[si]
            for (k=0; k<N; k++)
            {
                if (a[k]==si)   // Si existe el posible amigo en el arreglo
                {
                    sa=0;
                    
                    d=a[k]/2;
                    
                    for(j=1; j<=d; j++)
                    {
                        if (a[k]%j==0)
                        {
                            sa+=j;
                        }
                    }
                    
                    if (sa==a[i])
                    {
                        printf("%lld %lld\n", a[i], si);
                    }
                    
                    break;
                }
            }
        }
    }
    
    final=clock();
    
    int h,m;
	float s;
	s = (float)(final - comienzo) / CLOCKS_PER_SEC;
	
	m = ((int)s / 60) % 60;
	h = s / 3600;
	
	while(s >= 60) { // Como no es int no se puede usar modulo
		s -= 60.0;
	}
	
	printf("Tiempo total: %02d:%02d:%09.06f\n", h, m, s);
    
    return 0;
}
