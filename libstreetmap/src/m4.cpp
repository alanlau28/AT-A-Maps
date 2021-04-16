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
    for(int runNum = 0; runNum < 1; ++runNum){
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
           
           for(int j = 0; j< all_paths[depot_index].size()-depots.size(); j+=2){
               //pathTime = findEuclidianDistance(intersectionPosition[depots[i]],intersectionPosition[deliveries[j/2].pickUp]);
               pathTime = computePathTravelTime(all_paths[depot_index][j],turn_penalty);
               //std::cout<<pathTime<<std::endl;

               if(pathTime<currMin&& pathTime!=0){
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
       double currSecondMin;
       int secondNext;
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
           /*if(distr(eng)<0.1 && iteration!= 2*numDeliveries-2 && currSecondMin != INT_MAX){
               next = secondNext;
               currMin = currSecondMin;
           }*/
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
    
    bestRandomPath = bestPath;
    
    }
    

    //auto endTime = std::chrono::high_resolution_clock::now();
    //auto time = std::chrono::duration_cast<std::chrono::milliseconds>(endTime-startTime);
    //std::cout << time.count();
    return bestRandomPath;
    
    
}



