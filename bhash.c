#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define SIZE 100 // Tamanho da tabela hash
#define USERNAME_SIZE 50
#define PASSWORD_SIZE 12
#define BOOKNAME_SIZE 50
#define AUTHORNAME_SIZE 99

// ==== ESTRUTURA PARA USUÁRIOS ====
typedef struct
{
    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
} User;

// ==== ESTRUTURA PARA USUÁRIOS ====
typedef struct
{
    char namebook[BOOKNAME_SIZE];
    char author[AUTHORNAME_SIZE];
    int pages;
} Book;

// ==== VARIÁVEIS GLOBAIS (PARA SIMPLICIDADE) ====
User *users = NULL;   // Ponteiro para array dinâmico de usuários
int userCount = 0;    // Quantos usuários já cadastramos
int userCapacity = 0; // Capacidade atual do array de usuários
bool logado = false;
Book *books = NULL;   // Ponteiro para array dinâmico de livros
int bookCount = 0;    // Quantos livros já cadastramos
int bookCapacity = 0; // Capacidade atual do array de livros

// ==== FUNÇÕES DE CADASTRO/LOGIN DE USUARIOS ====
int criarLogin();
int logarUsuario();
void loadUsersFromFile(const char *filename);
void saveUsersToFile(const char *filename);

// ==== FUNÇÕES DE CADASTRO/LOGIN DE LIVROS ====
int registrarLivro();
int removerLivro();
void exibirLivros();
void ordenarLivros();

// Exemplo de uso da tabela hash
int main()
{
    loadUsersFromFile("usuarios.txt");

    int opcao;
    do {
        printf("\n===== MENU PRINCIPAL =====\n");
        if (!logado) {
            printf("1. Criar Login\n");
            printf("2. Logar\n");
        } else {
            printf("3. Registrar Livro\n");
            printf("4. Remover Livro\n");
            printf("5. Visualizar Livros\n");
            printf("6. Sair\n");
        }
        printf("===========================\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &opcao);
        getchar(); // consumir \n

        switch (opcao)
        {
        case 1:
            criarLogin();
            break;

        case 2:
            if (logarUsuario() != -1)
            {
                logado = true;
            }
            break;
        case 3:
            if (logado)
            {
                registrarLivro();
            }
            else
            {
                printf("Voce precisa estar logado!\n");
            }
            break;

        case 4:
        {
            printf("\n=== Remover Livro ===\n");

            break;
        }

        case 5:
        {
            printf("\n=== Visualizar Livro ===\n");

            break;
        }

        case 0:
            printf("\nSaindo...\n");
            break;

        default:
            printf("Opcao invalida. Tente novamente.\n");
            break;
        }

    } while (opcao != 0);

    return 0;
}

// ======================================================
//           FUNÇÕES DE SALVAR E LER DE USUARIOS
// ======================================================

void saveUsersToFile(const char *filename)
{
    FILE *file = fopen(filename, "w"); // Abre para escrita
    if (file == NULL)
    {
        perror("Erro ao abrir o arquivo");
        return;
    }
    for (int i = 0; i < userCount; i++)
    {
        fprintf(file, "%s,%s\n", users[i].username, users[i].password);
        // <- aqui estava errado no seu código
    }
    fclose(file);
}

void loadUsersFromFile(const char *filename)
{
    FILE *file = fopen(filename, "r"); // Abre para leitura
    if (file == NULL)
    {
        // Se o arquivo não existe, apenas retorna (primeiro uso do programa, por exemplo)
        return;
    }

    char line[USERNAME_SIZE + PASSWORD_SIZE + 2]; // buffer para a linha inteira

    while (fgets(line, sizeof(line), file))
    {
        // Remove o \n no final
        line[strcspn(line, "\n")] = '\0';

        char *token = strtok(line, ",");
        if (token == NULL)
            continue;

        strcpy(users[userCount].username, token);

        token = strtok(NULL, ",");
        if (token == NULL)
            continue;

        strcpy(users[userCount].password, token);

        userCount++;
        // Se quiser segurança, aqui você também pode checar userCapacity e fazer realloc se necessário
    }

    fclose(file);
}

// ======================================================
//           FUNÇÕES DE SALVAMENTO DE LIVROS
// ======================================================

void saveLivrosToFile(const char *filename)
{
    FILE *file = fopen(filename, "w"); // Abre o arquivo para escrita
    if (file == NULL)
    {
        perror("Erro ao abrir o arquivo");
        return;
    }
    for (int i = 0; i < bookCount; i++)
    {
        fprintf(file, "%s,%s,%s\n", books[i].namebook, books[i].author, books[i].pages);
    }
    fclose(file); // Fecha o arquivo
}

// ======================================================
//               FUNÇÕES DE CADASTRO/LOGIN
// ======================================================

// Cria um novo usuário (username e senha). Retorna -1 se falhar ou índice do novo usuário.
int criarLogin()
{
    printf("\n=== CRIAR LOGIN ===\n");

    // Verifica se precisamos aumentar a capacidade antes de ler o novo username
    if (userCount == userCapacity)
    {
        // dobramos a capacidade
        userCapacity *= 2;
        User *temp = (User *)realloc(users, userCapacity * sizeof(User));
        if (temp == NULL)
        {
            printf("Erro ao realocar memoria para usuarios.\n");
            return -1;
        }
        users = temp;
    }

    char username[USERNAME_SIZE];
    char password[PASSWORD_SIZE];
    char confpassword[PASSWORD_SIZE];
    printf("Digite um nome de usuario: ");
    fgets(username, USERNAME_SIZE, stdin);
    printf("Digite uma senha: ");
    fgets(password, PASSWORD_SIZE, stdin);
    printf("Confirme a sua senha: ");
    fgets(confpassword, PASSWORD_SIZE, stdin);
    username[strcspn(username, "\n")] = '\0';         // remover \n
    password[strcspn(password, "\n")] = '\0';         // remover \n
    confpassword[strcspn(confpassword, "\n")] = '\0'; // remover \n

    // Verificar se já existe
    for (int i = 0; i < userCount; i++)
    {
        if (strcmp(users[i].username, username) == 0)
        {
            printf("Esse nome de usuario ja existe. Tente outro.\n");
            return -1;
        }
    }

    if (strcmp(password, confpassword) == 0)
    {
        // Armazenar no array global
        strcpy(users[userCount].username, username);
        strcpy(users[userCount].password, password);

        saveUsersToFile("usuarios.txt");

        userCount++;
        printf("Usuario criado com sucesso!\n");

        // Retorna o índice do novo usuário
        return (userCount - 1);
    }
    else
    {
        printf("Senhas não são iguais\n");
        return -1;
    }
}

// Tenta logar usuário buscando apenas pelo nome e senha
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

            return i;
        }
    }

    printf("Usuario nao encontrado.\n");
    return -1;
}

// ======================================================
//          FUNÇÕES DE CADASTRO/LOGIN DE LIVROS
// ======================================================

// Cria um novo usuário (username e senha). Retorna -1 se falhar ou índice do novo usuário.
int registrarLivro()
{
    printf("\n=== REGISTRAR LIVRO ===\n");

    if (bookCount == bookCapacity)
    {
        // Se o espaço dos livros acabar, aumenta a capacidade
        bookCapacity = (bookCapacity == 0) ? 2 : bookCapacity * 2;
        Book *temp = (Book *)realloc(books, bookCapacity * sizeof(Book));
        if (temp == NULL)
        {
            printf("Erro ao realocar memoria para livros.\n");
            return -1;
        }
        books = temp;
    }

    char namebook[BOOKNAME_SIZE];
    char author[AUTHORNAME_SIZE];
    int pages;

    printf("Digite o nome do livro: ");
    fgets(namebook, BOOKNAME_SIZE, stdin);
    namebook[strcspn(namebook, "\n")] = '\0';

    printf("Digite o nome do autor: ");
    fgets(author, AUTHORNAME_SIZE, stdin);
    author[strcspn(author, "\n")] = '\0';

    printf("Digite o numero de paginas: ");
    scanf("%d", &pages);
    getchar(); // consumir o '\n' depois do número

    // Preenche os dados no array de livros
    strcpy(books[bookCount].namebook, namebook);
    strcpy(books[bookCount].author, author);
    books[bookCount].pages = pages;

    bookCount++;

    // Salva no arquivo (se quiser depois implementar)
    // saveLivrosToFile("livros.txt");

    printf("Livro registrado com sucesso!\n");
    return bookCount - 1;
}

// Tenta logar usuário buscando apenas pelo nome e senha
int logUsuario()
{
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
            return i;
        }
    }

    printf("Usuario nao encontrado.\n");
    return -1;
}