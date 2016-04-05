#ifndef SCANMATCHER
#define SCANMATCHER

#include <exception>
#include <memory>

#include <pcl/point_cloud.h>
#include <pcl/point_types.h>

#include <nav_msgs/Odometry.h>
#include <tf/transform_datatypes.h>

#include <Eigen/Dense>
#include <Eigen/Geometry> 

#include <ml_ndt_scanmatching/other/layer.h>
#include <ml_ndt_scanmatching/other/field.h>
#include <ml_ndt_scanmatching/eigen_tools.h>

#include <ml_ndt_scanmatching/NDTMapMsg.h>

#ifndef DEBUG
  #define DEBUG(out); std::cout<<out<<"\n";
#endif

class Field;
class Layer;

class Scanmatcher {
public:
  typedef size_t Id_t;
  typedef Eigen::Vector3d pose_t;
  typedef Eigen::Vector2d point_t;
  typedef Eigen::Transform<double,2, Eigen::TransformTraits::Affine> transform_t;
  typedef pcl::PointCloud<pcl::PointXYZ> pcl_t;
  typedef std::vector<point_t> points2_t;
  typedef std::vector<Eigen::Vector3d> points3_t;
  
  Scanmatcher(double max_range=4.0, size_t resolution=8, size_t layers=4)
      : pose_(pose_t::Zero()), max_range_(max_range), resolution_(resolution), layers_count_(layers),
        initialized_(false), points_(points2_t(360)) {}

  void initialize(const pose_t &pose, const points2_t &points);
  void initialize(const pose_t &pose, const pcl_t &points);
  bool calculate(const pose_t &pose, const points2_t &points);
  bool calculate(const pose_t &pose, const pcl_t &points);

  pose_t calculate(const pose_t &prev_pose, const points2_t &first_scan, const pose_t &curr_pose,
                   const points2_t &second_scan);
  bool match(const pcl_t & source,const pcl_t & target, transform_t & trans, const transform_t & init_guess = transform_t::Identity());
  bool matchBiber(const pcl_t & source,const pcl_t & target, transform_t & trans, const transform_t & init_guess = transform_t::Identity());
  pose_t getTransformation() const;
  tf::Transform getTFTransform() const;
  pose_t getPose() const;
  nav_msgs::Odometry getOdom() const;
  ml_ndt_scanmatching::NDTMapMsg getLayerData(size_t layer_id) const;

  point_t getPoint(Id_t id) const;

  void setResolution(const size_t res);
  void setLayers(const size_t layers);
  void setMaxRange(const double range);

private:
  pose_t pose_;
  pose_t last_odom_;
  transform_t transform_;
 // transformation beetween last odometry pose and calculated pose
  //transform_t odom_pose_trans_;
  double max_range_;
  size_t resolution_;
  size_t layers_count_;
  bool initialized_;
  points2_t points_;
  std::vector<Layer> layer_;

  const double PI_F = 3.14159265358979;
  const double EPSILON = 0.0001;
  const double MAX_ITER = 50;

  points2_t projectPointsTo2D(const pcl_t &points);
  void createLayers();

  void initializeNdt(const pose_t &pose, const points2_t &points);
  void initializeNdt(const pose_t &pose, points2_t && points);
  void updateLayers(const pose_t & calc_pose,const pose_t & odom,const points2_t & points);

  bool calculateNdt(const pose_t &pose, const points2_t &points);
  double scoreLayers(const transform_t & trans, const points2_t & cloud_in) const;
  // calculates transform between last stored pose and new pose as a parameter
};

#endif