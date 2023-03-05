//
// Created by Göksu Güvendiren on 2019-05-14.
//

#include "Scene.hpp"


void Scene::buildBVH() {
    printf(" - Generating BVH...\n\n");
    this->bvh = new BVHAccel(objects, 1, BVHAccel::SplitMethod::NAIVE);
}

Intersection Scene::intersect(const Ray &ray) const
{
    return this->bvh->Intersect(ray);
}

void Scene::sampleLight(Intersection &pos, float &pdf) const
{
    float emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
        }
    }
    float p = get_random_float() * emit_area_sum;
    emit_area_sum = 0;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        if (objects[k]->hasEmit()){
            emit_area_sum += objects[k]->getArea();
            if (p <= emit_area_sum){
                objects[k]->Sample(pos, pdf);
                break;
            }
        }
    }
}

bool Scene::trace(
        const Ray &ray,
        const std::vector<Object*> &objects,
        float &tNear, uint32_t &index, Object **hitObject)
{
    *hitObject = nullptr;
    for (uint32_t k = 0; k < objects.size(); ++k) {
        float tNearK = kInfinity;
        uint32_t indexK;
        Vector2f uvK;
        if (objects[k]->intersect(ray, tNearK, indexK) && tNearK < tNear) {
            *hitObject = objects[k];
            tNear = tNearK;
            index = indexK;
        }
    }


    return (*hitObject != nullptr);
}

// Implementation of Path Tracing
Vector3f Scene::castRay(const Ray &ray, int depth) const
{
    // TO DO Implement Path Tracing Algorithm here
    Intersection p = intersect(ray);
    if(p.happened==false) return {0.f,0.f,0.f};
    if(p.m->hasEmission()) return p.m->getEmission();

    Vector3f wo = ray.direction;
    Intersection inter;
    float pdf_light = 0.f;
    sampleLight(inter,pdf_light);
    Vector3f inter_p = inter.coords-p.coords;
    Vector3f ws = inter_p.normalized();
   
    
    Vector3f NN = inter.normal.normalized(), N = p.normal.normalized();
    Vector3f L_dir, L_indir;
    
    Intersection inter1 = intersect(Ray(p.coords,ws));
    if(inter1.distance-inter_p.norm() > -0.0001)
    {   
        
        L_dir = inter.emit*p.m->eval(wo,ws,N)*
              dotProduct(ws,N)*dotProduct(-ws,NN)/
              dotProduct(inter_p,inter_p)/
              pdf_light;
        
    }

   
    if(get_random_float()>RussianRoulette)
        return L_dir;
    
    Vector3f wi = normalize(p.m->sample(wo,N));
    
    Ray p_wi(p.coords,wi);
    Intersection q = intersect(p_wi);
    
    if(q.happened==true&&q.m->hasEmission()==false)
        L_indir = castRay(p_wi,depth+1)*p.m->eval(wo,wi,N)
            *dotProduct(wi,N)/p.m->pdf(wo,wi,N)/RussianRoulette;
    return L_dir+L_indir;
               
}
