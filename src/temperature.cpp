#include <temperature.h>
#include <configuration.h>

void TaskTemperature(void *pvParameters) {
  void **localParameters = (void **)pvParameters; // Array of pointers to parameters

    // Casting of parameters to the correct type
    float &electronicsTemperature = *(float *)localParameters[0];
    float &transformerTemperature = *(float *)localParameters[1];
    DallasTemperature &sensors = *(DallasTemperature *)localParameters[2];

    float electronicsReadings[TEMPERATURE_SENSOR_RESOLUTION];
    float transformerReadings[TEMPERATURE_SENSOR_RESOLUTION];

    while (1) {
        // Take multiple readings
        for (int i = 0; i < TEMPERATURE_SENSOR_RESOLUTION; i++) {
            sensors.requestTemperatures();
            electronicsReadings[i] = sensors.getTempCByIndex(0);
            transformerReadings[i] = sensors.getTempCByIndex(1);
            vTaskDelay(pdMS_TO_TICKS(TEMPERATURE_UPDATE_INTERVAL/TEMPERATURE_SENSOR_RESOLUTION));
        }

        // Calculate averages
        float electronicsSum = 0;
        float transformerSum = 0;
        for (int i = 0; i < TEMPERATURE_SENSOR_RESOLUTION; i++) {
            electronicsSum += electronicsReadings[i];
            transformerSum += transformerReadings[i];
        }
        electronicsTemperature = electronicsSum / TEMPERATURE_SENSOR_RESOLUTION;
        transformerTemperature = transformerSum / TEMPERATURE_SENSOR_RESOLUTION;
    }
}