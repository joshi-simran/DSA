#include "cinema_booking.h"
#include <ctype.h>

// Helper function to create a new show node
ShowNode* create_show_node(Show show) {
    ShowNode* node = (ShowNode*)malloc(sizeof(ShowNode));
    if (node) {
        node->show = show;
        node->left = node->right = NULL;
        // Initialize per-seat map to available
        for (int r = 0; r < NUM_ROWS; r++) {
            for (int c = 0; c < SEATS_PER_ROW; c++) {
                node->show.seats[r][c] = 0;
            }
        }
    }
    return node;
}

// Insert a show into the BST
ShowNode* insert_show(ShowNode* root, Show show) {
    if (root == NULL) {
        return create_show_node(show);
    }
    
    if (show.id < root->show.id) {
        root->left = insert_show(root->left, show);
    } else if (show.id > root->show.id) {
        root->right = insert_show(root->right, show);
    }
    
    return root;
}

// In-order traversal to display all shows
void display_shows(ShowNode* root) {
    if (root != NULL) {
        display_shows(root->left);
        printf("\nID: %d | %-30s | %-15s | %s | Duration: %.1f hrs | Rs.%.2f | Seats: %d/%d",
               root->show.id, root->show.name, root->show.genre,
               root->show.time, root->show.duration, root->show.price,
               root->show.seats_available, root->show.total_seats);
        display_shows(root->right);
    }
}

// Search for a show by ID
Show* search_show(ShowNode* root, int show_id) {
    if (root == NULL) {
        return NULL;
    }
    
    if (show_id < root->show.id) {
        return search_show(root->left, show_id);
    } else if (show_id > root->show.id) {
        return search_show(root->right, show_id);
    } else {
        return &(root->show);
    }
}

// Display detailed information about a show
void display_show_details(Show* show) {
    if (show == NULL) {
        printf("\nShow not found!\n");
        return;
    }
    
    printf("\n=== SHOW DETAILS ===\n");
    printf("ID: %d\n", show->id);
    printf("Name: %s\n", show->name);
    printf("Genre: %s\n", show->genre);
    printf("Time: %s\n", show->time);
    printf("Duration: %.1f hours\n", show->duration);
    printf("Price: Rs.%.2f  | VIP Price: Rs.%.2f\n", show->price, show->vip_price);
    printf("Seats Available: %d/%d\n", show->seats_available, show->total_seats);
    printf("\nSeating Chart:\n");
    display_seating_chart(show);
}

// Display seating chart for a show
void display_seating_chart(Show* show) {
    if (show == NULL) return;
    
    printf("\n    SCREEN THIS WAY    \n");
    printf("  ------------------\n");
    // Print column headers
    printf("   ");
    for (int c = 1; c <= SEATS_PER_ROW; c++) {
        printf("%2d ", c);
    }
    printf("\n");
    for (int r = 0; r < NUM_ROWS; r++) {
        printf("%c |", 'A' + r);
        for (int c = 0; c < SEATS_PER_ROW; c++) {
            int linear = r * SEATS_PER_ROW + (c + 1);
            if (linear <= show->total_seats) {
                printf(" %c ", show->seats[r][c] ? 'X' : 'O');
            }
        }
        printf("\n");
    }
    printf("\nO = Available   X = Booked\n");
}

// Initialize ticket queue
void init_ticket_queue(TicketQueue *q) {
    q->front = q->rear = NULL;
    q->size = 0;
}

// Add ticket to queue (FIFO)
void enqueue_ticket(TicketQueue *q, int show_id, const char* customer_name, bool is_vip) {
    TicketNode* new_node = (TicketNode*)malloc(sizeof(TicketNode));
    if (!new_node) return;
    
    new_node->show_id = show_id;
    strncpy(new_node->customer_name, customer_name, MAX_NAME_LENGTH - 1);
    new_node->customer_name[MAX_NAME_LENGTH - 1] = '\0';
    new_node->is_vip = is_vip;
    new_node->next = NULL;
    
    if (q->rear == NULL) {
        q->front = q->rear = new_node;
    } else {
        // If VIP, add to front of queue
        if (is_vip) {
            new_node->next = q->front;
            q->front = new_node;
        } else {
            q->rear->next = new_node;
            q->rear = new_node;
        }
    }
    q->size++;
}

// Remove ticket from queue
TicketNode* dequeue_ticket(TicketQueue *q) {
    if (q->front == NULL) return NULL;
    
    TicketNode* temp = q->front;
    q->front = q->front->next;
    
    if (q->front == NULL) {
        q->rear = NULL;
    }
    
    q->size--;
    return temp;
}

// Book a ticket
void book_ticket(ShowNode* root, TicketQueue *q, WaitingList *wl) {
    int show_id;
    char customer_name[MAX_NAME_LENGTH];
    char vip_choice;
    bool is_vip = false;
    int seats_to_book = 1;
    
    printf("\n=== BOOK TICKET ===\n");
    printf("Enter Show ID: ");
    scanf("%d", &show_id);
    
    Show* show = search_show(root, show_id);
    if (show == NULL) {
        printf("Show not found!\n");
        return;
    }
    
    display_show_details(show);
    
    if (show->seats_available <= 0) {
        printf("\nSorry, no seats available for this show. Would you like to be added to the waiting list? (y/n): ");
        scanf(" %c", &vip_choice);
        if (tolower(vip_choice) == 'y') {
            printf("Enter your name: ");
            scanf(" %[^\n]", customer_name);
            printf("Are you a VIP member? (y/n): ");
            scanf(" %c", &vip_choice);
            is_vip = (tolower(vip_choice) == 'y');
            add_to_waiting(wl, show_id, customer_name, is_vip);
            printf("Added to waiting list. We'll notify you if seats become available.\n");
        }
        return;
    }
    
    printf("\nEnter your name: ");
    scanf(" %[^\n]", customer_name);
    
    printf("Are you a VIP member? (y/n): ");
    scanf(" %c", &vip_choice);
    is_vip = (tolower(vip_choice) == 'y');

    printf("\nAvailable seats: %d\n", show->seats_available);
    printf("How many seats do you want to book? ");
    scanf("%d", &seats_to_book);
    if (seats_to_book <= 0) {
        printf("Invalid number of seats.\n");
        return;
    }
    if (seats_to_book > show->seats_available) {
        printf("Not enough seats available. You can book up to %d.\n", show->seats_available);
        return;
    }

    char seat_code[8];
    int selected_rows[100];
    int selected_cols[100];
    int selected_count = 0;
    
    for (int k = 0; k < seats_to_book; k++) {
        printf("Enter seat code (e.g., A3): ");
        scanf(" %7s", seat_code);
        int row, col;
        if (parse_seat_code(seat_code, &row, &col) != 0) {
            printf("Invalid seat code. Use format RowLetter + ColumnNumber, e.g., B5.\n");
            k--; // retry this seat
            continue;
        }
        int linear = row * SEATS_PER_ROW + (col + 1);
        if (linear < 1 || linear > show->total_seats) {
            printf("Seat out of range for this show.\n");
            k--; continue;
        }
        if (show->seats[row][col]) {
            printf("Seat %c%d is already booked.\n", 'A' + row, col + 1);
            k--; continue;
        }
        // Check duplicates in current selection
        bool dup = false;
        for (int m = 0; m < selected_count; m++) {
            if (selected_rows[m] == row && selected_cols[m] == col) { dup = true; break; }
        }
        if (dup) {
            printf("You already selected seat %c%d in this booking.\n", 'A' + row, col + 1);
            k--; continue;
        }
        selected_rows[selected_count] = row;
        selected_cols[selected_count] = col;
        selected_count++;
    }

    // Commit booking
    for (int m = 0; m < selected_count; m++) {
        int r = selected_rows[m], c = selected_cols[m];
        show->seats[r][c] = 1;
        show->seats_available--;
        show->bookings_count++;
        enqueue_ticket(q, show_id, customer_name, is_vip);
    }

    float per_seat = is_vip ? show->vip_price : show->price;
    float total = per_seat * selected_count;

    printf("\n=== TICKET(S) BOOKED SUCCESSFULLY ===\n");
    printf("Show: %s\n", show->name);
    printf("Time: %s\n", show->time);
    printf("Seats: ");
    for (int m = 0; m < selected_count; m++) {
        printf("%c%d%s", 'A' + selected_rows[m], selected_cols[m] + 1, (m == selected_count - 1) ? "" : ", ");
    }
    printf("\nCustomer: %s\n", customer_name);
    printf("Tickets: %d\n", selected_count);
    printf("Price per ticket: Rs.%.2f\n", per_seat);
    printf("Total Price: Rs.%.2f\n", total);
    printf("Thank you for your booking!\n");
}

// Initialize waiting list
void init_waiting_list(WaitingList *wl) {
    wl->front = wl->rear = NULL;
    wl->size = 0;
}

// Add customer to waiting list
void add_to_waiting(WaitingList *wl, int show_id, const char* customer_name, bool is_vip) {
    if (wl->size >= MAX_WAITING) {
        printf("Waiting list is full!\n");
        return;
    }
    
    TicketNode* new_node = (TicketNode*)malloc(sizeof(TicketNode));
    if (!new_node) return;
    
    new_node->show_id = show_id;
    strncpy(new_node->customer_name, customer_name, MAX_NAME_LENGTH - 1);
    new_node->customer_name[MAX_NAME_LENGTH - 1] = '\0';
    new_node->is_vip = is_vip;
    new_node->next = NULL;
    
    if (wl->rear == NULL) {
        wl->front = wl->rear = new_node;
    } else {
        // Add to end of waiting list
        wl->rear->next = new_node;
        wl->rear = new_node;
    }
    wl->size++;
}

// Process waiting list when seats become available
void process_waiting_list(ShowNode* root, WaitingList *wl, TicketQueue *q) {
    if (wl->front == NULL) return;
    
    TicketNode* current = wl->front;
    TicketNode* prev = NULL;
    
    while (current != NULL) {
        Show* show = search_show(root, current->show_id);
        if (show && show->seats_available > 0) {
            // Book the ticket for waiting customer
            show->seats_available--;
            show->bookings_count++;
            
            // Move from waiting list to ticket queue
            if (prev == NULL) {
                wl->front = current->next;
                if (wl->front == NULL) {
                    wl->rear = NULL;
                }
            } else {
                prev->next = current->next;
                if (current->next == NULL) {
                    wl->rear = prev;
                }
            }
            
            TicketNode* temp = current;
            current = current->next;
            
            // Add to ticket queue with priority
            enqueue_ticket(q, temp->show_id, temp->customer_name, temp->is_vip);
            
            printf("\nNotification: Seat available for show ID %d. %s has been booked.\n", 
                   temp->show_id, temp->customer_name);
            
            free(temp);
            wl->size--;
        } else {
            prev = current;
            current = current->next;
        }
    }
}

// Push a cancellation onto the stack
void push_cancellation(TicketStackNode **top, int show_id, int seat_number) {
    TicketStackNode* new_node = (TicketStackNode*)malloc(sizeof(TicketStackNode));
    if (!new_node) return;
    
    new_node->show_id = show_id;
    new_node->seat_number = seat_number;
    new_node->next = *top;
    *top = new_node;
}

// Cancel ticket(s) using seat codes (e.g., A3)
void cancel_ticket(ShowNode* root, TicketQueue *q, TicketStackNode **stack) {
    int show_id;
    
    printf("\n=== CANCEL TICKET ===\n");
    printf("Enter Show ID: ");
    scanf("%d", &show_id);
    
    Show* show = search_show(root, show_id);
    if (show == NULL) {
        printf("Show not found!\n");
        return;
    }
    int count;
    printf("How many tickets do you want to cancel? ");
    scanf("%d", &count);
    if (count <= 0) {
        printf("Invalid number of tickets.\n");
        return;
    }
    
    for (int i = 0; i < count; i++) {
        char seat_code[8];
        printf("Enter seat code to cancel (e.g., B5): ");
        scanf(" %7s", seat_code);
        int row, col;
        if (parse_seat_code(seat_code, &row, &col) != 0) {
            printf("Invalid seat code!\n");
            i--; // retry same index
            continue;
        }
        int linear = row * SEATS_PER_ROW + (col + 1);
        if (linear < 1 || linear > show->total_seats) {
            printf("Seat is out of range for this show.\n");
            i--; continue;
        }
        if (!show->seats[row][col]) {
            printf("Seat %c%d is not currently booked.\n", 'A' + row, col + 1);
            i--; continue;
        }
        // Free the seat
        show->seats[row][col] = 0;
        show->seats_available++;
        // Push cancellation to stack with linear index for record
        push_cancellation(stack, show_id, linear);
        printf("Ticket for seat %c%d has been cancelled.\n", 'A' + row, col + 1);
    }
}

// Display top N most booked shows using BST traversal + min-heap
typedef struct { int bookings; Show data; } HeapItem;

static void heap_swap(HeapItem* a, HeapItem* b) { HeapItem t = *a; *a = *b; *b = t; }
static void heapify_up(HeapItem* heap, int idx) {
    while (idx > 0) {
        int parent = (idx - 1) / 2;
        if (heap[parent].bookings <= heap[idx].bookings) break; // min-heap
        heap_swap(&heap[parent], &heap[idx]);
        idx = parent;
    }
}
static void heapify_down(HeapItem* heap, int size, int idx) {
    while (1) {
        int left = 2 * idx + 1, right = 2 * idx + 2, smallest = idx;
        if (left < size && heap[left].bookings < heap[smallest].bookings) smallest = left;
        if (right < size && heap[right].bookings < heap[smallest].bookings) smallest = right;
        if (smallest == idx) break;
        heap_swap(&heap[idx], &heap[smallest]);
        idx = smallest;
    }
}

static void topn_traverse(ShowNode* node, HeapItem* heap, int* heap_size, int k) {
    if (!node) return;
    topn_traverse(node->left, heap, heap_size, k);
    int b = node->show.bookings_count;
    if (*heap_size < k) {
        heap[*heap_size] = (HeapItem){ b, node->show };
        heapify_up(heap, *heap_size);
        (*heap_size)++;
    } else if (k > 0 && b > heap[0].bookings) {
        heap[0] = (HeapItem){ b, node->show };
        heapify_down(heap, k, 0);
    }
    topn_traverse(node->right, heap, heap_size, k);
}

void display_top_n_shows(ShowNode* root, int n) {
    if (root == NULL || n <= 0) return;
    if (n > MAX_SHOWS) n = MAX_SHOWS;
    HeapItem heap[MAX_SHOWS];
    int heap_size = 0;
    topn_traverse(root, heap, &heap_size, n);
    // Heap currently holds up to n smallest of the top set; to print descending, collect and sort descending by bookings
    // Simple selection since n is small
    for (int i = 0; i < heap_size - 1; i++) {
        int max_idx = i;
        for (int j = i + 1; j < heap_size; j++) {
            if (heap[j].bookings > heap[max_idx].bookings) max_idx = j;
        }
        if (max_idx != i) heap_swap(&heap[i], &heap[max_idx]);
    }
    printf("\n=== TOP %d MOST BOOKED SHOWS ===\n", heap_size);
    for (int i = 0; i < heap_size; i++) {
        printf("%d. %-30s | Bookings: %d\n", i + 1, heap[i].data.name, heap[i].bookings);
    }
}

// Filter shows by genre
static void genre_traverse(ShowNode* node, const char* genre, int* found) {
    if (!node) return;
    genre_traverse(node->left, genre, found);
    if (ci_strcmp(node->show.genre, genre) == 0) {
        if (!*found) {
            printf("\n=== SHOWS IN GENRE: %s ===\n", genre);
        }
        *found = 1;
        printf("ID: %d | %-30s | %-15s | %s | Duration: %.1f hrs | Rs.%.2f | Seats: %d/%d\n",
               node->show.id, node->show.name, node->show.genre,
               node->show.time, node->show.duration, node->show.price,
               node->show.seats_available, node->show.total_seats);
    }
    genre_traverse(node->right, genre, found);
}

void filter_shows_by_genre(ShowNode* root, const char* genre) {
    int found = 0;
    genre_traverse(root, genre, &found);
    if (!found) {
        printf("\nNo shows found in genre: %s\n", genre);
    }
}

// Free memory used by the BST
void free_show_tree(ShowNode* root) {
    if (root == NULL) return;
    
    free_show_tree(root->left);
    free_show_tree(root->right);
    free(root);
}

// Case-insensitive strcmp portable
int ci_strcmp(const char* a, const char* b) {
#ifdef _WIN32
    return _stricmp(a, b);
#else
    // Fallback implementation
    while (*a && *b) {
        char ca = (char)tolower((unsigned char)*a);
        char cb = (char)tolower((unsigned char)*b);
        if (ca != cb) return (unsigned char)ca - (unsigned char)cb;
        a++; b++;
    }
    return (unsigned char)tolower((unsigned char)*a) - (unsigned char)tolower((unsigned char)*b);
#endif
}

// Parse seat code like A3 -> row=0, col=2; returns 0 on success
int parse_seat_code(const char* code, int* out_row, int* out_col) {
    if (!code || !out_row || !out_col) return -1;
    if (!isalpha((unsigned char)code[0])) return -1;
    int row = toupper((unsigned char)code[0]) - 'A';
    if (row < 0 || row >= NUM_ROWS) return -1;
    // parse number after letter
    int col = 0;
    for (int i = 1; code[i]; i++) {
        if (!isdigit((unsigned char)code[i])) return -1;
        col = col * 10 + (code[i] - '0');
    }
    if (col <= 0 || col > SEATS_PER_ROW) return -1;
    *out_row = row;
    *out_col = col - 1;
    return 0;
}

int seat_index_linear(int row, int col) {
    return row * SEATS_PER_ROW + (col + 1);
}

// Search shows by movie name (case-insensitive contains match)
static void search_by_name_traverse(ShowNode* node, const char* name, int* found) {
    if (!node) return;
    search_by_name_traverse(node->left, name, found);
    // case-insensitive substring search
    const char* title = node->show.name;
    // convert both to lowercase temp buffers
    char tbuf[MAX_NAME_LENGTH];
    char nbuf[MAX_NAME_LENGTH];
    strncpy(tbuf, title, MAX_NAME_LENGTH - 1); tbuf[MAX_NAME_LENGTH - 1] = '\0';
    strncpy(nbuf, name, MAX_NAME_LENGTH - 1); nbuf[MAX_NAME_LENGTH - 1] = '\0';
    for (char* p = tbuf; *p; ++p) *p = (char)tolower((unsigned char)*p);
    for (char* p = nbuf; *p; ++p) *p = (char)tolower((unsigned char)*p);
    if (strstr(tbuf, nbuf) != NULL) {
        if (!*found) {
            printf("\n=== SEARCH RESULTS ===\n");
            *found = 1;
        }
        printf("ID: %d | %-30s | %-15s | %s | Seats: %d/%d\n",
               node->show.id, node->show.name, node->show.genre, node->show.time,
               node->show.seats_available, node->show.total_seats);
    }
    search_by_name_traverse(node->right, name, found);
}

void search_shows_by_name(ShowNode* root, const char* name) {
    int found = 0;
    search_by_name_traverse(root, name, &found);
    if (!found) {
        printf("\nNo shows found matching: %s\n", name);
    }
}
