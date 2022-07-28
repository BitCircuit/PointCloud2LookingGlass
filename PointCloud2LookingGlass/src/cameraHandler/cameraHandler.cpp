#include "cameraHandler.h"

#include <thread>

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/io/openni2_grabber.h>
#include <pcl/common/time.h>
#include <pcl/io/pcd_io.h>
#include <pcl/search/kdtree.h> // for KdTree
#include <pcl/features/normal_3d.h>
#include <pcl/surface/gp3.h>

#include "pcl/surface/vtk_smoothing/vtk_utils.h"
#include "../vtkHandler/vtkHandler.h"

using namespace std::chrono_literals;

class SimpleOpenNIProcessor
{
public:
    void cloud_cb_(const pcl::PointCloud<pcl::PointXYZRGBA>::ConstPtr& cloud)
    {
        static unsigned count = 0;
        static double last = pcl::getTime();
        if (++count == 30)
        {
            double now = pcl::getTime();
            std::cout << "distance of center pixel :" << cloud->points[(cloud->width >> 1) * (cloud->height + 1)].z << " mm. Average framerate: " << double(count) / double(now - last) << " Hz" << std::endl;
            count = 0;
            last = now;
        }
    }

    void run(char* deviceID)
    {
        // create a new grabber for OpenNI devices
        pcl::Grabber* interface = new pcl::io::OpenNI2Grabber();

        // make callback function from member function
        std::function<void(const pcl::PointCloud<pcl::PointXYZRGBA>::ConstPtr&)> f =
            [this](const pcl::PointCloud<pcl::PointXYZRGBA>::ConstPtr& cloud) { cloud_cb_(cloud); };

        // connect callback function for desired signal. In this case its a point cloud with color values
        boost::signals2::connection c = interface->registerCallback(f);

        // start receiving point clouds
        interface->start();

        // wait until user quits program with Ctrl-C, but no busy-waiting -> sleep (1);
        while (true)
            std::this_thread::sleep_for(1s);

        // stop the grabber
        interface->stop();
    }
};

void cameraHandler::cameraHandler(char* deviceID) {
    //SimpleOpenNIProcessor v;
    //v.run(deviceID);
    
     // Load input file into a PointCloud<T> with an appropriate type
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud(new pcl::PointCloud<pcl::PointXYZ>);
    pcl::PCLPointCloud2 cloud_blob;
    pcl::io::loadPCDFile("D:\\Users\\xiaoa\\Documents\\MSc_Project\\pcl\\src\\test\\bun0.pcd", cloud_blob);
    pcl::fromPCLPointCloud2(cloud_blob, *cloud);
    //* the data should be available in cloud

    // Normal estimation*
    pcl::NormalEstimation<pcl::PointXYZ, pcl::Normal> n;
    pcl::PointCloud<pcl::Normal>::Ptr normals(new pcl::PointCloud<pcl::Normal>);
    pcl::search::KdTree<pcl::PointXYZ>::Ptr tree(new pcl::search::KdTree<pcl::PointXYZ>);
    tree->setInputCloud(cloud);
    n.setInputCloud(cloud);
    n.setSearchMethod(tree);
    n.setKSearch(20);
    n.compute(*normals);
    //* normals should not contain the point normals + surface curvatures

    // Concatenate the XYZ and normal fields*
    pcl::PointCloud<pcl::PointNormal>::Ptr cloud_with_normals(new pcl::PointCloud<pcl::PointNormal>);
    pcl::concatenateFields(*cloud, *normals, *cloud_with_normals);
    //* cloud_with_normals = cloud + normals

    // Create search tree*
    pcl::search::KdTree<pcl::PointNormal>::Ptr tree2(new pcl::search::KdTree<pcl::PointNormal>);
    tree2->setInputCloud(cloud_with_normals);

    // Initialize objects
    pcl::GreedyProjectionTriangulation<pcl::PointNormal> gp3;
    pcl::PolygonMesh triangles;

    // Set the maximum distance between connected points (maximum edge length)
    gp3.setSearchRadius(0.025);

    // Set typical values for the parameters
    gp3.setMu(2.5);
    gp3.setMaximumNearestNeighbors(100);
    gp3.setMaximumSurfaceAngle(M_PI / 4); // 45 degrees
    gp3.setMinimumAngle(M_PI / 18); // 10 degrees
    gp3.setMaximumAngle(2 * M_PI / 3); // 120 degrees
    gp3.setNormalConsistency(false);

    // Get result
    gp3.setInputCloud(cloud_with_normals);
    gp3.setSearchMethod(tree2);
    gp3.reconstruct(triangles);

    // Additional vertex information
    std::vector<int> parts = gp3.getPartIDs();
    std::vector<int> states = gp3.getPointStates();
    
    vtkSmartPointer<vtkPolyData> polydata;
    pcl::VTKUtils::convertToVTK(triangles, polydata);
    vtkHandler::pclInterface(polydata);
    
}