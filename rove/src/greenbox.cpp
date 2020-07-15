#include <ros/ros.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Header.h>
#include <opencv2/opencv.hpp>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/image_encodings.h>
#include <geometry_msgs/Pose.h>
#include <geometry_msgs/Pose2D.h>
#include <vector>

using namespace std;
using namespace cv;

Mat input;
float flag = 0;

void imgcb(const sensor_msgs::Image& msg){
   cv_bridge::CvImagePtr cv_ptr;
    try{
        cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
        input = cv_ptr->image;
    } 
    catch(cv_bridge::Exception& err) {
        ROS_ERROR("cv_bridge exception: %s", err.what());
        return;
    }
}

int main(int argc, char** argv){

    ros::init(argc, argv, "centre_box");
    ros::NodeHandle nh;
    ros::Subscriber imgsub = nh.subscribe("/rover/camera/image_raw", 1, imgcb);
    ros::Publisher centre_pub = nh.advertise<geometry_msgs::Pose2D>("centre_box/centre", 1);
	ros::Rate loopRate(10);
	cv::Scalar hmin(50,0,0);
	cv::Scalar hmax(100, 255, 255);

	while(ros::ok()){
  		ros::spinOnce();
  		if(input.empty())continue;
  		Mat hsv_,thrh,smth;
  		cvtColor(input, hsv_, CV_BGR2HSV);
  		inRange(hsv_, hmin, hmax, thrh);
  		morphologyEx(thrh, smth, CV_MOP_OPEN, getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3)), cv::Point(-1, -1), 1);
  
  		vector<vector<Point>> contours;
  		findContours(smth, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
  		double max_ar = 0, max_index = 0, countx = 0, county = 0;
  		for(int i=0; i<contours.size(); i++){
    		double ar = contourArea(contours[i]);
    		if(ar > max_ar && contours[i].size() == 4){
      			max_index = i;
      			max_ar = ar;
    		}
  		}
  		vector<Point> contour = contours[max_index];
  		for(int i=0; i<contour.size(); i++){
    		countx += contour[i].x;
    		county += contour[i].y;
  		}
  		countx/= contour.size();
  		county/= contour.size();
  		geometry_msgs::Pose2D pos;
  		pos.x= countx;
  		pos.y= county;
  		pub.publish(pos);
  		loopRate.sleep();
	}
	return 0;
}