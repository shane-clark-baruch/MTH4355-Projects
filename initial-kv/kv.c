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

// Initialize a new list with a dummy head
list_t *list_init() {
    list_t *list = malloc(sizeof(list_t));
    if (!list) {
        perror("Failed to allocate list");
        return NULL;
    }
    list->head = malloc(sizeof(db_t));
    if (!list->head) {
        perror("Failed to allocate dummy head");
        free(list);
        return NULL;
    }
    list->head->next = NULL;
    return list;
}

// Initialize a new node with data
void init_node(db_t *node, int k, char *str){
    node->k = k;
    node->v_len = strlen(str);
    node->v = malloc((node->v_len + 1)* sizeof(char));
    strcpy(node->v, str);
}

void insert_node(list_t *list, db_t *node){
    node->next = list->head->next;
    list->head->next = node;
}

void clear(list_t *list){
    db_t *current = list->head->next;
    db_t *temp;
    while (current){
        temp = current->next;
        free(current->v);
        free(current);
        current = temp;
    }
    
    list->head->next = NULL;
    FILE *fp = fopen(db_file, "w");
    if (fp){
        int length = 0;
        fwrite(&length, sizeof(int), 1, fp);
        fclose(fp);
    }
}

void list_destroy(list_t *list){
    clear(list);
    free(list->head);
    free(list);
}

void print_all(list_t *list){
    db_t *node = list->head->next;
    while (node){
        fprintf(stdout, "%d,%s\n", node->k, node->v);
        node = node->next;
    }
}

void save_to_file(list_t *list){
    FILE *fp = fopen(db_file, "w");
    if (!fp) {
        perror("Failed to open database file for writing");
        return;
    }
    fclose(fp);
    
    fp = fopen(db_file, "wb");
    
    // First, count the nodes
    int count = 0;
    db_t *node = list->head->next;
    while (node) {
        count++;
        node = node->next;
    }
    printf("count list: %d\n", count);
    fwrite(&count, sizeof(int), 1, fp);

    node = list->head->next;
    while (node) {
        printf("count node: %d\n", node->k);
        fwrite(&node->k, sizeof(int), 1, fp);
        fwrite(&node->v_len, sizeof(int), 1, fp);
        fwrite(node->v, sizeof(char), node->v_len, fp);
        node = node->next;
    }
    fclose(fp);
}

int main(int argc, char *argv[]){
    list_t *db_list = list_init();
    if (db_list == NULL) return 1;

    FILE *fp = fopen(db_file, "r");
    db_t *node = NULL;

    if (fp == NULL) {
        fp = fopen(db_file, "w"); 
        if (fp) {
            int tmp = 0;
            fwrite(&tmp, sizeof(int), 1, fp);
            fclose(fp);
        }
    } else {
        int count = 0;
        fread(&count, sizeof(int), 1, fp);
     
        // Read database into memory
        for (int i = 0; i < count; i++){
            node = malloc(sizeof(db_t));
            // read data into node 
            fread(&node->k, sizeof(int), 1, fp);
            fread(&node->v_len, sizeof(int), 1, fp);
            node->v = malloc((node->v_len + 1) * sizeof(char));
            if (node->v == NULL) {
                free(node);
                break;
            }
            fread(node->v, sizeof(char), node->v_len, fp);
            node->v[node->v_len] = '\0';
            // add node into db list
            insert_node(db_list, node);
        }
        fclose(fp);
    }
    print_all(db_list);

    if (argc < 2){
        list_destroy(db_list);
        return 0;
    }
 
    for (int i = 1; i < argc; i++){
        char *arg_copy = strdup(argv[i]); 
        if (arg_copy == NULL) continue;
        
        char *stringp = arg_copy;
        char *act = strsep(&stringp, ",");
        if (act == NULL){
            free(arg_copy);
            continue;
        }

        if (strcmp(act, "p") == 0){
            // Put key into database
            char *key_str = strsep(&arg_copy, ",");
            char *val_str = strsep(&arg_copy, ",");
            if (key_str && val_str){
                char *str = malloc(strlen(val_str) + 1); 
                int key = atoi(key_str); 
                
                strcpy(str, val_str);

                db_t *node = db_list->head->next;
                while (node){
                    if(node->k == key){
                        free(node->v);
                        init_node(node, key, str);
                        free(str);
                        str = NULL;
                        break;
                    }
                    node = node->next;
                } 
                if (node == NULL){
                    db_t *new_node = malloc(sizeof(db_t));
                    if (new_node) {
                        init_node(new_node, key, str);
                    
                        insert_node(db_list, new_node);
                    }
                    if (str) free(str);
                    // no found, add new data into database
                    save_to_file(db_list); 
                }
            }
        } else if (strcmp(act, "g") == 0){
            // Get key from database
            char *key_str = strsep(&arg_copy, ",");
            int key = atoi(key_str); 
            if(key){ 
                node = db_list->head->next;
                while (node) {
                    if (node->k == key){
                        fprintf(stdout, "%d,%s\n", node->k, node->v);
                        break;
                    }
                    node = node->next;
                }      
            }      
        } else if (strcmp(act, "d") == 0){
            char *key_str = strsep(&arg_copy, ",");
            if (key_str) { 
                int key = atoi(key_str); 
    
                // delete key
                node = db_list->head->next;
                db_t *prev = db_list->head;
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
                save_to_file(db_list);
            }
        } else if (strcmp(act, "c") == 0){
            // clear all data
            clear(db_list);
            save_to_file(db_list);
        } else if (strcmp(act, "a") == 0){
            // print all data
            print_all(db_list);
        }
        free(arg_copy);
    } 
    list_destroy(db_list);
    return 0;
}
