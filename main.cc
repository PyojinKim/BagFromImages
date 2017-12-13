#include<iostream>
#include<ros/ros.h>
#include<rosbag/bag.h>
#include<rosbag/view.h>
#include<sensor_msgs/Image.h>
#include<std_msgs/Time.h>
#include<std_msgs/Header.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>

#include "Thirdparty/DLib/FileFunctions.h"


using namespace std;

int main(int argc, char **argv)
{
    ros::init(argc, argv, "BagFromImages");

    if(argc!=6)
    {
        cerr << "Usage: rosrun BagFromImages BagFromImages <path to left image> <path to right image> <image extension .ext> <frequency> <path to output bag>" << endl;
        return 0;
    }

    ros::start();

    // Vector of paths to image
    vector<string> cam0_filenames = DUtils::FileFunctions::Dir(argv[1], argv[3], true);
    vector<string> cam1_filenames = DUtils::FileFunctions::Dir(argv[2], argv[3], true);

    cout << "Left(cam0) Images: " << cam0_filenames.size() << endl;
    cout << "Right(cam1) Images: " << cam1_filenames.size() << endl;

    // Frequency
    double freq = atof(argv[4]);

    // Output bag
    rosbag::Bag bag_out(argv[5],rosbag::bagmode::Write);

    ros::Time t = ros::Time::now();

    const float T=1.0f/freq;
    ros::Duration d(T);

    for(size_t i=0;i<cam0_filenames.size();i++)
    {
        if(!ros::ok())
        {
          break;
        }

        cv::Mat im_cam0 = cv::imread(cam0_filenames[i],CV_LOAD_IMAGE_COLOR);
        cv::Mat im_cam1 = cv::imread(cam1_filenames[i],CV_LOAD_IMAGE_COLOR);
        cv_bridge::CvImage cv_cam0_image, cv_cam1_image;
        cv_cam0_image.image = im_cam0;
        cv_cam0_image.encoding = sensor_msgs::image_encodings::RGB8;
        cv_cam0_image.header.stamp = t;

        cv_cam1_image.image = im_cam1;
        cv_cam1_image.encoding = sensor_msgs::image_encodings::RGB8;
        cv_cam1_image.header.stamp = t;

        bag_out.write("/cam0/image_raw",ros::Time(t),cv_cam0_image.toImageMsg());
        bag_out.write("/cam1/image_raw",ros::Time(t),cv_cam1_image.toImageMsg());
        t+=d;
        cout << i << " / " << cam0_filenames.size() << endl;
    }

    bag_out.close();

    ros::shutdown();

    return 0;
}
