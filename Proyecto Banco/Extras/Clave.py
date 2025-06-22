import random
import string

def generar_clave(bloques=5, caracteres_por_bloque=5):
    letras = string.ascii_uppercase  # Letras A-Z
    clave = '-'.join(
        ''.join(random.choices(letras, k=caracteres_por_bloque))
        for _ in range(bloques)
    )
    return clave

# Generar 10 claves como ejemplo
for _ in range(10):
    print(generar_clave())
