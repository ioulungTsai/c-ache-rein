#include <stdio.h>
#include <stdint.h>
#include <string.h>

// states
typedef enum {
    STATE_IDLE = 0,
    STATE_RECEIVING,
    STATE_VALIDATING,
    STATE_PROCESSING,
    STATE_ERROR,
    STATE_COUNT,
} state_t;

// events
typedef enum {
    EVENT_BYTE_RECEIVED = 0,
    EVENT_FRAME_COMPLETE,
    EVENT_CHECKSUM_OK,
    EVENT_CHECKSUM_FAIL,
    EVENT_PROCESS_DONE,
    EVENT_TIMEOUT,
    EVENT_RESET,
    EVENT_COUNT,
} event_t;

// frame definition
#define FRAME_MAX_LEN   16

typedef struct {
    uint8_t data[FRAME_MAX_LEN];
    uint8_t length;
    uint8_t checksum;
} frame_t;

// state machine context
typedef struct {
    state_t current;
    frame_t frame;
    uint32_t rx_count;
    uint32_t error_count;
    uint32_t processed_count;
} sm_ctx_t;

// action function type
typedef void (*action_fn)(sm_ctx_t *ctx, uint8_t data);

// transition entry
typedef struct {
    state_t next_state;
    action_fn action;
} transition_t;

// action declarations
void action_ignore(sm_ctx_t *ctx, uint8_t data);
void action_start_frame(sm_ctx_t *ctx, uint8_t data);
void action_store_byte(sm_ctx_t *ctx, uint8_t data);
void action_validate(sm_ctx_t *ctx, uint8_t data);
void action_process(sm_ctx_t *ctx, uint8_t data);
void action_complete(sm_ctx_t *ctx, uint8_t data);
void action_error(sm_ctx_t *ctx, uint8_t data);
void action_reset(sm_ctx_t *ctx, uint8_t data);

// tansition table
static const transition_t transitions[STATE_COUNT][EVENT_COUNT] = {
    [STATE_IDLE] = {
        [EVENT_BYTE_RECEIVED]   = {STATE_RECEIVING,     action_start_frame},
        [EVENT_FRAME_COMPLETE]  = {STATE_IDLE,          action_ignore},
        [EVENT_CHECKSUM_OK]     = {STATE_IDLE,          action_ignore},
        [EVENT_CHECKSUM_FAIL]   = {STATE_IDLE,          action_ignore},
        [EVENT_PROCESS_DONE]    = {STATE_IDLE,          action_ignore},
        [EVENT_TIMEOUT]         = {STATE_IDLE,          action_ignore},
        [EVENT_RESET]           = {STATE_IDLE,          action_reset},
    },
    [STATE_RECEIVING] = {
        [EVENT_BYTE_RECEIVED]   = {STATE_RECEIVING,     action_store_byte},
        [EVENT_FRAME_COMPLETE]  = {STATE_VALIDATING,    action_validate},
        [EVENT_CHECKSUM_OK]     = {STATE_RECEIVING,     action_ignore},
        [EVENT_CHECKSUM_FAIL]   = {STATE_RECEIVING,     action_ignore},
        [EVENT_PROCESS_DONE]    = {STATE_RECEIVING,     action_ignore},
        [EVENT_TIMEOUT]         = {STATE_ERROR,         action_error},
        [EVENT_RESET]           = {STATE_IDLE,          action_reset},
    },
    [STATE_VALIDATING] = {
        [EVENT_BYTE_RECEIVED]   = {STATE_VALIDATING,    action_ignore},
        [EVENT_FRAME_COMPLETE]  = {STATE_VALIDATING,    action_ignore},
        [EVENT_CHECKSUM_OK]     = {STATE_PROCESSING,    action_process},
        [EVENT_CHECKSUM_FAIL]   = {STATE_ERROR,         action_error},
        [EVENT_PROCESS_DONE]    = {STATE_VALIDATING,    action_ignore},
        [EVENT_TIMEOUT]         = {STATE_ERROR,         action_error},
        [EVENT_RESET]           = {STATE_IDLE,          action_reset},
    },
    [STATE_PROCESSING] = {
        [EVENT_BYTE_RECEIVED]   = {STATE_PROCESSING,    action_ignore},
        [EVENT_FRAME_COMPLETE]  = {STATE_PROCESSING,    action_ignore},
        [EVENT_CHECKSUM_OK]     = {STATE_PROCESSING,    action_ignore},
        [EVENT_CHECKSUM_FAIL]   = {STATE_PROCESSING,    action_ignore},
        [EVENT_PROCESS_DONE]    = {STATE_IDLE,          action_complete},
        [EVENT_TIMEOUT]         = {STATE_ERROR,         action_error},
        [EVENT_RESET]           = {STATE_IDLE,          action_reset},
    },
    [STATE_ERROR] = {
        [EVENT_BYTE_RECEIVED]   = {STATE_ERROR,         action_ignore},
        [EVENT_FRAME_COMPLETE]  = {STATE_ERROR,         action_ignore},
        [EVENT_CHECKSUM_OK]     = {STATE_ERROR,         action_ignore},
        [EVENT_CHECKSUM_FAIL]   = {STATE_ERROR,         action_ignore},
        [EVENT_PROCESS_DONE]    = {STATE_ERROR,         action_ignore},
        [EVENT_TIMEOUT]         = {STATE_ERROR,         action_ignore},
        [EVENT_RESET]           = {STATE_IDLE,          action_reset},
    },
};

// names for debug
static const char *state_names[] = {
    "IDLE", "RECEIVING", "VALIDATING", "PROCESSING", "ERROR"
};

static const char *event_names[] = {
    "BYTE_RX", "FRAME_COMPLETE", "CHKSUM_OK",
    "CHKSUM_FAIL", "PROCESS_DONE", "TIMEOUT", "RESET"
};

// engine
void sm_init(sm_ctx_t *ctx);
void sm_handle_event(sm_ctx_t *ctx, event_t event, uint8_t data);
void sm_print(sm_ctx_t *ctx);

int main(void)
{
    sm_ctx_t ctx;
    sm_init(&ctx);

    printf("=== normal frame flow ===\n");
    sm_handle_event(&ctx, EVENT_BYTE_RECEIVED,  0x01);
    sm_handle_event(&ctx, EVENT_BYTE_RECEIVED,  0x02);
    sm_handle_event(&ctx, EVENT_BYTE_RECEIVED,  0x03);
    sm_handle_event(&ctx, EVENT_FRAME_COMPLETE, 0x00);
    sm_handle_event(&ctx, EVENT_CHECKSUM_OK,    0x00);
    sm_handle_event(&ctx, EVENT_PROCESS_DONE,   0x00);
    sm_print(&ctx);

    printf("\n=== checksum fail flow ===\n");
    sm_handle_event(&ctx, EVENT_BYTE_RECEIVED,  0xAA);
    sm_handle_event(&ctx, EVENT_FRAME_COMPLETE, 0x00);
    sm_handle_event(&ctx, EVENT_FRAME_COMPLETE, 0x00);
    sm_handle_event(&ctx, EVENT_RESET,          0x00);
    sm_print(&ctx);

    printf("\n=== timeout flow ===\n");
    sm_handle_event(&ctx, EVENT_BYTE_RECEIVED,  0xBB);
    sm_handle_event(&ctx, EVENT_TIMEOUT,        0x00);
    sm_handle_event(&ctx, EVENT_RESET,          0x00);
    sm_print(&ctx);
    
    return 0;
}

void sm_init(sm_ctx_t *ctx)
{
    ctx->current         = STATE_IDLE;
    ctx->rx_count        = 0;
    ctx->error_count     = 0;
    ctx->processed_count = 0;
    memset(&ctx->frame, 0, sizeof(frame_t));
}

void sm_handle_event(sm_ctx_t *ctx, event_t event, uint8_t data)
{
    const transition_t *t = &transitions[ctx->current][event];
    printf("[%-11s] + %-14s -> [%s]\n",
            state_names[ctx->current],
            event_names[event],
            state_names[t->next_state]);
    t->action(ctx, data);
    ctx->current = t->next_state;
}

void sm_print(sm_ctx_t *ctx)
{
    printf("state=%s rx=%u errors=%u processed=%u\n",
            state_names[ctx->current],
            ctx->rx_count,
            ctx->error_count,
            ctx->processed_count);
}

void action_ignore(sm_ctx_t *ctx, uint8_t data)
{
    (void)ctx; (void)data;
}

void action_start_frame(sm_ctx_t *ctx, uint8_t data)
{
    memset(&ctx->frame, 0, sizeof(frame_t));
    ctx->frame.data[0] = data;
    ctx->frame.length = 1;
    ctx->frame.checksum = data;
    ctx->rx_count++;
    printf("    -> frame started, byte=0x%02X\n", data);
}

void action_store_byte(sm_ctx_t *ctx, uint8_t data)
{
    if (ctx->frame.length < FRAME_MAX_LEN) {
        ctx->frame.data[ctx->frame.length++] = data;
        ctx->frame.checksum ^= data;
    }
    ctx->rx_count++;
    printf("    -> stored 0x%02X, len=%u\n", data, ctx->frame.length);
}

void action_validate(sm_ctx_t *ctx, uint8_t data)
{
    (void)data;
    printf("    -> validating frame len=%u chksum=0x%02X\n",
            ctx->frame.length, ctx->frame.checksum);
}

void action_process(sm_ctx_t *ctx, uint8_t data)
{
    (void)data;
    printf("    -> processing %u bytes\n", ctx->frame.length);
}

void action_complete(sm_ctx_t *ctx, uint8_t data)
{
    (void)data;
    ctx->processed_count++;
    printf("    -> complete, total processed=%u\n", ctx->processed_count);
}

void action_error(sm_ctx_t *ctx, uint8_t data)
{
    (void)data;
    ctx->error_count++;
    printf("    -> error! total error=%u\n", ctx->error_count);
}

void action_reset(sm_ctx_t *ctx, uint8_t data)
{
    (void)data;
    memset(&ctx->frame, 0, sizeof(frame_t));
    printf("    -> reset\n");
}
