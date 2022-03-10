#include "declarations.h"

namespace psyduck
{
  namespace commands
  {
    void CommandHandler::parse(String input)
    {
      auto command = extractFirstToken(input);
      auto args = input.length() > command.length()
                      ? input.substring(command.length() + 1)
                      : emptyString;
      auto match = this->commands.find(command);
      auto handler = match != this->commands.end() ? nullptr : match->second;
      if (handler != nullptr)
        handler->execute(args);
    }

    void CommandHandler::registerCommand(String commandName, Command::handler_t* handler)
    {
      this->commands.insert(std::pair<String, Command *>(commandName, new Command(handler)));
    }

    void CommandHandler::registerCommand(String commandName, CommandHandler* handler)
    {
      this->commands.insert(std::pair<String, Command *>(commandName, new Command(handler)));
    }

    String CommandHandler::extractFirstToken(String input)
    {
      auto space = input.indexOf(' ');
      return space >= 0
                 ? input.substring(0, space)
                 : input;
    }
  }
}