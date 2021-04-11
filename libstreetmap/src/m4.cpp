/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
#include "m3.h"
#include "m3_header.h"
#include "m2_header.h"
#include "m1.h"
#include <iostream>
#include <climits>
#include <unordered_map>
#include <queue>
#include <algorithm>
#include <math.h>
#include "m4.h"


std::vector<CourierSubPath> travelingCourier(
		            const std::vector<DeliveryInf>& deliveries,
                            const std::vector<int>& depots, 
		            const float turn_penalty){
    
    std::vector<IntersectionIdx> intersections_dest;
    std::vector<std::vector<std::vector<StreetSegmentIdx>>> all_paths;
    std::vector<CourierSubPath> subPath;
            
    for(int i = 0; i < deliveries.size();i++){
        intersections_dest.push_back(deliveries[i].pickUp);
        intersections_dest.push_back(deliveries[i].dropOff);
    }
    
    for(int i = 0;i < depots.size();i++){
        intersections_dest.push_back(depots[i]);
    }
    
    all_paths = findAllPaths(intersections_dest,turn_penalty);

    return subPath;
    
}