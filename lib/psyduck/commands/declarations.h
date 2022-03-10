#pragma once
#include <Arduino.h>
#include <functional>
#include <map>

namespace psyduck
{
  namespace commands
  {
    class CommandHandler;

    enum CommandType
    {
      COMMAND_TYPE_FUNCTION = 0,
      COMMAND_TYPE_CHILD_HANDLER = 1
    };

    class Command
    {
    private:
      CommandType type;
      void *ref;

    public:
      typedef void (*handler_t)(String args);

      Command(handler_t *handler);
      Command(CommandHandler *handler);

      CommandType getType();
      void execute(String args);
    };

    class CommandHandler
    {
    public:
      void parse(String input);

      void registerCommand(String command, Command::handler_t* handler);
      void registerCommand(String commandName, CommandHandler* handler);

    private:
      std::map<String, Command *> commands;

      static String extractFirstToken(String input);
    };

  }
}