# Usa l'immagine ufficiale con GCC
FROM gcc:latest

# Crea una directory nel container
WORKDIR /app

# Copia i file sorgente e il Makefile nella directory del container
COPY . .

# Pulisce eventuali build precedenti (importante se usi cache o ricostruisci spesso)
RUN make clean && make

# Comando di default: esegui il programma
CMD ["./main"]
