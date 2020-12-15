Trabalho de Sistemas Operacionais, UFPEL, pelo professor Maurício Lima Pilla.

Implementar a função: nqueens.
Os parâmetros de entrada da função são:

* Número de rainhas
* Dimensões do tabuleiro (sempre tabuleiros quadrados)

A função deverá retornar a soma das posições das rainhas em cada uma das soluções. Cada casa do tabuleiro vale uma potência de 2, começando da primeira casa como 2^0 e incrementando a potência de 1 em 1 da esquerda para direita, de cima para baixo. Por exemplo, para um tabuleiro 2x2 e 1 rainha, as soluções são:

		X 0
		0 0  -->  2^0

		0 X
		0 0  -->  2^1

		0 0
		X 0  -->  2^2

		0 0
		0 X  -->  2^3

Somando, temos 1 + 2 + 4 + 8 = *15*.

Caso haja mais de uma rainha em uma solução, os valores das potências deverão ser somados:

		X 0 0
		0 0 X
		0 0 0  --> 2^0 + 2^5 = 33 

A implementação deverá explorar concorrência criando *threads* para explorar diferentes posicionamentos. A solução deverá usar alguma das técnicas de sincronização estudadas (*mutexes*, *semáforos* ou *monitores*) para garantir que não haja condição de corrida.
