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
	long int size;
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
char *my_gets(FILE*, int);
char **read_schema(int*);
SCHEMA *create_schema(void);
NODE *create_node(void);
void get_node(NODE*, char*);
void delete_node(NODE**);
void get_item(char**, FILE*, SCHEMA*);
void print_item(SCHEMA*, char**);
void copy_data(FILE*, FILE*, long int, SCHEMA*, NODE*);
void swap(FILE*, NODE*, int, int);
int compare_in_file(FILE*, NODE*, int, int);
long int sequential_search(FILE*, SCHEMA*, NODE*, char*, int*, long int);
long int binary_search(FILE*, SCHEMA*, NODE*, char*, int, int, int*);
int compare_outside(NODE*, void*, char*);


// Funcoes------------------------------------------------------------------------------------------------------------
SCHEMA *create_schema(void);
void get_schema(SCHEMA*);
void dump_schema(SCHEMA*);
void delete_schema(SCHEMA**);
void dump_data(SCHEMA*);
void get_index(SCHEMA*);
void print_index(SCHEMA*);
void sort_index(SCHEMA*);
void insert_data(SCHEMA*);
void search_index_data(SCHEMA*);

// Main---------------------------------------------------------------------------------------------------------------
int main(int argc, char *argv[]){

	int repeat, aux;
	char *input;

	SCHEMA *schema = create_schema();
	get_schema(schema);

	do{
		repeat = 1;
		input = my_get_line(stdin, &aux);
		if(input == NULL) fprintf(stderr, "chamada errada\n");

		if(strcmp(input, "dump_schema") == 0){
			dump_schema(schema);
		}else if(strcmp(input, "dump_data") == 0){
			dump_data(schema);
		}else if(strcmp(input, "dump_index") == 0){
			print_index(schema);
		}else if(strcmp(input, "update_index") == 0){
			get_index(schema);
			sort_index(schema);
		}else if(strcmp(input, "insert") == 0){
			insert_data(schema);
		}else if(strcmp(input, "select") == 0){
			search_index_data(schema);
		}else if(strcmp(input, "exit") == 0){
			repeat = 0;
		}

		if(input != NULL) free(input);
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

void swap(FILE *fp, NODE *node, int i, int j){

	long int data_size = (node->size + sizeof(long int));
	void *aux1 = malloc(data_size);
	void *aux2 = malloc(data_size);

	fseek(fp, i * data_size, SEEK_SET);
	fread(aux1, data_size, 1, fp);
	fseek(fp, j * data_size, SEEK_SET);
	fread(aux2, data_size, 1, fp);
	fseek(fp, j * data_size, SEEK_SET);
	fwrite(aux1, data_size, 1, fp);
	fseek(fp, i * data_size, SEEK_SET);
	fwrite(aux2, data_size, 1, fp);

	free(aux1);
	free(aux2);
}

int compare_in_file(FILE *fp, NODE *node, int i, int j){

	long int data_size = (node->size + sizeof(long int));
	int result = 0;
	void *aux1 = malloc(node->size);
	void *aux2 = malloc(node->size);

	fseek(fp, i * data_size, SEEK_SET);
	fread(aux1, node->size, 1, fp);
	fseek(fp, j * data_size, SEEK_SET);
	fread(aux2, node->size, 1, fp);


	if(node->id == INT_T){
		if( (*((int*)aux1)) < (*((int*)aux2)) ) result = -1;
		else if( (*((int*)aux1)) > (*((int*)aux2)) ) result = 1;
	}else if(node->id == DOUBLE_T){
		if( (*((double*)aux1)) < (*((double*)aux2)) ) result = -1;
		else if( (*((double*)aux1)) > (*((double*)aux2)) ) result = 1;
	}else if(node->id == STRING_T){
		result = strcmp((char*)aux1, (char*)aux2);
	}

	free(aux1);
	free(aux2);

	return result;
}

int compare_outside(NODE *node, void *check, char *key){
	int result = 0;

	if(node->id == INT_T){
		if( (*((int*)check)) > atoi(key) ) result = -1;
		else if( (*((int*)check)) < atoi(key) ) result = 1;
	}else if(node->id == DOUBLE_T){
		if( (*((double*)check)) > atof(key) ) result = -1;
		else if( (*((double*)check)) < atof(key) ) result = 1;
	}else if(node->id == STRING_T){
		result = strcmp(key, (char*)check);
	}

	return result;
}

long int sequential_search(FILE *fp, SCHEMA *schema, NODE *node, char *search_key, int *test_count, long int offset){
	char *filename_data, *filename_index;
	FILE *fp_data, *fp_index;
	int i, n_elements_data, n_elements_index, compare_result;
	long int end_file;
	void *aux;
	
	filename_data = (char*)malloc(sizeof(char) * (strlen(schema->name)+6));
	strcpy(filename_data, schema->name);
	strcat(filename_data, ".data");
	fp_data = fopen(filename_data, "rb");
	if(fp_data == NULL){
		fprintf(stderr, "could not open file\n");
		exit(1);
	}
	fseek(fp_data, 0, SEEK_END);
	end_file = ftell(fp_data);
	fseek(fp_data, 0, SEEK_SET);
	n_elements_data = (int)end_file/schema->size;

	filename_index = (char*)malloc(sizeof(char) * (strlen(schema->name) + 6 + strlen(node->name)));
	strcpy(filename_index, schema->name);
	strcat(filename_index, "-");
	strcat(filename_index, node->name);
	strcat(filename_index, ".idx");
	fp_index = fopen(filename_index, "rb");
	fseek(fp_index, 0, SEEK_END);
	end_file = ftell(fp_index);
	fseek(fp_index, 0, SEEK_SET);
	n_elements_index = (int)(end_file/(node->size+sizeof(long int)));

	aux = malloc(node->size);
	i = n_elements_index;
	compare_result = 1;

	while(i < n_elements_data && compare_result != 0){
		(*test_count)++;
		fseek(fp_data, (i*schema->size)+offset, SEEK_SET);
		fread(aux, node->size, 1, fp_data);
		compare_result = compare_outside(node, aux, search_key);
		i++;
	}
	free(filename_data);
	free(filename_index);
	free(aux);
	fclose(fp_data);
	fclose(fp_index);

	if(compare_result == 0) return ((i-1)*schema->size);
	else return -1;
}

long int binary_search(FILE *fp, SCHEMA *schema, NODE *node, char *search_key, int begin, int end, int *test_count){

	if(begin > end) return -1;

	int middle = (begin+end)/2, compare_result;
	long int result;
	void *aux = malloc(node->size);
	fseek(fp, middle*(node->size + sizeof(long int)), SEEK_SET);
	fread(aux, node->size, 1, fp);

	compare_result = compare_outside(node, aux, search_key);
	(*test_count)++;
	free(aux);
	if(compare_result < 0){
		result = binary_search(fp, schema, node, search_key, begin, middle-1, test_count);
	}else if(compare_result > 0){
		result = binary_search(fp, schema, node, search_key, middle+1, end, test_count);
	}else{
		fread(&result, sizeof(long int), 1, fp);
	}
	return result;
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
	}else{
		fprintf(stderr, "error creating schema\n");
		exit(3);
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

		get_index(schema);
		sort_index(schema);
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
				printf("%s %s[%ld]", aux->name, STR_CHAR, (aux->size/(int)sizeof(char)));
			}
			if(aux->order) printf(" %s", STR_ORDER);
			printf("(%ld bytes)\n", aux->size);
		}
	}
}

void dump_data(SCHEMA *schema){
	
	int i, n_elements;
	long int aux;
	char **item = (char**)malloc(schema->n_elements * sizeof(char*));
	char *filename = (char*)malloc(sizeof(char) * (strlen(schema->name)+6));
	strcpy(filename, schema->name);
	strcat(filename, ".data");
	FILE *fp = fopen(filename, "rb");
	if(fp == NULL){
		fprintf(stderr, "could not open file\n");
		exit(1);
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

void get_index(SCHEMA *schema){

	int i;
	long int cur_offset = 0;
	NODE *aux = schema->sentry;
	char *filename_index, *filename_data;
	FILE *fp_data, *fp_index;

	for(i = 0; i < schema->n_elements; i++){
		aux = aux->next;
		if(aux->order){
			filename_data = (char*)malloc(sizeof(char) * (strlen(schema->name)+6));
			strcpy(filename_data, schema->name);
			strcat(filename_data, ".data");
			fp_data = fopen(filename_data, "rb");
			if(fp_data == NULL){
				fprintf(stderr, "could not open file\n");
				exit(1);
			}

			filename_index = (char*)malloc(sizeof(char) * (strlen(schema->name) + 6 + strlen(aux->name)));
			strcpy(filename_index, schema->name);
			strcat(filename_index, "-");
			strcat(filename_index, aux->name);
			strcat(filename_index, ".idx");
			fp_index = fopen(filename_index, "wb");
			if(fp_index == NULL){
				fprintf(stderr, "error creating file");
				exit(1);
			}

			copy_data(fp_data, fp_index, cur_offset, schema, aux);

			if(fp_data != NULL) fclose(fp_data);
			if(fp_index != NULL) fclose(fp_index);
			if(filename_data != NULL) free(filename_data);
			if(filename_index != NULL) free(filename_index);
		}

		cur_offset += aux->size;
	}
}

void print_index(SCHEMA *schema){

	int i, j, n_elements;
	long int location;
	void *aux;
	char *filename_index;
	NODE *node = schema->sentry;
	FILE *fp_index;

	for(i = 0; i < schema->n_elements; i++){
		node = node->next;
		if(node->order){
			filename_index = (char*)malloc(sizeof(char) * (strlen(schema->name) + 6 + strlen(node->name)));
			strcpy(filename_index, schema->name);
			strcat(filename_index, "-");
			strcat(filename_index, node->name);
			strcat(filename_index, ".idx");
			fp_index = fopen(filename_index, "rb");

			if(fp_index != NULL){
				fseek(fp_index, 0, SEEK_END);
				location = ftell(fp_index);
				n_elements = (int)(location/(sizeof(long int) + node->size));
				fseek(fp_index, 0, SEEK_SET);

				for(j = 0; j < n_elements; j++){
					aux = malloc(node->size);
					fread(aux, node->size, 1, fp_index);
					fread(&location, sizeof(long int), 1, fp_index);

					if(node->id == INT_T){
						printf("%d = %ld\n", *((int*)aux), location);
					}else if(node->id == DOUBLE_T){
						printf("%.2lf = %ld\n", *((double*)aux), location);
					}else if(node->id == STRING_T){
						printf("%s = %ld\n", (char*)aux, location);
					}
					free(aux);
				}
				fclose(fp_index);
			}

			free(filename_index);
		}
	}
}

void sort_index(SCHEMA *schema){

	int i, j, k, n_elements;
	long int location;
	char *filename_index;
	NODE *node = schema->sentry;
	FILE *fp_index;

	for(i = 0; i < schema->n_elements; i++){
		node = node->next;
		if(node->order){
			filename_index = (char*)malloc(sizeof(char) * (strlen(schema->name) + 6 + strlen(node->name)));
			strcpy(filename_index, schema->name);
			strcat(filename_index, "-");
			strcat(filename_index, node->name);
			strcat(filename_index, ".idx");
			fp_index = fopen(filename_index, "r+b");

			if(fp_index != NULL){
				fseek(fp_index, 0, SEEK_END);
				location = ftell(fp_index);
				n_elements = (int)(location/(sizeof(long int) + node->size));
				fseek(fp_index, 0, SEEK_SET);

				for(j = 1; j < n_elements; j++){

					for(k = j-1; (k >= 0) && (compare_in_file(fp_index, node, k, k+1) > 0); k--){
						swap(fp_index, node, k, k+1);
					}

				}
				fclose(fp_index);
			}

			free(filename_index);
		}
	}
}

void insert_data(SCHEMA *schema){

	int i;
	void *aux;
	NODE *node = schema->sentry;
	char *filename = (char*)malloc(sizeof(char) * (strlen(schema->name)+6));
	strcpy(filename, schema->name);
	strcat(filename, ".data");
	FILE *fp_data = fopen(filename, "r+b");
	if(fp_data == NULL){
		fprintf(stderr, "could not open file\n");
		exit(1);
	}

	fseek(fp_data, 0, SEEK_END);
	for(i = 0; i < schema->n_elements; i++){
		node = node->next;
		aux = malloc(node->size);

		if(node->id == INT_T){
			scanf("%d", (int*)aux);
		}else if(node->id == DOUBLE_T){
			scanf("%lf", (double*)aux);
		}else if(node->id == STRING_T){
			free(aux);
			aux = (void*)my_gets(stdin, (node->size/sizeof(char)));
		}
		fwrite(aux, node->size, 1, fp_data);
		if(aux != NULL) free(aux);
	}
	free(filename);
	fclose(fp_data);
}

void search_index_data(SCHEMA *schema){

	int i, test_count, search_return, n_elements;
	long int location, offset;
	void *aux;
	char *filename_index, *search_term, *filename_data, *print_field, *search_key;
	NODE *node = schema->sentry;
	FILE *fp_index, *fp_data;

	search_term = my_get_line(stdin, &i);
	search_key = my_get_line(stdin, &i);
	print_field = my_get_line(stdin, &i);

	test_count = 0;
	offset = 0;
	for(i = 0; i < schema->n_elements; i++){
		node = node->next;
		if(strcmp(search_term, node->name) == 0 && node->order){

			filename_index = (char*)malloc(sizeof(char) * (strlen(schema->name) + 6 + strlen(node->name)));
			strcpy(filename_index, schema->name);
			strcat(filename_index, "-");
			strcat(filename_index, node->name);
			strcat(filename_index, ".idx");
			fp_index = fopen(filename_index, "rb");

			if(fp_index != NULL){

				fseek(fp_index, 0, SEEK_END);
				location = ftell(fp_index);
				n_elements = (int)(location/(sizeof(long int) + node->size));
				fseek(fp_index, 0, SEEK_SET);

				search_return = binary_search(fp_index, schema, node, search_key, 0, n_elements-1, &test_count);
				fclose(fp_index);
				if(search_return == -1){

					filename_data = (char*)malloc(sizeof(char) * (strlen(schema->name)+6));
					strcpy(filename_data, schema->name);
					strcat(filename_data, ".data");
					fp_data = fopen(filename_data, "rb");
					if(fp_data == NULL){
						fprintf(stderr, "could not open file\n");
						exit(1);
					}

					search_return = sequential_search(fp_data, schema, node, search_key, &test_count, offset);
					fclose(fp_data);
					free(filename_data);
				}
			}else{
				fprintf(stderr, "could not open file\n");
				exit(1);
			}

			free(filename_index);

			printf("%d\n", test_count);
			if(search_return == -1){
				printf("value not found\n");
			}else{
				node = schema->sentry;
				offset = 0;
				for(i = 0; i < schema->n_elements; i++){
					node = node->next;
					if(strcmp(print_field, node->name) == 0){
						break;
					}
					offset += node->size;
				}

				filename_data = (char*)malloc(sizeof(char) * (strlen(schema->name)+6));
				strcpy(filename_data, schema->name);
				strcat(filename_data, ".data");
				fp_data = fopen(filename_data, "r+b");
				if(fp_data == NULL){
					fprintf(stderr, "could not open file\n");
					exit(1);
				}

				fseek(fp_data, search_return+offset, SEEK_SET);
				aux = malloc(node->size);
				fread(aux, node->size, 1, fp_data);
				fclose(fp_data);

				if(node->id == INT_T){
					printf("%d\n", *((int*)aux));
				}else if(node->id == DOUBLE_T){
					printf("%.2lf\n", *((double*)aux));
				}else if(node->id == STRING_T){
					printf("%s\n", (char*)aux);
				}
				free(aux);
				free(filename_data);
			}
			i = schema->n_elements+1;
		}
		if(i <= schema->n_elements){
			offset += node->size;
		}
	}

	if(test_count == 0){
		printf("index not found\n");
	}
	free(search_term);
	free(search_key);
	free(print_field);
}

// Funcoes de leitura-------------------------------------------------------------------------------------------------

char *my_get_line(FILE *stream, int *ending){
	char *string = NULL;
	int input, size = 0;

	do{
		input = fgetc(stream);
	}while((char)input != '\n' && input != EOF && !isalnum(input));

	if(/*(char)input == '\n' || */input == EOF){   // Chamada da função em local invalido do arquivo:
	//	if(input == '\n') (*ending) = 1;       // -linha vazia
		/*else*/ (*ending) = -2;               // -final do arquivo
		return NULL;
	}
	while(input == '\n') input = fgetc(stream);

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

char *my_gets(FILE *stream, int str_size){
	char *string = (char*)calloc(str_size, 1);
	int input, cur_size = 0;

	do{
		input = fgetc(stream);
	}while( ((char)input == ' ' || (char)input == '\n') && input != EOF);
	if(input == EOF){
		fprintf(stderr, "error reading string");
		exit(4);
	}

	do{
		string[cur_size++] = (char)input;
		input = fgetc(stream);
	}while((char)input != '\n' && input != EOF && cur_size < str_size-1);

	string[cur_size] = '\0';
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

void copy_data(FILE *origin, FILE *destiny, long int cur_offset, SCHEMA *schema, NODE *type){

	long int location;
	int i, n_elements;
	int data_size = (int)sizeof(long int) + type->size;
	void *aux = malloc(data_size);

	fseek(origin, 0, SEEK_END);
	location = ftell(origin);
	n_elements = (int)(location/schema->size);
	fseek(origin, 0, SEEK_SET);

	for(i = 0; i < n_elements; i++){
		fseek(origin, (i * schema->size) + cur_offset, SEEK_SET);
		fread(aux, type->size, 1, origin);
		location = (long int)(i * schema->size);
		memcpy(aux+(type->size), &location, sizeof(long int));
		fwrite(aux, data_size, 1,destiny);
	}

	if(aux != NULL) free(aux);
}
