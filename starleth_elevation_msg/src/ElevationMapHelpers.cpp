/*
 * ElevationMapHelpers.cpp
 *
 *  Created on: Nov 27, 2013
 *      Author: Péter Fankhauser
 *	 Institute: ETH Zurich, Autonomous Systems Lab
 */

#include "ElevationMapHelpers.hpp"

// StarlETH Elevation Map
#include "TransformationMath.hpp"

// ROS
#include <ros/ros.h>

// Boost
#include <boost/assign.hpp>

namespace starleth_elevation_msg {

const int nDimensions()
{
  return 2;
}

std::map<StorageIndices, std::string> storageIndexNames = boost::assign::map_list_of
    (StorageIndices::Column,  "column_index")
    (StorageIndices::Row, "row_index");

bool isRowMajor(const std_msgs::Float64MultiArray& messageData)
{
  if (messageData.layout.dim[0].label == storageIndexNames[StorageIndices::Column]) return false;
  else if (messageData.layout.dim[0].label == storageIndexNames[StorageIndices::Row]) return true;

  ROS_ERROR("starleth_elevation_msg: isRowMajor() failed because layout label is not set correctly.");
  return false;
}

unsigned int getCols(const std_msgs::Float64MultiArray& messageData)
{
  if (isRowMajor(messageData)) return messageData.layout.dim.at(1).size;
  return messageData.layout.dim.at(0).size;
}

unsigned int getRows(const std_msgs::Float64MultiArray& messageData)
{
  if (isRowMajor(messageData)) return messageData.layout.dim.at(0).size;
  return messageData.layout.dim.at(1).size;
}

unsigned int get1dIndexFrom2dIndex(
    const Eigen::Array2i& index,
    const starleth_elevation_msg::ElevationMap& map)
{
  unsigned int n;

  if(!isRowMajor(map.elevation)) n = map.elevation.layout.data_offset + index(1) * map.elevation.layout.dim[1].stride + index(0);
  else n = map.elevation.layout.data_offset + index(0) * map.elevation.layout.dim[1].stride + index(1);

  return n;
}

//Eigen::Array2i get2dIndexFrom1dIndex(
//    unsigned int n, const starleth_elevation_msg::ElevationMap& map)
//{
//  Eigen::Vector2i index;
//  index(1) = n - map.elevation.layout.data_offset % map.elevation.layout.dim[1].stride;
//  index(0) = (int)((n - map.elevation.layout.data_offset - index(1)) / map.elevation.layout.dim[1].stride);
//  return index;
//}

bool getPositionFromIndex(Eigen::Vector2d& position,
                          const Eigen::Array2i& index,
                          const starleth_elevation_msg::ElevationMap& map)
{
  Array2d mapLength(map.lengthInX, map.lengthInY);
  return getPositionFromIndex(position, index, mapLength, map.resolution);
}

bool getIndexFromPosition(Eigen::Array2i& index,
                          const Eigen::Vector2d& position,
                          const starleth_elevation_msg::ElevationMap& map)
{
  Array2d mapLength(map.lengthInX, map.lengthInY);
  return getIndexFromPosition(index, position, mapLength, map.resolution);
}

bool getColorVectorFromColorValue(Eigen::Vector3i& colorVector,
                                  const unsigned long& colorValue)
{
  colorVector(0) = (colorValue >> 16) & 0x0000ff;
  colorVector(1) = (colorValue >> 8) & 0x0000ff;
  colorVector(2) =  colorValue & 0x0000ff;
  return true;
}

bool getColorVectorFromColorValue(Eigen::Vector3f& colorVector,
                                  const unsigned long& colorValue)
{
  Vector3i tempColorVector;
  getColorVectorFromColorValue(tempColorVector, colorValue);
  colorVector = ((tempColorVector.cast<float>()).array() / 255.0).matrix();
  return true;
}

bool getColorValueFromColorVector(unsigned long& colorValue,
                                  const Eigen::Vector3i& colorVector)
{
  colorValue = ((int)colorVector(0)) << 16 | ((int)colorVector(1)) << 8 | ((int)colorVector(2));
  return true;
}

} // namespace
