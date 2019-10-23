/*
	EXEMPLO DE GERAÇÃO DE GIF

	Programa recebe um inteiro N, cria um vetor com N
	elementos variando de 0 a N-1, posteriormente embaralha
	os valores e chama a função de ordenação. Também recebe
	M que será usado para definir o tamanho do gif.

	Dentro da função de ordenação, além de ordenar, também é 
	criado um gif, sempre que swap é chamado, logo em seguida 
	adiciona-se um novo frame ao gif com o estado atual do vetor.

	O resultado é um gif (dimensão MxM) que pega uma sequência 
	de cores desordenada e a ordena para formar o espectro de cores.

	Recomenda-se que o valor N seja entre 10 a 100. A medida que 
	aumenta a quantidade de frames do gif também aumenta o tempo 
	de execução, programa fica bem lento (a visualização 
	final também).

*/

#include<math.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include "gifenc.h"

/*
	(Função auxiliar)
	Embaralha valores de um vetor

	v: lista de inteiros que serão embaralhados
	n: tamanho de v

	retorno: void
*/
void shuffle(int *v, size_t n) {
    srand(3);
    if (n > 1) {
        size_t i;
        for (i = 0; i < n - 1; i++) {
          size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
          int t = v[j];
          v[j] = v[i];
          v[i] = t;
        }
    }
}

/*
	(Função auxiliar)
	Transforma uma cor na representação HSV para representação RGB
	(fonte: https://pt.wikipedia.org/wiki/HSV)

	h, s, v: componentes da cor HSV
	r, g, b: componentes da cor RGB

	retorno: void
*/
void hsv2rgb(float h, float s, float v, int *r, int *g, int *b) {
	int h_i = (int)(h/60) % 6;
	float f = h/60 - h_i;
	float p = v*(1 - s);
	float q = v*(1 - f*s);
	float t = v*(1 - (1 - f)*s);
	
	if(h_i == 0) {
		*r = 255*v; *g = 255*t; *b = 255*p;
	} else if(h_i == 1) {
		*r = 255*q; *g = 255*v; *b = 255*p;
	} else if(h_i == 2) {
		*r = 255*p; *g = 255*v; *b = 255*t;
	} else if(h_i == 3) {
		*r = 255*p; *g = 255*q; *b = 255*v;
	} else if(h_i == 4) {
		*r = 255*t; *g = 255*p; *b = 255*v;
	} else { // h_i == 5
		*r = 255*v; *g = 255*p; *b = 255*q;
	}
}

/*
	(Função auxiliar)
	Gera uma paleta de cores com tamanho n. 
	As cores variam no espectro do HSV quando 
	fixa s = 1 e v = 1 e altera h entre 0 a 360

	n: número de cores da paleta

	retorno: vetor de tamanho n*3 (pois cada cor tem três componentes),
	posições 0, 1, 2 são a primeira cor da paleta, 3, 4, 5 são segunda 
	cor e assim por diante 
*/
uint8_t* get_palette(int n) {
	uint8_t *palette = (uint8_t*)malloc(n*3*sizeof(uint8_t));
	
	int h,s = 1,v = 1;
	int r, g, b;
	for (int i = 0, j = 0; i < n*3; i+=3, j++) {
		h = j*360/n;
		hsv2rgb(h,s,v,&r,&g,&b);
		palette[i] = r;
		palette[i+1] = g;
		palette[i+2] = b;
	}
	return palette;
}

/*
	Cria uma variável gif inicial com as informações 
	de nome do arquivo, dimensões, paleta de cores

	n: número de cores da variável 
	(também usado para definir o tamanho do gif n x n pixels)
	filename: nome do arquivo onde o gif será salvo

	retorno: ponteiro para a instância do gif criada
*/
ge_GIF* creat_gif(int m, char filename[50]) {
	uint8_t *palette = get_palette(255);

	ge_GIF *gif = ge_new_gif(filename,m,m,palette,8,0);
    
    return gif;
}

/*
	Adiciona um frame ao gif, mapeia os valores de 
	um vetor de inteiros para as cores das linhas do gif
	em forma de triângulo 

	gif: instância de gif onde o frame é inserido
	values: lista de inteiros, o valor x na posição i
	define que a cor na linha i do gif será a x-iésima 
	cor da paleta de cores
	n: total de cores da paleta (ou dimensão do gif)

	retorno: void
*/
void add_frame(ge_GIF *gif, int *values, int n, int m) {
	int row = 0;
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m/n; j++) {
			for (int k = 0; k < m; k++) {
				if (k > (values[i]*(m/n))/2 && k < m - (values[i]*(m/n))/2) {
					gif->frame[row*m+k] = values[i]*255/n;
				} else {
					gif->frame[row*m+k] = 255;
				}
			}
			row ++;
		}

	}
	ge_add_frame(gif, 5);
}

/*
	Quando não tiver mais nenhum frame a ser adicionado, 
	então essa função deve ser chamada para salvar o gif

	gif: instância a ser salva

	retorno: void
*/
void save_gif(ge_GIF *gif) {
	ge_close_gif(gif);
}

/*
	Troca os valores v[i] e v[j] no vetor
	
	v: lista de valores
	i, j: posições cujos valores devem ser trocados

	retorno: void
*/
void swap(int *v, int i, int j) {
	int temp = v[i];
	v[i] = v[j];
	v[j] = temp;
}

/*
	Ordena uma lista de valores (e cria gif do passo a passo)

	values: lista de valores que serão ordenados
	n: tamanho de values
	
	retorno: void
*/
void sort(int *values, int n, int m) {
	ge_GIF *gif = creat_gif(m,"out.gif"); // exemplo de má prática de programação :(
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			if (values[i] > values[j]) {
				swap(values,i,j);
				add_frame(gif,values,n,m);
			}
		}
	}
	save_gif(gif);
}

int main() {

	/*
		Espera o usuário inserir o tamanho do vetor
	*/
	int n;
	scanf("%d", &n);

	int v[n];
	for(int i = 0; i < n; i++) {
		v[i] = i;
	}

	int m;
	scanf("%d", &m);

	shuffle(v,n);

	sort(v,n,m);

	return 0;

}
