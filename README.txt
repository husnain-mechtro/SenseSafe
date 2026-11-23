SENSESAFE — FINAL SUBMISSION SOURCE PACKAGE

PROJECT
SenseSafe: IoT-Based LPG Leakage Detection and Alert System
Air University — Department of Mechatronics Engineering
Section A | Supervisor: Umer Farooq
Document: ICT-MTS-SS-001 | Revision 1.0 | 22 November 2026

TEAM
- Husnain Shaheen — 2600847
- Minahil Masood — 2600838
- Muhammad Waleed bin Mahmood — 2600832

MAIN DELIVERABLES
1. SenseSafe_Proposal.pdf — three-page technical proposal.
2. SenseSafe_Proposal.docx — editable proposal source.
3. SenseSafe_Design_Artifact.html — interactive engineering design document.
4. website/index.html — showcase portfolio and simulated dashboard.
5. website/SenseSafe_Design_Artifact.html — artifact copy embedded by the website.
6. ESP32_WebHost/ — Arduino firmware and LittleFS data folder for serving the site from the ESP32-C3.
7. Claude_Artifact_Prompt.txt — project-specific prompt prepared for the assignment's Claude Artifacts step.
8. sensesafe_architecture.png — proposal architecture diagram source image.

WEBSITE HOSTING DECISION
The showcase is designed to be served directly by the ESP32-C3 using Arduino WebServer and LittleFS. In AP mode, join the device network:
  SSID: SenseSafe-2600847
  Password: SenseSafeDemo
  URL: http://192.168.4.1
In station mode, connect the ESP32-C3 to the same router as the viewer and use the local IP printed in Serial Monitor. A GitHub repository should contain the source/firmware and be linked in the final submission. ESP32 hosting is local-network/device-hosted; it is not reachable from the public Internet by default.

HOW TO PREPARE THE ESP32 HOST
See ESP32_WebHost/README.md. Copy the two files from ESP32_WebHost/data/ into the LittleFS filesystem root. The sketch currently provides simulated telemetry only.

IMPORTANT CLAUDE REQUIREMENT
The instructor's handout specifies that the interactive HTML engineering artifact is to be generated via Claude with Artifacts enabled. The included HTML is a prepared working draft, and Claude_Artifact_Prompt.txt contains the project-specific prompt to paste into Claude. If the lecturer requires proof of Claude Artifact provenance, open Claude, run that prompt, review the returned artifact and download its HTML. Do not claim that the included draft was generated inside the team's Claude account.

BEFORE FINAL SUBMISSION
- Review spellings of all names, registration numbers, section and supervisor.
- Confirm exact ESP32-C3 SuperMini OLED board revision and the OLED driver/pins (common variant: SDA GPIO5, SCL GPIO6, 72x40 SSD1306-compatible).
- Confirm the actual MQ-2 module, buzzer module voltage/trigger polarity, and available power supply before wiring.
- Verify all supplier prices and stock. Several small parts/power costs are marked estimates.
- Verify the local ESP32 host after uploading the sketch and LittleFS data folder.
- Create a GitHub repository and add the source files. Provide the repository link with submission if required.
- Test the design artifact 3D view on a network with Internet access because the assignment-approved Three.js libraries load from pinned CDNs.

TECHNICAL LIMITS
- The dashboard data and OLED states served by the included host sketch are simulated. The firmware does not currently sample MQ-2 or energise a physical buzzer/LED.
- The proposed hardware pins are provisional pending confirmation against the exact board revision and actual modules.
- MQ-2 is broad-response/cross-sensitive, needs warm-up and calibration, and raw ADC units are not ppm. It is not a certified LPG life-safety detector and must not replace an approved gas alarm.
- Never connect an MQ-2 analogue output that may reach 5 V directly to an ESP32-C3 input. Proposed divider: 12 kOhm from AO to ADC node and 10 kOhm from ADC node to GND (5 V becomes about 2.27 V).
- No physical circuit has been assembled or tested as part of this file preparation.
