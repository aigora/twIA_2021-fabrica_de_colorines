#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include <string.h>
#include "SerialClass.h"
#include <malloc.h>


#define NUM_OPCIONES 7

#define SI 1
#define NO 0
#define N 10
#define LONGCAD 50

typedef struct
{
	char id[LONGCAD];
	char color[LONGCAD];
	char cantidad;
}Producto;


//============================
// Funciones prototipo comunes
//============================
void configura(void);
int menu_principal(void);

//============================================================
// Funciones para Alta - Baja - Consulta - Listado de productos 
//============================================================

int alta_producto_v2(Producto**, int*, int*);
void listado_productos_v1_2(Producto*, int);
int baja_producto_v1_2(Producto*, int*);
int consulta_producto_v1_2(Producto*, int);

int posicion_producto_v1_2(Producto*, int, char*);

int modificacion_producto_v1_2(Producto*, int);
Producto* leer_fichero_productos_v2(int*, int*);
int escribir_fichero_productos_v1_2(Producto*, int);

//quitar
//int envia_producto_a_Arduino(Producto*, int);
//hasta aqui

// nuevo
void envia_producto_a_Arduino(void);
//fin nuevo


//quitar
//int enviar_datos(char*, int);
//hasta aqui

//nuevo
int enviar_datos(int);
//fin nuevo

//quitar
//char get_color(char*);
//hasta aqui


void leer_datos_producto(char, Producto*);
void mostrar_titulos_producto(void);
void mostrar_datos_producto(Producto);

//==================
// Función Principal
//==================
int main(void)
{
	Producto* producto = NULL;
	int n = 0;
	int longitud = 0;
	

	// Variables comunes
	int resultado;
	int opc;  // Opcion del meno principal seleccionada
	configura(); // Tareas de configuracion

	producto = leer_fichero_productos_v2(&n, &longitud);
	do // Bucle principal de la aplicaci�n
	{
		opc = menu_principal();
		resultado = -1;
		switch (opc)
		{
		case 1: resultado = alta_producto_v2(&producto, &n, &longitud); break;
		case 2: resultado = baja_producto_v1_2(producto, &n); break;
		case 3: resultado = modificacion_producto_v1_2(producto, n); break;
		case 4: consulta_producto_v1_2(producto, n); break;
		case 5: listado_productos_v1_2(producto, n); break;
			//quitar
		//case 6: envia_producto_a_Arduino(producto, n); break;
			//hasta aqui

			//nuevo
		case 6: envia_producto_a_Arduino(); break;
		//fin nuevo
		
		case 7:	break;
		}
		if (resultado == 0)
			escribir_fichero_productos_v1_2(producto, n);
		printf("\n\n");
	} while (opc != NUM_OPCIONES);
	return 0;
}

void configura(void)
{
	// Establece juego de caracteres castellano
	// Para que funcione hay que partir de un proyecto vac�o
	// No utilice la plantilla Aplicaci�n de consola C++
	setlocale(LC_ALL, "spanish");
}

int menu_principal(void)
{
	int opcion;
	char intro;
	do
	{
		printf("FABRICA DE COLORINES\n");
		printf("==============\n");
		printf("1 - Alta de producto\n");
		printf("2 - Baja de producto\n");
		printf("3 - Modificación producto\n");
		printf("4 - Consulta producto\n");
		printf("5 - Listado productos\n");
		printf("6 - Enviar producto a Fábrica de Colorines\n");
		printf("7 - Cerrar Aplicación\n");
		printf("Seleccione opción:");
		scanf_s("%d", &opcion);
		intro = getchar();
		if (opcion < 1 || opcion > NUM_OPCIONES)
			printf("\nOpción inexistente.\n\n");
	} while (opcion < 1 || opcion > NUM_OPCIONES);
	printf("\n");
	return opcion;
}

void leer_datos_producto(char operacion, Producto *producto)
{
	if (operacion == 'A')
	{
		printf("Alta de un nuevo producto\n");
		printf("=======================\n");
	}
	else
	{
		printf("Modificar producto\n");
		printf("==============================================\n");
	}

	if (operacion == 'A') // En el alta lee el login. En modificaci�n mantiene el anterior.
	{
		printf("Id:");
		gets_s(producto->id, LONGCAD);
	}
	printf("Color:");
	gets_s(producto->color, LONGCAD);
	printf("Cantidad:");
	int cantidad;
	scanf_s("%d", &cantidad); 
	producto->cantidad = (char)cantidad;
	getchar(); // Evitamos que se quede un '\n' en el buffer del teclado.
	return;
}

void mostrar_titulos_producto(void)
{
	const char titulos[][LONGCAD] = { "Id", "Color", "Cantidad" };
	int i;

	for (i = 0; i < 3; i++)
		printf("%-15s", titulos[i]);
	printf("\n");
	for (i = 0; i < 15 * 3; i++)
		printf("=");
	printf("\n");
}

void mostrar_datos_producto(Producto u)
{
	printf("%-15s%-15s%-15d", u.id, u.color , u.cantidad);
}

int alta_producto_v2(Producto** plista, int* pn, int* pl)
{
	int n = *pn;
	int l = *pl;
	Producto* lista = *plista;
	Producto* lista_old;
	int cod_error = 0;

	if (l == 0) // Si la longitud es cero hay que crear la lista por primera vez
	{
		lista = (Producto*)malloc(sizeof(Producto) * N); // Solicita memoria para los primeros N productos
		if (lista == NULL)
		{
			printf("No se ha podido reservar memoria\n");
			cod_error = -1;
		}
		else
			l = N; // Actualizamos tama�o de la lista
	}
	else
	{
		if (l == n) // Si la lista actual est� completa
		{
			lista_old = lista; // Salvaguardamos la lista actual por si realloc falla
			lista = (Producto*)realloc(lista, sizeof(Producto) * (l + N)); // Solicitamos un incremento de N
			if (lista == NULL) // No es posible el incremento
			{
				printf("No se ha podido incrementar el espacio para nuevos productos");
				lista = lista_old; // Recuperamos la lista 
				cod_error = -2;
			}
			else
				l = l + N; // Actualizamos tama�o de la lista
		}
	}

	if (cod_error == 0) // Si no hemos tenido problemas con la reserva de memoria
	{
		leer_datos_producto('A', lista + n); // Lee los datos del nuevo producto
		n++; // Incrementa la cantidad de productos
		*pn = n;  // Actualiza los valores de los argumentos pasados por referencia
		*pl = l;
		*plista = lista;
	}
	return cod_error;
}

Producto* leer_fichero_productos_v2(int* pnumero, int* plongitud)
{
	Producto* p; // Puntero a la lista de productos que sera creada
	FILE* fichero; // Puntero para manipular el fichero
	int num = 0; // Variable auxiliar para numero de productos le�dos
	int i, pos; // Variable bucle y posicion final cadena
	errno_t cod_error; // Codigo de error tras el proceso de apertura.
	char intro[2];

	cod_error = fopen_s(&fichero, "Productos.txt", "rt"); // Se intenta abrir el fichero de texto
	if (cod_error != 0)  // Si el fichero no se ha podido abrir
		p = NULL; // La lista estara vacia
	else  // Si el fichero ha podido abrirse 
	{
		fscanf_s(fichero, "%d", &num); // Se lee la cantidad de registros
		if (num == 0) // Si esa cantidad es cero
			p = NULL; // La lista estar� vac�a
		else  // Si hay registros para leer (seg�n el entero le�do)
		{
			p = (Producto*)malloc(sizeof(Producto) * num); // Se solicita memoria para todos ellos
			if (p == NULL) // Si no hay memoria suficiente
				printf("Memoria insuficiente para almacenar los datos del fichero\n");
			else // Si hay memoria suficiente
			{
				fgets(intro, 2, fichero); // Saltamos el intro que hay tras el n�mero (Ascii 10 y 13)
				for (i = 0; i < num; i++)  // Se leen los registros uno por uno
				{
					fgets((p + i)->id, LONGCAD, fichero);
					pos = strlen((p + i)->id);  // Calcula la longitud del nombre para ubicar el \n.
					(p + i)->id[pos - 1] = '\0';
					fgets((p + i)->color , LONGCAD, fichero);
					pos = strlen((p + i)->color);  // Calcula la longitud del nombre para ubicar el \n.
					(p + i)->color[pos - 1] = '\0';
					fscanf_s(fichero, "%d", &(p + i)->cantidad);
					fgets(intro, 2, fichero); // Saltamos el intro que hay tras el n�mero (Ascii 10 y 13)
				}
			}
		}
		fclose(fichero); // Se cierra el fichero
	}

	*pnumero = num;  // Se actualiza el n�mero de registros de la lista productos
	*plongitud = num; // Se actualiza la longitud de la lista
	return p;  // Se retorna la direcci�n de memoria donde comienza la lista de productos
}

void listado_productos_v1_2(Producto* producto, int n)
{
	int i;

	if (n == 0)
		printf("En este momento no hay productos en la aplicación\n");
	else
	{
		mostrar_titulos_producto();
		for (i = 0; i < n; i++) {
			mostrar_datos_producto(producto[i]);
			if (i != n - 1)
				printf("\n");
		}
	}
}

int consulta_producto_v1_2(Producto* u, int n)
{
	char id[LONGCAD];
	int posicion = -1;
	int cod_error = 0;

	printf("Introduzca id del producto a buscar:");
	gets_s(id, LONGCAD);
	posicion = posicion_producto_v1_2(u, n, id);
	if (posicion == -1)
	{
		printf("No existe ningún producto con ese id\n");
		cod_error = -1;
	}
	else
	{
		mostrar_titulos_producto();
		mostrar_datos_producto(u[posicion]);
	}
	return cod_error;
}

int modificacion_producto_v1_2(Producto* u, int n)
{
	char id[LONGCAD];
	int posicion = -1;
	int cod_error = 0;

	printf("Introduzca id del producto a buscar:");
	gets_s(id, LONGCAD);
	posicion = posicion_producto_v1_2(u, n, id);
	if (posicion == -1)
	{
		printf("No existe ningún producto con ese id\n");
		cod_error = -1;
	}
	else
	{
		mostrar_titulos_producto();
		mostrar_datos_producto(u[posicion]);
		printf("\n");
		Producto* nuevos_datos = (Producto*)malloc(sizeof(Producto));
		if (nuevos_datos != NULL) {
			leer_datos_producto('M', nuevos_datos);
			if (strcmp(nuevos_datos->color, "") != 0)
				strcpy_s(u[posicion].color, LONGCAD, nuevos_datos->color);
			if (nuevos_datos->cantidad != 0)
				u[posicion].cantidad = nuevos_datos->cantidad;
		//	free(nuevos_datos);
		}
	}
	return cod_error;
}

int baja_producto_v1_2(Producto* u, int* pn)
{
	char id[LONGCAD], respuesta[2];
	int encontrado = NO;
	int i, j;
	int n = *pn;
	int cod_error = 0;

	if (n == 0)
	{
		printf("La lista de productos está vacía\n");
		cod_error = -1;
	}
	else
	{
		printf("Introduzca id del producto a eliminar:");
		gets_s(id, LONGCAD);
		for (i = 0; i < n && encontrado == NO; i++)
			if (strcmp(id, u[i].id) == 0)
			{
				printf("Datos del producto encontrado:\n");
				mostrar_titulos_producto();
				mostrar_datos_producto(u[i]);
				printf("\nDesea borrar este producto (S/N):");
				gets_s(respuesta, 2);
				if (strcmp(respuesta, "S") == 0 || strcmp(respuesta, "s") == 0)
				{
					encontrado = SI;
					for (j = i; j < n - 1; j++)
						u[j] = u[j + 1];
					*pn = n - 1;
				}
				else
					cod_error = -1;
			}
		if (encontrado == NO)
		{
			printf("No se ha encontrado el producto\n");
			cod_error = -1;
		}
	}
	return cod_error;
}

int escribir_fichero_productos_v1_2(Producto* lista, int numero)
{
	int i;
	FILE* fichero;
	errno_t err;

	err = fopen_s(&fichero, "Productos.txt", "w");
	if (err == 0) // Si el fichero se ha podido crear
	{
		fprintf(fichero, "%d\n", numero); // Se graba en el fichero el n�mero de productos
		for (i = 0; i < numero; i++)
		{
			fprintf(fichero, "%s\n", (lista + i)->id);
			fprintf(fichero, "%s\n", (lista + i)->color);
			fprintf(fichero, "%d\n", (lista + i)->cantidad);
		}
		fclose(fichero);
	}
	else
		printf("Se ha producido un problema a la hora de grabar el fichero de productos\n");
	return err;
}

int posicion_producto_v1_2(Producto* u, int n, char* id)
{
	int i, posicion = -1;

	for (i = 0; i < n && posicion == -1; i++)
		if (strcmp(id, u[i].id) == 0)
			posicion = i;
	return posicion;
}
//quitar
/*
int envia_producto_a_Arduino(Producto* u, int n) {
	char id[LONGCAD];
	int posicion = -1;
	int cod_error = 0;
	
	printf("Introduzca id del producto a enviar:");
	gets_s(id, LONGCAD);
	posicion = posicion_producto_v1_2(u, n, id);
	if (posicion == -1)
	{
		printf("No existe ningún producto con ese id\n");
		cod_error = -1;
	}
	else
	{
		enviar_datos(u[posicion].color, u[posicion].cantidad);
		
	}
	return cod_error;
}
*/
//hasta aqui


//nuevo
void envia_producto_a_Arduino() {
    int numero;
	printf("Introduce la cantidad que quieras organizar: ");
	scanf_s("%d",&numero);
	enviar_datos(numero);
	}


//fin nuevo
#define ERROR_ENVIANDO -1
#define TRANSMITIDO 0
#define NO_CONECTA -1



//quitar

/*
int enviar_datos(char* color, int cantidad) {
	Serial* arduino = new Serial((char*) "COM3");
	char datos[3];
	datos[0] = get_color(color);
	datos[1] = (char)cantidad;
	datos[2] = NULL;
	bool estado = false;
	int codigo_resultado = ERROR_ENVIANDO;
	while (estado = arduino->IsConnected() && datos[0] != 0) {
		arduino->WriteData(datos, sizeof(datos) - 1);
		printf("\nConectado\n");
		Sleep(500);
		codigo_resultado = TRANSMITIDO;
	}
	if (!estado) {
		printf("\nConecta la placa\n");
		codigo_resultado = NO_CONECTA;
	}
	return codigo_resultado;
}*/

//hasta aqui



//nuevo
int enviar_datos(int numero) {
	Serial* arduino = new Serial((char*) "COM8");
	
	char datos[2];
	datos[0] = (char)numero;
	datos[1] = NULL;
	bool estado = false;
	int codigo_resultado = ERROR_ENVIANDO;
	if (estado = arduino->IsConnected() && datos[0] != 0) {
		arduino->WriteData(datos, sizeof(datos) - 1);
		printf("\nConectado\n");
		Sleep(500);
		codigo_resultado = TRANSMITIDO;
	}
	if (!estado) {
		printf("\nConecta la placa\n");
		codigo_resultado = NO_CONECTA;
	}
	return codigo_resultado;
}
//fin nuevo




//quitar


/*
#define AZUL 1
#define ROJO 2
#define AMARILLO 3
#define VERDE 4
#define ROSA 5
#define NARANJA 6

char get_color(char* color){
	_strupr_s(color, (size_t)(strlen(color)+1));
	const char* colores_validos[] = { "AZUL", "ROJO", "AMARILLO", "VERDE", "ROSA", "NARANJA" };
	for (int i = 0; i < sizeof(colores_validos); i++) {
		if (strcmp(color, colores_validos[i]) == 0) {
			return i + 1;
		}
	}
	return 0;	
}*/

//hasta aqui