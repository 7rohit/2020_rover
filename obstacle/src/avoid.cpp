#include <ros/ros.h>
#include <std_msgs/String.h>
#include <std_msgs/Float64.h>
#include<geometry_msgs/Twist.h>
#include<geometry_msgs/Pose.h>
#include<nav_msgs/Odometry.h>
#include<bits/stdc++.h>
#include<std_msgs/Int32.h>
#include<sensor_msgs/LaserScan.h>

using namespace std;

geometry_msgs::Twist spd;
std_msgs::Int32 flag;

void fwd(){
    spd.linear.x = 800;
    spd.angular.z = 0;  
}

void lft(){
     spd.angular.z = -800;
     spd.linear.x = 50;
}

void lasercb(sensor_msgs::LaserScan msg){
    float range_max = msg.range_max;
    for(int i = 0; i < 360; i++){
        if((msg.ranges[i] < range_max) && (msg.ranges[i] > 0.40) && (msg.ranges[i] < 4.0)){
            flag.data = 1;
        }
        else flag.data = 0;
    }  
}

int main(int argc, char** argv){

	ros::init(argc, argv, "laser");
	ros::NodeHandle nh;
	ros::Subscriber sub = nh.subscribe("rover/scan",10, lasercb);
	ros::Publisher pubflag = nh.advertise<std_msgs::Int32>("/debug/flag",10);
	ros::Publisher pubvel = nh.advertise<geometry_msgs::Twist>("/rover/cmd_vel",10);
	ros::Rate loopRate(10);

	while(ros::ok()){

    	ros::spinOnce();
    	pubflag.publish(flag);

    	if(flag.data == 1) {
        	lft();
        	pubvel.publish(spd);
        	loopRate.sleep();

        	fwd();
        	pubvel.publish(spd);
        	loopRate.sleep();
    	}

    	loopRate.sleep();
	}

	return 0;
}
