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

#ifndef LANELET2_EXTENSION__REGULATORY_ELEMENTS__BUS_STOP_HPP_
#define LANELET2_EXTENSION__REGULATORY_ELEMENTS__BUS_STOP_HPP_

// NOLINTBEGIN(readability-identifier-naming)

#include <lanelet2_core/primitives/BasicRegulatoryElements.h>
#include <lanelet2_core/primitives/Lanelet.h>

#include <memory>
#include <vector>

namespace lanelet::autoware
{
class BusStop : public lanelet::RegulatoryElement
{
public:
  using Ptr = std::shared_ptr<BusStop>;
  static constexpr char RuleName[] = "bus_stop";

  //! Directly construct a stop line from its required rule parameters.
  //! Might modify the input data in oder to get correct tags.
  static Ptr make(
    Id id, const AttributeMap & attributes, const Polygons3d & busStops,
    const LineString3d & stopLine)
  {
    return Ptr{new BusStop(id, attributes, busStops, stopLine)};
  }

  /**
   * @brief get the relevant bus_stops
   * @return bus_stops
   */
  [[nodiscard]] ConstPolygons3d busStops() const;
  [[nodiscard]] Polygons3d busStops();

  /**
   * @brief add a new bus stop
   * @param primitive bus stop to add
   */
  void addBusStop(const Polygon3d & primitive);

  /**
   * @brief remove a bus stop
   * @param primitive the primitive
   * @return true if the bus stop existed and was removed
   */
  bool removeBusStop(const Polygon3d & primitive);

  /**
   * @brief get the stop line for the bus stop
   * @return the stop line as LineString
   */
  [[nodiscard]] ConstLineString3d stopLine() const;
  [[nodiscard]] LineString3d stopLine();

  /**
   * @brief set a new stop line, overwrite the old one
   * @param stopLine new stop line
   */
  void setStopLine(const LineString3d & stopLine);

  //! Deletes the stop line
  void removeStopLine();

private:
  // the following lines are required so that lanelet2 can create this object
  // when loading a map with this regulatory element
  friend class lanelet::RegisterRegulatoryElement<BusStop>;
  BusStop(
    Id id, const AttributeMap & attributes, const Polygons3d & busStops,
    const LineString3d & stopLine);
  explicit BusStop(const lanelet::RegulatoryElementDataPtr & data);
};
static lanelet::RegisterRegulatoryElement<BusStop> regBusStop;

}  // namespace lanelet::autoware

// NOLINTEND(readability-identifier-naming)

#endif  // LANELET2_EXTENSION__REGULATORY_ELEMENTS__BUS_STOP_HPP_
