#include <iostream>
#include <string>
#include <fstream>

int main(int argc, char *argv[])
{
    uint elementLength = 1;
    std::string dir = "";

    // arguments check
    if(argc == 4)
    {
        dir = std::string(argv[3]);
    }
    else if(argc == 3)
    {
        elementLength = static_cast<uint>(atoi(argv[2]));
    }
    else if(argc == 1 || argc > 4)
    {
        std::cout << "usage:" << std::endl;
        std::cout << "\t./generator minimum_number_of_elements" << std::endl;
        std::cout << "or" << std::endl;
        std::cout << "\t./generator minimum_number_of_elements element_length" << std::endl;
        std::cout << "E.g. ./generator 100" << std::endl;
        std::cout << "creates file 101.truss with minimum 101 elements truss." << std::endl;
        std::cout << "With default element length 1m." << std::endl;
        std::cout << "Result will have 5 + 4*n elements, not less than given treshold." << std::endl;
        std::cout << "E.g. ./generator 100 10" << std::endl;
        std::cout << "creates file 101.truss with minimum 101 elements truss." << std::endl;
        std::cout << "With element length 10m." << std::endl;
    }

    // truss size calculation
    size_t inputElementsCount = static_cast<size_t>(atoi(argv[1]));
    size_t elementsCount = 5;   // minimum 5 elements
    size_t nodesCount = 4;      // connecting 4 nodes
    
    if(inputElementsCount > 5)
    {
        size_t temp = (inputElementsCount - 5) / 4;
        elementsCount = 5 + 4 * temp;        
        nodesCount = 4 + 2 * temp;
    }
    
    std::cout << "Nodes #: " << nodesCount << std::endl;
    std::cout << "Elements #: " << elementsCount << std::endl;
    std::cout << "Element length: " << elementLength << std::endl;

    // create the output file
    std::string fileName = "";
    if(dir != "") 
        fileName= dir + '/';
    fileName += std::to_string(elementsCount) + ".truss";

    std::ofstream file;
    file.open(fileName,std::ios::out);
    
    if(!file.is_open())
    {
        std::cout << "Cannot create file " << fileName << "." << std::endl;
        exit(-1);
    }

    // constant part of the TRUSS file
    file << "TRUSS" << std::endl << "A 0.3" << std::endl << "E 120e4" << std::endl;

    // generate all the nodes
    file << "DOFS " << 2 * nodesCount << std::endl;
    // base span nodes
    file << "0 0" << std::endl;
    file << "0 " << elementLength << std::endl;
    file << elementLength << " 0" << std::endl;
    file << elementLength << " " << elementLength << std::endl;
    // following spans nodes
    uint lastX = elementLength;
    for(size_t node=4; node<nodesCount; node+=2)
    {
        uint x = lastX + elementLength;
        file << x << " 0" << std::endl;
        file << x << " " << elementLength << std::endl;
        lastX = x;
    }

    // generate all the elements
    file << "BARS " << elementsCount << std::endl;
    // base span elements
    file << "0 1 2 3" << std::endl;
    file << "2 3 6 7" << std::endl;
    file << "4 5 6 7" << std::endl;
    file << "0 1 4 5" << std::endl;
    file << "0 1 6 7" << std::endl;
    // following spans elements
    uint lastNode = 3;
    for(size_t element=5; element<elementsCount; element+=4)
    {
        // top bar
        uint x1 = 2 * lastNode;
        uint y1 = 2 * lastNode + 1;
        uint x2 = 2 * (lastNode + 2);
        uint y2 = 2 * (lastNode + 2) + 1;
        file << x1 << " " << y1 << " " << x2 << " " << y2 << std::endl;
        // right bar
        x1 = 2 * (lastNode + 2);
        y1 = 2 * (lastNode + 2) + 1;
        x2 = 2 * (lastNode + 1);
        y2 = 2 * (lastNode + 1) + 1;
        file << x1 << " " << y1 << " " << x2 << " " << y2 << std::endl;
        // bottom bar
        x1 = 2 * (lastNode + 1);
        y1 = 2 * (lastNode + 1) + 1;
        x2 = 2 * (lastNode - 1);
        y2 = 2 * (lastNode - 1) + 1;
        file << x1 << " " << y1 << " " << x2 << " " << y2 << std::endl;
        // diagonal bar
        x1 = 2 * (lastNode - 1);
        y1 = 2 * (lastNode - 1) + 1;
        x2 = 2 * (lastNode + 2);
        y2 = 2 * (lastNode + 2) + 1;
        file << x1 << " " << y1 << " " << x2 << " " << y2 << std::endl;

        lastNode += 2;
    } 

    // generate constrains
    file << "CONSTRAINS" << std::endl;
    // base span constrains
    file << "1 1" << std::endl;
    file << "0 0" << std::endl;
    file << "1 1" << std::endl;
    file << "0 0" << std::endl;
    // following spans constrains
    for(size_t node=4; node<nodesCount; node++)
    {   
        file << ((node % 11 == 0) ? "1 1" : "0 0") << std::endl;
    }

    // generate external forces
    file << "FORCES" << std::endl;
    // base span loads
    file << "0 0" << std::endl;
    file << "10000 0" << std::endl; // horizontal load
    file << "0 0" << std::endl;
    file << "0 0" << std::endl;
    // following spans loads
    for(size_t node=4; node<nodesCount; node++)
    {   
        file << ((node % 6 == 0) ? "0 -10000" : "0 0") << std::endl;
    }
    return 0;
}
