import random
import time

def get_random_color():
    """Gera uma cor aleatória em formato RGB."""
    return random.randint(0, 255), random.randint(0, 255), random.randint(0, 255)

def print_colored_text(text, bg_color, fg_color):
    """Imprime o texto com cores de fundo e primeiro plano."""
    print(f"\033[48;2;{bg_color[0]};{bg_color[1]};{bg_color[2]}m"  # Cor de fundo
          f"\033[38;2;{fg_color[0]};{fg_color[1]};{fg_color[2]}m"  # Cor do texto
          f"{text}\033[0m")  # Reseta as cores

counter = 0

try:
    while True:
        msg = input("Digite uma mensagem (ou 'sair' para encerrar): ")
        if msg.lower() == 'sair':
            break
        
        bg_color = get_random_color()
        fg_color = get_random_color()
        
        counter += 1
        print(f"Contador: {counter}")
        print_colored_text(msg, bg_color, fg_color)
        
        time.sleep(10)

except KeyboardInterrupt:
    print("\nPrograma encerrado.")
