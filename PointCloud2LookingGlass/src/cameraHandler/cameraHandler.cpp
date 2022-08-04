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
#include <librealsense2/rs.hpp>

#include "pcl/surface/vtk_smoothing/vtk_utils.h"
#include "../vtkHandler/vtkHandler.h"

using namespace std::chrono_literals;

class SimpleOpenNIProcessor
{
public:
    void cloud_cb_(const pcl::PointCloud<pcl::PointXYZ>::ConstPtr& cloud)
    {
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

        vtkSmartPointer<vtkPolyData> polydata;
        pcl::VTKUtils::convertToVTK(triangles, polydata);
        vtkHandler::pclInterface(polydata);
    }

    void run(char* deviceID)
    {
        // create a new grabber for OpenNI devices
        pcl::Grabber* interface = new pcl::io::OpenNI2Grabber();

        // make callback function from member function
        std::function<void(const pcl::PointCloud<pcl::PointXYZ>::ConstPtr&)> f =
            [this](const pcl::PointCloud<pcl::PointXYZ>::ConstPtr& cloud) { cloud_cb_(cloud); };

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
    SimpleOpenNIProcessor v;
    v.run(deviceID);
    

}