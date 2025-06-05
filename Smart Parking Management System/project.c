#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define ORDER 4

int regular_count = 0;
int premium_count = 0;
int golden_count = 0;
const int MAX_REGULAR_SLOTS = 30;
const int MAX_PREMIUM_SLOTS = 10;
const int MAX_GOLDEN_SLOTS = 10;
// structure for vehicle information
typedef struct info_vehicle
{
    char name[50];
    char veh_num[20];
    char ent_time[10];
    char ent_date[12];
    char exi_time[10];
    char exi_date[12];
    int hours;
    char membership[20];
    int amount;
    int noOfParkings;
    int ID;
} vehicle;

// node structure for vehicle with b+ tree
struct vehicle_node;

typedef struct internal_vehicle
{
    char keys[ORDER][20];
    struct vehicle_node *children[ORDER + 1];
    int num_keys;
} internal_vehicle;

typedef struct leaf_vehicle
{
    char keys[ORDER][20];
    vehicle *data[ORDER];
    struct leaf_vehicle *next;
    int num_keys;
} leaf_vehicle;

// main structure for vehicle node to be used.
typedef struct vehicle_node
{
    bool is_leaf;
    union
    {
        internal_vehicle *internal_v;
        leaf_vehicle *leaf_v;
    };
} vehicle_node;

// structure for parking slot information
typedef struct parking_slot
{
    int slot_id;
    int revenue;
    int occupancy_count;
    bool occupied; // 0 for vacant, 1 for occupied
} parking_slot;
struct parking_node;

typedef struct internal_parking
{
    int keys[ORDER];
    struct parking_node *children[ORDER + 1];
    int num_keys;
} internal_parking;

typedef struct leaf_parking
{
    int keys[ORDER];
    parking_slot *data[ORDER];
    struct leaf_parking *next;
    int num_keys;
} leaf_parking;

// main structure for parking node to be used.
typedef struct parking_node
{
    bool is_leaf;
    union
    {
        internal_parking *internal_p;
        leaf_parking *leaf_p;
    };

} parking_node;

parking_node *create_parking_node(bool is_leaf)
{
    parking_node *node = (parking_node *)malloc(sizeof(parking_node));
    node->is_leaf = is_leaf;
    if (is_leaf)
    {
        node->leaf_p = (leaf_parking *)malloc(sizeof(leaf_parking));
        node->leaf_p->num_keys = 0;
        node->leaf_p->next = NULL;
    }
    else
    {
        node->internal_p = (internal_parking *)malloc(sizeof(internal_parking));
        node->internal_p->num_keys = 0;
        for (int i = 0; i <= ORDER; i++)
            node->internal_p->children[i] = NULL;
    }
    return node;
}

vehicle_node *create_vehicle_node(bool is_leaf)
{
    vehicle_node *node = (vehicle_node *)malloc(sizeof(vehicle_node));
    node->is_leaf = is_leaf;
    if (is_leaf)
    {
        node->leaf_v = (leaf_vehicle *)malloc(sizeof(leaf_vehicle));
        node->leaf_v->num_keys = 0;
        node->leaf_v->next = NULL;
    }
    else
    {
        node->internal_v = (internal_vehicle *)malloc(sizeof(internal_vehicle));
        node->internal_v->num_keys = 0;
        for (int i = 0; i <= ORDER; i++)
            node->internal_v->children[i] = NULL;
    }
    return node;
}

void search_vehicle(vehicle_node *node, char *key, vehicle **found_vehicle)
{
    if (node == NULL)
    {
        printf("Tree is empty.\n");
        return;
    }

    if (node->is_leaf)
    {
        for (int i = 0; i < node->leaf_v->num_keys; i++)
        {
            if (strcmp(node->leaf_v->keys[i], key) == 0)
            {
                printf("Vehicle found: %s\n", node->leaf_v->data[i]->veh_num);
                *found_vehicle = node->leaf_v->data[i];
                return;
            }
        }
    }
    else
    {
        for (int i = 0; i < node->internal_v->num_keys; i++)
        {
            if (strcmp(key, node->internal_v->keys[i]) < 0)
            {
                search_vehicle(node->internal_v->children[i], key, found_vehicle);
                return;
            }
        }
        search_vehicle(node->internal_v->children[node->internal_v->num_keys], key, found_vehicle);
    }
}

void search_parking(parking_node *node, int key, parking_slot **found_slot)
{
    if (node == NULL)
    {
        printf("Tree is empty.\n");
        return;
    }

    if (node->is_leaf)
    {
        for (int i = 0; i < node->leaf_p->num_keys; i++)
        {
            if (node->leaf_p->keys[i] == key)
            {
                // printf("Parking slot found: %d\n", node->leaf_p->data[i]->slot_id);
                *found_slot = node->leaf_p->data[i];
                return;
            }
        }
    }
    else
    {
        for (int i = 0; i < node->internal_p->num_keys; i++)
        {
            if (key < node->internal_p->keys[i])
            {
                search_parking(node->internal_p->children[i], key, found_slot);
                return;
            }
        }
        search_parking(node->internal_p->children[node->internal_p->num_keys], key, found_slot);
    }
}
// Splits a full leaf node and returns the new right leaf node.
// Also sets 'new_key' to the first key of the new right leaf node.
vehicle_node *split_leaf_vehicle(vehicle_node *node, char *new_key)
{
    int mid = (ORDER + 1) / 2;
    vehicle_node *new_node = create_vehicle_node(true);

    new_node->leaf_v->num_keys = ORDER - mid;
    // Move second half of keys and data to the new leaf node
    for (int i = mid, j = 0; i < ORDER; i++, j++)
    {
        strcpy(new_node->leaf_v->keys[j], node->leaf_v->keys[i]);
        new_node->leaf_v->data[j] = node->leaf_v->data[i];
    }
    node->leaf_v->num_keys = mid;// Update key count in original node

    // Update the linked list pointer of the leaf nodes
    new_node->leaf_v->next = node->leaf_v->next;
    node->leaf_v->next = (leaf_vehicle *)new_node->leaf_v;
    // Set the new key for promotion
    strcpy(new_key, new_node->leaf_v->keys[0]);
    return new_node;
}
// Splits a full internal node and returns the new right internal node.
// The middle key is passed back via 'new_key' to be inserted into the parent.
vehicle_node *split_internal_vehicle(vehicle_node *node, char *new_key)
{
    int mid = ORDER / 2;
    strcpy(new_key, node->internal_v->keys[mid]);// Promote the middle key

    vehicle_node *new_node = create_vehicle_node(false);
    new_node->internal_v->num_keys = ORDER - mid - 1;

    // Move keys and children to the new internal node

    for (int i = mid + 1, j = 0; i < ORDER; i++, j++)
    {
        strcpy(new_node->internal_v->keys[j], node->internal_v->keys[i]);
        new_node->internal_v->children[j] = node->internal_v->children[i];
    }
    // Move the last child pointer
    new_node->internal_v->children[new_node->internal_v->num_keys] = node->internal_v->children[ORDER];
    node->internal_v->num_keys = mid;
    return new_node;
}
// Recursively inserts a vehicle into the tree starting from an internal node.
// Handles splitting if needed and returns new split node via 'newChild'.

vehicle_node *insert_internal_vehicle(vehicle_node *node, vehicle *new_vehicle, char *new_key, vehicle_node **newChild)
{
    if (node->is_leaf)
    {
        int i = node->leaf_v->num_keys - 1;
        while (i >= 0 && strcmp(new_vehicle->veh_num, node->leaf_v->keys[i]) < 0)
        {
            strcpy(node->leaf_v->keys[i + 1], node->leaf_v->keys[i]);
            node->leaf_v->data[i + 1] = node->leaf_v->data[i];
            i--;
        }
        strcpy(node->leaf_v->keys[i + 1], new_vehicle->veh_num);
        node->leaf_v->data[i + 1] = new_vehicle;
        node->leaf_v->num_keys++;
        // Split if overflow
        if (node->leaf_v->num_keys == ORDER)
        {
            *newChild = split_leaf_vehicle(node, new_key);
        }
        return NULL;
    }
    // Find correct child index to recurse into
    int i = 0;
    while (i < node->internal_v->num_keys && strcmp(new_vehicle->veh_num, node->internal_v->keys[i]) > 0)
    {
        i++;
    }
    // Recurse into the selected child

    vehicle_node *child = node->internal_v->children[i];
    vehicle_node *new_split = NULL;
    char new_key_temp[20];

    insert_internal_vehicle(child, new_vehicle, new_key_temp, &new_split);

    if (new_split == NULL)
        return NULL;

         // Shift keys and children to make space for new key and split node

    for (int j = node->internal_v->num_keys; j > i; j--)
    {
        strcpy(node->internal_v->keys[j], node->internal_v->keys[j - 1]);
        node->internal_v->children[j + 1] = node->internal_v->children[j];
    }
// Insert new key and new child
    strcpy(node->internal_v->keys[i], new_key_temp);
    node->internal_v->children[i + 1] = new_split;
    node->internal_v->num_keys++;

    if (node->internal_v->num_keys == ORDER)
    {
        *newChild = split_internal_vehicle(node, new_key);
    }
    return NULL;
}

vehicle_node *insert_vehicle(vehicle_node *node, vehicle *new_vehicle)
{
    if (node == NULL)
    {
        node = create_vehicle_node(true);
        node->leaf_v->data[0] = new_vehicle;
        strcpy(node->leaf_v->keys[0], new_vehicle->veh_num);
        node->leaf_v->num_keys = 1;
        return node;
    }

    vehicle_node *newChild = NULL;
    char new_key[20];

    insert_internal_vehicle(node, new_vehicle, new_key, &newChild);
    // If split at root, create new root
    if (newChild != NULL)
    {
        vehicle_node *newRoot = create_vehicle_node(false);
        strcpy(newRoot->internal_v->keys[0], new_key);
        newRoot->internal_v->children[0] = node;
        newRoot->internal_v->children[1] = newChild;
        newRoot->internal_v->num_keys = 1;
        return newRoot;
    }
    return node;
}

parking_node *split_leaf_parking(parking_node *node, int *new_key)
{
    int mid = (ORDER + 1) / 2;
    parking_node *new_split = create_parking_node(true);

    new_split->leaf_p->num_keys = ORDER - mid;

    for (int i = mid, j = 0; i < ORDER; i++, j++)
    {
        new_split->leaf_p->keys[j] = node->leaf_p->keys[i];
        new_split->leaf_p->data[j] = node->leaf_p->data[i];
    }
    node->leaf_p->num_keys = mid;
    new_split->leaf_p->next = node->leaf_p->next;
    node->leaf_p->next = (leaf_parking *)new_split->leaf_p;
    *new_key = new_split->leaf_p->keys[0];
    return new_split;
}
parking_node *split_internal_parking(parking_node *node, int *new_key)
{
    int mid = ORDER / 2;
    *new_key = node->internal_p->keys[mid];

    parking_node *new_node = create_parking_node(false);
    new_node->internal_p->num_keys = ORDER - mid - 1;

    for (int i = 0, j = mid + 1; j < ORDER; i++, j++)
    {
        new_node->internal_p->keys[i] = node->internal_p->keys[j];
        new_node->internal_p->children[i] = node->internal_p->children[j];
    }
    new_node->internal_p->children[new_node->internal_p->num_keys] = node->internal_p->children[ORDER];

    node->internal_p->num_keys = mid;

    return new_node;
}
parking_node *insert_internal_parking(parking_node *node, parking_slot *new_slot, int *new_key, parking_node **newChild)
{
    if (node->is_leaf)
    {
        int i = node->leaf_p->num_keys - 1;
        while (i >= 0 && new_slot->slot_id < node->leaf_p->keys[i])
        {
            node->leaf_p->keys[i + 1] = node->leaf_p->keys[i];
            node->leaf_p->data[i + 1] = node->leaf_p->data[i];
            i--;
        }
        node->leaf_p->keys[i + 1] = new_slot->slot_id;
        node->leaf_p->data[i + 1] = new_slot;
        node->leaf_p->num_keys++;

        if (node->leaf_p->num_keys == ORDER)
        {
            *newChild = split_leaf_parking(node, new_key);
        }
        return NULL;
    }

    int i = 0;
    while (i < node->internal_p->num_keys && new_slot->slot_id > node->internal_p->keys[i])
    {
        i++;
    }

    parking_node *child = node->internal_p->children[i];
    parking_node *split_node = NULL;
    int temp_key;

    insert_internal_parking(child, new_slot, &temp_key, &split_node);

    if (split_node == NULL)
        return NULL;

    for (int j = node->internal_p->num_keys; j > i; j--)
    {
        node->internal_p->keys[j] = node->internal_p->keys[j - 1];
        node->internal_p->children[j + 1] = node->internal_p->children[j];
    }

    node->internal_p->keys[i] = temp_key;
    node->internal_p->children[i + 1] = split_node;
    node->internal_p->num_keys++;

    if (node->internal_p->num_keys == ORDER)
    {
        *newChild = split_internal_parking(node, new_key);
    }

    return NULL;
}

parking_node *insert_parking(parking_node *node, parking_slot *new_slot)
{
    if (node == NULL)
    {
        node = create_parking_node(true);
        node->leaf_p->data[0] = new_slot;
        node->leaf_p->keys[0] = new_slot->slot_id;
        node->leaf_p->num_keys = 1;
        return node;
    }

    parking_node *newChild = NULL;
    int new_key;

    insert_internal_parking(node, new_slot, &new_key, &newChild);
    if (newChild != NULL)
    {
        parking_node *newRoot = create_parking_node(false);
        newRoot->internal_p->keys[0] = new_key;
        newRoot->internal_p->children[0] = node;
        newRoot->internal_p->children[1] = newChild;
        newRoot->internal_p->num_keys = 1;
        return newRoot;
    }
    return node;
}

// Creating entry/ exit functions
vehicle *createVehicle()
{
    vehicle *new_vehicle = (vehicle *)malloc(sizeof(vehicle));
    if (new_vehicle == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    strcpy(new_vehicle->name, "");
    strcpy(new_vehicle->veh_num, "");
    strcpy(new_vehicle->ent_time, "");
    strcpy(new_vehicle->ent_date, "");
    strcpy(new_vehicle->exi_time, "");
    strcpy(new_vehicle->exi_date, "");
    strcpy(new_vehicle->membership, "regular");

    new_vehicle->hours = 0;
    new_vehicle->amount = 0;
    new_vehicle->noOfParkings = 0;
    new_vehicle->ID = -1;

    return new_vehicle;
}

parking_slot *createParkingSlot()
{
    parking_slot *new_slot = (parking_slot *)malloc(sizeof(parking_slot));
    if (new_slot == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    new_slot->slot_id = 0;
    new_slot->revenue = 0;
    new_slot->occupancy_count = 0;
    new_slot->occupied = false;

    return new_slot;
}

void assign_parking_slot(parking_node *root, vehicle *v)
{
    if (root == NULL)
    {
        printf("Empty parking B+ Tree.\n");
        return;
    }

    parking_node *current = root;
    while (!current->is_leaf)
    {
        current = current->internal_p->children[0];
    }

    leaf_parking *leaf = current->leaf_p;
    int assigned = 0;

    while (leaf != NULL && assigned == 0)
    {
        for (int i = 0; i < leaf->num_keys; i++)
        {
            parking_slot *slot = leaf->data[i];
            int is_valid_slot = 0;

            if (!slot->occupied)
            {

                if (strcmp(v->membership, "golden") == 0)
                {
                    if (slot->slot_id <= MAX_GOLDEN_SLOTS && golden_count < MAX_GOLDEN_SLOTS)
                    {
                        is_valid_slot = 1;
                        golden_count++;
                    }
                }
                else if (strcmp(v->membership, "premium") == 0)
                {
                    if (slot->slot_id > MAX_GOLDEN_SLOTS && slot->slot_id <= (MAX_GOLDEN_SLOTS + MAX_PREMIUM_SLOTS) && premium_count < MAX_PREMIUM_SLOTS)
                    {
                        is_valid_slot = 1;
                        premium_count++;
                    }
                }
                else if (strcmp(v->membership, "regular") == 0)
                {
                    if (slot->slot_id > (MAX_GOLDEN_SLOTS + MAX_PREMIUM_SLOTS) && regular_count < MAX_REGULAR_SLOTS)
                    {
                        is_valid_slot = 1;
                        regular_count++;
                    }
                }

                if (is_valid_slot)
                {
                    slot->occupied = true;
                    slot->occupancy_count++;
                    v->ID = slot->slot_id;
                    printf("Vehicle assigned to slot ID: %d\n", v->ID);
                    assigned = 1;
                }
            }

            if (assigned == 1)
            {
                i = leaf->num_keys;
            }
        }

        if (assigned == 0)
        {
            leaf = leaf->next;
        }
    }

    if (assigned == 0)
    {
        printf("No available parking slots for membership: %s\n", v->membership);
    }
}

void vehicle_entry(vehicle_node **vehicledb, parking_node **parkingdb)
{
    char s[20];
    printf("Enter vehicle number(ABCxxx): ");
    fgets(s, 20, stdin);
    s[strcspn(s, "\n")] = '\0';

    vehicle *existing_vehicle = NULL;
    search_vehicle(*vehicledb, s, &existing_vehicle);
    if (existing_vehicle != NULL && existing_vehicle->ID != -1)
    {
        printf("Vehicle already parked.\n");
        return;
    }

    int found = 0;
    if (existing_vehicle != NULL)
    {
        found = 1;
        printf("Vehicle already exists.\n");
    }
    else
    {
        if (regular_count >= MAX_REGULAR_SLOTS)
        {
            printf("No available parking slots for regular membership!\n");
            return;
        }
        vehicle *new_vehicle = createVehicle();
        if (new_vehicle == NULL)
        {
            printf("Memory allocation failed.\n");
            exit(1);
        }
        strcpy(new_vehicle->veh_num, s);
        printf("Enter vehicle name: ");
        fgets(new_vehicle->name, 50, stdin);
        new_vehicle->name[strcspn(new_vehicle->name, "\n")] = '\0';

        existing_vehicle = new_vehicle;
    }

    if (strcmp(existing_vehicle->membership, "golden") == 0 && golden_count >= MAX_GOLDEN_SLOTS)
    {
        printf("No available parking slots for golden membership!\n");
        return;
    }
    else if (strcmp(existing_vehicle->membership, "premium") == 0 && premium_count >= MAX_PREMIUM_SLOTS)
    {
        printf("No available parking slots for premium membership!\n");
        return;
    }
    printf("Enter entry time (HH:MM 24hr System): ");
    fgets(existing_vehicle->ent_time, 10, stdin);
    existing_vehicle->ent_time[strcspn(existing_vehicle->ent_time, "\n")] = '\0';

    printf("Enter entry date (YYYY-MM-DD): ");
    fgets(existing_vehicle->ent_date, 12, stdin);
    existing_vehicle->ent_date[strcspn(existing_vehicle->ent_date, "\n")] = '\0';

    strcpy(existing_vehicle->exi_time, existing_vehicle->ent_time);
    strcpy(existing_vehicle->exi_date, existing_vehicle->ent_date);
    existing_vehicle->noOfParkings++;

    assign_parking_slot(*parkingdb, existing_vehicle);
    if (!found)
        *vehicledb = insert_vehicle(*vehicledb, existing_vehicle);
}

void finddate(char dateStr[], int dateArr[])
{
    sscanf(dateStr, "%d-%d-%d", &dateArr[0], &dateArr[1], &dateArr[2]);
}

void findtime(char timeStr[], int timeArr[])
{
    sscanf(timeStr, "%d:%d", &timeArr[0], &timeArr[1]);
}

int intominutes(int year, int month, int day, int hour, int minute)
{
    return (((year * 365 + month * 30 + day) * 24 + hour) * 60) + minute;
}

int calctotalhours(vehicle *vehicle)
{
    if (vehicle == NULL)
        return 0;

    char entryDate[12], entryTime[10];
    char exitDate[12], exitTime[10];

    strcpy(entryDate, vehicle->ent_date);
    strcpy(entryTime, vehicle->ent_time);
    strcpy(exitDate, vehicle->exi_date);
    strcpy(exitTime, vehicle->exi_time);

    int entry[3], entryT[2];
    int exit[3], exitT[2];

    finddate(entryDate, entry);
    findtime(entryTime, entryT);
    finddate(exitDate, exit);
    findtime(exitTime, exitT);

    int totalentryMinutes = intominutes(entry[0], entry[1], entry[2], entryT[0], entryT[1]);
    int totalexitMinutes = intominutes(exit[0], exit[1], exit[2], exitT[0], exitT[1]);

    return (totalexitMinutes - totalentryMinutes) / 60;
}

int calc_pymnt(vehicle *vehicle, int hours)
{
    int parking_hours = hours;
    int charges;

    if (parking_hours <= 3)
        charges = 100;
    else
        charges = 100 + (parking_hours - 3) * 50.0;

    if ((strcmp(vehicle->membership, "golden") == 0) || (strcmp(vehicle->membership, "premium") == 0))
    {
        charges = charges * 0.9;
    }

    return charges;
}

void membership_upd(vehicle *v)
{
    if (v == NULL)
        return;

    int parking_hours = v->hours;

    if (parking_hours >= 200 && strcmp(v->membership, "golden") != 0)
    {
        strcpy(v->membership, "golden");
        printf("Membership upgraded to golden for vehicle %s\n", v->veh_num);
    }
    else if (parking_hours >= 100 &&
             strcmp(v->membership, "premium") != 0 &&
             strcmp(v->membership, "golden") != 0)
    {
        strcpy(v->membership, "premium");
        printf("Membership upgraded to premium for vehicle %s\n", v->veh_num);
    }
}

void exiting_vehicle(vehicle_node **vehicledb, parking_node **parkingbd)
{
    char s[20];
    printf("Enter vehicle number(ABCxxx): ");
    fgets(s, 20, stdin);
    s[strcspn(s, "\n")] = '\0';

    vehicle *existing_vehicle = NULL;
    search_vehicle(*vehicledb, s, &existing_vehicle);
    if (existing_vehicle == NULL)
    {
        printf("Vehicle not found in database!\n");
        return;
    }

    printf("Enter exit time (HH:MM 24hr System): ");
    fgets(existing_vehicle->exi_time, 10, stdin);
    existing_vehicle->exi_time[strcspn(existing_vehicle->exi_time, "\n")] = '\0';
    printf("Enter exit date (YYYY-MM-DD): ");
    fgets(existing_vehicle->exi_date, 12, stdin);
    existing_vehicle->exi_date[strcspn(existing_vehicle->exi_date, "\n")] = '\0';

    int id = existing_vehicle->ID;
    existing_vehicle->ID = -1;

    int hours = calctotalhours(existing_vehicle);
    existing_vehicle->hours += hours;

    
    int charge = calc_pymnt(existing_vehicle, hours);
    existing_vehicle->amount += charge;
    
    parking_slot *found_id = NULL;
    search_parking(*parkingbd, id, &found_id);
    
    if (found_id != NULL)
    {
        found_id->occupied = false;
        found_id->revenue += charge;
        
        if (strcmp(existing_vehicle->membership, "golden") == 0)
        {
            golden_count--;
        }
        else if (strcmp(existing_vehicle->membership, "premium") == 0)
        {
            premium_count--;
        }
        else
        {
            regular_count--;
        }
        printf("Vehicle exited successfully.\n");
    }
    else
    {
        printf("Parking slot not found.\n");
    }
    membership_upd(existing_vehicle);
}
// Sorting functions
//  Sort vehicles based on the number of parkings done
vehicle_node *split_leaf_vehicle_parking(vehicle_node *node, char *new_key)
{
    int mid = (ORDER + 1) / 2;
    vehicle_node *new_node = create_vehicle_node(true);

    new_node->leaf_v->num_keys = ORDER - mid;

    for (int i = mid, j = 0; i < ORDER; i++, j++)
    {
        strcpy(new_node->leaf_v->keys[j], node->leaf_v->keys[i]);
        new_node->leaf_v->data[j] = node->leaf_v->data[i];
    }

    node->leaf_v->num_keys = mid;
    new_node->leaf_v->next = node->leaf_v->next;
    node->leaf_v->next = (leaf_vehicle *)new_node->leaf_v;

    strcpy(new_key, new_node->leaf_v->keys[0]);
    return new_node;
}

vehicle_node *split_internal_vehicle_parking(vehicle_node *node, char *new_key)
{
    int mid = ORDER / 2;
    strcpy(new_key, node->internal_v->keys[mid]);

    vehicle_node *new_node = create_vehicle_node(false);
    new_node->internal_v->num_keys = ORDER - mid - 1;

    for (int i = mid + 1, j = 0; i < ORDER; i++, j++)
    {
        strcpy(new_node->internal_v->keys[j], node->internal_v->keys[i]);
        new_node->internal_v->children[j] = node->internal_v->children[i];
    }

    new_node->internal_v->children[new_node->internal_v->num_keys] = node->internal_v->children[ORDER];
    node->internal_v->num_keys = mid;

    return new_node;
}

vehicle_node *insert_internal_vehicle_parking(vehicle_node *node, vehicle *new_vehicle, char *new_key, vehicle_node **newChild)
{
    char new_key_string[20];
    sprintf(new_key_string, "%05d", new_vehicle->noOfParkings);

    if (node->is_leaf)
    {
        int i = node->leaf_v->num_keys - 1;
        while (i >= 0 && strcmp(new_key_string, node->leaf_v->keys[i]) < 0)
        {
            strcpy(node->leaf_v->keys[i + 1], node->leaf_v->keys[i]);
            node->leaf_v->data[i + 1] = node->leaf_v->data[i];
            i--;
        }
        strcpy(node->leaf_v->keys[i + 1], new_key_string);
        node->leaf_v->data[i + 1] = new_vehicle;
        node->leaf_v->num_keys++;

        if (node->leaf_v->num_keys == ORDER)
        {
            *newChild = split_leaf_vehicle_parking(node, new_key);
        }
        return NULL;
    }

    int i = 0;
    while (i < node->internal_v->num_keys && strcmp(new_key_string, node->internal_v->keys[i]) > 0)
    {
        i++;
    }

    vehicle_node *child = node->internal_v->children[i];
    vehicle_node *new_split = NULL;
    char new_key_temp[20];

    insert_internal_vehicle_parking(child, new_vehicle, new_key_temp, &new_split);

    if (new_split == NULL)
        return NULL;

    for (int j = node->internal_v->num_keys; j > i; j--)
    {
        strcpy(node->internal_v->keys[j], node->internal_v->keys[j - 1]);
        node->internal_v->children[j + 1] = node->internal_v->children[j];
    }

    strcpy(node->internal_v->keys[i], new_key_temp);
    node->internal_v->children[i + 1] = new_split;
    node->internal_v->num_keys++;

    if (node->internal_v->num_keys == ORDER)
    {
        *newChild = split_internal_vehicle_parking(node, new_key);
    }

    return NULL;
}

vehicle_node *insert_vehicle_by_parking(vehicle_node *node, vehicle *new_vehicle)
{
    if (node == NULL)
    {
        node = create_vehicle_node(true);
        node->leaf_v->data[0] = new_vehicle;
        sprintf(node->leaf_v->keys[0], "%05d", new_vehicle->noOfParkings);
        node->leaf_v->num_keys = 1;
        return node;
    }

    vehicle_node *newChild = NULL;
    char new_key[20];

    insert_internal_vehicle_parking(node, new_vehicle, new_key, &newChild);
    if (newChild != NULL)
    {
        vehicle_node *newRoot = create_vehicle_node(false);
        strcpy(newRoot->internal_v->keys[0], new_key);
        newRoot->internal_v->children[0] = node;
        newRoot->internal_v->children[1] = newChild;
        newRoot->internal_v->num_keys = 1;
        return newRoot;
    }
    return node;
}

vehicle_node *sort_vehicles_by_parkings(vehicle_node *root)
{
    if (root == NULL)
    {
        printf("Tree is empty.\n");
        return NULL;
    }

    vehicle_node *sorted_tree = NULL;

    while (!root->is_leaf)
    {
        root = root->internal_v->children[0];
    }

    leaf_vehicle *leaf = root->leaf_v;

    while (leaf != NULL)
    {
        for (int i = 0; i < leaf->num_keys; i++)
        {
            sorted_tree = insert_vehicle_by_parking(sorted_tree, leaf->data[i]);
        }
        leaf = leaf->next;
    }

    return sorted_tree;
}
// SORTING ON AMOUNT PAID
vehicle_node *split_leaf_vehicle_AMOUNT(vehicle_node *node, char *new_key)
{
    int mid = (ORDER + 1) / 2;
    vehicle_node *new_node = create_vehicle_node(true);

    new_node->leaf_v->num_keys = ORDER - mid;

    for (int i = mid, j = 0; i < ORDER; i++, j++)
    {
        strcpy(new_node->leaf_v->keys[j], node->leaf_v->keys[i]);
        new_node->leaf_v->data[j] = node->leaf_v->data[i];
    }

    node->leaf_v->num_keys = mid;
    new_node->leaf_v->next = node->leaf_v->next;
    node->leaf_v->next = (leaf_vehicle *)new_node->leaf_v;

    strcpy(new_key, new_node->leaf_v->keys[0]);
    return new_node;
}

vehicle_node *split_internal_vehicle_AMOUNT(vehicle_node *node, char *new_key)
{
    int mid = ORDER / 2;
    strcpy(new_key, node->internal_v->keys[mid]);

    vehicle_node *new_node = create_vehicle_node(false);
    new_node->internal_v->num_keys = ORDER - mid - 1;

    for (int i = mid + 1, j = 0; i < ORDER; i++, j++)
    {
        strcpy(new_node->internal_v->keys[j], node->internal_v->keys[i]);
        new_node->internal_v->children[j] = node->internal_v->children[i];
    }

    new_node->internal_v->children[new_node->internal_v->num_keys] = node->internal_v->children[ORDER];
    node->internal_v->num_keys = mid;

    return new_node;
}

vehicle_node *insert_internal_vehicle_AMOUNT(vehicle_node *node, vehicle *new_vehicle, char *new_key, vehicle_node **newChild)
{
    char new_key_string[20];
    sprintf(new_key_string, "%010d", new_vehicle->amount);

    if (node->is_leaf)
    {
        int i = node->leaf_v->num_keys - 1;
        while (i >= 0 && strcmp(new_key_string, node->leaf_v->keys[i]) < 0)
        {
            strcpy(node->leaf_v->keys[i + 1], node->leaf_v->keys[i]);
            node->leaf_v->data[i + 1] = node->leaf_v->data[i];
            i--;
        }
        strcpy(node->leaf_v->keys[i + 1], new_key_string);
        node->leaf_v->data[i + 1] = new_vehicle;
        node->leaf_v->num_keys++;

        if (node->leaf_v->num_keys == ORDER)
        {
            *newChild = split_leaf_vehicle_AMOUNT(node, new_key);
        }
        return NULL;
    }

    int i = 0;
    while (i < node->internal_v->num_keys && strcmp(new_key_string, node->internal_v->keys[i]) > 0)
    {
        i++;
    }

    vehicle_node *child = node->internal_v->children[i];
    vehicle_node *new_split = NULL;
    char new_key_temp[20];

    insert_internal_vehicle_AMOUNT(child, new_vehicle, new_key_temp, &new_split);

    if (new_split == NULL)
        return NULL;

    for (int j = node->internal_v->num_keys; j > i; j--)
    {
        strcpy(node->internal_v->keys[j], node->internal_v->keys[j - 1]);
        node->internal_v->children[j + 1] = node->internal_v->children[j];
    }

    strcpy(node->internal_v->keys[i], new_key_temp);
    node->internal_v->children[i + 1] = new_split;
    node->internal_v->num_keys++;

    if (node->internal_v->num_keys == ORDER)
    {
        *newChild = split_internal_vehicle_AMOUNT(node, new_key);
    }

    return NULL;
}

vehicle_node *insert_vehicle_by_AMOUNT(vehicle_node *node, vehicle *new_vehicle)
{
    if (node == NULL)
    {
        node = create_vehicle_node(true);
        node->leaf_v->data[0] = new_vehicle;
        sprintf(node->leaf_v->keys[0], "%010d", new_vehicle->amount);
        node->leaf_v->num_keys = 1;
        return node;
    }

    vehicle_node *newChild = NULL;
    char new_key[20];

    insert_internal_vehicle_AMOUNT(node, new_vehicle, new_key, &newChild);
    if (newChild != NULL)
    {
        vehicle_node *newRoot = create_vehicle_node(false);
        strcpy(newRoot->internal_v->keys[0], new_key);
        newRoot->internal_v->children[0] = node;
        newRoot->internal_v->children[1] = newChild;
        newRoot->internal_v->num_keys = 1;
        return newRoot;
    }
    return node;
}

vehicle_node *sort_vehicles_by_AMOUNT(vehicle_node *root)
{
    if (root == NULL)
    {
        printf("Tree is empty.\n");
        return NULL;
    }

    vehicle_node *sorted_tree = NULL;

    while (!root->is_leaf)
    {
        root = root->internal_v->children[0];
    }

    leaf_vehicle *leaf = root->leaf_v;

    while (leaf != NULL)
    {
        for (int i = 0; i < leaf->num_keys; i++)
        {
            sorted_tree = insert_vehicle_by_AMOUNT(sorted_tree, leaf->data[i]);
        }
        leaf = leaf->next;
    }

    return sorted_tree;
}

// SORTING ON PARKING SLOTS
// Sort parking slots based on occupancy
parking_node *split_leaf_parking_occupancy(parking_node *node, int *new_key)
{
    int mid = (ORDER + 1) / 2;
    parking_node *new_split = create_parking_node(true);

    new_split->leaf_p->num_keys = ORDER - mid;

    for (int i = mid, j = 0; i < ORDER; i++, j++)
    {
        new_split->leaf_p->keys[j] = node->leaf_p->keys[i];
        new_split->leaf_p->data[j] = node->leaf_p->data[i];
    }
    node->leaf_p->num_keys = mid;
    new_split->leaf_p->next = node->leaf_p->next;
    node->leaf_p->next = (leaf_parking *)new_split->leaf_p;
    *new_key = new_split->leaf_p->keys[0];
    return new_split;
}
parking_node *split_internal_parking_occupancy(parking_node *node, int *new_key)
{
    int mid = ORDER / 2;
    *new_key = node->internal_p->keys[mid];

    parking_node *new_node = create_parking_node(false);
    new_node->internal_p->num_keys = ORDER - mid - 1;

    for (int i = 0, j = mid + 1; j < ORDER; i++, j++)
    {
        new_node->internal_p->keys[i] = node->internal_p->keys[j];
        new_node->internal_p->children[i] = node->internal_p->children[j];
    }
    new_node->internal_p->children[new_node->internal_p->num_keys] = node->internal_p->children[ORDER];

    node->internal_p->num_keys = mid;

    return new_node;
}
parking_node *insert_internal_parking_occupancy(parking_node *node, parking_slot *new_slot, int *new_key, parking_node **newChild)
{
    if (node->is_leaf)
    {
        int i = node->leaf_p->num_keys - 1;
        while (i >= 0 && new_slot->occupancy_count < node->leaf_p->keys[i])
        {
            node->leaf_p->keys[i + 1] = node->leaf_p->keys[i];
            node->leaf_p->data[i + 1] = node->leaf_p->data[i];
            i--;
        }
        node->leaf_p->keys[i + 1] = new_slot->occupancy_count;
        node->leaf_p->data[i + 1] = new_slot;
        node->leaf_p->num_keys++;

        if (node->leaf_p->num_keys == ORDER)
        {
            *newChild = split_leaf_parking_occupancy(node, new_key);
        }
        return NULL;
    }

    int i = 0;
    while (i < node->internal_p->num_keys && new_slot->occupancy_count > node->internal_p->keys[i])
    {
        i++;
    }

    parking_node *child = node->internal_p->children[i];
    parking_node *split_node = NULL;
    int temp_key;

    insert_internal_parking_occupancy(child, new_slot, &temp_key, &split_node);

    if (split_node == NULL)
        return NULL;

    for (int j = node->internal_p->num_keys; j > i; j--)
    {
        node->internal_p->keys[j] = node->internal_p->keys[j - 1];
        node->internal_p->children[j + 1] = node->internal_p->children[j];
    }

    node->internal_p->keys[i] = temp_key;
    node->internal_p->children[i + 1] = split_node;
    node->internal_p->num_keys++;

    if (node->internal_p->num_keys == ORDER)
    {
        *newChild = split_internal_parking_occupancy(node, new_key);
    }

    return NULL;
}

parking_node *insert_parking_occupancy(parking_node *node, parking_slot *new_slot)
{
    if (node == NULL)
    {
        node = create_parking_node(true);
        node->leaf_p->data[0] = new_slot;
        node->leaf_p->keys[0] = new_slot->occupancy_count;
        node->leaf_p->num_keys = 1;
        return node;
    }

    parking_node *newChild = NULL;
    int new_key;

    insert_internal_parking_occupancy(node, new_slot, &new_key, &newChild);
    if (newChild != NULL)
    {
        parking_node *newRoot = create_parking_node(false);
        newRoot->internal_p->keys[0] = new_key;
        newRoot->internal_p->children[0] = node;
        newRoot->internal_p->children[1] = newChild;
        newRoot->internal_p->num_keys = 1;
        return newRoot;
    }
    return node;
}

parking_node *sort_parking_by_occupancy(parking_node *root)
{
    if (root == NULL)
    {
        printf("Tree is empty.\n");
        return NULL;
    }

    parking_node *sorted_tree = NULL;

    while (!root->is_leaf)
    {
        root = root->internal_p->children[0];
    }

    leaf_parking *leaf = root->leaf_p;

    while (leaf != NULL)
    {
        for (int i = 0; i < leaf->num_keys; i++)
        {
            sorted_tree = insert_parking_occupancy(sorted_tree, leaf->data[i]);
        }
        leaf = leaf->next;
    }

    return sorted_tree;
}
// sorting on revenue
parking_node *split_leaf_parking_revenue(parking_node *node, int *new_key)
{
    int mid = (ORDER + 1) / 2;
    parking_node *new_split = create_parking_node(true);

    new_split->leaf_p->num_keys = ORDER - mid;

    for (int i = mid, j = 0; i < ORDER; i++, j++)
    {
        new_split->leaf_p->keys[j] = node->leaf_p->keys[i];
        new_split->leaf_p->data[j] = node->leaf_p->data[i];
    }
    node->leaf_p->num_keys = mid;
    new_split->leaf_p->next = node->leaf_p->next;
    node->leaf_p->next = (leaf_parking *)new_split->leaf_p;
    *new_key = new_split->leaf_p->keys[0];
    return new_split;
}
parking_node *split_internal_parking_revenue(parking_node *node, int *new_key)
{
    int mid = ORDER / 2;
    *new_key = node->internal_p->keys[mid];

    parking_node *new_node = create_parking_node(false);
    new_node->internal_p->num_keys = ORDER - mid - 1;

    for (int i = 0, j = mid + 1; j < ORDER; i++, j++)
    {
        new_node->internal_p->keys[i] = node->internal_p->keys[j];
        new_node->internal_p->children[i] = node->internal_p->children[j];
    }
    new_node->internal_p->children[new_node->internal_p->num_keys] = node->internal_p->children[ORDER];

    node->internal_p->num_keys = mid;

    return new_node;
}
parking_node *insert_internal_parking_revenue(parking_node *node, parking_slot *new_slot, int *new_key, parking_node **newChild)
{
    if (node->is_leaf)
    {
        int i = node->leaf_p->num_keys - 1;
        while (i >= 0 && new_slot->revenue < node->leaf_p->keys[i])
        {
            node->leaf_p->keys[i + 1] = node->leaf_p->keys[i];
            node->leaf_p->data[i + 1] = node->leaf_p->data[i];
            i--;
        }
        node->leaf_p->keys[i + 1] = new_slot->revenue;
        node->leaf_p->data[i + 1] = new_slot;
        node->leaf_p->num_keys++;

        if (node->leaf_p->num_keys == ORDER)
        {
            *newChild = split_leaf_parking_revenue(node, new_key);
        }
        return NULL;
    }

    int i = 0;
    while (i < node->internal_p->num_keys && new_slot->revenue > node->internal_p->keys[i])
    {
        i++;
    }

    parking_node *child = node->internal_p->children[i];
    parking_node *split_node = NULL;
    int temp_key;

    insert_internal_parking_revenue(child, new_slot, &temp_key, &split_node);

    if (split_node == NULL)
        return NULL;

    for (int j = node->internal_p->num_keys; j > i; j--)
    {
        node->internal_p->keys[j] = node->internal_p->keys[j - 1];
        node->internal_p->children[j + 1] = node->internal_p->children[j];
    }

    node->internal_p->keys[i] = temp_key;
    node->internal_p->children[i + 1] = split_node;
    node->internal_p->num_keys++;

    if (node->internal_p->num_keys == ORDER)
    {
        *newChild = split_internal_parking_revenue(node, new_key);
    }

    return NULL;
}

parking_node *insert_parking_revenue(parking_node *node, parking_slot *new_slot)
{
    if (node == NULL)
    {
        node = create_parking_node(true);
        node->leaf_p->data[0] = new_slot;
        node->leaf_p->keys[0] = new_slot->revenue;
        node->leaf_p->num_keys = 1;
        return node;
    }

    parking_node *newChild = NULL;
    int new_key;

    insert_internal_parking_revenue(node, new_slot, &new_key, &newChild);
    if (newChild != NULL)
    {
        parking_node *newRoot = create_parking_node(false);
        newRoot->internal_p->keys[0] = new_key;
        newRoot->internal_p->children[0] = node;
        newRoot->internal_p->children[1] = newChild;
        newRoot->internal_p->num_keys = 1;
        return newRoot;
    }
    return node;
}

parking_node *sort_parking_by_revenue(parking_node *root)
{
    if (root == NULL)
    {
        printf("Tree is empty.\n");
        return NULL;
    }

    parking_node *sorted_tree = NULL;

    while (!root->is_leaf)
    {
        root = root->internal_p->children[0];
    }

    leaf_parking *leaf = root->leaf_p;

    while (leaf != NULL)
    {
        for (int i = 0; i < leaf->num_keys; i++)
        {
            sorted_tree = insert_parking_revenue(sorted_tree, leaf->data[i]);
        }
        leaf = leaf->next;
    }

    return sorted_tree;
}

// FILE HANDLING
void load_vehicles_from_file(vehicle_node **root)
{
    FILE *fp = fopen("vehicledatabase.txt", "r");
    if (fp == NULL)
    {
        printf("Failed to open vehicle database.\n");
        return;
    }

    char line[256];

    while (fgets(line, sizeof(line), fp))
    {
        line[strcspn(line, "\n")] = 0;

        vehicle *v = (vehicle *)malloc(sizeof(vehicle));
        if (!v)
        {
            printf("Memory allocation failed.\n");
            continue;
        }

        int fields_read = sscanf(line, "%49s %19s %9s %11s %9s %11s %d %19s %d %d %d",
                                 v->name,
                                 v->veh_num,
                                 v->ent_time,
                                 v->ent_date,
                                 v->exi_time,
                                 v->exi_date,
                                 &v->hours,
                                 v->membership,
                                 &v->amount,
                                 &v->noOfParkings,
                                 &v->ID);

        if (fields_read != 11)
        {
            // printf("Skipping malformed line: %s\n", line);
            free(v);
            continue;
        }

        *root = insert_vehicle(*root, v);
        if (v->ID != -1)
        {

            if (strcmp(v->membership, "golden") == 0)
            {
                golden_count++;
            }
            else if (strcmp(v->membership, "premium") == 0)
            {
                premium_count++;
            }
            else
            {
                regular_count++;
            }
        }

        // printf("Parsed: %s %s %s %s %s %s %d %s %d %d %d\n",
        //     v->name, v->veh_num, v->ent_time, v->ent_date,
        //     v->exi_time, v->exi_date, v->hours,
        //     v->membership, v->amount, v->noOfParkings, v->ID);
    }

    fclose(fp);
}

void save_all_vehicles_to_file(vehicle_node *root, char *filename)
{
    if (root == NULL)
        return;

    while (!root->is_leaf)
    {
        root = root->internal_v->children[0];
    }

    leaf_vehicle *leaf = root->leaf_v;

    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("Failed to open vehicle database file.\n");
        return;
    }

    // Write header
    fprintf(fp, "%-10s %-12s %-10s %-12s %-10s %-12s %-5s %-10s %-8s %-14s %-4s\n",
            "name", "vehicle_no", "entry_time", "entry_date",
            "exit_time", "exit_date", "hours", "membership",
            "amount", "noOfParkings", "ID");

    // Write data
    while (leaf != NULL)
    {
        for (int i = 0; i < leaf->num_keys; i++)
        {
            vehicle *v = leaf->data[i];
            fprintf(fp, "%-10s %-12s %-10s %-12s %-10s %-12s %-5d %-10s %-8d %-14d %-4d\n",
                    v->name,
                    v->veh_num,
                    v->ent_time,
                    v->ent_date,
                    v->exi_time,
                    v->exi_date,
                    v->hours,
                    v->membership,
                    v->amount,
                    v->noOfParkings,
                    v->ID);
        }
        leaf = leaf->next;
    }

    fclose(fp);
    printf("Vehicle data saved in tabular format.\n");
}

void load_parking_slots_from_file(parking_node **root)
{
    FILE *fp = fopen("parkingdatabase.txt", "r");
    if (fp == NULL)
    {
        printf("No existing parking slot data found.\n");
        return;
    }

    // Skip the header line
    char header[256];
    fgets(header, sizeof(header), fp);

    parking_slot slot;
    while (fscanf(fp, "%d %d %d %d",
                  &slot.slot_id,
                  &slot.revenue,
                  &slot.occupancy_count,
                  (int *)&slot.occupied) == 4)
    {
        parking_slot *found_slot = NULL;
        search_parking(*root, slot.slot_id, &found_slot);

        if (found_slot != NULL)
        {
            found_slot->revenue = slot.revenue;
            found_slot->occupancy_count = slot.occupancy_count;
            found_slot->occupied = slot.occupied;
        }
    }

    fclose(fp);
}

void save_parking_slots_to_file(parking_node *root, char *filename)
{
    if (root == NULL)
        return;

    // Go to the leftmost leaf
    while (!root->is_leaf)
    {
        root = root->internal_p->children[0];
    }

    leaf_parking *leaf = root->leaf_p;

    FILE *fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("Failed to open parking slot file.\n");
        return;
    }

    // Write header
    fprintf(fp, "%-8s %-8s %-10s %-8s\n", "slot_id", "revenue", "occupancy", "occupied");

    // Write each occupied slot
    while (leaf != NULL)
    {
        for (int i = 0; i < leaf->num_keys; i++)
        {
            parking_slot *slot = leaf->data[i];
            if (slot->occupancy_count > 0)
            {
                fprintf(fp, "%-8d %-8d %-10d %-8d\n",
                        slot->slot_id,
                        slot->revenue,
                        slot->occupancy_count,
                        slot->occupied);
            }
        }
        leaf = leaf->next;
    }

    fclose(fp);
}

void print_all_vehicles(vehicle_node *root)
{
    if (root == NULL)
    {
        printf("Vehicle B+ Tree is empty.\n");
        return;
    }

    while (!root->is_leaf)
    {
        root = root->internal_v->children[0];
    }

    leaf_vehicle *leaf = root->leaf_v;

    printf("\n---------------- Vehicle Details ----------------\n");
    printf(" %-10s | %-10s | %-10s | %-10s | %-5s | %-10s | %-6s | %-10s | %-10s\n",
           "Veh Num", "Name", "Entry Date", "Exit Date", "Hrs", "Membership", "Amt", "NoParkings", "SlotID");
    printf("-----------------------------------------------------------------------------------------------------------\n");

    while (leaf != NULL)
    {
        for (int i = 0; i < leaf->num_keys; i++)
        {
            vehicle *v = leaf->data[i];
            printf(" %-10s | %-10s | %-10s | %-10s | %-5d | %-10s | %-6d | %-10d | %-10d\n",
                   v->veh_num,
                   v->name,
                   v->ent_date,
                   v->exi_date,
                   v->hours,
                   v->membership,
                   v->amount,
                   v->noOfParkings,
                   v->ID);
        }
        leaf = leaf->next;
    }

    printf("-----------------------------------------------------------------------------------------------------------\n\n");
}


void print_parking_slots(parking_node *root)
{
    if (root == NULL)
    {
        printf("Parking B+ Tree is empty.\n");
        return;
    }

    while (!root->is_leaf)
    {
        root = root->internal_p->children[0];
    }

    leaf_parking *leaf = root->leaf_p;

    printf("\n---------- Parking Slot Details ----------\n");
    printf(" %-10s | %-8s | %-16s | %-9s\n",
           "Slot ID", "Revenue", "Occupancy Count", "Occupied");
    printf("----------------------------------------------------------\n");

    while (leaf != NULL)
    {
        for (int i = 0; i < leaf->num_keys; i++)
        {
            parking_slot *slot = leaf->data[i];
            if (slot->occupancy_count > 0)
            {
                printf(" %-10d | %-8d | %-16d | %-9s\n",
                       slot->slot_id,
                       slot->revenue,
                       slot->occupancy_count,
                       slot->occupied ? "Yes" : "No");
            }
        }
        leaf = leaf->next;
    }

    printf("----------------------------------------------------------\n\n");
}

parking_slot *create_parking_slot(int slot_id, bool occupied)
{
    parking_slot *new_slot = (parking_slot *)malloc(sizeof(parking_slot));
    if (new_slot == NULL)
    {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    new_slot->slot_id = slot_id;
    new_slot->revenue = 0;
    new_slot->occupancy_count = 0;
    new_slot->occupied = occupied;

    return new_slot;
}

void free_vehicle_tree(vehicle_node* node) {
    if (node == NULL) return;

    if (node->is_leaf) {
        leaf_vehicle* leaf = node->leaf_v;

        for (int i = 0; i < leaf->num_keys; i++) {
            if (leaf->data[i]) {
                free(leaf->data[i]);
            }
        }

        free(leaf);     
        free(node);    

    } else {
        internal_vehicle* internal = node->internal_v;

        for (int i = 0; i <= internal->num_keys; i++) {
            free_vehicle_tree(internal->children[i]);
        }

        free(internal); 
        free(node);     
    }
}

void free_parking_tree(parking_node* node) {
    if (node == NULL) return;

    if (node->is_leaf) {
        leaf_parking* leaf = node->leaf_p;

        for (int i = 0; i < leaf->num_keys; i++) {
            if (leaf->data[i]) {
                free(leaf->data[i]);
            }
        }

        free(leaf);   
        free(node);  

    } else {
        internal_parking* internal = node->internal_p;

        for (int i = 0; i <= internal->num_keys; i++) {
            free_parking_tree(internal->children[i]);
        }

        free(internal); 
        free(node);     
    }
}

int main()
{
    vehicle_node *vehicleDataBase = NULL;
    parking_node *parkingSlot = NULL;
    for (int i = 1; i <= 50; i++)
    {
        parking_slot *new_slot = create_parking_slot(i, false);
        parkingSlot = insert_parking(parkingSlot, new_slot);
    }
    load_vehicles_from_file(&vehicleDataBase);
    printf("Vehicle database loaded successfully.\n");
    printf("%d", regular_count);
    printf("%d", premium_count);
    printf("%d", golden_count);
    load_parking_slots_from_file(&parkingSlot);

    int flag = 1;
    while (flag)
    {
        int choice;
        printf("DIGITAL PARKING LOT SYSTEM\n");
        printf("enter your choice from given below\n");
        printf("1- vehicle registration\n");
        printf("2- vehicle exit\n");
        printf("3- Sort the list of vehicles based on number of parkings done.\n");
        printf("4- sorted list of vehicles based on paid amount \n");
        printf("5- Sort the list of parking spaces based on their occupancy\n");
        printf("6- Sort the list of parking spaces which generated maximum revenue. \n");
        printf("7- Save all vehicles to file\n");
        printf("8- Save parking slots to file\n");
        printf("9- Print all vehicles\n");
        printf("10- Print all parking slots\n");
        printf("0- choose Any other number to Quit the process\n");
        scanf("%d", &choice);
        getchar();
        switch (choice)
        {
        case 1:
            printf("Initialising vehicle entry\n");
            vehicle_entry(&vehicleDataBase, &parkingSlot);

            break;
        case 2:
            printf("Initialising vehicle exit\n");
            exiting_vehicle(&vehicleDataBase, &parkingSlot);
            break;
        case 3:
            printf("Initialising vehicle list sorting according to no. of parking.\n");
            vehicle_node *sorted_vehicles = sort_vehicles_by_parkings(vehicleDataBase);
            save_all_vehicles_to_file(sorted_vehicles, "sorted_by_parkings.txt");
            print_all_vehicles(sorted_vehicles);
            
            free(sorted_vehicles);
            break;
        case 4:
            printf("Initialising vehicle list sorting according to paid amount.\n");
            vehicle_node *sorted_amount = sort_vehicles_by_AMOUNT(vehicleDataBase);
            print_all_vehicles(sorted_amount);
            save_all_vehicles_to_file(sorted_amount, "sorted_by_amount.txt");
            
            free(sorted_amount);
            break;
        case 5:
            printf("Initialising parking slot sorting according to occupancy.\n");
            parking_node *sorted_occupancy = sort_parking_by_occupancy(parkingSlot);
            save_parking_slots_to_file(sorted_occupancy, "sorted_by_occupancy.txt");
            print_parking_slots(sorted_occupancy);
            
            free(sorted_occupancy);
            break;
        case 6:
            printf("Initialising parking slot sorting according to revenue.\n");
            parking_node *sorted_revenue = sort_parking_by_revenue(parkingSlot);
            save_parking_slots_to_file(sorted_revenue, "sorted_by_revenue.txt");
            print_parking_slots(sorted_revenue);
            
            free(sorted_revenue);
            break;
        case 7:
            save_all_vehicles_to_file(vehicleDataBase, "vehicledatabase.txt");
            printf("All vehicles saved to file.\n");
            break;
        case 8:
            save_parking_slots_to_file(parkingSlot, "parkingdatabase.txt");
            printf("Parking slot saved to file.\n");
            break;
        case 9:
            print_all_vehicles(vehicleDataBase);
            break;
        case 10:
            print_parking_slots(parkingSlot);
            break;
        default:
            flag = 0;
            break;
        }
    }
    print_all_vehicles(vehicleDataBase);
    save_all_vehicles_to_file(vehicleDataBase, "vehicledatabase.txt");
    printf("All vehicles saved to file.\n");
    print_parking_slots(parkingSlot);
    save_parking_slots_to_file(parkingSlot, "parkingdatabase.txt");
    printf("Parking slot saved to file.\n");
    free_vehicle_tree(vehicleDataBase);
    free_parking_tree(parkingSlot);
    return 0;
}
