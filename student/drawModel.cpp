/*!
 * @file
 * @brief This file contains functions for model rendering
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */
#include <student/drawModel.hpp>
#include <student/gpu.hpp>


/**
 * @brief This function renders a model
 *
 * @param ctx GPUContext
 * @param matrix identity matrix
 * @param model model structure
 * @param node node to draw
 */
void drawNode(GPUContext &ctx, glm::mat4 matrix, Model model, Node node){
    if (node.mesh >= 0){
        ctx.vao.vertexAttrib[0] = model.meshes[node.mesh].position;
        ctx.vao.vertexAttrib[1] = model.meshes[node.mesh].normal;
        ctx.vao.vertexAttrib[2] = model.meshes[node.mesh].texCoord;

        ctx.prg.uniforms.uniform[1].m4 = matrix * node.modelMatrix;
        // inverse matrix
        ctx.prg.uniforms.uniform[2].m4 = glm::inverse(matrix*node.modelMatrix);
        // transpose matrix
        ctx.prg.uniforms.uniform[2].m4 = glm::transpose(ctx.prg.uniforms.uniform[2].m4);
        ctx.prg.uniforms.uniform[5].v4 = model.meshes[node.mesh].diffuseColor;
        ctx.prg.uniforms.uniform[6].v1 = model.meshes[node.mesh].diffuseTexture >= 0 ? 1.0 : 0.0;
        ctx.prg.uniforms.textures[0] = model.meshes[node.mesh].diffuseTexture >= 0 ? model.textures[model.meshes[node.mesh].diffuseTexture] : Texture{};
        // call draw
        draw(ctx, model.meshes[node.mesh].nofIndices);
    }
 
    for(size_t i = 0;i < node.children.size(); ++i){
        drawNode(ctx, matrix * node.modelMatrix, model, node.children[i]);
    }
}

/**
 * @brief This function renders a model
 *
 * @param ctx GPUContext
 * @param model model structure
 * @param proj projection matrix
 * @param view view matrix
 * @param light light position
 * @param camera camera position (unused)
 */
//! [drawModel]
void drawModel(GPUContext&ctx,Model const&model,glm::mat4 const&proj,glm::mat4 const&view,glm::vec3 const&light,glm::vec3 const&camera){
    ctx.prg.fragmentShader = drawModel_fragmentShader;
    ctx.prg.vertexShader = drawModel_vertexShader;

    glm::mat4 ident_matrix = glm::mat4(1.f);
    for(size_t i=0;i<model.roots.size();++i){
        drawNode(ctx, ident_matrix, model, model.roots[i]);
    }
}
//! [drawModel]

/**
 * @brief This function represents vertex shader of texture rendering method.
 *
 * @param outVertex output vertex
 * @param inVertex input vertex
 * @param uniforms uniform variables
 */
//! [drawModel_vs]
void drawModel_vertexShader(OutVertex&outVertex,InVertex const&inVertex,Uniforms const&uniforms){
    (void)outVertex;
    (void)inVertex;
    (void)uniforms;
}
//! [drawModel_vs]

/**
 * @brief This functionrepresents fragment shader of texture rendering method.
 *
 * @param outFragment output fragment
 * @param inFragment input fragment
 * @param uniforms uniform variables
 */
//! [drawModel_fs]
void drawModel_fragmentShader(OutFragment&outFragment,InFragment const&inFragment,Uniforms const&uniforms){
  (void)outFragment;
  (void)inFragment;
  (void)uniforms;
  /// \todo Tato funkce reprezentujte fragment shader.<br>
  /// Vaším úkolem je správně obarvit fragmenty a osvětlit je pomocí lambertova osvětlovacího modelu.
  /// Bližší informace jsou uvedeny na hlavní stránce dokumentace.
}
//! [drawModel_fs]

