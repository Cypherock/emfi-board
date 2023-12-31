
# Cypherock EMFI Board

The Cypherock EMFI device is a low-cost Electromagnetic Fault Injection (EMFI) device, developed by Cypherock for studying and researching physical security threats that appear at a circuit level in modern chips.  

![pcb_front](https://github.com/Cypherock/emfi-board/blob/main/images/pcb_front.jpg)      

![pcb_back](https://github.com/Cypherock/emfi-board/blob/main/images/pcb_back.jpg)





## Background

Electromagnetic fault injection (EMFI) is used to disrupt the behavior of a chip and for weakening its security, typically targetting simple microcontrollers. It involves injecting electromagnetic shock at specific points on a target device's hardware to induce faults or errors. EMFI is one of the most common and popular physical attacks deployed or performed on various embedded systems, known for its capability to easily bypass and break through most of the security measures employed in modern chips. The following are two different attacks that can be performed using EMFI.

*Control Flow Manipulation Attacks* : 
Control flow manipulation in applications involves altering the execution order of instructions, often for malicious purposes. In embedded devices and microcontrollers, attackers can manipulate control flow without requiring vulnerabilities in the application code. Hardware attacks like voltage glitching, electromagnetic fault injection (EMFI), and laser fault injection (LFI) are employed to disrupt control flow and modify application code. These fault attacks can lead to unexpected behavior, including instruction skips, data corruption in memory, instruction modification, and tampering with the program counter. Such behavior can then be exploited by attackers to gain privileges, bypass security measures, extract information, inject malware, and more.

*Read Out Protection Bypass Attacks* : 
Read out protection (RDP) is a mechanism that restricts external access to the internals (flash memory and SRAM) of a microcontroller. Typically, this access is facilitated through a debug interface. However, depending on the application a microcontroller is used for, partial access might be sufficient for plotting attacks, for example only to the internal SRAM or some CPU registers. To compromise the RDP mechanism, physical attacks aims to interfere with the behavior of the underlying hardware implementations of a read-out protection. Again methods like voltage glitching and EMFI are perfect  for inducing faults in the MCUs code and operation. EMFI is a non-invasive attack method that does not require physical modifications to the microcontroller's hardware such as chip decapsulation etc. In practice, the read-out protection of some microcontrollers can be temporarily disabled and bypassed by applying a voltage glitch or injecting a pulse via EMFI during the power-up phase of the microcontrollers. 

## Working

The device is powered by USB. A programmable PWM signal is given as input from an STM32L4 MCU, this is used to control the output voltage (by changing the PWM parameters) generated by the device. When this signal is high the step up transformer will raise the voltage at its primary, converting the low voltage to high voltage at the secondary terminal. The step up transformer has primary to secondary winding ratio of 1:200. This voltage in the secondary is then again multiplied by a factor of “4” by the voltage multiplier and a very high voltage is generated. The charge is stored in the capacitor bank. All the capacitors used in this setup have a breakdown voltage of 1kV. The switching circuit is used to transfer this charge on to the EM probe. An external trigger or a controllable pulse is given as an input signal to the IGBT for a short period of time for switching. The external trigger can be given directly to the switching circuit to inject the fault, or through the MCU in which the MCU can modify or add delays to the signal programmatically. The MCU on its own can also be used to generate the pulse. When the IGBT is in ON state, the high voltage is transferred to the load which is the EM probe. Due to the high current that circulates in the coil of the probe, a magnetic field would be generated and the probe uses this electromagnetic field to transfer an electromagnetic shock to the DUT, injecting faults.


![working_block_diagram](https://github.com/Cypherock/emfi-board/blob/main/images/working_block_diagram.png)



The target microcontroller could be mounted on a 3-axis position stage. With this, the injection coil can be positioned precisely on the device and an automated grid scan can be performed. To ensure that the target device is always in a defined state, a programmable power supply source to power cycle the microcontroller should be used. Power-cycling could be applied before each measurement or for recovery, if the faulted microcontroller malfunctions. With the delay generator capability within the EMFI board, it is ensured that the fault is injected at the right point in time. A host PC could be used to control the entire setup and instrument the target microcontroller via a debug probe.

## Features

* Supports external power supply.  
* Supports external trigger or signal as input to generate pulse. 
* Supports modifying the signal programmatically.
* Programmable High Voltage Generation.
* Protection case to eliminate exposure to high voltage circuitry and for safe operation.
* Three on board LEDs for which can be used as indicators such as warning light etc.
* Low Input Power Requirement.
* Controlled Shock Injection.
* Two SMA Connectors to facilitate changing the polarity of the injected EM pulse.

## Electromagnetic (EM) Probe

The EM probe is required to inject shock into the target DUT. It can be constructed easily using a [power inductor](https://www.mouser.in/ProductDetail/710-744710610) and an [SMA connector](https://www.mouser.in/ProductDetail/712-CONSMA013.062-G). The power inductor would have a ferrite core wrapped around with coil. It converts the electrical energy into magnetic energy. The power inductor can be connected to an SMA connector to complete this setup. 

## Device Under Test (DUTs)   

The below list shows the microcontrollers that are commonly used in hardware crypto wallets. These microcontrollers are susceptible to EMFI based attacks. We have designed the DUT boards for EMFI analysis on these microcontrollers. These boards are included inside the hardware folder.

* STM32F205RET6
* STM32F427VIT6TR
* STM32L496RGT6
* STM32H753VIT6
* ESP32-D0WD-V3
* ATSAMD51J20A-AU
* ATMEGA328P-AU

## Usage

### EMFI firmware 
- STM32CubeIDE is required for building and debugging the project.
- After installation of the IDE, the project can be accessed by either using the .project/.cproject files.
- Use the launch configurations for debugging and programming the EMFI board.

### Use the EMFI tool for generating shoots
- Install python dependencies from requirements.txt
- Connect EMFI board's power and USB comm ports
- Run the python in Scripts/Cypherock/test.py
- Enter the COM port detected on the system
- After connection is created, EMFI voltage would start rising
- On pressing enter on the terminal, the device would start a EMFI shoot
- On pressing enter again, the device would turn off the PWM and voltage would gradually decrease


## License

Cypherock EMFI Board is released under [GNU General Public License](https://www.gnu.org/licenses/gpl-3.0.en.html) Version 3.

