#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

/*
	Si la frecuencia es muy alta tardara mucho en hacer Reseed
	Por cada lectura tendremos 4 bytes.
	
	Si la frecuencia es 1048576 Tendremos un Reseed cada 4 Megabyte
	
	Un numero muy bajo en la frecuencia tendremos reseed muy seguipo
	pero el programa podria ser  algo lento. Valores mayores de 1024 son aceptables
	Reseed Apartir de cada 4 Kilobytes
 */
#define FRECUENCIA 1048576

/*
	MAGIC_VALUE es definido por el usario
 */
#define MAGIC_VALUE 3753902

void rerand_init();
unsigned int rerand_read();
void rerand_remap();
void rerand_srand();
unsigned int rerand_buffer(unsigned char *buffer,unsigned int size);

struct rerand	{
	unsigned char dup[256];
	unsigned char map[256];
	unsigned char minidup[4];
	unsigned char minimap[4];
	unsigned long long int contador;
	union r	{
		unsigned int v;
		unsigned char c[4];
	}o,d;
}rerand;

void rerand_init()	{
	srand(time(NULL)+rand()+MAGIC_VALUE);
	rerand.contador = 0;
	rerand_remap();
}

unsigned int rerand_read()	{
	rerand.o.v = 0;
	rerand.d.v = 0;
	rerand.o.v = rand();
	rerand.o.v = rerand.o.v << 16;
	rerand.d.v = rand();
	rerand.d.v <<= 16;
	rerand.d.v >>= 16;
	rerand.o.v += rerand.d.v;
	
	rerand.d.c[rerand.minimap[0]] = rerand.map[rerand.o.c[0]];
	rerand.d.c[rerand.minimap[1]] = rerand.map[rerand.o.c[1]];
	rerand.d.c[rerand.minimap[2]] = rerand.map[rerand.o.c[2]];
	rerand.d.c[rerand.minimap[3]] = rerand.map[rerand.o.c[3]];

	rerand.contador++;
	if(rerand.contador % FRECUENCIA == 0){ 
		rerand_remap();
		rerand_srand();
	}
	return rerand.d.v;
}

void rerand_srand()	{
	srand(time(NULL)+rand()+MAGIC_VALUE);
}
void rerand_remap()	{
	int i;
	unsigned char r = 0;
	memset(rerand.dup,0,256);
	memset(rerand.minidup,0,4);
	i = 0;
	while(i < 4)	{
		r = rand() % 4;
		if(rerand.minidup[r] == 0)	{
			rerand.minimap[i] = r;
			rerand.minidup[r] = 1;
			i++;
		}
	}
	i = 0;
	while(i < 256)	{
		r = rand() % 256;
		if(rerand.dup[r] == 0)	{
			rerand.map[i] = r;
			rerand.dup[r] = 1;
			i++;
		}
	}
}

unsigned int rerand_buffer(unsigned char *buffer,unsigned int size)	{
	unsigned int valor,len,offset = 0;
	unsigned int *b;
	b = (unsigned int*)buffer;
	len = size;
	while(offset < size)	{
		valor = rerand_read();
		*b++ = valor;
		offset += 4;
		len -= 4;
	}
	return (size - len);
}

int main()	{
	int i = 0;
	unsigned char *buffer;
	rerand_init();
	buffer = malloc(1000);
	rerand_buffer(buffer,1000);
	while(i < 1000)	{
		printf("%.2x",buffer[i]);
		i++;
	}
}