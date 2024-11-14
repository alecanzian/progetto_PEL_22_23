#include <iostream>
#include <fstream>
#include <vector>
#include <istream>
#include <limits>
#include <cassert>
#include <sstream>

#include "json.cpp"


int main(){
    try{
        for (int i=1; i<=5;i++){
            std::string newFile = "test/file"+std::to_string(i) +".txt";
            
            
            std::cout<<"Apro il file "<<i<<std::endl;
            std::ifstream file(newFile);
            if (!file.is_open()) {
                std::cout << "Failed to open the file." << std::endl;
                return 1;
            }
            std::stringstream ss;
            ss << file.rdbuf();
            file.close();

            json data;

            ss >> data;
            //std::cout << data << std::endl;
            std::cout<<"Il parsing del file "<< i << " e' andato a buon fine"<<std::endl;
        }
    }
    catch(json_exception const & e){
        std::cout <<e.msg<<std::endl;
        return 1;
    } 
    
}

//VALGRIND: 
// $ g++ -g main.cpp -o main
// $ valgrind --leak-check=full --track-origins=yes ./main

//SANITIZERS: 
// $ g++ -fsanitize=address -fno-omit-frame-pointer main.cpp -o main  ./main

