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
