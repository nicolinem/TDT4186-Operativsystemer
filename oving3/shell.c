#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#ifndef MAX_BUF
#define MAX_BUF 500
#endif

int batchMode=0;
int breakString(char *str);
char path[MAX_BUF];


int showPrompt(void) 
{
      char *cmds[100];
      char buffer[100];
      char * tmp;
      int num_cmds, i, flag, rc=0;

      
      while (!rc) 
      {
          num_cmds = 0;
         printf(getcwd(path, MAX_BUF)); fgets(buffer, 512, stdin);

	  for(i=0; buffer[i]!='\0'; i++) {
	    if(buffer[i]=='+')
	      flag++;
	  }

	  int j = i-2;
	  int endingWithPlus = 0;
	  for(; j >= 0; j--)
	  {
	    if(buffer[j] == ' ')
	      continue;
	    else if(buffer[j] == '+')
	      {
		endingWithPlus = 1;
		break;
	      }
	    else
	      break;
	  }
          tmp = strtok(buffer,"+");
           
          while (tmp) 
          {
              cmds[num_cmds] = tmp;
              num_cmds++;
              tmp = strtok(NULL,"+");
          } 

	  int numCmndsToFork = num_cmds;
	  
	  if(flag==0)
	  {
	    if((rc=breakString(buffer))==101) {
	      break; 
	      exit(1);
	    }
	  }
	  else
	    {
	      
	      if(endingWithPlus == 0)
	      {
		  numCmndsToFork = num_cmds - 1;
		   if((rc=breakString(cmds[num_cmds-1]))==101)
		   {
		     break; exit(1);
		   }
	      }

	      int i;
	      for (i = 0; i < numCmndsToFork; i++) 
		{
            
		  int ret;
		  if( (ret=fork()) > 0 )
		    {
		    }
		  else if ( ret == 0 )
		    {
		      if(breakString(cmds[i])==101) 
			{ 
			  exit(0); 
			}
		      break;
		    }
		  else 
		    {
		      char error_message[30] = "An error has occurred\n";
		      write(STDERR_FILENO, error_message, strlen(error_message));
		    }
		}
	    }
      }
      return 0;
}

int breakCommand(char *str)
{
       char *tmp;
       char *subcmds[1000];
       char buffer[1000];
       strcpy(buffer, str);
       tmp = strtok(buffer," \n\t");
       int num_subcmds = 0;
       int out, flag1=0;
       char *subnew[1000];
	   
        
       while (tmp) 
       {
          subcmds[num_subcmds] = tmp;
          num_subcmds++;
          tmp = strtok(NULL," \n\t");
       } 
            
       int j, loc=0;


       for (j = 0; j < num_subcmds; j++) 
	 {
	 }

       subcmds[j] = NULL;

       if(num_subcmds > 1)
	 {
	   for (j = 0; j < num_subcmds; j++) 
	     {
	       if(strcmp(subcmds[j], ">") == 0)
		 {
		   loc=j;
		   flag1=1;
		   break;
		 }
	     }

	   if(flag1==1)
	     {
	       for (j = 0; j < loc; j++) 
		 {
		   subnew[j]=subcmds[j];
		 }
	     }
       
	   subnew[loc]=NULL;
	 }
       
       int i, savedJ, flag2 = 0;

       if(flag1!=1)
	   for (j = 0; j < num_subcmds; j++) 
	     {
	       i = strlen(subcmds[j]) - 1;
	       if(subcmds[j][i]=='>')
		 {
		   subcmds[j][i]='\0';
		   flag2 = 1;
		   savedJ=j;
		 }
	     }

       if(flag2==1)
	 {
	   for (j = 0; j <= savedJ; j++) 
	     {
	       subnew[j]=subcmds[j];
	     }
	   subnew[savedJ+1]=NULL;
	 }
       
       if(flag1==1)
	 {
	   close(STDOUT_FILENO);
	   out = open(subcmds[loc+1], O_RDWR | O_CREAT , 0666 ); 
	   if(out < 0)
	     {
	       char error_message[30] = "An error has occurred\n";
	       write(STDERR_FILENO, error_message, strlen(error_message));
	       exit(0);
	     }
	   dup2(out, STDOUT_FILENO);
	   if(execvp(subnew[0], subnew) < 0)
	     {
	       char error_message[30] = "An error has occurred\n";
	       write(STDERR_FILENO, error_message, strlen(error_message));
	       exit(101);
	     }
	   close(out);
	   return 101;
	 }
       else if(flag2==1)
	 {
	   close(STDOUT_FILENO);
	   out = open(subcmds[savedJ+1], O_RDWR | O_CREAT , 0666 ); 
	   if(out < 0)
	     {
	       char error_message[30] = "An error has occurred\n";
	       write(STDERR_FILENO, error_message, strlen(error_message));
	       exit(0);
	     }
	   dup2(out, STDOUT_FILENO);
	   if(execvp(subnew[0], subnew) < 0)
	     {
	       char error_message[30] = "An error has occurred\n";
	       write(STDERR_FILENO, error_message, strlen(error_message));
	       exit(101);
	     }
	   close(out);
	   return 101;
	 }
	 else if(strcmp(subcmds[0], "cd") == 0) 
	 {
	   int res;
	   if(subcmds[1]!=NULL)
	     {
	       res = chdir(subcmds[1]);
		   wait(&res);
		   if(WIFEXITED(res)) {
			   int statusCode = WEXITSTATUS(res);
			   printf("Exit status %s = %d\n", subcmds[0],statusCode);
		   }
		   
	     }
	   else
	     {
	       res = chdir(getenv("HOME"));
	     }

	   if(res == -1)
	     {
	       char error_message[30] = "An error has occurred\n";
	       write(STDERR_FILENO, error_message, strlen(error_message));
	       exit(101);
	     }
	 }
       else if(strcmp(subcmds[0], "exit") == 0)
	 {
	   int wstatus;
	   wait(&wstatus);
	   if (WIFEXITED(wstatus)) {
		   int statusCode = WEXITSTATUS(wstatus);
		   if (statusCode == 0) {
			   printf("Exit status %s = %d\n", subcmds[0],statusCode); 
			   exit(0);
		   }
	   }
	  // printf("Exit status: 0");
	   //exit(0);
	 }
       else if(strcmp(subcmds[0], "pwd") == 0)
	 {
	   if(subcmds[1]!=NULL)
	     {
	       char error_message[30] = "An error has occurred\n";
	       write(STDERR_FILENO, error_message, strlen(error_message));
	       exit(0);
	     }
	   else if (execvp(subcmds[0], subcmds) < 0)
	     {
	       char error_message[30] = "An error has occurred\n";
	       write(STDERR_FILENO, error_message, strlen(error_message));
	       exit(101); 
	     }
	 }
       else if (execvp(subcmds[0], subcmds) < 0)
	 {
	   char error_message[30] = "An error has occurred\n";
	   write(STDERR_FILENO, error_message, strlen(error_message));
	   return 1;
	 }
       return 1;
}

int breakString(char *str)
{
       char *tmp;
       char *subcmds[1000];
       char buffer[1000];
       strcpy(buffer, str);
       tmp = strtok(buffer,";");
       int num_subcmds = 0;
        
       while (tmp) 
       {
          subcmds[num_subcmds] = tmp;
          num_subcmds++;
          tmp = strtok(NULL,";");
       } 

       int j, status;
           
       for (j = 0; j < num_subcmds; j++) 
       {
	 int ret;
   if((subcmds[j][0]=='c' && subcmds[j][1]=='d') == 1)
	   {
	     breakCommand(subcmds[j]);
	   }
	 else if(strcmp(subcmds[j],"exit") == 0)
	   {
	     breakCommand(subcmds[j]);
	   }
	 else
	   {
	     if( (ret=fork()) > 0 )
	       {
		 waitpid(ret,&status,WUNTRACED);
		 int x = WEXITSTATUS(status);
		 if(x==101)
		   return 101;
	       }
	     else if ( ret == 0 )
	       {
		 if(breakCommand(subcmds[j])==1)  
		   { 
		     exit(1);
		     return 101; 
		   }
		 else return 0;
		 break;
	       }
	     else 
	       {
		 char error_message[30] = "An error has occurred\n";
		 write(STDERR_FILENO, error_message, strlen(error_message));
		 exit(101);
	       }
	   }
       }
}

int main(int argc, char *argv[]) 
{
      char *cmds[1000];
      char buffer[1000]="test";
      char * tmp;
      int num_cmds, i, flag, rc=0;
      char *fileToRead = "/no/such/file";
	  int status;

      if(argc>2) 
	{
	  char error_message[30] = "An error has occurred\n";
	  write(STDERR_FILENO, error_message, strlen(error_message));
	  exit(1);
	}
      else if(argc==2)
	{
	  batchMode = 1;	        
	  fileToRead = argv[1];
	}

      FILE *new;
      FILE *fp;

      if(batchMode==1)
	{
	  fp = fopen(fileToRead,"r");
	  if (fp==NULL) 
	    {
	      char error_message[30] = "An error has occurred\n";
	      write(STDERR_FILENO, error_message, strlen(error_message));
	      exit(1);
	    }
	  new=fp;
	}
      else
	new=stdin;
	
      while (!feof(new)) 
      {
          num_cmds = 0;
        if(batchMode==0) write(STDOUT_FILENO, getcwd(path, MAX_BUF), strlen(getcwd(path,MAX_BUF)));  
        printf(": ");
	  if(batchMode==1)
	      fgets(buffer, 1000, fp);
	  else
	      fgets(buffer, 1000, stdin);

	  int j;

	  strtok(buffer, "\n\r");

	  if(batchMode == 1 && strcmp(buffer,"xyz")!=0)
	    {
		  write(STDOUT_FILENO, buffer, strlen(buffer));
		  write(STDOUT_FILENO, "\n", strlen("\n"));
		  if(strcmp(buffer,"+")==0)
		    {
		      exit(0);
		    }
	    }

	  if(strcmp(buffer,"xyz")==0) exit(0);
	  

	  for(i=0; buffer[i]!='\0'; i++) {
	    if(buffer[i]=='+')
	      flag++;
	  }
	  
	  if(strlen(buffer)==0)
	    {
	      char error_message[30] = "An error has occurred\n";
	      write(STDERR_FILENO, error_message, strlen(error_message));
	    }

	  j = i-2;
	  int endingWithPlus = 0;
	  for(; j >= 0; j--)
	  {
	    if(buffer[j] == ' ')
	      continue;
	    else if(buffer[j] == '+')
	      {
		endingWithPlus = 1;
		break;
	      }
	    else
	      break;
	  }
          tmp = strtok(buffer,"+");
           
          while (tmp) 
          {
              cmds[num_cmds] = tmp;
              num_cmds++;
              tmp = strtok(NULL,"+");
          } 

	  int numCmndsToFork = num_cmds;
	  
	  if(flag==0)
	  {
	    if((rc=breakString(buffer))==101) {
	      break; exit(0);
	    }
	  }
	  else
	    {
	      
	      if(endingWithPlus == 0)
	      {
		  numCmndsToFork = num_cmds - 1;
		   if((rc=breakString(cmds[num_cmds-1]))==101)
		   {
		     break;
		     exit(0);
		   }
	      }

	      int i, status;
	      for (i = numCmndsToFork-1; i >= 0; i--) 
		{
            
		  int ret;
		  if( (ret=fork()) > 0 )
		    {
		      while (1) {
			int status;
			pid_t done = waitpid(ret,&status,WUNTRACED);
			if (done == -1) {
			  if (errno == ECHILD) break; 
			} else {
			  int x = WEXITSTATUS(status);

			  if (!WIFEXITED(x) || WEXITSTATUS(x) != 101) {
			    exit(0);
			
			  }
			}
		      }
		    }
		  else if ( ret == 0 )
		    {
		      if(breakString(cmds[i])==101) 
			{ 
			  exit(0); 
			}
		    }
		  else 
		    {
		      char error_message[30] = "An error has occurred\n";
		      write(STDERR_FILENO, error_message, strlen(error_message));
		      exit(0);
		    }
		}
	    }
	  strcpy(buffer,"xyz");
      }
      return 0;
}