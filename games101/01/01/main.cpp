#include "Triangle.hpp"
#include "rasterizer.hpp"
#include <eigen3/Eigen/Eigen>
#include <iostream>
#include <opencv4/opencv2/opencv.hpp>
using std::sin;
using std::cos;
constexpr double MY_PI = 3.1415926;

Eigen::Matrix4f get_view_matrix(Eigen::Vector3f eye_pos)
{
    Eigen::Matrix4f view = Eigen::Matrix4f::Identity();

    Eigen::Matrix4f translate;
    translate << 1, 0, 0, -eye_pos[0], 0, 1, 0, -eye_pos[1], 0, 0, 1,
        -eye_pos[2], 0, 0, 0, 1;

    view = translate * view;

    return view;
}

Eigen::Matrix4f get_model_matrix(float rotation_angle)
{
    float a = rotation_angle/180.0*std::acos(-1);
    Eigen::Matrix4f model = Eigen::Matrix4f::Identity();
    Eigen::Matrix4f translate;
    //绕z轴
    translate<< cos(a),-sin(a),0,0,
            sin(a),cos(a),0,0,
            0,0,1,0,
            0,0,0,1;
    //绕y轴
    /*translate << cos(a),0,sin(a),0,
                0,1,0,0,
                -sin(a),0,cos(a),0,
                0,0,0,1;*/
    // TODO: Implement this function
    // Create the model matrix for rotating the triangle around the Z axis.
    // Then return it.
    model = translate*model;
    return model;
}

Eigen::Matrix4f get_projection_matrix(float eye_fov, float aspect_ratio,
                                      float zNear, float zFar)
{
    // Students will implement this function

    Eigen::Matrix4f projection = Eigen::Matrix4f::Identity();
    float t = zNear*std::tan(eye_fov/2);
    float r = t*aspect_ratio;
    Eigen::Matrix4f translate;
    Eigen::Matrix4f ortho1,ortho2;
    //透视投影分为两步
    //1.挤压
    translate << zNear,0,0,0,
              0,zNear,0,0,
              0,0,zNear+zFar,-zNear*zFar,
              0,0,1,0;
    //2.正交投影
    ortho1<<-1/r,0,0,0,
            0,-1/t,0,0,
            0,0,2/(zFar-zNear),0,
            0,0,0,1;//压缩到(-1,1)的立方体中
    ortho2<<1,0,0,0,
            0,1,0,0,
            0,0,1,-(zNear+zFar)/2,
            0,0,0,1;//将立方体的中心移动到原点
    projection = ortho1*ortho2*translate*projection;
    // TODO: Implement this function
    // Create the projection matrix for the given parameters.
    // Then return it.

    return projection;
}
Eigen::Matrix4f get_rotation(Vector3f axis,float angle)//提高
{
    float a =angle/180.0*std::acos(-1);
    Eigen::Matrix3f I,N,R;
    I << 1,0,0,
        0,1,0,
        0,0,1;
    N << 0,-axis.z(),axis.y(),
        axis.z(),0,-axis.x(),
        -axis.y(),axis.x(),0;
    R = cos(a)*I+(1-cos(a))*(axis*axis.transpose())+sin(a)*N;
    Eigen::Matrix4f RR;
    RR << R(0,0),R(0,1),R(0,2),0,
            R(1,0),R(1,1),R(1,2),0,
            R(2,0),R(2,1),R(2,2),0,
            0,0,0,1;
    //std::cout<< a <<'\n'<<N<<'\n'<<R<<'\n'<<RR<<'\n';
    return RR;
}

int main(int argc, const char** argv)
{
    float angle = 0;
    bool command_line = false;
    std::string filename = "output.png";
    Vector3f axis;
    if (argc >= 3) {
        command_line = true;
        angle = std::stof(argv[2]); // -r by default
        if (argc >= 4) {
            filename = std::string(argv[3]);
            axis.x() = std::stof(argv[4]);
            axis.y() = std::stof(argv[5]); 
            axis.z() = std::stof(argv[6]);
        }
        else
            return 0;
    }

    rst::rasterizer r(700, 700);

    Eigen::Vector3f eye_pos = {0, 0, 10};

    std::vector<Eigen::Vector3f> pos{{2, 0, -2}, {0, 2, -2}, {-2, 0, -2}};

    std::vector<Eigen::Vector3i> ind{{0, 1, 2}};

    auto pos_id = r.load_positions(pos);
    auto ind_id = r.load_indices(ind);

    int key = 0;
    int frame_count = 0;

    if (command_line) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_rotation(axis,angle));
        //r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);
        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);

        cv::imwrite(filename, image);

        return 0;
    }

    while (key != 27) {
        r.clear(rst::Buffers::Color | rst::Buffers::Depth);

        r.set_model(get_model_matrix(angle));
        r.set_view(get_view_matrix(eye_pos));
        r.set_projection(get_projection_matrix(45, 1, 0.1, 50));

        r.draw(pos_id, ind_id, rst::Primitive::Triangle);

        cv::Mat image(700, 700, CV_32FC3, r.frame_buffer().data());
        image.convertTo(image, CV_8UC3, 1.0f);
        cv::imshow("image", image);
        key = cv::waitKey(10);

        std::cout << "frame count: " << frame_count++ << '\n';

        if (key == 'a') {
            angle += 10;
        }
        else if (key == 'd') {
            angle -= 10;
        }
    }

    return 0;
}
