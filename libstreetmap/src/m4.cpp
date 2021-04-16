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
#include "m4_header.h"
#include <chrono>
#include <random>


std::vector<CourierSubPath> travelingCourier(
		            const std::vector<DeliveryInf>& deliveries,
                            const std::vector<int>& depots, 
		            const float turn_penalty){
    const int numDeliveries = deliveries.size();
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<float> distr(0.0, 1.0);
    
    std::vector<IntersectionIdx> intersections_dest;
    std::vector<std::vector<std::vector<StreetSegmentIdx>>> all_paths;
    std::vector<CourierSubPath> subPath;
   
            
    for(int i = 0; i < numDeliveries;i++){
        intersections_dest.push_back(deliveries[i].pickUp);
        intersections_dest.push_back(deliveries[i].dropOff);
    }
    
    for(int i = 0;i < depots.size();i++){
        intersections_dest.push_back(depots[i]);
    }
    
    all_paths = findAllPaths(intersections_dest,turn_penalty);
    
    double bestMinRandom = INT_MAX;
    
    std::vector<CourierSubPath> bestRandomPath;

    //auto startTime = std::chrono::high_resolution_clock::now();
    for(int runNum = 0; runNum < 8; ++runNum){

    int depotit = 0;
    
    std::vector<CourierSubPath> bestPath;
    double totalMin = INT_MAX;
    while(depotit<depots.size()){
        
       //std::vector<std::vector<StreetSegmentIdx>> answer;
       std::vector<CourierSubPath> currPath;
       //std::vector<double> travel_time_of_paths;
       std::vector<bool> legal;
       std::vector<bool> notVisited;
       for (int i = 0; i < numDeliveries; i++){
           legal.push_back(false);
           notVisited.push_back(true);
           notVisited.push_back(true);
       }
       double currMin = INT_MAX;
       

       int depot_start = (numDeliveries*2);
       int depot_end = (numDeliveries*2);
       int prev;
       int next = 0;
       double pathTime, pathDistance;

       //for(int i = 0; i<depots.size();i++){
       //while(depotit<depots.size()){
           double total = 0;
           int depot_index = (numDeliveries*2)+depotit;
           double currSecondMin = INT_MAX;
           int secondNext;
           for(int j = 0; j< all_paths[depot_index].size()-depots.size(); j+=2){
               //pathTime = findEuclidianDistance(intersectionPosition[depots[i]],intersectionPosition[deliveries[j/2].pickUp]);
               pathTime = computePathTravelTime(all_paths[depot_index][j],turn_penalty);

               if(pathTime<currMin&& pathTime!=0){
                   if(j!=0){
                   currSecondMin = currMin;
                       secondNext = next;
                   }
                   currMin = pathTime;
                   depot_start = depot_index;
                   
                   next = j;
               }
           }
           if(currMin==INT_MAX){
                //std::cout<<"ewfwefewefwfewfewfwf"<<std::endl;
               depotit+=1;
               continue;
           }
           
           if(currSecondMin!= INT_MAX && (runNum ==1 || runNum %4 ==0)){
               next = secondNext;
               currMin = currSecondMin;
               //neverRanSecond = false;
           }
       //}
       //answer.push_back(all_paths[depot_start][next]);
       //travel_time_of_paths.push_back(currMin);
           total+=currMin;
       CourierSubPath path;
       path.start_intersection = depots[depot_start-2*numDeliveries];

       path.end_intersection = deliveries[next/2].pickUp;
       path.subpath = all_paths[depot_start][next];
       currPath.push_back(path);

       notVisited[next]=false;   
       legal[next/2]= true; 

       prev = next;
       
       for(int iteration = 0; iteration < 2*numDeliveries-1; iteration++){
           currMin=INT_MAX;
           currSecondMin = INT_MAX;
           
           if(prev%2==0){
               path.start_intersection= deliveries[prev/2].pickUp;
           }else if (prev%2!=0){
               path.start_intersection= deliveries[prev/2].dropOff;
           }

           for(int i = 0; i<(all_paths[prev].size()-depots.size()); i++){
               if((legal[i/2]||i%2==0)&&notVisited[i]&&i!=prev){
                   pathTime = computePathTravelTime(all_paths[prev][i],turn_penalty);
                   if(prev%2==0){
                       if(i%2==0){
                           pathDistance = findEuclidianDistance(intersectionPosition[deliveries[prev/2].pickUp], intersectionPosition[deliveries[i/2].pickUp]);
                       }else if(i%2!=0){
                           pathDistance = findEuclidianDistance(intersectionPosition[deliveries[prev/2].pickUp], intersectionPosition[deliveries[i/2].dropOff]);
                       }
                   }else if (prev%2!=0){
                       if(i%2==0){
                           pathDistance = findEuclidianDistance(intersectionPosition[deliveries[prev/2].dropOff], intersectionPosition[deliveries[i/2].pickUp]);
                       }else if(i%2!=0){
                           pathDistance = findEuclidianDistance(intersectionPosition[deliveries[prev/2].dropOff], intersectionPosition[deliveries[i/2].dropOff]);
                       }
                   }
                   
                   if((pathTime<currMin && pathTime!=0) ||(pathTime==0 && pathDistance == 0)){
                       currSecondMin = currMin;
                       secondNext = next;
                        currMin = pathTime;
                        next = i;             
                   }
               }       

           }
           if(distr(eng)<0.1 && iteration!= 2*numDeliveries-2 && runNum > 1&& currSecondMin != INT_MAX){
               next = secondNext;
               currMin = currSecondMin;
           }
           if(next%2==0){
               legal[next/2]=true;
               path.end_intersection = deliveries[next/2].pickUp;
           }else if(next%2!=0){
               legal[next/2]=false;
               path.end_intersection = deliveries[next/2].dropOff;
           }
           notVisited[next]=false;
           //travel_time_of_paths.push_back(currMin);
           //answer.push_back(all_paths[prev][next]);
           path.subpath=all_paths[prev][next];
           prev= next;
           currPath.push_back(path);
           total+=currMin;
       }


        currMin = INT_MAX;

       path.start_intersection= deliveries[next/2].dropOff; 
       for(int i = 2*numDeliveries; i < all_paths.size(); i++){
           pathTime = computePathTravelTime(all_paths[next][i],turn_penalty);
           //pathTime = findEuclidianDistance(intersectionPosition[deliveries[next/2].dropOff], intersectionPosition[depots[i-(2*numDeliveries)]]);
           if(pathTime< currMin && pathTime!=0){
               currMin = pathTime;
               depot_end = i;
           }
       }
       total+=currMin;
       //answer.push_back(all_paths[next][depot_end]);
       //travel_time_of_paths.push_back(currMin);
       path.end_intersection = depots[depot_end-2*numDeliveries];
       path.subpath = all_paths[next][depot_end];
       currPath.push_back(path);
       if(total<totalMin){
           totalMin = total;
           bestPath = currPath;  
           //return bestPath;
       }
       
       currPath.clear();
       depotit+=1;
       
       for(int i = 0; i<notVisited.size();++i){
            if(notVisited[i]){
                bestPath.clear();
                return bestPath;
            }
         }
    
    }
    if(totalMin<bestMinRandom){
        bestRandomPath = bestPath;
        bestMinRandom = totalMin;
    }
    
    
    }
    
     auto startTime = std::chrono::high_resolution_clock::now();

    
    std::vector<IntersectionIdx> order = generate_intersection_order(bestRandomPath);
    std::vector<CourierSubPath> two_opt;
    two_opt.resize(bestRandomPath.size());
    two_opt = two_opt_algorithm_order(bestRandomPath, order, deliveries, all_paths, intersections_dest);
    
     auto endTime = std::chrono::high_resolution_clock::now();
     
     auto time = std::chrono::duration_cast<std::chrono::milliseconds>(endTime-startTime);
     std::cout << time.count() << std::endl;
    
    
    return two_opt;

}






//generates two opt swap of a given intersection order
//make sure index1 is less than index 2
std::vector<IntersectionIdx> two_opt_swap_order (std::vector<IntersectionIdx>& order,
                                                 int index1, int index2) {
    std::vector<IntersectionIdx> new_order(order);
    std::reverse(new_order.begin() + index1, new_order.begin() + index2);
    
    return new_order;
}

std::vector<CourierSubPath> generate_new_courier (std::vector<IntersectionIdx>& order,
                                                  std::vector<std::vector<std::vector<StreetSegmentIdx>>>& all_paths,
                                                  std::vector<IntersectionIdx>& intersections_dest) {
    std::vector<CourierSubPath> new_path;
    
    for (int i = 0; i < order.size() - 1; i++) {
        CourierSubPath new_node;
        new_node.start_intersection = order[i];
        new_node.end_intersection = order[i+1];
        
        int start = 0;
        int end = 0;
        
        for (int j = 0; j < intersections_dest.size(); j++) {
            if (intersections_dest[j] == order[i]) start = j;
        }
        for (int j = 0; j < intersections_dest.size(); j++) {
            if (intersections_dest[j] == order[i+1]) end = j;
        }
        
        new_node.subpath = all_paths[start][end];
        
        new_path.push_back(new_node);
        
    }
    
    return new_path;
}

//generates a vector of intersection ids from a legal path, 
std::vector<IntersectionIdx> generate_intersection_order (std::vector<CourierSubPath>& path) {
    
    std::vector<IntersectionIdx> result;
    
    for (int i = 0; i < path.size(); i++) {
        result.push_back(path[i].start_intersection);
    }
    
    result.push_back(path.back().end_intersection);
    
    //std::cout << result.back() << " " << path[path.size()].end_intersection << std::endl;
    
    return result;
}

//check that a given intersection order is legal
bool order_is_legal (std::vector<IntersectionIdx>& order, std::vector<DeliveryInf> deliveries) {
    
    std::vector<IntersectionIdx> pickups;
    std::vector<IntersectionIdx> dropoffs;
    std::vector<bool> delivered;
    
    pickups.resize(deliveries.size());
    dropoffs.resize(deliveries.size());
    delivered.resize(deliveries.size());
    
    for (int i = 0; i < deliveries.size(); i++) {
        dropoffs[i] = deliveries[i].dropOff;
        pickups[i] = deliveries[i].pickUp;
        delivered[i] = false;
    }
    
    
    for (int i = 0; i < order.size(); i++) {
        
        for (int j = 0; j < deliveries.size(); j++) {
            //if a package is picked up, set value to 0
            if (order[i] == pickups[j]) {
                pickups[j] = 0;
            }
            
        }
        
        for (int j = 0; j < deliveries.size(); j++) {
            if (order[i] == dropoffs[j]) {
                if (pickups[j] == 0) {
                    dropoffs[j] = 0;
                    delivered[j] = true;
                } else if (pickups[j] == 0 && dropoffs[j] != 0) {
                    return false;
                }
            }
        }
        
    }
    
    for (int i = 0; i < delivered.size(); i++) {
        if (!delivered[i]) return false;
    }

    return true;
}

std::vector<CourierSubPath> two_opt_algorithm_order (std::vector<CourierSubPath>& path,
                                                    std::vector<IntersectionIdx>& order,
                                                    std::vector<DeliveryInf> deliveries,
                                                    std::vector<std::vector<std::vector<StreetSegmentIdx>>>& all_paths,
                                                    std::vector<IntersectionIdx>& intersections_dest) {
    
    //no two opt swap can be done if path size is less than 4
    if (path.size() < 4) return path;
    
    double initial_time = 0;
    
    double old_time = 0;
    

    for (int l = 0; l < path.size(); l++) {
        initial_time += computePathTravelTime(path[l].subpath, 15.00);
    }
    
    old_time = initial_time;
    
    std::vector<CourierSubPath> final_path;
    final_path.resize(path.size());
    
    for (int i = 1; i < order.size() - 1; i++) {
        for (int j = i + 1; j < order.size() - 1; j++) {
            if (j > i && j - i > 1) {
                std::vector<IntersectionIdx> two_opt_order = two_opt_swap_order(order, i, j);

                if (!two_opt_order.empty() && order_is_legal(two_opt_order, deliveries)) {

                    std::vector<CourierSubPath> two_opt_path = generate_new_courier (two_opt_order, all_paths, intersections_dest);
                    double new_time = 0;
                    for (int l = 0; l < path.size(); l++) {
                        new_time += computePathTravelTime(two_opt_path[l].subpath, 15.00);
                    }
                    
                    if (new_time < initial_time && !two_opt_path.empty()) {
                        
                        initial_time = new_time;

                        final_path = two_opt_path;
                        
                    }
                } 
            }
           
        }
    }
    
    
    
    if (!final_path.empty() && initial_time < old_time) {
        return final_path;
    } else return path;
}

