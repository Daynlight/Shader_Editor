#include "Renderer.h"
#include "Gui.h"

#include "Shaders.h"

#include <filesystem>
#include <fstream>


char path[255] = "Texture";

std::chrono::time_point<std::filesystem::__file_clock> vertex_last_update;
std::chrono::time_point<std::filesystem::__file_clock> fragment_last_update;

const char* types[] = {"int", "ivec2", "ivec3", "float", "vec2", "vec3"};
std::unordered_map<std::string, const std::type_info*> uniform_list;
char new_name[255] = {0};
int new_current_type = 0;






inline std::function<void(CW::Renderer::iRenderer *renderer)> settings(CW::Renderer::DrawShader *shader, CW::Renderer::Uniform *uniform){
return [shader, uniform](CW::Renderer::iRenderer *renderer){
  ImGui::InputText("Path: ", path, 255);

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



std::string read_file(const std::string& path){
  std::ifstream file(path);
  std::string data;
  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
        data += line + "\n";
    }
    file.close();
  }

  return data;
}




void update_shaders(CW::Renderer::DrawShader *shader){
  std::string fragment_shader_path = "../Shaders/"+std::string(path)+"/shader.frag";
  std::string vertex_shader_path = "../Shaders/"+std::string(path)+"/shader.vert";

  bool update = 0;

  if(std::string(path).empty() || !std::filesystem::exists("../Shaders/" + std::string(path)))
    return;

  std::chrono::time_point<std::filesystem::__file_clock> vertex_current_time = std::filesystem::last_write_time(vertex_shader_path);
  std::chrono::time_point<std::filesystem::__file_clock> fragment_current_time = std::filesystem::last_write_time(fragment_shader_path);


  if(vertex_current_time != vertex_last_update){
    shader->setVertexShader(read_file(vertex_shader_path));
    vertex_last_update = vertex_current_time;
    update = 1;
  }

  if(fragment_current_time != fragment_last_update){
    shader->setFragmentShader(read_file(fragment_shader_path));
    fragment_last_update = fragment_current_time;
    update = 1;
  }

  if(update)
    shader->compile();
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

  CW::Renderer::DrawShader shader(Texture::vertex, Texture::fragment);
  CW::Renderer::Uniform uniform;
  shader.getUniforms().emplace_back(&uniform);
  uniform["uTexture"]->set<int>(0);

  gui.addWindow("Uniform Editor", settings(&shader, &uniform));


  while(!window.getWindowData()->should_close){
    update_shaders(&shader);

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