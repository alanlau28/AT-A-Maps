/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   m4_header.h
 * Author: haniftaw
 *
 * Created on April 13, 2021, 3:44 PM
 */

#ifndef M4_HEADER_H
#define M4_HEADER_H

#ifdef __cplusplus
extern "C" {
#endif


struct PD{
    int index;
   
    PD(int id){
        index = id;
       
    }
};



std::vector<PD> generate_intersection_order (std::vector<CourierSubPath>& path, std::vector<IntersectionIdx> destination);

std::vector<std::vector<PD>> swap_subpaths (std::vector<PD> order, int index1, int index2);

std::vector<CourierSubPath> generate_new_courier (std::vector<IntersectionIdx>& order,
                                                  std::vector<std::vector<std::vector<StreetSegmentIdx>>>& all_paths,
                                                  std::vector<IntersectionIdx>& intersections_dest);

bool order_is_legal (std::vector<PD>& path, std::vector<IntersectionIdx>&  intersections_dest);


std::vector<CourierSubPath> two_opt_algorithm_order (std::vector<PD> delivery_order,
                                                    std::vector<CourierSubPath>& path,
                                                    std::vector<DeliveryInf> deliveries,
                                                    std::vector<std::vector<std::vector<StreetSegmentIdx>>>& all_paths,
                                                    std::vector<IntersectionIdx>& intersections_dest);


#ifdef __cplusplus
}
#endif

#endif /* M4_HEADER_H */

