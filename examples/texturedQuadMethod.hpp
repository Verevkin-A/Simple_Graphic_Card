/*!
 * @file
 * @brief This file contains texture rendering method
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#pragma once

#include <framework/method.hpp>

namespace texturedQuad{

class ConstructionData: public MethodConstructionData{
  public:
    ConstructionData(std::string imageFile):imageFile(imageFile){}
    virtual ~ConstructionData(){}
    std::string imageFile;
};

/**
 * @brief This class represents texture rendering method
 */
class Method: public ::Method{
  public:
    Method(ConstructionData const*mcd);
    Method(MethodConstructionData const*mcd):Method((ConstructionData const*)mcd){}
    virtual ~Method();
    virtual void onDraw(Frame&frame,SceneParam const&sceneParam) override;
    GPUContext ctx;///< gpu context
    TextureData tex;///< texture
};

}
