# Robotic Nurse: Autonomous Guidance Vehicle (AGV) for Hospital Assistance

This repository houses the innovative project I had the privilege to contribute to at Fluxing Engineering. We designed and built a driverless robotic nurse for COVID-19 patients. The principal objective was to minimize human interaction in healthcare facilities while ensuring the patients' needs are met effectively.

## :gear: What's Inside?

The project followed a structured process:

1. **Feasibility Analysis** - We conducted comprehensive market research and cost analysis, considering various factors.
2. **Prototype Design** - Our team developed a functional prototype of the robotic nurse.
3. **Hardware Assembly** - We handpicked the hardware components, ensuring compatibility and effectiveness.
4. **Software Integration** - The chosen hardware was fused with a well-structured software that adhered to the customer's specifications.
5. **Microcontroller Programming** - We utilized the FRDM-KL25Z microcontroller to control the AGV and enabled it to receive and send information through a website.
6. **Website Development** - In addition to the robotic nurse, we also developed a website to control and monitor the device.
7. **Testing and Refinement** - Rigorous testing was conducted to ensure the reliability and efficiency of the robot.

## :computer: Technologies Used

The following technologies were employed in this project:

- C/C++ for microcontroller programming
- HTML/CSS/JavaScript for website development
- FRDM-KL25Z Microcontroller
- ESP8266 WiFi Module
- MPU-6000 IMU

## 📝 Documentation

This repository features an extensive suite of documentation meticulously detailing each facet of the project. This includes comprehensive reports on manufacturing drawings, actuator systems, movement dynamics, and battery consumption, providing invaluable insights into the intricate operation of the autonomous guidance vehicle.

Furthermore, the repository houses an informative flyer, elucidating the project's objectives, design, and utility.

The `Doc` directory in the repository serves as a key resource, housing detailed manuals for the FRDM-KL25Z microcontroller and the MPU-6000 IMU (Inertial Measurement Unit). These manuals are heavily referenced throughout the comments in the codebase, guiding the understanding and modification of the software controlling the AGV.

The meticulous documentation provided in this repository ensures an in-depth understanding of the project, facilitating potential future enhancements and adaptations.

## 📂 Project Structure

This project has the following file structure:

```bash
├── Advisories
│   ├── Exercise1.c
│   └── Exercise2.c
│
├── Common
│   ├── IMU_1.c
│   ├── MPU_RAW.ino
│   ├── RUart.c
│   ├── main(1).c
│   ├── main wifi.c
│   ├── main wifi2.c
│   ├── susy.html
│   └── esp8266_trick
│       └── esp8266_trick.ino
│
├── New_Version_Codes
│
├── Examples
│
├── Test1
│
├── IMU
│
└── Prototype3.2
    └── Sources
```

- Advisories: This directory contains two exercise files, Exercise1.c and Exercise2.c, which serve as practice exercises for better understanding the functionality of the microcontroller.

- Common: This directory hosts a collection of code files gathered from various resources, primarily focused on the IMU and WiFi module functionalities.

- New_Version_Codes: This directory contains corrected versions of the modules detailed in the FRDM-KL25Z manual.

- Examples: This directory includes the initial versions of the microcontroller's functional modules such as tpm by interruption and polling, GPIO, Uart, I2C, etc.

- Test1: This directory holds a workspace from CodeWarrior, encapsulating an exam/test designed to assess the programmers' abilities under a time constraint.

- IMU: This directory contains the code files related to the Inertial Measurement Unit (IMU).

- Prototype3.2: This directory contains the entire project workspace from CodeWarrior. All codes related to the project are located in a subdirectory named "Sources".

# :mortar_board: Credits
This project was undertaken during my time at Fluxing Engineering in collaboration with my university, Tecnológico de Monterrey. The team and I embarked on this journey to leverage our engineering skills to contribute positively to the healthcare sector, specifically during the COVID-19 pandemic.

## :email: Contact Information
For additional details or discussion related to this project, please connect with me via jair2000.0224@hotmail.com.

## :page_with_curl: License
This project is licensed under the terms of the MIT License. It was primarily designed for academic and research

