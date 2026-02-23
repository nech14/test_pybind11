#include <pybind11/pybind11.h>
#include <pybind11/stl.h>   
#include <pybind11/numpy.h> 
#include <vector>
#include <string>
#include <stdexcept> // Для стандартных исключений

// Важно: _USE_MATH_DEFINES должен быть перед cmath
#define _USE_MATH_DEFINES 
#include <cmath>

#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif

// Подключаем сгенерированные докстринги
#include "docstrings.h" 

namespace py = pybind11;

/**
 * @brief Класс для генерации цифровых сигналов.
 * 
 * Позволяет создавать различные типы сигналов и управлять их метаданными через теги.
 */
class SignalGenerator {
public:
    /**
     * @brief Конструктор генератора.
     * @param sample_rate Частота дискретизации в Гц (количество отсчетов в секунду).
     * @throw py::value_error если sample_rate <= 0.
     */
    SignalGenerator(double sample_rate) {
        if (sample_rate <= 0) {
            throw py::value_error("Sample rate must be a positive number (greater than 0).");
        }
        sample_rate_ = sample_rate;
    }

    /**
     * @brief Генерирует массив данных, представляющий синусоидальный сигнал.
     * 
     * Использует формулу: amplitude * sin(2 * PI * frequency * t + phase).
     * 
     * @param amplitude Пиковая амплитуда сигнала.
     * @param frequency Частота синусоиды в Гц.
     * @param phase Начальное смещение фазы в радианах.
     * @param n_points Количество точек (отсчетов), которые нужно сгенерировать.
     * @return py::array_t<double> Массив NumPy (ndarray) с результатами.
     * @throw py::value_error если n_points <= 0 или frequency < 0.
     */
    py::array_t<double> generate_sine(double amplitude, double frequency, double phase, int n_points) {
        // Проверка: количество точек должно быть положительным для выделения памяти
        if (n_points <= 0) {
            throw py::value_error("Number of points (n_points) must be greater than zero.");
        }
        
        // Проверка: частота обычно не должна быть отрицательной в данном контексте
        if (frequency < 0) {
            throw py::value_error("Frequency cannot be negative.");
        }

        auto result = py::array_t<double>(n_points);
        py::buffer_info buf = result.request();
        double* ptr = static_cast<double*>(buf.ptr);

        for (int i = 0; i < n_points; ++i) {
            double t = i / sample_rate_;
            ptr[i] = amplitude * std::sin(2.0 * M_PI * frequency * t + phase);
        }
        return result;
    }

    /**
     * @brief Устанавливает список текстовых тегов для описания сигнала.
     * @param tags Вектор строк (например, ["sine", "test"]).
     * @throw py::value_error если передан пустой список.
     */
    void set_tags(std::vector<std::string> tags) {
        if (tags.empty()) {
            throw py::value_error("Tags list cannot be empty.");
        }
        tags_ = tags;
    }

    /**
     * @brief Возвращает текущие установленные теги.
     * @return std::vector<std::string> Список тегов.
     */
    std::vector<std::string> get_tags() const {
        return tags_;
    }

private:
    double sample_rate_;
    std::vector<std::string> tags_;
};


PYBIND11_MODULE(example, m) {
    m.doc() = "Библиотека для генерации сигналов на C++";

    py::class_<SignalGenerator>(m, "SignalGenerator", DOC(SignalGenerator))
        .def(py::init<double>(), 
             py::arg("sample_rate"), 
             DOC(SignalGenerator, SignalGenerator)) // Конструктор
             
        .def("generate_sine", &SignalGenerator::generate_sine, 
             py::arg("amplitude"), py::arg("frequency"), py::arg("phase"), py::arg("n_points"),
             DOC(SignalGenerator, generate_sine))  // Метод generate_sine
             
        .def("set_tags", &SignalGenerator::set_tags, 
             py::arg("tags"), 
             DOC(SignalGenerator, set_tags))
             
        .def("get_tags", &SignalGenerator::get_tags, 
             DOC(SignalGenerator, get_tags));
}