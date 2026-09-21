# Real-Time Temperature Monitoring System Using STM32 Nucleo and FreeRTOS

## Project Overview

This project implements a **Real-Time Temperature Monitoring System** using the **STM32 Nucleo-F446RE** development board and **FreeRTOS**. The system continuously acquires temperature data from an **LM35 Temperature Sensor** through the ADC peripheral, processes the sensor readings, and distributes the data to multiple FreeRTOS tasks using queues.

The measured temperature is displayed on an **I2C 16x2 LCD**, transmitted through **UART** for serial monitoring, and used to control an **LED** and **Buzzer** for over-temperature alerts.

This project demonstrates the practical implementation of:

- FreeRTOS Task Management
- Inter-Task Communication using Queues
- ADC Sensor Interfacing
- UART Communication
- I2C LCD Interfacing
- Real-Time Embedded System Design

---

## Features

✅ LM35 Temperature Sensor Interfacing

✅ ADC-Based Temperature Acquisition

✅ I2C LCD Temperature Display

✅ UART Transmission of:
- ADC Value
- Voltage
- Temperature

✅ LED Status Indication

✅ Buzzer Alarm System

✅ FreeRTOS Multitasking

✅ Queue-Based Inter-Task Communication

✅ Periodic Task Scheduling

✅ Modular Firmware Architecture

---

## Hardware Components

| Component | Description |
|------------|------------|
| STM32 Nucleo-F446RE | Main Controller |
| LM35 | Temperature Sensor |
| I2C LCD 16x2 | Temperature Display |
| LED | Status Indicator |
| Buzzer | Temperature Alarm |
| USB Cable | Programming & UART |
| Jumper Wires | Connections |

---

## Software Tools

- STM32CubeIDE
- STM32 HAL Drivers
- FreeRTOS
- Embedded C

---

## System Architecture

```text
                     +----------------+
                     |  LM35 Sensor   |
                     +--------+-------+
                              |
                              v
                     +----------------+
                     |  Sensor Task   |
                     +--------+-------+
                              |
                              |
                          Queue
                              |
       ------------------------------------------------
       |                      |                       |
       v                      v                       v

+--------------+    +----------------+    +----------------+
|   LCD Task   |    |   UART Task    |    |   Alarm Task   |
+--------------+    +----------------+    +----------------+
                                                |
                                      ---------------------
                                      |                   |
                                      v                   v
                                   LED ON             Buzzer ON
```

---

## FreeRTOS Task Structure

### 1. Sensor Task

#### Responsibilities

- Read ADC value from LM35
- Convert ADC to Voltage
- Convert Voltage to Temperature
- Send data to Queue

#### Priority

```c
Priority = 3
```

#### Execution Period

```c
1000 ms
```

---

### 2. LCD Task

#### Responsibilities

- Receive temperature data from Queue
- Display temperature on I2C LCD

#### Priority

```c
Priority = 2
```

---

### 3. UART Task

#### Responsibilities

Transmit:

```text
ADC Value
Voltage
Temperature
```

through USART2.

Example:

```text
ADC=323
Voltage=0.260 V
Temperature=26.0 C
```

#### Priority

```c
Priority = 2
```

---

### 4. Alarm Task

#### Responsibilities

Monitor temperature level.

If temperature exceeds threshold:

```text
LED ON
Buzzer ON
```

Otherwise:

```text
LED OFF
Buzzer OFF
```

#### Priority

```c
Priority = 1
```

---

## Data Structure

```c
typedef struct
{
    uint16_t adc_value;
    float voltage;
    float temperature;
} TempData_t;
```

---

## Queue Implementation

### Queue Creation

```c
QueueHandle_t TempQueue;

TempQueue = xQueueCreate(
                        5,
                        sizeof(TempData_t)
                       );
```

### Queue Send

```c
xQueueSend(
           TempQueue,
           &tempData,
           portMAX_DELAY
          );
```

### Queue Receive

```c
xQueueReceive(
              TempQueue,
              &tempData,
              portMAX_DELAY
             );
```

---

## Temperature Calculation

### ADC Conversion

STM32 ADC Resolution:

```text
12-bit ADC
Range = 0 – 4095
```

Voltage Calculation:

```c
Voltage = (ADC_Value * 3.3) / 4095
```

---

### LM35 Temperature Calculation

LM35 Characteristics:

```text
10mV = 1°C
```

Temperature:

```c
Temperature = Voltage * 100
```

Example:

```text
Voltage = 0.300V

Temperature = 30°C
```

---

## LCD Output

```text
Temp:
26.5 C
```

---

## UART Output

```text
ADC=328
Voltage=0.264 V
Temperature=26.40 C
```

---

## Alarm Logic

### Temperature < 35°C

```text
LED OFF
Buzzer OFF
```

### Temperature ≥ 35°C

```text
LED ON
Buzzer ON
```

---

## Peripheral Configuration

### ADC

| Parameter | Value |
|------------|---------|
| Resolution | 12-bit |
| Channel | ADC Channel 0 |
| Trigger | Software |
| Sampling | Single Conversion |

---

### UART

| Parameter | Value |
|------------|---------|
| Instance | USART2 |
| Baud Rate | 115200 |
| Data Bits | 8 |
| Stop Bits | 1 |
| Parity | None |

---

### I2C

| Parameter | Value |
|------------|---------|
| Instance | I2C1 |
| Clock Speed | 100 kHz |
| Addressing Mode | 7-bit |

---

## FreeRTOS APIs Used

### Task Management

```c
xTaskCreate()

vTaskStartScheduler()

vTaskDelay()

vTaskDelayUntil()
```

---

### Queue Management

```c
xQueueCreate()

xQueueSend()

xQueueReceive()
```

---

## Folder Structure

```text
RTOS_TEMP_MONITER
│
├── Core
│   ├── Inc
│   │   ├── main.h
│   │   ├── FreeRTOSConfig.h
│   │   └── lcd_i2c.h
│   │
│   └── Src
│       ├── main.c
│       ├── lcd_i2c.c
│       ├── stm32f4xx_it.c
│       └── stm32f4xx_hal_msp.c
│
├── Drivers
│
├── Middlewares
│   └── FreeRTOS
│
└── README.md
```

---

## Learning Outcomes

Through this project, the following Embedded Systems concepts were implemented and practiced:

- FreeRTOS Task Scheduling
- Real-Time System Design
- Queue-Based Communication
- STM32 ADC Programming
- UART Communication
- I2C Communication
- Sensor Interfacing
- Embedded Firmware Development
- Peripheral Driver Usage
- Multitasking Application Design

---

## Future Enhancements

- Data Logging to SD Card
- Wi-Fi Monitoring using ESP32
- MQTT Cloud Integration
- Mobile Application Monitoring
- FreeRTOS Software Timers
- Low Power Mode Support
- Temperature History Graph
- Remote Alarm Notifications

---

## Author

### Guni Reddy Charan Kumar Reddy

**B.Tech – Electronics and Communication Engineering**

Skills:
- Embedded C
- STM32
- FreeRTOS
- UART
- I2C
- ADC
- Embedded Systems
- IoT

---

## License

This project is developed for educational, learning, and research purposes.
