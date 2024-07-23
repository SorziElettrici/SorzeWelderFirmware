#include <heartbeat.h>
#include <configuration.h>

void TaskHeartbeat(void *pvParameters) {
  while (1) {
    digitalWrite(HEARTHBEAT_LED_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(100));
    digitalWrite(HEARTHBEAT_LED_PIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(900));
  }
}