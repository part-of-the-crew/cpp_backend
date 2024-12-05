#include <iostream>
#include <string>
#include <curl/curl.h>

// Callback-функция для обработки данных от cURL
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t total_size = size * nmemb;
    output->append(static_cast<char*>(contents), total_size);
    return total_size;
}

int main() {
    // Инициализация cURL
    CURL* curl;
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        // URL для прогноза погоды в Суздале
        const char* url = "wttr.in/Suzdal?format=%t+%C+%w";

        // Устанавливаем URL
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Устанавливаем функцию обратного вызова для записи данных
        std::string response_data;
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);

        // Выполняем запрос
        CURLcode res = curl_easy_perform(curl);

        // Проверяем наличие ошибок
        if (res != CURLE_OK) {
            fprintf(stderr, "cURL failed: %s\n", curl_easy_strerror(res));
        } else {
            // Выводим полученные данные (прогноз погоды)
            std::cout << "Weather Forecast for Suzdal:\n" << response_data << std::endl;
        }

        // Освобождаем ресурсы cURL
        curl_easy_cleanup(curl);
    }

    // Глобальная деинициализация cURL
    curl_global_cleanup();

    return 0;
}

* Engineered and Deployed over 10 embedded systems solutions for diesel engines,
    generators, and reduction drives, supporting both local and international clients.
    Utilized RTOS QNX (POSIX-compliant) and bare-metal architectures.

* Architected and Co-Designed    software and hardware systems, including comprehensive
    networking, functional distribution, data storage, and software standardization.

* Developed Safety-Critical Software for bare-metal modules (STM32M4F) and
    OS-based platforms (Intel x86, MIPS, ARM32v7), adhering to industry standards.

* Implemented Functional Components, Networking Solutions, and Control Systems, 
    including measurement algorithms, PID controllers, and digital filters, conducted
    research how its adhering to safety and performance standards.

* Designed APIs for Integration, connecting application layers with network drivers
    and hardware to cross-functional collaboration.

* Conducted Testing    through unit testing, virtual environments, and physical test
    stands, ensuring the robustness and reliability of developed systems.
    
* Owned Full System Lifecycle for recent projects, from initial design through deployment,
    and mentored team members in software best practices and project
    ownership.
