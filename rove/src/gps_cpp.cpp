#include<ros/ros.h>
#include<geometry_msgs/Pose.h>
#include<sensor_msgs/NavSatFix.h>
#include<nav_msgs/Odometry.h>
#include<bits/stdc++.h>
#include<GeographicLib/Geodesic.hpp>
#include<GeographicLib/UTMUPS.hpp>

using namespace std;
using namespace GeographicLib;

double flat,flon,lon,lat,initx,inity;

void cb1(sensor_msgs::NavSatFix msg){
    lat= msg.latitude;
    lon= msg.longitude;
}

void cb2(sensor_msgs::NavSatFix msg){
    flat = msg.latitude;
    flon = msg.longitude;
}

void posecb(nav_msgs::Odometry msg){
    initx= msg->pose.pose.position.x;
    inity= msg->pose.pose.position.y;
}

int main(int argc, char** argv){
    ros::init(argc,argv,"gps_cpp");
    ros::NodeHandle nh;
    const Geodesic &geod = Geodesic::WGS84();

    ros::Subscriber sub1 = nh.subscribe("/fix",10,cb1);
    ros::Subscriber sub2 = nh.subscribe("/rover/desired_point_gps",10,cb2);
    ros::Subscriber pose_sub = nh.subscribe("/rover/odom",10,posecb);
    ros::Publisher pose_pub = nh.advertise<geometry_msgs::Pose>("/rover/cmd_pose",10);
    ros::Rate loopRate(10);
    while(ros::ok){
        ros::spinOnce();
        double x1,x2,y1,y2;
        int flag;
        bool flag2
        UTMUPS::Forward(lat, lon, flag, flag2, x1, y1);
        UTMUPS::Forward(flat, flon, flag, flag2, x2, y2);
        geometry_msgs::Pose ans;
        ans.position.x= x2-x1+initx;
        ans.position.y= y2-y1+inity;
        pose_pub.publish(ans);
        loopRate.sleep();
    }
}