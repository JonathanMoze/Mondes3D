#include "viewer.h"

#include "parser.h"
#include "timer.h"

#include <filesystem/resolver.h>

#include <nanogui/button.h>
#include <nanogui/checkbox.h>
#include <nanogui/layout.h>
#include <nanogui/opengl.h>
#include <nanogui/renderpass.h>
#include <nanogui/shader.h>
#include <nanogui/slider.h>
#include <nanogui/texture.h>
#include <nanogui/window.h>

#include <thread>

Viewer::Viewer()
    : nanogui::Screen(nanogui::Vector2i(512, 512 + 50), "Raytracer", false),
      m_resultImage(nullptr), m_renderingDone(true),
      m_texture(nullptr) {

  /* Add some UI elements to adjust the exposure value and gamma */
  using namespace nanogui;
  inc_ref();
  m_panel = new Widget(this);
  m_panel->set_layout(
      new BoxLayout(Orientation::Horizontal, Alignment::Middle, 10, 10));

  m_checkbox = new CheckBox(m_panel, "srgb");
  m_checkbox->set_checked(true);
  m_checkbox->set_callback([&](bool value) { m_srgb = value ? 1 : 0; });

  m_slider = new Slider(m_panel);
  m_slider->set_value(0.5f);
  m_slider->set_fixed_width(150);
  m_slider->set_callback(
      [&](float value) { m_scale = std::pow(2.f, (value - 0.5f) * 20); });

  m_button1 = new Button(m_panel);
  m_button1->set_caption("PNG");
  m_button1->set_enabled(false);
  m_button1->set_callback([&](void) {
    nanogui::Vector2i size = this->size();
    size.y() -= 50;
    ref<nanogui::Texture> blitTexture =
        new nanogui::Texture(nanogui::Texture::PixelFormat::RGB,
                             nanogui::Texture::ComponentFormat::Float32, size,
                             nanogui::Texture::InterpolationMode::Nearest,
                             nanogui::Texture::InterpolationMode::Nearest,
                             nanogui::Texture::WrapMode::ClampToEdge, 1,
                             nanogui::Texture::TextureFlags::ShaderRead |
                                 nanogui::Texture::TextureFlags::RenderTarget);
    m_tonemapProgram->set_uniform("scale", m_scale);
    m_tonemapProgram->set_uniform("srgb", m_srgb);
    m_renderPass->resize(size);
    m_renderPass->begin();
    m_renderPass->set_viewport(
        nanogui::Vector2i(0, 0),
        nanogui::Vector2i(size[0], size[1]));
    m_texture->upload((uint8_t *)m_resultImage->data());
    m_tonemapProgram->set_texture("source", m_texture);
    m_tonemapProgram->begin();
    m_tonemapProgram->draw_array(nanogui::Shader::PrimitiveType::Triangle, 0, 6,
                                 true);
    m_tonemapProgram->end();
    m_renderPass->set_viewport(nanogui::Vector2i(0, 0), framebuffer_size());
    m_renderPass->end();

    ref<RenderPass> blitPass = new RenderPass({blitTexture});
    m_renderPass->blit_to(nanogui::Vector2i(0, 0), size, blitPass,
                          nanogui::Vector2i(0, 0));

    Bitmap img(::Vector2i(size.x(), size.y()));
    blitTexture->download((uint8_t *)img.data());
    std::string outputName = m_curentFilename;
    size_t lastdot = outputName.find_last_of(".");
    if (lastdot != std::string::npos)
      outputName.erase(lastdot, std::string::npos);
    outputName += ".png";
    img.savePNG(outputName);
  });

  m_button2 = new Button(m_panel);
  m_button2->set_caption("EXR");
  m_button2->set_enabled(false);
  m_button2->set_callback([&](void) {
    std::string outputName = m_curentFilename;
    size_t lastdot = outputName.find_last_of(".");
    if (lastdot != std::string::npos)
      outputName.erase(lastdot, std::string::npos);
    outputName += ".exr";

    Bitmap *img = m_resultImage->toBitmap();
    img->saveEXR(outputName);
    delete img;
  });

  m_panel->set_size(nanogui::Vector2i(512, 50));
  perform_layout();
  m_panel->set_position(
      nanogui::Vector2i((512 - m_panel->size().x()) / 2, 512));

  initializeGL();

  draw_all();
  set_visible(true);
}

void Viewer::initializeGL() {
  using namespace nanogui;
  std::cout << "Using OpenGL version: \"" << glGetString(GL_VERSION) << "\""
            << std::endl;

  m_renderPass = new RenderPass({this});
  m_renderPass->set_clear_color(0, Color(0.3f, 0.3f, 0.3f, 1.f));

  // load the default shaders
  m_tonemapProgram = new Shader(m_renderPass, "Tonemapper",
                                /* Vertex shader */
                                R"(#version 330
        uniform ivec2 size;
        uniform int borderSize;

        in vec2 position;
        out vec2 uv;
        void main() {
            gl_Position = vec4(position.x * 2 - 1, position.y * 2 - 1, 0.0, 1.0);

            // Crop away image border (due to pixel filter)
            vec2 total_size = size + 2 * borderSize;
            vec2 scale = size / total_size;
            uv = vec2(position.x * scale.x + borderSize / total_size.x,
                      1 - (position.y * scale.y + borderSize / total_size.y));
        })",
                                /* Fragment shader */
                                R"(#version 330
        uniform sampler2D source;
        uniform float scale;
        uniform int srgb;
        in vec2 uv;
        out vec4 out_color;
        float toSRGB(float value) {
            if (value < 0.0031308)
                return 12.92 * value;
            return 1.055 * pow(value, 0.41666) - 0.055;
        }
        void main() {
            vec4 color = texture(source, uv);
            color *= scale / color.w;
            if(srgb == 1)
                out_color = vec4(toSRGB(color.r), toSRGB(color.g), toSRGB(color.b), 1);
            else
                out_color = vec4(color.rgb,1);
        })");

  // Draw 2 triangles
  uint32_t indices[3 * 2] = {0, 1, 2, 2, 3, 0};
  float positions[2 * 4] = {0.f, 0.f, 1.f, 0.f, 1.f, 1.f, 0.f, 1.f};

  m_tonemapProgram->set_buffer("indices", VariableType::UInt32, {3 * 2},
                               indices);
  m_tonemapProgram->set_buffer("position", VariableType::Float32, {4, 2},
                               positions);
}

void Viewer::draw_contents() {
  if (m_resultImage) // raytracing in progress
  {
    /* Reload the partially rendered image onto the GPU */
    m_resultImage->lock();
    const Vector2i &size = m_resultImage->getSize();
    m_tonemapProgram->set_uniform("scale", m_scale);
    m_tonemapProgram->set_uniform("srgb", m_srgb);
    m_renderPass->resize(framebuffer_size());
    m_renderPass->begin();
    m_renderPass->set_viewport(
        nanogui::Vector2i(0, 0),
        nanogui::Vector2i(m_pixel_ratio * size[0], m_pixel_ratio * size[1]));
    m_texture->upload((uint8_t *)m_resultImage->data());
    m_tonemapProgram->set_texture("source", m_texture);
    m_tonemapProgram->begin();
    m_tonemapProgram->draw_array(nanogui::Shader::PrimitiveType::Triangle, 0, 6,
                                 true);
    m_tonemapProgram->end();
    m_renderPass->set_viewport(nanogui::Vector2i(0, 0), framebuffer_size());
    m_renderPass->end();
    m_resultImage->unlock();
  } else {
    m_renderPass->resize(framebuffer_size());
    m_renderPass->begin();
    m_renderPass->end();
  }
}

bool Viewer::resize_event(const nanogui::Vector2i &size) {
  m_panel->set_size(nanogui::Vector2i(size.x(), 50));
  perform_layout();
  m_panel->set_position(
      nanogui::Vector2i((size.x() - m_panel->size().x()) / 2, size.y() - 50));
  return true;
}

void Viewer::loadScene(const std::string &filename) {
  if (filename.size() > 0) {
    filesystem::path path(filename);

    if (path.extension() != "scn")
      return;
    m_renderingDone = true;
    if (m_resultImage) {
      delete m_resultImage;
      m_resultImage = nullptr;
    }

    getFileResolver()->prepend(path.parent_path());

    ::Object *root = loadFromXML(filename);
    if (root->getClassType() == ::Object::EScene) {
      if (m_scene)
        delete m_scene;
      m_scene = static_cast<Scene *>(root);
      m_curentFilename = filename;

      // Update GUI
      Vector2i outputSize = m_scene->camera()->outputSize();
      set_size(nanogui::Vector2i(outputSize.x(), outputSize.y() + 50));
      glfwSetWindowSize(glfw_window(), outputSize.x(), outputSize.y() + 50);
      m_panel->set_size(nanogui::Vector2i(outputSize.x(), 50));
      perform_layout();
      m_panel->set_position(nanogui::Vector2i(
          (outputSize.x() - m_panel->size().x()) / 2, outputSize.y()));
    }
    draw_all();
  }
}

void Viewer::loadImage(const filesystem::path &filename) {
  m_curentFilename = filename.str();
  Bitmap bitmap(filename);
  m_resultImage =
      new ImageBlock(Eigen::Vector2i(bitmap.cols(), bitmap.rows()));
  m_resultImage->fromBitmap(bitmap);
  m_renderingDone = false;
  // Update GUI
  const ::Vector2i &size = m_resultImage->getSize();
  m_tonemapProgram->set_uniform("size", nanogui::Vector2i(size.x(), size.y()));
  m_tonemapProgram->set_uniform("borderSize", m_resultImage->getBorderSize());

  // Allocate texture memory for the rendered image
  using nanogui::Texture;
  m_texture = new Texture(
      Texture::PixelFormat::RGBA, Texture::ComponentFormat::Float32,
      nanogui::Vector2i(size.x() + 2 * m_resultImage->getBorderSize(),
                        size.y() + 2 * m_resultImage->getBorderSize()),
      Texture::InterpolationMode::Nearest, Texture::InterpolationMode::Nearest);

  m_button1->set_enabled(true);
  m_button2->set_enabled(true);
  set_size(nanogui::Vector2i(m_resultImage->getSize().x(),
                             m_resultImage->getSize().y() + 50));
  glfwSetWindowSize(glfw_window(), m_resultImage->getSize().x(),
                    m_resultImage->getSize().y() + 50);
  m_panel->set_size(nanogui::Vector2i(m_resultImage->getSize().x(),
                                      m_resultImage->getSize().y()));
  perform_layout();
  m_panel->set_position(nanogui::Vector2i(
      (m_resultImage->getSize().x() / m_pixel_ratio - m_panel->size().x()) / 2,
      m_resultImage->getSize().y()));
}

void Viewer::render(Scene *scene, ImageBlock *result, bool *done) {
  if (!scene)
    return;
  result->clear();

  cout << "Rendering .. " << endl;
  cout.flush();
  Timer timer;

  Integrator *integrator = scene->integrator();
  integrator->preprocess(scene);

  const Camera *camera = scene->camera();

  /// TODO:
  ///  1. iterate over the image pixels (Camera::outputSize)
  ///  2. generate a primary ray (Camera::sampleRay)
  ///  3. call the integrator to compute the color along this ray
  ///  4. write this color in the result image (ImageBlock::put)
  for(int i=0; i<camera->outputSize().x(); i++){
    for(int j=0; j<camera->outputSize().y(); j++){
      Ray ray = Ray();
      Point2f ptn = Point2f(i, j);
      camera->sampleRay(ray, ptn);
      Color3f col = integrator->Li(scene, ray);
      result->put(ptn, col);
      
    }
  } 

  cout << "done. (took " << timer.elapsedString() << ")" << endl;
  *done = true;
}

bool Viewer::keyboard_event(int key, int scancode, int action, int modifiers) {
  if (Screen::keyboard_event(key, scancode, action, modifiers))
    return true;
  if (action == GLFW_PRESS) {
    switch (key) {
    case GLFW_KEY_L: {
      std::string filename = nanogui::file_dialog(
          {{"scn", "Scene file"}, {"exr", "Image file"}}, false);
      filesystem::path path(filename);
      if (path.extension() == "scn")
        loadScene(filename);
      else if (path.extension() == "exr" || path.extension() == "png")
        loadImage(filename);
      return true;
    }
    case GLFW_KEY_R: {
      if (m_scene && m_renderingDone) {
        m_renderingDone = false;
        /* Determine the filename of the output bitmap */
        std::string outputName = m_curentFilename;
        size_t lastdot = outputName.find_last_of(".");
        if (lastdot != std::string::npos)
          outputName.erase(lastdot, std::string::npos);
        outputName += ".exr";

        /* Allocate memory for the entire output image */
        if (m_resultImage)
          delete m_resultImage;
        m_resultImage =
            new ImageBlock(m_scene->camera()->outputSize());
        const ::Vector2i &size = m_resultImage->getSize();
        m_tonemapProgram->set_uniform("size",
                                      nanogui::Vector2i(size.x(), size.y()));
        m_tonemapProgram->set_uniform("borderSize",
                                      m_resultImage->getBorderSize());

        // Allocate texture memory for the rendered image
        using nanogui::Texture;
        m_texture = new Texture(
            Texture::PixelFormat::RGBA, Texture::ComponentFormat::Float32,
            nanogui::Vector2i(size.x() + 2 * m_resultImage->getBorderSize(),
                              size.y() + 2 * m_resultImage->getBorderSize()),
            Texture::InterpolationMode::Nearest,
            Texture::InterpolationMode::Nearest);

        std::thread render_thread(render, m_scene, m_resultImage,
                                  &m_renderingDone);
        render_thread.detach();

        // Update GUI
        m_button1->set_enabled(true);
        m_button2->set_enabled(true);
      }
      return true;
    }
    case GLFW_KEY_ESCAPE:
      exit(0);
    default:
      break;
    }
  }
  return false;
}

bool Viewer::drop_event(const std::vector<std::string> &filenames) {
  // only tries to load the first file
  filesystem::path path(filenames.front());
  if (path.extension() == "scn")
    loadScene(filenames.front());
  else if (path.extension() == "exr" || path.extension() == "png")
    loadImage(filenames.front());

  draw_all();
  return true;
}
