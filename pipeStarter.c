#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>


typedef struct Program{
  int numArguments;
  char* arguments[10];
} Program;

void createNewPipe(int previousPipe[], int nextPipe[]){
  previousPipe[0] = nextPipe[0];
  previousPipe[1] = nextPipe[1];
  pipe(nextPipe);
}


void makeChildren(int argc, char* argv[], int numChildren){

  // Make Program Table
  Program* programTable[numChildren];
  int currentIndex;
  int argumentPos = 2;

  for(currentIndex = 0; currentIndex<numChildren; ++currentIndex){

    // Initialize program object
    Program* currentChild = malloc(sizeof(Program));

    int n = 0;
    int start;
    for(start = argumentPos; argumentPos<argc-1; ++argumentPos){

      if( !(strcasecmp(argv[argumentPos],"x")) ){
        break;
      }

      currentChild->arguments[n] = argv[argumentPos];
      ++n;
    }
    ++argumentPos;

    currentChild->numArguments = n;
    //fprintf(stderr, "%d\n", n);
    programTable[currentIndex] = currentChild;
  }

  // Create n children
  pid_t pid = -1;
  int childNumber = -1;
  int previousPipe[2] = {-1,-1};
  int nextPipe[2] = {-1,-1};

  for(currentIndex = 0; currentIndex<numChildren; ++currentIndex){
    ++childNumber;

    // Create pipe if not last child
    if(childNumber < numChildren-1){
      createNewPipe(previousPipe, nextPipe);
    }

    // If last pipe, just make the former next pipe the previous pipe
    else{
      previousPipe[0] = nextPipe[0];
      previousPipe[1] = nextPipe[1];
      nextPipe[0] = -1;
      nextPipe[1] = -1;
    }

    pid = fork();

    // If child, do not make more children
    if(pid == 0){
      break;
    }

    // Close obsolete parent-child pipage
    else if(currentIndex > 0){
      close(previousPipe[0]);
      close(previousPipe[1]);
    }

  }

  // If there is an error forking child
  if(pid < 0){
    fprintf(stderr, "Error forking child\n");
    exit(-4);
  }

  // If parent, delete program table and wait for all children
  else if(pid > 0){
    close(nextPipe[0]);
    close(nextPipe[1]);
    pid_t wpid;
    while( (wpid=wait(NULL)) >= 0);
    int i;
    for(i=0; i<numChildren; ++i){
      free(programTable[i]);
    }
  }
  
  // If child, close appropriate pipes, dupe and execute
  else{

    // If first child
    if(childNumber == 0){
      dup2(nextPipe[1], 1); // Stdout writes to next pipe
      close(nextPipe[0]);
      execvp(programTable[0]->arguments[0], programTable[0]->arguments);
    }

    // If last child
    else if(childNumber == numChildren-1){
      dup2(previousPipe[0],0); //Stdin reads from previous pipe
      close(previousPipe[1]);
      execvp(programTable[numChildren-1]->arguments[0], programTable[numChildren-1]->arguments);
    }

    // If middle child
    else{
      dup2(previousPipe[0], 0); // Stdin reads from previous pipe
      dup2(nextPipe[1], 1); // Stdout writes to next pipe
      close(previousPipe[1]);
      close(nextPipe[0]);
      execvp(programTable[childNumber]->arguments[0], programTable[childNumber]->arguments);
    }

  }

}


int determineNumChildren(int argc, char* argv[]){
  int i;
  int count = 0;
  for(i=3; i<argc; ++i){
    if( !(strcasecmp(argv[i],"x")) || !(strcasecmp(argv[i],"e")) ){
      ++count;
    }
  }
  return count;
}

int main(int argc, char* argv[]){
  if( argc < 3 || (strcasecmp(argv[1], "s")) || (strcasecmp(argv[argc-1], "e")) ){
    fprintf(stderr, "Error: Invalid myStarter syntax\n");
    exit(-1);
  }

  int numChildren = determineNumChildren(argc, argv);
  makeChildren(argc, argv, numChildren);
  return 0;
}
