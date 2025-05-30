#include "Shader.h"

Shader::Shader()
{

}

Shader::Shader(const std::string& vertex_path, const std::string& fragment_path)
{
    m_id = glCreateProgram();

    std::string vertex_source   = LoadFromFile(vertex_path);
    std::string fragment_source = LoadFromFile(fragment_path);

    u32 vertex_shader = Compile(VERTEX, vertex_source);
    u32 fragment_shader = Compile(FRAGMENT, fragment_source);

    Attach(vertex_shader);
    Attach(fragment_shader);

    Link();

    Detach(vertex_shader);
    Detach(fragment_shader);

    Delete(vertex_shader);
    Delete(fragment_shader);
}

Shader::~Shader()
{
    glDeleteProgram(m_id);
}

void Shader::Use()
{
    glUseProgram(m_id);
}

void Shader::Unuse()
{
    glUseProgram(0);
}

u32 Shader::GetID()
{
    return m_id;
}

std::string Shader::LoadFromFile(const std::string& filepath)
{
    std::string data;
    std::ifstream file(filepath, std::ios::in | std::ios::binary);
    if (!file.is_open())
        std::cout << "Could not open " << filepath << "\n";

    std::copy(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>(), std::back_inserter(data));
    file.close();

    return data;
}

u32 Shader::Compile(ShaderType type, std::string& source)
{
    u32 shader = glCreateShader(type);

    const char* shader_src = source.c_str();
    glShaderSource(shader, 1, &shader_src, nullptr);

    glCompileShader(shader);

    s32 status = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE)
    {
        s32 max_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &max_length);

        std::vector<char> error_log(max_length);
        glGetShaderInfoLog(shader, max_length, &max_length, &error_log[0]);

        glDeleteShader(shader);

        std::printf("%s\n", &(error_log[0]));
    }

    return shader;
}

void Shader::Attach(u32& shader)
{
    glAttachShader(m_id, shader);
}

void Shader::Link()
{
    glLinkProgram(m_id);

    s32 status = 0;
    glGetProgramiv(m_id, GL_LINK_STATUS, &status);
    if (status == GL_FALSE)
    {
        s32 maxLength = 0;
        glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<char> vErrorLog(maxLength);
        glGetProgramInfoLog(m_id, maxLength, &maxLength, &vErrorLog[0]);

        glDeleteProgram(m_id);

        std::printf("%s\n", &(vErrorLog[0]));
    }
}

void Shader::Detach(u32& shader)
{
    glDetachShader(m_id, shader);
}

void Shader::Delete(u32& shader)
{
    glDeleteShader(shader);
}

u32 Shader::GetAttribute(const std::string& name) const
{
    return glGetAttribLocation(m_id, name.c_str());
}

u32 Shader::GetUniform(const std::string& name) const
{
    if (m_UniformLocations.find(name) != m_UniformLocations.end())
        return m_UniformLocations[name];

    u32 location = glGetUniformLocation(m_id, name.c_str());
    m_UniformLocations[name] = location;
    return location;
}

void Shader::SetUniform(const std::string& name, const s32& val)       { glUniform1i(GetUniform(name), val); }
void Shader::SetUniform(const std::string& name, f32* val, s32 count)  { glUniform1fv(GetUniform(name), count, val); }
void Shader::SetUniform(const std::string& name, s32* val, s32 count)  { glUniform1iv(GetUniform(name), count, val); }
void Shader::SetUniform(const std::string& name, const f64& val)       { glUniform1f(GetUniform(name), val); }
void Shader::SetUniform(const std::string& name, const f32& val)       { glUniform1f(GetUniform(name), val); }
void Shader::SetUniform(const std::string& name, const vf2& vector)    { glUniform2f(GetUniform(name), vector.x, vector.y); }
void Shader::SetUniform(const std::string& name, const vf3& vector)    { glUniform3f(GetUniform(name), vector.x, vector.y, vector.z); }
void Shader::SetUniform(const std::string& name, const vf4& vector)    { glUniform4f(GetUniform(name), vector.x, vector.y, vector.z, vector.w); }
void Shader::SetUniform(const std::string& name, const mf4x4& matrix)  { glUniformMatrix4fv(GetUniform(name), 1, GL_FALSE, glm::value_ptr(matrix)); }