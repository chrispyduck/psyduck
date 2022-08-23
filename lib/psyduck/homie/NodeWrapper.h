#pragma once

#include "homie-declarations.h"

namespace psyduck
{
  namespace homie
  {
    class NodeWrapper
    {
    public:
      NodeWrapper(HomieDevice *device, const char *id, const char *name, const char *type);

    protected:
      HomieNode* getNode();

    private:
      HomieNode *node;
    };
  }
}