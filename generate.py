import os
import sys

TEXT = (
    "La inteligencia artificial esta transformando la manera en que vivimos, trabajamos y nos comunicamos. "
    "Desde los asistentes virtuales hasta los algoritmos que detectan enfermedades, la IA ofrece soluciones innovadoras. "
    "Su capacidad para analizar grandes cantidades de datos permite tomar decisiones mas informadas y precisas. "
    "Sin embargo, tambien plantea desafios eticos como la privacidad, el sesgo algoritmico y la automatizacion del empleo. "
    "El desarrollo responsable de estas tecnologias es crucial para maximizar beneficios y minimizar riesgos. "
    "La colaboracion entre cientificos, gobiernos y empresas resulta esencial para garantizar un uso etico. "
    "A medida que la IA evoluciona, se vuelve cada vez mas presente en nuestra vida diaria, desde la educacion hasta la medicina. "
    "Comprender sus implicancias y limitaciones es clave para aprovechar su potencial sin comprometer nuestros valores.\n"
)

def generate_file(filename, target_size_gb):
    target_size_bytes = target_size_gb * 1024**3
    block = (TEXT * 100).encode('utf-8')
    block_size = len(block)

    written = 0
    with open(filename, 'wb') as f:
        while written < target_size_bytes:
            f.write(block)
            written += block_size
            if written % (1024**3) < block_size:
                print(f'Progress: {written / 1024**3:.2f} GB written')

    print(f'✅ Archivo \"{filename}\" generado con tamaño: {written / 1024**3:.2f} GB')

if __name__ == "__main__":
    if len(sys.argv) > 1:
        try:
            gb = int(sys.argv[1])
        except ValueError:
            print("❌ El argumento debe ser un número entero (GB)")
            sys.exit(1)
    else:
        gb = 20
    filename = f"text_{gb}GB.txt"
    print(len(TEXT.split()))
    generate_file(filename, target_size_gb=gb)
