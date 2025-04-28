#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define SIZE 100
#define USERNAME_SIZE 50
#define PASSWORD_SIZE 12
#define BOOKNAME_SIZE 50
#define AUTHORNAME_SIZE 99

// ==== ESTRUTURAS ====

typedef struct
{
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
} User;

typedef struct Livro
{
    char namebook[BOOKNAME_SIZE];
    char author[AUTHORNAME_SIZE];
    int pages;
    struct Livro *next; // para tratar colisões
} Livro;

// ==== VARIÁVEIS GLOBAIS ====
User *users = NULL;
int userCount = 0;
bool logado = false;

Livro *livroTable[SIZE]; // Tabela hash para livros

// ==== FUNÇÕES ====
unsigned int hash(const char *key);

void inserirLivro(Livro novoLivro);
Livro *buscarLivro(const char *nomeLivro);
void removerLivro(const char *nomeLivro);

void saveUsersToFile(const char *filename);
void loadUsersFromFile(const char *filename);

void saveLivrosToFile(const char *filename);
void loadLivrosFromFile(const char *filename);

int criarLogin();
int logarUsuario();
int registrarLivro();

// ==== FUNÇÃO PRINCIPAL ====
int main()
{
    for (int i = 0; i < SIZE; i++)
        livroTable[i] = NULL;

    loadUsersFromFile("usuarios.txt");
    loadLivrosFromFile("livros.txt");

    int opcao;
    do
    {
        printf("\n===== MENU PRINCIPAL =====\n");
        if (!logado)
        {
            printf("1. Criar Login\n");
            printf("2. Logar\n");
            printf("3. Visualizar Livros\n");
            printf("0. Sair\n");
        }
        else
        {
            printf("1. Registrar Livro\n");
            printf("2. Visualizar Livros\n");
            printf("3. Buscar Livro\n");
            printf("0. Sair\n");
        }
        printf("===========================\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        getchar(); // limpar buffer

        if (!logado)
        {
            switch (opcao)
            {
            case 1:
                criarLogin();
                break;

            case 2:
                if (logarUsuario() == 0)
                    logado = true;
                break;
            case 3:
                printf("\n=== Livros Cadastrados ===\n");
                for (int i = 0; i < SIZE; i++)
                {
                    Livro *atual = livroTable[i];
                    while (atual)
                    {
                        printf("Livro: %s | Autor: %s | Paginas: %d\n", atual->namebook, atual->author, atual->pages);
                        atual = atual->next;
                    }
                }
                break;

            case 0:
                printf("\nSaindo...\n");
                break;
            default:
                printf("Opcao invalida.\n");
            }
        }
        else
        {
            switch (opcao)
            {
            case 1:
                registrarLivro();
                break;
            case 2:
                printf("\n=== Livros Cadastrados ===\n");
                for (int i = 0; i < SIZE; i++)
                {
                    Livro *atual = livroTable[i];
                    while (atual)
                    {
                        printf("Livro: %s | Autor: %s | Paginas: %d\n", atual->namebook, atual->author, atual->pages);
                        atual = atual->next;
                    }
                }
                break;
            case 3:
            {
                char nomeBusca[BOOKNAME_SIZE];
                printf("Digite o nome do livro: ");
                fgets(nomeBusca, BOOKNAME_SIZE, stdin);
                nomeBusca[strcspn(nomeBusca, "\n")] = '\0';

                Livro *encontrado = buscarLivro(nomeBusca);
                if (encontrado)
                {
                    printf("Livro encontrado: %s | Autor: %s | Paginas: %d\n", encontrado->namebook, encontrado->author, encontrado->pages);
                }
                else
                {
                    printf("Livro nao encontrado.\n");
                }
            }
            break;
            case 0:
                printf("\nSaindo...\n");
                break;
            default:
                printf("Opcao invalida.\n");
            }
        }

    } while (opcao != 0);

    saveLivrosToFile("livros.txt");
    saveUsersToFile("usuarios.txt");

    return 0;
}

// ==== HASH PARA LIVROS ====

unsigned int hash(const char *key)
{
    unsigned int hash = 0;
    while (*key)
        hash = (hash * 31) + *key++;
    return hash % SIZE;
}

void inserirLivro(Livro novoLivro)
{
    unsigned int idx = hash(novoLivro.namebook);
    Livro *novo = (Livro *)malloc(sizeof(Livro));
    if (!novo)
    {
        perror("Erro de memoria");
        return;
    }
    *novo = novoLivro;
    novo->next = livroTable[idx];
    livroTable[idx] = novo;
}

Livro *buscarLivro(const char *nomeLivro)
{
    unsigned int idx = hash(nomeLivro);
    Livro *atual = livroTable[idx];
    while (atual)
    {
        if (strcmp(atual->namebook, nomeLivro) == 0)
            return atual;
        atual = atual->next;
    }
    return NULL;
}

void removerLivro(const char *nomeLivro)
{
    unsigned int idx = hash(nomeLivro);
    Livro **indireto = &livroTable[idx];
    while (*indireto)
    {
        if (strcmp((*indireto)->namebook, nomeLivro) == 0)
        {
            Livro *temp = *indireto;
            *indireto = temp->next;
            free(temp);
            return;
        }
        indireto = &(*indireto)->next;
    }
}

// ==== FUNÇÕES DE ARQUIVO PARA USUARIOS ====

void saveUsersToFile(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        perror("Erro ao salvar usuarios");
        return;
    }
    for (int i = 0; i < userCount; i++)
    {
        fprintf(file, "%s,%s\n", users[i].username, users[i].password);
    }
    fclose(file);
}

void loadUsersFromFile(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
        return;

    char line[USERNAME_SIZE + PASSWORD_SIZE + 2];
    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = '\0';
        char *username = strtok(line, ",");
        char *password = strtok(NULL, ",");

        if (username && password)
        {
            users = realloc(users, (userCount + 1) * sizeof(User));
            strcpy(users[userCount].username, username);
            strcpy(users[userCount].password, password);
            userCount++;
        }
    }
    fclose(file);
}

// ==== FUNÇÕES DE ARQUIVO PARA LIVROS ====

void saveLivrosToFile(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        perror("Erro ao salvar livros");
        return;
    }
    for (int i = 0; i < SIZE; i++)
    {
        Livro *atual = livroTable[i];
        while (atual)
        {
            fprintf(file, "%s,%s,%d\n", atual->namebook, atual->author, atual->pages);
            atual = atual->next;
        }
    }
    fclose(file);
}

void loadLivrosFromFile(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
        return;

    char line[BOOKNAME_SIZE + AUTHORNAME_SIZE + 20];
    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = '\0';
        char *namebook = strtok(line, ",");
        char *author = strtok(NULL, ",");
        char *pagesStr = strtok(NULL, ",");

        if (namebook && author && pagesStr)
        {
            Livro novo;
            strcpy(novo.namebook, namebook);
            strcpy(novo.author, author);
            novo.pages = atoi(pagesStr);
            inserirLivro(novo);
        }
    }
    fclose(file);
}

// ==== FUNÇÕES DE LOGIN/REGISTRO ====

int criarLogin()
{
    printf("\n=== CRIAR LOGIN ===\n");
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
    char confpassword[PASSWORD_SIZE];

    printf("Digite um nome de usuario: ");
    fgets(username, USERNAME_SIZE, stdin);
    printf("Digite uma senha: ");
    fgets(password, PASSWORD_SIZE, stdin);
    printf("Confirme a sua senha: ");
    fgets(confpassword, PASSWORD_SIZE, stdin);

    username[strcspn(username, "\n")] = '\0';
    password[strcspn(password, "\n")] = '\0';
    confpassword[strcspn(confpassword, "\n")] = '\0';

    for (int i = 0; i < userCount; i++)
    {
        if (strcmp(users[i].username, username) == 0)
        {
            printf("Usuario ja existente.\n");
            return -1;
        }
    }

    if (strcmp(password, confpassword) == 0)
    {
        users = realloc(users, (userCount + 1) * sizeof(User));
        strcpy(users[userCount].username, username);
        strcpy(users[userCount].password, password);
        userCount++;
        saveUsersToFile("usuarios.txt");
        printf("Usuario criado com sucesso!\n");
        return 0;
    }
    else
    {
        printf("Senhas nao conferem.\n");
        return -1;
    }
}

int logarUsuario()
{
    printf("\n=== FAZER LOGIN ===\n");
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];

    printf("Digite seu nome de usuario: ");
    fgets(username, USERNAME_SIZE, stdin);
    printf("Digite sua senha: ");
    fgets(password, PASSWORD_SIZE, stdin);

    username[strcspn(username, "\n")] = '\0';
    password[strcspn(password, "\n")] = '\0';

    for (int i = 0; i < userCount; i++)
    {
        if (strcmp(users[i].username, username) == 0 && strcmp(users[i].password, password) == 0)
        {
            printf("Login bem-sucedido!\n");
            return 0;
        }
    }
    printf("Usuario ou senha incorretos.\n");
    return -1;
}

// ==== REGISTRAR LIVRO ====

int registrarLivro()
{
    printf("\n=== REGISTRAR LIVRO ===\n");
    Livro novo;

    printf("Nome do livro: ");
    fgets(novo.namebook, BOOKNAME_SIZE, stdin);
    novo.namebook[strcspn(novo.namebook, "\n")] = '\0';

    printf("Autor do livro: ");
    fgets(novo.author, AUTHORNAME_SIZE, stdin);
    novo.author[strcspn(novo.author, "\n")] = '\0';

    printf("Numero de paginas: ");
    scanf("%d", &novo.pages);
    getchar(); // limpar buffer

    inserirLivro(novo);
    saveLivrosToFile("livros.txt");

    printf("Livro registrado com sucesso!\n");
    return 0;
}
