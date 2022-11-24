#include "val.h"
// add op to this
Value Value::operator+(const Value &op) const {
  Value newVal;
  if (this->IsInt() && op.IsInt())
    newVal = Value(this->GetInt() + op.GetInt());
  else if (this->IsReal() && op.IsReal())
    newVal = Value(this->GetReal() + op.GetReal());
  else if (this->IsInt() && op.IsReal())
    newVal = Value(this->GetInt() + op.GetReal());
  else if (this->IsReal() && op.IsInt())
    newVal = Value(this->GetReal() + op.GetInt());
  return newVal;
}

// subtract op from this
Value Value::operator-(const Value &op) const {
  Value newVal;
  if (this->IsInt() && op.IsInt())
    newVal = Value(this->GetInt() - op.GetInt());
  else if (this->IsReal() && op.IsReal())
    newVal = Value(this->GetReal() - op.GetReal());
  else if (this->IsInt() && op.IsReal())
    newVal = Value(this->GetInt() - op.GetReal());
  else if (this->IsReal() && op.IsInt())
    newVal = Value(this->GetReal() - op.GetInt());
  return newVal;
}

// multiply this by op
Value Value::operator*(const Value &op) const {
  Value newVal;
  if (this->IsInt() && op.IsInt())
    newVal = Value(this->GetInt() * op.GetInt());
  else if (this->IsReal() && op.IsReal())
    newVal = Value(this->GetReal() * op.GetReal());
  else if (this->IsInt() && op.IsReal())
    newVal = Value(this->GetInt() * op.GetReal());
  else if (this->IsReal() && op.IsInt())
    newVal = Value(this->GetReal() * op.GetInt());
  return newVal;
}

// divide this by op
Value Value::operator/(const Value &op) const {
  Value newVal;
  if (this->IsInt() && op.IsInt())
    newVal = Value(this->GetInt() / op.GetInt());
  else if (this->IsReal() && op.IsReal())
    newVal = Value(this->GetReal() / op.GetReal());
  else if (this->IsInt() && op.IsReal())
    newVal = Value(this->GetInt() / op.GetReal());
  else if (this->IsReal() && op.IsInt())
    newVal = Value(this->GetReal() / op.GetInt());
  return newVal;
}

Value Value::operator==(const Value &op) const {
  Value newVal;
  if (this->IsBool() && op.IsBool())
    newVal = Value(this->GetBool() == op.GetBool());
  else if (this->IsInt() && op.IsInt())
    newVal = Value(this->GetInt() == op.GetInt());
  else if (this->IsReal() && op.IsReal())
    newVal = Value(this->GetReal() == op.GetReal());
  else if (this->IsInt() && op.IsReal())
    newVal = Value(this->GetInt() == op.GetReal());
  else if (this->IsReal() && op.IsInt())
    newVal = Value(this->GetReal() == op.GetInt());
  return newVal;
}

Value Value::operator>(const Value &op) const {
  Value newVal;
  if (this->IsBool() && op.IsBool())
    newVal = Value(this->GetBool() > op.GetBool());
  else if (this->IsInt() && op.IsInt())
    newVal = Value(this->GetInt() > op.GetInt());
  else if (this->IsReal() && op.IsReal())
    newVal = Value(this->GetReal() > op.GetReal());
  else if (this->IsInt() && op.IsReal())
    newVal = Value(this->GetInt() > op.GetReal());
  else if (this->IsReal() && op.IsInt())
    newVal = Value(this->GetReal() > op.GetInt());
  return newVal;
}

Value Value::operator<(const Value &op) const {
  Value newVal;
  if (this->IsBool() && op.IsBool())
    newVal = Value(this->GetBool() < op.GetBool());
  else if (this->IsInt() && op.IsInt())
    newVal = Value(this->GetInt() < op.GetInt());
  else if (this->IsReal() && op.IsReal())
    newVal = Value(this->GetReal() < op.GetReal());
  else if (this->IsInt() && op.IsReal())
    newVal = Value(this->GetInt() < op.GetReal());
  else if (this->IsReal() && op.IsInt())
    newVal = Value(this->GetReal() < op.GetInt());
  return newVal;
}

Value Value::operator&&(const Value &op) const {
  Value newVal;
  if (this->IsBool() && op.IsBool())
    newVal = Value(this->GetBool() && op.GetBool());
  return newVal;
}

Value Value::operator||(const Value &op) const {
  Value newVal;
  if (this->IsBool() && op.IsBool())
    newVal = Value(this->GetBool() || op.GetBool());
  return newVal;
}

Value Value::operator!() const {
  Value newVal;
  if (this->IsBool())
    newVal = Value(!this->GetBool());
  return newVal;
}