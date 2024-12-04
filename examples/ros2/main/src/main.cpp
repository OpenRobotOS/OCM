#include "rclcpp/rclcpp.hpp"
#include "ros2_msg/msg/my_message.hpp"
#include "rclcpp/serialization.hpp"
#include "ocm/shared_memory_topic_ros2.hpp"

int main(int argc, char ** argv)
{
    rclcpp::init(argc, argv);
    openrobot::ocm::SharedMemoryTopic topic;
    // 使用 std_msgs::msg::String 进行序列化
    rclcpp::Serialization<ros2_msg::msg::MyMessage> serializer;
    ros2_msg::msg::MyMessage msg;
    msg.data = "Hello, ROS 2!";
    topic.Publish("test", "test", msg);
    // 创建一个序列化消息对象
    rclcpp::SerializedMessage serialized_msg;
    
    // 使用 SerializedMessage 进行序列化
    serializer.serialize_message(&msg, &serialized_msg);

    // 反序列化
    ros2_msg::msg::MyMessage new_msg;
    serializer.deserialize_message(&serialized_msg, &new_msg);

    std::cout << "Deserialized message: " << new_msg.data << std::endl;
    topic.Subscribe<ros2_msg::msg::MyMessage>("test", "test", [](ros2_msg::msg::MyMessage msg){
        std::cout << "Received message: " << msg.data << std::endl;
    });
    
    rclcpp::shutdown();
    return 0;
}
