*This project has been created as a part of the 42 curriculum by etchipoq.*

<div align="center">

# 🧵 Codexion

### *Where coders fight over dongles, and only good synchronization saves the sprint.*

<img src="https://img.shields.io/badge/Language-C-blue.svg" alt="Language">
<img src="https://img.shields.io/badge/Threads-POSIX-green.svg" alt="Threads">
<img src="https://img.shields.io/badge/Synchronization-Mutexes%20%26%20Condition%20Variables-orange.svg" alt="Synchronization">
<img src="https://img.shields.io/badge/Status-Completed-success.svg" alt="Status">


</div>

---

## 📖 What is this?

Picture an open-plan office where every developer needs a **dongle** to compile — and there are never quite enough to go around. That's Codexion: a multithreaded C simulation where a crew of "coders" fight (politely, via mutexes) over shared dongles to get their work done.

Each coder loops through the daily grind:

```
🔧 Acquire dongles  →  💻 Compile  →  🐞 Debug  →  🛠️ Refactor  →  🔁 Repeat
```

Simple on paper. In practice, it's a minefield of **deadlocks**, **race conditions**, **starvation**, and other classic concurrency nightmares — which is exactly the point. Codexion is a playground for solving the Dining Philosophers problem in disguise, dressed up in a dev-team costume.

---

## ✨ Feature Board

| Category | What's inside |
|---|---|
| 🧵 Threading | POSIX Threads (pthreads) |
| 🔒 Safety | Mutex-protected shared resources |
| 📡 Signaling | Condition-variable based scheduling (no busy-waiting) |
| 📋 Schedulers | FIFO & EDF (Earliest Deadline First) |
| 🔥 Health | Burnout detection + configurable cooldown system |
| 🚫 Deadlock avoidance | Deterministic resource ordering |
| ⚖️ Fairness | Anti-starvation scheduling |
| 🖨️ Logging | Thread-safe output |
| 🧹 Cleanup | Dynamic memory management + graceful shutdown |

---

## ⚙️ Build it

```bash
make        # compile
make clean  # remove object files
make fclean # remove everything
make re     # rebuild from scratch
```

## ▶️ Run it

```bash
./codexion \
  <number_of_coders> \
  <time_to_burnout> \
  <time_to_compile> \
  <time_to_debug> \
  <time_to_refactor> \
  <number_of_required_compiles> \
  <dongle_cooldown> \
  <fifo|edf>
```

**Example** — 5 coders, FIFO scheduling:

```bash
./codexion 5 3000 200 200 200 5 400 fifo
```

---

## 🧠 The Daily Grind (Simulation Flow)

```
        🚀 Start
           │
           ▼
   👥 Create Threads
           │
           ▼
   ⏳ Wait for Scheduler
           │
           ▼
   🔌 Acquire Dongles
           │
           ▼
      💻 Compile
           │
           ▼
   🔓 Release Dongles
           │
           ▼
       🐞 Debug
           │
           ▼
     🛠️ Refactor
           │
           ▼
    🔁 Repeat Cycle
```

---

## 🥊 Concurrency Boss Fights

Every good concurrency project has to beat these classic bosses. Here's how Codexion does it:

### 💀 Deadlocks
Dongles are always acquired in a **deterministic order**, so circular waits simply can't happen. No boss fight here — just a locked door.

### 🏁 Race Conditions
All shared state is mutex-protected, including:
- Compile counters
- Queue state
- Scheduler state
- Shared stop flag
- Dongle ownership

No two threads touch the same memory at the same time. Ever.

### 😴 Starvation
Both schedulers guarantee that **every coder eventually gets their turn**:

| Scheduler | Strategy |
|---|---|
| **FIFO** | First come, first compiled — coders run in arrival order |
| **EDF** | Earliest Deadline First — whoever's closest to burnout goes next |

### 🔀 Resource Contention
Only non-conflicting coders run in parallel — the simulation squeezes out as much concurrency as it can while guaranteeing **no two coders ever share a dongle** at the same time.

### 🛑 Graceful Shutdown
When burnout or completion is detected:
1. All waiting threads are woken up
2. Everyone finishes their current task
3. Every thread is joined
4. Synchronization primitives are destroyed
5. All allocated memory is freed

Clean exit, no leaks, no zombies. 🧟‍♂️❌

---

## 🧵 The Synchronization Toolbox

| Primitive | Role |
|---|---|
| `pthread_create` | Spawns one worker thread per coder, plus a monitor thread |
| `pthread_join` | Waits for every thread to finish before cleanup |
| `pthread_mutex_t` | Guards dongles, scheduler queue, and per-coder state |
| `pthread_cond_t` | Lets waiting threads sleep instead of spinning, waking only when it's their turn |

Condition variables are the real MVP here — they kill busy-waiting and keep the scheduling efficient.

---

## 📚 Resources

**Documentation**
- POSIX Threads Documentation
- Linux pthread Manual Pages
- The Open Group Base Specifications
- 42 Curriculum Documentation

**References**
- Dining Philosophers Problem
- Earliest Deadline First Scheduling
- Producer–Consumer Synchronization
- Monitor Synchronization Pattern


---

## 🤖 AI Usage

AI was used as a learning and productivity tool — for code reviews, debugging ideas, documentation writing, concurrency discussions, and general explanations.

Every implementation decision, algorithm, debugging session, testing process, and final code integration was done **manually**, by hand, thread by thread.

---

<div align="center">

Made with ❤️, C, and *way* too many threads by **etchipoq**

</div>
