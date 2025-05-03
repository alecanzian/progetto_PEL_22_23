@echo off
echo Avvio del container Docker per sviluppo C++...

REM Avvia un container interattivo con la directory corrente montata
docker run --rm -v "%cd%":/app -w /app gcc:latest bash -c "echo 'container avviato...'; make clean; echo 'make clean completato...'; make; echo 'make completato...'; ./main; echo 'Esecuzione del programma completata...'"

echo.
pause
