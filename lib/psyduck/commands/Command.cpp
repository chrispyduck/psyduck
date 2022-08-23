#include "declarations.h"

namespace psyduck
{
  namespace commands
  {
    Command::Command(handler_t *handler)
    {
      this->type = COMMAND_TYPE_FUNCTION;
      this->ref = (void*)handler;
    }

    Command::Command(CommandHandler *handler)
    {
      this->type = COMMAND_TYPE_CHILD_HANDLER;
      this->ref = handler;
    }

    CommandType Command::getType() { return this->type; }
    void Command::execute(String args)
    {
      switch (this->type)
      {
      case COMMAND_TYPE_CHILD_HANDLER:
      {
        CommandHandler *handler = static_cast<CommandHandler *>(this->ref);
        handler->parse(args);
        break;
      }
      case COMMAND_TYPE_FUNCTION:
      {
        handler_t* handler = static_cast<handler_t*>(this->ref);
        (*handler)(args);
        break;
      }
      }
    }
  }
}