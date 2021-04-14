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
#include <chrono>

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
    //auto startTime = std::chrono::high_resolution_clock::now();
    all_paths = findAllPaths(intersections_dest,turn_penalty);
    //auto endTime = std::chrono::high_resolution_clock::now();
    //auto time = std::chrono::duration_cast<std::chrono::milliseconds>(endTime-startTime);
    //std::cout << time.count();
    
     //std::vector<std::vector<std::vector<StreetSegmentIdx>>> intersections_dest;
    std::vector<std::vector<StreetSegmentIdx>> answer;
    std::vector<double> travel_time_of_paths;
    std::vector<bool> legal;
    std::vector<bool> notVisited;
    for (int i = 0; i < deliveries.size(); i++){
        legal.push_back(false);
        notVisited.push_back(true);
        notVisited.push_back(true);
    }
    double currMin = INT_MAX;
    int depot_start, depot_end;
    int prev;
    int next;
    double pathTime;
    for(int i = 0; i<depots.size();i++){
        int depot_index = (deliveries.size()*2)+i;
        for(int j = 0; j< all_paths[depot_index].size()-depots.size(); j+=2){
            pathTime = computePathTravelTime(all_paths[depot_index][j],turn_penalty);
            //std::cout<<pathTime<<std::endl;
            if(pathTime<currMin&& pathTime!=0){
                currMin = pathTime;
                depot_start = depot_index;
                next = j;
            }
        }     
    }
    answer.push_back(all_paths[depot_start][next]);
    travel_time_of_paths.push_back(currMin);
    
    CourierSubPath path;
    path.start_intersection = depots[depot_start-2*deliveries.size()];
    
    path.end_intersection = deliveries[next/2].pickUp;
    path.subpath = all_paths[depot_start][next];
    subPath.push_back(path);
    
    notVisited[next]=false;   
    legal[next/2]= true; 
    
    prev = next;
    
    for(int iteration = 0; iteration < 2*deliveries.size()-1; iteration++){
        currMin=INT_MAX;
        if(prev%2==0){
            path.start_intersection= deliveries[prev/2].pickUp;
        }else if (prev%2!=0){
            path.start_intersection= deliveries[prev/2].dropOff;
        }
        
        for(int i = 0; i<(all_paths[prev].size()-depots.size()); i++){
            if((legal[i/2]||i%2==0)&&notVisited[i]&&i!=prev){
                pathTime = computePathTravelTime(all_paths[prev][i],turn_penalty);
                if(pathTime<currMin && pathTime!=0){
                     currMin = pathTime;
                     next = i;             
                }
            }        

        }
        if(next%2==0){
            legal[next/2]=true;
            path.end_intersection = deliveries[next/2].pickUp;
        }else if(next%2!=0){
            legal[next/2]=false;
            path.end_intersection = deliveries[next/2].dropOff;
        }
        notVisited[next]=false;
        travel_time_of_paths.push_back(currMin);
        answer.push_back(all_paths[prev][next]);
        path.subpath=all_paths[prev][next];
        prev= next;
        subPath.push_back(path);        
    }
    
    
    
    currMin = INT_MAX;
   
    path.start_intersection= deliveries[next/2].dropOff; 
    for(int i = 2*deliveries.size(); i < all_paths.size(); i++){
        pathTime = computePathTravelTime(all_paths[next][i],turn_penalty);
        if(pathTime< currMin){
            currMin = pathTime;
            depot_end = i;
        }
    }
    answer.push_back(all_paths[next][depot_end]);
    travel_time_of_paths.push_back(currMin);
    path.end_intersection = depots[depot_end-2*deliveries.size()];
    path.subpath = all_paths[next][depot_end];
    subPath.push_back(path);
    
    return subPath;
    
    
}



