# Using SX1276 for Long-Range Communication with Narrow Bandwidth Modulation

## Overview

The **SX1276** chip natively supports **FSK**, **LoRa**, and **OOK** modulations. **LoRa** is an excellent modulation technology that has proven itself in long-range communication due to its high sensitivity and resistance to interference. However, it has some drawbacks, notably limited throughput due to the required channel width (the minimum bandwidth for LoRaWAN is **125 kHz**).

To increase the range of data transmission over a radio channel, it's necessary to reduce the data transfer rate. According to Claude Shannon's information theory, there is a fundamental limitation on the data transfer rate depending on the signal-to-noise ratio and the channel bandwidth.

**Shannon's Theory:**

> **The maximum achievable communication distance is proportional to the square root of the energy invested in an information bit and practically does not depend on the channel width and modulation method if the channel width equals or exceeds the data transfer rate.**

This means that to transmit data over long distances with limited power, we need to decrease the data rate and use a narrow bandwidth.

## Solution

This project allows the **SX1276** chip to transmit signals by emulating **DBPSK** (Differential Binary Phase Shift Keying) modulation at a minimal data rate with a bandwidth of about **100 Hz**. This is achieved by using the **FSK** mode of the SX1276 chip and forming a special bitstream that shifts phase.

Using a bandwidth of around 100 Hz significantly increases communication range at the same transmitter power and requires minimal channel occupancy.

This project uses PlatformIO and [B-L072Z-LRWAN1 LoRa Discovery kit](https://os.mbed.com/platforms/ST-Discovery-LRWAN1/) as a development platform.

## Demo
![Demo](demo.gif)
