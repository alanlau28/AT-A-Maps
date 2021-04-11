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


Node::Node(int id, double t, const std::vector<StreetSegmentIdx> &out){
    ID = id;
    leading = NOEDGE;
    time = t;
    outgoing = out;
}

//vector that holds the StreetSegmentInfo for each street segment
std::vector<StreetSegmentInfo> segmentInfo;

//vector that holds the x,y coordinate of each intersection
std::vector<std::pair<double,double>> intersectionPosition;

//vector to hold each node
std::vector<Node> graph;

//override for the priority queue to put the smallest element in the front
bool operator<(const waveElement& lhs,const waveElement& rhs){
    if((lhs.traveltime + lhs.estTime) >= (rhs.traveltime + rhs.estTime)) return true;
    else return false;
}

//initializes each node and loads vector of nodes
void loadGraph(){
    int num = getNumIntersections();
    for (int i = 0; i < num; i++){   
        Node currNode(i, INT_MAX, findStreetSegmentsOfIntersection(i)); //each node is initialized with "infinite" travel time
        graph.push_back(currNode);
    }
   
}

/*Uses A* algorithm
*given a starting node and the end destination intersection id, the function will return true
*or false if there is a path
* If a path is found, every node is updated with the total travel time and its reaching edge
*/

bool path(Node* source_node, IntersectionIdx destination, double turn_penalty){
    std::priority_queue<waveElement> wavefront; //priority queue to sort through nodes by travel time
    waveElement source(source_node,NOEDGE, 0,0);
    wavefront.push(source);
    
    while(!wavefront.empty()){
        waveElement wave = wavefront.top();
        wavefront.pop();
        Node *currNode = wave.node;
        
        //make sure a node is not visited twice
        if(wave.traveltime < currNode -> time){ 
            currNode -> leading = wave.edgeID;
            currNode -> time = wave.traveltime;
            
            //if the current node is the destination a path is found
            if(currNode -> ID == destination){
                return true;
            }
            
            //for each reaching edge of node
            for(int i = 0; i < currNode -> outgoing.size(); i++){
                IntersectionIdx to = findOtherIntersection(currNode -> ID,currNode -> outgoing[i]);
                if(to != NOEDGE){
                    double travelTime = findStreetSegmentTravelTime(currNode -> outgoing[i])+ currNode -> time;
                    
                    //if there is a reaching edge for the current node, check if the next edge is a different street to add turn penalty
                    if(currNode -> leading != NOEDGE){
                        StreetSegmentInfo infoTo = segmentInfo[(currNode -> outgoing[i])];
                        StreetSegmentInfo infoFrom = segmentInfo[currNode -> leading];
                        
                        if(infoTo.streetID != infoFrom.streetID){ 
                            travelTime += turn_penalty;  
                        }
                        
                    }
                    
                    double estTime = findEuclidianDistance(intersectionPosition[currNode -> ID]
                    ,intersectionPosition[to])/max_speed; //the estimated time from the next node to the end destination 
                    
                    //push node at the opposite end of the edge into the wavefront
                    Node* toNode = &graph[to];
                    waveElement elem(toNode,currNode -> outgoing[i],travelTime, estTime);
                    wavefront.push(elem); 
                } 
            }
        } 
    }
    return false;
}

//Goes through the end node and traverses all the street segments until the starting intersection and returns the path
std::vector<StreetSegmentIdx> traceBack(int destination){
    std::vector<StreetSegmentIdx> finalPath;
    Node *currNode = &graph[destination];
    int reachingEdge = currNode -> leading;
    
    //keep traversing each edge until there is no edge left
    while(reachingEdge != NOEDGE){
        finalPath.push_back(reachingEdge);
        
        //find which  node is the node at the opposite edge
        StreetSegmentInfo info = segmentInfo[reachingEdge];
        if(info.from == currNode->ID) currNode = &graph[info.to];
        else currNode = &graph[info.from];
        
        reachingEdge = currNode -> leading;
    }
    
    std::reverse(finalPath.begin(),finalPath.end()); //reverse order to go from starting intersection to destination intersection
    return finalPath;
}

std::vector<StreetSegmentIdx> findPathBetweenIntersections(const IntersectionIdx intersect_id_start, 
    const IntersectionIdx intersect_id_destination, const double turn_penalty){
    
    loadGraph();
    
    std::vector<StreetSegmentIdx> finalPath;
    Node* start = &graph[intersect_id_start];
    
    //found is true if there is a valid path, if true find the path
    //if no path is valid, found is false
    bool found = path(start,intersect_id_destination,turn_penalty);
    if(found){
        finalPath = traceBack(intersect_id_destination);
    }
    
    graph.clear();
    return finalPath;
   
}

double computePathTravelTime(const std::vector<StreetSegmentIdx>& path, 
                                const double turn_penalty){
    //handles error when no path is found
    if(path.size()==0) return 0.0; 
    
    //for every segment in vector, find the next path and check if streetid changes
    //adds a turn penalty if it does
    double total_travel_time = 0;
    StreetSegmentIdx next;
    for (int i = 0; i < path.size()-1; i++){
        next = path[i+1];
        total_travel_time += findStreetSegmentTravelTime(path[i]);
        if(segmentInfo[path[i]].streetID != segmentInfo[next].streetID ){
            total_travel_time += turn_penalty;
        }
    }
    //add the last segment
    total_travel_time += findStreetSegmentTravelTime(path.back());
    return total_travel_time;
}

//finds the Euclidean Distance from one intersection to another intersection
double findEuclidianDistance(std::pair<double,double> intersectionOne,std::pair<double,double> intersectionTwo){   
    return sqrt(pow(intersectionTwo.second-intersectionOne.second,2)+ pow(intersectionTwo.first-intersectionOne.first,2));
}

//Assume that there is never an invalid argument (always exists a node at the end of a street segment)
//Finds the node at the opposite end of a street segment given starting node of the street
IntersectionIdx findOtherIntersection(IntersectionIdx start, StreetSegmentIdx outgoing){
    StreetSegmentInfo info = segmentInfo[outgoing];
    if(start != info.to){
        return info.to;
    }
    else if(!info.oneWay){
        return info.from;
    }
    return -1;
}
