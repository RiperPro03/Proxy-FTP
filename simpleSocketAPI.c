#include  <stdio.h>
#include  <sys/socket.h>
#include  <netdb.h>
#include  <string.h>
#include  <unistd.h>
#include  <stdbool.h>

int connect2Server(const char *serverName, const char *port, int *descSock){

    int ecode;                     // Retour des fonctions
	struct addrinfo *res,*resPtr;  // Résultat de la fonction getaddrinfo
	struct addrinfo hints;		   // Structure pour contrôler getaddrinfo
	bool isConnected;      // booléen indiquant que l'on est bien connecté
    
    // Initailisation de hints
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;  // TCP
	hints.ai_family = AF_UNSPEC;      // les adresses IPv4 et IPv6 seront présentées par 
				                      // la fonction getaddrinfo

	//Récupération des informations sur le serveur
	ecode = getaddrinfo(serverName,port,&hints,&res);
	if (ecode){
		fprintf(stderr,"getaddrinfo: %s\n", gai_strerror(ecode));
		return -1;
	}

	resPtr = res;

	isConnected = false;

	while(!isConnected && resPtr!=NULL){

		//Création de la socket IPv4/TCP
		*descSock = socket(resPtr->ai_family, resPtr->ai_socktype, resPtr->ai_protocol);
		if (*descSock == -1) {
			perror("Erreur creation socket");
			return -1;
		}
  
  		//Connexion au serveur
		ecode = connect(*descSock, resPtr->ai_addr, resPtr->ai_addrlen);
		if (ecode == -1) {
			resPtr = resPtr->ai_next;    		
			close(*descSock);	
		}
		// On a pu se connecter
		else isConnected = true;
	}
	freeaddrinfo(res);
	
	// On retourne -1 si pas possible de se connecter
	if (!isConnected){
		perror("Connexion impossible");
		return -1;
	}

	//On retourne 0 si on a pu établir la connexion TCP
    return 0;
}