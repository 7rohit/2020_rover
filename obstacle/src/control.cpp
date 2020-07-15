#include<ros/ros.h>
#include<sensor_msgs/Image.h>
#include<std_msgs/Int32.h>
#include<std_msgs/Float64.h>
#include<std_msgs/Header.h>
#include<geometry_msgs/Point.h>
#include<geometry_msgs/Twist.h>
#include<rove/Data.h>

ros::Subscriber vel_sub;
ros::Publisher publb,publm,publf,pubrb,pubrm,pubrf,pubjlf,pubjlb,pubjrf,pubjrb;
geometry_msgs::Twist msg;
std_msgs::Float64 msglb,msglm,msglf,msgrb,msgrm,msgrf,msgjlf,msgjlb,msgjrf,msgjrb;
double vel,ang;

void callback(const geometry_msgs::Twist& msg_vel){
    vel=msg_vel.linear.x;
    ang=msg_vel.angular.z;
    msglb.data=vel;
    msglm.data=vel;
    msglf.data=vel;
    msgrb.data=-1*vel;
    msgrm.data=-1*vel;
    msgrf.data=-1*vel;
    msgjlf.data=-1*ang*0.1;
    msgjlb.data=ang*0.1;
    msgjrf.data=-1*ang*0.1;
    msgjrb.data=ang*0.1;
}

void cmd(ros::NodeHandle& nh, ros::NodeHandle& nh_private){
    vel_sub=nh.subscribe("cmd_vel", 1, callback);
    publb=nh.advertise<std_msgs::Float64>("/rover/corner_lb_wheel_lb_controller/command", 1);
    publm=nh.advertise<std_msgs::Float64>("/rover/bogie_left_wheel_lm_controller/command", 1);
    publf=nh.advertise<std_msgs::Float64>("/rover/corner_lf_wheel_lf_controller/command", 1);
    pubrb=nh.advertise<std_msgs::Float64>("/rover/corner_rb_wheel_rb_controller/command", 1);
    pubrm=nh.advertise<std_msgs::Float64>("/rover/bogie_right_wheel_rm_controller/command", 1);
    pubrf=nh.advertise<std_msgs::Float64>("/rover/corner_rf_wheel_rf_controller/command", 1);
    pubjlf=nh.advertise<std_msgs::Float64>("/rover/bogie_left_corner_lf_controller/command", 1);
    pubjlb=nh.advertise<std_msgs::Float64>("/rover/rocker_left_corner_lb_controller/command", 1);
    pubjrf=nh.advertise<std_msgs::Float64>("/rover/bogie_right_corner_rf_controller/command", 1);
    pubjrb=nh.advertise<std_msgs::Float64>("/rover/rocker_right_corner_rb_controller/command", 1);
}

void moveForward(){
    publb.publish(msglb);publm.publish(msglm);
    publf.publish(msglf);pubrb.publish(msgrb);
    pubrm.publish(msgrm);pubrf.publish(msgrf);
    pubjlf.publish(msgjlf);pubjlb.publish(msgjlb);
    pubjrf.publish(msgjrf);pubjrb.publish(msgjrb);
}

int main(int argc,char** argv){
    ros::init(argc,argv,"execy");
    ros::NodeHandle nh;
    ros::NodeHandle nh_private("~");
    cmd(nh,nh_private);
    ros::Rate loopRate(10);

    while(ros::ok()){
        moveForward();
        ros::spinOnce();
        loopRate.sleep();
    }

    return 0;
}