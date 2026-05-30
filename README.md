# Team LockBusters Spring Quarterly Project

## Inspiration

UCSD students need scooters to traverse around the large campus. However, they face the ever-looming possibility of their scooter being stolen. Emails are sent out regularly alerting of a new scooter theft that has happened on campus. We aim to alleviate some of the worry students face by creating a security system that will keep them updated on the security of their scooter, ward off potential thieves, and provide easy access to locking/unlocking their scooter. The theme of security and safety is closely intertwined with the goals of our project as we aim to keep scooters secure and make those who own them feel safer leaving them behind during classes.

## What it does

The Scooter Lock System is an ESP32-powered security device that combines hardware and software to keep scooters safe. It uses an RFID scanner for authorized locking and unlocking, a vibration sensor to detect suspicious movement or tampering, two LED strips to provide clear visual status indicators., and a buzzer to alarm people around. Everything is tied together through a web interface that lets users monitor and control the lock remotely in real time.

## How we built it

We integrated an ESP32 microcontroller as the brain of the system, connecting it to an RFID scanner, a vibration sensor, a buzzer, and LED strips. The ESP32 communicates with a web application that serves as the control panel, allowing users to arm, disarm, and receive alerts from their device. Besides that, we can also physically arm or disarm using authorized ID cards.

## Challenges we ran into

One of our biggest challenges was modifying the sensitivity of the vibration sensor. We needed the system to be alert enough to catch a real theft attempt, but smart enough not to trigger a false alarm from a light bump or someone walking by. Distinguishing between random contact and an actual theft in progress required a lot of testing and modification.

## Accomplishments that we're proud of

We're proud that what started as an idea born out of a real campus problem actually came to life as a working system. Beyond the technical build, we're proud of how much the project evolved as we made upgrades throughout the process. Ultimately, we built it as a team, navigating and combining different ideas and perspectives to find common ground and deliver something we all stand behind.

## What we learned

This project taught us how to build an end-to-end embedded system from the scratch, coming up with ideas, choosing suitable components, and  bridging hardware and web software in a way that actually works together. We also learn to work as collaborators, learning how to communicate effectively, divide work, and align on decisions as a team. Testing the system under different conditions gave us a much deeper appreciation for how important edge case thinking is in engineering and computer science.

## What's next for Scooter Lock System

If we have more time,  we'd love to optimize across the board, refining the sensor algorithms for even smarter theft detection, implying a screen monitor, and polishing the web interface for a smoother user experience. We also see potential in adding features like GPS tracking, using solar batteries, and integration with campus security systems to make it a truly comprehensive anti-theft solution.
