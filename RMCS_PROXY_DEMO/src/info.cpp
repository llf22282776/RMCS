#include "info.hpp"

namespace hebi {

Info::FloatField::FloatField(HebiInfoPtr internal, InfoFloatField field)
  : internal_(internal), field_(field)
{
}

Info::FloatField::operator bool() const
{
  return has();
}

bool Info::FloatField::has() const
{
  return (hebiInfoHasFloat(internal_, field_) == 1);
}

float Info::FloatField::get() const
{
  return hebiInfoGetFloat(internal_, field_);
}

Info::BoolField::BoolField(HebiInfoPtr internal, InfoBoolField field)
  : internal_(internal), field_(field)
{
}

bool Info::BoolField::has() const
{
  return (hebiInfoHasBool(internal_, field_) == 1);
}

bool Info::BoolField::get() const
{
  return (hebiInfoGetBool(internal_, field_) == 1);
}

Info::StringField::StringField(HebiInfoPtr internal, InfoStringField field)
  : internal_(internal), field_(field)
{
}

Info::StringField::operator bool() const
{
  return has();
}

bool Info::StringField::has() const
{
  return (hebiInfoHasString(internal_, field_) == 1);
}

std::string Info::StringField::get() const
{
  // Get the size first
  int required_size = hebiInfoGetString(internal_, field_, nullptr, 0);
  char* buffer = new char [required_size];
  hebiInfoGetString(internal_, field_, buffer, required_size);
  std::string tmp(buffer);
  delete[] buffer;
  return tmp;
}

Info::FlagField::FlagField(HebiInfoPtr internal, InfoFlagField field)
  : internal_(internal), field_(field)
{
}

Info::FlagField::operator bool() const
{
  return has();
}

bool Info::FlagField::has() const
{
  return (hebiInfoHasFlag(internal_, field_) == 1);
}

Info::LedField::LedField(HebiInfoPtr internal, InfoLedField field)
  : internal_(internal), field_(field)
{
}

bool Info::LedField::hasColor() const
{
  return (hebiInfoHasLedColor(internal_, field_) == 1);
}

Color Info::LedField::getColor() const
{
  uint8_t r, g, b;
  hebiInfoGetLedColor(internal_, field_, &r, &g, &b);
  return Color(r, g, b);
}

Info::Info(HebiInfoPtr info)
  : internal_(info),
    settings_(internal_),
    led_(internal_, InfoLedLed)
{
}
Info::~Info() noexcept
{
}

Info::Info(Info&& other)
  : internal_(other.internal_),
    settings_(internal_),
    led_(internal_, InfoLedLed)
{
  // NOTE: it would be nice to also cleanup the actual internal pointer of other
  // but alas we cannot change a const variable.
}

const Info::LedField& Info::led() const
{
  return led_;
}

} // namespace hebi
