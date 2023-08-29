#include <iostream>
#include <fstream>
#include <vector>
#include <string>

struct Point 
{
    int id;
    float x;
    float y;
    float z;
};

struct Muscle
{
    int id;
    int node1;
    int node2;
};

Point calculateCenterPoint(const std::vector<Point>& points) 
{
    Point centerPoint {-1, 0.0, 0.0, 0.0};

    for (const auto& point : points) 
    {
        centerPoint.x += point.x;
        centerPoint.y += point.y;
        centerPoint.z += point.z;
    }

    int nodeCount = points.size();
    centerPoint.x /= nodeCount;
    centerPoint.y /= nodeCount;
    centerPoint.z /= nodeCount;

    return centerPoint;
}

void createLayers(std::vector<Point>& points, Point centerPoint, int nodeCount, float factor, int newLayers)
{
    for(int i = 0; i < newLayers; i++)
    {  

        for(int j = nodeCount*i; j < nodeCount*(i+1); j++)
        {   
            float dx = centerPoint.x - points[j].x;
            float dy = centerPoint.y - points[j].y;
            float dz = centerPoint.z - points[j].z;

            int id = points[j].id + nodeCount;
            float x = points[j].x + factor*dx;
            float y = points[j].y + factor*dy;
            float z = points[j].z + factor*dz;

            Point newPoint {id, x, y, z};
            points.push_back(newPoint);
            
            //std::cout << newPoint.id << x << y << z << std::endl;
        }
    }
}

void connectMuscles(std::vector<Muscle>& muscles, int muscleCount, std::vector<Point>& points, int nodeCount, int newLayers)
{

//connect every new node to neighbor nodes

    for(int i = 0; i < newLayers; i++)
    {
        for(int j = i*muscleCount; j < muscleCount*(i+1); j++)
        {
            int id = muscles[j].id + muscleCount;
            int node1 = muscles[j].node1+nodeCount;
            int node2 = muscles[j].node2+nodeCount;

            Muscle newMuscle {id, node1, node2};
            muscles.push_back(newMuscle);
        }
    }

//connect every new node to below nodes

    for(int i = 0; i < 2*muscleCount; i++)
    {

        int id = muscles[i].id + muscleCount*3;
        int node1 = muscles[i].node1;
        int node2 = muscles[i].node2 + nodeCount;

        Muscle newMuscle {id, node1, node2};
        muscles.push_back(newMuscle);

        
        
    }

/*
//connect every node to parent node
    for(int i = 0; i < newLayers; i++)
    {
        for(int j = nodeCount*i; j < nodeCount*(i+1); j++)
        {
            int id = j + (newLayers+1)*muscleCount;
            int node1 = points[j].id;
            int node2 = points[j+nodeCount].id;

            Muscle newMuscle {id, node1, node2};
            muscles.push_back(newMuscle);
        }
    }
    */
}

int main() 
{

//input directory/files
    std::string readDirectory = "IdealLeftAtrium2.3K/";
    std::string nodeFilePath = readDirectory + "Nodes";
    std::string muscleFilePath = readDirectory + "Muscles";

//input factor to move closer to center and number of layers
    float factor = 0.01;
    int newLayers = 2;

//input where to output data
    std::string writeDirectory = "3DTestFolder/";
    std::string newNodeFile = writeDirectory + "Nodes";
    std::string newMuscleFile = writeDirectory + "Muscles";


    std::ifstream inNodeFile(nodeFilePath);
    if (!inNodeFile) 
    {
        std::cerr << "Can't open file: " << nodeFilePath << std::endl;
        return 1;
    }
    std::ifstream inMuscleFile(muscleFilePath);
    if (!inMuscleFile) 
    {
        std::cerr << "Can't open file: " << muscleFilePath << std::endl;
        return 1;
    }


//get nodeCount and skip other lines
    int nodeCount, val1, val2, val3;
    inNodeFile >> nodeCount;
    inNodeFile >> val1;
    inNodeFile >> val2;
    inNodeFile >> val3;

//read points from file
    std::vector<Point> points;

    int nodeid;
    float x, y, z;
    while (inNodeFile >> nodeid >> x >> y >> z) 
    {
        points.push_back({nodeid, x, y, z});
    }

    inNodeFile.close();

    if (points.empty()) 
    {
        std::cerr << "No points found in the file." << std::endl;
        return 1;
    }
//calculate center
    Point centerPoint = calculateCenterPoint(points);
    
//get muscleCount
    int muscleCount;
    inMuscleFile >> muscleCount;

    std::vector<Muscle> muscles;

//read muscles from file
    int muscleid, node1, node2;
    while(inMuscleFile >> muscleid >> node1 >> node2)
    {
        muscles.push_back({muscleid, node1, node2});
    }

//create new node layers   
    createLayers(points, centerPoint, nodeCount, factor, newLayers);
    
//create new muscle connections

    connectMuscles(muscles, muscleCount, points, nodeCount, newLayers);

//writing new nodes to file
    std::ofstream nodeFile(newNodeFile);
    if (!nodeFile) 
    {
        std::cerr << "Failed to open the file for writing." << std::endl;
        //return 1;
    }

    nodeFile << (newLayers+1)*nodeCount << std::endl;
    nodeFile << val1 << std::endl;
    nodeFile << val2 << std::endl;
    nodeFile << val3 << std::endl;
    for (const auto& point : points) 
    {
        nodeFile << point.id << " " << point.x << " " << point.y << " " << point.z << std::endl;
    }

    nodeFile.close();
    std::cout << "Points written to the file: " << newNodeFile << std::endl;
    
//writing new muscle connections to file
    std::ofstream muscleFile(newMuscleFile);
    if (!muscleFile) 
    {
        std::cerr << "Failed to open the file for writing." << std::endl;
        return 1;
    }

    muscleFile << (newLayers+1)*muscleCount + 2*muscleCount << std::endl;
    for (const auto& muscle : muscles) 
    {
        muscleFile << muscle.id << " " << muscle.node1 << " " << muscle.node2 << std::endl;
    }

    muscleFile.close();
    std::cout << "Points written to the file: " << newMuscleFile << std::endl;


    return 0;
}
