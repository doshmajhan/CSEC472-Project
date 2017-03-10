#include <security/pam_appl.h>
#include <security/pam_misc.h>
#include <security/pam_modules.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static struct pam_conv conv = {
    misc_conv,
    NULL
};

int pam_creds_check(){
    /*
        Checks to see if a user can be authenticated to the system

        :param username: the username to be checked

        :returns retval: the status of the check, 1 if true 0 if not
    */
    char *username;
    pam_handle_t *pamh=NULL;
    int retval, ch, toolong;
    size_t len = 30;
    printf("Username: ");
    if ((username = malloc(len)) == NULL){
	fprintf(stderr, "Out of memory\n");
	exit(1);
    }
    if(fgets(username, len, stdin) == NULL){
	exit(1);
    }
    if(username[strlen(username)-1] != '\n'){
	toolong = 0;
	while(((ch = getchar()) != '\n') && (ch != EOF)){
	    toolong = 1;
	}
	if(toolong == 1){
	    printf("Input too long, max size is 30\n");
	    exit(1);
	}
    }
    username[strlen(username)-1] = '\0';

    retval = pam_start("login", username, &conv, &pamh);
    if(retval == PAM_SUCCESS){
        retval = pam_authenticate(pamh, 0);
    }
    if(retval == PAM_SUCCESS){
        retval = pam_acct_mgmt(pamh, 0);
    }
    printf("Username: %s\n", username);
    if(retval == PAM_SUCCESS){
        fprintf(stdout, "Authenticated\n");
        fprintf(stdout, "Change password?(Y/N):");
        int ans = getchar();
        if(ans == 'Y'){
            retval = pam_chauthtok(pamh, PAM_SILENT);
        }
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

int main(){
    int orig_uid = getuid();
    setuid(0);
    pam_creds_check();
    setuid(orig_uid);
    return EXIT_SUCCESS;

}
