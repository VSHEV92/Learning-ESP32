#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

static const char* TAG = "freertos_example";

static SemaphoreHandle_t mutex;
static SemaphoreHandle_t recursive_mutex;

void app_main(void) {
    BaseType_t status;

    // create semaphore
    ESP_LOGI(TAG, "Create mutexes");
    mutex = xSemaphoreCreateMutex();
    recursive_mutex = xSemaphoreCreateRecursiveMutex();

    // mutex actions
    ESP_LOGI(TAG, "Try to take mutex to times!");

    for (int i = 0; i < 2; i++) {
        status = xSemaphoreTake(mutex, 0);
        if (status == pdPASS) {
            ESP_LOGI(TAG, "Mutex is taken!");
        } 
        else {
            ESP_LOGW(TAG, "Mutex is no taken!");
        }
    }
    xSemaphoreGive(mutex);


    // recursive mutex actions
    ESP_LOGI(TAG, "Try to take recursive mutex to times!");

    for (int i = 0; i < 2; i++) {
        status = xSemaphoreTakeRecursive(recursive_mutex, 0);
        if (status == pdPASS) {
            ESP_LOGI(TAG, "Recursive mutex is taken!");
        } 
        else {
            ESP_LOGW(TAG, "Recursive mutex is no taken!");
        }
    }
    xSemaphoreGiveRecursive(recursive_mutex);
    xSemaphoreGiveRecursive(recursive_mutex);

    // delete mutexes
    ESP_LOGI(TAG, "Delete mutexes");
    vSemaphoreDelete(mutex);
    vSemaphoreDelete(recursive_mutex);

}
