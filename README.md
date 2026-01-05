
# STM32F4 Multi‑Slot Bootloader

## Overview

This project implements a **multi‑slot bootloader** for the **STM32F4 Discovery Board (STM32F407VG)**.  
The bootloader allows the user to select and boot one of multiple firmware images stored in different Flash sectors at runtime using physical buttons.

The design focuses on:
- Deterministic bare‑metal boot flow
- Safe application validation before jumping
- Clear separation between bootloader and applications

This project is intended to demonstrate **firmware, bootloader, and low‑level embedded systems knowledge**.

---

### Development Board Setup

 <img src="https://github.com/user-attachments/assets/ff830db3-2195-41d8-ae59-1ea94834de1b" width="25%">

---

## Key Features

- Multi‑slot application support (Default App, App1, Factory App)
- User‑controlled boot menu using hardware buttons
- Safe application validation before execution
- Clean handoff from bootloader to application
- Explicit stack pointer re‑initialization
- Custom linker script with fixed Flash layout
- Shared bootloader APIs via dedicated Flash section

---

## Hardware Requirements

- STM32F4 Discovery Board (STM32F407VG)
- On‑board **Blue User Button** (boot menu trigger + selection input)
- External pushbutton connected to **PA8** (acts as ENTER / START)

---

## Flash Memory Layout

| Flash Region | Address Range | Purpose |
|-------------|--------------|--------|
| Sector 0 | `0x08000000 – 0x08003FFF` | Bootloader |
| Sector 1 | `0x08004000 – 0x08007FFF` | Default Application |
| Sector 2 | `0x08008000 – 0x0800BFFF` | Application 1 |
| Sector 3 | `0x0800C000 – 0x0800FFFF` | Factory Application |
| Custom API Table | `0x08010000` | Shared Bootloader APIs |

---

## Boot Process Explained

### 1. Power‑On / Reset

When the board is powered on or reset:

1. The STM32 fetches the **initial Main Stack Pointer (MSP)** from address:
   ```
   0x08000000
   ```
2. The **Reset_Handler address** is fetched from:
   ```
   0x08000004
   ```
3. Execution begins in the bootloader.

This behavior is defined by the ARM Cortex‑M architecture.

---

### 2. Vector Table Configuration

The bootloader configures the Vector Table Offset Register (VTOR):

```c
SCB->VTOR = FLASH_BASE | 0x0000;
```

This ensures:
- Interrupts initially resolve to the bootloader vector table
- Applications can later relocate VTOR to their own vector tables

---

### 3. Bootloader Initialization

The bootloader:
- Enables the FPU
- Initializes the system timebase
- Initializes an LED
- Initializes buttons
- Exposes common APIs through a shared Flash table

The shared API table is placed at a fixed address (`0x08010000`) so applications can reuse bootloader services.

---

### 4. Boot Mode Decision

At startup, the bootloader checks the **Blue User Button**:

- **Button NOT pressed**
  - Automatically jumps to the **Default Application**
- **Button pressed**
  - Enters the **Bootloader Menu**

---

## Bootloader Menu Operation

When the bootloader menu is active:

1. The user presses the **Blue User Button**:
   - 1 press → App 1
   - 2 presses → Factory App
   - 3 presses → Default App
2. The user presses the **external PA8 button** to confirm (ENTER)
3. The selected application is validated and executed

Menu feedback is printed over the debug interface.

---

## Example of Bootloader Menu

<img width="25%" alt="image" src="https://github.com/user-attachments/assets/98b7fccc-e1db-440e-9906-f39812e545cc" />

---

## Example of App 1 Running

<img width="25%" height="375" alt="image" src="https://github.com/user-attachments/assets/32771af4-b929-4e94-ad15-607059daac7e" />

---

## Application Validation

Before jumping to an application, the bootloader validates the image:

```c
if ((*(uint32_t*)app_address & 0x2FFE0000) == 0x20020000)
```

This ensures:
- The first word is a valid SRAM address
- The image likely contains a valid vector table

This prevents jumping to empty or corrupted Flash.

---

## Jumping to an Application

The bootloader performs a **clean handoff**:

1. Reads application MSP from `app_address`
2. Reads application Reset_Handler from `app_address + 4`
3. Sets MSP using `__set_MSP()`
4. Jumps to the application Reset_Handler

```c
__set_MSP(*(uint32_t*)app_address);
jump_to_application();
```

At this point, the bootloader is no longer executing.

---

## Linker Script Highlights

- Bootloader Flash size limited to **16 KB**
- Fixed Flash addresses ensure deterministic layout
- Dedicated section for shared APIs:
  ```ld
  .COMMON_APIS 0x08010000
  ```
- Applications can link against known addresses
- RAM, CCMRAM, and Flash explicitly controlled

This layout mimics real production firmware systems.

---

## Possible Extensions

- CRC or signature‑based image verification
- UART / USB firmware updates
- Dual‑bank fail‑safe upgrades
- Application version metadata
- Watchdog‑based rollback logic
