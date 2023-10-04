# portscan

 - Este código em C é um scanner de portas simples que verifica se as portas estão abertas em um host específico.


# Instalação
- Crie o arquivo scanport.c
- compile usando "gcc -o scanport scanport.c"
- Permitindo execução do código "chmod +x scanport" 


# Como usar?
./scan -a 0.0.0.0    # Verifica todas as portas
./scan -p 22 0.0.0.0  # Verifica a porta 22
