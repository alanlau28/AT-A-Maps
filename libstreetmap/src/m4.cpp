/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include <vector>
#include "m4.h"
#include "m3_header.h"




std::vector<CourierSubPath> greedyTravelingCourier(
		            const std::vector<DeliveryInf>& deliveries,
	       	        const std::vector<int>& depots, 
		            const float turn_penalty){
    std::vector<std::vector<std::vector<StreetSegmentIdx>>> intersections_dest;
    std::vector<CourierSubPath> answer;
    double currMin = INT_MAX;
    int depot_start;
    for(int i = 0; i<depots.size();i++){
        int depot_index = (deliveries.size()*2)+i;
        for(int j = 0; j< intersections_dest[depot_index].size(); j+=2){
            if((computePathTravelTime(intersections_dest[depot_index[j]],turn_penalty)<currMin){
                currMin = (computePathTravelTime(intersections_dest[depot_index[j]],turn_penalty);
                //depot_start = 
            }
        }
        
        
    }
    
}