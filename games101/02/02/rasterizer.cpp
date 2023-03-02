// clang-format off
//
// Created by goksu on 4/6/19.
//

#include <algorithm>
#include <vector>
#include "rasterizer.hpp"
#include <opencv2/opencv.hpp>
#include <math.h>


rst::pos_buf_id rst::rasterizer::load_positions(const std::vector<Eigen::Vector3f> &positions)
{
    auto id = get_next_id();
    pos_buf.emplace(id, positions);

    return {id};
}

rst::ind_buf_id rst::rasterizer::load_indices(const std::vector<Eigen::Vector3i> &indices)
{
    auto id = get_next_id();
    ind_buf.emplace(id, indices);

    return {id};
}

rst::col_buf_id rst::rasterizer::load_colors(const std::vector<Eigen::Vector3f> &cols)
{
    auto id = get_next_id();
    col_buf.emplace(id, cols);

    return {id};
}

auto to_vec4(const Eigen::Vector3f& v3, float w = 1.0f)
{
    return Vector4f(v3.x(), v3.y(), v3.z(), w);
}


static bool insideTriangle(float x, float y, const Vector3f* _v)
{   
    Vector3f v[3];
    v[0]={_v[0].x(),_v[0].y(),1.0f};
    v[1]={_v[1].x(),_v[1].y(),1.0f};
    v[2]={_v[2].x(),_v[2].y(),1.0f};
    Vector3f point = {x,y,1.0f};

    auto v01 = v[0]-v[1],v0p = v[0]-point,
        v12=v[1]-v[2],v1p=v[1]-point,
        v20=v[2]-v[0],v2p=v[2]-point;
    return v01.cross(v0p).z()>0&&v12.cross(v1p).z()>0&&v20.cross(v2p).z()>0
        || v01.cross(v0p).z()<0&&v12.cross(v1p).z()<0&&v20.cross(v2p).z()<0;
    // TODO : Implement this function to check if the point (x, y) is inside the triangle represented by _v[0], _v[1], _v[2]
}

static std::tuple<float, float, float> computeBarycentric2D(float x, float y, const Vector3f* v)
{
    float c1 = (x*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*y + v[1].x()*v[2].y() - v[2].x()*v[1].y()) / (v[0].x()*(v[1].y() - v[2].y()) + (v[2].x() - v[1].x())*v[0].y() + v[1].x()*v[2].y() - v[2].x()*v[1].y());
    float c2 = (x*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*y + v[2].x()*v[0].y() - v[0].x()*v[2].y()) / (v[1].x()*(v[2].y() - v[0].y()) + (v[0].x() - v[2].x())*v[1].y() + v[2].x()*v[0].y() - v[0].x()*v[2].y());
    float c3 = (x*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*y + v[0].x()*v[1].y() - v[1].x()*v[0].y()) / (v[2].x()*(v[0].y() - v[1].y()) + (v[1].x() - v[0].x())*v[2].y() + v[0].x()*v[1].y() - v[1].x()*v[0].y());
    return {c1,c2,c3};
}

void rst::rasterizer::draw(pos_buf_id pos_buffer, ind_buf_id ind_buffer, col_buf_id col_buffer, Primitive type)
{
    auto& buf = pos_buf[pos_buffer.pos_id];
    auto& ind = ind_buf[ind_buffer.ind_id];
    auto& col = col_buf[col_buffer.col_id];

    float f1 = (50 - 0.1) / 2.0;
    float f2 = (50 + 0.1) / 2.0;
    std::vector<Triangle> tt;
    Eigen::Matrix4f mvp = projection * view * model;
    for (auto& i : ind)
    {
        Triangle t;
        Eigen::Vector4f v[] = {
                mvp * to_vec4(buf[i[0]], 1.0f),
                mvp * to_vec4(buf[i[1]], 1.0f),
                mvp * to_vec4(buf[i[2]], 1.0f)
        };
        //Homogeneous division
        for (auto& vec : v) {
            vec /= vec.w();
        }
        //Viewport transformation
        for (auto & vert : v)
        {
            vert.x() = 0.5*width*(vert.x()+1.0);
            vert.y() = 0.5*height*(vert.y()+1.0);
            vert.z() = vert.z() * f1 + f2;
        }

        for (int i = 0; i < 3; ++i)
        {
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
            t.setVertex(i, v[i].head<3>());
        }

        auto col_x = col[i[0]];
        auto col_y = col[i[1]];
        auto col_z = col[i[2]];

        t.setColor(0, col_x[0], col_x[1], col_x[2]);
        t.setColor(1, col_y[0], col_y[1], col_y[2]);
        t.setColor(2, col_z[0], col_z[1], col_z[2]);
        tt.emplace_back(t); 
    }
    if(MSAA)
        rasterize_triangles(tt);
    else
        for(auto&t:tt)
            rasterize_triangle(t);
}
void rst::rasterizer::rasterize_triangles(const std::vector<Triangle> tt)
{
    for(int i = 0;i<tt.size();++i)//初始化color_buf,
    {
        auto v = tt[i].toVector4();
        int min_x = std::min(std::min(v[0].x(), v[1].x()), v[2].x());
        int min_y = std::min(std::min(v[0].y(), v[1].y()), v[2].y());
        int max_x = std::max(std::max(v[0].x(), v[1].x()), v[2].x());
        int max_y = std::max(std::max(v[0].y(), v[1].y()), v[2].y());
         Vector2f a[4] = { {0.25f,0.25f},{0.25f,0.75f},
                    {0.75f,0.25f},{0.75f,0.75f}};
        for (int x = min_x; x <= max_x; ++x) 
        {
            for (int y = min_y; y <= max_y; ++y) 
            {
                int index = get_index(x,y);
                if(insideTriangle(x+0.5, y+0.5, tt[i].v))
                {
                    auto[alpha,beta,gamma]=computeBarycentric2D(x, y, tt[i].v);
                    float w_reciprocal = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                    float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                    z_interpolated *= w_reciprocal;
                    color_buf[index].emplace(z_interpolated,pixel_buf{i+1,tt[i].getColor()});
                }
            }
        }
    }
    for(int i = 0;i<tt.size();++i)//根据color_buf里的值计算
    {
        auto v = tt[i].toVector4();
        int min_x = std::min(std::min(v[0].x(), v[1].x()), v[2].x());
        int min_y = std::min(std::min(v[0].y(), v[1].y()), v[2].y());
        int max_x = std::max(std::max(v[0].x(), v[1].x()), v[2].x());
        int max_y = std::max(std::max(v[0].y(), v[1].y()), v[2].y());
        Vector2f a[4] = { {0.25f,0.25f},{0.25f,0.75f},
                    {0.75f,0.25f},{0.75f,0.75f}};
        if(depth_buf_MSAA.size()==0)
        {
            depth_buf_MSAA.resize(height*width*4);
            std::fill(depth_buf_MSAA.begin(), depth_buf_MSAA.end(), std::numeric_limits<float>::lowest());
        }
        
        for (int x = min_x; x <= max_x; ++x) 
        {
            for (int y = min_y; y <= max_y; ++y) 
            {
                Vector3f w_color={0.f,0.f,0.f};
                float w_depth=0.0f;
                int index = get_index(x, y);
                int flag = 0;
                for(int j = 0;j<4;++j)
                {
                    if(insideTriangle(x+a[j].x(),y+a[j].y(),tt[i].v))
                    {
                        auto[alpha,beta,gamma]=computeBarycentric2D(x+a[j].x(), y+a[j].y(), tt[i].v);
                        float w_reciprocal = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                        float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                        z_interpolated *= w_reciprocal;
                        int MSAA_index = index*4+j;
                        w_depth=std::max(z_interpolated,w_depth);
                        if(z_interpolated>depth_buf_MSAA[MSAA_index])//当前三角形深度值大于depth_buf_MSAA[index]中的值，则更新depth_buf_MSAA,并选取当前三角形颜色
                        {
                             depth_buf_MSAA[MSAA_index] = z_interpolated;
                             w_color+=tt[i].getColor();
                        }//若小于，则选择buf中下一层的颜色
                        else
                        {
                            w_color+=(--(--color_buf[index].end()))->second.color;
                        }
                    }
                    else//若当前点不在三角形里，则判断当前color_buf中最高层是否为当前层，若是，则选择下一层的颜色，若不是，则选择最高层的颜色
                    {
                        if((--color_buf[index].end())->second.id==(i+1))
                            w_color+=(--(--color_buf[index].end()))->second.color;
                        else
                            w_color+=(--color_buf[index].end())->second.color;
                    }
                }
                
                    if(w_depth>depth_buf[index])
                    {
                        depth_buf[index] = w_depth;
                        set_pixel({(float)x,(float)y,0.f},w_color*0.25);
                    }
            }
        }
    }
}
//Screen space rasterization
void rst::rasterizer::rasterize_triangle(const Triangle& t) {
    auto v = t.toVector4();
    int min_x = std::min(std::min(v[0].x(), v[1].x()), v[2].x());
    int min_y = std::min(std::min(v[0].y(), v[1].y()), v[2].y());
    int max_x = std::max(std::max(v[0].x(), v[1].x()), v[2].x());
    int max_y = std::max(std::max(v[0].y(), v[1].y()), v[2].y());
    
    
        
        
        for (int x = min_x; x <= max_x; ++x) 
        {
            for (int y = min_y; y <= max_y; ++y) 
            {
                if(insideTriangle(x+0.5f, y+0.5f, t.v))
                {
                    auto[alpha,beta,gamma]=computeBarycentric2D(x, y, t.v);
                    float w_reciprocal = 1.0 / (alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
                    float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
                    z_interpolated *= w_reciprocal;
                    int index = get_index(x, y);
                    if(z_interpolated>depth_buf[index])
                    {
                        depth_buf[index] = z_interpolated;
                        set_pixel({(float)x,(float)y,1.0f}, t.getColor());
                    }
                }
            }
        }
    
    // TODO : Find out the bounding box of current triangle.
    // iterate through the pixel and find if the current pixel is inside the triangle

    // If so, use the following code to get the interpolated z value.
    //auto[alpha, beta, gamma] = computeBarycentric2D(x, y, t.v);
    //float w_reciprocal = 1.0/(alpha / v[0].w() + beta / v[1].w() + gamma / v[2].w());
    //float z_interpolated = alpha * v[0].z() / v[0].w() + beta * v[1].z() / v[1].w() + gamma * v[2].z() / v[2].w();
    //z_interpolated *= w_reciprocal;

    // TODO : set the current pixel (use the set_pixel function) to the color of the triangle (use getColor function) if it should be painted.
}

void rst::rasterizer::set_model(const Eigen::Matrix4f& m)
{
    model = m;
}

void rst::rasterizer::set_view(const Eigen::Matrix4f& v)
{
    view = v;
}

void rst::rasterizer::set_projection(const Eigen::Matrix4f& p)
{
    projection = p;
}

void rst::rasterizer::clear(rst::Buffers buff)
{
    if ((buff & rst::Buffers::Color) == rst::Buffers::Color)
    {
        std::fill(frame_buf.begin(), frame_buf.end(), Eigen::Vector3f{0, 0, 0});
    }
    if ((buff & rst::Buffers::Depth) == rst::Buffers::Depth)
    {
        std::fill(depth_buf.begin(), depth_buf.end(), std::numeric_limits<float>::lowest());
        std::map<float,pixel_buf> m = {{0,{0,{0.f,0.f,0.f}}}};
        
        std::fill(color_buf.begin(),color_buf.end(),m);
    }
}

rst::rasterizer::rasterizer(int w, int h) : width(w), height(h),MSAA(false)
{
    frame_buf.resize(w * h);
    depth_buf.resize(w * h);
    color_buf.resize(w * h);
}

int rst::rasterizer::get_index(int x, int y)
{
    return (height-1-y)*width + x;
}

void rst::rasterizer::set_pixel(const Eigen::Vector3f& point, const Eigen::Vector3f& color)
{
    //old index: auto ind = point.y() + point.x() * width;
    auto ind = (height-1-point.y())*width + point.x();
    frame_buf[ind] = color;

}

// clang-format on
