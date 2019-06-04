// source. https://www.quora.com/How-do-I-create-a-web-server-in-pure-C

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "libnumberpic.h"
 
#define BUFSIZE 8096
#define ERROR 42
#define SORRY 43
#define LOG   44
 
struct {
	char *ext;
	char *filetype;
} extensions [] = {
	{"gif", "image/gif" },
	{"jpg", "image/jpeg"},
	{"jpeg","image/jpeg"},
	{"png", "image/png" },
	{"zip", "image/zip" },
	{"gz",  "image/gz"  },
	{"tar", "image/tar" },
	{"htm", "text/html" },
	{"html","text/html" },
	{"js","text/javascript" },
	{0,0} };
 
void wlog(int type, char *s1, char *s2, int num)
{
	int fd ;
	char logbuffer[BUFSIZE*2];
 
	switch (type) {
	case ERROR: (void)sprintf(logbuffer,"ERROR: %s:%s Errno=%d exiting pid=%d",s1, s2, errno,getpid()); break;
	case SORRY:
		(void)sprintf(logbuffer, "<HTML><BODY><H1>nweb Web Server Sorry: %s %s</H1></BODY></HTML>\r\n", s1, s2);
		(void)write(num,logbuffer,strlen(logbuffer));
		(void)sprintf(logbuffer,"SORRY: %s:%s",s1, s2);
		break;
	case LOG: (void)sprintf(logbuffer," INFO: %s:%s:%d",s1, s2,num); break;
	}
	/* no checks here, nothing can be done a failure anyway */
	if((fd = open("nweb.log", O_CREAT| O_WRONLY | O_APPEND,0644)) >= 0) {
		(void)write(fd,logbuffer,strlen(logbuffer));
		(void)write(fd,"\n",1);
		(void)close(fd);
	}
}

static int pix (int value, int max)
{
    if (value < 0) {
        return 0;
    }
    return (int) (256.0 *((double) (value)/(double) max));
}

 
/* this is a child web server process, so we can exit on errors */
void web(int fd, int hit)
{
	int j, file_fd, buflen, len;
	long i, ret;
	char * fstr;
	static char buffer[BUFSIZE+1]; /* static so zero filled */
 
	ret =read(fd,buffer,BUFSIZE); 	/* read Web request in one go */
 
	if(ret == 0 || ret == -1) {	/* read failure stop now */
		wlog(SORRY,"failed to read browser request","",fd);
	}
 
	if(ret > 0 && ret < BUFSIZE)	/* return code is valid chars */
		buffer[ret]=0;		/* terminate the buffer */
	else
		buffer[0]=0;
 
	for(i=0;i<ret;i++)	/* remove CF and LF characters */
		if(buffer[i] == '\r' || buffer[i] == '\n')
			buffer[i]='*';
 
	wlog(LOG,"request",buffer,hit);
 
	if( strncmp(buffer,"GET ",4) && strncmp(buffer,"get ",4) )
		wlog(SORRY,"Only simple GET operation supported",buffer,fd);
 
	for(i=4;i<BUFSIZE;i++) { /* null terminate after the second space to ignore extra stuff */
		if(buffer[i] == ' ' || buffer[i] == '?') { /* string is "GET URL " +lots of other stuff */
			buffer[i] = 0;
			break;
		}
	}
 
	for(j=0;j<i-1;j++) 	/* check for illegal parent directory use .. */
		if(buffer[j] == '.' && buffer[j+1] == '.')
			wlog(SORRY,"Parent directory (..) path names not supported",buffer,fd);
 
	if( !strncmp(&buffer[0],"GET /\0",6) || !strncmp(&buffer[0],"get /\0",6) ) /* convert no filename to index file */
		(void)strcpy(buffer,"GET /index.html");
 
	/* work out the file type and check we support it */
	buflen=strlen(buffer);
	fstr = (char *)0;
	for(i=0;extensions[i].ext != 0;i++) {
		len = strlen(extensions[i].ext);
		if( !strncmp(&buffer[buflen-len], extensions[i].ext, len)) {
			fstr =extensions[i].filetype;
			break;
		}
	}
	if(fstr == 0) wlog(SORRY,"file extension type not supported",buffer,fd);

	if(!strncmp(&buffer[buflen-3], "png", 3)) {
		fprintf(stderr, "Handling png %s\n", buffer);
		fflush(stderr);
		// special handling .png
		bitmap_t *bm = numberpic_mkbitmap(150, 100);
		for (int y = 0; y < bm->height; y++) {
    		    for (int x = 0; x < bm->width; x++) {
            		pixel_t * pixel = pixel_at (bm, x, y);
            		pixel->red = pix (x, bm->width);
            		pixel->green = pix (y, bm->height);
        	    }
   		}
		int i=0;
		while(buffer[5+i]>='0'&&buffer[5+i]<='9') {
		   int n = buffer[5+i]-'0';
		   placenum(n, bm, 20+14*i, 40);
		   placenum(n, bm, 20+14*i, 40);
		   i++;
		}
		FILE *f = fdopen(fd, "w");
		(void)sprintf(buffer,"HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n", fstr);
		(void)write(fd,buffer,strlen(buffer));
		save_png_to_FILE(bm, f);		
		fprintf(stderr, "Done\n");
		fflush(stderr);
		fclose(f);
	} else {
 
	if(( file_fd = open(&buffer[5],O_RDONLY)) == -1) /* open the file for reading */
		wlog(SORRY, "failed to open file",&buffer[5],fd);
 
	wlog(LOG,"SEND",&buffer[5],hit);
 
	(void)sprintf(buffer,"HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n", fstr);
	(void)write(fd,buffer,strlen(buffer));
 
	/* send file in 8KB block - last block may be smaller */
	while (	(ret = read(file_fd, buffer, BUFSIZE)) > 0 ) {
		(void)write(fd, buffer, ret);
	}
	}
}
 
 
int main(int argc, char **argv)
{
	int i, port, /*pid,*/ listenfd, socketfd, hit;
/*	size_t length; */
	unsigned int length;
	static struct sockaddr_in cli_addr; /* static = initialised to zeros */
	static struct sockaddr_in serv_addr; /* static = initialised to zeros */
 
	if( argc < 3  || argc > 3 || !strcmp(argv[1], "-?") ) {
		(void)printf("hint: nweb Port-Number Top-Directory\n\n"
	"\tnweb is a small and very safe mini web server\n"
	"\tnweb only servers out file/web pages with extensions named below\n"
	"\t and only from the named directory or its sub-directories.\n"
	"\tThere is no fancy features = safe and secure.\n\n"
	"\tExample: nweb 8181 /home/nwebdir &\n\n"
	"\tOnly Supports:");
		for(i=0;extensions[i].ext != 0;i++)
			(void)printf(" %s",extensions[i].ext);
 
		(void)printf("\n\tNot Supported: URLs including \"..\", Java, Javascript, CGI\n"
	"\tNot Supported: directories / /etc /bin /lib /tmp /usr /dev /sbin \n"
	"\tNo warranty given or implied\n\tNigel Griffiths nag@uk.ibm.com\n"
		    );
		exit(0);
	}
	if( !strncmp(argv[2],"/"   ,2 ) || !strncmp(argv[2],"/etc", 5 ) ||
	    !strncmp(argv[2],"/bin",5 ) || !strncmp(argv[2],"/lib", 5 ) ||
	    !strncmp(argv[2],"/tmp",5 ) || !strncmp(argv[2],"/usr", 5 ) ||
	    !strncmp(argv[2],"/dev",5 ) || !strncmp(argv[2],"/sbin",6) ){
		(void)printf("ERROR: Bad top directory %s, see nweb -?\n",argv[2]);
		exit(3);
	}
	if(chdir(argv[2]) == -1){
		(void)printf("ERROR: Can't Change to directory %s\n",argv[2]);
		exit(4);
	}
#if 0 
	/*  Become deamon + unstopable and no zombies children (= no wait()) */
	if(fork() != 0)
		return 0; /* parent returns OK to shell */
#endif
	(void)signal(SIGCHLD, SIG_IGN); /* ignore child death */
	(void)signal(SIGHUP, SIG_IGN); /* ignore terminal hangups */
	for(i=3;i<32;i++)
		(void)close(i);		/* close open files */
//	(void)setpgrp();		/* break away from process group */
 
	wlog(LOG,"nweb starting",argv[1],getpid());
 
	/* setup the network socket */
 
	if((listenfd = socket(AF_INET, SOCK_STREAM,0)) <0) {
		wlog(ERROR, "system call","socket",0);
		exit(5);
	}
 
	wlog(LOG, "listenfd", NULL, listenfd);
	int reuse = 1;
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        	perror("setsockopt(SO_REUSEADDR) failed");

#ifdef SO_REUSEPORT
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0) 
        	perror("setsockopt(SO_REUSEPORT) failed");
#endif
 
	port = atoi(argv[1]);
	if(port < 0 || port >60000) {
		wlog(ERROR,"Invalid port number (try 1->60000)",argv[1],0);
		exit(6);
	}
 
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);
  
	if(bind(listenfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) <0) {
		wlog(ERROR,"system call","bind",0);
		exit(7);
	}
 
	if( listen(listenfd,64) <0) {
		wlog(ERROR,"system call","listen",0);
		exit(7);
	}
 
	for(hit=1; ;hit++) {
		length = sizeof(cli_addr);
		if((socketfd = accept(listenfd, (struct sockaddr *)&cli_addr, &length)) < 0) {
			wlog(ERROR,"system call","accept",0);
			exit(8);
		}
 
		wlog(LOG, "socketfd", NULL, socketfd);
#if 0 
		// for now, do not use fork for simplicity..

		if((pid = fork()) < 0) {
			wlog(ERROR,"system call","fork",0);
		}
		else {
			if(pid == 0) { 	/* child */
				(void)close(listenfd);
				fprintf(stderr, "Handling connection\n");
				fflush(stderr);
				web(socketfd,hit); /* never returns */
			} else { 	/* parent */
				(void)close(socketfd);
			}
		}
#else
		web(socketfd, hit);
		close(socketfd);
#endif
	}
}

