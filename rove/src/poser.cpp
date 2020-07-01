#include<ros/ros.h>
#include<geometry_msgs/Twist.h>
#include<geometry_msgs/Pose.h>
#include<nav_msgs/Odometry.h>
#include<std_msgs/Float64.h>
#include<cmath>
#include<tf/tf.h>

double initx, inity, roll, pitch, yaw, posx, posy, speed, direc;
double rad = 57.295779513;
void initcb(nav_msgs::Odometry msg){
    initx= msg->pose.pose.position.x;
    inity= msg->pose.pose.position.y;
    tf::Quaternion q(
        msg->pose.pose.orientation.x,
        msg->pose.pose.orientation.y,
        msg->pose.pose.orientation.z,
        msg->pose.pose.orientation.w);
    tf::Matrix3x3 rot(q);
    rot.getRPY(roll,pitch,yaw);
}

void poscb(geometry_msgs::Pose msg){
    posx=msg.position.x;
    posy=msg.position.y;
}

int main(int argc , char** argv){
    ros::init(argc,argv,"poser");
    ros::NodeHandle nh;
    ros::Subscriber init_sub = nh.subscribe("/rover/odom",10,initcb);
    ros::Subscriber pos_sub = nh.subscribe("/rover/cmd_pose",10,poscb);
    ros::Publisher pub = nh.advertise<geometry_msgs::Twist>("/rover/cmd_vel",10);
    ros::Publisher yaw_pub = nh.advertise<std_msgs::Float64>("/debug/yaw",5);
    ros::Publisher ang_pub = nh.advertise<std_msgs::Float64>("/debug/angle",5);
    ros::Rate loopRate(10);

    while(ros::ok){
        ros::spinOnce();
        double movx, movy;
        double dis,dir;
        movx = posx - initx;
        movy = posy - inity;
        dis = pow((pow(movx,2) + pow(movy,2)),0.5);
        dir = rad*(yaw - atan(movy)/(movx));
        if(movx > 0){
            if(yaw >= 0) dir = dir - 180;
            else dir = dir + 180;
        }
        if(dis<0.3){
            speed=0;
            dir=0;
        }
        else{
            if(abs(dir)<4){
                speed=10*dis;
                if(abs(dir)<2)
                direc=0;    
            }
            else{
                speed=0;
                direc=dir;
            }
        }
        std_msgs::Float64 angp, yawp;
        angp.data=direc;
        yawp.data=rad*yaw;
        ang_pub.publish(angp);
        yaw_pub.publish(yawp);
        geometry_msgs::Twist ans;
        ans.linear.x=speed;
        ans.angular.z=direc;
        pub.publish(ans);
        loopRate.sleep();
    }
}