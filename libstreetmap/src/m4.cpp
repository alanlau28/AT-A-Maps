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
#include <algorithm>






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
    std::vector<PD> bestRandomOrder;

    //auto startTime = std::chrono::high_resolution_clock::now();
    for(int runNum = 0; runNum < 4; ++runNum){

        int depotit = 0;

        std::vector<CourierSubPath> bestPath;
        std::vector<PD> bestOrder;
        double totalMin = INT_MAX;
        while(depotit<depots.size()){

           //std::vector<std::vector<StreetSegmentIdx>> answer;
           std::vector<CourierSubPath> currPath;
           std::vector<PD> currOrder;
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
           int next;
           double pathTime, pathDistance;

           
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
           CourierSubPath path1;
           path1.start_intersection = depots[depot_start-2*numDeliveries];

           path1.end_intersection = deliveries[next/2].pickUp;
           path1.subpath = all_paths[depot_start][next];
           currPath.push_back(path1);

           notVisited[next]=false;   
           legal[next/2]= true; 

           prev = next;

           for(int iteration = 0; iteration < 2*numDeliveries-1; iteration++){
               PD temp(prev);
               currOrder.push_back(temp);
               CourierSubPath path2;
               currMin=INT_MAX;
               currSecondMin = INT_MAX;

               if(prev%2==0){
                   path2.start_intersection= deliveries[prev/2].pickUp;
               }else if (prev%2!=0){
                   path2.start_intersection= deliveries[prev/2].dropOff;
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
                            /*if(next==3){
                                if(next%2==0){
                                    //std::cout<<deliveries[next/2].pickUp<<"  "<<next<<"  "<<notVisited[2]<<std::endl;
                                 }else if(next%2!=0){
                                    std::cout<<deliveries[next/2].dropOff<<"  "<<next<<"  "<<notVisited[2]<<std::endl;
                                 }
                           
                           
                                std::cout<<"\n";
                             }*/
                       }
                   }       

               }
               if(distr(eng)<0.1 && iteration!= 2*numDeliveries-2 && runNum > 1&& currSecondMin != INT_MAX){
                   next = secondNext;
                   currMin = currSecondMin;
               }
               if(next%2==0){
                   legal[next/2]=true;
                   path2.end_intersection = deliveries[next/2].pickUp;
               }else if(next%2!=0){
                   legal[next/2]=false;
                   //std::cout<<deliveries[next/2].dropOff<<"  "<<next<<"  "<<notVisited[2]<<std::endl;
                   path2.end_intersection = deliveries[next/2].dropOff;
               }
               notVisited[next]=false;
               //travel_time_of_paths.push_back(currMin);
               //answer.push_back(all_paths[prev][next]);
               path2.subpath=all_paths[prev][next];
               
               currPath.push_back(path2);
               prev= next;
               total+=currMin;
           }


            currMin = INT_MAX;
            CourierSubPath lastPath;
            PD lastStart(next);
            currOrder.push_back(lastStart);

           lastPath.start_intersection= deliveries[next/2].dropOff; 
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
           lastPath.end_intersection = depots[depot_end-2*numDeliveries];
           lastPath.subpath = all_paths[next][depot_end];
           currPath.push_back(lastPath);
           
           if(total<totalMin){
               totalMin = total;
               bestPath = currPath;  
               bestOrder = currOrder;
               //return bestPath;
           }

           currPath.clear();
           currOrder.clear();
           depotit+=1;

           for(int i = 0; i<notVisited.size();++i){
                if(notVisited[i]){
                    bestPath.clear();
                    return bestPath;
                }
             }
           
           legal.clear();
           notVisited.clear();
        }
        if(totalMin<bestMinRandom){
            bestRandomPath = bestPath;
            bestMinRandom = totalMin;
            bestRandomOrder = bestOrder;
        }


    }


    std::vector<CourierSubPath> newCourier;// =  generate_new_courier(bestRandomOrder, all_paths, intersections_dest, deliveries, depots, turn_penalty);
    
    //std::cout<<test<<std::endl;


    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>> (endTime - startTime);
    //std::cout << duration.count() << std::endl;
    
//    bool test = order_is_legal(two_opt_order, intersections_dest);
//    std::cout<<test<<std::endl;
    
//    for (int i = 0; i < bestRandomOrder.size(); i++) {
//        std::cout << bestRandomOrder[i].index << " " << two_opt_order[i].index << std::endl;
//    }
//    std::cout << std::endl;
//    for (int i = 0; i < bestRandomOrder.size(); i++) {
//        std::cout << two_opt_order[i].index << " ";
//    }
//    
     newCourier = simulatedAnnealing(bestRandomPath,bestRandomOrder, intersections_dest, all_paths,deliveries,depots,turn_penalty);

     return newCourier;


}




//generates the three subpaths that result from deleting two connections
//make sure index1 is smaller than index2
std::vector<std::vector<PD>> swap_subpaths (std::vector<PD> order, int index1, int index2) {
    
    std::vector<PD> first = {order.begin(), order.begin() + index1};
    std::vector<PD> second = {order.begin() + index1, order.begin() + index2};
    std::vector<PD> third = {order.begin() + index2, order.end() };
    
    std::vector<std::vector<PD>> result = {first, second, third};
    
    return result;
}




//generates two opt swap of a given intersection order
//make sure index1 is less than index 2
//std::vector<std::vector<IntersectionIdx>> two_opt_swap_order (std::vector<IntersectionIdx>& order,
//                                                 int index1, int index2) {
//    std::vector<IntersectionIdx> new_order(order);
//    std::reverse(new_order.begin() + index1, new_order.begin() + index2);
//    
//    return new_order;
//}



std::vector<CourierSubPath> generate_new_courier (std::vector<PD>& order,
                                                  std::vector<std::vector<std::vector<StreetSegmentIdx>>>& all_paths,
                                                  std::vector<IntersectionIdx>& intersections_dest,
                                                  const std::vector<DeliveryInf>& deliveries, const std::vector<int>& depots, double turn_penalty) {
    std::vector<CourierSubPath> new_path;
    
    for (int i = 0; i < order.size() - 1; i++) {
        CourierSubPath new_node;
        new_node.start_intersection = intersections_dest[order[i].index];
        new_node.end_intersection = intersections_dest[order[i+1].index];
        
        int start = 0;
        int end = 0;
       
        new_node.subpath = all_paths[order[i].index][order[i+1].index];
        
        new_path.push_back(new_node);
        
    }
    
    firstAndLastDepot(order, new_path, deliveries, depots, all_paths, turn_penalty);
    
    return new_path;
}

void firstAndLastDepot (std::vector<PD>& order, std::vector<CourierSubPath>& new_path, const std::vector<DeliveryInf>& deliveries,
                            const std::vector<int>& depots, std::vector<std::vector<std::vector<StreetSegmentIdx>>>& all_paths, double turn_penalty ){
    double pathTime;
    double currMin = INT_MAX;
    const int curr = order[order.size()-1].index;
    const int first = order[0].index;
    int depot_end, depot_start;
    
    CourierSubPath lastPath, firstPath;
    
    lastPath.start_intersection = new_path.back().end_intersection;
    
    for(int i = 2*deliveries.size(); i < all_paths.size(); i++){
               pathTime = computePathTravelTime(all_paths[curr][i],turn_penalty);
               //pathTime = findEuclidianDistance(intersectionPosition[deliveries[next/2].dropOff], intersectionPosition[depots[i-(2*numDeliveries)]]);
               if(pathTime< currMin && pathTime!=0){
                   currMin = pathTime;
                   depot_end = i;
               }
           }
    
    lastPath.end_intersection = depots[depot_end-2*(deliveries.size())];
    
    lastPath.subpath = all_paths[curr][depot_end];
    new_path.push_back(lastPath);
    
    firstPath.end_intersection=new_path[0].start_intersection;
    currMin = INT_MAX;
    for (int i = 2*deliveries.size(); i < all_paths.size(); i++){
        pathTime = computePathTravelTime(all_paths[i][first],turn_penalty);
        if(pathTime<currMin&& pathTime!=0){                   
                   currMin = pathTime;
                   depot_start = i;

                   
               }
    }
    firstPath.start_intersection = depots[depot_start-(2*deliveries.size())];
    firstPath.subpath = all_paths[depot_start][first];
    
    new_path.insert(new_path.begin(), firstPath);
    
    
}


//check that a given intersection order is legal
bool order_is_legal (std::vector<PD>& order, std::vector<IntersectionIdx>& intersections_dest) {
    //std::cout<<intersections_dest[154]<<std::endl;
    std::vector<bool> legality;
    for(int i = 0; i< intersections_dest.size(); i++){
        legality.push_back(false);
    }
    
    for (int i = 0; i < order.size(); i++){
       // std::cout<<order[i].index<<std::endl;
        if(order[i].index%2==1){
            if(!legality[order[i].index-1]){
                return false;
            }
        }else if (order[i].index%2==0){
            legality[order[i].index] = true;
        }
    }
    
    
    //legality.clear();
    return true;    
    

}


std::vector<PD> swap_nodes (std::vector<PD> order, std::vector<IntersectionIdx>&  intersections_dest,
                                                  std::vector<std::vector<std::vector<StreetSegmentIdx>>>& all_paths,
                                                  const std::vector<DeliveryInf>& deliveries, const std::vector<int>& depots, double turn_penalty,
                                                  double bestCost) {
    
    if (order.size() < 4) return order;
    int k = 0;
    
    std::random_device rd;     // only used once to initialise (seed) engine
    std::mt19937 rng(rd());    // random-number engine used (Mersenne-Twister in this case)
    std::uniform_int_distribution<int> uni(0,order.size() - 1); // guaranteed unbiased
    
    
    std::vector<PD> new_order(order);

    
    do {
      auto rand1 = uni(rng);
      auto rand2 = uni(rng);
      
      
      std::swap(new_order[rand1], new_order[rand2]);
      
      if (order_is_legal(new_order, intersections_dest)) {
          return new_order;
      }
      
      std::swap(new_order[rand1], new_order[rand2]);
      
    } while (1) ;
    
    
    return new_order;
} 


std::vector<CourierSubPath> simulatedAnnealing(std::vector<CourierSubPath> &initial, std::vector<PD> &current, 
        std::vector<IntersectionIdx> &intersections_dest, std::vector<std::vector<std::vector<StreetSegmentIdx>>>& all_paths,
        const std::vector<DeliveryInf>& deliveries, const std::vector<int>& depots, double turn_penalty){
    std::random_device rd;
    std::default_random_engine eng(rd());
    std::uniform_real_distribution<float> distr(0.0, 1.0);
    
    double bestCost = 0.0;
    double prevCost = 0.0;
    double newCost = 0.0;
    double temperature = 50.0;
    
    std::vector<CourierSubPath> bestPath;
    std::vector<CourierSubPath> newPath = initial;
    std::vector <PD> order = current;
    for(int i = 0;i < initial.size();i++){
        bestCost += computePathTravelTime(initial[i].subpath,15.00000);
    }
    
    while(prevCost != bestCost){
        for(int i = 0;i < 3000;i++){
            newCost = 0.0;
            current = swap_nodes(order,intersections_dest,all_paths,deliveries,depots,turn_penalty,bestCost);
            newPath = generate_new_courier(current,all_paths,intersections_dest, deliveries, depots, turn_penalty);

            for(int i = 0;i < initial.size();i++){
                newCost += computePathTravelTime(newPath[i].subpath,turn_penalty);
            }
            double deltaCost = newCost - bestCost;
            if(newCost < bestCost || distr(eng) < exp(-1.0*deltaCost/temperature)){
                prevCost = bestCost;
                order = current; 
                bestPath = newPath;
                bestCost = newCost;
            }
        }
        std::cout << temperature << std::endl;
        temperature -= 0.01;
    }
    std::cout << bestCost;
    return bestPath;
}

