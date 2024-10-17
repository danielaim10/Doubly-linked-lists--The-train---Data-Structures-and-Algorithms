#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct wagon {
    char *elem;
    struct wagon *next, *prev;
} wagon;

typedef struct train {
    wagon *sentry;
    wagon *mechanic;

} train;

typedef struct nod {
    char command[20];
    struct nod *next;
    struct nod *prev;
} nod;

typedef struct queue {
    nod *front;
    nod *rear;
    int count;
} queue;

// FUNCTII PENTRU INITIALIZAREA FIECAREI STRUCTURI DE DATE FOLOSITE
queue *init_queue() {
    queue *q = (queue *)malloc(sizeof(queue));
    q->front = NULL;
    q->rear = NULL;
    q->count = 0;
    return q;
}

void init_wagon(wagon **w) {
    *w = (wagon *)malloc(sizeof(wagon));
    (*w)->elem = (char *)malloc(10 * sizeof(char));
    strcpy((*w)->elem, "#");
    (*w)->next = NULL;
    (*w)->prev = NULL;
}

void init_train(train **t) {
    *t = NULL;
    *t = (train *)malloc(sizeof(train));
    init_wagon(&(*t)->sentry);
    init_wagon(&(*t)->mechanic);
    (*t)->sentry->next = (*t)->mechanic;
    (*t)->sentry->prev = (*t)->mechanic;
    (*t)->mechanic->prev = (*t)->sentry;
    (*t)->mechanic->next = (*t)->sentry;
}

// OPERATII DE TIP UPDATE
void MOVE_LEFT(train *t) {
    wagon *new = t->mechanic->prev;
    // initializez in new pozitia la care doresc sa mut mecanicul
    // daca inaintea mecanicului este santinela, mecanicul va deveni ultimul
    // vagon
    if (t->mechanic->prev == t->sentry) {
        t->mechanic = t->sentry->prev;
        t->mechanic->next = t->sentry;
        t->mechanic->prev = t->sentry->prev->prev;
    } else {
        // in acest caz mecanicul va deveni vagonul de dinaintea lui
        // si facem in continuare legaturile noi create
        t->mechanic = t->mechanic->prev;
        t->mechanic->next = new->next;
        t->mechanic->prev = new->prev;
    }
}

void MOVE_RIGHT(train *t) {
    wagon *new = t->mechanic->next;
    // initializez in new pozitia la care doresc sa mut mecanicul
    if (t->mechanic->next == t->sentry) {
        // daca dupa mecanic este santinela, adaugam un nou vagon(plus) in care
        // vom pozitiona mecanicul si facem in continuare legaturile noi create
        wagon *plus = NULL;
        init_wagon(&plus);
        plus->prev = t->mechanic;
        plus->next = t->sentry;
        t->mechanic->next = plus;
        t->sentry->prev = plus;
        t->mechanic = plus;

    } else {
        t->mechanic = new;
    }
}

void WRITE(train *t, char x[]) { strcpy(t->mechanic->elem, x); }

void CLEAR_CELL(train *t) {
    if (t->mechanic->prev == t->sentry) {
        // in cazul in care inaintea mecanicului este santinela
        // avem alte 2 cazuri pe care le vom trata separat
        if (t->mechanic->next == t->sentry) {
            // daca vagonul este singutul vagon din tren, reinitializam
            free(t->mechanic);
            t->mechanic = NULL;
            init_wagon(&t->mechanic);
            t->sentry->next = t->mechanic;
        } else {
            // mecanicul este in primul vagon si nu este doar singurul
            // eliminam vagon in care este mecanicul facand legaturile cu
            // santinela pentru vagon ce urmeaza dupa mecanic
            // in plus, m-am folosit de functia  MOVE_LEFT pentru a modifica si
            // pozitia noua a mecanicului
            wagon *eliminate = t->mechanic;
            MOVE_LEFT(t);
            t->sentry->next = eliminate->next;
            eliminate->next->prev = t->sentry;
            free(eliminate);
        }
    } else {
        // analog, dar pentru situatia in care nu ne aflam langa santinela
        wagon *eliminate = t->mechanic;
        MOVE_LEFT(t);
        eliminate->prev->next = eliminate->next;
        eliminate->next->prev = eliminate->prev;
        free(eliminate);
    }
}

// am redus la starea initiala eliberand memoria utilizata de tren si
// initializand din nou
void CLEAR_ALL(train *t) {
    free(t);
    init_train(&t);
}

void INSERT_RIGHT(train *t, char elem[]) {
    // initializam un nou vagon new si facem legaturile acestuia
    wagon *next = t->mechanic->next;
    wagon *new = NULL;
    init_wagon(&new);
    new->prev = t->mechanic;
    new->next = next;
    t->mechanic->next = new;
    if (next != NULL) {
        next->prev = new;
    }
    // actualizam adresa curenta a mecanicului
    t->mechanic = new;
    strcpy(t->mechanic->elem, elem);
}

void INSERT_LEFT(train *t, char elem[], FILE *Output) {
    wagon *prev = t->mechanic->prev;
    // cazul de eroare
    if (t->mechanic->prev == t->sentry) {
        fprintf(Output, "ERROR\n");
        return;
    } else {
        // initializam un nou vagon new si facem legaturile acestuia
        wagon *new = NULL;
        init_wagon(&new);
        new->prev = prev;
        new->next = t->mechanic;
        t->mechanic->prev = new;
        if (prev != NULL) {
            prev->next = new;
        }

        // actualizam adresa curenta a mecanicului
        t->mechanic = new;
        strcpy(t->mechanic->elem, elem);
    }
}

// OPERATII DE TIP SEARCH
void SEARCH(train *t, char val[], FILE *Output) {
    wagon *curent = t->mechanic;
    wagon *ifnot = t->mechanic;
    unsigned long i = 0, k = 0, ok = 0;
    // in cazul in care nu exista decat vagonul in care este mecanicul
    if (t->sentry->next == t->mechanic && t->mechanic->next == t->sentry) {
        fprintf(Output, "ERROR\n");
        return;
    }
    // am folosit do while pentru a verifica inclusiv vagonul in care este
    // mecanicul
    do {
        // evitam parcurgerea prin santinela
        if (curent == t->sentry) {
            curent = t->sentry->next;
        }
        if (curent->elem[0] == val[i]) {
            i++;
            if (i == strlen(val)) {
                t->mechanic = curent;
                break;
            }
        } else {
            i = 0;
            if (curent->elem[0] == val[i]) {
                i++;
            }
        }
        // trecem la urmatorul vagon pentru a verifica sirul de caractere pe
        // care il cautam
        curent = curent->next;
        k++;
    } while (curent != t->mechanic || k == 0);
    // dupa verificare, ne repozitionam pe vagonul unde am descoperit primul
    // caracter din sirul cautat
    for (ok = 0; ok < strlen(val) - 1; ok++) {
        if (curent->prev == t->sentry) {
            t->mechanic = curent->prev->prev;
            curent = curent->prev;
        } else {
            t->mechanic = curent->prev;
            curent = curent->prev;
        }
    }
    // in cazul in care nu am gasit sau nu am gasit sirul complet, revenim la
    // pozitia initiala
    if (i < strlen(val)) {
        fprintf(Output, "ERROR\n");
        t->mechanic = ifnot;
    }
}

void SEARCH_LEFT(train *t, char val[], FILE *Output) {
    wagon *curent = t->mechanic;
    unsigned long i = 0, k = 0;

    do {
        if (curent == t->sentry) {
            break;
        }
        if (curent->elem[0] == val[i]) {
            i++;
            if (i == strlen(val)) {
                t->mechanic = curent;
                break;
            }
        }
        // analog cu functia search doar ca parcurgerea va fi inversa
        curent = curent->prev;
        k++;
    } while (curent != t->mechanic || k == 0);

    if (i < strlen(val)) {
        fprintf(Output, "ERROR\n");
    }
}

void SEARCH_RIGHT(train *t, char val[], FILE *Output) {
    wagon *curent = t->mechanic;
    unsigned long i = 0, k = 0;
    // analog cu functia search
    do {
        if (curent == t->sentry) {
            break;
        }
        if (curent->elem[0] == val[i]) {
            i++;
            if (i == strlen(val)) {
                t->mechanic = curent;
                break;
            }
        }

        curent = curent->next;
        k++;
    } while (curent != t->mechanic || k == 0);

    if (i < strlen(val)) {
        fprintf(Output, "ERROR\n");
    }
}

// OPERATII DE TIP QUERY
void SHOW_CURRENT(train *t, FILE *Output) {
    fprintf(Output, "%s", t->mechanic->elem);
    fprintf(Output, "\n");
}

void SHOW(train *t, FILE *Output) {
    wagon *p = t->sentry->next;
    while (p != t->sentry) {
        if (p == t->mechanic)
            // aratam pozitia pe care este mecanicul
            fprintf(Output, "|%s|", p->elem);
        else
            fprintf(Output, "%s", p->elem);
        p = p->next;
    }
    fprintf(Output, "\n");
}

void SWITCH(queue *q) {
    if (q->count == 0 || q->count == 1)
        return;
    else {
        nod *prev = NULL;
        nod *curent = q->front;
        nod *next;
        // interschimbarea legaturilor din coada
        while (curent != NULL) {
            next = curent->next;
            curent->next = prev;
            prev = curent;
            curent = next;
        }
        q->rear = q->front;
        q->front = prev;
    }
}

// FUNCTIE DE ADAUGARE IN COADA NOI COMENZI
void enqueue(queue *q, char command[]) {
    nod *new = (nod *)malloc(sizeof(nod));
    strcpy(new->command, command);
    new->next = NULL;

    if (q->count == 0) {
        q->front = new;
        q->rear = new;
    } else {
        q->rear->next = new;
        new->prev = q->rear;
        q->rear = new;
    }
    q->count++;
}
// FUNCTIE DE A STERGE DIN COADA COMENZILE
char *dequeue(queue *q) {
    nod *temp = q->front;
    char *command = (char *)malloc(2 * sizeof(char));
    strcpy(command, temp->command);

    q->front = q->front->next;
    free(temp);

    if (q->front == NULL) {
        q->rear = NULL;
    }
    q->count--;
    return command;
}
// OPERATIA EXECUTE
void EXECUTE(train *t, queue *q, FILE *Output, char command[]) {
    if (q->count != 0) {
        command = dequeue(q);
        char *p = strtok(command, " ");
        if (strcmp("MOVE_RIGHT", p) == 0) MOVE_RIGHT(t);
        if (strcmp("MOVE_LEFT", p) == 0) MOVE_LEFT(t);
        if (strcmp("WRITE", p) == 0) WRITE(t, p + 6);
        if (strcmp("CLEAR_CELL", p) == 0) CLEAR_CELL(t);
        if (strcmp("CLEAR_ALL", p) == 0) CLEAR_ALL(t);
        if (strcmp("INSERT_RIGHT", p) == 0) INSERT_RIGHT(t, p + 13);
        if (strcmp("INSERT_LEFT", p) == 0) INSERT_LEFT(t, p + 12, Output);
        if (strcmp("SEARCH", p) == 0) SEARCH(t, p + 7, Output);
        if (strcmp("SEARCH_RIGHT", p) == 0) SEARCH_RIGHT(t, p + 13, Output);
        if (strcmp("SEARCH_LEFT", p) == 0) SEARCH_LEFT(t, p + 12, Output);
    }
}
// FUNCTIE CARE CITESTE DIN FISIER SI APLICA FUNCTIILE NECESARE
void read_file(train *t, queue *q, FILE *Input, FILE *Output, char command[]) {
    int i = 0, N;
    fscanf(Input, "%d", &N);
    fgetc(Input);
    for (i = 0; i < N; i++) {
        fgets(command, 20, Input);
        if (command[strlen(command) - 1] == '\n')
            command[strlen(command) - 1] = '\0';
        char aux[20];
        strcpy(aux, command);
        char *p = strtok(command, " ");
        if (strcmp("EXECUTE", p) == 0) {
            EXECUTE(t, q, Output, aux);
        } else if (strcmp("SHOW_CURRENT", p) == 0) {
            SHOW_CURRENT(t, Output);
        } else if (strcmp("SHOW", p) == 0) {
            SHOW(t, Output);
        } else if (strcmp("SWITCH", p) == 0) {
            SWITCH(q);

        } else {
            enqueue(q, aux);
        }
    }
}
int main() {
    FILE *Input, *Output;
    Input = fopen("tema1.in", "r");
    Output = fopen("tema1.out", "w");

    train *t;
    init_train(&t);
    if (t == NULL) printf("fail");

    char command[20];
    queue *q = init_queue();
    read_file(t, q, Input, Output, command);
    
    free(t);
    fclose(Input);
    fclose(Output);
    return 0;
}
