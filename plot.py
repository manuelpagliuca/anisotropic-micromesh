import matplotlib.pyplot as plt

# Definisci gli intervalli e le frequenze
intervalli = [
    (-float('inf'), 0.0000000),
    (0.0000000, 0.1385000),
    (0.1385000, 0.2770000),
    (0.2770000, 0.4155000),
    (0.4155000, 0.5540000),
    (0.5540000, 0.6925000),
    (0.6925000, 0.8310000),
    (0.8310000, 0.9695000),
    (0.9695000, 1.1080000),
    (1.1080000, 1.2465000),
    (1.2465000, 1.3850000),
    (1.3850000, 1.5235000),
    (1.5235000, 1.6619999),
    (1.6619999, 1.8005000),
    (1.8005000, 1.9390000),
    (1.9390000, 2.0774999),
    (2.0774999, 2.2160001),
    (2.2160001, 2.3545001),
    (2.3545001, 2.4930000),
    (2.4930000, 2.6315000),
    (2.6315000, 2.7700000),
    (2.7700000, float('inf'))  # Utilizziamo float('inf') per rappresentare l'infinito positivo
]
frequenze = [
    2, 42785, 2759, 665, 225, 101, 86, 51, 43, 29, 31, 13, 16, 18, 8, 12, 10, 14, 4, 2, 3, 0
]

# Crea l'istogramma
plt.bar(range(len(intervalli)), frequenze, align='center')
plt.xlabel('Intervalli')
plt.ylabel('Frequenza')
plt.title('Istogramma dei dati')
plt.xticks(range(len(intervalli)), [f'{x[0]:.2f} - {x[1]:.2f}' if x[0] != -
           float('inf') else f'-inf - {x[1]:.2f}' for x in intervalli], rotation=45)
plt.show()
