#ifndef COMMAND_TYPE_HPP
#define COMMAND_TYPE_HPP

#include <QObject>

enum class CommandType {
    // Connection
    CONNECT,
    DISCONNECT,

    // Target Control
    RESET_HALT,
    RESET_RUN,
    HALT,
    RESUME,
    STEP,

    // Memory Operations
    READ_MEMORY,
    WRITE_MEMORY,
    FILL_MEMORY,

    // Flash Operations
    FLASH_ERASE,
    FLASH_WRITE,
    FLASH_VERIFY,
    LOAD_IMAGE,

    // Debug
    READ_REGISTER,
    WRITE_REGISTER,
    BREAKPOINT_SET,
    BREAKPOINT_CLEAR,

    // System
    VERSION,
    SHUTDOWN,
    CUSTOM
};

// Command data structure
struct Command {
    CommandType type = CommandType::CUSTOM;
    QStringList args;

    // Default constructor
    Command() = default;
    
    Command(CommandType t, const QStringList& a = {})
        : type(t), args(a) {}
};

#endif // COMMAND_TYPE_HPP
