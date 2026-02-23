import sys
import os
import numpy as np
import matplotlib.pyplot as plt

# Добавляем путь к собранному модулю
for folder in ["build", "build/Release", "build/Debug"]:
    sys.path.append(os.path.join(os.getcwd(), folder))

try:
    import example
except ImportError as e:
    print(f"Ошибка: не удалось найти модуль example. Проверьте папку build! {e}")
    sys.exit(1)

gen = example.SignalGenerator(1000)
try:
    # Пытаемся сгенерировать 0 точек
    gen.generate_sine(1.0, 5.0, 0, 0)
except ValueError as e:
    print(f"Поймали ошибку из C++: {e}")

# 1. Создаем экземпляр C++ класса
gen = example.SignalGenerator(sample_rate=1000.0)

# 2. Устанавливаем теги (демонстрация std::vector<string>)
gen.set_tags(["sine", "test", "pybind11"])
print(f"Теги из C++: {gen.get_tags()}")

# 3. Генерируем данные в C++ (получаем numpy array)
amplitude = 1.0
frequency = 5.0  # 5 Гц
phase = np.pi / 4  # Смещение фазы
n_points = 500

# Вызов C++ функции
y = gen.generate_sine(amplitude, frequency, phase, n_points)
x = np.linspace(0, n_points / 1000.0, n_points)

# 4. Строим график
plt.figure(figsize=(10, 4))
plt.plot(x, y, label=f'Sine {frequency}Hz', color='blue')
plt.title("Сгенерировано в C++ (через pybind11 + NumPy)")
plt.xlabel("Время (сек)")
plt.ylabel("Амплитуда")
plt.grid(True)
plt.legend()
plt.show()