/*
		Trabalho 3: Base de Dados

			Italo Tobler Silva - nUSP 8551910		17/09/2015

	OBS: Recomendável para a leitura do codigo manter aberto uma copia dos defines e dos conteudos das structs
	para facilitar consulta
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Length items é o numero maximo de digitos dos numeros lidos
#define LENGTH_ITEMS 100
// Esses sao os identificados usados para cada tipo de dado manipulado
#define INT_T 0
#define STRING_T 1
#define DOUBLE_T 2

/*
	Essas strings sao definidas para o caso do usuario desejar mudar o identificador dos tipos de dado ou do nome dos arquivos
   ou do indicador de ordenacao. A string DELIMITERS é foi criada para o uso da função strtok
*/
#define STR_TABLE "table"
#define STR_ORDER "order"
#define STR_CHAR "char"
#define STR_INT "int"
#define STR_DOUBLE "double"
#define DELIMITERS " []\t"

/*
	As informações do arquivo .schema são armazenadas como se fizessem parte de uma lista duplamente ligada onde cada nó contém
   as informações necessárias para serem feitas operações de comparação, impressão e análise de tamanho
*/
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
/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
char *my_get_line(FILE*, int*);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
char *my_gets(FILE*, int);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
char **read_schema(int*);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
SCHEMA *create_schema(void);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
NODE *create_node(void);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void get_node(NODE*, char*);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void delete_node(NODE**);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void get_item(char**, FILE*, SCHEMA*);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void print_item(SCHEMA*, char**);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void copy_data(FILE*, FILE*, long int, SCHEMA*, NODE*);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void swap(FILE*, NODE*, int, int);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
int compare_in_file(FILE*, NODE*, int, int);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
long int sequential_search(SCHEMA*, NODE*, char*, int*, long int);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
long int binary_search(FILE*, SCHEMA*, NODE*, char*, int, int, int*);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
int compare_outside(NODE*, void*, char*);


// Funcoes------------------------------------------------------------------------------------------------------------
/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
SCHEMA *create_schema(void);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void get_schema(SCHEMA*);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void dump_schema(SCHEMA*);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void delete_schema(SCHEMA**);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void dump_data(SCHEMA*);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void get_index(SCHEMA*);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void print_index(SCHEMA*);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void sort_index(SCHEMA*);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void insert_data(SCHEMA*);

/*------------------------------------------------------------------------------------------------------------
   Nome()
        Descricao
        - Parametros
          tipo : descricao
        - Retorno
          tipo : descricao
*/
void search_index_data(SCHEMA*);

// Main---------------------------------------------------------------------------------------------------------------
int main(int argc, char *argv[]){

	int repeat, aux;
	char *input;

	// A lista schema é criada e lida da stdin
	SCHEMA *schema = create_schema();
	get_schema(schema);

	do{
		// A cada repeticao le um comando da stdin
		repeat = 1;
		input = my_get_line(stdin, &aux);
		if(input == NULL) fprintf(stderr, "chamada errada\n");

		// Analisa qual o comando desejado e chama a(s) funcao(oes) responsavel(is) por realiza-lo
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
			// Caso seja digitado "exit", repeat recebe 0, saindo do loop
			repeat = 0;
		}

		// A cada repeticao input eh liberado caso tenha sido alocado adequadamente
		if(input != NULL) free(input);
	}while(repeat);

	// Libera a memoria alocada
	delete_schema(&schema);

	return 0;
}

// Funcoes internas do TAD--------------------------------------------------------------------------------------------
NODE *create_node(void){
	// Aloca memoria
	NODE *node = (NODE*)malloc(sizeof(NODE));

	// Checa se foi criada adequadamente
	if(node != NULL){
		// Inicializa valores
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
	// Checa se eh valido
	if(node != NULL && (*node) != NULL){
		// Aloca os campos que podem ter memoria alocada
		if((*node)->name != NULL) free((*node)->name);
		// Libera a memoria e coloca NULL no lugar do endereco liberado
		free(*node);
		(*node) = NULL;
	}
}

void get_node(NODE *node, char *line){
	char *aux;
	// Aux recebe o primeiro token da linha
	aux = strtok(line, DELIMITERS);
	// Esse token corresponde ao nome do elemento
	node->name = strdup(aux);
	// O segundo token corresponde ao tipo de dado do elemento
	aux = strtok(NULL, DELIMITERS);
	// Deve ser analizado qual o tipo para decidir o proximo passo
	if(strcmp(aux, STR_INT) == 0){
		// No caso de int e double, salva-se o id correspondente ao tipo e o tamanho
		node->id = INT_T;
		node->size = sizeof(int);
	}else if(strcmp(aux, STR_DOUBLE) == 0){
		node->id = DOUBLE_T;
		node->size = sizeof(double);
	}else if(strcmp(aux, STR_CHAR) == 0){
		// Caso seja um char[*], outro strtok deve ser chamado para identificar o numero de caracteres da string
		node->id = STRING_T;
		aux = strtok(NULL, DELIMITERS);
		// O numero de caracteres fica salvo no tamanho do elemento, pois sempre um produto do tamanho por sizeof(char)
		node->size = atoi(aux) * sizeof(char);
	}
	// Mais uma chamada de strtok analisa se o elemento deve ser ordernado ou nao
	aux = strtok(NULL, DELIMITERS);

	if(aux != NULL && strcmp(aux, "order") == 0) node->order = 1;
}

void get_item(char **item, FILE *stream, SCHEMA *schema){

	int aux_int;
	double aux_double;

	NODE *aux = schema->sentry;
	int i;
	for(i = 0; i < schema->n_elements; i++){
		// Esse loop percorre todos os elementos salvos no schema e todas as linhas da tabela char**
		aux = aux->next;

		// Analisa qual o tipo de dado do elemento atual
		if(aux->id == INT_T){
			// O define LENGTH_ITEMS guarda o numero maximo de digitos a serem lidos por vez
			item[i] = (char*)realloc(item[i], LENGTH_ITEMS * sizeof(char));
			// O inteiro eh lido do arquivo e salvo numa variavel auxiliar
			fread(&aux_int, aux->size, 1, stream);
			// E dpois eh convertido para uma string salva em item[i]
			snprintf(item[i], LENGTH_ITEMS, "%d", aux_int);
		}else if(aux->id == STRING_T){
			// No caso de strings, a memoria eh copiada diretamente para item[i]
			item[i] = (char*)realloc(item[i], aux->size);
			fread(item[i], aux->size, 1, stream);
		}else if(aux->id == DOUBLE_T){
			// Para elementos do tipo double o procedimento eh semelhante ao de inteiros
			item[i] = (char*)realloc(item[i], LENGTH_ITEMS * sizeof(char));
			fread(&aux_double, aux->size, 1, stream);
			snprintf(item[i], LENGTH_ITEMS, "%.2lf", aux_double);
		}
	}
}

void print_item(SCHEMA *schema, char **item){

	// aux armazena os detalhes do elemento sendo imprimido
	NODE *aux = schema->sentry;
	int i;
	for(i = 0; i < schema->n_elements; i++){
		aux = aux->next;
		// Para cada elemento imprime o tipo e o que foi convertido para string com get_item
		printf("%s = %s\n", aux->name, item[i]);
	}
}

void swap(FILE *fp, NODE *node, int i, int j){

	// Data_size guarda o tamanho do bloco de memoria que contem um item e um long int de offset para cada item no .idx
	long int data_size = (node->size + sizeof(long int));
	void *aux1 = malloc(data_size);
	void *aux2 = malloc(data_size);

	// A variavel aux1 armazena o elemento na "posicao i" dentro do arquivo
	fseek(fp, i * data_size, SEEK_SET);
	fread(aux1, data_size, 1, fp);
	// A variavel aux2 armazena o elemento na "posicao j"
	fseek(fp, j * data_size, SEEK_SET);
	fread(aux2, data_size, 1, fp);
	// Os conteudos de cada posicao sao reescritos com o conteudo anterior da outra posicao
	fseek(fp, j * data_size, SEEK_SET);
	fwrite(aux1, data_size, 1, fp);
	fseek(fp, i * data_size, SEEK_SET);
	fwrite(aux2, data_size, 1, fp);

	// A memoria alocada eh liberada
	free(aux1);
	free(aux2);
}

int compare_in_file(FILE *fp, NODE *node, int i, int j){

	// Assim como em swap(), data_size armazena o tamanho de cada elemento salvo no .idx
	long int data_size = (node->size + sizeof(long int));
	int result = 0;
	// As variaveis aux dessa vez armazenarao apenas o conteudo, ignorando o offset
	void *aux1 = malloc(node->size);
	void *aux2 = malloc(node->size);

	// O arquivo eh lido para obter o conteudo das "posicoes" i e j
	fseek(fp, i * data_size, SEEK_SET);
	fread(aux1, node->size, 1, fp);
	fseek(fp, j * data_size, SEEK_SET);
	fread(aux2, node->size, 1, fp);


	// De acordo com o tipo de dado sendo analisado, a comparacao eh feita de uma maneira diferente
	if(node->id == INT_T){
		if( (*((int*)aux1)) < (*((int*)aux2)) ) result = -1;
		else if( (*((int*)aux1)) > (*((int*)aux2)) ) result = 1;
	}else if(node->id == DOUBLE_T){
		if( (*((double*)aux1)) < (*((double*)aux2)) ) result = -1;
		else if( (*((double*)aux1)) > (*((double*)aux2)) ) result = 1;
	}else if(node->id == STRING_T){
		result = strcmp((char*)aux1, (char*)aux2);
	}
	// Independente do tipo, o resultado é semelhante em quesito de ordenacao ao retorno da funcao strcmp()
	// negativo quer dizer i e j estão crescentes, positivo decrescente e 0 igual

	// A memoria alocada eh liberada e o resultado da analise eh retornado
	free(aux1);
	free(aux2);

	return result;
}

int compare_outside(NODE *node, void *check, char *key){
	// valor de result eh inicializado para o caso de igualdade
	int result = 0;

	// Para cada tipo de dado desejado o char* e o void* sao convertidos de acordo para permitir a comparacao
	if(node->id == INT_T){
		if( (*((int*)check)) > atoi(key) ) result = -1;
		else if( (*((int*)check)) < atoi(key) ) result = 1;
	}else if(node->id == DOUBLE_T){
		if( (*((double*)check)) > atof(key) ) result = -1;
		else if( (*((double*)check)) < atof(key) ) result = 1;
	}else if(node->id == STRING_T){
		result = strcmp(key, (char*)check);
	}
	// O retorno da funcao se da de maneira semelhante a compare_inside_file()

	return result;
}

long int sequential_search(SCHEMA *schema, NODE *node, char *search_key, int *test_count, long int offset){
	char *filename_data, *filename_index;
	FILE *fp_data, *fp_index;
	int i, n_elements_data, n_elements_index, compare_result;
	long int end_file;
	void *aux;
	
	// O bloco de codigo abaixo abre o arquivo .data para leitura, checa se a abertura foi feita corretamente e analisa quantos elementos
	// estao contidos no arquivo (seria melhor implementar uma funcao para isso, mas n ha tempo agora)
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

	// O bloco de codigo abaixo faz o mesmo que o acima, mas dessa vez para o arquivo .idx
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

	// As variaveis auxiliares sao inicializadas
	aux = malloc(node->size);
	// O valor inicial de i faz com que a busca se inicie apos os elementos que ja haviam sido adicionados ao indice
	i = n_elements_index;
	compare_result = 1;

	// Enquanto n for encontrado ou sejam verificados todos, entra no loop
	while(i < n_elements_data && compare_result != 0){
		// O contador de iteracoes eh incrementado
		(*test_count)++;
		// Para cada repeticao procura a posicao do dado a ser analisado
		fseek(fp_data, (i*schema->size)+offset, SEEK_SET);
		// Guarda o resultado em uma posicao da memoria e compara com a chave procurada fora do arquivo
		fread(aux, node->size, 1, fp_data);
		compare_result = compare_outside(node, aux, search_key);
		// incrementa o passo
		i++;
	}
	// Liberacao da memoria alocada
	free(filename_data);
	free(filename_index);
	free(aux);
	fclose(fp_data);
	fclose(fp_index);

	// Caso haja sido encontrado o item buscado, retorna a posicao do registro que o contem dentro do .data
	if(compare_result == 0) return ((i-1)*schema->size);
	// Caso contrario, retorna -1
	else return -1;
}

long int binary_search(FILE *fp, SCHEMA *schema, NODE *node, char *search_key, int begin, int end, int *test_count){

	// A condicao de parada da busca eh estabelecida para depois que ela checa um bloco que contem apenas um item
	// E o retorno para quando o item n for encontrado eh -1
	if(begin > end) return -1;

	// Define-se o ponto medio
	int middle = (begin+end)/2, compare_result;
	long int result;
	// Aux recebe o conteudo do ponto medio
	void *aux = malloc(node->size);
	fseek(fp, middle*(node->size + sizeof(long int)), SEEK_SET);
	fread(aux, node->size, 1, fp);

	// Eh feita a comparacao com a chave buscada fora do arquivo
	compare_result = compare_outside(node, aux, search_key);
	// O contador de iteracoes eh incrementado
	(*test_count)++;
	// E a memoria alocada eh liberada
	free(aux);
	// De acordo com o resultado da comparacao, chama binary_search() recursivamente ou armazena a localizacao do item no
	// .data para ser retornado
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
	// Verifica se a criacao foi bem sucedida
	if(schema != NULL){
		// Cria o "no cabeca"
		NODE *sentry = create_node();
		// Caso seja bem sucedida, inicializa os valores
		if(sentry != NULL){
			sentry->next = sentry;
			sentry->previous = sentry;
			schema->name = NULL;
			schema->n_elements = 0;
			schema->sentry = sentry;
			schema->size = 0;
		// Caso contrario, libera memoria e retorna NULL
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
	// Verifica se o schema foi criado adequadamente
	if(schema != NULL){
		// Caso tenha sido, usa a funcao read_schema para ler as linhas do arquivo .schema indicado pela stdin e armazena quantos elementos
		// o schema contera
		NODE *new_node;
		int n_elements, i;
		char *aux;
		char **table = read_schema(&n_elements);

		// A primeira linha é analizada para obter-se o nome do arquivo e o numero de elementos lido é armazenado
		aux = strtok(table[0], DELIMITERS);
		aux = strtok(NULL, DELIMITERS);
		schema->name = strdup(aux);
		schema->n_elements = n_elements;
		// O tamanho eh inicializado como 0
		schema->size = 0;

		// Cria um no para cada elemento de acordo com o numero de linhas lidas
		for(i = 1; i <= n_elements; i++){
			new_node = create_node();
			if(new_node == NULL){
				fprintf(stderr, "error creating node\n");
				exit(3);
			}

			// Obtem as informacoes do elemento a partir da linha atual
			get_node(new_node, table[i]);
			schema->size += new_node->size;

			// Insere o elemento no final da lista criada
			new_node->next = schema->sentry;
			new_node->previous = schema->sentry->previous;
			schema->sentry->previous->next = new_node;
			schema->sentry->previous = new_node;
		}

		// Libera a memoria alocada
		for(i = 0; i <= n_elements; i++){
			free(table[i]);
		}
		free(table);

		// Cria os arquivos index necessarios de acordo com o .schema e o .data
		get_index(schema);
		sort_index(schema);
	}
}

void delete_schema(SCHEMA **schema){
	// Verifica se é um endereco valido
	if(schema != NULL && (*schema) != NULL){
		NODE *aux;
		// Deleta todos os nós criados para armazenar os elementos do registro
		while((*schema)->n_elements > 0){
			aux = (*schema)->sentry->previous;
			aux->next->previous = aux->previous;
			aux->previous->next = aux->next;
			delete_node(&aux);
			// Descresce o tamanho
			(*schema)->n_elements--;
		}
		// Deleta o no sentinela
		delete_node(&((*schema)->sentry));
		// Libera o nome se necessario
		if((*schema)->name != NULL) free((*schema)->name);
		// Libera o schema em si e seta NULL
		free(*schema);
		(*schema) = NULL;
	}
}

void dump_schema(SCHEMA *schema){
	if(schema != NULL){
		int i;
		NODE *aux = schema->sentry;

		// Imprime as informacoes do registro como um todo
		printf("%s %s(%d bytes)\n", STR_TABLE, schema->name, schema->size);
		// Depois percorre todos os elementos com a variavel aux
		for(i = 0; i < schema->n_elements; i++){
			aux = aux->next;
			// Analisa o tipo do elemento e imprime as informacoes de acordo com o que foi lido
			if(aux->id == INT_T){
				printf("%s %s", aux->name, STR_INT);
			}else if(aux->id == DOUBLE_T){
				printf("%s %s",aux->name, STR_DOUBLE);
			}else if(aux->id == STRING_T){
				printf("%s %s[%ld]", aux->name, STR_CHAR, (aux->size/(int)sizeof(char)));
			}
			// Analisa se é necessario imprimir order e depois imprime o tamanho do elemento em bytes
			if(aux->order) printf(" %s", STR_ORDER);
			printf("(%ld bytes)\n", aux->size);
		}
	}
}

void dump_data(SCHEMA *schema){
	
	int i, n_elements;
	long int aux;
	// A variavel item armazenara as informacoes do elemento sendo lido
	char **item = (char**)malloc(schema->n_elements * sizeof(char*));
	// Mais uma vez o bloco responsavel por abrir o arquivo .data para leitura e obter o numero de elementos
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

	// Todos os campos de item são inicializados como NULL
	for(i = 0; i < schema->n_elements; i++){
		item[i] = NULL;
	}

	// n_elements sao lidos do arquivo .data e imprimidos na stdout de acordo com o que foi definido na funcao print_item()
	for(i = 0; i < n_elements; i++){
		get_item(item, fp, schema);
		print_item(schema, item);
	}

	// A memoria alocada é liberada
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

	// O registro é percorrido analisando se o elemento deve ser ordenado
	for(i = 0; i < schema->n_elements; i++){
		aux = aux->next;
		if(aux->order){
			// Caso seja necessario, o arquivo .data é aberto para leitura
			filename_data = (char*)malloc(sizeof(char) * (strlen(schema->name)+6));
			strcpy(filename_data, schema->name);
			strcat(filename_data, ".data");
			fp_data = fopen(filename_data, "rb");
			if(fp_data == NULL){
				fprintf(stderr, "could not open file\n");
				exit(1);
			}

			// E o arquivo .idx é criado para escrita
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

			// A funcao copy_data() é utilizada para copiar a memoria diretamente de um arquivo para o outro
			copy_data(fp_data, fp_index, cur_offset, schema, aux);

			// E toda a memoria alocada é liberada
			if(fp_data != NULL) fclose(fp_data);
			if(fp_index != NULL) fclose(fp_index);
			if(filename_data != NULL) free(filename_data);
			if(filename_index != NULL) free(filename_index);
		}

		// O offset atual é incrementado com o tamanho do elemento que acaba de ser analisado
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

	// Percorre todos os elementos do registro checando se foram ordenados
	for(i = 0; i < schema->n_elements; i++){
		node = node->next;
		if(node->order){
			// Caso tenham sido ordenados, abre o arquivo .idx correspondente para leitura
			filename_index = (char*)malloc(sizeof(char) * (strlen(schema->name) + 6 + strlen(node->name)));
			strcpy(filename_index, schema->name);
			strcat(filename_index, "-");
			strcat(filename_index, node->name);
			strcat(filename_index, ".idx");
			fp_index = fopen(filename_index, "rb");

			if(fp_index != NULL){
				// O arquivo é analisado para obter-se o numero de elementos contidos nele e o ponteiro volta ao inicio do arquivo
				fseek(fp_index, 0, SEEK_END);
				location = ftell(fp_index);
				n_elements = (int)(location/(sizeof(long int) + node->size));
				fseek(fp_index, 0, SEEK_SET);

				// O arquivo é lido um a um ate o final
				for(j = 0; j < n_elements; j++){
					// aux armazena o dado a ser impresso
					aux = malloc(node->size);
					fread(aux, node->size, 1, fp_index);
					// enquanto location armazena a localizacao salva logo apos
					fread(&location, sizeof(long int), 1, fp_index);

					// De acordo com o timpo de dado sendo analisado, a impressao é feita na stdout
					if(node->id == INT_T){
						printf("%d = %ld\n", *((int*)aux), location);
					}else if(node->id == DOUBLE_T){
						printf("%.2lf = %ld\n", *((double*)aux), location);
					}else if(node->id == STRING_T){
						printf("%s = %ld\n", (char*)aux, location);
					}
					// A memoria alocada é liberada a medida que é necessario
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

	// Percorre todos os elementos do registro checando se precisam ser ordenados
	for(i = 0; i < schema->n_elements; i++){
		node = node->next;
		if(node->order){
			// Caso seja o caso, o arquivo .idx correspondete é aberto para atualização
			filename_index = (char*)malloc(sizeof(char) * (strlen(schema->name) + 6 + strlen(node->name)));
			strcpy(filename_index, schema->name);
			strcat(filename_index, "-");
			strcat(filename_index, node->name);
			strcat(filename_index, ".idx");
			fp_index = fopen(filename_index, "r+b");

			if(fp_index != NULL){
				// Analisa-se qual o numero de elementos salvos dentro do arquivo .idx
				fseek(fp_index, 0, SEEK_END);
				location = ftell(fp_index);
				n_elements = (int)(location/(sizeof(long int) + node->size));
				fseek(fp_index, 0, SEEK_SET);

				// Utiliza-se um algoritmo de insertion sort e as funções compare_in_file() e swap() para aplicá-lo
				for(j = 1; j < n_elements; j++){

					for(k = j-1; (k >= 0) && (compare_in_file(fp_index, node, k, k+1) > 0); k--){
						swap(fp_index, node, k, k+1);
					}

				}
				// O arquivo é fechado
				fclose(fp_index);
			}
			// E a memoria alocada é liberada
			free(filename_index);
		}
	}
}

void insert_data(SCHEMA *schema){

	int i;
	void *aux;
	NODE *node = schema->sentry;
	// O arquivo .data é aberto para atualizacao
	char *filename = (char*)malloc(sizeof(char) * (strlen(schema->name)+6));
	strcpy(filename, schema->name);
	strcat(filename, ".data");
	FILE *fp_data = fopen(filename, "r+b");
	if(fp_data == NULL){
		fprintf(stderr, "could not open file\n");
		exit(1);
	}
	// E o ponteiro é movido para o fim do arquivo
	fseek(fp_data, 0, SEEK_END);
	// O registro é percorrido analisando qual o tipo de dado a ser salvo na ordem desejada
	for(i = 0; i < schema->n_elements; i++){
		node = node->next;
		// aux armazena os dados a serem escritos
		aux = malloc(node->size);

		// De acordo com o tipo sendo analisado, as informações são lidas da stdin e armazendas em aux
		if(node->id == INT_T){
			scanf("%d", (int*)aux);
		}else if(node->id == DOUBLE_T){
			scanf("%lf", (double*)aux);
		}else if(node->id == STRING_T){
			free(aux);
			aux = (void*)my_gets(stdin, (node->size/sizeof(char)));
		}
		// As informacoes sao entao escritas no final do arquivo .data
		fwrite(aux, node->size, 1, fp_data);
		// A memoria alocada é liberada
		if(aux != NULL) free(aux);
	}
	free(filename);
	// E o arquivo é fechado
	fclose(fp_data);
}

void search_index_data(SCHEMA *schema){

	int i, test_count, search_return, n_elements;
	long int location, offset;
	void *aux;
	char *filename_index, *search_term, *filename_data, *print_field, *search_key;
	NODE *node = schema->sentry;
	FILE *fp_index, *fp_data;

	// Sao lidos da stdin o campo a ser analisado na busca, a chave desejada e o campo a ser impresso apos a busca
	search_term = my_get_line(stdin, &i);
	search_key = my_get_line(stdin, &i);
	print_field = my_get_line(stdin, &i);

	// test_count armazena o numero de iteracoes de busca e offset armazena o offset do tipo sendo analisado em relação á posição do elemento
	// como um todo
	test_count = 0;
	offset = 0;
	// Sao analizados todos os elementos ate ser encontrado o que deseja-se para a busca
	for(i = 0; i < schema->n_elements; i++){
		node = node->next;
		if(strcmp(search_term, node->name) == 0 && node->order){

			// O arquivo .idx é aberto para leitura
			filename_index = (char*)malloc(sizeof(char) * (strlen(schema->name) + 6 + strlen(node->name)));
			strcpy(filename_index, schema->name);
			strcat(filename_index, "-");
			strcat(filename_index, node->name);
			strcat(filename_index, ".idx");
			fp_index = fopen(filename_index, "rb");

			if(fp_index != NULL){

				// Analisa-se quantos elementos estão presentes no  .idx
				fseek(fp_index, 0, SEEK_END);
				location = ftell(fp_index);
				n_elements = (int)(location/(sizeof(long int) + node->size));
				fseek(fp_index, 0, SEEK_SET);

				// É realizada a busca binaria
				search_return = binary_search(fp_index, schema, node, search_key, 0, n_elements-1, &test_count);
				// E o arquivo é fechado
				fclose(fp_index);
				if(search_return == -1){
					// Caso não tenha sido encontrado o item desejado no indice, realiza-se a busca sequencial
					// nos itens que foram adicionados no fim do .data
					search_return = sequential_search(schema, node, search_key, &test_count, offset);
				}
			}else{
				fprintf(stderr, "could not open file\n");
				exit(1);
			}
			// Liberacao da memoria alocada
			free(filename_index);

			// Imprime o numero de iteracoes da busca realizadas
			printf("%d\n", test_count);
			// Caso nao tenha sido encontrado o item, imprime a mensagem de erro
			if(search_return == -1){
				printf("value not found\n");
			}else{
				// Caso o item tenha sido encontrado, procura o offset do item a ser impresso em relação
				// ao elemento como um todo
				node = schema->sentry;
				offset = 0;
				for(i = 0; i < schema->n_elements; i++){
					node = node->next;
					if(strcmp(print_field, node->name) == 0){
						break;
					}
					offset += node->size;
				}

				// O arquivo .data é aberto para leitura
				filename_data = (char*)malloc(sizeof(char) * (strlen(schema->name)+6));
				strcpy(filename_data, schema->name);
				strcat(filename_data, ".data");
				fp_data = fopen(filename_data, "rb");
				if(fp_data == NULL){
					fprintf(stderr, "could not open file\n");
					exit(1);
				}

				// A variavel aux armazena o conteudo do item a ser impresso
				fseek(fp_data, search_return+offset, SEEK_SET);
				aux = malloc(node->size);
				fread(aux, node->size, 1, fp_data);
				// E o arquivo é fechado
				fclose(fp_data);

				// A impressao é feita de acordo com o tipo sendo analisado
				if(node->id == INT_T){
					printf("%d\n", *((int*)aux));
				}else if(node->id == DOUBLE_T){
					printf("%.2lf\n", *((double*)aux));
				}else if(node->id == STRING_T){
					printf("%s\n", (char*)aux);
				}
				// E a memoria alocada é liberada
				free(aux);
				free(filename_data);
			}
			// caso o item seja encontrado, o valor de i é alterado para que saia do loop
			i = schema->n_elements+1;
		}
		// O valor de offset é incrementado com o tamanho do item anterior analisado
		if(i <= schema->n_elements){
			offset += node->size;
		}
	}

	// Caso não tenha sido realizada nenhuma iteracao de busca, não foi encontrado nenhum arquivo .idx do campo buscado
	if(test_count == 0){
		printf("index not found\n");
	}
	// Libera a memoria alocada
	free(search_term);
	free(search_key);
	free(print_field);
}

// Funcoes de leitura-------------------------------------------------------------------------------------------------

char *my_get_line(FILE *stream, int *ending){
	char *string = NULL;
	int input, size = 0;

	// Procura o primeiro caracter valido para comecar a leitura da string
	do{
		input = fgetc(stream);
	}while((char)input != '\n' && input != EOF && !isalnum(input));

	if(input == EOF){   // Chamada da função em local invalido do arquivo:
//		if(input == '\n') (*ending) = 1;       // -linha vazia  OBS: Essa checagem precisou ser removida para evitar erros
		(*ending) = -2;             	       // -final do arquivo
		return NULL;
	}
	// Assim como a remocao da linha acima, essa repeticao evita que ocorra erro por ler uma quebra de linha, mas pode gerar outros
	// Para esse caso especifico, ela cumpre a funcao desejada
	while(input == '\n') input = fgetc(stream);

	// Le um caracter por vez alocando a memoria necessaria ate encontrar quebra de linha ou fim de arquivo
	do{
		string = (char*)realloc(string, (size+1) * sizeof(char));
		string[size++] = (char)input;
		input = fgetc(stream);
	}while(input != '\n' && input != EOF);
	// Adicao o indicado de fim de string
	string = (char*)realloc(string, (size+1) * sizeof(char));
	string[size] = '\0';

	// Registra se foi encontrada quebra de linha ou fim de arquivo
	if(input == '\n') (*ending) = 0;
	else (*ending) = -1;

	// Retorna a string lida
	return string;
}

char *my_gets(FILE *stream, int str_size){
	// Aloca a memoria de acordo com o tamanho passado
	char *string = (char*)calloc(str_size, 1);
	int input, cur_size = 0;

	// Ignora todos os caracteres invalidos ate encontrar um valido ou o fim do arquivo
	do{
		input = fgetc(stream);
	}while( ((char)input == ' ' || (char)input == '\n') && input != EOF);
	// Caso atinja o fim do arquivo nessa busca, encerra o programa com mensagem de erro
	if(input == EOF){
		fprintf(stderr, "error reading string");
		exit(4);
	}

	// Le no maximo str_size caracteres da stream, parando caso encontre uma quebra de linha ou fim de arquivo
	do{
		string[cur_size++] = (char)input;
		input = fgetc(stream);
	}while((char)input != '\n' && input != EOF && cur_size < str_size-1);

	// Adicao do indicador de fim de string e retorno
	string[cur_size] = '\0';
	return string;
}

char **read_schema(int *n_elements){
	int ending, size = 0;
	FILE *fp;
	char *aux, *filename;
	char **table = NULL;

	// Le o nome do arquivo a ser aberto para leitura da stdin
	filename = my_get_line(stdin, &ending);
	fp = fopen(filename, "r");
	// Checa se foi aberto de maneira correta
	if(fp == NULL){
		fprintf(stderr, "could not open file\n");
		free(filename);
		exit(1);
	}

	// Le o arquivo linha por linha
	do{
		aux = my_get_line(fp, &ending);

		// Caso a linha seja lida corretamente, salva o que foi lido em uma tabela de char**
		if(aux != NULL){
			table = (char**)realloc(table, (size+1) * sizeof(char*));
			table[size++] = aux;
		}
	}while(ending >= 0); // Se ending eh positivo, ainda nao foi atingido o fim do arquivo

	// Registra quantos elementos o registro contem, tendo em vista que a primeira linha contem o nome do arquivo
	(*n_elements) = size-1;

	// Precaucao
	if(size == 0){
		fprintf(stderr, "error reading .schema\n");
		exit(2);
	}

	// Libera memoria alocara e retorna as linhas lidas
	fclose(fp);
	free(filename);
	return table;
}

void copy_data(FILE *origin, FILE *destiny, long int cur_offset, SCHEMA *schema, NODE *type){

	long int location;
	int i, n_elements;
	// Cada elemento a ser salvo no arquivo de destino (.idx) deve conter um elemento do registro e um long int contendo o offset do registro
	// dentro do arquivo de origem
	int data_size = (int)sizeof(long int) + type->size;
	void *aux = malloc(data_size);

	// Analisa quantos elementos estao salvos na origem
	fseek(origin, 0, SEEK_END);
	location = ftell(origin);
	n_elements = (int)(location/schema->size);
	// Retorna o pornteiro para o inicio do arquivo
	fseek(origin, 0, SEEK_SET);

	// Percorre o arquivo de origem lendo o elemento correspondente ao no e escrevendo-o no arquivo de destino
	// Juntamente com a posicao original do registro que contem o elemento lido
	for(i = 0; i < n_elements; i++){
		// cur_offset guarda o offset do elemento em relacao ao registro como um todo
		fseek(origin, (i * schema->size) + cur_offset, SEEK_SET);
		fread(aux, type->size, 1, origin);
		location = (long int)(i * schema->size);
		// As duas informacoes sao salvas uma diretamente apos a outra
		memcpy(aux+(type->size), &location, sizeof(long int));
		fwrite(aux, data_size, 1,destiny);
	}

	// Liberacao de memoria alocada
	if(aux != NULL) free(aux);
}
