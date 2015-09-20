#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define LENGTH_ITEMS 100
#define INT_T 0
#define STRING_T 1
#define DOUBLE_T 2

#define STR_TABLE "table"
#define STR_ORDER "order"
#define STR_CHAR "char"
#define STR_INT "int"
#define STR_DOUBLE "double"
#define DELIMITERS " []\t"

// Structs utilizadas-------------------------------------------------------------------------------------------------
typedef struct node{
	int order;
	int id;
	char *name;
	int size;
	struct node *next;
	struct node *previous;
}NODE;

typedef struct schema{
	char *name;
	int n_elements;
	struct node *sentry;
	int size;
}SCHEMA;

// Funcoes auxiliares-------------------------------------------------------------------------------------------------
char *my_get_line(FILE*, int*);
char **read_schema(int*);
SCHEMA *create_schema(void);
NODE *create_node(void);
void get_node(NODE*, char*);
void delete_node(NODE**);
void get_item(char**, FILE*, SCHEMA*);
void print_item(SCHEMA*, char**);

// Funcoes------------------------------------------------------------------------------------------------------------
SCHEMA *create_schema(void);
void get_schema(SCHEMA*);
void dump_schema(SCHEMA*);
void delete_schema(SCHEMA**);
void dump_data(SCHEMA*);

// Main---------------------------------------------------------------------------------------------------------------
int main(int argc, char *argv[]){

	int repeat, aux;
	char *input;

	SCHEMA *schema = create_schema();
	get_schema(schema);
	do{
		repeat = 1;
		input = my_get_line(stdin, &aux);

		if(strcmp(input, "dump_schema") == 0){
			dump_schema(schema);
		}else if(strcmp(input, "dump_data") == 0){
			dump_data(schema);
		}else if(strcmp(input, "exit") == 0){
			repeat = 0;
		}

		free(input);
	}while(repeat);

	delete_schema(&schema);

	return 0;
}

// Funcoes internas do TAD--------------------------------------------------------------------------------------------
NODE *create_node(void){
	NODE *node = (NODE*)malloc(sizeof(NODE));

	if(node != NULL){
		node->id = -1;
		node->name = NULL;
		node->size = 0;
		node->next = NULL;
		node->previous = NULL;
		node->order = 0;
	}

	return node;
}

void delete_node(NODE **node){
	if(node != NULL && (*node) != NULL){
		if((*node)->name != NULL) free((*node)->name);
		free(*node);
		(*node) = NULL;
	}
}

void get_node(NODE *node, char *line){
	char *aux;
	aux = strtok(line, DELIMITERS);
	node->name = strdup(aux);
	aux = strtok(NULL, DELIMITERS);
	if(strcmp(aux, STR_INT) == 0){
		node->id = INT_T;
		node->size = sizeof(int);
	}else if(strcmp(aux, STR_DOUBLE) == 0){
		node->id = DOUBLE_T;
		node->size = sizeof(double);
	}else if(strcmp(aux, STR_CHAR) == 0){
		node->id = STRING_T;
		aux = strtok(NULL, DELIMITERS);
		node->size = atoi(aux) * sizeof(char);
	}
	aux = strtok(NULL, DELIMITERS);

	if(aux != NULL && strcmp(aux, "order") == 0) node->order = 1;
}

void get_item(char **item, FILE *stream, SCHEMA *schema){

	int aux_int;
	double aux_double;

	NODE *aux = schema->sentry;
	int i;
	for(i = 0; i < schema->n_elements; i++){

		aux = aux->next;

		if(aux->id == INT_T){
			item[i] = (char*)realloc(item[i], LENGTH_ITEMS * sizeof(char));
			fread(&aux_int, aux->size, 1, stream);
			snprintf(item[i], LENGTH_ITEMS, "%d", aux_int);
		}else if(aux->id == STRING_T){
			item[i] = (char*)realloc(item[i], aux->size);
			fread(item[i], aux->size, 1, stream);
		}else if(aux->id == DOUBLE_T){
			item[i] = (char*)realloc(item[i], LENGTH_ITEMS * sizeof(char));
			fread(&aux_double, aux->size, 1, stream);
			snprintf(item[i], LENGTH_ITEMS, "%.2lf", aux_double);
		}
	}
}

void print_item(SCHEMA *schema, char **item){

	NODE *aux = schema->sentry;
	int i;
	for(i = 0; i < schema->n_elements; i++){
		aux = aux->next;
		printf("%s = %s\n", aux->name, item[i]);
	}
}

// Funcoes utilizadas do TAD------------------------------------------------------------------------------------------

SCHEMA *create_schema(void){
	SCHEMA *schema = (SCHEMA*)malloc(sizeof(SCHEMA));

	if(schema != NULL){
		NODE *sentry = create_node();
		if(sentry != NULL){
			sentry->next = sentry;
			sentry->previous = sentry;
			schema->name = NULL;
			schema->n_elements = 0;
			schema->sentry = sentry;
			schema->size = 0;
		}else{
			free(schema);
			schema = NULL;
		}
	}

	return schema;
}

void get_schema(SCHEMA *schema){
	if(schema != NULL){
		NODE *new_node;
		int n_elements, i;
		char *aux;
		char **table = read_schema(&n_elements);

		aux = strtok(table[0], DELIMITERS);
		aux = strtok(NULL, DELIMITERS);
		schema->name = strdup(aux);
		schema->n_elements = n_elements;
		schema->size = 0;

		for(i = 1; i <= n_elements; i++){
			new_node = create_node();
			if(new_node == NULL){
				fprintf(stderr, "error creating node\n");
				exit(3);
			}

			get_node(new_node, table[i]);
			schema->size += new_node->size;

			new_node->next = schema->sentry;
			new_node->previous = schema->sentry->previous;
			schema->sentry->previous->next = new_node;
			schema->sentry->previous = new_node;
		}

		for(i = 0; i <= n_elements; i++){
			free(table[i]);
		}
		free(table);
	}
}

void delete_schema(SCHEMA **schema){
	if(schema != NULL && (*schema) != NULL){
		NODE *aux;
		while((*schema)->n_elements > 0){
			aux = (*schema)->sentry->previous;
			aux->next->previous = aux->previous;
			aux->previous->next = aux->next;
			delete_node(&aux);
			(*schema)->n_elements--;
		}
		delete_node(&((*schema)->sentry));
		if((*schema)->name != NULL) free((*schema)->name);
		free(*schema);
		(*schema) = NULL;
	}
}

void dump_schema(SCHEMA *schema){
	if(schema != NULL){
		int i;
		NODE *aux = schema->sentry;

		printf("%s %s(%d bytes)\n", STR_TABLE, schema->name, schema->size);
		for(i = 0; i < schema->n_elements; i++){
			aux = aux->next;
			if(aux->id == INT_T){
				printf("%s %s", aux->name, STR_INT);
			}else if(aux->id == DOUBLE_T){
				printf("%s %s",aux->name, STR_DOUBLE);
			}else if(aux->id == STRING_T){
				printf("%s %s[%d]", aux->name, STR_CHAR, (aux->size/(int)sizeof(char)));
			}
			if(aux->order) printf(" %s", STR_ORDER);
			printf("(%d bytes)\n", aux->size);
		}
	}
}

void dump_data(SCHEMA *schema){
	
	int i, n_elements;
	long int aux;
	char **item = (char**)malloc(schema->n_elements * sizeof(char*));
	char *filename = (char*)malloc(sizeof(char) * (strlen(schema->name)+6));
	filename = strcpy(filename, schema->name);
	filename = strcat(filename, ".data");
	FILE *fp = fopen(filename, "rb");
	if(fp == NULL){
		fprintf(stderr, "could not open file\n");
		exit(2);
	}

	fseek(fp, 0, SEEK_END);
	aux = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	n_elements = aux / schema->size;

	for(i = 0; i < schema->n_elements; i++){
		item[i] = NULL;
	}

	for(i = 0; i < n_elements; i++){
		get_item(item, fp, schema);
		print_item(schema, item);
	}

	for(i = 0; i < schema->n_elements; i++){
		if(item[i] != NULL) free(item[i]);
	}
	free(item);
	free(filename);
	fclose(fp);
}

// Funcoes de leitura-------------------------------------------------------------------------------------------------

char *my_get_line(FILE *stream, int *ending){
	char *string = NULL;
	int input, size = 0;

	do{
		input = fgetc(stream);
	}while((char)input != '\n' && input != EOF && iscntrl(input));

	if((char)input == '\n' || input == EOF){   // Chamada da função em local invalido do arquivo:
		if(input == '\n') (*ending) = 1;   // -linha vazia
		else (*ending) = -2;               // -final do arquivo
		return NULL;
	}

	do{
		string = (char*)realloc(string, (size+1) * sizeof(char));
		string[size++] = (char)input;
		input = fgetc(stream);
	}while(input != '\n' && input != EOF);
	string = (char*)realloc(string, (size+1) * sizeof(char));
	string[size] = '\0';

	if(input == '\n') (*ending) = 0;
	else (*ending) = -1;

	return string;
}

char **read_schema(int *n_elements){
	int ending, size = 0;
	FILE *fp;
	char *aux, *filename;
	char **table = NULL;

	filename = my_get_line(stdin, &ending);
	fp = fopen(filename, "r");
	if(fp == NULL){
		fprintf(stderr, "could not open file\n");
		free(filename);
		exit(1);
	}

	do{
		aux = my_get_line(fp, &ending);

		if(aux != NULL){
			table = (char**)realloc(table, (size+1) * sizeof(char*));
			table[size++] = aux;
		}
	}while(ending >= 0); // Se ending eh positivo, ainda nao foi atingido o fim do arquivo

	(*n_elements) = size-1;

	// Precaucao
	if(size == 0){
		fprintf(stderr, "error reading .schema\n");
		exit(2);
	}

	fclose(fp);
	free(filename);
	return table;
}
