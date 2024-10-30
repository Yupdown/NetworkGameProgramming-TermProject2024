#include "pch.h"
#include "Shader.h"
#include "Core.h"

static bool bInit = true;

Shader::Shader()
    :Resource{RESOURCE_TYPE::SHADER}
{
}

Shader::~Shader()
{
}

void Shader::SetUniformMat4(const glm::mat4& _mat4,string_view _uName) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_shaderID, _uName.data()), 1, GL_FALSE, glm::value_ptr(_mat4));
}

std::pair<string, string> Shader::loadShader(string_view _fileName)
{
    ifstream in{ _fileName.data() };
    stringstream vert{}, frag{};
    string line{};

    bool vertTurn = false;

    while (std::getline(in, line))
    {
        if (line.find("#version") != std::string::npos)
        {
            vertTurn = !vertTurn;
        }

        if (vertTurn)
        {
            vert << line << '\n';
        }
        else
        {
            frag << line << '\n';
        }
       
    }

    return std::make_pair(vert.str(), frag.str());
}

void Shader::Load(string_view _filePath, string_view _resName)
{
    GLint success;

    const auto [vertCodeStr, fragCodeStr] = loadShader(GetResPath());

    const char* vertCode = vertCodeStr.c_str();
    const char* fragCode = fragCodeStr.c_str();

    GLuint vert, frag;

    vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vertCode, NULL);
    glCompileShader(vert);

    glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
    if (!success) 
    {
        GLchar infoLog[512];
        glGetShaderInfoLog(vert, 512, NULL, infoLog);
        std::cout << "Vert Shader compilation failed:\n" << infoLog << std::endl;
    }

    frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fragCode, NULL);
    glCompileShader(frag);

    glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
    if (!success) 
    {
        GLchar infoLog[512];
        glGetShaderInfoLog(frag, 512, NULL, infoLog);
        std::cout << "Frag Shader compilation failed:\n" << infoLog << std::endl;
    }

    m_shaderID = glCreateProgram();
    glAttachShader(m_shaderID, vert);
    glAttachShader(m_shaderID, frag);
    glLinkProgram(m_shaderID);

    glDeleteShader(vert);
    glDeleteShader(frag);

    if (bInit)
    {
        Use();
        bInit = false;
    }
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, Mgr(Core)->GetUBO_ID());
}

void Shader::Use()const
{
    glUseProgram(m_shaderID);
    static const GLuint uboIdx = Mgr(Core)->GetUBO_ID();
    glUniformBlockBinding(m_shaderID, glGetUniformBlockIndex(m_shaderID, "UBOData"), 0);
}
