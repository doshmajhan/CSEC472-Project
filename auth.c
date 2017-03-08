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
    const char *username;
    pam_handle_t *pamh=NULL;
    int retval;
    
    retval = pam_start("login", username, &conv, &pamh);
    retval = pam_get_user(pamh, &username, "Username: ");
    if(retval != PAM_SUCCESS){

        return retval;
    }
    retval = pam_start("check_user", username, &conv, &pamh);
    if(retval == PAM_SUCCESS){
        retval = pam_authenticate(pamh, 0);
    }
    if(retval == PAM_SUCCESS){
        retval = pam_acct_mgmt(pamh, 0);
    }

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
    pam_creds_check();
    return EXIT_SUCCESS;

}
