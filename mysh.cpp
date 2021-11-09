/*
  Quanminh Nguyen (qu963161)
  Grayson North (gr967419)
  Jacques Parizeau (ja044762)
*/


#include <iostream>
#include <cstring>
#include <sys/stat.h>
#include <fstream>
#include <string>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <dirent.h>
#include <bits/stdc++.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>


using namespace std;

//global variables
string currentDir;
vector <string> tokens;
vector <string> history;
int collectUserInput = 1; //bool to bypass getting user input for replay command
struct stat info;
//History //TODO
//functions
//======================= Quanminh Nguyen ==============================
void Init(){
  char in[50];
    
  currentDir = getcwd(in, 100);
  //================= Grayson's portion just for history ============================
  ofstream historyFile("history.txt");
  if (!historyFile.is_open())
    perror("Error creating history.txt");
  //=====================================================================
}

void movetodir(){
  string directory = tokens[1];
  string temp = currentDir;
  if(directory.compare(0,1,"/") == 0)
  {
    temp = directory;
  }
  else
    temp.append("/" + directory);
  char str[temp.length() + 1];
  strcpy(str, temp.c_str());
  
   
  if( stat( str, &info ) != 0 )
    cout << "Directory does not exist" << endl;
  else if( info.st_mode & S_IFDIR ){
    currentDir = temp;
  } 
  else
     cout << "Directory does not exist" << endl;

  return;
}
void whereami(){
  cout << currentDir << endl;
}
void dalek(){
  pid_t pid = stoi(tokens[1]);
  if(kill(pid, SIGKILL) == 0){
    cout << "Process " << pid <<" has been terminated"<<endl;
  }
  else{
    cout << "Process " << pid <<" has not been terminated"<<endl;
  }
  return;
}
// ========================== Grayson's Section ===================================
void setHistory(string userInput) {
  history.push_back(userInput);
}

void printHistory() {
  int i, n;
  n = 0;
  for (i = history.size(); i > 0; i--) { 
    cout << n << ": " << history[i-1] << endl;
    n++;
  }
}

void clearHistory() {
  history.clear();
}

void saveHistory() {
  int i;
  ofstream historyFile;
  historyFile.open("history.txt", ios::out | ios::in);

  for (i = 0; i < history.size(); i++)
    historyFile << history[i] << endl;
  
  historyFile.close();
}

void readInHistory() {
  string txt;
  ifstream historyFile;
  historyFile.open("history.txt", ios::out | ios::in);

  while (getline(historyFile, txt))
    setHistory(txt);
  historyFile.close();
}
// ================================================================================
//tokenize
void tokenize(string Input){
  stringstream input(Input);

  string buffer;

  while(getline(input, buffer, ' ')){
    tokens.push_back(buffer);
  }
  
}


int main() {

  int number;
  string userInput = "";
  readInHistory();
  Init();
  
  while(0==0)
  {
    tokens.clear();
    if(collectUserInput == 1)
    {
      //get user's input
      cout << "#";
      getline(cin, userInput);
      cout << "\n";
      
    }
    
    collectUserInput = 1;
    
    //process request
    if(userInput.compare("byebye") == 0)
    {
      saveHistory();
      cout << "\nBye bye! See you again soon!\n";
      exit(0);
    }
    else
    {
      //tokenize userInput
      tokenize(userInput);
      setHistory(userInput);
      if(tokens.size() == 2
        && tokens[0].compare("movetodir") == 0) //movetodir
      {
          movetodir();
      }
      else if(tokens.size() == 1
              && tokens[0].compare("whereami") == 0) //whereami
      {
        whereami(); //prints out currentDir
      }
      else if((tokens.size() == 1 || tokens.size() == 2) //Grayson
              && tokens[0].compare("history") == 0) //history [-c]
      {
        if(tokens.size() == 2 && tokens[1].compare("-c") == 0)
          clearHistory();
        else
          printHistory();
      }
      else if(tokens.size() == 2
              && tokens[0].compare("replay") == 0) //replay number
      {
        //check if number is a number -and only execute function if so
        collectUserInput = 0;

        //parse number from tokens[1]
        if (history.back().compare("history") == 0)
          number = (history.size() - stoi(tokens[1])) - 1;
        else
          number = (history.size() - stoi(tokens[1])) - 2;

        //set userInput to the command at history index designated by number
        userInput = history[number];
      }
      //Jacques Parizeau
      else if(tokens.size() >= 2
              && tokens[0].compare("start") == 0) //start program [parameters]
      {
        //if the program dir starts with '/', read as full path, otherwise combine with currentDir
        string programPath = tokens[1];
        if(programPath[0] != '/')
        {
          programPath = currentDir + '/' + programPath;
        }

        //generating parameters from tokens
        vector <char*> params;
        for(int i = 1;i<tokens.size();i++)
        {
          params.emplace_back(const_cast<char*>(tokens.at(i).c_str()));
        }
        params.push_back(nullptr);

        pid_t forkPID = fork();
        if(forkPID == -1)
        {
            cout << "error creating fork";
        } 
        else if(forkPID > 0) //parent
        {
            int status;
            waitpid(forkPID, &status, 0);
            if(WIFEXITED(status))
            {
              cout << "child process finished executing normally\n";
            }
            else
            {
              cout << "Something went wrong exiting/executing the child process\n";
            }
        }
        else //forkPID has the process ID of the child. this being the parent
        {
            int statusCode = execv(programPath.c_str(), params.data());
            if (statusCode == -1)
            {
                cout << "Process did not terminate correctly\n";
            }
            return 0;
        }
        //example: start usr/bin/x-terminal-emulator
        //start usr/bin/ls
        //start usr/bin/ls -l
      }
      //Jacques Parizeau
      else if(tokens.size() >= 2
              && tokens[0].compare("background") == 0) //background program [parameters] //parameters are optional
      {
        string programPath = tokens[1];
        if(programPath[0] != '/')
        {
          programPath = currentDir + '/' + programPath;
        }

        //generating params from tokens
        vector <char*> params;
        for(int i = 1;i<tokens.size();i++)
        {
          params.emplace_back(const_cast<char*>(tokens.at(i).c_str()));
        }
        params.push_back(nullptr);

        pid_t forkPID = fork();
        if(forkPID == -1)
        {
            cout << "error creating fork";
        } 
        else if(forkPID > 0) //parent
        {
            cout << "Child process " << forkPID << endl;
        }
        else //forkPID has the process ID of the child. this being the parent
        {
            int statusCode = execv(programPath.c_str(), params.data());
            if (statusCode == -1)
            {
                cout << "Process did not terminate correctly\n";
            }
            return 0;
        }
      }
      //Quanminh
      else if(tokens.size() == 2
              && tokens[0].compare("dalek") == 0) //dalek PID
      {
        //use kill function if successfully launched program to kill this program
        dalek();
      }
      else
      {
        cout << "invalid command found -check the name and number of parameters." << endl;
      }
    }
    
  }

  cout << "\nshould never get here\n";
  return -1;
}