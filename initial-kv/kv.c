#include <string.h>
#include <stdio.h>
#include <stdlib.h>

char db_file[] = "database.txt";
/*
 * Database format: Count (int) Key (int) str_len(int) str Key strlen str ... 
 * */
typedef struct db {
    int k;
    int v_len;
    char *v;
    struct db *next;
} db_t;

typedef struct list_t {
    db_t *head; // This will point to the dummy node
} list_t;

void init_node(db_t *node, int k, char *str){
    node->k = k;
    node->v_len = strlen(str);
    node->v = malloc(node->v_len * sizeof(char));
    strcpy(node->v, str);
}

void insert_node(list_t *head, db_t *node){
    node->next = head->next;
    head->next = node;
}

void clear(list_t *head){
    db_t *node = head-next;
    while (head->next){
        node = head;
        head = head->next;
        
        free(node->v);
        free(node);
    }
    FILE *fp = fopen(db_file, "w");
    int length = 0;
    fwrite(&length, sizeof(int), 1, fp);
    fclose(fp);
}
void print_all(db_t **head){
    fprintf(stdout, "test");
    db_t *node = (*head)->next;
    while (node){
        fprintf(stdout, "%d,%s\n", node->k, node->v);
        node = node->next;
         fprintf(stdout, "test");
    
    }
}

int main(int argc, char *argv[]){
    int count = 0;
    db_t *node = NULL;
    int i;    
    
    db_t *db_head = malloc(sizeof(db_t));
    db_head->next = NULL;
    FILE *fp = fopen(db_file, "r");
    
    if (fp == NULL) {
        fp = fopen(db_file, "w+"); 
        int tmp = 0;
        fwrite(&tmp, sizeof(int), 1, fp);
    } else {
        fread(&count, sizeof(int), 1, fp);
     
        // Read database into memory
        for (i = 0; i < count; i++){
            node = malloc(sizeof(db_t));
            node->next = NULL;
            // read data into node 
            fread(&node->k, sizeof(int), 1, fp);
            fread(&node->v_len, sizeof(int), 1, fp);
            node->v = malloc((node->v_len + 1) * sizeof(char));
            fread(node->v, sizeof(char), node->v_len, fp);
            // add node into db list
            insert_node(&db_head, &node);
            
        }
    }
    fclose(fp);
    print_all(&db_head);

    if (argc < 2)
        return 0;
 
    for (i = 1; i < argc; i++){
        char *token = strsep(&argv[i], ",");
        char *act = malloc(strlen(token) * sizeof(char)); 
        strcpy(act, token);
        token = strsep(&argv[i], ",");
        int key = atoi(token); 
        token = strsep(&argv[i], ",");
        char *str = malloc(strlen(token) * sizeof(char));
        strcpy(str, token);

        fprintf(stdout, "%s, %d, %s\n", act, key, str);        
        if (strcmp(act, "p") == 0){
            // Put key into database
            node = db_head->next;
            while (node){
                if(node->k == key){
                    strcpy(node->v, str);
                    node->v_len = strlen(node->v); 
                    break;
                }
                node = node->next;
            } 
            if (node == NULL){
                node = malloc(sizeof(db_t));
                init_node(node, key, str);
                
                insert_node(&db_head, &node);
                // no found, add new data into database
                fp = fopen(db_file, "a+"); 
                fseek(fp, 0, SEEK_END); 
                fwrite(&node->k, sizeof(int), 1, fp);
                fwrite(&node->v_len, sizeof(int), 1, fp);
                fwrite(node->v, sizeof(char), node->v_len, fp);
            }
        } else if (strcmp(act, "g")){
            // Get key from database
            node = db_head->next;
            while (node) {
                if (node->k == key){
                    fprintf(stdout, "%d,%s\n", node->k, node->v);
                    break;
                }
                node = node->next;
            }            
        } else if (strcmp(act, "d")){
            // delete key
            node = db_head->next;
            db_t *prev = db_head;
            while (node) {
                if (node->k == key){
                    prev->next = node->next;
                    free(node->v);
                    free(node); 
                    break;
                }
                prev = node;
                node = node->next;
            }
        } else if (strcmp(act, "c")){
            // clear all data
            clear(db_head);
        } else if (strcmp(act, "a")){
            // print all data
            print_all(&db_head);
        }
    } 
    return 0;
}
