#include "esp_log.h"
#include "bmp180.h"
#include "string.h"

static const char *TAG = "bmp180_probe";

#define BMP180_SCL       7
#define BMP180_SDA       6 
#define BMP180_ADDR      0x77 
#define BMP180_FREQ_HZ   10000


void app_main(void) {
    bmp180_dev_t dev;
    memset(&dev, 0, sizeof(bmp180_dev_t)); // Zero descriptor

    ESP_ERROR_CHECK(bmp180_init_desc(&dev, 0, BMP180_SDA, BMP180_SCL));
  //  ESP_ERROR_CHECK(bmp180_init(&dev));

  //  while (1)
  //  {
  //      float temp;
  //      uint32_t pressure;

  //      esp_err_t res = bmp180_measure(&dev, &temp, &pressure, BMP180_MODE_STANDARD);
  //      if (res != ESP_OK)
  //          printf("Could not measure: %d\n", res);
  //      else
  //          /* float is used in printf(). you need non-default configuration in
  //           * sdkconfig for ESP8266, which is enabled by default for this
  //           * example. see sdkconfig.defaults.esp8266
  //           */
  //          printf("Temperature: %.2f degrees Celsius; Pressure: %" PRIu32 " Pa\n", temp, pressure);

  //      vTaskDelay(pdMS_TO_TICKS(500));
  //  }
}
