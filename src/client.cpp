#include <stdio.h>
#include <stdlib.h>
#include <cjson/cJSON.h>

#define PROJECT_NAME "client"

int main(int argc, char **argv) {
    if(argc != 1) {
        printf("%s takes no arguments.\n", argv[0]);
        return 1;
    }
    printf("This is project %s.\n", PROJECT_NAME);


    // Criar um objeto JSON
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "name", "OpenAI");
    cJSON_AddNumberToObject(root, "age", 5);
    
    // Serializar o objeto JSON
    char *json_string = cJSON_Print(root);
    printf("JSON output:\n%s\n", json_string);
    
    // Liberar memória
    cJSON_Delete(root);
    free(json_string);  // Agora a função free será reconhecida


    return 0;
}
