# pbash - Minimalist Pseudo Bash Simulator

A lightweight, educational bash simulator written in C for learning purposes and terminal experimentation.

## Features

- **Command System**: Modular command architecture with easy extension
- **Raw Terminal Handling**: Full control over terminal input/output
- **Signal Handling**: Proper SIGINT/SIGTERM handling with graceful shutdown
- **Cross-platform**: Designed for Linux systems with POSIX compliance
- **Minimal Dependencies**: Only requires standard C libraries and pthreads

## Project Structure

```
pbash/
├── src/
│   ├── main.c              # Entry point and signal handling
│   ├── cli.c               # Terminal interface and raw mode handling
│   ├── cli.h               # CLI definitions and prototypes
│   ├── cmd_common.c        # Command registration and resolution
│   ├── cmd_common.h        # Command system interface
│   ├── cmd_glue_gen.c      # Auto-generated command glue code
│   ├── cmd_glue_gen.h      # Command glue interface
│   ├── history.c           # Command history management (WiP)
│   ├── history.h           # History system definitions
│   └── commands/           # Individual command implementations
│       ├── cd.h/cd.c       # Change directory
│       ├── cp.h/cp.c       # Copy files
│       ├── grep.h/grep.c   # Pattern search
│       ├── ls.h/ls.c       # List directory contents
│       ├── ping.h/ping.c   # Network connectivity test
│       └── pwd.h/pwd.c     # Print working directory
├── cmake/
│   ├── cmd_glue_gen.cmake # Command code generation
│   └── templates/         # Code generation templates
└── CMakeLists.txt         # Build configuration
```

## Building

### Prerequisites
- CMake 3.12+
- GCC compiler
- Linux system with POSIX compliance

### Build Steps
```bash
# Clone and setup
git clone <repository-url>
cd pbash

# Configure build
mkdir build && cd build
cmake ..

# Compile
make

# Install system-wide (optional)
sudo make install
```

## Usage

Run the simulator:
```bash
./pbash
```

You'll see a prompt like:
```
~pbash:/current/path> 
```

### Available Commands
- `cd` - Change directory
- `cp` - Copy files  
- `grep` - Search patterns
- `ls` - List files
- `ping` - Network test
- `pwd` - Show current directory

### Key Bindings
- `Enter` - Execute command
- `Backspace` - Delete character
- `Tab` - Auto-completion (basic)
- `Ctrl+C` - Graceful exit

## Command System Architecture

The project uses a modular command system where each command is implemented as a separate C file:

1. **Command Registration**: Commands auto-register via CMake code generation
2. **Interface**: All commands implement `int command_name(int argc, char** argv)`
3. **Discovery**: New commands are automatically detected and compiled

### Adding New Commands

1. Create header file in `src/commands/`:
```c
// mycmd.h
#ifndef _MYCMD_H_
#define _MYCMD_H_

int mycmd(int argc, char** argv);

#endif
```

2. Create implementation:
```c
// mycmd.c
#include "mycmd.h"

int mycmd(int argc, char** argv) {
    // Command implementation
    return 0;
}
```

3. Rebuild - CMake automatically detects and integrates the new command

## Technical Details

### Terminal Handling
- Uses termios for raw terminal mode
- Implements proper line editing
- Handles escape sequences and special keys

### Signal Management
- Dedicated thread for signal handling
- Graceful shutdown on SIGINT/SIGTERM
- Proper resource cleanup

### Memory Safety
- Bounded input buffers
- Proper allocation/deallocation
- Static analysis friendly code

## Development

### Code Style
- C99 standard compliance
- 4-space indentation
- Descriptive variable names
- Header guards with `_FILE_H_` convention

### Debugging
```bash
# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make

# Run with gdb
gdb ./pbash
```

### Testing
Currently manual testing. Future versions may include unit tests for the command system.

## Limitations

- **History System**: Work in progress - not yet implemented
- **Error Handling**: Basic error reporting - needs enhancement
- **Platform Support**: Linux-focused, limited POSIX compatibility
- **Feature Completeness**: Educational project - not production-ready

## Contributing

This is an educational project. Contributions should focus on:
- Code clarity and educational value
- POSIX compliance improvements
- Documentation enhancements
- Safe memory handling practices

## License

GNU GPL 2.0

## Acknowledgments

Built for learning terminal systems, command line interfaces, and C programming best practices.
