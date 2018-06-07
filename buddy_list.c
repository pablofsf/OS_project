typedef struct buddylist_t{
struct buddylist_t *prev,*next;
} buddylist_t;

static buddylist_t freelist[10];
static size_t node_limit;
static uint8_t split_node[(1 << 9)/8];
static uint8_t *min_ptr;
static uint8_t *max_ptr;

static void init(buddylist_t *list){
  list->prev = list;
  list->next = list;
}

static void push(buddylist_t *list, buddylist_t *node){
buddylist_t *prev = list->prev;
node->prev = prev;
node->next = list;;
prev->next = node;
list->prev = node;
}

static void remove(buddylist_t *node){
buddylist_t *prev = node->prev;
buddylist_t *next = node-> next;
prev->next = next;
next->prev = prev;
}

static
