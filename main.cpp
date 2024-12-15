#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>

#include "json.hpp"

namespace fs = std::filesystem;

int main() {
    try {
        const std::string directory = "source_test";

        // Check if the "source_test" directory exists
        if (!fs::exists(directory)) {
            std::cerr << "Errore: La directory " << directory << " non esiste." << std::endl;
            return 1;
        }

        // Count the number of files in the directory
        int fileCount = 0;
        for (const auto& entry : fs::directory_iterator(directory)) {
            if (fs::is_regular_file(entry)) { // Check if it's a regular file
                fileCount++;
            }
        }

        // Loop over the number of files
        int i = 1; // Iterator for your file naming scheme (e.g., file1, file2, ...)
        for (const auto& entry : fs::directory_iterator(directory)) {
            if (fs::is_regular_file(entry)) {
                std::string newFile = entry.path().string(); // Use full file path
                std::cout << "Apro il file " << i << ": " << newFile << std::endl;

                std::ifstream file(newFile);
                if (!file.is_open()) {
                    std::cout << "Errore: impossibile aprire il file " << newFile << std::endl;
                } else {
                    std::cout << "File aperto correttamente: " << newFile << std::endl;
                    std::stringstream ss;
                    ss << file.rdbuf();
                    file.close();

                    if (ss.str().empty()) {
                        std::cout << "Errore: il file è vuoto o non è stato letto correttamente." << std::endl;
                    } else {
                        std::cout << "Contenuto del file letto correttamente." << std::endl;
                        json data;
                        ss >> data;
                        std::cout << data << std::endl;
                        std::cout << "Il parsing del file " << i << " e' andato a buon fine" << std::endl;
                    }
                }
                i++; // Increment iterator
            }
        }
    } catch (json_exception const& e) {
        std::cout << e.msg << std::endl;
        return 1;
    } catch (std::exception const& e) {
        std::cerr << "Errore: " << e.what() << std::endl;
        return 1;
    }
}