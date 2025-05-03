# Nome dell'eseguibile finale
TARGET = main

# Compilatore
CXX = g++

# Opzioni di compilazione
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# File sorgenti e oggetti
SRCS = main.cpp json.cpp
OBJS = $(SRCS:.cpp=.o)

# Regola principale
all: $(TARGET)

# Link finale
$(TARGET): $(OBJS)
	@$(CXX) $(CXXFLAGS) -o $@ $^

# Compilazione dei .cpp in .o
%.o: %.cpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@

# Pulizia dei file temporanei
clean:
	@rm -f $(OBJS) $(TARGET)

# Regole ausiliarie
.PHONY: all clean
