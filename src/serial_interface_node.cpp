#include <ros/ros.h>
#include <std_msgs/String.h>
#include "../include/SerialInterface.hpp"

int main(int argc, char **argv)
{
    ros::init(argc, argv, "serial_interface_node");
    ros::NodeHandle n;
    ros::Rate r(10);
    ros::Rate r_connect(0.5);

    SerialInterface serialInterface;

    char port_name_buffer[SD_MAX_PORT_NAME_LEN + 1];

    strcpy(port_name_buffer, "ttyACM0");

    ROS_INFO("log:%s", "start");

    while (n.ok())
    {
        if (serialInterface.check_is_ready())
        {
            ReceiveData receiveData;
            serialInterface.get_data(&receiveData);

            ROS_INFO("ReceiveData.count %d", receiveData.count);
            ROS_INFO("ReceveData.timeStamp: %f", receiveData.time_stamp); // seconds
            ROS_INFO("ReceveData.rightInput: %f", receiveData.right_input);
            ROS_INFO("ReceveData.leftInput: %f", receiveData.left_input);
            ROS_INFO("ReceveData.rightAngVel: %f", receiveData.right_ang_vel);
            ROS_INFO("ReceveData.leftAngVel: %f", receiveData.left_ang_vel);
            ROS_INFO("ReceveData.rightAng: %f", receiveData.right_ang);
            ROS_INFO("ReceveData.leftAng: %f", receiveData.left_ang);

            SendData sendData;
            sendData.right_vel_setpoint = 3.0;
            sendData.left_vel_setpoint = 2.5;

            if (!serialInterface.send_data(&sendData))
            {
                ROS_ERROR("send data fail");
            }
        }
        else
        {
            serialInterface.setup_serial_driver(port_name_buffer);
            r_connect.sleep();
        }

        r.sleep();
    }

    serialInterface.close_port();

    return 0;
}