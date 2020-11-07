UART1_write("AT+RST"); //Resetear el modulo
UART1_read("ready");   //Esperar a que el modulo este listo
while (!(messageComplete))
    ;
UART1_write("AT"); //Confirmar que haya comunicacion
UART1_read("OK");
while (!(messageComplete))
    ;
UART1_write("AT+CWMODE=1"); //Configurar como cliente
UART1_read("OK");
while (!(messageComplete))
    ;
UART1_write("AT+CWLAP"); //Mostar redes disponibles
UART1_read("OK");
while (!(messageComplete))
    ;
UART1_write("AT+CWJAP=" INFINITUM8234 "," Rki4KckWRi ""); //Conectarse a red
UART1_read("OK");
while (!(messageComplete))
    ;
UART1_write("AT+CIFSR"); //Mostrar direccion IP
UART1_read("OK");
while (!(messageComplete))
    ;
UART1_write("AT+CIPMUX=1"); //Configurar para multiples conexiones
UART1_read("OK");
while (!(messageComplete))
    ;
UART1_write("AT+CIPSERVER=1,80"); //Configurar puerto
UART1_read("OK");
while (!(messageComplete))
    ;

buffer = "" found_char = 0 word = "ESTO" while (!encontrado)
    temp = UART_D
        buffer += temp if (temp == word[found_char]) //Coincidencia
    found_char += 1 if (word[found_char] == '\0')    //Encontrado
    encontrado = 1 else found_char = 0