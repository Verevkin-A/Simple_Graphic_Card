/*!
 * @file
 * @brief This file contains phong rendering method
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <framework/bunny.hpp>
#include <framework/method.hpp>


namespace phongMethod{

/**
 * @brief This class holds all variables of phong method.
 */
class Method: public ::Method{
  public:
    Method(MethodConstructionData const*);
    Method():Method(nullptr){}
    virtual ~Method();
    virtual void onDraw(Frame&frame,SceneParam const&sceneParam) override;
    GPUContext ctx;
};

}
