#include "Renderer.h"
#include "Gui.h"

#include "Shaders.h"

#include <filesystem>
#include <fstream>


char path[255] = "Texture";

std::chrono::time_point<std::filesystem::__file_clock> vertex_last_update;
std::chrono::time_point<std::filesystem::__file_clock> fragment_last_update;
std::chrono::time_point<std::filesystem::__file_clock> uniform_last_update;

const char* types[] = {"int", "ivec2", "ivec3", "float", "vec2", "vec3"};
CW::Renderer::Uniform *uniform;





inline std::function<void (std::function<void ()> render_windows)> workspace(){
return [](std::function<void()> render_windows){
  ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
  const ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->WorkPos);
  ImGui::SetNextWindowSize(viewport->WorkSize);
  ImGui::SetNextWindowViewport(viewport->ID);
  window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f)); 
  ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
  ImGui::Begin("Window DockSpace", nullptr, window_flags);
  ImGui::PopStyleVar(2);
  ImGui::PopStyleColor();
  ImGuiID docspace_id = ImGui::GetID("MyDockSpace");
  ImGui::DockSpace(docspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);

  if(ImGui::BeginMainMenuBar()){
    ImGui::InputText("Path: ", path, 255);
    ImGui::EndMainMenuBar();
  }

  render_windows();

  ImGui::End();
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
};


struct uniform_record{
  std::string type = "float";
  std::string value = "0.0f";
};



inline std::vector<std::string> split(const std::string &s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    for (char c : s) {
        if (c == delimiter) {
            if (!token.empty())
                tokens.push_back(token);
            token.clear();
        } else if (c != ' ') {
            token += c;
        }
    }
    if (!token.empty()) tokens.push_back(token);
    return tokens;
}

inline void updateUniforms(const std::string &path) {
  (*uniform).clear();
  std::unordered_map<std::string, uniform_record> uniform_list;

  std::ifstream file(path);
  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line)) {
      if (line.empty()) continue;

      std::string values[3];
      int i = 0;
      for (char el : line) {
        if (i >= 2 || el != ' ')
          values[i] += el;
        else
          i++;
      }

      uniform_list[values[1]] = {values[0], values[2]};
    }
    file.close();
  }

  for (auto &el : uniform_list) {
    if (el.second.type == "int") {
      (*uniform)[el.first]->set<int>(std::stoi(el.second.value));
    }
    else if (el.second.type == "ivec2") {
      auto parts = split(el.second.value, ',');
      (*uniform)[el.first]->set<glm::ivec2>(
        { std::stoi(parts[0]), std::stoi(parts[1]) });
    }
    else if (el.second.type == "ivec3") {
      auto parts = split(el.second.value, ',');
      (*uniform)[el.first]->set<glm::ivec3>(
        { std::stoi(parts[0]), std::stoi(parts[1]), std::stoi(parts[2]) });
    }
    else if (el.second.type == "float") {
      (*uniform)[el.first]->set<float>(std::stof(el.second.value));
    }
    else if (el.second.type == "vec2") {
      auto parts = split(el.second.value, ',');
      (*uniform)[el.first]->set<glm::vec2>(
        { std::stof(parts[0]), std::stof(parts[1]) });
    }
    else if (el.second.type == "vec3") {
      auto parts = split(el.second.value, ',');
      (*uniform)[el.first]->set<glm::vec3>(
       { std::stof(parts[0]), std::stof(parts[1]), std::stof(parts[2]) });
    }
  }
}







void update_shaders(CW::Renderer::DrawShader *shader){
  std::string fragment_shader_path = "../Shaders/"+std::string(path)+"/shader.frag";
  std::string vertex_shader_path = "../Shaders/"+std::string(path)+"/shader.vert";
  std::string uniform_path = "../Shaders/"+std::string(path)+"/uniforms.txt";

  bool update = 0;

  std::chrono::time_point<std::filesystem::__file_clock> vertex_current_time = vertex_last_update;
  std::chrono::time_point<std::filesystem::__file_clock> fragment_current_time = fragment_last_update;
  std::chrono::time_point<std::filesystem::__file_clock> uniform_current_time = uniform_last_update;

  if(std::filesystem::exists(fragment_shader_path))
    vertex_current_time = std::filesystem::last_write_time(vertex_shader_path);
  if(std::filesystem::exists(vertex_shader_path))
    fragment_current_time = std::filesystem::last_write_time(fragment_shader_path);
  if(std::filesystem::exists(uniform_path))
    uniform_current_time = std::filesystem::last_write_time(uniform_path);


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

  if(uniform_last_update != uniform_current_time){
    updateUniforms(uniform_path);
    uniform_last_update = uniform_current_time;
  }
  
};










int main(){
  CW::Renderer::Renderer window;
  CW::Gui::Gui gui(&window);
  gui.setWorkspace(workspace());

  uniform = new CW::Renderer::Uniform();

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
  shader.getUniforms().emplace_back(uniform);
  (*uniform)["uTexture"]->set<int>(0);

  // gui.addWindow("Uniform Editor", settings(&shader, &uniform));


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

  delete uniform;

  return 0;
}