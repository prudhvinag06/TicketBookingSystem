#include<sys/types.h>
#include<sys/socket.h>
#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include <arpa/inet.h> 
#include<netinet/in.h>
#include <string.h>
#define port 12322
int isAuthenticated(int client_number, char client_username[], char client_type[]);
void helper(int sd, int client_id, char client_type[30]);
void viewRecords(int sd);
void signup(int sd);
void deleteUser(int sd);
void searchUser(int sd);
void viewTrains(int sd);
void addTrains(int sd);
void deleteTrain(int sd);
void searchTrain(int sd);


/*---------------------------------------------Client-------------------------------------------*/
int main(int argc, char * argv[]){
    struct sockaddr_in serv;
    char client_name[20], client_password[20], client_type[20];
    int client_id;
    int sd, connection_status, type;
    int client_number = 23;
    sd = socket(AF_INET, SOCK_STREAM, 0);
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY;   //inet_addr("ser ip") //in case of not a local host  
    serv.sin_port = htons(port);
    connection_status = connect(sd, (struct sockaddr *) &serv, sizeof(serv));
    if(connection_status == -1){
        printf("Error\n");
    }
/*--------------------------------------------Initialization------------------------------------*/
    int choice;
    write(sd, &client_number, sizeof(client_number));
    //read(sd, &client_number, sizeof(client_number));
    //printf("Server Says : %d\n", client_number);
    printf("Select the type : \n");
    printf("1. Login\n");
    printf("2. Sign Up\n");
    scanf("%d", &choice);
    while(choice != 1 && choice != 2){
        printf("Warning : Please select the valid option\n");
        scanf("%d", &choice);
    }
    if(choice == 1){ //Login 
        printf("Enter user id : ");
        scanf("%d", &client_id);
        printf("Enter Name : ");
        scanf("%s", client_name);
        printf("Enter password : \n");
        scanf("%s", client_password);
        printf("Enter type of User : \n");
        printf("1. Customer\n");
        printf("2. Admin\n");
        printf("3. Agent\n");
        scanf("%d", &type);
        if(type == 1){
            strcpy(client_type, "customer");
        }
        else if(type == 2){
            strcpy(client_type, "admin");
        }
        else{
            strcpy(client_type, "agent");
        }
        // write(sd, &client_id, sizeof(client_id));
        int val = 1;
        write(sd, &val, sizeof(val));
        write(sd, &client_id, sizeof(client_id));
        write(sd, &client_name, sizeof(client_name));
        write(sd, &client_password, sizeof(client_password));
        write(sd, &client_type, sizeof(client_type));
        int auth = 0;
        //authenticate password
        read(sd, &auth, sizeof(auth));
        if(auth == -1){
            printf("User Doesn't Exist in Database\n");
        }
        else{
            while(auth == 0){
                printf("Error!. Please Enter Password again \n");
                scanf("%s", client_password);
                write(sd, &client_password, sizeof(client_password));
                read(sd, &auth, sizeof(auth));
                printf("Auth value : %d\n", auth);
            }
        printf("\tLOGGED IN SUCCESSFULLY\n");
        helper(sd, client_id, client_type);
        }
        
        /*
        Can add if user doesn't exist u can call signup function
        */
    }
    else if(choice == 2){//Signup
        //signup();
        // printf("Enter user id : \n");
        // scanf("%d", &client_id);
        int user_id;
        printf("Enter Name : \n");
        scanf("%s", client_name);
        printf("Enter password : \n");
        scanf("%s", client_password);
        printf("Select Type of user : \n");
        printf("1. Customer\n");
        printf("2. Admin\n");
        printf("3. Agent\n");
        scanf("%d", &type);
        if(type == 1){
            strcpy(client_type, "customer");
        }
        else if(type == 2){
            strcpy(client_type, "admin");
        }
        else{
            strcpy(client_type, "agent");
        }
        // write(sd, &client_id, sizeof(client_id));
        int val = 2;
        write(sd, &val, sizeof(val));
        write(sd, &client_name, sizeof(client_name));
        write(sd, &client_password, sizeof(client_password));
        write(sd, &client_type, sizeof(client_type));
        read(sd, &user_id, sizeof(user_id));
        printf("Login id of user : %d\n", user_id);
    }
}

/*--------------------------------------------helper()---------------------------------------------------------------*/
void helper(int sd, int client_id, char client_type[30]){
    int type = -1;
    if(strcmp(client_type, "customer") == 0){
        type = 1;
    }
    else if(strcmp(client_type, "agent") == 0){
        type = 2; 
    }
    else type = 3;
    if(type == 1 || type == 2){
        int choice;
        printf("Select Options Below : \n");
        printf("1. Book Ticket\n");
		printf("2. View Bookings\n");
		printf("3. Update Booking\n");
		printf("4. Cancel booking\n");
        scanf("%d", &choice);
        write(sd, &choice, sizeof(choice));

    }
    else if(type == 3){
        int choice;
        printf("Select Options You Want To Add/Update Below : \n");
        printf("1. User Account \n");
        printf("2. Train Accounts \n");
        scanf("%d", &choice);
        write(sd, &choice, sizeof(choice));
        
        
        
        if(choice == 1){ //users
            printf("1. View Users \n");
            printf("2. Add User\n");
            //call modified signup() function
            printf("3. Delete User\n");
            printf("4. Modify User\n");
            printf("5. Search\n");
            scanf("%d", &choice);
            printf("choice : %d\n", choice);
            int x = write(sd, &choice, sizeof(choice));
            printf("%d\n",x);
            switch (choice)
            {
            case 1:
            viewRecords(sd);
            break;
            case 2:
            signup(sd);
            break;
            case 3:
            deleteUser(sd);
            break;
            case 4:
            //modifyUser();
            break;
            case 5:
            searchUser(sd);
            break;
            default:
            break;
            }
        }
        else if(choice == 2){ //trains
            printf("1. View Trains\n");
            printf("2. Add Train\n");
            printf("3. Delete Train\n");
            printf("4. Modify Train Details\n");
            printf("5. Search Train\n");
            scanf("%d", &choice);
            write(sd, &choice, sizeof(choice));
            switch (choice)
            {
            case 1:
            viewTrains(sd);
            break;
            case 2:
            addTrains(sd);
            break;
            case 3:
            deleteTrain(sd);
            break;
            case 4:
            //modifyTrain();
            break;
            case 5:
            searchTrain(sd);
            break;
            default:
            break;
            }
        }
    }

}
void viewRecords(int sd){
   int user_id, count = 0;
   char user_name[30];
   read(sd, &count, sizeof(count));
   if(count > 0){
        printf("Total Number of users : %d\n", count);
        while(count--){
            read(sd, &user_name, sizeof(user_name));
            read(sd, &user_id, sizeof(user_id));
            printf("USER ID: %d  USER NAME : %s\n", user_id, user_name);
        }
   }
   else{
       printf("<--------NO USERS IN DB--------->\n");
   }
}

void signup(int sd){
        int user_id, type, id;
        char client_name[20], client_password[20], client_type[20];
        printf("Enter Name : \n");
        scanf("%s", client_name);
        printf("Enter password : \n");
        scanf("%s", client_password);
        printf("Select Type of user : \n");
        printf("1. Customer\n");
        printf("2. Admin\n");
        printf("3. Agent\n");
        scanf("%d", &type);
        if(type == 1){
            strcpy(client_type, "customer");
        }
        else if(type == 2){
            strcpy(client_type, "admin");
        }
        else{
            strcpy(client_type, "agent");
        }
        int x = write(sd, &client_name, sizeof(client_name));
        printf("x : %d",x);
        sleep(1);
        write(sd, &client_password, sizeof(client_password));
        sleep(1);

        write(sd, &client_type, sizeof(client_type));
        sleep(5);
        read(sd, &user_id, sizeof(user_id));
        printf("Login id of user : %d\n", user_id);
}

void deleteUser(int sd){
    int user_id;
    printf("Select the User you want to delete : \n");
    scanf("%d", &user_id);
    write(sd, &user_id, sizeof(user_id));
    printf("<-----------------DELETION SUCCESS-------------------->\n");
}

void searchUser(int sd){
    int user_id, type;
    char name[30];
    printf("Select the User you want to search : \n");
    scanf("%d", &user_id);
    write(sd, &user_id, sizeof(user_id));
    read(sd, &name, sizeof(name));
    read(sd, &user_id, sizeof(user_id));
    read(sd, &type, sizeof(type));

    printf("<-----------------SEARCH SUCCESS-------------------->\n");
}


void viewTrains(int sd){
    int train_id, count = 0, seats_left = 0;
    char train_name[30], train_src[30], train_dest[30];
    read(sd, &count, sizeof(count));
    if(count > 0){
        printf("Total Number of users : %d\n", count);
        while(count--){
           
            read(sd, &train_name, sizeof(train_name));
            read(sd, &train_id, sizeof(train_id));
            read(sd, &train_src, sizeof(train_src));
            read(sd, &train_dest, sizeof(train_dest));
            read(sd, &seats_left, sizeof(seats_left));
            printf("TRAIN ID: %d  TRAIN NAME : %s\n", train_id, train_name);
            printf("TRAIN SRC: %s  TRAIN DEST : %s\n SEATS LEFT : %d\n", train_src, train_dest, seats_left);
            printf("********************************\n");
        }
    }
    else{
       printf("<--------NO TRAINS IN DB--------->\n");
    }
}

void addTrains(int sd){
    int x, total_seats, train_id;
    char train_name[30], train_source[30], train_dest[30];
    //read(sd, &x, sizeof(x));
    //printf("Testing check : %d\n", x);
    printf("Enter Train Name :\n");
    scanf("%s", train_name);
    printf("Enter Total Seats In Train : \n");
    scanf("%d", &total_seats);
    printf("Enter Starting Point : \n");
    scanf("%s", train_source);
    printf("Enter Destination Point : \n");
    scanf("%s", train_dest);
    write(sd, &train_name, sizeof(train_name));
    write(sd, &total_seats, sizeof(total_seats));
    write(sd, &train_source, sizeof(train_source));
    write(sd, &train_dest, sizeof(train_dest));

    read(sd, &train_id, sizeof(train_id));
    printf("Train id of train : %d\n", train_id);


}

void deleteTrain(int sd){

}

void searchTrain(int sd){

}