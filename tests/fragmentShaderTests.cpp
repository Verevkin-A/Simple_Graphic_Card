#include <tests/catch.hpp>

#include <iostream>
#include <string.h>
#include <map>
#include <set>
#include <algorithm>
#include <numeric>
#include <cstring>

#include <glm/gtc/matrix_transform.hpp>

#include <student/gpu.hpp>
#include <framework/framebuffer.hpp>
#include <tests/testCommon.hpp>

using namespace tests;

namespace dtl{

void initOutVertices(){
  outVertices.clear();
  outVertices.resize(3);
  outVertices[0].gl_Position=glm::vec4(-1.f,-1.f,-0.f,1.f);
  outVertices[1].gl_Position=glm::vec4(+1.f,-1.f,-0.f,1.f);
  outVertices[2].gl_Position=glm::vec4(-1.f,+1.f,-0.f,1.f);
}

float area(glm::vec2 const&A,glm::vec2 const&B,glm::vec2 const&C){
  auto a=glm::length(B-A);
  auto b=glm::length(C-B);
  auto c=glm::length(A-C);
  auto s = (a+b+c)/2.f;
  return glm::sqrt(s*glm::abs(s-a)*glm::abs(s-b)*glm::abs(s-c));
}

glm::vec3 barycentrics(OutVertex const&a,OutVertex const&b,OutVertex const&c,glm::vec2 x,uint32_t w,uint32_t h){
  glm::vec4 clip[3]={a.gl_Position,b.gl_Position,c.gl_Position};
  glm::vec3 ndc[3];
  for(int i=0;i<3;++i)ndc[i] = glm::vec3(clip[i])/clip[i].w;
  glm::vec3 wc[3];
  for(int i=0;i<3;++i)wc[i] = glm::vec3((glm::vec2(ndc[i])*.5f+.5f)*glm::vec2(w,h),ndc[i].z);
  float T = area(wc[0],wc[1],wc[2]);
  glm::vec3 l;
  for(int i=0;i<3;++i)l[i] = area(wc[(i+1)%3],wc[(i+2)%3],x)/T;
  return l;
}

glm::vec3 barycentricsPerspective(OutVertex const&a,OutVertex const&b,OutVertex const&c,glm::vec2 x,uint32_t w,uint32_t h){
  auto l = barycentrics(a,b,c,x,w,h);
  glm::vec4 clip[3]={a.gl_Position,b.gl_Position,c.gl_Position};
  for(int i=0;i<3;++i)l[i] /= clip[i].w;
  l /= glm::dot(l,glm::vec3(1.f));
  return l;
}

glm::uvec2 computeNofRasterizedFragments(glm::vec2 const&a,glm::vec2 const&b,glm::vec2 const&c,glm::uvec2 const&res,bool backFaceCulling){
  glm::vec2 P[3]={a,b,c};
  glm::vec2 E[3];for(int i=0;i<3;++i)E[i]=P[(i+1)%3]-P[i];
  glm::vec2 N[3];for(int i=0;i<3;++i)N[i]=glm::vec2(-E[i].y,E[i].x);
  glm::vec3 p[3];for(int i=0;i<3;++i)p[i]=glm::vec3(N[i],-dot(N[i],P[i]));

  if(!backFaceCulling)
    if(glm::dot(p[0],glm::vec3(P[2],1.f))<0.f)
      for(int i=0;i<3;++i)p[i]*=-1.f;

  glm::vec4 border = glm::ivec4(res,0,0);
  for(int i=0;i<3;++i){
    border.x=glm::min(border.x,P[i].x);
    border.y=glm::min(border.y,P[i].y);
    border.z=glm::max(border.z,P[i].x);
    border.w=glm::max(border.w,P[i].y);
  }
  border += glm::vec4(-1,-1,+1,+1);
  border = glm::clamp(border,glm::vec4(0,0,0,0),glm::vec4(res,res));
  auto bb = glm::uvec4(border);

  uint32_t counter=0;
  for(uint32_t y=bb.y;y<bb.w;++y)
    for(uint32_t x=bb.x;x<bb.z;++x){
      auto sample = glm::vec3(glm::vec2(x,y)+0.5f,1.f);
      bool inside = true;
      for(int i=0;i<3;++i)inside &= dot(p[i],sample)>=0.f;
      if(inside)counter++;
    }
  uint32_t allowedErr = (bb.z-bb.x) + 2*(bb.w-bb.y);
  return glm::uvec2(counter,allowedErr);
}

struct V2{
  V2(){}
  V2(glm::vec2 const&a):data(a){}
  glm::vec2 data = glm::vec2(0.f);
};

bool operator<(V2 const&a,V2 const&b){
  if(a.data.x < b.data.x)return true;
  if(a.data.x > b.data.x)return false;
  if(a.data.y < b.data.y)return true;
  if(a.data.y > b.data.y)return false;
  return false;
}

struct UV2{
  UV2(){}
  UV2(glm::uvec2 const&a):data(a){}
  glm::uvec2 data = glm::uvec2(0);
};

bool operator<(UV2 const&a,UV2 const&b){
  if(a.data.x < b.data.x)return true;
  if(a.data.x > b.data.x)return false;
  if(a.data.y < b.data.y)return true;
  if(a.data.y > b.data.y)return false;
  return false;
}


void triangleRasterizationTest(glm::vec4 const&a,glm::vec4 const&b,glm::vec4 const&c,glm::uvec2 const&res,bool backFaceCulling){

  inFragments.clear();
  outVertices.clear();
  outVertices.resize(3);
  outVertices[0].gl_Position=a;
  outVertices[1].gl_Position=b;
  outVertices[2].gl_Position=c;
  glm::vec3 ndc      [3];for(int i=0;i<3;++i)ndc[i] = glm::vec3(outVertices[i].gl_Position)/outVertices[i].gl_Position.w;
  glm::vec3 viewSpace[3];for(int i=0;i<3;++i)viewSpace[i] = glm::vec3((glm::vec2(ndc[i])*.5f+.5f)*glm::vec2(res),ndc[i].z);

  std::cerr << "  rasterizace troj??heln??ku: A=" << str(glm::vec2(viewSpace[0])) << " B=" <<str(glm::vec2(viewSpace[1])) << " C=" << str(glm::vec2(viewSpace[2]));
  std::cerr << " p??i rozli??en??: " << str(res) << " a backFaceCulling: " << str(backFaceCulling) << " dopadla: ";

  auto framebuffer = std::make_shared<Framebuffer>(res.x,res.y);
  GPUContext ctx;
  ctx.frame = framebuffer->getFrame();
  ctx.prg.vertexShader   = vertexShaderInject;
  ctx.prg.fragmentShader = fragmentShaderDump;
  ctx.backfaceCulling    = backFaceCulling   ;

  draw(ctx,3);


  bool success = true;
  
  auto expectedCount = computeNofRasterizedFragments(viewSpace[0],viewSpace[1],viewSpace[2],res,backFaceCulling);
  success &= equalCounts(inFragments.size(),expectedCount.x,expectedCount.y);

  if(!success){
    std::cerr << "??PATN??!" << std::endl;
    std::cerr << R".(
    Tento test zkou???? vyrasterizovat troj??heln??k a po????t?? mno??stv?? vyresterizovan??ch fragment??.

    Pro spr??vn?? fungov??n?? je pot??eba sestavit troj??heln??k, prov??st perspektivn?? d??len??, prov??st view-port transformaci.

    Vrchol co vyleze z vertex shaderu (x,y,z,w)
    Po perspektivn??m d??len?? (x/w,y/w,z/w), m??lo by to b??t v rozsahu [-1,-1,-1] -> [+1,+1,-1]
    Po view-port trasnformaci ((x/w*.5+.5)*w,(y/w*.5+.5)*h,...) , m??lo by to b??t v rozsahu [0,0] -> [w,h].

    Co se v tomto testu d??lo?
    Framebuffer m?? velikost: )."<<res.x<<"x"<<res.y<<R".(

    Rasterizuje se troj??heln??k, kter?? m?? sou??adnice v clip space:
    A.gl_Position = vec4)."<<str(outVertices[0].gl_Position)<<R".(;
    B.gl_Position = vec4)."<<str(outVertices[1].gl_Position)<<R".(;
    C.gl_Position = vec4)."<<str(outVertices[2].gl_Position)<<R".(;

    Co?? odpov??d?? rasterizaci troj??heln??k?? v NDC (po perspektivn??m d??len??):
    A = vec3)."<<str(ndc[0])<<R".(;
    B = vec3)."<<str(ndc[1])<<R".(;
    C = vec3)."<<str(ndc[2])<<R".(;

    Co?? odpov??d?? rasterizaci troj??heln??k?? ve view-space:
    A = vec3)."<<str(viewSpace[0])<<R".(;
    B = vec3)."<<str(viewSpace[1])<<R".(;
    C = vec3)."<<str(viewSpace[2])<<R".(;

    Pozor na backFaceCulling, pokud je zapnut?? (ctx.backFaceCulling==true),
    troj??heln??ky, kter?? jsou specifikov??ny po sm??ru hodinov??ch ru??i??ek nejsou vykresleny.
    V tomto testu byl backFaceCulling=)."<<str(backFaceCulling)<<R".(

    po??et vyrasterizovan??ch fragmentu: )."<<inFragments.size()<<R".(
    p??ibli??n?? po??et fragment??, kter?? se m??l vyrasterizovat: )."<<expectedCount.x<<R".(
    s odchylkou: )."<<expectedCount.y<<std::endl;

    std::cerr << R".(
    N??pov??da k implementaci:

    struct Triangle{
      OutVertex points[3];
    };
    
    void loadTriangle(Triangle&triangle,Program const&ptr,VertexArray const&vao,uint32_t tId){
      for(...){ // smy??ka p??es vrcholy troj??heln??ku
        InVertex inVertex;
        loadVertex(inVertex,vao,tId+i);
        prg.vertexShader(triangle.points[i],inVertex,...);
      }
    }

    void rasterize(Frame&frame,Triangle const&triangle,Program const&prg){
      ...
      // spo????tat hranice, troj??heln??ka
      // 
     
      if(!backFaceCulling){
        // pokud je vypnuty back face culling, zobrazuj?? se i odvracene troj??heln??ky
        // prohod?? se, co je uvnit?? a venku
      }

      for() // smy??ka p??es pixely
        for(){
          if(pixel in triangle){
            InFragment inFragment;
            OutFragment outFragment;
            prg.fragmentShader(outFragment,inFragment,...);
            ...
          }
        }
    }

    void drawImpl(GPUContext& ctx,uint32_t nofVertices){

      for( t...){//smy??ka p??e?? troj??heln??ky
        Triangle triangle;
        loadTriangle(triangle,ctx.vao,t);

        perspectiveDivision(triangle);

        viewportTransformation(triangle,ctx.frame.width,ctx.frame.height);

        rasterize(ctx.frame,triangle,ctx.prg);

      }

    }

    ).";

    REQUIRE(false);
  }else
    std::cerr << "DOB??E!" << std::endl;
}

}

using namespace dtl;

SCENARIO("12"){
  std::cerr << "12 - rasterization should produce correct number of fragments" << std::endl;

  std::vector<glm::uvec2>resolutions = {
    glm::uvec2(100,100),
    glm::uvec2(200,100),
    glm::uvec2(100,200),
    glm::uvec2(337,113),
  };

  struct Triangle{
    Triangle(){}
    Triangle(glm::vec4 const&a,glm::vec4 const&b,glm::vec4 const&c):a(a),b(b),c(c){}
    glm::vec4 a=glm::vec4(1.f);
    glm::vec4 b=glm::vec4(1.f);
    glm::vec4 c=glm::vec4(1.f);
  };

  std::vector<Triangle>triangles = {
    {glm::vec4(-1.0,-1.0,-1,1),glm::vec4(+1.0,+0.0,-1,1),glm::vec4(+0.0,+1.0,-1,1)},
    {glm::vec4(-0.5,-0.6,-1,1),glm::vec4(+0.3,-0.1,-1,1),glm::vec4(-0.6,+0.9,-1,1)},
    {glm::vec4(-1.0,-1.0,-1,1),glm::vec4(+1.0,-1.0,-1,1),glm::vec4(-1.0,+1.0,-1,1)},
    {glm::vec4(+0.0,+0.0,-1,1),glm::vec4(+1.0,+0.0,-1,1),glm::vec4(+0.0,+1.0,-1,1)},
    {glm::vec4(-2.0,-1.0,-1,1),glm::vec4(+1.0,-1.0,-1,1),glm::vec4(-2.0,+1.0,-1,1)},
    {glm::vec4(-1.0,-1.0,-1,1),glm::vec4(+2.0,-1.0,-1,1),glm::vec4(-1.0,+1.0,-1,1)},
    {glm::vec4(-1.0,-2.0,-1,1),glm::vec4(+1.0,-2.0,-1,1),glm::vec4(-1.0,+1.0,-1,1)},
    {glm::vec4(+0.0,+0.0,-1,1),glm::vec4(+0.0,+0.0,-1,1),glm::vec4(+0.0,+0.0,-1,1)},
    {glm::vec4(+0.0,+0.0,-1,1),glm::vec4(+0.0,+0.0,-1,1),glm::vec4(+1.0,+1.0,-1,1)},
    {glm::vec4(+0.0,+0.0,-1,1),glm::vec4(+0.0,+0.0,-1,1),glm::vec4(-1.0,+1.0,-1,1)},
    {glm::vec4(+0.0,+0.0,-1,1),glm::vec4(+0.0,+0.0,-1,1),glm::vec4(-1.0,-1.0,-1,1)},
    {glm::vec4(+0.0,+0.0,-1,1),glm::vec4(+0.0,+0.0,-1,1),glm::vec4(+1.0,-1.0,-1,1)},
    {glm::vec4(-0.4,-1.3,-1,1),glm::vec4(+1.2,+0.2,-1,1),glm::vec4(-0.9,+1.4,-1,1)},
    {glm::vec4(-2.0,-2.0,-1,1),glm::vec4(+8.0,-2.0,-1,1),glm::vec4(-2.0,+8.0,-1,1)},
  };

  for(int b=0;b<2;++b)//backfaceculling false/true
    for(int f=0;f<2;++f)//CCW/CW
      for(auto const&t:triangles)
        for(auto const&r:resolutions){
          if(f==0)//flip CCW to CW
            dtl::triangleRasterizationTest(t.a,t.b,t.c,r,(bool)b);
          else
            dtl::triangleRasterizationTest(t.a,t.c,t.b,r,(bool)b);
        }
}



SCENARIO("13"){
  std::cerr << "13 - fragment shader uniforms" << std::endl;
  uint32_t w = 100;
  uint32_t h = 100;
  GPUContext ctx;
  auto framebuffer = std::make_shared<Framebuffer>(w,h);
  ctx.frame = framebuffer->getFrame();

  ctx.prg.vertexShader   = vertexShaderInject;
  ctx.prg.fragmentShader = fragmentShaderDump;

  auto const value =glm::ortho(1.f,2.f,3.f,5.f,6.f,8.f);
  auto const u = 3;
  ctx.prg.uniforms.uniform[u].m4 = value;

  initOutVertices();

  draw(ctx,3);

  if(unif.uniform[u].m4 != value){
    std::cerr << "Tento test zkou????, zda fragment shader obdr??el uniformn?? prom??nn??" << std::endl;
    REQUIRE(false);
  }
}

SCENARIO("14"){
  std::cerr << "14 - perspective division" << std::endl;
  auto res = glm::uvec2(100,100);
  auto backFaceCulling = true;

  float f0 = 0.1f;
  float f1 = 0.3f;
  float f2 = 4.f;
  initOutVertices();
  outVertices[0].gl_Position*=f0;
  outVertices[1].gl_Position*=f1;
  outVertices[2].gl_Position*=f2;
  inFragments.clear();
  glm::vec3 ndc      [3];for(int i=0;i<3;++i)ndc[i] = glm::vec3(outVertices[i].gl_Position)/outVertices[i].gl_Position.w;
  glm::vec3 viewSpace[3];for(int i=0;i<3;++i)viewSpace[i] = glm::vec3((glm::vec2(ndc[i])*.5f+.5f)*glm::vec2(res),ndc[i].z);


  auto framebuffer = std::make_shared<Framebuffer>(res.x,res.y);
  GPUContext ctx;
  ctx.frame = framebuffer->getFrame();
  ctx.prg.vertexShader   = vertexShaderInject;
  ctx.prg.fragmentShader = fragmentShaderDump;
  ctx.backfaceCulling    = backFaceCulling;

  draw(ctx,3);

  bool success = true;

  auto expectedCount = computeNofRasterizedFragments(viewSpace[0],viewSpace[1],viewSpace[2],res,backFaceCulling);
  success &= equalCounts(inFragments.size(),expectedCount.x,expectedCount.y);

  if(!success){
    std::cerr << R".(
    Tento test zkou????, zda se prov??d?? perspektivn?? d??len??.
    Z vertex shaderu vych??zej?? vrcholy, kter?? maj?? homogenn?? slo??ku jinou od 1.
   
    Framebuffer m?? rozli??en??: )."<<res.x<<"x"<<res.y<<R".(

    Rasterizuje se troj??heln??k, kter?? m?? sou??adnice v clip space:
    A.gl_Position = vec4)."<<str(outVertices[0].gl_Position)<<R".(;
    B.gl_Position = vec4)."<<str(outVertices[1].gl_Position)<<R".(;
    C.gl_Position = vec4)."<<str(outVertices[2].gl_Position)<<R".(;

    Co?? odpov??d?? rasterizaci troj??heln??k?? v NDC (po perspektivn??m d??len??):
    A = vec3)."<<str(ndc[0])<<R".(;
    B = vec3)."<<str(ndc[1])<<R".(;
    C = vec3)."<<str(ndc[2])<<R".(;

    Co?? odpov??d?? rasterizaci troj??heln??k?? ve view-space:
    A = vec3)."<<str(viewSpace[0])<<R".(;
    B = vec3)."<<str(viewSpace[1])<<R".(;
    C = vec3)."<<str(viewSpace[2])<<R".(;

    po??et vyrasterizovan??ch fragmentu: )."<<inFragments.size()<<R".(
    p??ibli??n?? po??et fragment??, kter?? se m??l vyrasterizovat: )."<<expectedCount.x<<R".(
    s odchylkou: )."<<expectedCount.y<<std::endl;

    REQUIRE(false);
  }

}



SCENARIO("15"){
  std::cerr << "15 - gl_FragCoord.xy should be inside triangle" << std::endl;


  initOutVertices();
  inFragments.clear();

  auto res = glm::uvec2(100,100);
  auto framebuffer = std::make_shared<Framebuffer>(res.x,res.y);
  GPUContext ctx;
  ctx.frame = framebuffer->getFrame();
  ctx.prg.vertexShader   = vertexShaderInject;
  ctx.prg.fragmentShader = fragmentShaderDump;


  draw(ctx,3);

  uint32_t expectedCount = res.x*res.y/2;
  uint32_t err = res.x;

  bool success = true;

  success &= equalCounts(inFragments.size(),expectedCount,err);

  glm::vec3 ndc      [3];for(int i=0;i<3;++i)ndc[i] = glm::vec3(outVertices[i].gl_Position)/outVertices[i].gl_Position.w;
  glm::vec3 viewSpace[3];for(int i=0;i<3;++i)viewSpace[i] = glm::vec3((glm::vec2(ndc[i])*.5f+.5f)*glm::vec2(res),ndc[i].z);

  std::map<V2,size_t>fragPos;

  std::set<V2>duplicatedPositions;
  for(auto const&f:inFragments){
    success &= f.gl_FragCoord.x >   0.f;
    success &= f.gl_FragCoord.y <  100.f;
    success &= lessFloat(f.gl_FragCoord.x + f.gl_FragCoord.y,(float)res.y);

    auto pos = glm::vec2(f.gl_FragCoord);
    auto it = fragPos.find(pos);
    if(it == fragPos.end())
      fragPos[pos] = 0;

    fragPos[pos]++;

    if(fragPos.at(pos)>1)duplicatedPositions.insert(pos);
  }

  success &= duplicatedPositions.empty();
  
  if(!success){
    std::cerr << R".(
    Tento test zkou????, zda je pozice fragment?? gl_FragCoord.xy vr??mci okna a troj??heln??ku

    Framebuffer m?? rozli??en??: )."<<res.x<<"x"<<res.y<<R".(

    Rasterizuje se troj??heln??k, kter?? pokr??v?? polovinu obrazovky:
    A.gl_Position = vec4)."<<str(outVertices.at(0).gl_Position)<<R".(;
    B.gl_Position = vec4)."<<str(outVertices.at(1).gl_Position)<<R".(;
    C.gl_Position = vec4)."<<str(outVertices.at(2).gl_Position)<<R".(;

    Co?? odpov??d?? rasterizaci troj??heln??ka ve view-space:
    A = vec3)."<<str(viewSpace[0])<<R".(;
    B = vec3)."<<str(viewSpace[1])<<R".(;
    C = vec3)."<<str(viewSpace[2])<<R".(;
     ________
    |*      |
    |***    |
    |*****  |
    |*******|

    O??ek??v?? se, ??e gl_FragCoord.xy ka??d??ho fragmentu bude v rozmez??

    x > 0 // lev?? hranice okna
    y > 0 // doln?? hranice okna
    y < )."<<res.x<<R".( // horn?? hranice okna
    x < )."<<res.y<<R".( // prav?? hranice okna
    x+y <= )."<<res.y<<R".( // p??epona troj??heln??ku

    Pozice fragmentu gl_FragCoord.xy by m??la odpov??dat pozici pixelu + 0.5.
    Fragment pro lev?? doln?? pixel obrazovky bude m??t sou??adnice (0.5,0.5).
    Fragment pro prav?? horn?? pixel obrazovky bude m??t sou??adnice ()."<<res.x-1.f+0.5f<<R".(,)."<<res.y-1.f+0.5f<<R".().

    Byly nalezeny fragmenty, kter?? maj?? ??patn?? gl_FragCoord.xy

    ).";
    if(!duplicatedPositions.empty()){
    std::cerr << R".(
    Byly nalezeny fragmenty, kter?? maj?? stejn?? pozice, co?? nesm?? nastat.
    Jsou to t??eba tyto fragmenty:)." << std::endl;
      for(auto const&d:duplicatedPositions){
        std::cerr << "    gl_Position.xy == " << str(d.data) << " se vyskytl: " << fragPos.at(d) << "x."<<std::endl;
      }
    }
    REQUIRE(false);
  }

}

SCENARIO("16"){
  std::cerr << "16 - depth interpolation" << std::endl;

  uint32_t w = 100;
  uint32_t h = 100;
  GPUContext ctx;
  auto framebuffer = std::make_shared<Framebuffer>(w,h);
  ctx.frame = framebuffer->getFrame();

  outVertices.clear();
  inFragments.clear();

  float hc[3] = {1,2,.5};
  float zz[3] = {.9f,.4f,.8f};
  outVertices.resize(3);

  outVertices[0].gl_Position=glm::vec4(-hc[0],-hc[0],zz[0],hc[0]);
  outVertices[1].gl_Position=glm::vec4(+hc[1],-hc[1],zz[1],hc[1]);
  outVertices[2].gl_Position=glm::vec4(-hc[2],+hc[2],zz[2],hc[2]);

  ctx.prg.vertexShader   = vertexShaderInject;
  ctx.prg.fragmentShader = fragmentShaderDump;

  draw(ctx,3);

  bool success = true;

  uint32_t expectedCount = w*h/2;
  uint32_t err = w;
  success &= equalCounts(inFragments.size(),expectedCount,err);

  std::vector<InFragment>badFragments;

  for(auto const&f:inFragments){
    auto l = barycentrics(outVertices[0],outVertices[1],outVertices[2],f.gl_FragCoord,w,h);

    auto att = 
      outVertices[0].gl_Position.z/outVertices[0].gl_Position.w * l[0]+
      outVertices[1].gl_Position.z/outVertices[1].gl_Position.w * l[1]+
      outVertices[2].gl_Position.z/outVertices[2].gl_Position.w * l[2];

    bool good = equalFloats(f.gl_FragCoord.z,att); 

    if(!good)
      badFragments.push_back(f);

    success &= good;
  }

  glm::vec3 ndc      [3];for(int i=0;i<3;++i)ndc[i] = glm::vec3(outVertices[i].gl_Position)/outVertices[i].gl_Position.w;
  glm::vec3 viewSpace[3];for(int i=0;i<3;++i)viewSpace[i] = glm::vec3((glm::vec2(ndc[i])*.5f+.5f)*glm::vec2(w,h),ndc[i].z);

  if(!success){
    std::cerr << R".(
    Tento test zkou????, zda se spr??vn?? interpoluje hloubka fragment??.

    Framebuffer m?? rozli??en??: )."<<w<<"x"<<h<<R".(

    Rasterizuje se troj??heln??k, kter?? m?? sou??adnice v clip space:
    A.gl_Position = vec4)."<<str(outVertices[0].gl_Position)<<R".(;
    B.gl_Position = vec4)."<<str(outVertices[1].gl_Position)<<R".(;
    C.gl_Position = vec4)."<<str(outVertices[2].gl_Position)<<R".(;

    Co?? odpov??d?? rasterizaci troj??heln??k?? v NDC:
    A = vec3)."<<str(ndc[0])<<R".(;
    B = vec3)."<<str(ndc[1])<<R".(;
    C = vec3)."<<str(ndc[2])<<R".(;

    Co?? odpov??d?? rasterizaci troj??heln??k?? ve view-space:
    A = vec3)."<<str(viewSpace[0])<<R".(;
    B = vec3)."<<str(viewSpace[1])<<R".(;
    C = vec3)."<<str(viewSpace[2])<<R".(;

    Hloubka by m??la b??t interpolov??na pomoc?? barycentrick??ch sou??adnic, BEZ perspektvin?? korekce.

    To znamen??, ??e pokud jsou barycentrick?? sou??adnice l0,l1,l2 spo????tan?? z obsah?? ve 2D, pak
    by v??sledn?? hloubka m??la b??t:

    A.z*l0 + B.z*l1 + C.z*l2

    Byly nalezeny fragmenty, kter?? maj?? ??patn?? interpolovanou hloubu.

    Nap????klad tyto fragmenty jsou ??patn??:)."<<std::endl;

    uint32_t counter = 0;
    for(auto const&b:badFragments){
      counter++;

      auto l = barycentrics(outVertices[0],outVertices[1],outVertices[2],b.gl_FragCoord,w,h);

      auto att = 
        outVertices[0].gl_Position.z/outVertices[0].gl_Position.w * l[0]+
        outVertices[1].gl_Position.z/outVertices[1].gl_Position.w * l[1]+
        outVertices[2].gl_Position.z/outVertices[2].gl_Position.w * l[2];
      std::cerr << "    gl_FragCoord.xy == " << str(glm::vec2(b.gl_FragCoord)) << " ma hloubku == " << str(b.gl_FragCoord.z) << ", ale m??l m??t hloubku: " << str(att) << std::endl;

      if(counter>=10)break;
    }

    REQUIRE(false);
  }

}



SCENARIO("17"){
  std::cerr << "17 - vertex attributes interpolated to fragment attributes" << std::endl;
  uint32_t w = 100;
  uint32_t h = 100;
  GPUContext ctx;
  auto framebuffer = std::make_shared<Framebuffer>(w,h);
  ctx.frame = framebuffer->getFrame();

  outVertices.clear();
  inFragments.clear();

  initOutVertices();
  outVertices[0].attributes[0].v3 = glm::vec3(1,0,0);
  outVertices[1].attributes[0].v3 = glm::vec3(0,1,0);
  outVertices[2].attributes[0].v3 = glm::vec3(0,0,1);

  ctx.prg.vertexShader   = vertexShaderInject;
  ctx.prg.fragmentShader = fragmentShaderDump;
  ctx.prg.vs2fs[0]       = AttributeType::VEC3;

  draw(ctx,3);

  bool success = true;

  uint32_t expectedCount = w*h/2;
  uint32_t err = w;
  success &= equalCounts(inFragments.size(),expectedCount,err);

  std::vector<InFragment>badFragments;

  for(auto const&f:inFragments){
    auto const&fragCoord = f.gl_FragCoord;

    auto l = barycentrics(outVertices[0],outVertices[1],outVertices[2],f.gl_FragCoord,w,h);

    auto att = 
      outVertices[0].attributes[0].v3 * l[0]+
      outVertices[1].attributes[0].v3 * l[1]+
      outVertices[2].attributes[0].v3 * l[2];

    bool good = equalVec3(f.attributes[0].v3,att,100.f); 

    if(!good)
      badFragments.push_back(f);

    success &= good;
  }

  if(!success){
    std::cerr << R".(
    Tento test zkou????, zda se interpoluj?? vertex attributy do fragment attribut??.

    Framebuffer m?? rozli??en??: )."<<w<<"x"<<h<<R".(

    Rasterizuje se troj??heln??k, kter?? pokr??v?? polovinu obrazovky:
    A.gl_Position = vec4(-1,-1,-1,+1);
    B.gl_Position = vec4(+1,-1,-1,+1);
    C.gl_Position = vec4(-1,+1,-1,+1);

     ________
    |*      |
    |***    |
    |*****  |
    |*******|
    
    Vrcholy obsahuj?? barvu v nult??m atributu
    A.attributes[0].v3 = vec3(1,0,0);
    B.attributes[0].v3 = vec3(0,1,0);
    C.attributes[0].v3 = vec3(0,0,1);

    Vyrasterizovan?? fragmenty by m??ly m??t spr??vn?? interpolovanou barvu pomoc?? barycentrick??ch sou??adnic.

    Barycentrick?? sou??adnice se spo????taj?? pomoc?? obsah?? troj??heln??k??.

    Byly nalezeny fragmenty, kter?? maj?? ??patn?? interpolovanou barvu.

    Nap????klad tyto fragmenty jsou ??patn??:)."<<std::endl;

    uint32_t counter = 0;
    for(auto const&b:badFragments){
      counter++;

      auto const&fragCoord = b.gl_FragCoord;

      auto l = barycentricsPerspective(outVertices[0],outVertices[1],outVertices[2],b.gl_FragCoord,w,h);

      auto att = 
        outVertices[0].attributes[0].v3 * l[0]+
        outVertices[1].attributes[0].v3 * l[1]+
        outVertices[2].attributes[0].v3 * l[2];

      std::cerr << "    gl_FragCoord.xy == " << str(glm::vec2(fragCoord)) << " ma barvu attributes[0].v3 == " << str(b.attributes[0].v3) << ", ale m??l m??t barvu: " << str(att) << std::endl;

      if(counter>=10)break;
    }

    REQUIRE(false);
  }
}



SCENARIO("18"){
  std::cerr << "18 - perspective correct interpolation of vertex attributes to fragment attributes" << std::endl;

  uint32_t w = 100;
  uint32_t h = 100;
  GPUContext ctx;
  auto framebuffer = std::make_shared<Framebuffer>(w,h);
  ctx.frame = framebuffer->getFrame();

  outVertices.clear();
  inFragments.clear();

  float hc[3] = {1,2,.5};
  float zz[3] = {.9f,.4f,.8f};
  outVertices.resize(3);

  outVertices[0].gl_Position=glm::vec4(-hc[0],-hc[0],zz[0],hc[0]);
  outVertices[1].gl_Position=glm::vec4(+hc[1],-hc[1],zz[1],hc[1]);
  outVertices[2].gl_Position=glm::vec4(-hc[2],+hc[2],zz[2],hc[2]);
  outVertices[0].attributes[0].v3 = glm::vec3(1,0,0);
  outVertices[1].attributes[0].v3 = glm::vec3(0,1,0);
  outVertices[2].attributes[0].v3 = glm::vec3(0,0,1);

  ctx.prg.vertexShader   = vertexShaderInject;
  ctx.prg.fragmentShader = fragmentShaderDump;
  ctx.prg.vs2fs[0]       = AttributeType::VEC3;

  draw(ctx,3);

  bool success = true;

  uint32_t expectedCount = w*h/2;
  uint32_t err = w;
  success &= equalCounts(inFragments.size(),expectedCount,err);

  std::vector<InFragment>badFragments;

  for(auto const&f:inFragments){
    auto l = barycentricsPerspective(outVertices[0],outVertices[1],outVertices[2],f.gl_FragCoord,w,h);

    auto att = 
      outVertices[0].attributes[0].v3 * l[0]+
      outVertices[1].attributes[0].v3 * l[1]+
      outVertices[2].attributes[0].v3 * l[2];

    bool good = equalVec3(f.attributes[0].v3,att); 

    if(!good)
      badFragments.push_back(f);

    success &= good;
  }

  glm::vec3 ndc      [3];for(int i=0;i<3;++i)ndc[i] = glm::vec3(outVertices[i].gl_Position)/outVertices[i].gl_Position.w;
  glm::vec3 viewSpace[3];for(int i=0;i<3;++i)viewSpace[i] = glm::vec3((glm::vec2(ndc[i])*.5f+.5f)*glm::vec2(w,h),ndc[i].z);

  if(!success){
    std::cerr << R".(
    Tento test zkou????, zda se perspektivn?? spr??vn?? interpoluje vertex attributy.

    Framebuffer m?? rozli??en??: )."<<w<<"x"<<h<<R".(

    Rasterizuje se troj??heln??k, kter?? m?? sou??adnice v clip space:
    A.gl_Position = vec4)."<<str(outVertices[0].gl_Position)<<R".(;
    B.gl_Position = vec4)."<<str(outVertices[1].gl_Position)<<R".(;
    C.gl_Position = vec4)."<<str(outVertices[2].gl_Position)<<R".(;

    Co?? odpov??d?? rasterizaci troj??heln??k?? v NDC:
    A = vec3)."<<str(ndc[0])<<R".(;
    B = vec3)."<<str(ndc[1])<<R".(;
    C = vec3)."<<str(ndc[2])<<R".(;

    Co?? odpov??d?? rasterizaci troj??heln??k?? ve view-space:
    A = vec3)."<<str(viewSpace[0])<<R".(;
    B = vec3)."<<str(viewSpace[1])<<R".(;
    C = vec3)."<<str(viewSpace[2])<<R".(;

    Vrcholy obsahuj?? barvu v nult??m atributu
    A.attributes[0].v3 = vec3(1,0,0);
    B.attributes[0].v3 = vec3(0,1,0);
    C.attributes[0].v3 = vec3(0,0,1);

    Atributy by m??ly b??t interpolov??ny pomoc?? barycentrick??ch sou??adnic s perspektvin?? korekc??.

    To znamen??, ??e pokud jsou barycentrick?? sou??adnice l0,l1,l2 spo????tan?? z obsah?? ve 2D, pak
    by v??sledn?? barva m??la b??t:

    (A.color*l0/h0 + B.color*l1/h0 + C.color*l2/h0) / (l0/h0 + l1/h0 + l2/h2)

    Byly nalezeny fragmenty, kter?? maj?? ??patn?? interpolovanou barvu.

    Nap????klad tyto fragmenty jsou ??patn??:)."<<std::endl;

    uint32_t counter = 0;
    for(auto const&f:badFragments){
      counter++;

      auto l = barycentricsPerspective(outVertices[0],outVertices[1],outVertices[2],f.gl_FragCoord,w,h);

      auto att = 
        outVertices[0].attributes[0].v3 * l[0]+
        outVertices[1].attributes[0].v3 * l[1]+
        outVertices[2].attributes[0].v3 * l[2];

      std::cerr << "    gl_FragCoord.xy == " << str(glm::vec2(f.gl_FragCoord)) << " ma barvu == " << str(f.attributes[0].v3) << ", ale m??l m??t barvu: " << str(att) << std::endl;

      if(counter>=10)break;
    }

    REQUIRE(false);
  }
}

void fragmentShaderWhite(OutFragment&outFragment,InFragment const&inF,Uniforms const&u){
  outFragment.gl_FragColor = glm::vec4(1.f);
  fragmentShaderDump(outFragment,inF,u);
}

SCENARIO("19"){
  std::cerr << "19 - fragment color should be written to framebuffer" << std::endl;

  auto res = glm::uvec2(100,100);

  initOutVertices();
  inFragments.clear();
  glm::vec3 ndc      [3];for(int i=0;i<3;++i)ndc[i] = glm::vec3(outVertices[i].gl_Position)/outVertices[i].gl_Position.w;
  glm::vec3 viewSpace[3];for(int i=0;i<3;++i)viewSpace[i] = glm::vec3((glm::vec2(ndc[i])*.5f+.5f)*glm::vec2(res),ndc[i].z);

  auto framebuffer = std::make_shared<Framebuffer>(res.x,res.y);
  GPUContext ctx;  
  ctx.frame = framebuffer->getFrame();
  ctx.prg.vertexShader = vertexShaderInject;
  ctx.prg.fragmentShader = fragmentShaderWhite;

  draw(ctx,3);

  bool success = true;

  std::set<UV2>rasterized;
  for(auto const&f:inFragments)
    rasterized.insert({f.gl_FragCoord});

  std::set<UV2>shouldHaveColor;
  std::set<UV2>shouldBeEmpty;

  for(uint32_t y=0;y<res.y;++y)
    for(uint32_t x=0;x<res.x;++x){
      auto coord = glm::uvec2(x,y);
      auto color = readColor(ctx.frame,coord);
      bool good;
      if(rasterized.find(coord) != rasterized.end()){
        good = color == glm::uvec3(255);
        if(!good)shouldHaveColor.insert(coord);
      }else{
        good = color == glm::uvec3(0);
        if(!good)shouldBeEmpty.insert(coord);
      }
      success &= good;
    }


  if(!success){
    std::cerr << R".(
    Tento test zkou????, zda se barva fragmentu zapsala do framebufferu.

    Framebuffer m?? rozli??en??: )."<<res.x<<"x"<<res.y<<R".(

    Rasterizuje se troj??heln??k:
    A = vec2)."<<str(glm::vec2(viewSpace[0]))<<R".(;
    B = vec2)."<<str(glm::vec2(viewSpace[1]))<<R".(;
    C = vec2)."<<str(glm::vec2(viewSpace[2]))<<R".(;

    Ale byly nalezeny pixely ve framebufferu, kter?? by m??ly b??t vybarven??, ale nejsou.
    Nebo byly nalezeny pixely ve framebufferu, kter?? nejsou vybarven??, ale m??ly by b??t.)." << std::endl;

    std::cerr << R".(
    Tyto pixely by m??ly b??t obarven??, ale nebyly: {)."<<std::endl;
    uint32_t counter=0;
    for(auto const&v:shouldHaveColor){
      std::cerr << "    " << str(v.data) << std::endl;
      counter++;
      if(counter>=10)break;
    }
    std::cerr << "    }" << std::endl;
    counter=0;
    std::cerr << R".(
    Tyto pixely by nem??ly b??t obarven??, ale byly: {)."<<std::endl;
    for(auto const&v:shouldBeEmpty){
      std::cerr << "    " << str(v.data) << std::endl;
      counter++;
      if(counter>=10)break;
    }
    std::cerr << "    }" << std::endl;

    REQUIRE(false);
  }
}

SCENARIO("20"){
  std::cerr << "20 - fragment depth should be written to framebuffer" << std::endl;

  auto res = glm::uvec2(100,100);

  initOutVertices();
  inFragments.clear();
  glm::vec3 ndc      [3];for(int i=0;i<3;++i)ndc[i] = glm::vec3(outVertices[i].gl_Position)/outVertices[i].gl_Position.w;
  glm::vec3 viewSpace[3];for(int i=0;i<3;++i)viewSpace[i] = glm::vec3((glm::vec2(ndc[i])*.5f+.5f)*glm::vec2(res),ndc[i].z);

  auto framebuffer = std::make_shared<Framebuffer>(res.x,res.y);
  GPUContext ctx;
  ctx.frame = framebuffer->getFrame();
  ctx.prg.vertexShader = vertexShaderInject;
  ctx.prg.fragmentShader = fragmentShaderWhite;

  draw(ctx,3);

  bool success = true;

  std::set<UV2>rasterized;
  for(auto const&f:inFragments)
    rasterized.insert({f.gl_FragCoord});

  std::set<UV2>shouldHaveDepth;
  std::set<UV2>shouldBeEmpty;

  auto&depth = ctx.frame.depth;
  for(uint32_t y=0;y<res.y;++y)
    for(uint32_t x=0;x<res.x;++x){
      auto pix = y*res.x+x;
      bool good;
      if(rasterized.find(glm::uvec2(x,y)) != rasterized.end()){
        good = depth[pix] != 1.f;
        if(!good)shouldHaveDepth.insert(glm::uvec2(x,y));
      }else{
        good = depth[pix] == 1.f;
        if(!good)shouldBeEmpty.insert(glm::uvec2(x,y));
      }
      success &= good;
    }


  if(!success){
    std::cerr << R".(
    Tento test zkou????, zda se houbka fragmentu zapsala do framebufferu.

    Framebuffer m?? rozli??en??: )."<<res.x<<"x"<<res.y<<R".(

    Rasterizuje se troj??heln??k:
    A = vec2)."<<str(viewSpace[0])<<R".(;
    B = vec2)."<<str(viewSpace[1])<<R".(;
    C = vec2)."<<str(viewSpace[2])<<R".(;

    Ale byly nalezeny pixely ve framebufferu, kter?? by m??ly m??t zm??n??nou hloubku, ale nemaj??.
    Nebo byly nalezeny pixely ve framebufferu, kter?? nem??ly zm??n??nou hloubku, ale m??ly by m??t.)." << std::endl;

    std::cerr << R".(
    Tyto pixely by m??ly m??t zm??n??nou hloubku, ale nem??ly: {)."<<std::endl;
    uint32_t counter=0;
    for(auto const&v:shouldHaveDepth){
      std::cerr << "    " << str(v.data) << std::endl;
      counter++;
      if(counter>=10)break;
    }
    std::cerr << "    }" << std::endl;
    counter=0;
    std::cerr << R".(
    Tyto pixely by nem??ly m??t zm??n??nou hloubu, ale m??ly: {)."<<std::endl;
    for(auto const&v:shouldBeEmpty){
      std::cerr << "    " << str(v.data) << std::endl;
      counter++;
      if(counter>=10)break;
    }
    std::cerr << "    }" << std::endl;

    REQUIRE(false);
  }
}

void fragmentShaderDepthTest(OutFragment&outF,InFragment const&inF,Uniforms const&){
  outF.gl_FragColor = glm::vec4(inF.attributes[0].v4);
}


SCENARIO("21"){
  std::cerr << "21 - depth test" << std::endl;

  auto res = glm::uvec2(100,100);

  outVertices.clear();
  outVertices.resize(9);
  outVertices[0].gl_Position = glm::vec4(-1,-1,0,1);
  outVertices[1].gl_Position = glm::vec4(+4,-1,0,1);
  outVertices[2].gl_Position = glm::vec4(-1,+4,0,1);
  outVertices[3].gl_Position = glm::vec4(-1,-1,-.1,1);
  outVertices[4].gl_Position = glm::vec4(+0,-1,-.1,1);
  outVertices[5].gl_Position = glm::vec4(-1,+0,-.1,1);
  outVertices[6].gl_Position = glm::vec4(-1,-1,+.1,1);
  outVertices[7].gl_Position = glm::vec4(+0,-1,+.1,1);
  outVertices[8].gl_Position = glm::vec4(-1,+0,+.1,1);

  outVertices[0].attributes[0].v4 = glm::vec4(1,0,0,1);
  outVertices[1].attributes[0].v4 = glm::vec4(1,0,0,1);
  outVertices[2].attributes[0].v4 = glm::vec4(1,0,0,1);
  outVertices[3].attributes[0].v4 = glm::vec4(0,1,0,1);
  outVertices[4].attributes[0].v4 = glm::vec4(0,1,0,1);
  outVertices[5].attributes[0].v4 = glm::vec4(0,1,0,1);
  outVertices[6].attributes[0].v4 = glm::vec4(0,0,1,1);
  outVertices[7].attributes[0].v4 = glm::vec4(0,0,1,1);
  outVertices[8].attributes[0].v4 = glm::vec4(0,0,1,1);

  inFragments.clear();
  glm::vec3 ndc      [3];for(int i=0;i<3;++i)ndc[i] = glm::vec3(outVertices[i].gl_Position)/outVertices[i].gl_Position.w;
  glm::vec3 viewSpace[3];for(int i=0;i<3;++i)viewSpace[i] = glm::vec3((glm::vec2(ndc[i])*.5f+.5f)*glm::vec2(res),ndc[i].z);

  auto framebuffer = std::make_shared<Framebuffer>(res.x,res.y);
  GPUContext ctx;
  ctx.frame = framebuffer->getFrame();
  ctx.prg.vertexShader   = vertexShaderInject;
  ctx.prg.fragmentShader = fragmentShaderDepthTest;
  ctx.prg.vs2fs[0]       = AttributeType::VEC4;

  draw(ctx,9);

  bool success = true;

  success &= ctx.frame.color[(0*res.x+0)*4+0] == 0;
  success &= ctx.frame.color[(0*res.x+0)*4+1] == 255;
  success &= ctx.frame.color[(0*res.x+0)*4+2] == 0  ;

  success &= ctx.frame.color[((res.y-1)*res.x+res.x-1)*4+0] == 255;
  success &= ctx.frame.color[((res.y-1)*res.x+res.x-1)*4+1] == 0  ;
  success &= ctx.frame.color[((res.y-1)*res.x+res.x-1)*4+2] == 0  ;


  if(!success){
    std::cerr << R".(Tento test kontroluje, jetli funguje depth test.
    Fragmenty, kter?? maj?? men???? hloubku, ne?? je ta, kter?? je pozna??en?? ve framebufferu
    by m??ly zm??nit barvu a hloubu ve framebuffer.

    Ale n??co je ??patn??...

    N??pov??da:

    if(fragDepth < frame.depth[pix]){
      frame.depth[pix] = fragDepth;
      frame.color[pix] = fragColor;
    }

    ).";
    REQUIRE(false);
  }
}

SCENARIO("22"){
  std::cerr << "22 - blending" << std::endl;

  auto res = glm::uvec2(100,100);

  auto color = glm::vec4(.3,.4,.2,0.7);
  auto frameColor = glm::uvec3(120,30,90);

  outVertices.clear();
  outVertices.resize(3);
  outVertices[0].gl_Position = glm::vec4(-1,-1,0,1);
  outVertices[1].gl_Position = glm::vec4(+4,-1,0,1);
  outVertices[2].gl_Position = glm::vec4(-1,+4,0,1);

  outVertices[0].attributes[0].v4 = color;
  outVertices[1].attributes[0].v4 = color;
  outVertices[2].attributes[0].v4 = color;

  inFragments.clear();

  auto framebuffer = std::make_shared<Framebuffer>(res.x,res.y);
  GPUContext ctx;
  ctx.frame = framebuffer->getFrame();
  ctx.prg.vertexShader   = vertexShaderInject;
  ctx.prg.fragmentShader = fragmentShaderDepthTest;
  ctx.prg.vs2fs[0]       = AttributeType::VEC4;

  clearFrame(ctx.frame,frameColor);

  draw(ctx,3);

  bool success = true;

  auto samplePos = glm::uvec2(10,10);

  auto finalColor    = readColor(ctx.frame,samplePos);
  auto expectedColor = alphaMix(frameColor,color);

  success &= finalColor.r == expectedColor.r;
  success &= finalColor.g == expectedColor.g;
  success &= finalColor.b == expectedColor.b;

  if(!success){
    std::cerr << R".(Tento test kontroluje, jetli funguje blending (p??im??ch??v??n?? barev).

    Fragmenty, kter?? maj?? 4. komponentu barvy jinou od 1 by se m??ly p??im??chat ke st??vaj??c?? barv?? framebufferu.
    Jedn?? se o takzvanou alpha - "nepr??hlednost". 0 - absolutn?? pr??hledn??, 1 - absolutn?? nepr??hledn??.
    P??im??ch??n?? spo????v?? v tom, ??e se vyv??huje barva fragmentu s barvou ve framebufferu pomoc?? alpha hodnoty.

    Ale n??co je ??patn??...

    Barva framebufferu byla nastavena na hodnotu:)."<< str(frameColor)<<R".(
    Rasterizuje se troj??heln??k, kter?? pokr??v?? celou obrazovku a m?? barvu:)."<<str(color)<<R".(
    V??sledn?? barva:)."<<str(finalColor)<<R".( se li???? od barvy, co by tam m??la b??t:)."<<str(expectedColor)<<R".(

    N??pov??da:

    vec4 fragColor;
    float a = fragColor.w;//nepruhlenost

    //mus??me prevest barvu ulozenou v bytech 0-255 na float
    //potom to sm??chat s barvou fragmentu pomoc?? alphy
    //pak ove??it, kdyby to n??hodou bylo mimo interval [0,1]
    //a vr??tit do bajtu *255
    frameColor.rgb = clamp((frameColor.rgb/255.f)*(1-a) + (fragColor.rgb)*(a),0.f,1.f)*255.f

    Blending se v????e k OpenGL blendingu a blendovac??mu m??du:
    glEnable(GL_BLEND);
    glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    ).";
    REQUIRE(false);
  }
}

SCENARIO("23"){
  std::cerr << "23 - blending + depth" << std::endl;

  auto res = glm::uvec2(100,100);

  auto color1 = glm::vec4(.3,.4,.2,0.7);
  auto color2 = glm::vec4(.3,.4,.2,0.3);
  auto frameColor = glm::uvec3(120,30,90);
  auto frameDepth = 1.f;

  outVertices.clear();
  outVertices.resize(6);
  outVertices[0+0].gl_Position = glm::vec4(-1,-1,0,1);
  outVertices[0+1].gl_Position = glm::vec4(+0,-1,0,1);
  outVertices[0+2].gl_Position = glm::vec4(-1,+0,0,1);
  outVertices[3+0].gl_Position = glm::vec4(+0,+0,0,1);
  outVertices[3+1].gl_Position = glm::vec4(+1,+0,0,1);
  outVertices[3+2].gl_Position = glm::vec4(+0,+1,0,1);

  outVertices[0+0].attributes[0].v4 = color1;
  outVertices[0+1].attributes[0].v4 = color1;
  outVertices[0+2].attributes[0].v4 = color1;
  outVertices[3+0].attributes[0].v4 = color2;
  outVertices[3+1].attributes[0].v4 = color2;
  outVertices[3+2].attributes[0].v4 = color2;

  inFragments.clear();

  auto framebuffer = std::make_shared<Framebuffer>(res.x,res.y);
  GPUContext ctx;
  ctx.frame = framebuffer->getFrame();
  ctx.prg.vertexShader   = vertexShaderInject;
  ctx.prg.fragmentShader = fragmentShaderDepthTest;
  ctx.prg.vs2fs[0]       = AttributeType::VEC4;

  clearFrame(ctx.frame,frameColor,frameDepth);

  draw(ctx,(uint32_t)outVertices.size());

  bool success = true;

  auto samplePos1 = glm::uvec2(        10,        10);
  auto samplePos2 = glm::uvec2(res.x/2+10,res.y/2+10);

  auto finalColor1 = readColor(ctx.frame,samplePos1);
  auto finalColor2 = readColor(ctx.frame,samplePos2);

  auto expectedColor1 = alphaMix(frameColor,color1);
  auto expectedColor2 = alphaMix(frameColor,color2);
  auto finalDepth1    = readDepth(ctx.frame,samplePos1);
  auto finalDepth2    = readDepth(ctx.frame,samplePos2);

  success &= finalColor1.r == expectedColor1.r;
  success &= finalColor1.g == expectedColor1.g;
  success &= finalColor1.b == expectedColor1.b;
  success &= finalDepth1   != frameDepth      ;

  success &= finalColor2.r == expectedColor2.r;
  success &= finalColor2.g == expectedColor2.g;
  success &= finalColor2.b == expectedColor2.b;
  success &= finalDepth2   == frameDepth      ;

  if(!success){
    std::cerr << R".(

    Tento test kontroluje, jetli se maskuje z??pis hloubky u fragment??, kter?? maj?? nepr??hlednost men???? ne?? 0.5.

    Pouze fragmenty, jejich nepr??hlednost je v??t???? n???? 0.5 mohou p??epsat hloubku ve framebufferu.
  
    Ale n??co je ??patn??...

    Barva framebufferu byla nastavena na hodnotu: )."<< str(frameColor)<<R".(

    Rasterizuje se troj??heln??k, kter?? m?? barvu: )."<<str(color1)<<R".(
    V??sledn?? barva: )."<<str(finalColor1)<<R".( by m??la b??t: )."<<str(expectedColor1)<<R".( a hloubka se M??LA zm??nit ze: )."<<str(frameDepth)<<R".( na n??co jin??ho, ve framebufferu se objevila: )."<<finalDepth1<<R".(

    Rasterizuje se troj??heln??k, kter?? m?? barvu: )."<<str(color2)<<R".(
    V??sledn?? barva: )."<<str(finalColor2)<<R".( by m??la b??t: )."<<str(expectedColor2)<<R".( a hloubka se NEM??LA zm??nit ze: )."<<str(frameDepth)<<R".( na n??co jin??ho, ve framebufferu se objevila: )."<<finalDepth2<<R".(

    N??pov??da:

    vec4 fragColor;
    float fragDepth;
    float a = fragColor.w;//nepruhlenost

    if(a > 0.5f){ // pouze pokud je nepruhlenost vet???? ne?? .5 tak se p??ep????e hloubka
      frame.depth = fragDepth;
    }

    Pozn??mka:
    V OpenGL/Vulkan/DirectX nen?? umo??n??no takto p????mo nezapisovat hloubku, pokud je alpha p????li?? mal??.
    Pr??hlednost se v 3D aplikac??ch ??e???? jinak - nejprve se vykresl?? nepr??hledn?? objekty a potom se se??ad?? pr??hledn?? a ty se vykresl?? odzadu.

    ).";
    REQUIRE(false);
  }
}

