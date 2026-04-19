# Cinema Ticket Booking System

## Project Overview
The Cinema Ticket Booking System is a console-based application developed in C that allows users to view movie shows, search for movies, book tickets, cancel tickets, filter shows by genre, and display the most booked shows.

The system uses a Binary Search Tree (BST) to store show details efficiently, along with a queue for ticket booking, a waiting list for unavailable shows, and a stack for cancellation tracking.

## Features
- Display all available movie shows
- Search shows by movie name
- Book tickets for selected seats
- Cancel booked tickets
- Filter shows by genre
- View top booked shows
- VIP booking support
- Waiting list management when seats are full
- Seat-wise booking using seat codes like A1, B5, etc.

## Data Structures Used
- Binary Search Tree (BST) – for storing shows
- Queue – for managing bookings
- Waiting List Queue – for handling overflow requests
- Stack – for tracking cancellations

## File Structure
- main.c → Main program and menu
- cinema_booking.c → Core functionality
- cinema_booking.h → Structure definitions and declarations
- data.c → Show data handling
- shows_data.txt → Input file containing movie data

## How to Compile
gcc main.c cinema_booking.c data.c -o cinema_booking

(If needed)
gcc -std=c99 main.c cinema_booking.c data.c -o cinema_booking

## How to Run

Linux / macOS / Git Bash:
./cinema_booking

Windows:
cinema_booking.exe

## Show Data Format
id,name,genre,time,duration,price,vip_price,seats_available,total_seats,bookings_count

Example:
1,The Dark Knight,Action,14:30,2.5,350.0,420.0,50,50,0

## Menu Options
1. Display All Shows
2. Search for a Show (by Name)
3. Book a Ticket
4. Cancel a Ticket
5. Filter Shows by Genre
6. View Top Booked Shows
7. Exit

## Booking Details
- Seat format: A1, B3, C10
- Rows: A to E
- Seats per row: 10
- O = Available
- X = Booked

## Additional Features
- VIP users get priority and different pricing
- Waiting list is used when seats are full
- Cancellation updates availability and processes waiting list
- Fully console-based application

## Conclusion
This project demonstrates practical usage of data structures like BST, Queue, Stack, and file handling in C through a real-world cinema booking system.

---
Developed as a C Programming Mini Project
