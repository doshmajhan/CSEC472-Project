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


int check_creds(char *username, char *password, MYSQL *conn){
    /*
        Checks if the entered credentials match any in the database  
        :param username: the username to be checked
        :param password: the password to be checked
        :param conn: the connection object for MySQL
        :returns id: the id of the user if they existed, 0 if they didn't
    */
    MYSQL_BIND params[2];
    MYSQL_BIND result[1];
    MYSQL_STMT *stmt;
    int id = 0;
    unsigned long username_len = strlen(username);
    unsigned long password_len = strlen(password);
    my_bool is_null[1];

    memset(params, 0, sizeof(params));
    memset(result, 0, sizeof(result));

    char *query = "SELECT id FROM users WHERE username=? AND password=?"; 
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
    
    params[0].buffer_type = MYSQL_TYPE_STRING;
    params[0].buffer = (char*)username;
    params[0].buffer_length = strlen(username);
    params[0].is_null = 0;
    params[0].length = &username_len;
    
    params[1].buffer_type = MYSQL_TYPE_STRING;
    params[1].buffer = (char*)password;
    params[1].buffer_length = strlen(password);
    params[1].is_null = 0;
    params[1].length = &password_len;
    
    unsigned long result_len = 0;
    result[0].buffer_type = MYSQL_TYPE_LONG;
    result[0].buffer = &id;
    result[0].buffer_length = sizeof(int);
    result[0].is_null = &is_null[0];
    result[0].length = &result_len;

    mysql_stmt_bind_param(stmt, params);
    mysql_stmt_bind_result(stmt, result);
    if (mysql_stmt_execute(stmt)){
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }

    if(mysql_stmt_store_result(stmt)){
        fprintf(stderr, "failed\n");
        fprintf(stderr, "%s\n", mysql_stmt_error(stmt));
    }
    mysql_stmt_fetch(stmt);
    printf("%d\n", id);
    mysql_stmt_close(stmt);

    return id;
}

void create_user(char *username, char *password, MYSQL *conn){
    /*
     *  Creates a new user in the MySQL database
     *
     *  :param username: the username of the account to be created
     *  :param password: the password of the account to be created
     *  :param conn: the connection object for MySQL
     */
    MYSQL_BIND params[2];
    MYSQL_STMT *stmt;
    unsigned long username_len = strlen(username);
    unsigned long password_len = strlen(password);

    memset(params, 0, sizeof(params));

    char *query = "INSERT INTO users (username, password) VALUES(?, ?)"; 
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
    
    params[0].buffer_type = MYSQL_TYPE_STRING;
    params[0].buffer = (char*)username;
    params[0].buffer_length = strlen(username);
    params[0].is_null = 0;
    params[0].length = &username_len;
    
    params[1].buffer_type = MYSQL_TYPE_STRING;
    params[1].buffer = (char*)password;
    params[1].buffer_length = strlen(password);
    params[1].is_null = 0;
    params[1].length = &password_len;

    mysql_stmt_bind_param(stmt, params);
    if (mysql_stmt_execute(stmt)){
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    printf("User created\n");
}


void change_password(char *username, MYSQL *conn){
    /*
     *  Changes the users password in the MySQL database
     *
     *  :param username: the username of the account to be created
     *  :param conn: the connection object for MySQL
     */
    MYSQL_BIND params[2];
    MYSQL_STMT *stmt;
    char *password;
    int toolong, ch;
    size_t len = 20;

    if ((password = malloc(len)) == NULL) {
        fprintf(stderr, "out of memory\n");
    }
    printf("New password: ");
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
    password[strlen(password)-1] = '\0';

    unsigned long username_len = strlen(username);
    unsigned long password_len = strlen(password);

    memset(params, 0, sizeof(params));

    char *query = "UPDATE users SET password=? WHERE username=?"; 
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
    params[0].buffer_type = MYSQL_TYPE_STRING;
    params[0].buffer = (char*)password;
    params[0].buffer_length = strlen(password);
    params[0].is_null = 0;
    params[0].length = &password_len;

   
    params[1].buffer_type = MYSQL_TYPE_STRING;
    params[1].buffer = (char*)username;
    params[1].buffer_length = strlen(username);
    params[1].is_null = 0;
    params[1].length = &username_len;
    
    mysql_stmt_bind_param(stmt, params);
    if (mysql_stmt_execute(stmt)){
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(1);
    }
    printf("Password changed\n");
}


void login(){
    /* 
     *  Prompts the user to login with their username and password, if
     *  they account doesn't exist they can create it
     */
    char * username;
    char * password;
    int toolong, ch;
    int id = 0;
    size_t len = 20;
    MYSQL *conn = connect_db();
    printf("Username: ");

    if ((username = malloc(len)) == NULL) {
        fprintf(stderr, "out of memory\n");
        exit(1);
    }
    if ((password = malloc(len)) == NULL) {
        fprintf(stderr, "out of memory\n");
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
    password[strlen(password)-1] = '\0';

    id = check_creds(username, password, conn);
    if(id){
        printf("Your id is: %d\n", id);
        printf("Would you like to change the password?(Y/N):");
        int answer = getchar();
        getchar(); // fix this to loop over remaining buffer
        printf("%c\n", answer);
        if(answer == 'Y'){
            change_password(username, conn);
        }
    }
    else {
        printf("User account doesn't exist\n");
        printf("Would you like to create it?(Y/N):");
        int answer = getchar();
        printf("%c\n", answer);
        if(answer == 'Y'){
            create_user(username, password, conn);
        }
    }
}


int main(int argc, char *argv[]){
    (void)argv;
    (void)argc;
    login();
    return EXIT_SUCCESS;
}
