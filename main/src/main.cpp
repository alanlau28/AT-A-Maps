/* 
 * Copyright 2021 University of Toronto
 *
 * Permission is hereby granted, to use this software and associated 
 * documentation files (the "Software") in course work at the University 
 * of Toronto, or for personal use. Other uses are prohibited, in 
 * particular the distribution of the Software either publicly or to third 
 * parties.
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#include <iostream>
#include <string>
#include <vector>
#include "m1.h"
#include "m2.h"
#include "m3.h"

#include <vector>

//Program exit codes
constexpr int SUCCESS_EXIT_CODE = 0;        //Everything went OK
constexpr int ERROR_EXIT_CODE = 1;          //An error occurred
constexpr int BAD_ARGUMENTS_EXIT_CODE = 2;  //Invalid command-line usage

//The default map to load if none is specified
std::string default_map_path = "/cad2/ece297s/public/maps/toronto_canada.streets.bin";

// The start routine of your program (main) when you are running your standalone
// mapper program. This main routine is *never called* when you are running 
// ece297exercise (the unit tests) -- those tests have their own main routine
// and directly call your functions in /libstreetmap/src/ to test them.
// Don't write any code in this file that you want run by ece297exercise -- it 
// will not be called!
int main(int argc, char** argv) {

    std::string map_path;
    if(argc == 1) {
        //Use a default map
        map_path = default_map_path;
    } else if (argc == 2) {
        //Get the map from the command line
        map_path = argv[1];
    } else {
        //Invalid arguments
        std::cerr << "Usage: " << argv[0] << " [map_file_path]\n";
        std::cerr << "  If no map_file_path is provided a default map is loaded.\n";
        return BAD_ARGUMENTS_EXIT_CODE;
    }

    //Load the map and related data structures
    bool load_success = loadMap(map_path);
    if(!load_success) {
        do{
            std::cerr << "Failed to load map '" << map_path << "'\n";
            std::cout << "Please enter a new map path: " << std::endl;
            std::cin >> map_path;
            load_success = loadMap(map_path);
        }while(!load_success);
    }
    

    std::cout << "Successfully loaded map '" << map_path << "'\n";

    //You can now do something with the map data
  
    //do something in between
    
    drawMap();

    std::vector<StreetSegmentIdx> path;

    //path = findPathBetweenIntersections(7832, 19097, 0.00000000000000000);

    //path = findPathBetweenIntersections(13, 184, 0.00000000000000000);
    /*path = {118264, 118265, 118266, 118267, 23465, 90857, 90842, 90911, 90901, 90845, 90882, 90869, 90868, 90867, 90866, 90865, 90864, 90863, 90894, 171337, 44204, 207468, 207470, 207471, 207472, 207473, 207474, 144950, 144951, 144961, 144955, 144956, 144957, 44213, 184812, 44211, 155812, 138873, 138872, 86708, 859, 860, 861, 123030, 123031, 2852, 2851, 123028, 15421, 15420, 15430, 863, 123029, 100120, 100121, 9745, 9746, 9747, 134653, 134654, 1510, 111289, 118004, 19091, 26861, 26862, 26815, 26813, 26814, 100107, 110510, 22210, 22211, 110512, 110511, 6022, 126074, 126075, 110518, 110519, 22193, 22194, 110504, 100108, 113340, 8748, 110491, 110490, 110492, 100127, 110493, 27175, 27176, 27188, 27189, 6037, 6038, 138725, 138730, 138728, 80875, 138723, 80879, 80880, 80881, 80902, 80903, 112472, 6027, 107054, 215518, 37994, 155375, 37995, 13569, 13570, 13571, 13572, 13573, 13574, 13575, 13576, 13577, 13578, 13579, 84975, 134623, 121612, 134625, 134626, 121610, 133707, 161842, 161841, 161840, 161839, 161838, 161837, 161836, 161843, 110890, 110889, 110893, 110894, 110895, 110891, 110892, 110896, 110897, 110898, 110899, 110900, 112096, 161850, 112093, 112095, 9082, 2695, 44233, 37986, 37987, 37988, 69499};
    
    double answer = computePathTravelTime(path, 7.92470683533765108);*/
    
path = findPathBetweenIntersections(35474, 9037, 15.00000000000000000);
std::cout<< (computePathTravelTime(path, 15.00000000000000000));
    
    //Clean-up the map data and related data structures
    std::cout << "Closing map\n";
    closeMap(); 

    return SUCCESS_EXIT_CODE;
}
