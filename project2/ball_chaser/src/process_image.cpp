#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;
// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
    ROS_INFO_STREAM("Drive bot to the ball - linear_x: " + std::to_string(lin_x) + ", angular_z: " + std::to_string(ang_z));
    
    ball_chaser::DriveToTarget srv;
    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if (!client.call(srv)) {
        ROS_ERROR("Failed to call service ball_chaser");
    }
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    int left = img.step / 3, right = 2 * img.step / 3;
    int area[3] = {0}; // 0:left, 1:forward, 2:right

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    // Request a stop when there's no white ball seen by the camera
    //int pos = 0;
    for (int i = 0; i < img.height * img.step - 3; i+=3) {
        if (img.data[i] == white_pixel && img.data[i+1] == white_pixel && img.data[i+2] == white_pixel) {
            if ((i % img.step) < left) {
                area[0]++;
            } else if ((i % img.step) >= left && (i % img.step) < right) {
                area[1]++;
            } else {
                area[2]++;
            }
        }
        //pos++;
    }
    ROS_INFO_STREAM("Area[0]: " + std::to_string(area[0]) + "\n");
    ROS_INFO_STREAM("Area[1]: " + std::to_string(area[1]) + "\n");
    ROS_INFO_STREAM("Area[2]: " + std::to_string(area[2]) + "\n");

    int max = 0, sec = 42;
    for (int i = 0; i < 3; i++) {
        if (max < area[i]) {
            max = area[i];
            sec = i;
        }
    }
    
    if (sec == 0) {
        drive_robot(0.3, 0.3);
        ROS_INFO_STREAM("Left");
    } else if (sec == 1) {
        drive_robot(0.3, 0.0);
        ROS_INFO_STREAM("Forward");
    } else if (sec == 2) {
        drive_robot(0.3, -0.3);
        ROS_INFO_STREAM("Right");
    } else {
        drive_robot(0.0, 0.0);
	ROS_INFO_STREAM("Stop");
    }
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
