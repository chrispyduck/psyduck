#include "homie-declarations.h"

namespace psyduck
{

  namespace homie
  {
    HomieProperty *HomieProperty::build(
        HomieNode *node,
        const char *id,
        const char *name,
        const char *type,
        const char *unit,
        const char *format,
        MessageReceivedCallback setter)
    {
      auto prop = new HomieProperty(node, id, name, type);
      if (unit != nullptr)
      {
        prop->setUnit(unit);
      }
      if (format != nullptr)
      {
        prop->setFormat(format);
      }
      if (setter != nullptr)
      {
        prop->setSettable(setter);
      }
      return prop;
    }

    HomieProperty *HomieProperty::percentage(HomieNode *node, const char *id, const char *name, float currentValue, MessageReceivedCallback setter)
    {
      auto prop = build(node, id, name, "integer", "%", "0:100", setter);
      prop->setValue(currentValue);
      return prop;
    }

    HomieProperty *HomieProperty::minutes(HomieNode *node, const char *id, const char *name, float currentValue, MessageReceivedCallback setter)
    {
      auto prop = build(node, id, name, "float", "Minutes", nullptr, setter);
      prop->setValue(currentValue);
      return prop;
    }

    HomieProperty *HomieProperty::boolean(HomieNode *node, const char *id, const char *name, bool currentValue, MessageReceivedCallback setter)
    {
      auto prop = build(node, id, name, "boolean", nullptr, nullptr, setter);
      prop->setValue(currentValue);
      return prop;
    }

    HomieProperty *HomieProperty::counter(HomieNode *node, const char *id, const char *name, int currentValue)
    {
      auto prop = build(node, id, name, "integer", nullptr, nullptr, nullptr);
      prop->setValue(currentValue);
      return prop;
    }
  }
}