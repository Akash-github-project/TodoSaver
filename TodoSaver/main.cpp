//
//  main.cpp
//  TodoSaver
//
//  Created by Akash Sinha on 10/11/24.
//

#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <filesystem>

#define FOLDER_NAME "todo_organizer"
#define ADD_TODO_FLAG "--a"
#define ADD_TODO_FLAG_SHORT "-A"
#define VIEW_TODO_FLAG "--v"
#define VIEW_TODO_FLAG_SHORT "-V"
//purspose of the project
//1. it should take todo as input only one line input
//2. after taking input it should save it into a file
//2.1 it should name the file based on current date like (date-todo.todo)
//3. It should list todo in a scrollabe fashion. i think we can levrage 'vi' editor or 'less' command for this or alternatively
//   we can also use 'fzf'.
//4. it should display status of each todo either "TODO", "IN_PROGRESS" or "DONE"
//5. It should give option to change status of a todo from any state
//6. By default it should only show today's todo

namespace fs = std::filesystem;

std::string getHomeDirectory(){
    
    #ifdef _WIN32
        const char* homeDir = std::getenv("USERPROFILE");
    #else
        const char* homeDir = std::getenv("HOME");
    #endif
    
    if(homeDir){
        return std::string(homeDir);
    }else {
        throw std::runtime_error("could not determine home directory");
    }
}

bool createDirectoryIfNotExist(const fs::path& dirPath){
    if(!fs::exists(dirPath)){
        if(!fs::create_directory(dirPath)){
           std::cerr<<"failed to create directory" + dirPath.string() <<std::endl;
            return false;
        }else {
            return true;
        }
    }else {
        return true;
    }
}


std::string getTodaysFileName(){
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm* localTime = std::localtime(&currentTime);
    std::ostringstream dataStream;
    dataStream << std::put_time(localTime,"%Y-%m-%d");
    return dataStream.str().append(".todo");
}

bool saveTodo(std::string stringToSave,fs::path filePath){
    std::ofstream file(filePath,std::ios::app);
    if(!file.is_open()){
        std::cerr<<"failed to open file"<<std::endl;
    }
    file << " * " + stringToSave <<std::endl;
    return true;
}

void askTodaysTodo(fs::path folderName){
    std::string todo;
    std::cout<<"Enter today's todo :-)"<<std::endl;
    std::getline(std::cin,todo);
    if(todo.length() > 0){
        if(saveTodo(todo,folderName / getTodaysFileName())){
            std::cout<<"successfully saved todo"<<std::endl;
        }else {
            std::cout<<"failed to save todo :-("<<std::endl;
        }
    }else {
        std::cout<<"Empty values not allowed!!! :-|"<<std::endl;
    }
}

bool flagChecker(std::string flag){
    if(flag.compare(ADD_TODO_FLAG) || flag.compare(ADD_TODO_FLAG_SHORT)){
        return true;
    }else if(flag.compare(VIEW_TODO_FLAG) || flag.compare(VIEW_TODO_FLAG_SHORT)){
        return true;
    }else {
        return false;
    }
}

fs::path getTodoFolder(){
    std::string homeDir = getHomeDirectory();
    fs::path todoFolder = fs::path(homeDir) / FOLDER_NAME;
    return todoFolder;
}


bool checkForExistingFolder(){
    try {
        fs::path todoFolder = getTodoFolder();
        bool createdOrExisted = createDirectoryIfNotExist(todoFolder);
        if(!createdOrExisted){
            return  false;
        }else {
            return true;
        }
    } catch (const std::exception& exception) {
        std::cerr<<"Error: Reason - " << exception.what() << std::endl;
        return false;
    }
}

void getUserInputForTodo(){
    try {
        bool isTodoFolderPresent = checkForExistingFolder();
        if(!isTodoFolderPresent){
            std::cerr<<"failed to create or access todo folder"<<std::endl;
        }else {
            fs::path todoFolder = getTodoFolder();
            askTodaysTodo(todoFolder);
        }
    } catch (const std::exception& exception) {
        std::cerr<<"Error: Reason - " << exception.what() << std::endl;
    }
}

bool doTodoExistForThisName(std::string fileName){
    try {
        fs::path todoFolder = getTodoFolder();
        std::ifstream file (todoFolder / fileName,std::ios::app);
        if(!file.is_open()){
            std::cerr<<"failed to open file"<<std::endl;
            return false;
        }else {
            file.close();
            return true;
        }
    } catch (const std::exception& error) {
        std::cerr<<error.what() << std::endl;
        return false;
    }
}

void printTodo(std::string fileName){
    try {
        fs::path todoFolder = getTodoFolder();
        std::ifstream file (todoFolder / fileName,std::ios::app);
        if(!file.is_open()){
            std::cerr<<"failed to open file"<<std::endl;
        }else {
            file.seekg(0,std::ios::end);
            std::size_t fileSize = file.tellg();
            file.seekg(0,std::ios::beg);
            
            std::string fileContent(fileSize,'\0');
            file.read(&fileContent[0], fileSize);
            
            if(!file){
                std::cerr<<"Error reading the file" <<std::endl;
                return;
            }
            std::cout<<fileContent<<std::endl;
        }
    } catch (const std::exception& error) {
        std::cerr<<error.what() << std::endl;
    }
}


void listTodo(std::string fileName){
    bool isTodoFolderPresent = checkForExistingFolder();
    bool filePresent = doTodoExistForThisName(fileName);
    if(isTodoFolderPresent && filePresent){
        printTodo(fileName);
    }else {
        std::cerr<<"---- Empty ----" <<std::endl;
    }
}

void decideAction(std::string flag){
    if(flag == ADD_TODO_FLAG || flag == ADD_TODO_FLAG_SHORT){
        getUserInputForTodo();
    }else if(flag == VIEW_TODO_FLAG || flag == VIEW_TODO_FLAG_SHORT){
        listTodo(getTodaysFileName());
    }else {
        std::cerr<<"operation not supported"<<std::endl;
    }
}

int main(int argc, const char * argv[]) {
    // insert code here...
    std::string currentExecuteMode = ADD_TODO_FLAG;
    if(argc == 2){
        std::string flag = argv[1];
        if(flagChecker(flag)){
            currentExecuteMode = argv[1];
           // do something
        }
    }else if(argc > 2){
        std::cerr<<"invalid number of arguments it should be one or none!!" <<std::endl;
        return 1;
    }
    
    decideAction(currentExecuteMode);
    return 0;
}
