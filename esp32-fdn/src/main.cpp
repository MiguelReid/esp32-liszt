#include <Arduino.h>
#include "driver/i2s.h"
#include <math.h>

#define I2S_PORT I2S_NUM_0

// ESP32 → MAX98357A pin mapping
#define PIN_BCLK 26
#define PIN_LRC  25
#define PIN_DOUT 22

#define SAMPLE_RATE 44100
#define BUFFER_SAMPLES 256

float phase = 0.0f;
float frequency = 440.0f; // A4 note
float amplitude = 0.1f; // Volume

void setupI2S() {

    i2s_config_t config = {};
    config.mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX);
    config.sample_rate = SAMPLE_RATE;
    config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
    config.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT;
    config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
    config.dma_buf_count = 8;
    config.dma_buf_len = 256;
    config.use_apll = false;
    config.tx_desc_auto_clear = true;

    i2s_pin_config_t pins = {};
    pins.bck_io_num = PIN_BCLK;
    pins.ws_io_num = PIN_LRC;
    pins.data_out_num = PIN_DOUT;
    pins.data_in_num = I2S_PIN_NO_CHANGE;

    i2s_driver_install(I2S_PORT, &config, 0, NULL);
    i2s_set_pin(I2S_PORT, &pins);
    i2s_zero_dma_buffer(I2S_PORT);
}

void setup() {

    Serial.begin(115200);
    Serial.println("Starting sine wave");

    setupI2S();
}

void loop() {

    static int16_t buffer[BUFFER_SAMPLES];

    float phaseIncrement = 2.0f * PI * frequency / SAMPLE_RATE;

    for (int i = 0; i < BUFFER_SAMPLES; i++) {

        float sample = sinf(phase);
        phase += phaseIncrement;

        if (phase > 2 * PI)
            phase -= 2 * PI;

        int16_t value = sample * amplitude * 32767;

        // Mono (only one speaker because of MAX)
        buffer[i] = value;
    }

    size_t bytesWritten;

    i2s_write(
        I2S_PORT,
        buffer,
        sizeof(buffer),
        &bytesWritten,
        portMAX_DELAY
    );
}