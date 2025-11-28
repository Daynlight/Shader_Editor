#include "Renderer.h"
#include "Gui.h"

#include "Shaders.h"

#define MAXSHADERSIZE 10240

char vertex_shader[MAXSHADERSIZE] = {0};
char fragment_shader[MAXSHADERSIZE] = {0};

const char* types[] = {"int", "ivec2", "ivec3", "float", "vec2", "vec3"};
std::unordered_map<std::string, const std::type_info*> uniform_list;
char new_name[255] = {0};
int new_current_type = 0;






inline std::function<void(CW::Renderer::iRenderer *renderer)> uniform_editor(CW::Renderer::DrawShader *shader, CW::Renderer::Uniform *uniform){
return [shader, uniform](CW::Renderer::iRenderer *renderer){
  for(std::pair<const std::string, const std::type_info*>& el : uniform_list){
    if(el.second == &typeid(int)){
      int value = (*uniform)[el.first]->get<int>();
      ImGui::InputInt(el.first.c_str(), &value);
      (*uniform)[el.first]->set<int>(value);
    }
    else if(el.second == &typeid(glm::ivec2)){
      glm::ivec2 value = (*uniform)[el.first]->get<glm::ivec2>();
      ImGui::InputInt2(el.first.c_str(), &value[0]);
      (*uniform)[el.first]->set<glm::ivec2>(value);
    }
    else if(el.second == &typeid(glm::ivec3)){
      glm::ivec3 value = (*uniform)[el.first]->get<glm::ivec3>();
      ImGui::InputInt3(el.first.c_str(), &value[0]);
      (*uniform)[el.first]->set<glm::ivec3>(value);
    }
    else if(el.second == &typeid(float)){
      float value = (*uniform)[el.first]->get<float>();
      ImGui::InputFloat(el.first.c_str(), &value);
      (*uniform)[el.first]->set<float>(value);
    }
    else if(el.second == &typeid(glm::vec2)){
      glm::vec2 value = (*uniform)[el.first]->get<glm::vec2>();
      ImGui::InputFloat2(el.first.c_str(), &value[0]);
      (*uniform)[el.first]->set<glm::vec2>(value);
    }
    else if(el.second == &typeid(glm::vec3)){
      glm::vec3 value = (*uniform)[el.first]->get<glm::vec3>();
      ImGui::InputFloat3(el.first.c_str(), &value[0]);
      (*uniform)[el.first]->set<glm::vec3>(value);
    }
  };

  ImGui::SeparatorText("Add new");
  ImGui::InputText("New name: ", new_name, 255);

  if (ImGui::BeginCombo("Type", types[new_current_type])) {
    for (int i = 0; i < IM_ARRAYSIZE(types); i++) {
        bool is_selected = (new_current_type == i);
        if (ImGui::Selectable(types[i], is_selected)) {
            new_current_type = i;
        }
        if (is_selected)
            ImGui::SetItemDefaultFocus();
    }
    ImGui::EndCombo();
  }

  if(ImGui::Button("Add")){
    if(new_current_type == 0){
      uniform_list[new_name] = &typeid(int);
      (*uniform)[new_name]->set<int>(0);
    }
    else if(new_current_type == 1){
      uniform_list[new_name] = &typeid(glm::ivec2);
      (*uniform)[new_name]->set<glm::ivec2>({0, 0});
    }
    else if(new_current_type == 2){
      uniform_list[new_name] = &typeid(glm::ivec3);
      (*uniform)[new_name]->set<glm::ivec3>({0, 0, 0});
    }
    else if(new_current_type == 3){
      uniform_list[new_name] = &typeid(float);
      (*uniform)[new_name]->set<float>(0);
    }
    else if(new_current_type == 4){
      uniform_list[new_name] = &typeid(glm::vec2);
      (*uniform)[new_name]->set<glm::vec2>({0, 0});
    }
    else if(new_current_type == 5){
      uniform_list[new_name] = &typeid(glm::vec3);
      (*uniform)[new_name]->set<glm::vec3>({0, 0, 0});
    }
  };
};
};







inline std::function<void(CW::Renderer::iRenderer *renderer)> vertex_shader_editor(CW::Renderer::DrawShader *shader, CW::Renderer::Uniform *uniform){
return [shader, uniform](CW::Renderer::iRenderer *renderer){
  ImVec2 size = ImGui::GetContentRegionAvail();
  if(ImGui::InputTextMultiline("###", vertex_shader, MAXSHADERSIZE, size)){
    shader->setVertexShader(vertex_shader);
    shader->compile();
  };
};
};







inline std::function<void(CW::Renderer::iRenderer *renderer)> fragment_shader_editor(CW::Renderer::DrawShader *shader, CW::Renderer::Uniform *uniform){
return [shader, uniform](CW::Renderer::iRenderer *renderer){
  ImVec2 size = ImGui::GetContentRegionAvail();
  if(ImGui::InputTextMultiline("##", fragment_shader, MAXSHADERSIZE, size)){
    shader->setFragmentShader(fragment_shader);
    shader->compile();
  };
};
};











int main(){
  CW::Renderer::Renderer window;
  CW::Gui::Gui gui(&window);

  CW::Renderer::Texture texture;
  if(texture.load("../Assets/image.png"))
    return -1;

  CW::Renderer::Mesh viewport = CW::Renderer::Mesh(
  {
    -1.0f,  1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f,
    1.0f,  1.0f, 0.0f,
    1.0f, -1.0f, 0.0f,
  }, 
  {
    0, 1, 2,
    1, 3, 2
  });

  viewport.addTextCords(
  {
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 0.0f
  });

  strncpy(vertex_shader, Texture::vertex.c_str(), Texture::vertex.size());
  strncpy(fragment_shader, Texture::fragment.c_str(), Texture::vertex.size());
  
  CW::Renderer::DrawShader shader(Texture::vertex, fragment_shader);
  CW::Renderer::Uniform uniform;
  shader.getUniforms().emplace_back(&uniform);
  uniform["uTexture"]->set<int>(0);


  gui.addWindow("Vertex Shader Editor", vertex_shader_editor(&shader, &uniform));
  gui.addWindow("Fragment Shader Editor", fragment_shader_editor(&shader, &uniform));
  gui.addWindow("Uniform Editor", uniform_editor(&shader, &uniform));


  while(!window.getWindowData()->should_close){
    window.beginFrame();

    texture.bind();
    shader.bind();
    viewport.render();
    shader.unbind();
    texture.unbind();

    gui.render();

    window.swapBuffer();
    window.windowEvents();
  };

  return 0;
}