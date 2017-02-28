#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static struct pam_conv conv = {
    misc_conv,
    NULL
};


int pam_creds_check(char *username){
    /*
        Checks to see if a user can be authenticated to the system

        :param username: the username to be checked

        :returns retval: the status of the check, 1 if true 0 if not
    */

    pam_handle_t *pamh=NULL;
    int retval;

    retval = pam_start("check_user", username, &conv, &pamh);
    if(retval == PAM_SUCCESS){
        retval = pam_authenticate(pamh, 0);
    }
    if(retval == PAM_SUCCESS){
        retval = pam_acct_mgmt(pamh, 0);
    }

    if(retval == PAM_SUCCESS){
        fprintf(stdout, "Authenticated\n");
    }
    else{
        fprintf(stdout, "Not Authenticated\n");
    }

    if(pam_end(pamh, retval) != PAM_SUCCESS){
        pamh = NULL;
        fprintf(stderr, "auth: failed to release authenticator\n");
        exit(1);
    }

    return (retval == PAM_SUCCESS ? 0:1);
}


void login(){
    /* 
        Prompts the user to login with their username and password, if
        they account doesn't exist they can create it
     */

    char * username;
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

    int reval = pam_creds_check(username);
}


int main(){
    login();
    return EXIT_SUCCESS;
}
