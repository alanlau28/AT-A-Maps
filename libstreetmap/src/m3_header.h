/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m3_header.h
 * Author: tanshiq1
 *
 * Created on March 29, 2021, 3:46 PM
 */

#ifndef M3_HEADER_H
#define M3_HEADER_H
#include <unordered_map>
#include <vector>
#include "StreetsDatabaseAPI.h"


void loadGraph();
extern std::vector<std::unordered_map<StreetSegmentIdx,IntersectionIdx>> adjacent;
extern double max_speed;
extern std::vector<StreetSegmentInfo> segmentInfo;

#endif /* M3_HEADER_H */

