# Alarm ESP32

College project that implements a proximity-based alarm system using an ESP32 Wroom microcontroller and Firebase for data storage.

## Features

- **Proximity Activation:** The alarm is triggered based on proximity readings.
- **Firebase Integration:** Utilizes Firebase for real-time data storage and retrieval.
- **LED Indicators:** Visual feedback through LED indicators for alarm state.

## Components

- **ESP32 Wroom:** Microcontroller used for sensor readings and control.
- **Firebase:** Cloud-based database for storing alarm state and activation time.
- **Proximity Sensor:** Measures distance for proximity-based activation.

## Setup

1. Clone the repository.
2. Set up your Arduino IDE with the necessary libraries (FirebaseESP32, etc.).
3. Configure Wi-Fi and Firebase credentials in the code.
4. Upload the code to your ESP32 Wroom.
5. Monitor the serial output for debugging and status information.

## Configuration

- Adjust Wi-Fi credentials, Firebase URL, and database secret in the code.
- Customize LED pin assignments, sensor pins, and other parameters as needed.

## Usage

1. Power up the ESP32 Wroom.
2. Monitor the serial output for Wi-Fi connection and status updates.
3. The alarm will activate when an object is in proximity, triggering LED indicators.
4. Firebase stores alarm state and activation time for monitoring.

## Contributors

- Caio Godoy

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details.
