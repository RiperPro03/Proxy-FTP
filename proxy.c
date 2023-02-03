#include  <stdio.h>
#include  <stdlib.h>
#include  <sys/socket.h>
#include  <netdb.h>
#include  <string.h>
#include  <unistd.h>
#include  <stdbool.h>
#include "./simpleSocketAPI.h"


#define SERVADDR "127.0.0.1"        // Définition de l'adresse IP d'écoute
#define SERVPORT "0"
                                    // Définition du port d'écoute, si 0 port choisi dynamiquement
#define LISTENLEN 1                 // Taille de la file des demandes de connexion
#define MAXBUFFERLEN 1024           // Taille du tampon pour les échanges de données
#define MAXHOSTLEN 64               // Taille d'un nom de machine
#define MAXPORTLEN 64               // Taille d'un numéro de port

int main(){

    // variable serveur
    int ecode;                       // Code retour des fonctions
    char pyServerAddr[MAXHOSTLEN];     // Adresse du serveur
    char pyServerPort[MAXPORTLEN];     // Port du server
    int descSockRDVpyClient;                 // Descripteur de socket de rendez-vous
    int descSockCOMpyCl;                 // Descripteur de socket de communication
    struct addrinfo hints;           // Contrôle la fonction getaddrinfo
    struct addrinfo *res;            // Contient le résultat de la fonction getaddrinfo
    struct sockaddr_storage myinfo;  // Informations sur la connexion de RDV
    struct sockaddr_storage from;    // Informations sur le client connecté
    socklen_t len;                   // Variable utilisée pour stocker les
				                             // longueurs des structures de socket
    char buffer[MAXBUFFERLEN];       // Tampon de communication entre le client et le serveur

    //variable client
    int descSockCOMpyClient;                  // Descripteur de la socket
  	int ecodePyClient;                     // Retour des fonctions
  	struct addrinfo *resPyClient,*resPtrPyClient;  // Résultat de la fonction getaddrinfo
  	struct addrinfo hintsPyClient;
  	char serverNamePyClient[MAXHOSTLEN];   // Nom de la machine serveur
  	char serverPortPyClient[MAXPORTLEN];   // Numéro de port
  	char bufferPyClient[MAXBUFFERLEN];     // Buffer stockant les messages entre
                                    // le client et le serveur
  	bool isConnected = false;      // booléen indiquant que l'on est bien connecté
    int descSockCOMpyServ;
    int * descSockCOMpyServAddresse = &descSockCOMpyServ;                 // Descripteur de socket
    char commande[100] ;
    char login[100];


    // Initialisation de la socket de RDV IPv4/TCP
    descSockRDVpyClient = socket(AF_INET, SOCK_STREAM, 0);
    if (descSockRDVpyClient == -1) {
         perror("Erreur création socket RDV\n");
         exit(2);
    }
    // Publication de la socket au niveau du système
    // Assignation d'une adresse IP et un numéro de port
    // Mise à zéro de hints
    memset(&hints, 0, sizeof(hints));
    // Initialisation de hints
    hints.ai_flags = AI_PASSIVE;      // mode serveur, nous allons utiliser la fonction bind
    hints.ai_socktype = SOCK_STREAM;  // TCP
    hints.ai_family = AF_INET;        // seules les adresses IPv4 seront présentées par
				                      // la fonction getaddrinfo

     // Récupération des informations du serveur
     ecode = getaddrinfo(SERVADDR, SERVPORT, &hints, &res);
     if (ecode) {
         fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(ecode));
         exit(1);
     }
     // Publication de la socket
     ecode = bind(descSockRDVpyClient, res->ai_addr, res->ai_addrlen);
     if (ecode == -1) {
         perror("Erreur liaison de la socket de RDV");
         exit(3);
     }
     // Nous n'avons plus besoin de cette liste chainée addrinfo
     freeaddrinfo(res);

     // Récuppération du nom de la machine et du numéro de port pour affichage à l'écran
     len=sizeof(struct sockaddr_storage);
     ecode=getsockname(descSockRDVpyClient, (struct sockaddr *) &myinfo, &len);
     if (ecode == -1)
     {
         perror("SERVEUR: getsockname");
         exit(4);
     }
     ecode = getnameinfo((struct sockaddr*)&myinfo, sizeof(myinfo), pyServerAddr,MAXHOSTLEN,
                         pyServerPort, MAXPORTLEN, NI_NUMERICHOST | NI_NUMERICSERV);
     if (ecode != 0) {
             fprintf(stderr, "error in getnameinfo: %s\n", gai_strerror(ecode));
             exit(4);
     }
     printf("L'adresse d'ecoute est: %s\n", pyServerAddr);
     printf("Le port d'ecoute est: %s\n", pyServerPort);

     // Definition de la taille du tampon contenant les demandes de connexion
     ecode = listen(descSockRDVpyClient, LISTENLEN);
     if (ecode == -1) {
         perror("Erreur initialisation buffer d'écoute");
         exit(5);
     }

	len = sizeof(struct sockaddr_storage);
     // Attente connexion du client
     // Lorsque demande de connexion, creation d'une socket de communication avec le client
     descSockCOMpyCl = accept(descSockRDVpyClient, (struct sockaddr *) &from, &len);
     if (descSockCOMpyCl == -1){
         perror("Erreur accept\n");
         exit(6);
     }
    // Echange de données avec le client connecté



    /*****
     * Envoi d'un message de login au user
     * **/
    strcpy(buffer, "220 Saissisez une requete, format: commande login@server\n");
    write(descSockCOMpyCl, buffer, strlen(buffer));

     //lire le message du user
   	ecode = read(descSockCOMpyCl, buffer, MAXBUFFERLEN);
   	if (ecode == -1) {perror("Problème de lecture\n"); exit(3);}
   	buffer[ecode] = '\0';
   	printf("MESSAGE RECU DU client: \"%s\".\n",buffer);

    sscanf(buffer,"%[^@]@%[^\n]",login,serverNamePyClient);
    serverNamePyClient[strlen(serverNamePyClient)-1]='\0';
    printf("Connexion au serveur : %s\n sous l'utilisateur%s\n",serverNamePyClient,login);
    strcat(login,"\r\n");
    //connextion ver le serveur distant
    strcpy(serverPortPyClient,"21");
    ecode = connect2Server(serverNamePyClient,serverPortPyClient,descSockCOMpyServAddresse);
    if (ecode == -1) {
        perror("Problème de connexion au serveur FTP\n"); exit(3);
    } else {
        isConnected = true;
    }

    //Message du serveur distant
    //exemple de la connexion à un serveur distant
    //Reponse du serveur distant
    ecode = read(descSockCOMpyServ, buffer, MAXBUFFERLEN);
   	if (ecode == -1) {perror("Problème de lecture\n"); exit(3);}
   	buffer[ecode] = '\0';
   	printf("MESSAGE RECU DU SERVEUR: message Connexion:\n \"%s\" \n ",buffer);

    //Envoi du message au user
    //write(descSockCOMpyCl, buffer, strlen(buffer));

    //envoie du login avec commande user au serveur
    write(descSockCOMpyServ,login,strlen(login));

    //write(descSockCOMpyServ,"USER anonymous\r",15);
    //lecture des messages du serveur
    ecode = read(descSockCOMpyServ, buffer, MAXBUFFERLEN);
    if (ecode == -1) {perror("Problème de lecture\n"); exit(3);}
    buffer[ecode] = '\0';
    printf("MESSAGE RECU DU SERVEUR: message demande de mot de passe:\n \"%s\"\n",buffer);
    //envoie du message du serveur au client
    write(descSockCOMpyCl, buffer, strlen(buffer));
    //renvoie du message password required

    //lecture du message du client
    ecode = read(descSockCOMpyCl, buffer, MAXBUFFERLEN);
    if (ecode == -1) {perror("Problème de lecture\n"); exit(3);}
    buffer[ecode] = '\0';
    printf("MESSAGE RECU DU client: pass du client:\n  \"%s\".\n",buffer);
    //envoie du PASS du client au serveur
    
    write(descSockCOMpyServ, buffer, strlen(buffer));

    //Retour du message de serveur au client
    ecode = read(descSockCOMpyServ, buffer, MAXBUFFERLEN);
   	if (ecode == -1) {perror("Problème de lecture\n"); exit(3);}
   	buffer[ecode] = '\0';
   	printf("MESSAGE RECU DU serveur: \"%s\".\n",buffer);

    //envoie du ok connecte au client
    write(descSockCOMpyCl, buffer, strlen(buffer));

    // BLOC ALLER RETOUR ***
    do
    {
        ecode = read(descSockCOMpyCl, buffer, MAXBUFFERLEN);
        if (ecode == -1)
        {
            perror("Problème de lecture\n");
            exit(3);
        }
        buffer[ecode] = '\0';
        printf("MESSAGE RECU DU Client: \"%s\".\n", buffer);

        write(descSockCOMpyServ, buffer, strlen(buffer));

        // Retour du message de serveur au client
        ecode = read(descSockCOMpyServ, buffer, MAXBUFFERLEN);
        if (ecode == -1)
        {
            perror("Problème de lecture\n");
            exit(3);
        }
        buffer[ecode] = '\0';
        printf("MESSAGE RECU DU serveur: \"%s\".\n", buffer);

        // envoie de la reponse au client
        write(descSockCOMpyCl, buffer, strlen(buffer));
    } while (isConnected);
    //FIN DE BLOC ALLER RETOUR ****


    //Fermeture de la connexion
    close(descSockCOMpyServ);
    close(descSockCOMpyCl);
    close(descSockRDVpyClient);
}
