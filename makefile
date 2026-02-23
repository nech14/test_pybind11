PYTHON = python
BUILD_DIR = build
MODULE_NAME = example
SCRIPT = main.py
STUB_NAME = $(MODULE_NAME).pyi
GENERATED_STUB = ./stubs/$(STUB_NAME)

# Автоматически получаем путь к инклудам pybind11 и меняем \ на / для совместимости с Clang
PY_INCLUDES = $(shell $(PYTHON) -c "import pybind11; print(pybind11.get_include().replace('\\', '/'))")

ifeq ($(OS),Windows_NT)
    # Настройки для Windows
    RM = powershell Remove-Item -Recurse -Force
    CP = powershell copy
    MV = powershell move
    TARGET_PATH = $(BUILD_DIR)/Release/$(MODULE_NAME)*.pyd
    # Команда для генерации заглушек с переменными окружения через PowerShell
    STUB_CMD = powershell "$$env:PYTHONUTF8=1; $$env:PYTHONPATH='.'; $(PYTHON) -m pybind11_stubgen $(MODULE_NAME)"
else
    # Настройки для Linux / macOS
    RM = rm -rf
    CP = cp
    MV = mv
    TARGET_PATH = $(BUILD_DIR)/$(MODULE_NAME)*.so
    STUB_CMD = PYTHONPATH=. PYTHONUTF8=1 $(PYTHON) -m pybind11_stubgen $(MODULE_NAME)
endif

.PHONY: all build run clean docs stubs

all: build stubs run

# 1. Генерация docstrings.h
# Мы берем путь в кавычки и используем прямые слеши.
# Ошибка FileNotFoundError лечится проверкой существования файла в Python.
docs:
	@echo "--- Generating docstrings ---"
	$(PYTHON) -m pybind11_mkdoc -o docstrings.h main.cpp -- -std=c++17 "-I$(PY_INCLUDES)"
	@echo "--- Fixing encoding ---"
	$(PYTHON) -c "import os; f='docstrings.h'; c=open(f, encoding='cp1251', errors='ignore').read() if os.path.exists(f) else ''; open(f, 'w', encoding='utf-8').write(c) if c else None"

# 2. Сборка (Создание папки через Python - кроссплатформенно)
build: docs
	$(PYTHON) -c "import os; os.makedirs('$(BUILD_DIR)', exist_ok=True)"
	cmake -S . -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR) --config Release
	$(CP) $(TARGET_PATH) .

# 3. Генерация .pyi и перенос
stubs: build
	@echo "--- Generating stubs ---"
	$(STUB_CMD)
	$(MV) $(GENERATED_STUB) .
	$(RM) stubs

# 4. Запуск
run:
	$(PYTHON) $(SCRIPT)

# 5. Очистка (Через Python, чтобы не зависеть от системных команд удаления)
clean:
	$(RM) $(BUILD_DIR)
	$(PYTHON) -c "import os, glob; [os.remove(f) for f in glob.glob('*.pyd') + glob.glob('*.so') + glob.glob('*.pyi') + glob.glob('docstrings.h')]"