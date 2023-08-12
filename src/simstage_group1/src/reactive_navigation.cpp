/*the reactive navigation node subscribes the data from the sensor and based 
on that publishes the commads of velocity*/

#include "ros/ros.h" 
#include "sensor_msgs/LaserScan.h"  // sensor message type
#include "geometry_msgs/Twist.h"    // cmd_vel message type


double distance;  //store the distance that the sensor read 

void laserCallback(const sensor_msgs::LaserScan::ConstPtr& msg){ 
    //where we manipulate and work with the message from the sensor

    distance = *std::min_element (msg->ranges.begin(), msg->ranges.begin());
    ROS_INFO("Distance read: %lf", *std::min_element (msg->ranges.begin(), msg->ranges.begin()));
}


int main(int argc, char **argv){

    ros::init(argc, argv, "reactive_navigation");  //name of the node : reactive naigation
    ros::NodeHandle n;  //the main acces point for communicate with the ROS sytem

    ros::Publisher pub_cmd_vel = n.advertise<geometry_msgs::Twist>("cmd_vel", 100); //Publisher object 
    ros::Subscriber sub_laser = n.subscribe("base_scan", 100, laserCallback);  //Subscribing the massage from base_scan

    ros::Rate loop_rate(10); //10 Hrtz

    geometry_msgs::Twist msg_cmd_vel; //The message object that will be publish

    while(ros::ok){
        //Define and send messages in a loop

        if (distance <= 0.88){

            for (int i = 0; i <= 4; i ++){ 
            //turn slowly for verify the distance while turn

            msg_cmd_vel.linear.x = 0.0;  //linear velocity
            msg_cmd_vel.angular.z = 0.1; //angular velocity  

            if (distance > 1.4){ //if it isn't near to some obstacle, stop turn

            msg_cmd_vel.linear.x = 1.0;  //linear velocity
            msg_cmd_vel.angular.z = 0.0; //angular velocity

            pub_cmd_vel.publish(msg_cmd_vel);  //Publishing message in cmd_vel
            loop_rate.sleep();
            break;
            }

            //always when made a decision, publish and subscribe again for based on that make another decision
            pub_cmd_vel.publish(msg_cmd_vel); //Publishing the message in cmd_vel
            n.subscribe("base_scan", 100, laserCallback);

            loop_rate.sleep();
            }

        } else {

            msg_cmd_vel.linear.x = 0.5;
            msg_cmd_vel.angular.z = 0.0;
        }

        pub_cmd_vel.publish(msg_cmd_vel); //Publishing the message in cmd_vel
        ros::spinOnce(); //just spin the while one time
        loop_rate.sleep();
    }

    return 0;
}