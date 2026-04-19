#ifndef CINEMA_BOOKING_H
#define CINEMA_BOOKING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define MAX_NAME_LENGTH 50
#define MAX_GENRE_LENGTH 20
#define MAX_SHOWS 100
#define SEATS_PER_ROW 10
#define NUM_ROWS 5
#define MAX_WAITING 10

// Show structure
typedef struct {
    int id;
    char name[MAX_NAME_LENGTH];
    char genre[MAX_GENRE_LENGTH];
    char time[6]; // HH:MM format
    float duration;
    float price;
    float vip_price; // distinct price for VIP members
    int seats_available;
    int total_seats;
    int bookings_count; // For tracking most booked shows
    int seats[NUM_ROWS][SEATS_PER_ROW]; // 0 = available, 1 = booked
} Show;

// BST Node for shows
typedef struct ShowNode {
    Show show;
    struct ShowNode *left;
    struct ShowNode *right;
} ShowNode;

// Queue Node for ticket booking
typedef struct TicketNode {
    int show_id;
    char customer_name[MAX_NAME_LENGTH];
    int seat_number;
    bool is_vip;
    struct TicketNode *next;
} TicketNode;

// Queue for ticket booking
typedef struct {
    TicketNode *front;
    TicketNode *rear;
    int size;
} TicketQueue;

// Stack for ticket cancellation
typedef struct TicketStackNode {
    int show_id;
    int seat_number;
    struct TicketStackNode *next;
} TicketStackNode;

// Waiting list structure (Queue)
typedef struct {
    TicketNode *front;
    TicketNode *rear;
    int size;
} WaitingList;

// Function declarations
ShowNode* create_show_node(Show show);
ShowNode* insert_show(ShowNode* root, Show show);
void display_shows(ShowNode* root);
Show* search_show(ShowNode* root, int show_id);
void search_shows_by_name(ShowNode* root, const char* name);
void display_show_details(Show* show);
void display_seating_chart(Show* show);

void init_ticket_queue(TicketQueue *q);
void enqueue_ticket(TicketQueue *q, int show_id, const char* customer_name, bool is_vip);
TicketNode* dequeue_ticket(TicketQueue *q);
void book_ticket(ShowNode* root, TicketQueue *q, WaitingList *wl);
void cancel_ticket(ShowNode* root, TicketQueue *q, TicketStackNode **stack);

void init_waiting_list(WaitingList *wl);
void add_to_waiting(WaitingList *wl, int show_id, const char* customer_name, bool is_vip);
void process_waiting_list(ShowNode* root, WaitingList *wl, TicketQueue *q);

void push_cancellation(TicketStackNode **top, int show_id, int seat_number);
void display_top_n_shows(ShowNode* root, int n);
void filter_shows_by_genre(ShowNode* root, const char* genre);
void free_show_tree(ShowNode* root);

// Helpers
int ci_strcmp(const char* a, const char* b);
int parse_seat_code(const char* code, int* out_row, int* out_col); // returns 0 on success
int seat_index_linear(int row, int col); // 1-based linear numbering

#endif // CINEMA_BOOKING_H
