// Copyright (c) 2012-2017, The CryptoNote developers, The Bytecoin developers
// Copyright (c) 2020, The Talleo developers
//
// This file is part of Bytecoin.
//
// Bytecoin is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Bytecoin is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Bytecoin.  If not, see <http://www.gnu.org/licenses/>.

#include "JsonValue.h"
#include <iomanip>
#include <sstream>

namespace Common {

JsonValue::JsonValue() : type(NIL) {
}

JsonValue::JsonValue(const JsonValue& other) {
  switch (other.type) {
  case ARRAY:
    {
      const Array* pArray = reinterpret_cast<const Array*>(other.valueArray);
      new(valueArray)Array(*pArray);
      break;
    }
  case BOOL:
    valueBool = other.valueBool;
    break;
  case INTEGER:
    valueInteger = other.valueInteger;
    break;
  case NIL:
    break;
  case OBJECT:
    {
      const Object* pObject = reinterpret_cast<const Object*>(other.valueObject);
      new(valueObject)Object(*pObject);
      break;
    }
  case REAL:
    valueReal = other.valueReal;
    break;
  case STRING:
    {
      const String* pString = reinterpret_cast<const String*>(other.valueString);
      new(valueString)String(*pString);
      break;
    }
  }

  type = other.type;
}

JsonValue::JsonValue(JsonValue&& other) {
  switch (other.type) {
  case ARRAY:
    {
      Array* pArray = reinterpret_cast<Array*>(other.valueArray);
      new(valueArray)Array(std::move(*pArray));
      pArray->~Array();
      break;
    }
  case BOOL:
    valueBool = other.valueBool;
    break;
  case INTEGER:
    valueInteger = other.valueInteger;
    break;
  case NIL:
    break;
  case OBJECT:
    {
      Object* pObject = reinterpret_cast<Object*>(other.valueObject);
      new(valueObject)Object(std::move(*pObject));
      pObject->~Object();
      break;
    }
  case REAL:
    valueReal = other.valueReal;
    break;
  case STRING:
    {
      String* pString = reinterpret_cast<String*>(other.valueString);
      new(valueString)String(std::move(*pString));
      pString->~String();
      break;
    }
  }

  type = other.type;
  other.type = NIL;
}

JsonValue::JsonValue(Type valueType) {
  switch (valueType) {
  case ARRAY:
    new(valueArray)Array;
    break;
  case NIL:
    break;
  case OBJECT:
    new(valueObject)Object;
    break;
  case STRING:
    new(valueString)String;
    break;
  default:
    throw std::runtime_error("Invalid JsonValue type for constructor");
  }

  type = valueType;
}

JsonValue::JsonValue(const Array& value) {
  new(valueArray)Array(value);
  type = ARRAY;
}

JsonValue::JsonValue(Array&& value) {
  new(valueArray)Array(std::move(value));
  type = ARRAY;
}

JsonValue::JsonValue(Bool value) : type(BOOL), valueBool(value) {
}

JsonValue::JsonValue(Integer value) : type(INTEGER), valueInteger(value) {
}

JsonValue::JsonValue(Nil) : type(NIL) {
}

JsonValue::JsonValue(const Object& value) {
  new(valueObject)Object(value);
  type = OBJECT;
}

JsonValue::JsonValue(Object&& value) {
  new(valueObject)Object(std::move(value));
  type = OBJECT;
}

JsonValue::JsonValue(Real value) : type(REAL), valueReal(value) {
}

JsonValue::JsonValue(const String& value) {
  new(valueString)String(value);
  type = STRING;
}

JsonValue::JsonValue(String&& value) {
  new(valueString)String(std::move(value));
  type = STRING;
}

JsonValue::~JsonValue() {
  destructValue();
}

JsonValue& JsonValue::operator=(const JsonValue& other) {
  if (type != other.type) {
    destructValue();
    switch (other.type) {
    case ARRAY:
      {
        type = NIL;
        const Array* pArray = reinterpret_cast<const Array*>(other.valueArray);
        new(valueArray)Array(*pArray);
        break;
      }
    case BOOL:
      valueBool = other.valueBool;
      break;
    case INTEGER:
      valueInteger = other.valueInteger;
      break;
    case NIL:
      break;
    case OBJECT:
      {
        type = NIL;
        const Object* pObject = reinterpret_cast<const Object*>(other.valueObject);
        new(valueObject)Object(*pObject);
        break;
      }
    case REAL:
      valueReal = other.valueReal;
      break;
    case STRING:
      {
        type = NIL;
        const String* pString = reinterpret_cast<const String*>(other.valueString);
        new(valueString)String(*pString);
        break;
      }
    }

    type = other.type;
  } else {
    switch (type) {
    case ARRAY:
      {
        Array* pArray = reinterpret_cast<Array *>(valueArray);
        const Array* pOtherArray = reinterpret_cast<const Array *>(other.valueArray);
        *pArray = *pOtherArray;
        break;
      }
    case BOOL:
      valueBool = other.valueBool;
      break;
    case INTEGER:
      valueInteger = other.valueInteger;
      break;
    case NIL:
      break;
    case OBJECT:
      {
        Object* pObject = reinterpret_cast<Object*>(valueObject);
        const Object* pOtherObject = reinterpret_cast<const Object*>(other.valueObject);
        *pObject = *pOtherObject;
        break;
      }
    case REAL:
      valueReal = other.valueReal;
      break;
    case STRING:
      {
        String* pString = reinterpret_cast<String*>(valueString);
        const String* pOtherString = reinterpret_cast<const String*>(other.valueString);
        *pString = *pOtherString;
        break;
      }
    }
  }

  return *this;
}

JsonValue& JsonValue::operator=(JsonValue&& other) {
  if (type != other.type) {
    destructValue();
    switch (other.type) {
    case ARRAY:
      {
        type = NIL;
        const Array* pArray = reinterpret_cast<const Array *>(other.valueArray);
        new(valueArray)Array(std::move(*pArray));
        pArray->~Array();
        break;
      }
    case BOOL:
      valueBool = other.valueBool;
      break;
    case INTEGER:
      valueInteger = other.valueInteger;
      break;
    case NIL:
      break;
    case OBJECT:
      {
        type = NIL;
        const Object* pObject = reinterpret_cast<const Object *>(other.valueObject);
        new(valueObject)Object(std::move(*pObject));
        pObject->~Object();
        break;
      }
    case REAL:
      valueReal = other.valueReal;
      break;
    case STRING:
      {
        type = NIL;
        const String* pString = reinterpret_cast<const String*>(other.valueString);
        new(valueString)String(std::move(*pString));
        pString->~String();
        break;
      }
    }

    type = other.type;
  } else {
    switch (type) {
    case ARRAY:
      {
        Array* pArray = reinterpret_cast<Array*>(valueArray);
        const Array* pOtherArray = reinterpret_cast<const Array*>(other.valueArray);
        *pArray = std::move(*pOtherArray);
        pOtherArray->~Array();
        break;
      }
    case BOOL:
      valueBool = other.valueBool;
      break;
    case INTEGER:
      valueInteger = other.valueInteger;
      break;
    case NIL:
      break;
    case OBJECT:
      {
        Object* pObject = reinterpret_cast<Object*>(valueObject);
        const Object* pOtherObject = reinterpret_cast<const Object*>(other.valueObject);
        *pObject = std::move(*pOtherObject);
        pOtherObject->~Object();
        break;
      }
    case REAL:
      valueReal = other.valueReal;
      break;
    case STRING:
      {
        String* pString = reinterpret_cast<String*>(valueString);
        const String * pOtherString = reinterpret_cast<const String*>(other.valueString);
        *pString = std::move(*pOtherString);
        pOtherString->~String();
        break;
      }
    }
  }

  other.type = NIL;
  return *this;
}

JsonValue& JsonValue::operator=(const Array& value) {
  if (type != ARRAY) {
    destructValue();
    type = NIL;
    new(valueArray)Array(value);
    type = ARRAY;
  } else {
    Array* pArray = reinterpret_cast<Array*>(valueArray);
    *pArray = value;
  }

  return *this;
}

JsonValue& JsonValue::operator=(Array&& value) {
  if (type != ARRAY) {
    destructValue();
    type = NIL;
    new(valueArray)Array(std::move(value));
    type = ARRAY;
  } else {
    Array* pArray = reinterpret_cast<Array*>(valueArray);
    *pArray = std::move(value);
  }

  return *this;
}

//JsonValue& JsonValue::operator=(Bool value) {
//  if (type != BOOL) {
//    destructValue();
//    type = BOOL;
//  }
//
//  valueBool = value;
//  return *this;
//}

JsonValue& JsonValue::operator=(Integer value) {
  if (type != INTEGER) {
    destructValue();
    type = INTEGER;
  }

  valueInteger = value;
  return *this;
}

JsonValue& JsonValue::operator=(Nil) {
  if (type != NIL) {
    destructValue();
    type = NIL;
  }

  return *this;
}

JsonValue& JsonValue::operator=(const Object& value) {
  if (type != OBJECT) {
    destructValue();
    type = NIL;
    new(valueObject)Object(value);
    type = OBJECT;
  } else {
    Object* pObject = reinterpret_cast<Object*>(valueObject);
    *pObject = value;
  }

  return *this;
}

JsonValue& JsonValue::operator=(Object&& value) {
  if (type != OBJECT) {
    destructValue();
    type = NIL;
    new(valueObject)Object(std::move(value));
    type = OBJECT;
  } else {
    Object* pObject = reinterpret_cast<Object*>(valueObject);
    *pObject = std::move(value);
  }

  return *this;
}

JsonValue& JsonValue::operator=(Real value) {
  if (type != REAL) {
    destructValue();
    type = REAL;
  }

  valueReal = value;
  return *this;
}

JsonValue& JsonValue::operator=(const String& value) {
  if (type != STRING) {
    destructValue();
    type = NIL;
    new(valueString)String(value);
    type = STRING;
  } else {
    String* pString = reinterpret_cast<String*>(valueString);
    *pString = value;
  }

  return *this;
}

JsonValue& JsonValue::operator=(String&& value) {
  if (type != STRING) {
    destructValue();
    type = NIL;
    new(valueString)String(std::move(value));
    type = STRING;
  } else {
    String* pString = reinterpret_cast<String*>(valueString);
    *pString = std::move(value);
  }

  return *this;
}

bool JsonValue::isArray() const {
  return type == ARRAY;
}

bool JsonValue::isBool() const {
  return type == BOOL;
}

bool JsonValue::isInteger() const {
  return type == INTEGER;
}

bool JsonValue::isNil() const {
  return type == NIL;
}

bool JsonValue::isObject() const {
  return type == OBJECT;
}

bool JsonValue::isReal() const {
  return type == REAL;
}

bool JsonValue::isString() const {
  return type == STRING;
}

JsonValue::Type JsonValue::getType() const {
  return type;
}

JsonValue::Array& JsonValue::getArray() {
  if (type != ARRAY) {
    throw std::runtime_error("JsonValue type is not ARRAY");
  }

  Array* pArray = reinterpret_cast<Array*>(valueArray);
  return *pArray;
}

const JsonValue::Array& JsonValue::getArray() const {
  if (type != ARRAY) {
    throw std::runtime_error("JsonValue type is not ARRAY");
  }

  const Array* pArray = reinterpret_cast<const Array*>(valueArray);
  return *pArray;
}

JsonValue::Bool JsonValue::getBool() const {
  if (type != BOOL) {
    throw std::runtime_error("JsonValue type is not BOOL");
  }

  return valueBool;
}

JsonValue::Integer JsonValue::getInteger() const {
  if (type != INTEGER) {
    throw std::runtime_error("JsonValue type is not INTEGER");
  }

  return valueInteger;
}

JsonValue::Object& JsonValue::getObject() {
  if (type != OBJECT) {
    throw std::runtime_error("JsonValue type is not OBJECT");
  }

  Object* pObject = reinterpret_cast<Object*>(valueObject);
  return *pObject;
}

const JsonValue::Object& JsonValue::getObject() const {
  if (type != OBJECT) {
    throw std::runtime_error("JsonValue type is not OBJECT");
  }

  const Object* pObject = reinterpret_cast<const Object*>(valueObject);
  return *pObject;
}

JsonValue::Real JsonValue::getReal() const {
  if (type != REAL) {
    throw std::runtime_error("JsonValue type is not REAL");
  }

  return valueReal;
}

JsonValue::String& JsonValue::getString() {
  if (type != STRING) {
    throw std::runtime_error("JsonValue type is not STRING");
  }

  String* pString = reinterpret_cast<String*>(valueString);
  return *pString;
}

const JsonValue::String& JsonValue::getString() const {
  if (type != STRING) {
    throw std::runtime_error("JsonValue type is not STRING");
  }

  const String* pString = reinterpret_cast<const String*>(valueString);
  return *pString;
}

size_t JsonValue::size() const {
  switch (type) {
  case ARRAY:
    {
      const Array* pArray = reinterpret_cast<const Array*>(valueArray);
      return pArray->size();
    }
  case OBJECT:
    {
      const Object* pObject = reinterpret_cast<const Object*>(valueObject);
      return pObject->size();
    }
  default:
    throw std::runtime_error("JsonValue type is not ARRAY or OBJECT");
  }
}

JsonValue& JsonValue::operator[](size_t index) {
  if (type != ARRAY) {
    throw std::runtime_error("JsonValue type is not ARRAY");
  }

  Array* pArray = reinterpret_cast<Array*>(valueArray);
  return pArray->at(index);
}

const JsonValue& JsonValue::operator[](size_t index) const {
  if (type != ARRAY) {
    throw std::runtime_error("JsonValue type is not ARRAY");
  }

  const Array* pArray = reinterpret_cast<const Array*>(valueArray);
  return pArray->at(index);
}

JsonValue& JsonValue::pushBack(const JsonValue& value) {
  if (type != ARRAY) {
    throw std::runtime_error("JsonValue type is not ARRAY");
  }

  Array* pArray = reinterpret_cast<Array*>(valueArray);
  pArray->emplace_back(value);
  return pArray->back();
}

JsonValue& JsonValue::pushBack(JsonValue&& value) {
  if (type != ARRAY) {
    throw std::runtime_error("JsonValue type is not ARRAY");
  }

  Array* pArray = reinterpret_cast<Array*>(valueArray);
  pArray->emplace_back(std::move(value));
  return pArray->back();
}

JsonValue& JsonValue::operator()(const Key& key) {
  return getObject().at(key);
}

const JsonValue& JsonValue::operator()(const Key& key) const {
  return getObject().at(key);
}

bool JsonValue::contains(const Key& key) const {
  return getObject().count(key) > 0;
}

JsonValue& JsonValue::insert(const Key& key, const JsonValue& value) {
  return getObject().emplace(key, value).first->second;
}

JsonValue& JsonValue::insert(const Key& key, JsonValue&& value) {
  return getObject().emplace(key, std::move(value)).first->second;
}

JsonValue& JsonValue::set(const Key& key, const JsonValue& value) {
  getObject()[key] = value;
  return *this;
}

JsonValue& JsonValue::set(const Key& key, JsonValue&& value) {
  getObject()[key] = std::move(value);
  return *this;
}

size_t JsonValue::erase(const Key& key) {
  return getObject().erase(key);
}

JsonValue JsonValue::fromString(const std::string& source) {
  JsonValue jsonValue;
  std::istringstream stream(source);
  stream >> jsonValue;
  if (stream.fail()) {
    throw std::runtime_error("Unable to parse JsonValue");
  }

  return jsonValue;
}

std::string JsonValue::toString() const {
  std::ostringstream stream;
  stream << *this;
  return stream.str();
}

std::ostream& operator<<(std::ostream& out, const JsonValue& jsonValue) {
  switch (jsonValue.type) {
  case JsonValue::ARRAY: {
    const JsonValue::Array* pArray = reinterpret_cast<const JsonValue::Array*>(jsonValue.valueArray);
    const JsonValue::Array& array = *pArray;
    out << '[';
    if (array.size() > 0) {
      out << array[0];
      for (size_t i = 1; i < array.size(); ++i) {
        out << ',' << array[i];
      }
    }

    out << ']';
    break;
  }
  case JsonValue::BOOL:
    out << (jsonValue.valueBool ? "true" : "false");
    break;
  case JsonValue::INTEGER:
    out << jsonValue.valueInteger;
    break;
  case JsonValue::NIL:
    out << "null";
    break;
  case JsonValue::OBJECT: {
    const JsonValue::Object* pObject = reinterpret_cast<const JsonValue::Object*>(jsonValue.valueObject);
    const JsonValue::Object& object = *pObject;
    out << '{';
    auto iter = object.begin();
    if (iter != object.end()) {
      out << '"' << iter->first << "\":" << iter->second;
      ++iter;
      for (; iter != object.end(); ++iter) {
        out << ",\"" << iter->first << "\":" << iter->second;
      }
    }

    out << '}';
    break;
  }
  case JsonValue::REAL: {
    std::ostringstream stream;
    stream << std::fixed << std::setprecision(11) << jsonValue.valueReal;
    std::string value = stream.str();
    while (value.size() > 1 && value[value.size() - 2] != '.' && value[value.size() - 1] == '0') {
      value.resize(value.size() - 1);
    }

    out << value;
    break;
  }
  case JsonValue::STRING:
    {
      const JsonValue::String* pString = reinterpret_cast<const JsonValue::String*>(jsonValue.valueString);
      out << '"' << *pString << '"';
      break;
    }
  }

  return out;
}


namespace {

char readChar(std::istream& in) {
  char c;

  if (!(in >> c)) {
    throw std::runtime_error("Unable to parse: unexpected end of stream");
  }

  return c;
}

char readNonWsChar(std::istream& in) {
  char c;

  do {
    c = readChar(in);
  } while (isspace(c));

  return c;
}

std::string readStringToken(std::istream& in) {
  char c;
  std::string value;

  while (in) {
    c = readChar(in);

    if (c == '"') {
      break;
    }

    if (c == '\\') {
      value += c;
      c = readChar(in);
    }

    value += c;
  }

  return value;
}

}


std::istream& operator>>(std::istream& in, JsonValue& jsonValue) {
  char c = readNonWsChar(in);

  if (c == '[') {
    jsonValue.readArray(in);
  } else if (c == 't') {
    jsonValue.readTrue(in);
  } else if (c == 'f') {
    jsonValue.readFalse(in);
  } else if ((c == '-') || (c >= '0' && c <= '9')) {
    jsonValue.readNumber(in, c);
  } else if (c == 'n') {
    jsonValue.readNull(in);
  } else if (c == '{') {
    jsonValue.readObject(in);
  } else if (c == '"') {
    jsonValue.readString(in);
  } else {
    throw std::runtime_error("Unable to parse");
  }

  return in;
}

void JsonValue::destructValue() {
  switch (type) {
  case ARRAY:
    {
      Array* pArray = reinterpret_cast<Array*>(valueArray);
      pArray->~Array();
      break;
    }
  case OBJECT:
    {
      Object* pObject = reinterpret_cast<Object*>(valueObject);
      pObject->~Object();
      break;
    }
  case STRING:
    {
      String* pString = reinterpret_cast<String*>(valueString);
      pString->~String();
      break;
    }
  default:
    break;
  }
}

void JsonValue::readArray(std::istream& in) {
  JsonValue::Array value;
  char c = readNonWsChar(in);

  if(c != ']') {
    in.putback(c);
    for (;;) {
      value.resize(value.size() + 1);
      in >> value.back();
      c = readNonWsChar(in);

      if (c == ']') {
        break;
      }

      if (c != ',') {
        throw std::runtime_error("Unable to parse");
      }
    }
  }

  if (type != JsonValue::ARRAY) {
    destructValue();
    type = JsonValue::NIL;
    new(valueArray)JsonValue::Array;
    type = JsonValue::ARRAY;
  }

  JsonValue::Array* pArray = reinterpret_cast<JsonValue::Array*>(valueArray);
  pArray->swap(value);
}

void JsonValue::readTrue(std::istream& in) {
  char data[3];
  in.read(data, 3);
  if (data[0] != 'r' || data[1] != 'u' || data[2] != 'e') {
    throw std::runtime_error("Unable to parse");
  }

  if (type != JsonValue::BOOL) {
    destructValue();
    type = JsonValue::BOOL;
  }

  valueBool = true;
}

void JsonValue::readFalse(std::istream& in) {
  char data[4];
  in.read(data, 4);
  if (data[0] != 'a' || data[1] != 'l' || data[2] != 's' || data[3] != 'e') {
    throw std::runtime_error("Unable to parse");
  }

  if (type != JsonValue::BOOL) {
    destructValue();
    type = JsonValue::BOOL;
  }

  valueBool = false;
}

void JsonValue::readNull(std::istream& in) {
  char data[3];
  in.read(data, 3);
  if (data[0] != 'u' || data[1] != 'l' || data[2] != 'l') {
    throw std::runtime_error("Unable to parse");
  }

  if (type != JsonValue::NIL) {
    destructValue();
    type = JsonValue::NIL;
  }
}

void JsonValue::readNumber(std::istream& in, char c) {
  std::string text;
  text += c;
  size_t dots = 0;
  for (;;) {
    int i = in.peek();
    if (i >= '0' && i <= '9') {
      in.read(&c, 1);
      text += c;
    } else  if (i == '.') {
      in.read(&c, 1);
      text += '.';
      ++dots;
    } else {
      break;
    }
  }

  if (dots > 0) {
    if (dots > 1) {
      throw std::runtime_error("Unable to parse");
    }

    int i = in.peek();
    if (in.peek() == 'e') {
      in.read(&c, 1);
      text += c;
      i = in.peek();
      if (i == '+') {
        in.read(&c, 1);
        text += c;
        i = in.peek();
      } else if (i == '-') {
        in.read(&c, 1);
        text += c;
        i = in.peek();
      }

      if (i < '0' || i > '9') {
        throw std::runtime_error("Unable to parse");
      }

      do {
        in.read(&c, 1);
        text += c;
        i = in.peek();
      } while (i >= '0' && i <= '9');
    }

    Real value;
    std::istringstream(text) >> value;
    if (type != REAL) {
      destructValue();
      type = REAL;
    }

    valueReal = value;
  } else {
    if (text.size() > 1 && ((text[0] == '0') || (text[0] == '-' && text[1] == '0'))) {
      throw std::runtime_error("Unable to parse");
    }

    Integer value;
    std::istringstream(text) >> value;
    if (type != INTEGER) {
      destructValue();
      type = INTEGER;
    }

    valueInteger = value;
  }
}

void JsonValue::readObject(std::istream& in) {
  char c = readNonWsChar(in);
  JsonValue::Object value;

  if (c != '}') {
    std::string name;

    for (;;) {
      if (c != '"') {
        throw std::runtime_error("Unable to parse");
      }

      name = readStringToken(in);
      c = readNonWsChar(in);

      if (c != ':') {
        throw std::runtime_error("Unable to parse");
      }

      in >> value[name];
      c = readNonWsChar(in);

      if (c == '}') {
        break;
      }

      if (c != ',') {
        throw std::runtime_error("Unable to parse");
      }

      c = readNonWsChar(in);
    }
  }

  if (type != JsonValue::OBJECT) {
    destructValue();
    type = JsonValue::NIL;
    new(valueObject)JsonValue::Object;
    type = JsonValue::OBJECT;
  }

  JsonValue::Object* pObject = reinterpret_cast<JsonValue::Object*>(valueObject);
  pObject->swap(value);
}

void JsonValue::readString(std::istream& in) {
  String value = readStringToken(in);

  if (type != JsonValue::STRING) {
    destructValue();
    type = JsonValue::NIL;
    new(valueString)String;
    type = JsonValue::STRING;
  }

  String* pString = reinterpret_cast<String*>(valueString);
  pString->swap(value);
}

}
