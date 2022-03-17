#ifndef VIEWER_ROS_H
#define VIEWER_ROS_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <QObject>
#include <QThread>
#include <QMutex>
#include <QPixmap>
#include <QVector>
#include <QVector3D>
#include <QDateTime>
#include <QReadLocker>
#include <QPainter>
#include <QLabel>
#include <algorithm>
#include <ros/ros.h>
#include <ros/time.h>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <image_transport/image_transport.h>
#include <image_transport/transport_hints.h>
#include <cv_bridge/cv_bridge.h>

#include <sensor_msgs/PointCloud2.h>
#include <sensor_msgs/Image.h>
#include <sensor_msgs/LaserScan.h>
#include <rosgraph_msgs/Clock.h>

#include <camera_info_manager/camera_info_manager.h>
#include <std_msgs/String.h>
#include <std_msgs/Bool.h>
#include <std_srvs/SetBool.h>
#include <std_msgs/Int64MultiArray.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Float64.h>
#include <sensor_msgs/NavSatFix.h>
#include <novatel_gps_msgs/Inspva.h>
#include <sensor_msgs/LaserScan.h>
#include <sensor_msgs/CameraInfo.h>
#include <sensor_msgs/SetCameraInfo.h>
#include <irp_sen_msgs/vrs.h>
#include <irp_sen_msgs/altimeter.h>
#include <irp_sen_msgs/encoder.h>
#include <irp_sen_msgs/fog.h>
#include <irp_sen_msgs/imu.h>
#include <irp_sen_msgs/fog_3axis.h>
#include <irp_sen_msgs/LaserScanArray.h>

#include <sensor_msgs/Imu.h>
#include <sensor_msgs/MagneticField.h>
#include <nav_msgs/Odometry.h>
#include <geometry_msgs/Quaternion.h>
#include <tf/transform_datatypes.h>


#include <dynamic_reconfigure/server.h>
#include <file_player/dynamic_file_playerConfig.h>
#include <Eigen/Dense>
#include <thread>
#include <mutex>
#include <condition_variable>

//pcl
#include <pcl_conversions/pcl_conversions.h>
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

#include "file_player/color.h"
#include "rosbag/bag.h"
#include <ros/transport_hints.h>
#include "file_player/datathread.h"
#include <sys/types.h>

#include <algorithm>
#include <iterator>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>



using namespace std;
using namespace cv;

class ROSThread : public QThread
{
    Q_OBJECT

public:
    explicit ROSThread(QObject *parent = 0, QMutex *th_mutex = 0);
    ~ROSThread();
    void ros_initialize(ros::NodeHandle &n);
    void run();
    QMutex *mutex_;
    ros::NodeHandle nh_;
    ros::NodeHandle left_camera_nh_;
    ros::NodeHandle right_camera_nh_;

    ros::NodeHandle thermal_left_camera_nh_;
    ros::NodeHandle thermal_right_camera_nh_;

    ros::NodeHandle thermal_14bit_left_camera_nh_;
    ros::NodeHandle thermal_14bit_right_camera_nh_;

    boost::shared_ptr<camera_info_manager::CameraInfoManager> left_cinfo_;
    boost::shared_ptr<camera_info_manager::CameraInfoManager> right_cinfo_;

    boost::shared_ptr<camera_info_manager::CameraInfoManager> thermal_left_cinfo_;
    boost::shared_ptr<camera_info_manager::CameraInfoManager> thermal_right_cinfo_;

    boost::shared_ptr<camera_info_manager::CameraInfoManager> thermal_14bit_left_cinfo_;
    boost::shared_ptr<camera_info_manager::CameraInfoManager> thermal_14bit_right_cinfo_;

    int64_t initial_data_stamp_;
    int64_t last_data_stamp_;

    bool auto_start_flag_;
    int stamp_show_count_;

    bool play_flag_;
    bool pause_flag_;
    bool loop_flag_;
    bool stop_skip_flag_;
    double play_rate_;
    string data_folder_path_;

    int imu_data_version_;

    void Ready();
    void ResetProcessStamp(int position);

signals:
    void StampShow(quint64 stamp);
    void StartSignal();

private:

    int search_bound_;

    bool stereo_active_;
    bool stereo_thermal_active_;
    bool stereo_thermal_14bit_left_active_;
    bool stereo_thermal_14bit_right_active_;

    bool omni_active_;

    ros::Subscriber start_sub_;
    ros::Subscriber stop_sub_;

    ros::Publisher gps_pub_;
    ros::Publisher inspva_pub_;

    ros::Publisher gps_odometry_pub_;
    ros::Publisher imu_origin_pub_;
    ros::Publisher imu_pub_;
    ros::Publisher magnet_pub_;
    ros::Publisher ouster_pub_;

    ros::Publisher stereo_left_pub_;
    ros::Publisher stereo_right_pub_;
    ros::Publisher stereo_thermal_14bit_left_pub_;
    ros::Publisher stereo_thermal_14bit_right_pub_;

    ros::Publisher stereo_left_info_pub_;
    ros::Publisher stereo_right_info_pub_;

    ros::Publisher stereo_thermal_left_info_pub_;
    ros::Publisher stereo_thermal_right_info_pub_;
    ros::Publisher stereo_thermal_14bit_left_info_pub_;
    ros::Publisher stereo_thermal_14bit_right_info_pub_;

    ros::Publisher clock_pub_;

    int64_t prev_clock_stamp_;

    multimap<int64_t, string>                    data_stamp_;
    map<int64_t, nav_msgs::Odometry>     odometry_data_;
    map<int64_t, sensor_msgs::NavSatFix>    gps_data_;
    map<int64_t, novatel_gps_msgs::Inspva>    inspva_data_;
    map<int64_t, nav_msgs::Odometry>     gps_odometry_data_;

    map<int64_t, irp_sen_msgs::imu>         imu_data_origin_;
    map<int64_t, sensor_msgs::Imu>         imu_data_;
    map<int64_t, sensor_msgs::MagneticField>         mag_data_;

    DataThread<int64_t> data_stamp_thread_;
    DataThread<int64_t> gps_thread_;
    DataThread<int64_t> inspva_thread_;
    DataThread<int64_t> imu_thread_;
    DataThread<int64_t> stereo_thread_;
    DataThread<int64_t> stereo_thermal_14bit_left_thread_;
    DataThread<int64_t> stereo_thermal_14bit_right_thread_;
    DataThread<int64_t> ouster_thread_;

    map<int64_t, int64_t> stop_period_; //start and stop stamp

    void DataStampThread();
    void GpsThread();
    void InspvaThread();
    void ImuThread();
    void OusterThread();

    void StereoThread();
    void StereoThermal14BitLeftThread();
    void StereoThermal14BitRightThread();

    void FilePlayerStart(const std_msgs::BoolConstPtr& msg);
    void FilePlayerStop(const std_msgs::BoolConstPtr& msg);

    vector<string> ouster_file_list_;
    vector<string> stereo_file_list_;
    vector<string> stereo_thermal_14bit_left_file_list_;
    vector<string> stereo_thermal_14bit_right_file_list_;

    ros::Timer timer_;
    void TimerCallback(const ros::TimerEvent&);
    int64_t processed_stamp_;
    int64_t pre_timer_stamp_;
    bool reset_process_stamp_flag_;

    pair<string,sensor_msgs::PointCloud2> ouster_next_;

    pair<string,cv::Mat> stereo_left_next_img_;
    pair<string,cv::Mat> stereo_right_next_img_;
    pair<string,cv::Mat> stereo_thermal_14bit_left_next_img_;
    pair<string,cv::Mat> stereo_thermal_14bit_right_next_img_;

    sensor_msgs::CameraInfo stereo_left_info_;
    sensor_msgs::CameraInfo stereo_right_info_;
    sensor_msgs::CameraInfo stereo_thermal_14bit_left_info_;
    sensor_msgs::CameraInfo stereo_thermal_14bit_right_info_;

    int GetDirList(string dir, vector<string> &files);


public slots:

};

#endif // VIEWER_LCM_H
