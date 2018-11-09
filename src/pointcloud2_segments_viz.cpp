#include <ros/ros.h>
#include <pcl/io/pcd_io.h>
#include <pcl/point_types.h>
#include <pcl/common/colors.h>
#include <pcl_ros/point_cloud.h>
#include <sensor_msgs/PointCloud.h>
#include <pcl/impl/point_types.hpp>
#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/ChannelFloat32.h>
#include <sensor_msgs/point_cloud_conversion.h>
#include <pointcloud_msgs/PointCloud2_Segments.h>


ros::Publisher pub;
std::string base_link_frame;


void pc2s_callback (const pointcloud_msgs::PointCloud2_Segments& msg){

    sensor_msgs::PointCloud2 accumulator;

    sensor_msgs::PointCloud2 cluster_msgs;

    for (size_t i=0; i < msg.clusters.size(); i++){

        pcl::PCLPointCloud2 cloud2;
        pcl_conversions::toPCL( msg.clusters[i] , cloud2);

        pcl::PointCloud<pcl::PointXYZRGB> cloud;
        pcl::fromPCLPointCloud2(cloud2, cloud);


        if (msg.cluster_id.size() > 0){
            std::vector<int> v(msg.cluster_id.size());
            v[i] = msg.cluster_id[i] % 3 ;

            for(size_t j=0; j < cloud.points.size(); j++){
                if ( v[i] == 0 ){
                    uint8_t r = 255 ;
                    uint8_t g = 255 * msg.cluster_id[i] %255;
                    uint8_t b = 255 * msg.cluster_id[i] %255;
                    cloud.points[j].r = r;
                    cloud.points[j].g = g; 
                    cloud.points[j].b = b;
                }
                if ( v[i] == 1 ){
                    uint8_t r = 255 * msg.cluster_id[i] %255;
                    uint8_t g = 255 ;
                    uint8_t b = 255 * msg.cluster_id[i] %255;
                    cloud.points[j].r = r;
                    cloud.points[j].g = g; 
                    cloud.points[j].b = b;
                }
                if ( v[i] == 2 ){
                    uint8_t r = 255 * msg.cluster_id[i] %255;
                    uint8_t g = 255 * msg.cluster_id[i] %255;
                    uint8_t b = 255 ;
                    cloud.points[j].r = r;
                    cloud.points[j].g = g;
                    cloud.points[j].b = b;
                }
            }
        }
        else {
            for(size_t j=0; j < cloud.points.size(); j++){
                uint8_t r = 255 - 25 * i;
                uint8_t g = 90 + 40 * i;
                uint8_t b = 40;
                int32_t rgb = (r << 16) | (g << 8) | b;
                cloud.points[j].rgb = *(float*)(&rgb);
            }
        }
        pcl::PCLPointCloud2 clouds;
        pcl::toPCLPointCloud2(cloud, clouds);
        pcl_conversions::fromPCL(clouds, cluster_msgs);
        cluster_msgs.header.stamp = ros::Time::now();
        cluster_msgs.header.frame_id = base_link_frame;

        sensor_msgs::PointCloud2 tmp = sensor_msgs::PointCloud2(accumulator);

        pcl::concatenatePointCloud( cluster_msgs, tmp, accumulator);
    }

    pub.publish(accumulator);
}

int main (int argc, char** argv){
    ros::init (argc, argv, "pointcloud2_segments_viz");
    ros::NodeHandle n_;
    pcl::console::setVerbosityLevel(pcl::console::L_ALWAYS);

    std::string input_topic;
    std::string out_topic;
    n_.param("pointcloud2_segments_viz/input_topic",input_topic, std::string("/new_pcl"));
    n_.param("pointcloud2_segments_viz/base_link_frame", base_link_frame, std::string("base_link"));
    n_.param("pointcloud2_segments_viz/out_topic", out_topic, std::string("pointcloud2_segments_viz/pointcloud2"));

    ros::Subscriber sub = n_.subscribe (input_topic, 1, pc2s_callback);

    pub = n_.advertise<sensor_msgs::PointCloud2> (out_topic, 1);

    ros::spin ();
}
