#include "cinema_booking.h"
#include <ctype.h>

// Function to display the main menu
void display_menu() {
    printf("\n=== CINEMA TICKET BOOKING SYSTEM ===\n");
    printf("1. Display All Shows\n");
    printf("2. Search for a Show (by Name)\n");
    printf("3. Book a Ticket\n");
    printf("4. Cancel a Ticket\n");
    printf("5. Filter Shows by Genre\n");
    printf("6. View Top Booked Shows\n");
    printf("7. Exit\n");
    printf("\nEnter your choice (1-7): ");
}

// Function to initialize shows from file
void initialize_sample_shows(ShowNode** root) {
    FILE* file = fopen("shows_data.txt", "r");
    if (file == NULL) {
        printf("Error: Could not open shows_data.txt\n");
        return;
    }

    char line[1024];
    int line_num = 0;
    int shows_loaded = 0;
    
    while (fgets(line, sizeof(line), file)) {
        line_num++;
        // Skip empty lines or lines with only whitespace
        if (strspn(line, " \t\r\n") == strlen(line)) {
            continue;
        }
        
        Show show = {0};
        // Initialize all seats to 0 (available)
        memset(show.seats, 0, sizeof(show.seats));
        
        // Parse the line using sscanf with more flexible whitespace handling
        int fields_read = sscanf(line, "%d , %49[^,], %19[^,], %5[^,], %f , %f , %f , %d , %d , %d",
                              &show.id, show.name, show.genre, show.time, &show.duration,
                              &show.price, &show.vip_price, &show.seats_available,
                              &show.total_seats, &show.bookings_count);
        
        if (fields_read == 10) {
            // Trim any whitespace from the end of strings
            show.name[strcspn(show.name, " \t\r\n")] = '\0';
            show.genre[strcspn(show.genre, " \t\r\n")] = '\0';
            show.time[strcspn(show.time, " \t\r\n")] = '\0';
            
            *root = insert_show(*root, show);
            shows_loaded++;
            printf("Successfully loaded show: %s (ID: %d)\n", show.name, show.id);
        } else {
            // Remove trailing newline for cleaner error message
            line[strcspn(line, "\r\n")] = '\0';
            printf("Warning: Skipping malformed entry at line %d. Fields read: %d/10\n", line_num, fields_read);
            printf("Line content: %s\n", line);
            printf("Expected format: id,name,genre,time,duration,price,vip_price,seats_available,total_seats,bookings_count\n");
            printf("Example: 1,The Dark Knight,Action,14:30,2.5,350.0,420.0,50,50,0\n\n");
        }
    }
    
    fclose(file);
    printf("Successfully loaded %d shows from shows_data.txt\n", shows_loaded);
}

int main() {
    ShowNode* root = NULL;
    TicketQueue ticket_queue;
    WaitingList waiting_list;
    TicketStackNode* cancellation_stack = NULL;
    
    // Initialize data structures
    init_ticket_queue(&ticket_queue);
    init_waiting_list(&waiting_list);
    
    // Load sample data
    initialize_sample_shows(&root);
    
    int choice;
    int show_id, n;
    char genre[MAX_GENRE_LENGTH];
    
    printf("=== WELCOME TO CINEMA TICKET BOOKING SYSTEM ===\n");
    
    do {
        display_menu();
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                printf("\n=== CURRENTLY SHOWING ===\n");
                display_shows(root);
                printf("\n");
                break;
                
            case 2: {
                char name_query[MAX_NAME_LENGTH];
                printf("\nEnter movie name to search: ");
                scanf(" %[^\n]", name_query);
                search_shows_by_name(root, name_query);
                break;
            }
                
            case 3:
                book_ticket(root, &ticket_queue, &waiting_list);
                break;
                
            case 4:
                cancel_ticket(root, &ticket_queue, &cancellation_stack);
                // After cancellation, check if we can process waiting list
                process_waiting_list(root, &waiting_list, &ticket_queue);
                break;
                
            case 5: {
                printf("\nEnter genre to filter by: ");
                scanf(" %[^\n]", genre);
                printf("\n=== SHOWS IN GENRE: %s ===\n", genre);
                filter_shows_by_genre(root, genre);
                printf("\n");
                break;
            }
                
            case 6: {
                printf("\nEnter number of top shows to display: ");
                scanf("%d", &n);
                display_top_n_shows(root, n);
                break;
            }
                
            case 7:
                printf("\nThank you for using Cinema Ticket Booking System. Goodbye!\n");
                break;
                
            default:
                printf("\nInvalid choice! Please try again.\n");
        }
        
        // Add a pause to see the output (no screen clearing)
        if (choice != 7) {
            printf("\nPress Enter to continue...");
            while (getchar() != '\n'); // Clear input buffer
            getchar(); // Wait for Enter key
        }
        
    } while (choice != 7);
    
    // Clear screen only on exit
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif

    // Free allocated memory
    free_show_tree(root);
    
    // Free ticket queue
    TicketNode* temp;
    while (ticket_queue.front != NULL) {
        temp = ticket_queue.front;
        ticket_queue.front = ticket_queue.front->next;
        free(temp);
    }
    
    // Free waiting list
    while (waiting_list.front != NULL) {
        temp = waiting_list.front;
        waiting_list.front = waiting_list.front->next;
        free(temp);
    }
    
    // Free cancellation stack
    TicketStackNode* temp_stack;
    while (cancellation_stack != NULL) {
        temp_stack = cancellation_stack;
        cancellation_stack = cancellation_stack->next;
        free(temp_stack);
    }
    
    return 0;
}
