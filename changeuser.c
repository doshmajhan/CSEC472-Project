#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


MYSQL connect_db(){
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


int check_creds(char *username, char *password){
    /*
        Checks if the entered credentials match any in the database  

    */
    MYSQL_RES *res;
    MYSQL_ROW row;
    MYSQL_BIND bind[2];
    MYSQL_STMT *stmt;
    char *query = "SELECT username FROM users WHERE username='?' AND password='?'"; 
    stmt = mysql_stmt_init(conn);
    if(!stmt){
        fprintf(stderr, "mysql_stmt_init(), out of memory\n");
        exit(1);
    }
    if(mysql_stmt_prepare(conn, query, strlen(query))){
        fprintf(stderr, "\n mysql_stmt_prepare(), INSERT failed");
        fprintf(stderr, "\n %s", mysql_stmt_error(stmt));
        exit(1);
    }
    bind[0].buffer_type = MYSQL_TYPE_STRING;
    bind[0].buffer = username;
    bind[0].buffer_length = strlen(username);
    bind[0].is_null = 0;
    bind[0].length = 0;
    
    bind[1].buffer_type = MYSQL_TYPE_STRING;
    bind[1].buffer = password;
    bind[1].buffer_length = strlen(password);
    bind[1].is_null = 0;
    bind[1].length = 0;

    mysql_stmt_bind_param(stmt, bind);

    if (mysql_stmt_execute(stmt)){
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    // do stuff ehre for checking
    res = mysql_use_result(conn);
    
    while((row = mysql_fetch_row(res)) != NULL){
        printf("%s \n", row[0]);
    }
    mysql_free_result(res);
    
}

int main(int argc, char *argv[]){
    login(); 
    return EXIT_SUCCESS;
}
