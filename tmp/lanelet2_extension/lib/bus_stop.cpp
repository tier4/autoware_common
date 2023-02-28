// Copyright 2015-2019 Autoware Foundation. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Authors: Ryohsuke Mitsudome

// NOLINTBEGIN(readability-identifier-naming)

#include "lanelet2_extension/regulatory_elements/bus_stop.hpp"

#include <boost/variant.hpp>

#include <lanelet2_core/primitives/RegulatoryElement.h>

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

namespace lanelet::autoware
{
namespace
{
template <typename T>
bool findAndErase(const T & primitive, RuleParameters * member)
{
  if (member == nullptr) {
    std::cerr << __FUNCTION__ << ": member is null pointer";
    return false;
  }
  auto it = std::find(member->begin(), member->end(), RuleParameter(primitive));
  if (it == member->end()) {
    return false;
  }
  member->erase(it);
  return true;
}

template <typename T>
RuleParameters toRuleParameters(const std::vector<T> & primitives)
{
  auto cast_func = [](const auto & elem) { return static_cast<RuleParameter>(elem); };
  return utils::transform(primitives, cast_func);
}

// template <>
// RuleParameters toRuleParameters(const std::vector<Polygon3d>& primitives)
// {
//   auto cast_func = [](const auto& elem) { return elem.asRuleParameter(); };
//   return utils::transform(primitives, cast_func);
// }

Polygons3d getPoly(const RuleParameterMap & paramsMap, RoleName role)
{
  auto params = paramsMap.find(role);
  if (params == paramsMap.end()) {
    return {};
  }

  Polygons3d result;
  for (auto & param : params->second) {
    auto p = boost::get<Polygon3d>(&param);
    if (p != nullptr) {
      result.push_back(*p);
    }
  }
  return result;
}

ConstPolygons3d getConstPoly(const RuleParameterMap & params, RoleName role)
{
  auto cast_func = [](auto & poly) { return static_cast<ConstPolygon3d>(poly); };
  return utils::transform(getPoly(params, role), cast_func);
}

RegulatoryElementDataPtr constructBusStopData(
  Id id, const AttributeMap & attributes, const Polygons3d & busStops,
  const LineString3d & stopLine)
{
  RuleParameterMap rpm = {{RoleNameString::Refers, toRuleParameters(busStops)}};

  RuleParameters rule_parameters = {stopLine};
  rpm.insert(std::make_pair(RoleNameString::RefLine, rule_parameters));

  auto data = std::make_shared<RegulatoryElementData>(id, rpm, attributes);
  data->attributes[AttributeName::Type] = AttributeValueString::RegulatoryElement;
  data->attributes[AttributeName::Subtype] = "bus_stop";
  return data;
}
}  // namespace

BusStop::BusStop(const RegulatoryElementDataPtr & data) : RegulatoryElement(data)
{
  if (getConstPoly(data->parameters, RoleName::Refers).empty()) {
    throw InvalidInputError("No bus stop defined!");
  }
  if (getParameters<ConstLineString3d>(RoleName::RefLine).size() != 1) {
    throw InvalidInputError("There must be exactly one stopline defined!");
  }
}

BusStop::BusStop(
  Id id, const AttributeMap & attributes, const Polygons3d & busStops,
  const LineString3d & stopLine)
: BusStop(constructBusStopData(id, attributes, busStops, stopLine))
{
}

ConstPolygons3d BusStop::busStops() const { return getConstPoly(parameters(), RoleName::Refers); }
Polygons3d BusStop::busStops() { return getPoly(parameters(), RoleName::Refers); }

void BusStop::addBusStop(const Polygon3d & primitive)
{
  parameters()["bus_stop"].emplace_back(primitive);
}

bool BusStop::removeBusStop(const Polygon3d & primitive)
{
  return findAndErase(primitive, &parameters().find("bus_stop")->second);
}

ConstLineString3d BusStop::stopLine() const
{
  return getParameters<ConstLineString3d>(RoleName::RefLine).front();
}

LineString3d BusStop::stopLine() { return getParameters<LineString3d>(RoleName::RefLine).front(); }

void BusStop::setStopLine(const LineString3d & stopLine)
{
  parameters()[RoleName::RefLine] = {stopLine};
}

void BusStop::removeStopLine() { parameters()[RoleName::RefLine] = {}; }

}  // namespace lanelet::autoware

// NOLINTEND(readability-identifier-naming)
