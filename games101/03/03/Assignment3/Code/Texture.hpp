//
// Created by LEI XU on 4/27/19.
//

#ifndef RASTERIZER_TEXTURE_H
#define RASTERIZER_TEXTURE_H
#include "global.hpp"
#include <eigen3/Eigen/Eigen>
#include <opencv4/opencv2/opencv.hpp>
class Texture{
private:
    cv::Mat image_data;

public:
    Texture(const std::string& name)
    {
        image_data = cv::imread(name);
        cv::cvtColor(image_data, image_data, cv::COLOR_RGB2BGR);
        width = image_data.cols;
        height = image_data.rows;
    }

    int width, height;

    Eigen::Vector3f getColor(float u, float v)
    {
        if (u < 0) u = 0;
        if (u > 1) u = 1;
        if (v < 0) v = 0;
        if (v > 1) v = 1;
        auto u_img = u * width;
        auto v_img = (1 - v) * height;
        

        auto color = image_data.at<cv::Vec3b>(v_img, u_img);
        return Eigen::Vector3f(color[0], color[1], color[2]);
    }

    Eigen::Vector3f getColorBilinear(float u,float v)
    {
        if (u < 0) u = 0;
        if (u > 1) u = 1;
        if (v < 0) v = 0;
        if (v > 1) v = 1;
        auto u_img = u * width;
        auto v_img = (1 - v) * height;
        int iu_img = static_cast<int>(u_img);
        int iv_img = static_cast<int>(v_img);
        int offset_x = (u_img-iu_img)>0.5?1:-1;
        int offset_y = (v_img-iv_img)>0.5?1:-1;
        int u1 = iu_img+offset_x;
        int u2 = iv_img+offset_y;

        auto color = image_data.at<cv::Vec3b>(iv_img, iu_img);
        auto color2 = image_data.at<cv::Vec3b>(iv_img, u1);
        auto color3 = image_data.at<cv::Vec3b>(v1, iu_img);
        auto color4 = image_data.at<cv::Vec3b>(v1, u1);

        //auto colormid1=
        //auto colormid2=
        //auto aolor = 
        return Eigen::Vector3f(color[0], color[1], color[2]);
    }

};
#endif //RASTERIZER_TEXTURE_H
