# Command Processing Flow - Event-Driven Approach

## State Machine

```
┌─────────────┐
│    IDLE     │ ◄─────────────────────┐
└─────────────┘                       │
       │                              │
       │ processNextCommand()         │
       ▼                              │
┌─────────────┐                       │
│  EXECUTING  │                       │
└─────────────┘                       │
       │                              │
       │ Send command to OpenOCD      │
       ▼                              │
┌──────────────────┐                  │
│ WAITING_RESPONSE │                  │
└──────────────────┘                  │
       │                              │
       │ onResponseReceived()         │
       └──────────────────────────────┘
```

## How It Works

### 1. GUI Enqueues Commands
```cpp
// User clicks "Connect" button
Command cmd;
cmd.type = CommandType::CONNECT;
cmd.args << "127.0.0.1" << "4444";
m_ocd_core->enqueueCommand(cmd);  // ← Queue it

// User immediately clicks another button
Command cmd2;
cmd2.type = CommandType::RESET_HALT;
m_ocd_core->enqueueCommand(cmd2);  // ← Also queued, will wait
```

### 2. Sequential Processing (No Race Conditions!)

```
Time  | State            | Queue            | Action
------|------------------|------------------|---------------------------
T0    | IDLE             | [CONNECT, RESET] | enqueueCommand(CONNECT)
T1    | EXECUTING        | [RESET]          | Dequeue CONNECT, send to OpenOCD
T2    | WAITING_RESPONSE | [RESET]          | Waiting for "OK" from server
T3    | WAITING_RESPONSE | [RESET]          | (still waiting...)
T4    | IDLE             | []               | Response arrives! Process RESET
T5    | EXECUTING        | []               | Dequeue RESET, send to OpenOCD
T6    | WAITING_RESPONSE | []               | Waiting for response...
T7    | IDLE             | []               | Response arrives! Queue empty.
```

### 3. Key Points

**Pain Point Solved:**
- ✅ If GUI calls `enqueueCommand()` while state is `WAITING_RESPONSE`, command is just added to queue
- ✅ It will NOT be processed until current command finishes
- ✅ No race conditions!

**Event-Driven (Not Polling):**
- ✅ No QTimer needed
- ✅ `onResponseReceived()` slot connected to telnet_client's signal
- ✅ When response arrives → process next command automatically

**Non-Blocking:**
- ✅ GUI remains responsive
- ✅ Commands processed one by one
- ✅ Each command waits for its response before next starts

## Integration with Telnet Client

```cpp
// In ocd_core constructor or setup method:
connect(telnet_client, &TelnetClient::responseReceived,
        this, &ocd_core::onResponseReceived);

// In processConnect():
m_state = CoreState::WAITING_RESPONSE;
telnet_client->sendCommand("connect " + ip + ":" + port);
// → Now we wait. onResponseReceived() will be called when data arrives
```

## Example Scenario

**User rapid-fires 5 commands:**
1. CONNECT
2. RESET_HALT
3. READ_MEMORY
4. WRITE_MEMORY
5. RESUME

**What happens:**
- All 5 get queued instantly (GUI doesn't block)
- CONNECT executes → waits for response → completes
- RESET_HALT executes → waits for response → completes
- READ_MEMORY executes → waits for response → completes
- WRITE_MEMORY executes → waits for response → completes
- RESUME executes → waits for response → completes

**Total time:** Sum of all command response times (sequential)
**GUI blocked:** Never! All processing happens via signals/slots in event loop
