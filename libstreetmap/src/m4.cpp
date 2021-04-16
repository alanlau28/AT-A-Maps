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

struct PickOrDrop {
    IntersectionIdx ID;
    int isPickup; //0 if dropoff, 1 if pickup, -1 if depot
    IntersectionIdx correspondingPickOrDrop; //corresponding pickup or dropoff for ID
    bool visited;
    
    PickOrDrop(IntersectionIdx id, int pickUp, IntersectionIdx corresponding, bool visit) 
        : ID(id), isPickup(pickUp), correspondingPickOrDrop(corresponding), visited(visit) {}
};


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
    
    
    std::vector<CourierSubPath> finalResult;
    finalResult.resize(subPath.size());
    std::vector<IntersectionIdx> order = generate_intersection_order(subPath);
    finalResult = two_opt_algorithm_order(subPath, order, deliveries, all_paths, intersections_dest);
    //finalResult = two_opt_algorithm (subPath, deliveries, all_paths, intersections_dest);

        
//   for (int k = 0; k < subPath.size(); k++) {                   
//       if (subPath[k].start_intersection != finalResult[k].start_intersection ||
//            subPath[k].end_intersection != finalResult[k].end_intersection    ) {
//           std::cout << subPath[k].start_intersection << " " << subPath[k].end_intersection <<
//            " -> " << finalResult[k].start_intersection << " " << finalResult[k].end_intersection << std::endl;
//       }
//
//    }
//    std::cout << std::endl;
 
    return finalResult;

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
                    //std::cout << i << " " << j << std::endl;
                    std::vector<CourierSubPath> two_opt_path = generate_new_courier (two_opt_order, all_paths, intersections_dest);
//                    std::cout << std::endl;
                    double new_time = 0;
                    for (int l = 0; l < path.size(); l++) {
                        new_time += computePathTravelTime(two_opt_path[l].subpath, 15.00);
                    }
                    
                    
                   
                    
                    if (new_time < initial_time && !two_opt_path.empty()) {
                        
                        initial_time = new_time;

                        final_path = two_opt_path;
                        
//                        std::cout << final_path[path.size()].end_intersection << " "
//                                  << path[path.size()].end_intersection << std::endl;
                    }
                } 
            }
           
        }
    }
    
    
    
    if (!final_path.empty() && initial_time < old_time) {
        //std::cout << initial_time << " " << old_time << std::endl;
        //std::cout << final_path[path.size()].end_intersection << " -> "
        //          << path[path.size()].end_intersection << std::endl;
        return final_path;
    } else return path;
}



//runs two opt algorithm on the array of courier subpath given from greedy algorithm
std::vector<CourierSubPath> two_opt_algorithm (const std::vector<CourierSubPath>& path,
                                               const std::vector<DeliveryInf>& info,
                                               std::vector<std::vector<std::vector<StreetSegmentIdx>>>& all_paths,
                                               std::vector<IntersectionIdx>& intersections_dest) {
    
    //no two opt swap can be done for a path with less than 4 subpaths
    if (path.size() < 4) return path;
    
    double initial_time = 0;
    //find initial length of path
    for (int l = 0; l < path.size(); l++) {
        initial_time += computePathTravelTime(path[l].subpath, 15.00);
    }
    
    
    std::vector<CourierSubPath> final_path;
    final_path.resize(path.size());

    for (int i = 1; i < path.size() - 1; i++) {
        for (int j = i + 1; j < path.size() - 1; j++) {
            //do the two opt swap, save path in new vector
            if (j - i > 1) {
                std::vector<CourierSubPath> two_opt_path = two_opt_swap(path, i, j, all_paths, intersections_dest);
                
                
                double new_time = 0;
            
                //find length of new path
                for (int k = 0; k < path.size(); k++) {
                    new_time += computePathTravelTime(two_opt_path[k].subpath, 15.00);
                }
                

                //if it's smaller update old time to new, and keep going
                if (new_time < initial_time) {
                
                    std::vector<PickOrDrop> order = generate_delivery_order(path, info);
   
                    if (path_is_legal(order) && !order.empty() && !two_opt_path.empty()) {

                        initial_time = new_time;
                        
                        //final_path.clear();
                        final_path = two_opt_path;
                        
                    }
                
                }
            }
                      
        }    
    }
    
    if (final_path.size() == path.size()) {
        return final_path;
    } else return path;
}

//does a two opt path swap given the two target nodes to swap
//make sure index1 is smaller than index2
std::vector<CourierSubPath> two_opt_swap (const std::vector<CourierSubPath>& path, 
                                          const int index1,
                                          const int index2,
                                          std::vector<std::vector<std::vector<StreetSegmentIdx>>>& all_paths,
                                          std::vector<IntersectionIdx>& intersections_dest) {
    
    std::vector<CourierSubPath> swapped_path;
    swapped_path.resize(path.size());
    
    //a swap with a difference of 1 means no swap
    if (index2 - index1 < 2) return path;
    
    //add the path in order until index1 - 1
    for (int i = 0; i < index1; i++) {
        swapped_path[i] = path[i];
    }
    
    
    int intersection1 = 0;
    int intersection2 = 0;
    //index for keeping track of where to put reversed subpath
    int index = index1;
    
    //add from index1 to index2 from path in reverse order to swapped_path
    for (int j = index2; j > index1; j--) {
        CourierSubPath subPath;
        subPath.start_intersection = path[j].end_intersection;
        subPath.end_intersection = path[j].start_intersection;
        
         for (int i = 0; i < intersections_dest.size(); i++) {
            if (intersections_dest[i] == subPath.start_intersection) intersection1 = i;
        }
    
        for (int i = 0; i < intersections_dest.size(); i++) {
            if (intersections_dest[i] == subPath.end_intersection) intersection2 = i;
        }
        
        subPath.subpath = all_paths[intersection1][intersection2];
        
        swapped_path[index] = subPath;
                
        index++;
    }
    
    swapped_path[index1].start_intersection = path[index1 - 1].end_intersection;
    //swapped_path[index1].end_intersection = path[index1 - 1].end_intersection;
    
    
    
    for (int i = 0; i < intersections_dest.size(); i++) {
        if (intersections_dest[i] == swapped_path[index1].start_intersection) intersection1 = i;
    }
    
    for (int i = 0; i < intersections_dest.size(); i++) {
        if (intersections_dest[i] == swapped_path[index1].end_intersection) intersection2 = i;
    }

    swapped_path[index1].subpath = all_paths[intersection1][intersection2];


    swapped_path[index2].start_intersection = path[index1 + 1].start_intersection;
    swapped_path[index2].end_intersection = path[index2 + 1].start_intersection;
    
    for (int i = 0; i < intersections_dest.size(); i++) {
        if (intersections_dest[i] == swapped_path[index2].start_intersection) intersection1 = i;
    }
    
    for (int i = 0; i < intersections_dest.size(); i++) {
        if (intersections_dest[i] == swapped_path[index2].end_intersection) intersection2 = i;
    }
    
    swapped_path[index2].subpath = all_paths[intersection1][intersection2];

    
    //add the rest of the path in order
     for (int i = index2 + 1; i < path.size(); i++) {
        swapped_path[i] = path[i];
    }
    

    
    return swapped_path;
}

//checks if a given order of pickups and dropoffs is legal
bool path_is_legal (std::vector<PickOrDrop>& deliveryOrder) {
       
    int packages_delivered = 0;
    //if (deliveryOrder.empty()) return false;
    for (int i = 0; i < deliveryOrder.size(); i++) {
        if (deliveryOrder[i].isPickup == 1) {
            packages_delivered++;
        } else if (deliveryOrder[i].isPickup == 0) {
            packages_delivered--;
        }
    }
    
    for (int i = 0; i < deliveryOrder.size(); i++) {
        deliveryOrder[i].visited = true;
        
        //if it's a dropoff
        if (deliveryOrder[i].isPickup == 0) {
            IntersectionIdx corresponding_pickup = deliveryOrder[i].correspondingPickOrDrop;
            for (int j = 0; j < i; j++) {
                if (deliveryOrder[j].ID == corresponding_pickup && deliveryOrder[j].visited == false) {
                    return false;
                }
            }
        }
    }
    

    if (packages_delivered != 0) return false;
    
    return true;
}

std::vector<PickOrDrop> generate_delivery_order(std::vector<CourierSubPath> path, 
                                                std::vector<DeliveryInf> deliveries) {
    
    std::vector<PickOrDrop> result;
    
    for (int i = 0; i < path.size(); i++) {
        IntersectionIdx start = path[i].start_intersection;
        
        bool found = false;
        
        int dropoff = -1;
        int pickup = -1;
        
        for (int j = 0; j < deliveries.size(); j++) {
            if (deliveries[j].pickUp == start) {
                dropoff = deliveries[j].dropOff;
                found = true;
                break;
            
            } else if (deliveries[j].dropOff == start) {
                pickup = deliveries[j].pickUp;
                found = true;
                break;
            } 
            
        }
        
        //if it's a dropoff, 
        if (pickup != -1) {
            result.push_back(PickOrDrop(start, 0, pickup, false));
            pickup = -1;
        } else if (dropoff != -1) {
            result.push_back(PickOrDrop(start, 1, dropoff, false));
            dropoff = -1;
        }
        
        
        if (!found) {
            //else depot
             result.push_back(PickOrDrop(start, -1, 0, false));
        }
        
        found = false;
    }
    
    //std::cout << path.size() << " " << result.size() << std::endl;
    
    return result;
}



