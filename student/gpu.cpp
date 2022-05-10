/*!
 * @file
 * @brief This file contains implementation of gpu
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include <student/gpu.hpp>
#include <cmath>

float CLR_MAX = 255.0;

struct trngl {
    OutVertex p[3];
};

/**
 * @brief Find area of the triangle using Heron's formula
 *
 * @param x1 side1 x
 * @param x2 side2 x
 * @param x3 side3 x
 * @param y1 side1 y
 * @param y2 side2 y
 * @param y3 side3 y
 */
double calculate_area(double x1, double x2, double x3, double y1, double y2, double y3) {
    long double side1 = sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
    long double side2 = sqrt(pow(x2 - x3, 2) + pow(y2 - y3, 2));
    long double side3 = sqrt(pow(x3 - x1, 2) + pow(y3 - y1, 2));
    double perm = (side1 + side2 + side3) / 2;    // triangle perimeter divided by 2
    return sqrt(perm * (perm - side1) * (perm - side2) * (perm - side3));   // calculate and return area of the triangle
}

/**
 * Triangle rasterization
 * 
 * @param t triangle to rasterize
 * @param prg active program
 * @param f active frame
 */
void rasterization(trngl t, Program prg, Frame &f) {
    // declare variables
    int minX, maxX, minY, maxY, pixel;
    double deltaX1, deltaX2, deltaX3, 
           deltaY1, deltaY2, deltaY3, 
           edge1, edge2, edge3, 
           trngl_area, result, point_bar;
    std::vector<double> bar_crds(3);
    // find minimum and maximum coordinates and crop edges based on window
    minX = glm::max(0, (int) glm::min(glm::min(t.p[0].gl_Position[0], t.p[1].gl_Position[0]), t.p[2].gl_Position[0]));
    minY = glm::max(0, (int) glm::min(glm::min(t.p[0].gl_Position[1], t.p[1].gl_Position[1]), t.p[2].gl_Position[1]));
    maxX = glm::min((int) f.width - 1, (int) glm::max(glm::max(t.p[0].gl_Position[0], t.p[1].gl_Position[0]), t.p[2].gl_Position[0]));
    maxY = glm::min((int) f.height - 1, (int) glm::max(glm::max(t.p[0].gl_Position[1], t.p[1].gl_Position[1]), t.p[2].gl_Position[1]));
    // calculate triangle area
    trngl_area = calculate_area(t.p[0].gl_Position[0], t.p[1].gl_Position[0], 
                                t.p[2].gl_Position[0], t.p[0].gl_Position[1], 
                                t.p[1].gl_Position[1], t.p[2].gl_Position[1]);
    // find edge function parameters
    deltaX1 = t.p[1].gl_Position[0] - t.p[0].gl_Position[0];
    deltaY1 = t.p[1].gl_Position[1] - t.p[0].gl_Position[1];
    deltaX2 = t.p[2].gl_Position[0] - t.p[1].gl_Position[0];
    deltaY2 = t.p[2].gl_Position[1] - t.p[1].gl_Position[1];
    deltaX3 = t.p[0].gl_Position[0] - t.p[2].gl_Position[0];
    deltaY3 = t.p[0].gl_Position[1] - t.p[2].gl_Position[1];

    edge1 = (minY - (t.p[0].gl_Position[1] - 0.5)) * deltaX1 - (minX - (t.p[0].gl_Position[0] - 0.5)) * deltaY1;
    edge2 = (minY - (t.p[1].gl_Position[1] - 0.5)) * deltaX2 - (minX - (t.p[1].gl_Position[0] - 0.5)) * deltaY2;
    edge3 = (minY - (t.p[2].gl_Position[1] - 0.5)) * deltaX3 - (minX - (t.p[2].gl_Position[0] - 0.5)) * deltaY3;

    for (int y = minY; y <= maxY; y++) {
        bool right = (y - minY) % 2 == 0;
        for (int x = right ? minX : maxX; right ? x <= maxX : x >= minX; x += right ? 1 : -1) {
            // if pixel in the trinagle area, put it
            if (edge1 >= 0 && edge2 >= 0 && edge3 >= 0) {
                // calclulate barycentrics coordinates
                bar_crds[0] = calculate_area(t.p[1].gl_Position[0], t.p[2].gl_Position[0], x + 0.5, t.p[1].gl_Position[1], t.p[2].gl_Position[1], y + 0.5) / trngl_area;
                bar_crds[1] = calculate_area(t.p[2].gl_Position[0], t.p[0].gl_Position[0], x + 0.5, t.p[2].gl_Position[1], t.p[0].gl_Position[1], y + 0.5) / trngl_area;
                bar_crds[2] = calculate_area(t.p[0].gl_Position[0], t.p[1].gl_Position[0], x + 0.5, t.p[0].gl_Position[1], t.p[1].gl_Position[1], y + 0.5) / trngl_area;
                point_bar = (bar_crds[0] / t.p[0].gl_Position[3]) + (bar_crds[1] / t.p[1].gl_Position[3]) + (bar_crds[2] / t.p[2].gl_Position[3]);
                // fragment creation
                InFragment fragIn;
                OutFragment fragOut;

                fragIn.gl_FragCoord.x = x + 0.5;
                fragIn.gl_FragCoord.y = y + 0.5;
                fragIn.gl_FragCoord.z = t.p[0].gl_Position[2] * bar_crds[0] + t.p[1].gl_Position[2] * bar_crds[1] + t.p[2].gl_Position[2] * bar_crds[2];
                
                bar_crds[0] /= (t.p[0].gl_Position[3] * point_bar);
                bar_crds[1] /= (t.p[1].gl_Position[3] * point_bar);
                bar_crds[2] /= (t.p[2].gl_Position[3] * point_bar);

                for (int i = 0; i < maxAttributes; i++) {
                    if (prg.vs2fs[i] == AttributeType::VEC3) {
                        for (int j = 0; j <= 2; j++) {
                            result = 0;
                            for (int k = 0; k <= 2; k++) {
                                result += bar_crds[k] * t.p[k].attributes[0].v3[j];
                            }
                            fragIn.attributes[0].v3[j] = result;     
                        }
                    } else if (prg.vs2fs[i] == AttributeType::VEC4) {
                        for (int l = 0; l <= 3; l++) {
                            result = 0;
                            for (int m = 0; m <= 2; m++) {
                                result += bar_crds[m] * t.p[m].attributes[0].v4[l];
                            }
                            fragIn.attributes[0].v4[l] = result;     
                        }
                    }
                } 
                // call fragment shader
                prg.fragmentShader(fragOut, fragIn, prg.uniforms);

                pixel = x + y * f.width;
                // per fragment operations
                float r, g, b, a;
                a = fragOut.gl_FragColor.w > 1 ? 1.0 : fragOut.gl_FragColor.w;
                r = fragOut.gl_FragColor.r > 1 ? 1.0 : fragOut.gl_FragColor.r;
                g = fragOut.gl_FragColor.g > 1 ? 1.0 : fragOut.gl_FragColor.g;
                b = fragOut.gl_FragColor.b > 1 ? 1.0 : fragOut.gl_FragColor.b;

                if (a < 1) {
                    r =  a * r + (f.color[pixel * 4] / CLR_MAX) * (1 - a);
                    if (r > 1) r = 1.0;
                    g = a * g + (f.color[(pixel * 4) + 1] / CLR_MAX) * (1 - a); 
                    if (g > 1) g = 1.0;
                    b = a * b + (f.color[(pixel * 4) + 2] / CLR_MAX) * (1 - a);
                    if (b > 1) b = 1.0;
                }

                if (fragIn.gl_FragCoord.z < f.depth[pixel]) {
                    if (a > 0.5) f.depth[pixel] = fragIn.gl_FragCoord.z;
                    pixel *= 4;
                    f.color[pixel] = r * CLR_MAX;
                    f.color[pixel + 1] = g * CLR_MAX;
                    f.color[pixel + 2] = b * CLR_MAX;
                }
            }
            // correction
            if (!(right && x == maxX) && (right || !(x == minX))) {
                edge1 += right ? -deltaY1 : deltaY1;
                edge2 += right ? -deltaY2 : deltaY2;
                edge3 += right ? -deltaY3 : deltaY3;
            }
        }
        edge1 += deltaX1;
        edge2 += deltaX2;
        edge3 += deltaX3;
    }
}

/**
 * Triangle raterization
 * 
 * @param vao active vertex array
 * @param vtx vertex
 * @param prg active program
 * @param t triangle
 */
void vertex_assembly(VertexArray vao, uint32_t vtx, Program prg, trngl *t) {
    uint32_t *index32bit = (uint32_t *)vao.indexBuffer;
    uint16_t *index16bit = (uint16_t *)vao.indexBuffer;
    uint8_t *index8bit = (uint8_t *)vao.indexBuffer;
    // going through all vertexes
    for (uint32_t vertex = 0; vertex <= 2; vertex++) {
        InVertex vIn;
        // assembly vertex
        if (vao.indexBuffer != nullptr) {
            if (vao.indexType == IndexType::UINT32) {
                vIn.gl_VertexID = index32bit[vtx + vertex];
            } else if (vao.indexType == IndexType::UINT16) {
                vIn.gl_VertexID = index16bit[vtx + vertex];
            } else if (vao.indexType == IndexType::UINT8) {
                vIn.gl_VertexID = index8bit[vtx + vertex];
            }
        // array is unindexed
        } else {
            vIn.gl_VertexID = vertex + vtx;
        }
        // vertex attributes configuration
        for (int i = 0; i < maxAttributes; i++) {
            uint8_t *cor = vao.vertexAttrib[i].offset + vIn.gl_VertexID * vao.vertexAttrib[i].stride + ((uint8_t *) vao.vertexAttrib[i].bufferData);
            
            if (vao.vertexAttrib[i].type == AttributeType::VEC4) {
                for (int j = 0; j <= 3; j++) {
                    uint8_t *p = cor + j * 4;
                    vIn.attributes[i].v4[j] = *((float *)p);
                }
            } else if (vao.vertexAttrib[i].type == AttributeType::VEC3) {
                for (int j = 0; j <= 2; j++) {
                    uint8_t *p = cor + j * 4;
                    vIn.attributes[i].v3[j] = *((float *)p);
                }
            } else if (vao.vertexAttrib[i].type == AttributeType::VEC2) {
                for (int j = 0; j <= 1; j++) {
                    uint8_t *p = cor + j * 4;
                    vIn.attributes[i].v2[j] = *((float *)p);
                }
            } else if (vao.vertexAttrib[i].type == AttributeType::FLOAT) {
                vIn.attributes[i].v1 = *((float *)cor);
            }
        }
        // call vertex shader
        prg.vertexShader(t->p[vertex], vIn, prg.uniforms);
    }
}

//! [drawImpl]
void drawImpl(GPUContext &ctx, uint32_t nofVertices)
{
  for (uint32_t vtx = 0; vtx < nofVertices; vtx += 3) {
    trngl t;
    // assembly vertexes
    vertex_assembly(ctx.vao, vtx, ctx.prg, &t);
    // normalization, perspective division
    for (int i = 0; i <= 2; i++) {
        for (int j = 0; j <= 2; j++) {
            t.p[i].gl_Position[j] = t.p[i].gl_Position[j] / t.p[i].gl_Position[3];
        }
    }
    // viewport transformation
    for (int i = 0; i <= 2; i++) {
        t.p[i].gl_Position[0] = ctx.frame.width * (0.5 + t.p[i].gl_Position[0] * 0.5);
        t.p[i].gl_Position[1] = ctx.frame.height * (0.5 + t.p[i].gl_Position[1] * 0.5);
    }
    rasterization(t, ctx.prg, ctx.frame);
  }
}
//! [drawImpl]

/**
 * @brief This function reads color from texture.
 *
 * @param texture texture
 * @param uv uv coordinates
 *
 * @return color 4 floats
 */
glm::vec4 read_texture(Texture const &texture, glm::vec2 uv)
{
  if (!texture.data)
    return glm::vec4(0.f);
  auto uv1 = glm::fract(uv);
  auto uv2 = uv1 * glm::vec2(texture.width - 1, texture.height - 1) + 0.5f;
  auto pix = glm::uvec2(uv2);
  // auto t   = glm::fract(uv2);
  glm::vec4 color = glm::vec4(0.f, 0.f, 0.f, 1.f);
  for (uint32_t c = 0; c < texture.channels; ++c)
    color[c] = texture.data[(pix.y * texture.width + pix.x) * texture.channels + c] / 255.f;
  return color;
}

/**
 * @brief This function clears framebuffer.
 *
 * @param ctx GPUContext
 * @param r red channel
 * @param g green channel
 * @param b blue channel
 * @param a alpha channel
 */
void clear(GPUContext &ctx, float r, float g, float b, float a)
{
  auto &frame = ctx.frame;
  auto const nofPixels = frame.width * frame.height;
  for (size_t i = 0; i < nofPixels; ++i)
  {
    frame.depth[i] = 10e10f;
    frame.color[i * 4 + 0] = static_cast<uint8_t>(glm::min(r * 255.f, 255.f));
    frame.color[i * 4 + 1] = static_cast<uint8_t>(glm::min(g * 255.f, 255.f));
    frame.color[i * 4 + 2] = static_cast<uint8_t>(glm::min(b * 255.f, 255.f));
    frame.color[i * 4 + 3] = static_cast<uint8_t>(glm::min(a * 255.f, 255.f));
  }
}

