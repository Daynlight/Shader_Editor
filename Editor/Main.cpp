#include "Renderer.h"

#include <filesystem>
#include <fstream>
#include <type_traits>
#include <variant>




///////////////////////////////////////////////////////
/////////////////////// globals ///////////////////////
///////////////////////////////////////////////////////
std::chrono::time_point<std::filesystem::__file_clock> vertex_last_update{};
std::chrono::time_point<std::filesystem::__file_clock> fragment_last_update{};
std::chrono::time_point<std::filesystem::__file_clock> uniform_last_update{};
std::chrono::time_point<std::filesystem::__file_clock> texture_last_update{};

std::chrono::time_point<std::filesystem::__file_clock> vertex_current_time;
std::chrono::time_point<std::filesystem::__file_clock> fragment_current_time;
std::chrono::time_point<std::filesystem::__file_clock> uniform_current_time;
std::chrono::time_point<std::filesystem::__file_clock> texture_current_time;

const std::string& fragment_shader_path = "shader.frag";
const std::string& vertex_shader_path = "shader.vert";
const std::string& uniform_path = "uniforms.txt";
const std::string& texture_path = "texture.png";




///////////////////////////////////////////////////////
/////////////////////// helpers ///////////////////////
///////////////////////////////////////////////////////
std::string read_file(const std::string& path){
  std::ifstream file(path);
  std::string data;
  if (file.is_open()) {
    std::string line;
    while (std::getline(file, line))
        data += line + "\n";
    file.close();
  };

  return data;
};




inline std::vector<std::string> split(const std::string &s, char delimiter) {
  std::vector<std::string> tokens;
  std::string token;
  for (char c : s) {
    if (c == delimiter) {
      if (!token.empty())
        tokens.emplace_back(token);
      token.clear();
    } else if (c != ' ') {
      token += c;
    }
  }
  if (!token.empty()) tokens.emplace_back(token);

  return tokens;
};


template<std::size_t I = 0>
constexpr std::size_t type_index(const std::type_info& t) {
    if constexpr (I < std::variant_size_v<CW::Renderer::DataVariants>) {
        using T = std::variant_alternative_t<I, CW::Renderer::DataVariants>;
        if (t == typeid(T)) return I;
        else return type_index<I + 1>(t);
    } else {
        return -1; // not found
    }
}



template<typename T>
T convertValue(const std::string& value){
  T v;
  if (typeid(T) == typeid(int))
    v = std::stoi(value);
  else if(typeid(T) == typeid(float))
    v = std::stof(value);
  
  return v;
};


template<typename T>
std::vector<T> convertArray(const std::vector<std::string>* values_list){
  std::vector<T> values_list_converted;
  for(int i = 0; i < values_list->size(); i++){
    T v = convertValue<T>((*values_list)[i]);
    values_list_converted.emplace_back(v);
  };
  return values_list_converted;
};



void loadUniform(CW::Renderer::Uniform *uniform, const std::string& name, const std::string type, const std::string& values){
  std::vector<std::string> values_list = split(values, ',');
  

  if(type == "int"){
    std::vector<int> converted = convertArray<int>(&values_list);    
    (*uniform)[name]->set<int>(converted[0]);
  }
  else if(type == "ivec2"){
    std::vector<int> converted = convertArray<int>(&values_list);    
    (*uniform)[name]->set<glm::ivec2>({converted[0], converted[1]});
  }
  else if(type == "ivec3"){
    std::vector<int> converted = convertArray<int>(&values_list);    
    (*uniform)[name]->set<glm::ivec3>({converted[0], converted[1], converted[2]});
  }
  else if(type == "float"){
    std::vector<float> converted = convertArray<float>(&values_list);    
    (*uniform)[name]->set<float>(converted[0]);
  }
  else if(type == "vec2"){
    std::vector<float> converted = convertArray<float>(&values_list);    
    (*uniform)[name]->set<glm::vec2>({converted[0], converted[1]});
  }
  else if(type == "vec3"){
    std::vector<float> converted = convertArray<float>(&values_list);    
    (*uniform)[name]->set<glm::vec3>({converted[0], converted[1], converted[2]});
  };
};




////////////////////////////////////////////////////////////
/////////////////////// file updater ///////////////////////
////////////////////////////////////////////////////////////
void updateCurrentTimeWrite(){
  if(std::filesystem::exists(vertex_shader_path))
    vertex_current_time = std::filesystem::last_write_time(vertex_shader_path);
  if(std::filesystem::exists(fragment_shader_path))
    fragment_current_time = std::filesystem::last_write_time(fragment_shader_path);
  if(std::filesystem::exists(uniform_path))
    uniform_current_time = std::filesystem::last_write_time(uniform_path);
  if(std::filesystem::exists(texture_path))
    texture_current_time = std::filesystem::last_write_time(texture_path);
};




void updateUniforms(CW::Renderer::Uniform *uniform) {
  uniform->clear();

  std::ifstream file(uniform_path);
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
      };

      loadUniform(uniform, values[1], values[0], values[2]);
    };
    file.close();
  };

  (*uniform)["uTexture"]->set<int>(0);
  uniform_last_update = uniform_current_time;
};




void updateShaders(CW::Renderer::DrawShader *shader){
  bool update_shader = 0;

  if(vertex_current_time != vertex_last_update){
    shader->setVertexShader(read_file(vertex_shader_path));
    vertex_last_update = vertex_current_time;
    update_shader = 1;
  }

  if(fragment_current_time != fragment_last_update){
    shader->setFragmentShader(read_file(fragment_shader_path));
    fragment_last_update = fragment_current_time;
    update_shader = 1;
  }

  if(update_shader)
    shader->compile();
};




void updateTexture(CW::Renderer::Texture *texture){
  if(texture_current_time != texture_last_update){
    texture->load(texture_path);
    texture_last_update = texture_current_time;
  };
};




void file_observer(CW::Renderer::DrawShader *shader, CW::Renderer::Uniform *uniform, CW::Renderer::Texture *texture){
  updateCurrentTimeWrite();

  updateShaders(shader);

  if(uniform_last_update != uniform_current_time)
    updateUniforms(uniform);

  updateTexture(texture);

};




////////////////////////////////////////////////////////
/////////////////////// Initials ///////////////////////
////////////////////////////////////////////////////////
CW::Renderer::Mesh canvas(){
  CW::Renderer::Mesh canvas = CW::Renderer::Mesh(
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

  canvas.addTextCords(
  {
    0.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    1.0f, 0.0f
  });

  return canvas;
};








////////////////////////////////////////////////////
/////////////////////// Main ///////////////////////
////////////////////////////////////////////////////
int main(){
  CW::Renderer::Renderer window;
  window.setWindowTitle("Shader Editor");
  
  CW::Renderer::Uniform uniform;
  CW::Renderer::Texture texture;
  
  CW::Renderer::DrawShader shader("", "");
  shader.getUniforms().emplace_back(&uniform);

  CW::Renderer::Mesh viewport = canvas();


  while(!window.getWindowData()->should_close){
    file_observer(&shader, &uniform, &texture);

    window.beginFrame();

    texture.bind();
    shader.bind();
    viewport.render();
    shader.unbind();
    texture.unbind();

    window.swapBuffer();
    window.windowEvents();
  };

  return 0;
};