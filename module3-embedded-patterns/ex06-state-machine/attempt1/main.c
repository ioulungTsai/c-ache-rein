#include <stdio.h>
#include <stdint.h>

// states
typedef enum {
    STATE_IDLE = 0,
    STATE_SENSING,
    STATE_PROCESSING,
    STATE_ERROR,
    STATE_COUNT     // always last - total number of states
} state_t;

// events
typedef enum {
    EVENT_START = 0,
    EVENT_DATA_READY,
    EVENT_PROCESS_DONE,
    EVENT_ERROR,
    EVENT_RESET,
    EVENT_COUNT     // always last
} event_t;

// state machine context
typedef struct {
    state_t current;
    uint32_t sensor_value;
    uint32_t error_count;
} sm_ctx_t;

// action function pointer type
typedef void (*action_fn)(sm_ctx_t *ctx);

// transition table entry
typedef struct {
    state_t next_state;
    action_fn action;
} transition_t;

// action function declarations
void action_start_sensing(sm_ctx_t *ctx);
void action_process_data(sm_ctx_t *ctx);
void action_complete(sm_ctx_t *ctx);
void action_handle_error(sm_ctx_t *ctx);
void action_reset(sm_ctx_t *ctx);
void action_ignore(sm_ctx_t *ctx);

// transition table [current_state][event]
static const transition_t transitions[STATE_COUNT][EVENT_COUNT] = {
    // STATE_IDLE
    [STATE_IDLE] = {
        [EVENT_START]           = {STATE_SENSING, action_start_sensing},
        [EVENT_DATA_READY]      = {STATE_IDLE, action_ignore},
        [EVENT_PROCESS_DONE]    = {STATE_IDLE, action_ignore},
        [EVENT_ERROR]           = {STATE_ERROR, action_handle_error},
        [EVENT_RESET]           = {STATE_IDLE, action_reset},
    },
    // STATE_SENSING
    [STATE_SENSING] = {
        [EVENT_START]           = {STATE_SENSING, action_ignore},
        [EVENT_DATA_READY]      = {STATE_PROCESSING, action_process_data},
        [EVENT_PROCESS_DONE]    = {STATE_SENSING, action_ignore},
        [EVENT_ERROR]           = {STATE_ERROR, action_handle_error},
        [EVENT_RESET]           = {STATE_IDLE, action_reset},
    },
    // STATE_PROCESSING
    [STATE_PROCESSING] = {
        [EVENT_START]           = {STATE_PROCESSING, action_ignore},
        [EVENT_DATA_READY]      = {STATE_PROCESSING, action_ignore},
        [EVENT_PROCESS_DONE]    = {STATE_IDLE, action_complete},
        [EVENT_ERROR]           = {STATE_ERROR, action_handle_error},
        [EVENT_RESET]           = {STATE_IDLE, action_reset},
    },
    // STATE_ERROR
    [STATE_ERROR] = {
        [EVENT_START]           = {STATE_ERROR, action_ignore},
        [EVENT_DATA_READY]      = {STATE_ERROR, action_ignore},
        [EVENT_PROCESS_DONE]    = {STATE_ERROR, action_ignore},
        [EVENT_ERROR]           = {STATE_ERROR, action_handle_error},
        [EVENT_RESET]           = {STATE_IDLE, action_reset},
    },
};

// state names for prointing
static const char *state_names[] = {
    "IDLE", "SENSING", "PROCESSING", "ERROR"
};

static const char *event_names[] = {
    "START", "DATA_READY", "PROCESS_DONE", "ERROR", "RESET"
};

// state machine functions
void sm_init(sm_ctx_t *ctx);
void sm_handle_event(sm_ctx_t *ctx, event_t event);
void sm_print(sm_ctx_t *ctx);


int main(void)
{
    sm_ctx_t ctx;
    sm_init(&ctx);

    printf("=== sensor state machine ===\n\n");
    sm_print(&ctx);

    // normal flow
    sm_handle_event(&ctx, EVENT_START);
    sm_handle_event(&ctx, EVENT_DATA_READY);
    sm_handle_event(&ctx, EVENT_PROCESS_DONE);

    // error flow
    printf("\n=== error flow ===\n\n");
    sm_handle_event(&ctx, EVENT_START);
    sm_handle_event(&ctx, EVENT_ERROR);
    sm_handle_event(&ctx, EVENT_RESET);

    // ignore events
    printf("\n=== ignored events ===\n\n");
    sm_handle_event(&ctx, EVENT_DATA_READY);
    sm_handle_event(&ctx, EVENT_PROCESS_DONE);
    
    return 0;
}

void sm_init(sm_ctx_t *ctx)
{
    ctx->current = STATE_IDLE;
    ctx->sensor_value = 0;
    ctx->error_count = 0;
}

void sm_handle_event(sm_ctx_t *ctx, event_t event)
{
    const transition_t *t = &transitions[ctx->current][event];

    printf("[%s] + %s -> [%s]\n",
            state_names[ctx->current],
            event_names[event],
            state_names[t->next_state]);

    t->action(ctx);
    ctx->current = t->next_state;
}

void sm_print(sm_ctx_t *ctx)
{
    printf("state=%s    sensor=%u   errors=%u\n",
            state_names[ctx->current],
            ctx->sensor_value,
            ctx->error_count);
}

void action_start_sensing(sm_ctx_t *ctx)
{
    printf("    -> starting sensor read\n");
    ctx->sensor_value = 0;
}

void action_process_data(sm_ctx_t *ctx)
{
    ctx->sensor_value = 2650;   // simulate ADC reading
    printf("    -> processing sensor value: %u\n", ctx->sensor_value);
}

void action_complete(sm_ctx_t *ctx)
{
    printf("    -> complete. final value: %u\n", ctx->sensor_value);
}

void action_handle_error(sm_ctx_t *ctx)
{
    ctx->error_count++;
    printf("    -> error! count=%u\n", ctx->error_count);
}

void action_reset(sm_ctx_t *ctx)
{
    printf("    -> resetting\n");
    ctx->sensor_value = 0;
    ctx->error_count = 0;
}

void action_ignore(sm_ctx_t *ctx)
{
    (void)ctx;
    printf("    -> event ignore in this state\n");
}
