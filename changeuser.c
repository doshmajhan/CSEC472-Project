#include <mysql/my_global.h>
#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


MYSQL * connect_db(){
    MYSQL *conn;
    char *server = "localhost";
    char *user = "root";
    char *password = "itsvariable17";
    char *database = "capp";

    conn = mysql_init(NULL);

    if (!mysql_real_connect(conn, server, user, 
         password, database, 0, NULL, 0)) {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    return conn;
}


void login(){
    char * username;
    char * password;
    int toolong, ch;
    size_t len = 20;
    printf("Username: ");

    if ((username = malloc(len)) == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    if(fgets(username, len, stdin) == NULL){
        exit(1);
    }
    if(username[strlen(username)-1] != '\n'){
        toolong = 0;
        while (((ch = getchar()) != '\n') && (ch != EOF)){
            toolong = 1;
        }
        if(toolong == 1){
            printf("Input too long, max size is 20\n");
            exit(1);
        }
    }
    username[strlen(username)-1] = '\0';
    printf("Password: ");
    if ((password = malloc(len)) == NULL) {
        fprintf(stderr, "out of memory\n");
    }
    if(fgets(password, len, stdin) == NULL){
        exit(1);
    }
    if(password[strlen(password)-1] != '\n'){
        toolong = 0;
        while (((ch = getchar()) != '\n') && (ch != EOF)){
            toolong = 1;
        }
        if(toolong == 1){
            printf("Input too long, max size is 20\n");
            exit(1);
        }
    }
    printf("%s\n", username);
    printf("%s\n", password);
}


int check_creds(char *username, char *password, MYSQL *conn){
    /*
        Checks if the entered credentials match any in the database  

    */
    MYSQL_BIND params[2];
    MYSQL_BIND result[1];
    MYSQL_STMT *stmt;
    int id;
    my_bool is_null[1];

    memset(params, 0, sizeof(params));
    memset(result, 0, sizeof(result));

    char *query = "SELECT id FROM users WHERE username='?' AND password='?'"; 
    stmt = mysql_stmt_init(conn);
    if(!stmt){
        fprintf(stderr, "mysql_stmt_init(), out of memory\n");
        exit(1);
    }
    if(mysql_stmt_prepare(stmt, query, strlen(query))){
        fprintf(stderr, "\n mysql_stmt_prepare(), INSERT failed");
        fprintf(stderr, "\n %s", mysql_stmt_error(stmt));
        exit(1);
    }
    printf("Binding\n");
    params[0].buffer_type = MYSQL_TYPE_STRING;
    params[0].buffer = username;
    params[0].buffer_length = strlen(username);
    params[0].is_null = 0;
    params[0].length = 0;
    
    params[1].buffer_type = MYSQL_TYPE_STRING;
    params[1].buffer = password;
    params[1].buffer_length = strlen(password);
    params[1].is_null = 0;
    params[1].length = 0;

    unsigned long result_len = 0;
    result[0].buffer_type = MYSQL_TYPE_LONG;
    result[0].buffer = &id;
    result[0].buffer_length = sizeof(id);
    result[0].is_null = &is_null[0];
    result[0].length = &result_len;

    mysql_stmt_bind_param(stmt, params);
    mysql_stmt_bind_result(stmt, result);
    printf("Executing\n");
    if (mysql_stmt_execute(stmt)){
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    mysql_stmt_store_result(stmt);
    mysql_stmt_fetch(stmt);
    printf("%d\n", id);
    printf("Freeing\n");
    //mysql_free_result(stmt);
    mysql_stmt_close(stmt);

    return 0;
}

int main(int argc, char *argv[]){
    (void)argv;
    (void)argc;
    MYSQL *conn = connect_db();
    printf("Checkin\n");
    check_creds("cam", "clark", conn);
    //login(); 
    return EXIT_SUCCESS;
}
