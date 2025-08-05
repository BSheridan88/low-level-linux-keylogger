# low-level-linux-keylogger(Education Purposes)

### ⚠️ Disclaimer

> This project is **strictly for educational and personal learning purposes.** It is **not intended for deployment, malicious use, or distribution** in any context outside of responsible research, ethical cybersecurity practice, or academic environments.

---

## 📚 About This Project

This is a personal project I developed in the summer before starting college, as a way to teach myself:

- Low-level Linux system interaction
- Raw input device handling
- Sockets and network communication
- Process control and daemonization
- Basic keylogger design
- Shared object manipulation and dynamic loading

It’s messy. It’s not stealthy. It’s full of gaps. But that’s **exactly the point** — I made this to learn how these components work at a fundamental level.

---

## 🧠 My Background (Why I Made This)

I’m 18 years old and just beginning college for cybersecurity. I’ve taken an AP CS class in high school and started learning C seriously this summer. My goal is to become a red teamer and malware analyst — someone who understands how things break, and how to defend against them.

This keylogger project is my way of diving deep into:

- `/dev/input` devices
- Linux’s event handling system
- How key events map to actual characters
- Socket-based communication between a keylogger and remote host
- Shared object (`.so`) injection and `dlopen` behavior

Everything in this project was written by me, from scratch, as part of my self-education.

---

## 🛠️ How It Works

- A **dropper binary** places and loads a shared object.
- The `.so` uses `__attribute__((constructor))` to trigger on load.
- It forks into the background, finds a keyboard device, and listens to raw key events.
- Characters are parsed from scancodes and sent over a socket to a listening host.
- The **host script** receives keystrokes and writes them to a local `log.txt`.

---

## 🏃 How to Run

1. Compile the dropper binary and the shared object (`libdes.so`).
2. Run the dropper binary on the target Linux machine.
3. Run the host program on your listening machine to capture keystrokes.
4. View captured keystrokes in the `log.txt` file created by the host.

*Note: This project is for educational use only. Use responsibly.*

---

## 🔒 KNOWN LIMITATIONS

This code is **not production-quality**, and I’m fully aware of that. Some known flaws:

- No encryption or obfuscation
- No buffering or efficient batching
- Fragile key conversion logic (limited symbol support)
- Static file paths
- Poor operational security (no persistence, artifacts left behind)

This was built as a first attempt to understand the core mechanics. It’s a learning sandbox — not an actual implant.

---

## 💡 What I Learned

Through this project, I learned:

- How `/proc/bus/input/devices` works
- How to interpret `input_event` structs
- How `fork()`, `setsid()`, and background processes work
- How to use raw sockets with `send()`/`recv()` in C
- How shared objects can be injected and triggered with `dlopen`
- How to log input and manipulate files using `ftell()`, `fseek()`, `ftruncate()`

---

## 🔐 Responsible Use & Ethics

Again — this project is **not intended to be used against anyone or anything**. It exists purely for study, experimentation, and learning how real-world malware operates on a low level, so I can better defend against it in the future.

If you clone, fork, or study this project, please do so **legally and ethically**. Never run it on a machine or device that you do not fully own or have explicit permission to test.

---

## 👋 Final Notes

This is a beginner’s project — not a weapon. I’m proud of how much I learned from building it, even though the code has a long way to go. If you’re learning too, feel free to dig into it — and always build with **purpose, ethics, and curiosity.**
